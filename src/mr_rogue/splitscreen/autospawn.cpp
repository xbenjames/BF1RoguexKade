// BF1GameExt - autospawn.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// 18u: stands aside while player 2's own spawn screens look after his spawning
// (split_spawn_menus.cpp, P2SpawnFlowOwnsSpawn); otherwise as before.
// Phase 7b (2020 build): presses player 2's Spawn button through the game's own
// SetReady, from the game thread. [spawn] lines.
//
// Found by pattern (proven on GOG 1.3, the same code as Steam): the spawn
// screen's "Spawn pressed" sequence
//   mov ecx, [esi+2370h]   ; the screen's character
//   push eax
//   call SetSpawnChoice    ; (team, class, spawn point)
//   mov ecx, [esi+2370h]
//   push 1
//   call SetReady          ; [character + 0CCh] = 1
// SetReady body: push ebp; mov ebp, esp; mov al, [ebp+8];
//                mov [ecx+0CCh], al; pop ebp; ret 4

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "autospawn.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "split_spawn_menus.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

namespace bf1 {
namespace {

const char* const kSpawnPressed =
    "8B 8E 70 23 00 00 50 E8 ?? ?? ?? ?? 8B 8E 70 23 00 00 6A 01 E8 ?? ?? ?? ??";
constexpr uint32_t kOffSetReadyCall = 20;
const char* const kSetReadyBody = "55 8B EC 8A 45 08 88 81 CC 00 00 00 5D C2 04 00";

constexpr uint32_t kIndex = 0x9C;
constexpr uint32_t kTeam = 0xA0;
constexpr uint32_t kClass = 0xAC;
constexpr uint32_t kSpawnPoint = 0xB0;
constexpr uint32_t kUnit = 0xB4;
constexpr uint32_t kPost = 0xC8;
constexpr uint32_t kReady = 0xCC;

constexpr DWORD kTryEveryMs = 3000;
constexpr int kMaxLines = 120;

typedef void(__fastcall* SetReady_t)(void* self, void* edx, uint32_t ready);

SetReady_t g_setReady = nullptr;
bool g_enabled = false;
DWORD g_lastTry = 0;
DWORD g_readySince = 0;
bool g_inWorldLogged = false;
bool g_badIndexLogged = false;
bool g_standingAside = false; // 18u: player 2's spawn screens look after him
int g_attempts = 0;
int g_lines = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

int32_t ReadI32At(uint32_t rva)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

uint32_t CallTarget(uint32_t rva)
{
    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    return t;
}

bool ReadIniEnabled()
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return true;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return true;
    *slash = 0;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
    return GetPrivateProfileIntA("Splitscreen", "AutoSpawnP2", 1, ini) != 0;
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 0;
    return ReadU32(Rva(b->localPlayerCountRva));
}

void LogCharacter(const char* what, uintptr_t ch)
{
    if (!Budget())
        return;
    Log("[spawn] %lu ms: %s: team %ld, class 0x%08X, spawn point 0x%08X, post %ld, ready %u, unit "
        "0x%08X",
        ScanMsSinceAttach(), what, static_cast<long>(static_cast<int32_t>(ReadU32(ch + kTeam))),
        static_cast<unsigned>(ReadU32(ch + kClass)), static_cast<unsigned>(ReadU32(ch + kSpawnPoint)),
        static_cast<long>(static_cast<int32_t>(ReadU32(ch + kPost))),
        static_cast<unsigned>(*reinterpret_cast<const volatile uint8_t*>(ch + kReady)),
        static_cast<unsigned>(ReadU32(ch + kUnit)));
}

} // namespace

void AutoSpawnStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[spawn] not used on this build");
        return;
    }

    g_enabled = ReadIniEnabled();
    if (!g_enabled)
    {
        Log("[spawn] BF1GameExt.ini [Splitscreen] AutoSpawnP2=0: player 2 auto-spawn OFF");
        return;
    }

    uint32_t hits[4] = {};
    const int n = FindPattern(kSpawnPressed, hits, 4);
    if (n != 1)
    {
        Log("[spawn] spawn screen 'Spawn pressed' code: %d pattern hits (need 1) - auto-spawn "
            "disabled",
            n);
        g_enabled = false;
        return;
    }
    const uint32_t setReady = CallTarget(hits[0] + kOffSetReadyCall);
    if (!MatchPatternAt(setReady, kSetReadyBody))
    {
        Log("[spawn] SetReady at rva 0x%08X does NOT match - auto-spawn disabled", setReady);
        g_enabled = false;
        return;
    }
    g_setReady = reinterpret_cast<SetReady_t>(Rva(setReady));
    Log("[spawn] 'Spawn pressed' code at rva 0x%08X, SetReady at rva 0x%08X  OK", hits[0],
        setReady);
    Log("[spawn] auto-spawn ON: every %lu s while player 1 is in the world and player 2 is not, "
        "BF1GameExt presses player 2's Spawn (SetReady(1) on character 1)",
        static_cast<unsigned long>(kTryEveryMs / 1000));
}

void AutoSpawnFrame()
{
    if (!g_enabled || !g_setReady)
        return;
    const DWORD tick = GetTickCount();
    if (tick - g_lastTry < kTryEveryMs)
        return;
    g_lastTry = tick;

    if (LocalPlayers() < 2)
        return;
    // 18u: player 2's own spawn screens spawn him (A on his class screen)
    const bool aside = P2SpawnFlowOwnsSpawn();
    if (aside != g_standingAside)
    {
        g_standingAside = aside;
        if (Budget())
            Log("[spawn] %lu ms: %s", ScanMsSinceAttach(),
                aside ? "player 2's spawn screens are up to him - auto-spawn stands aside"
                      : "auto-spawn back on (player 2's spawn screens are not running)");
    }
    if (aside)
        return;
    const uintptr_t p1 = SpawnScanCharacter(0);
    const uintptr_t p2 = SpawnScanCharacter(1);
    if (!p1 || !p2)
        return;

    if (static_cast<int32_t>(ReadU32(p2 + kIndex)) != 1)
    {
        if (!g_badIndexLogged && Budget())
        {
            Log("[spawn] character 1 has index %ld, not 1 - not player 2, auto-spawn waits",
                static_cast<long>(static_cast<int32_t>(ReadU32(p2 + kIndex))));
            g_badIndexLogged = true;
        }
        return;
    }

    if (ReadU32(p2 + kUnit))
    {
        if (!g_inWorldLogged)
        {
            g_inWorldLogged = true;
            g_readySince = 0;
            LogCharacter("PLAYER 2 IS IN THE WORLD", p2);
        }
        return;
    }
    if (g_inWorldLogged && Budget())
        Log("[spawn] %lu ms: player 2 left the world (died?) - will spawn them again",
            ScanMsSinceAttach());
    g_inWorldLogged = false;

    if (!ReadU32(p1 + kUnit))
        return; // wait for player 1 to be in the world

    if (*reinterpret_cast<const volatile uint8_t*>(p2 + kReady))
    {
        if (!g_readySince)
            g_readySince = tick;
        else if (tick - g_readySince > 6000 && Budget())
        {
            LogCharacter("player 2 is READY but the game has not spawned them yet", p2);
            g_readySince = tick;
        }
        return;
    }

    ++g_attempts;
    if (g_attempts <= 3 || (g_attempts % 10) == 0)
    {
        LogCharacter("player 1", p1);
        LogCharacter("pressing Spawn for player 2", p2);
    }
    g_setReady(reinterpret_cast<void*>(p2), nullptr, 1);
    g_readySince = tick;
}

} // namespace bf1
