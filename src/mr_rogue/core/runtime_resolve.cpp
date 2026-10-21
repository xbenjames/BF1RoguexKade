// BF1GameExt - runtime_resolve.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Finds the split screen and controls sites in memory for builds whose code is
// locked on disk (Steam). Runs after Steam has unlocked the game; never
// changes any game memory.
//
// Every pattern below is GOG 1.3 code (same source as Steam) with "??" over
// call distances and absolute addresses. Each one is verified on the GOG 1.3
// exe: matches exactly once, and the addresses read out of it are correct.
//
// Split screen (all required):
//   ScriptCB_SetSplitscreen / IsSplitscreen / WasSplitscreen are anchored by
//   the Lua table, then followed through their calls:
//     SetSplitscreen -> setter  "mov [count], eax"   -> local player count
//     IsSplitscreen  -> getter  "mov eax, [count]"   -> must be the same count
//     WasSplitscreen -> getter  "mov al, [was]"      -> was-split flag
//   boot-split site: unique pattern whose call goes to the setter -> boot flag
//   back-to-menus site: unique pattern calling the getter and the setter and
//                       writing the was-split flag (check only)
// Controls (optional):
//   GetCurrentProfile -> profile index and profile array
//   local player slot loop -> slot table (must span exactly 2 x 0x30)
//   binding table loop and the SetJoystickEnabled +0x370 write (checks only)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "runtime_resolve.hpp"

namespace bf1 {
namespace {

const char* const kSetSplit =
    "55 8B EC 6A 01 FF 75 08 E8 ?? ?? ?? ?? 33 C9 84 C0 0F 95 C1 41 51 E8 ?? ?? ?? ?? 83 C4 0C "
    "33 C0 5D C3";
const char* const kIsSplit =
    "55 8B EC E8 ?? ?? ?? ?? 83 F8 01 0F 9F C0 0F B6 C0 50 FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 B8 01 "
    "00 00 00 5D C3";
const char* const kWasSplit =
    "55 8B EC E8 ?? ?? ?? ?? 0F B6 C0 50 FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 B8 01 00 00 00 5D C3";
const char* const kSetter = "55 8B EC 8B 45 08 A3 ?? ?? ?? ?? 5D C3";
const char* const kGetter = "A1 ?? ?? ?? ?? C3";
const char* const kWasGetter = "A0 ?? ?? ?? ?? C3";
const char* const kBoot = "80 3D ?? ?? ?? ?? 00 74 0A 6A 02 E8 ?? ?? ?? ??";
const char* const kMenus =
    "E8 ?? ?? ?? ?? 83 F8 01 0F 9F 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 E8 ?? ?? ?? ??";
const char* const kProfile = "A1 ?? ?? ?? ?? 83 F8 01 77 0C 69 C0 58 04 00 00 05 ?? ?? ?? ?? C3";
const char* const kSlots =
    "BE ?? ?? ?? ?? 0F 1F 40 00 39 7E 10 75 0A FF 36 8D 4F 28 E8 ?? ?? ?? ?? 83 C6 30 81 FE ?? ?? "
    "?? ?? 7C E6";
const char* const kTables =
    "8D 7B 0C 89 45 F8 33 F6 8B D8 56 57 8B CB E8 ?? ?? ?? ?? 46 81 C7 AC 00 00 00 83 FE 03 7C EB";
const char* const kJoyWrite = "38 9E 70 03 00 00 74 ?? 8B CE 88 9E 70 03 00 00";

constexpr int kMaxHits = 8;
constexpr int kMaxThunkHops = 8;
constexpr uint32_t kSlotTableBytes = 2 * 0x30;
constexpr uint32_t kJoySearchBytes = 0x80;

// Published copy of the build, with the addresses found here.
BuildInfo g_resolved;
ByteSite g_splitSites[8];
ByteSite g_controlSites[6];

uint32_t ReadU32At(uint32_t rva)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

bool InCode(uint32_t rva)
{
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    return CodeSection(&textRva, &textSize) && rva >= textRva && rva < textRva + textSize;
}

// Absolute address stored at rva, as an RVA (0 if it points outside the exe).
uint32_t AbsToRva(uint32_t rva)
{
    const uint32_t abs = ReadU32At(rva);
    const uintptr_t base = ExeBase();
    if (abs < base)
        return 0;
    return static_cast<uint32_t>(abs - base);
}

// Target of the E8/E9 at rva, following incremental-link E9 thunks.
uint32_t CallTarget(uint32_t rva)
{
    uint32_t at = rva;
    for (int hop = 0; hop <= kMaxThunkHops; ++hop)
    {
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(Rva(at + 1)), sizeof(rel));
        const uint32_t target = static_cast<uint32_t>(static_cast<int64_t>(at) + 5 + rel);
        if (!InCode(target))
            return 0;
        const unsigned char op = *reinterpret_cast<const unsigned char*>(Rva(target));
        if (op != 0xE9)
            return target;
        at = target;
    }
    return 0;
}

bool Anchor(const char* luaName, const char* pattern, uint32_t* rva)
{
    *rva = LuaFuncRva(luaName);
    if (!*rva)
    {
        Log("[resolve] %s: not in this build's Lua table", luaName);
        return false;
    }
    if (!MatchPatternAt(*rva, pattern))
    {
        Log("[resolve] %s at rva 0x%08X: code does NOT match the GOG 1.3 pattern", luaName, *rva);
        LogBytes("[resolve]   actual ", *rva, reinterpret_cast<const void*>(Rva(*rva)), 32);
        return false;
    }
    Log("[resolve] %s at rva 0x%08X: pattern OK", luaName, *rva);
    return true;
}

bool ResolveSplit(BuildInfo& out, int* siteCount)
{
    uint32_t setRva = 0;
    uint32_t isRva = 0;
    uint32_t wasRva = 0;
    if (!Anchor("ScriptCB_SetSplitscreen", kSetSplit, &setRva) ||
        !Anchor("ScriptCB_IsSplitscreen", kIsSplit, &isRva) ||
        !Anchor("ScriptCB_WasSplitscreen", kWasSplit, &wasRva))
        return false;

    const uint32_t setter = CallTarget(setRva + 22);
    if (!setter || !MatchPatternAt(setter, kSetter))
    {
        Log("[resolve] player count setter (from SetSplitscreen) NOT found or wrong code");
        return false;
    }
    const uint32_t countRva = AbsToRva(setter + 7);
    Log("[resolve] setter at rva 0x%08X writes the local player count at rva 0x%08X", setter,
        countRva);

    const uint32_t getter = CallTarget(isRva + 3);
    if (!getter || !MatchPatternAt(getter, kGetter) || AbsToRva(getter + 1) != countRva)
    {
        Log("[resolve] player count getter (from IsSplitscreen) NOT found, wrong code, or reads a "
            "different address");
        return false;
    }
    Log("[resolve] getter at rva 0x%08X reads the same count: OK", getter);

    const uint32_t wasGetter = CallTarget(wasRva + 3);
    if (!wasGetter || !MatchPatternAt(wasGetter, kWasGetter))
    {
        Log("[resolve] was-split getter (from WasSplitscreen) NOT found or wrong code");
        return false;
    }
    const uint32_t wasSplit = AbsToRva(wasGetter + 1);
    Log("[resolve] was-split getter at rva 0x%08X reads the flag at rva 0x%08X", wasGetter, wasSplit);

    uint32_t hits[kMaxHits] = {};
    const int bootTotal = FindPattern(kBoot, hits, kMaxHits);
    uint32_t boot = 0;
    int bootGood = 0;
    for (int i = 0; i < bootTotal && i < kMaxHits; ++i)
    {
        if (CallTarget(hits[i] + 11) == setter)
        {
            boot = hits[i];
            ++bootGood;
        }
    }
    if (bootGood != 1)
    {
        Log("[resolve] boot-split site: %d pattern hits, %d call the setter (need exactly 1)",
            bootTotal, bootGood);
        return false;
    }
    const uint32_t bootFlag = AbsToRva(boot + 2);
    Log("[resolve] boot-split site at rva 0x%08X, boot flag at rva 0x%08X", boot, bootFlag);

    const int menusTotal = FindPattern(kMenus, hits, kMaxHits);
    uint32_t menus = 0;
    int menusGood = 0;
    for (int i = 0; i < menusTotal && i < kMaxHits; ++i)
    {
        if (CallTarget(hits[i]) == getter && CallTarget(hits[i] + 22) == setter &&
            AbsToRva(hits[i] + 11) == wasSplit)
        {
            menus = hits[i];
            ++menusGood;
        }
    }
    if (menusGood != 1)
    {
        Log("[resolve] back-to-menus site: %d pattern hits, %d verified (need exactly 1)",
            menusTotal, menusGood);
        return false;
    }
    Log("[resolve] back-to-menus site at rva 0x%08X: saves was-split, sets 1 player: OK", menus);

    out.localPlayerCountRva = countRva;
    out.wasSplitRva = wasSplit;
    out.bootSplitFlagRva = bootFlag;

    int n = 0;
    g_splitSites[n++] = { "ScriptCB_SetSplitscreen", setRva, kSetSplit };
    g_splitSites[n++] = { "ScriptCB_IsSplitscreen", isRva, kIsSplit };
    g_splitSites[n++] = { "ScriptCB_WasSplitscreen", wasRva, kWasSplit };
    g_splitSites[n++] = { "SetNumLocalPlayers", setter, kSetter };
    g_splitSites[n++] = { "GetNumLocalPlayers", getter, kGetter };
    g_splitSites[n++] = { "WasSplitscreen getter", wasGetter, kWasGetter };
    g_splitSites[n++] = { "boot-split: force 2 players", boot, kBoot };
    g_splitSites[n++] = { "back to menus: save was-split, set 1 player", menus, kMenus };
    *siteCount = n;
    return true;
}

bool ResolveControls(BuildInfo& out, int* siteCount)
{
    uint32_t hits[kMaxHits] = {};

    const int profTotal = FindPattern(kProfile, hits, kMaxHits);
    if (profTotal != 1)
    {
        Log("[resolve] GetCurrentProfile: %d pattern hits (need exactly 1)", profTotal);
        return false;
    }
    const uint32_t prof = hits[0];
    const uint32_t profileIndex = AbsToRva(prof + 1);
    const uint32_t profileArray = AbsToRva(prof + 17);
    Log("[resolve] GetCurrentProfile at rva 0x%08X: profile index rva 0x%08X, profile array rva "
        "0x%08X",
        prof, profileIndex, profileArray);

    const int slotsTotal = FindPattern(kSlots, hits, kMaxHits);
    uint32_t slots = 0;
    uint32_t slotTable = 0;
    int slotsGood = 0;
    for (int i = 0; i < slotsTotal && i < kMaxHits; ++i)
    {
        const uint32_t first = AbsToRva(hits[i] + 1);
        const uint32_t end = AbsToRva(hits[i] + 29);
        if (first && end == first + kSlotTableBytes)
        {
            if (slotsGood == 0 || first == slotTable)
            {
                slots = hits[i];
                slotTable = first;
                ++slotsGood;
            }
        }
    }
    if (slotsGood < 1)
    {
        Log("[resolve] local player slot loop: %d pattern hits, none span 2 x 0x30", slotsTotal);
        return false;
    }
    Log("[resolve] local player slot loop at rva 0x%08X: slot table rva 0x%08X", slots, slotTable);

    const int tablesTotal = FindPattern(kTables, hits, kMaxHits);
    if (tablesTotal < 1)
    {
        Log("[resolve] binding table loop (3 x 0xAC from profile + 0x34) NOT found");
        return false;
    }
    const uint32_t tables = hits[0];
    Log("[resolve] binding table loop at rva 0x%08X: OK (%d hit%s)", tables, tablesTotal,
        tablesTotal == 1 ? "" : "s");

    const uint32_t joyFunc = LuaFuncRva("ScriptCB_SetJoystickEnabled");
    uint32_t joy = 0;
    for (uint32_t off = 0; joyFunc && off < kJoySearchBytes; ++off)
    {
        if (MatchPatternAt(joyFunc + off, kJoyWrite))
        {
            joy = joyFunc + off;
            break;
        }
    }
    if (!joy)
    {
        Log("[resolve] SetJoystickEnabled: the profile + 0x370 write NOT found in its first 0x%X "
            "bytes",
            static_cast<unsigned>(kJoySearchBytes));
        return false;
    }
    Log("[resolve] SetJoystickEnabled writes profile + 0x370 at rva 0x%08X: OK", joy);

    out.profileIndexRva = profileIndex;
    out.profileArrayRva = profileArray;
    out.localSlotsRva = slotTable;

    int n = 0;
    g_controlSites[n++] = { "GetCurrentProfile (index, array, 0x458 stride)", prof, kProfile };
    g_controlSites[n++] = { "local player slots loop (0x30 stride, +0x10 = profile)", slots,
                            kSlots };
    g_controlSites[n++] = { "binding tables (3 x 0xAC from profile + 0x34)", tables, kTables };
    g_controlSites[n++] = { "SetJoystickEnabled (profile + 0x370)", joy, kJoyWrite };
    *siteCount = n;
    return true;
}

} // namespace

bool ResolveRuntimeBuild()
{
    const BuildInfo* build = CurrentBuild();
    if (!build || !build->resolveAtRuntime)
        return true;

    g_resolved = *build;
    g_resolved.resolveAtRuntime = false;

    int splitCount = 0;
    const bool splitOk = ResolveSplit(g_resolved, &splitCount);
    if (splitOk)
    {
        g_resolved.splitSites = g_splitSites;
        g_resolved.splitSiteCount = static_cast<size_t>(splitCount);
    }
    else
    {
        g_resolved.localPlayerCountRva = 0;
        g_resolved.wasSplitRva = 0;
        g_resolved.bootSplitFlagRva = 0;
        g_resolved.splitSites = nullptr;
        g_resolved.splitSiteCount = 0;
    }

    int controlCount = 0;
    const bool controlsOk = ResolveControls(g_resolved, &controlCount);
    if (controlsOk)
    {
        g_resolved.controlSites = g_controlSites;
        g_resolved.controlSiteCount = static_cast<size_t>(controlCount);
    }
    else
    {
        g_resolved.profileIndexRva = 0;
        g_resolved.profileArrayRva = 0;
        g_resolved.localSlotsRva = 0;
        g_resolved.controlSites = nullptr;
        g_resolved.controlSiteCount = 0;
    }

    PublishResolvedBuild(&g_resolved);
    Log("[resolve] split screen: %s | controls: %s", splitOk ? "FOUND" : "NOT FOUND",
        controlsOk ? "FOUND" : "NOT FOUND");
    return splitOk;
}

} // namespace bf1
