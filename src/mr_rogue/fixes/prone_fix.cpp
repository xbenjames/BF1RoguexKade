// BF1GameExt - prone_fix.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 10b (2020 build): a double-tap needs the second press within a window.
//
// ButtonState::Update(bool down) (thiscall; ecx = { signed counter, event }),
// matched as its exact 0x7B bytes (no addresses or calls in it). Its "pressed
// after a release" path (counter < 0) at +0x18:
//   cmp byte [ecx+1], 1 ; jne +4 ; mov byte [ecx+1], 2 ; mov byte [ecx], 1 ;
//   pop ebp ; ret 4                                       (17 bytes, to +0x29)
// becomes  jmp <stub> + 12 x int3, and the stub (DLL memory) does:
//   if (event == TAP) event = (counter >= -N) ? DOUBLE-TAP : none;
//   counter = 1 ; pop ebp ; ret 4                  (the original's own ending)
// Nothing else jumps into those 17 bytes (checked on GOG 1.3).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "prone_fix.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

const char* const kButtonStateUpdate =
    "55 8B EC 80 7D 08 00 8A 11 74 3B 84 D2 75 07 C6 01 02 5D C2 04 00 79 11 80 79 01 01 75 04 C6 "
    "41 01 02 C6 01 01 5D C2 04 00 80 FA 46 0F 9C C0 FE C2 FE C8 24 03 88 41 01 88 11 80 FA 64 7E "
    "3A C6 01 64 5D C2 04 00 84 D2 75 07 C6 01 FE 5D C2 04 00 7E 10 80 FA 46 0F 9C C0 88 41 01 C6 01 "
    "FF 5D C2 04 00 FE CA 56 BE 9C FF FF FF 0F B6 C2 80 FA 9C 88 11 0F 4C C6 88 01 5E 5D C2";
constexpr uint32_t kOffPressAfterRelease = 0x18;
constexpr uint32_t kRegion = 17;
constexpr int kDefaultMs = 600;
constexpr double kFrameMs = 1000.0 / 60.0; // the counter ticks once per frame

int ReadIniMs()
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return kDefaultMs;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return kDefaultMs;
    *slash = 0;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
    return static_cast<int>(GetPrivateProfileIntA("Controls", "DoubleTapMs", kDefaultMs, ini));
}

} // namespace

void ProneFixStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[pronefix] not used on this build");
        return;
    }
    const int ms = ReadIniMs();
    if (ms <= 0)
    {
        Log("[pronefix] BF1GameExt.ini [Controls] DoubleTapMs=0: the game's original double-tap");
        return;
    }
    if (!onGameThread)
    {
        Log("[pronefix] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }

    int frames = static_cast<int>(ms / kFrameMs + 0.5);
    if (frames < 1)
        frames = 1;
    if (frames > 99)
        frames = 99; // the counter's floor is -100

    uint32_t hits[4] = {};
    const int n = FindPattern(kButtonStateUpdate, hits, 4);
    if (n != 1)
    {
        Log("[pronefix] ButtonState update: %d exact matches (need exactly 1) - NOT installed", n);
        return;
    }
    const uintptr_t site = Rva(hits[0] + kOffPressAfterRelease);

    // The stub (28 bytes), verified by disassembly before it was written here.
    const unsigned char stub[] = {
        0x80, 0x79, 0x01, 0x01,                                    // cmp byte [ecx+1], 1
        0x75, 0x0F,                                                // jne .done
        0x80, 0xFA, static_cast<unsigned char>((256 - frames) & 0xFF), // cmp dl, -frames
        0x7C, 0x06,                                                // jl .stale
        0xC6, 0x41, 0x01, 0x02,                                    // mov byte [ecx+1], 2
        0xEB, 0x04,                                                // jmp .done
        0xC6, 0x41, 0x01, 0x00,                                    // .stale: mov byte [ecx+1], 0
        0xC6, 0x01, 0x01,                                          // .done: mov byte [ecx], 1
        0x5D,                                                      // pop ebp
        0xC2, 0x04, 0x00,                                          // ret 4
    };
    void* mem = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!mem)
    {
        Log("[pronefix] VirtualAlloc failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    memcpy(mem, stub, sizeof(stub));
    FlushInstructionCache(GetCurrentProcess(), mem, sizeof(stub));

    unsigned char patch[kRegion];
    patch[0] = 0xE9;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(mem) - (site + 5));
    memcpy(patch + 1, &rel, sizeof(rel));
    memset(patch + 5, 0xCC, kRegion - 5);

    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(site), kRegion, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[pronefix] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(site), patch, kRegion);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(site), kRegion, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(site), kRegion);

    Log("[pronefix] ButtonState update at rva 0x%08X (exact match): a double-tap now needs the second "
        "press within %d ms (%d frames at 60 fps); a press after a stale tap is a fresh press",
        hits[0], ms, frames);
}

} // namespace bf1
