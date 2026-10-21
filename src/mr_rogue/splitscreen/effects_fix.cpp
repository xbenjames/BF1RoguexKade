// BF1GameExt - effects_fix.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 11b (2020 build): keep the player's viewport when two effect passes
// switch to the full-screen effects target.
//
// Found by pattern (proven on GOG 1.3, each exactly one hit):
//   site 1, in 0x006EC770 (the 11a capture's Steam rva 0x2F3029):
//     push 1 ; call <get target> ; mov edi, eax ; push esi ; push edi ;
//     call SetRenderTarget          (offset 11)  ; add esp, 0Ch ; ...
//   site 2, in 0x006E45A0 (Steam rva 0x2EADBC):
//     lea eax, [ebp-14h] ; push eax ; call GetViewport (offset 4) ;
//     push 0 ; push [ebx+20h] ; call SetRenderTarget (offset 14) ; ...
//   the same function's own restore at its end (used to find SetViewport):
//     push [ebp-2Ch] ; push [ebp-30h] ; call SetRenderTarget (offset 6) ;
//     lea eax, [ebp-14h] ; push eax ; call SetViewport (offset 15)
// All three must lead to the same SetRenderTarget wrapper. All are cdecl:
//   GetViewport(void* out16)         copies the renderer's 16-byte viewport
//   SetRenderTarget(void* rt, void* ds)
//   SetViewport(const void* rect16)  applies a viewport record to the target
// The two "call SetRenderTarget" at sites 1 and 2 are redirected to
// SetRenderTargetKeepViewport (same arguments, same result).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "effects_fix.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

const char* const kSite1 =
    "6A 01 E8 ?? ?? ?? ?? 8B F8 56 57 E8 ?? ?? ?? ?? 83 C4 0C 8D 85 E4 FE FF FF 50 8D 85 E8 FE FF "
    "FF 50 8D 85 EC FE FF FF 50 8D 85 F0 FE FF FF 50 E8";
constexpr uint32_t kSite1Call = 11;
const char* const kSite2 =
    "8D 45 EC 50 E8 ?? ?? ?? ?? 6A 00 FF 73 20 E8 ?? ?? ?? ?? FF 73 18 E8 ?? ?? ?? ?? 33 C0 39 45 0C "
    "0F 95 C0 50 6A 1B E8";
constexpr uint32_t kSite2GetViewport = 4;
constexpr uint32_t kSite2Call = 14;
const char* const kRestore =
    "FF 75 D4 FF 75 D0 E8 ?? ?? ?? ?? 8D 45 EC 50 E8 ?? ?? ?? ?? 8B 4D FC 83 C4 0C";
constexpr uint32_t kRestoreSetRT = 6;
constexpr uint32_t kRestoreSetViewport = 15;

const char* const kGetViewportBody = "55 8B EC 8B 45 08 0F 10 05 ?? ?? ?? ?? 0F 11 00 5D C3";
const char* const kSetViewportBody =
    "55 8B EC 83 EC 30 A1 ?? ?? ?? ?? 33 C5 89 45 FC A1 ?? ?? ?? ?? 8D 55 D8 56 8B 75 08 52 8B 08 6A "
    "00 50 FF 91 98 00 00 00";
const char* const kSetRenderTargetBody =
    "55 8B EC 83 EC 28 A1 ?? ?? ?? ?? 33 C5 89 45 FC 56 8B 75 0C 57 8B 7D 08 85 F6 74";

typedef void(__cdecl* GetViewport_t)(void* out16);
typedef int(__cdecl* SetRenderTarget_t)(void* rt, void* ds);
typedef void(__cdecl* SetViewport_t)(const void* rect16);

GetViewport_t g_getViewport = nullptr;
SetRenderTarget_t g_setRenderTarget = nullptr;
SetViewport_t g_setViewport = nullptr;

// Same arguments and result as the game's SetRenderTarget wrapper, but the
// player's viewport survives the switch.
int __cdecl SetRenderTargetKeepViewport(void* rt, void* ds)
{
    alignas(16) unsigned char vp[16];
    g_getViewport(vp);
    const int result = g_setRenderTarget(rt, ds);
    g_setViewport(vp);
    return result;
}

int32_t ReadI32(uintptr_t va)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uintptr_t CallTargetVa(uintptr_t callAt)
{
    uintptr_t t = static_cast<uintptr_t>(static_cast<intptr_t>(callAt) + 5 + ReadI32(callAt + 1));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(t) == 0xE9; ++hop)
        t = static_cast<uintptr_t>(static_cast<intptr_t>(t) + 5 + ReadI32(t + 1));
    return t;
}

uint32_t RvaOf(uintptr_t va)
{
    return static_cast<uint32_t>(va - ExeBase());
}

bool FindOne(const char* what, const char* pattern, uintptr_t* va)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pattern, hits, 4);
    if (n != 1)
    {
        Log("[effectsfix] %s: %d pattern hits (need exactly 1) - NOT installed", what, n);
        return false;
    }
    *va = Rva(hits[0]);
    return true;
}

bool RedirectCall(uintptr_t callAt, const char* what)
{
    const int32_t rel = static_cast<int32_t>(
        reinterpret_cast<uintptr_t>(&SetRenderTargetKeepViewport) - (callAt + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[effectsfix] %s: VirtualProtect failed (error %lu)", what, GetLastError());
        return false;
    }
    memcpy(reinterpret_cast<void*>(callAt + 1), &rel, sizeof(rel));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(callAt), 5);
    return true;
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
    return GetPrivateProfileIntA("Display", "SplitEffectsFix", 1, ini) != 0;
}

} // namespace

void EffectsFixStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[effectsfix] not used on this build");
        return;
    }
    if (!ReadIniEnabled())
    {
        Log("[effectsfix] BF1GameExt.ini [Display] SplitEffectsFix=0: not installed");
        return;
    }
    if (!onGameThread)
    {
        Log("[effectsfix] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }

    uintptr_t site1 = 0;
    uintptr_t site2 = 0;
    uintptr_t restore = 0;
    if (!FindOne("glow pass switch (site 1)", kSite1, &site1) ||
        !FindOne("effects pass switch (site 2)", kSite2, &site2) ||
        !FindOne("effects pass restore", kRestore, &restore))
        return;

    const uintptr_t srt1 = CallTargetVa(site1 + kSite1Call);
    const uintptr_t srt2 = CallTargetVa(site2 + kSite2Call);
    const uintptr_t srt3 = CallTargetVa(restore + kRestoreSetRT);
    const uintptr_t getVp = CallTargetVa(site2 + kSite2GetViewport);
    const uintptr_t setVp = CallTargetVa(restore + kRestoreSetViewport);
    if (srt1 != srt2 || srt2 != srt3)
    {
        Log("[effectsfix] the three sites do not call the same SetRenderTarget (rva 0x%08X / "
            "0x%08X / 0x%08X) - NOT installed",
            RvaOf(srt1), RvaOf(srt2), RvaOf(srt3));
        return;
    }
    if (!MatchPatternAt(RvaOf(srt1), kSetRenderTargetBody) ||
        !MatchPatternAt(RvaOf(getVp), kGetViewportBody) ||
        !MatchPatternAt(RvaOf(setVp), kSetViewportBody))
    {
        Log("[effectsfix] SetRenderTarget rva 0x%08X / GetViewport rva 0x%08X / SetViewport rva "
            "0x%08X: a body does not match - NOT installed",
            RvaOf(srt1), RvaOf(getVp), RvaOf(setVp));
        return;
    }
    g_setRenderTarget = reinterpret_cast<SetRenderTarget_t>(srt1);
    g_getViewport = reinterpret_cast<GetViewport_t>(getVp);
    g_setViewport = reinterpret_cast<SetViewport_t>(setVp);
    Log("[effectsfix] SetRenderTarget rva 0x%08X, GetViewport rva 0x%08X, SetViewport rva 0x%08X  OK",
        RvaOf(srt1), RvaOf(getVp), RvaOf(setVp));

    const bool ok1 = RedirectCall(site1 + kSite1Call, "site 1");
    const bool ok2 = RedirectCall(site2 + kSite2Call, "site 2");
    Log("[effectsfix] glow pass switch at rva 0x%08X: %s; effects pass switch at rva 0x%08X: %s - "
        "the player's viewport now survives both switches",
        RvaOf(site1 + kSite1Call), ok1 ? "redirected" : "FAILED", RvaOf(site2 + kSite2Call),
        ok2 ? "redirected" : "FAILED");
}

} // namespace bf1
