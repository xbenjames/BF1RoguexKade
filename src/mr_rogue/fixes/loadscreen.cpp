// BF1GameExt - loadscreen.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 9b (2020 build): loading screens fill a widescreen display.
//
// LoadDisplay's draw helper (GOG 1.3 0x0046FFE0; found by pattern, proven on
// GOG 1.3) gets the viewport (x0, y0, x1, y1) and, when the widescreen flag
// argument is set, fits a centred 4:3 box:
//   block+0x00 call GetViewportRect
//   block+0x1B je <else>                 ; flag clear: use the viewport as is
//   block+0x1D boxW = (2*y0 + y1) * 4/3  ; constants 2, 2, 4, 3, 0.5
//              x0'  = ((2*x0 + x1) - boxW) * 0.5 ; x1' = x0' + boxW
//   block+0x66 jmp <continue>            ; block+0x6D
//   block+0x68 <else>                    ; untouched
// The 75 bytes at block+0x1D..+0x68 are replaced by a FILL branch:
//   x1 = [ebp-4] ; W = x1 - x0 ; H' = W * 0.75
//   y0' = ((y0 + y1) - H') * 0.5 ; y1' = y0' + H' ; jmp <continue>
// leaving the registers the code after it reads: xmm2 = x0, xmm3 = x1,
// xmm5 = y0', xmm4 = y1' (only xmm0 / xmm1 are scratch, as in the original).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

#include "game_build.hpp"
#include "loadscreen.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

const char* const kBranchBlock =
    "E8 ?? ?? ?? ?? F3 0F 10 65 F4 83 C4 10 80 7D 44 00 F3 0F 10 6D F0 F3 0F 10 55 F8 74 4B 0F 28 CD "
    "F3 0F 59 15 ?? ?? ?? ?? F3 0F 59 0D ?? ?? ?? ?? F3 0F 58 55 FC F3 0F 58 CC F3 0F 59 0D ?? ?? ?? "
    "?? F3 0F 5E 0D ?? ?? ?? ?? F3 0F 5C D1 F3 0F 59 15 ?? ?? ?? ?? 0F 28 DA F3 0F 11 55 F8 F3 0F 58 "
    "D9 F3 0F 11 5D FC EB 05 F3 0F 10 5D FC F3 0F 10 75 2C";
constexpr uint32_t kOffBranch = 0x1D;   // first byte of the 4:3 branch
constexpr uint32_t kOffElse = 0x68;     // the flag-clear path: must stay intact
constexpr uint32_t kOffContinue = 0x6D; // where both paths meet
constexpr uint32_t kRegion = kOffElse - kOffBranch; // 75 bytes

struct ConstCheck
{
    uint32_t operandOffset;
    float value;
};
const ConstCheck kConstants[] = {
    { 0x24, 2.0f }, { 0x2C, 2.0f }, { 0x3D, 4.0f }, { 0x45, 3.0f }, { 0x51, 0.5f },
};

// The fill branch's constants (mulss m32 needs no special alignment).
float g_threeQuarters = 0.75f;
float g_half = 0.5f;

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

float ReadF32(uintptr_t va)
{
    float v = 0.0f;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

bool InExe(uintptr_t va, size_t len)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va + len <= ExeBase() + b->sizeOfImage;
}

bool ReadIniFill()
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
    return GetPrivateProfileIntA("Display", "LoadScreenFill", 1, ini) != 0;
}

// The 75-byte fill branch for this run (the two constant addresses filled in).
void BuildFillBranch(uintptr_t branchVa, unsigned char (&out)[kRegion])
{
    const uint32_t q = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_threeQuarters));
    const uint32_t h = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_half));
    size_t n = 0;
    auto put = [&](std::initializer_list<unsigned char> bytes) {
        for (unsigned char b : bytes)
            out[n++] = b;
    };
    auto put32 = [&](uint32_t v) {
        memcpy(out + n, &v, sizeof(v));
        n += sizeof(v);
    };
    put({ 0xF3, 0x0F, 0x10, 0x5D, 0xFC }); // movss  xmm3, [ebp-4]   x1
    put({ 0x0F, 0x28, 0xCB });             // movaps xmm1, xmm3
    put({ 0xF3, 0x0F, 0x5C, 0xCA });       // subss  xmm1, xmm2      W = x1 - x0
    put({ 0xF3, 0x0F, 0x59, 0x0D });       // mulss  xmm1, [0.75]    H' = W * 3/4
    put32(q);
    put({ 0x0F, 0x28, 0xC5 });             // movaps xmm0, xmm5      y0
    put({ 0xF3, 0x0F, 0x58, 0xC4 });       // addss  xmm0, xmm4      y0 + y1
    put({ 0xF3, 0x0F, 0x5C, 0xC1 });       // subss  xmm0, xmm1      - H'
    put({ 0xF3, 0x0F, 0x59, 0x05 });       // mulss  xmm0, [0.5]     y0'
    put32(h);
    put({ 0x0F, 0x28, 0xE8 });             // movaps xmm5, xmm0
    put({ 0xF3, 0x0F, 0x58, 0xC1 });       // addss  xmm0, xmm1      y1' = y0' + H'
    put({ 0x0F, 0x28, 0xE0 });             // movaps xmm4, xmm0
    // jmp rel8 to the continuation
    const uintptr_t afterJmp = branchVa + n + 2;
    const uintptr_t target = branchVa - kOffBranch + kOffContinue;
    out[n++] = 0xEB;
    out[n++] = static_cast<unsigned char>(static_cast<int8_t>(target - afterJmp));
    while (n < kRegion)
        out[n++] = 0xCC; // unreachable
}

} // namespace

void LoadScreenFillStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[loadscreen] not used on this build");
        return;
    }
    if (!ReadIniFill())
    {
        Log("[loadscreen] BF1GameExt.ini [Display] LoadScreenFill=0: the 2020 4:3 box is kept");
        return;
    }
    if (!onGameThread)
    {
        Log("[loadscreen] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }

    uint32_t hits[4] = {};
    const int n = FindPattern(kBranchBlock, hits, 4);
    if (n != 1)
    {
        Log("[loadscreen] loading draw helper's 4:3 branch: %d pattern hits (need exactly 1) - NOT "
            "installed",
            n);
        return;
    }
    const uintptr_t block = Rva(hits[0]);
    for (const ConstCheck& c : kConstants)
    {
        const uintptr_t p = ReadU32(block + c.operandOffset);
        if (!InExe(p, 4) || ReadF32(p) != c.value)
        {
            Log("[loadscreen] constant at block+0x%02X is not %.1f - NOT installed",
                static_cast<unsigned>(c.operandOffset), static_cast<double>(c.value));
            return;
        }
    }

    const uintptr_t branch = block + kOffBranch;
    unsigned char fill[kRegion];
    BuildFillBranch(branch, fill);

    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(branch), kRegion, PAGE_EXECUTE_READWRITE,
                        &oldProt))
    {
        Log("[loadscreen] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(branch), fill, kRegion);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(branch), kRegion, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(branch), kRegion);

    Log("[loadscreen] loading draw helper's 4:3 branch at rva 0x%08X (constants 2, 2, 4, 3, 0.5 OK): "
        "replaced with FILL - full width, height = width x 3/4 centred, top and bottom cropped",
        hits[0] + kOffBranch);
}

} // namespace bf1
