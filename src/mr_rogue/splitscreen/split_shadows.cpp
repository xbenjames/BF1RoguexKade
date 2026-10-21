// BF1GameExt - split_shadows.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 12d (2020 build): High shadows act as Low while split screen is on.
// Phase 12e: the per-player motion blur draw is skipped while split screen is
// on (it copies each view into ONE texture, stretched to fill it, and samples
// it at the view's half - so each player gets the other player's picture,
// stretched 2x). Both share one "split screen on" flag and one updater.
//
// The stencil-shadow gate (GOG 1.3 0x006AA4E0, 37 bytes, unique):
//   cmp byte [switch], 0 ; je no ; call GetCaps ; cmp [eax+14h], 2 ; jne no ;
//   call GetState ; cmp [eax+4], 0 ; je no ; mov al, 1 ; ret ; no: xor al, al ; ret
// GetCaps / GetState are "mov eax, <struct> ; ret". The gate's first 5 bytes
// become a jump to a 46-byte replacement in DLL memory that asks one more
// thing first - "is split screen on?" - and otherwise does exactly the same
// (verified by disassembly and by simulating all 8 cases). Like the original
// it changes only eax and the flags.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"
#include "split_shadows.hpp"

namespace bf1 {
namespace {

const char* const kGate = "80 3D ?? ?? ?? ?? 00 74 19 E8 ?? ?? ?? ?? 83 78 14 02 75 0E E8 ?? ?? ?? ?? 83 78 04 "
                          "00 74 03 B0 01 C3 32 C0 C3";
constexpr uint32_t kGateSwitchOperand = 2;
constexpr uint32_t kGateCallCaps = 9;
constexpr uint32_t kGateCallState = 20;
const char* const kGetterBody = "B8 ?? ?? ?? ?? C3"; // mov eax, <struct> ; ret
constexpr uint32_t kActiveOffset = 0x100;           // the split-screen flag, in the stub page
constexpr uint32_t kBlurStubOffset = 0x40;          // the motion blur stub, in the stub page

// The per-player motion blur draw (GOG 1.3 0x00470320, a virtual method, ret 4):
//   push ebp ; mov ebp, esp ; sub esp, 24h ; push ebx ; call <view index> ;
//   mov ebx, eax ; xorps xmm0, xmm0 ; movss xmm1, [ebx*4 + <amounts>] ; ...
// Its first 6 bytes (no relative operands; nothing jumps into them) move into
// a stub that returns straight away (ret 4) while split screen is on.
const char* const kBlurDraw = "55 8B EC 83 EC 24 53 E8 ?? ?? ?? ?? 8B D8 0F 57 C0 F3 0F 10 0C 9D ?? ?? ?? ?? 0F 2E C8 "
                              "9F F6 C4 44 0F 8B ?? ?? ?? ?? E8";
constexpr uint32_t kBlurAmountsOperand = 22;
constexpr uint32_t kBlurPrologue = 6;

// Phase 12g-r2: the screen blur pass (BlurEffect::Render, GOG 1.3 0x0045D110)
// copies the view at quarter size, draws it over the WHOLE effects layer, then
// blends that layer back over the view sampling only the view's half - each
// player's blur is half their picture stretched 2x. r1 skipped the whole pass,
// but the effects layer it refreshes is also what holograms / refraction
// sample as "the screen picture", so a frozen layer left trails. r2 skips ONLY
// the final blend-back: the cdecl Draw2DQuad call at the end of the pass
//   push 0 x3 ; push 200h ; lea eax,[ebp-20h] ; push eax ; push <mat> ;
//   push [ebx+28h] ; push edi ; call Draw2DQuad ; add esp, 20h ; mov ecx, edi ;
//   call <cleanup> ; pop edi ; pop esi ; pop ebx ; mov esp, ebp ; pop ebp ; ret 4
// is redirected to a stub that returns at once in split screen (the caller
// cleans its own arguments; the result is unused) and otherwise jumps on.
const char* const kScreenBlurBlend = "6A 00 6A 00 6A 00 68 00 02 00 00 8D 45 E0 50 68 ?? ?? ?? ?? FF 73 28 57 E8 ?? "
                                     "?? ?? ?? 83 C4 20 8B CF E8 ?? ?? ?? ?? 5F 5E 5B 8B E5 5D C2 04 00";
constexpr uint32_t kScreenBlurBlendCall = 24;
constexpr uint32_t kScreenBlurStubOffset = 0x80;

// Phase 12h: the stencil shadow volumes. pcStencilShadowShader has two column
// variants of the volume pass; column 2's setup (GOG 1.3 0x006D6010) sets
// colour writes to 0 and then straight back to 7, with blending off - the
// 12g dumps show every volume draw in split screen with colour 0x7, blend
// off, ONE/ZERO: volumes painted onto the screen = the coloured wedges (the
// LESSEQUAL ones over sky / open air are never redrawn, so they stay). The
// final call of that setup
//   push 0 ; push 0A8h ; call SetRenderState ; push 7 ; push 0A8h ;
//   call SetRenderState ; add esp, 30h ; pop ebp ; ret 8
// goes through a stub that, in split screen, turns the value 7 into 0 (the
// arguments are the caller's, cdecl): colour stays off, the volumes only mark
// the stencil, and the High stencil shadows render normally.
const char* const kStencilSetupTail = "6A FF 6A 3B E8 ?? ?? ?? ?? 6A 00 6A 1B E8 ?? ?? ?? ?? 6A 00 68 A8 00 00 00 E8 ?? "
                                      "?? ?? ?? 6A 07 68 A8 00 00 00 E8 ?? ?? ?? ?? 83 C4";
constexpr uint32_t kStencilFinalCall = 37;
constexpr uint32_t kStencilStubOffset = 0xA0;

unsigned char* g_page = nullptr;

volatile unsigned char* g_active = nullptr;

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uintptr_t CallTargetVa(uintptr_t callAt)
{
    int32_t rel = 0;
    memcpy(&rel, reinterpret_cast<const void*>(callAt + 1), sizeof(rel));
    uintptr_t t = static_cast<uintptr_t>(static_cast<intptr_t>(callAt) + 5 + rel);
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(t) == 0xE9; ++hop)
    {
        memcpy(&rel, reinterpret_cast<const void*>(t + 1), sizeof(rel));
        t = static_cast<uintptr_t>(static_cast<intptr_t>(t) + 5 + rel);
    }
    return t;
}

bool InImage(uintptr_t va, const BuildInfo* b)
{
    return va >= ExeBase() && va < ExeBase() + b->sizeOfImage;
}

bool ReadIniEnabled(const char* key)
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
    return GetPrivateProfileIntA("Display", key, 1, ini) != 0;
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 0;
    return ReadU32(Rva(b->localPlayerCountRva));
}

DWORD WINAPI UpdateThread(LPVOID)
{
    bool was = false;
    for (;;)
    {
        const bool now = LocalPlayers() >= 2;
        *g_active = now ? 1 : 0;
        if (now != was)
        {
            Log("[splitshadows] %lu ms: %s", ScanMsSinceAttach(),
                now ? "split screen ON - split screen fixes active (shadows / motion blur as set in the INI)"
                    : "split screen OFF - shadows and motion blur follow the game again");
            was = now;
        }
        Sleep(100);
    }
}

} // namespace

namespace {

// One executable page for both stubs and the shared flag; the updater starts
// with the first stub that needs it.
bool EnsurePage()
{
    if (g_page)
        return true;
    g_page = static_cast<unsigned char*>(
        VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_page)
    {
        Log("[splitshadows] VirtualAlloc failed (error %lu) - NOT installed", GetLastError());
        return false;
    }
    g_active = g_page + kActiveOffset;
    *g_active = 0;
    const HANDLE t = CreateThread(nullptr, 0, UpdateThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
    return true;
}

bool WriteJump(uintptr_t at, const void* to, size_t len)
{
    unsigned char code[8];
    code[0] = 0xE9;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(to) - (at + 5));
    memcpy(code + 1, &rel, 4);
    for (size_t i = 5; i < len; ++i)
        code[i] = 0x90;
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(at), len, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(reinterpret_cast<void*>(at), code, len);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(at), len, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), len);
    return true;
}

void InstallShadowGate(const BuildInfo* build)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kGate, hits, 4);
    if (n != 1)
    {
        Log("[splitshadows] stencil-shadow gate: %d pattern hits (need exactly 1) - NOT installed", n);
        return;
    }
    const uintptr_t gate = Rva(hits[0]);
    const uintptr_t shadowSwitch = ReadU32(gate + kGateSwitchOperand);
    const uintptr_t capsFn = CallTargetVa(gate + kGateCallCaps);
    const uintptr_t stateFn = CallTargetVa(gate + kGateCallState);
    if (!InImage(shadowSwitch, build) || !InImage(capsFn, build) || !InImage(stateFn, build) ||
        !MatchPatternAt(static_cast<uint32_t>(capsFn - ExeBase()), kGetterBody) ||
        !MatchPatternAt(static_cast<uint32_t>(stateFn - ExeBase()), kGetterBody))
    {
        Log("[splitshadows] the gate's switch / getters do not check out - NOT installed");
        return;
    }
    const uint32_t caps = ReadU32(capsFn + 1);
    const uint32_t state = ReadU32(stateFn + 1);
    if (!InImage(caps, build) || !InImage(state, build))
    {
        Log("[splitshadows] the gate's state structs are outside the exe - NOT installed");
        return;
    }
    if (!EnsurePage())
        return;
    unsigned char* mem = g_page;
    const uint32_t active = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_active));
    const uint32_t sw = static_cast<uint32_t>(shadowSwitch);
    unsigned char stub[46];
    size_t p = 0;
    auto put = [&](const unsigned char* b, size_t len) {
        memcpy(stub + p, b, len);
        p += len;
    };
    auto put32 = [&](uint32_t v) {
        memcpy(stub + p, &v, 4);
        p += 4;
    };
    const unsigned char cmpByte[] = { 0x80, 0x3D };
    put(cmpByte, 2);  put32(active);  { const unsigned char t[] = { 0x00, 0x75, 0x22 }; put(t, 3); }
    put(cmpByte, 2);  put32(sw);      { const unsigned char t[] = { 0x00, 0x74, 0x19, 0xB8 }; put(t, 4); }
    put32(caps);                      { const unsigned char t[] = { 0x83, 0x78, 0x14, 0x02, 0x75, 0x0E, 0xB8 }; put(t, 7); }
    put32(state);
    { const unsigned char t[] = { 0x83, 0x78, 0x04, 0x00, 0x74, 0x03, 0xB0, 0x01, 0xC3, 0x32, 0xC0, 0xC3 }; put(t, 12); }
    if (p != sizeof(stub))
    {
        Log("[splitshadows] internal: stub is %u bytes, expected %u - NOT installed", static_cast<unsigned>(p),
            static_cast<unsigned>(sizeof(stub)));
        return;
    }
    memcpy(mem, stub, sizeof(stub));
    FlushInstructionCache(GetCurrentProcess(), mem, sizeof(stub));
    if (!WriteJump(gate, mem, 5))
    {
        Log("[splitshadows] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    Log("[splitshadows] stencil-shadow gate at rva 0x%08X (switch rva 0x%08X, caps rva 0x%08X, state rva "
        "0x%08X): split screen now gets the Low shadows; single player keeps the menu setting",
        hits[0], static_cast<unsigned>(shadowSwitch - ExeBase()), static_cast<unsigned>(caps - ExeBase()),
        static_cast<unsigned>(state - ExeBase()));
}

bool InstallSkipStub(uintptr_t fn, unsigned char* stub);

void InstallMotionBlurSkip(const BuildInfo* build)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kBlurDraw, hits, 4);
    if (n != 1)
    {
        Log("[splitblur] motion blur draw: %d pattern hits (need exactly 1) - NOT installed", n);
        return;
    }
    const uintptr_t draw = Rva(hits[0]);
    const uint32_t amounts = ReadU32(draw + kBlurAmountsOperand);
    if (!InImage(amounts, build))
    {
        Log("[splitblur] the per-player blur amounts are outside the exe - NOT installed");
        return;
    }
    if (!EnsurePage())
        return;
    if (!InstallSkipStub(draw, g_page + kBlurStubOffset))
    {
        Log("[splitblur] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    Log("[splitblur] motion blur draw at rva 0x%08X (per-player amounts at rva 0x%08X): skipped while split "
        "screen is on; single player keeps motion blur",
        hits[0], static_cast<unsigned>(amounts - ExeBase()));
}

// A 23-byte stub at 'stub': while split screen is on, return at once (ret 4);
// otherwise run the function's own 6-byte prologue and jump back to fn + 6.
// Then fn's first 6 bytes become a jump to the stub (+ 1 nop). For functions
// whose pattern pins the prologue as push ebp / mov ebp,esp / sub esp,imm8.
bool InstallSkipStub(uintptr_t fn, unsigned char* stub)
{
    const uint32_t active = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_active));
    unsigned char code[23];
    code[0] = 0x80;                                   // cmp byte [active], 0
    code[1] = 0x3D;
    memcpy(code + 2, &active, 4);
    code[6] = 0x00;
    code[7] = 0x74;                                   // je +3 (not split: run the function)
    code[8] = 0x03;
    code[9] = 0xC2;                                   // ret 4 (split screen: skip it)
    code[10] = 0x04;
    code[11] = 0x00;
    memcpy(code + 12, reinterpret_cast<const void*>(fn), kBlurPrologue); // push ebp ; mov ebp,esp ; sub esp,imm8
    code[18] = 0xE9;                                  // jmp fn + 6
    const int32_t back = static_cast<int32_t>((fn + kBlurPrologue) -
                                              (reinterpret_cast<uintptr_t>(stub) + sizeof(code)));
    memcpy(code + 19, &back, 4);
    memcpy(stub, code, sizeof(code));
    FlushInstructionCache(GetCurrentProcess(), stub, sizeof(code));
    return WriteJump(fn, stub, kBlurPrologue);
}

void InstallScreenBlurSkip()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kScreenBlurBlend, hits, 4);
    if (n != 1)
    {
        Log("[splitblur] screen blur blend-back: %d pattern hits (need exactly 1) - NOT installed", n);
        return;
    }
    if (!EnsurePage())
        return;
    const uintptr_t callAt = Rva(hits[0] + kScreenBlurBlendCall);
    const uintptr_t draw2d = CallTargetVa(callAt);
    unsigned char* stub = g_page + kScreenBlurStubOffset;
    const uint32_t active = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_active));
    unsigned char code[15];
    code[0] = 0x80;                                   // cmp byte [active], 0
    code[1] = 0x3D;
    memcpy(code + 2, &active, 4);
    code[6] = 0x00;
    code[7] = 0x75;                                   // jne +5 (split screen: skip the blend-back)
    code[8] = 0x05;
    code[9] = 0xE9;                                   // jmp Draw2DQuad
    const int32_t toDraw = static_cast<int32_t>(draw2d - (reinterpret_cast<uintptr_t>(stub) + 14));
    memcpy(code + 10, &toDraw, 4);
    code[14] = 0xC3;                                  // ret (cdecl: the caller cleans)
    memcpy(stub, code, sizeof(code));
    FlushInstructionCache(GetCurrentProcess(), stub, sizeof(code));

    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(stub) - (callAt + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[splitblur] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(callAt + 1), &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(callAt), 5);
    Log("[splitblur] screen blur blend-back (call at rva 0x%08X -> Draw2DQuad rva 0x%08X): skipped while split "
        "screen is on; the effects layer keeps refreshing (holograms / refraction); single player unchanged",
        hits[0] + kScreenBlurBlendCall, static_cast<unsigned>(draw2d - ExeBase()));
}

void InstallStencilColourFix()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kStencilSetupTail, hits, 4);
    if (n != 1)
    {
        Log("[splitstencil] shadow-volume setup tail: %d pattern hits (need exactly 1) - NOT installed", n);
        return;
    }
    if (!EnsurePage())
        return;
    const uintptr_t callAt = Rva(hits[0] + kStencilFinalCall);
    const uintptr_t setRs = CallTargetVa(callAt);
    unsigned char* stub = g_page + kStencilStubOffset;
    const uint32_t active = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_active));
    unsigned char code[22];
    code[0] = 0x80;                                   // cmp byte [active], 0
    code[1] = 0x3D;
    memcpy(code + 2, &active, 4);
    code[6] = 0x00;
    code[7] = 0x74;                                   // je +8 (not split: unchanged)
    code[8] = 0x08;
    code[9] = 0xC7;                                   // mov dword [esp+8], 0 (COLORWRITE 7 -> 0)
    code[10] = 0x44;
    code[11] = 0x24;
    code[12] = 0x08;
    code[13] = 0x00;
    code[14] = 0x00;
    code[15] = 0x00;
    code[16] = 0x00;
    code[17] = 0xE9;                                  // jmp SetRenderState
    const int32_t toRs = static_cast<int32_t>(setRs - (reinterpret_cast<uintptr_t>(stub) + 22));
    memcpy(code + 18, &toRs, 4);
    memcpy(stub, code, sizeof(code));
    FlushInstructionCache(GetCurrentProcess(), stub, sizeof(code));

    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(stub) - (callAt + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[splitstencil] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(callAt + 1), &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(callAt + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(callAt), 5);
    Log("[splitstencil] shadow-volume setup (column 2) final COLORWRITE call at rva 0x%08X -> SetRenderState rva "
        "0x%08X: in split screen colour writes stay OFF for the volumes (no wedges, stencil shadows kept)",
        hits[0] + kStencilFinalCall, static_cast<unsigned>(setRs - ExeBase()));
}

} // namespace

void SplitShadowsStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[splitshadows] not used on this build");
        return;
    }
    const bool shadows = ReadIniEnabled("SplitShadowsLow");
    const bool blur = ReadIniEnabled("SplitMotionBlurOff");
    const bool screenBlur = ReadIniEnabled("SplitScreenBlurOff");
    const bool stencil = ReadIniEnabled("SplitStencilFix");
    if (!shadows)
        Log("[splitshadows] BF1GameExt.ini [Display] SplitShadowsLow=0: shadows follow the menu setting");
    if (!blur)
        Log("[splitblur] BF1GameExt.ini [Display] SplitMotionBlurOff=0: motion blur stays on in split screen");
    if (!screenBlur)
        Log("[splitblur] BF1GameExt.ini [Display] SplitScreenBlurOff=0: the screen blur stays on in split screen");
    if (!stencil)
        Log("[splitstencil] BF1GameExt.ini [Display] SplitStencilFix=0: shadow volumes keep the game's colour writes");
    if (!shadows && !blur && !screenBlur && !stencil)
        return;
    if (!onGameThread)
    {
        Log("[splitshadows] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }
    if (shadows)
        InstallShadowGate(build);
    if (blur)
        InstallMotionBlurSkip(build);
    if (screenBlur)
        InstallScreenBlurSkip();
    if (stencil)
        InstallStencilColourFix();
}

} // namespace bf1
