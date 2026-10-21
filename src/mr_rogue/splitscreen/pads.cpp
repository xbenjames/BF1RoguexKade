// BF1GameExt - pads.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was pad2.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 20e: pads 3 and 4 (players34.cpp) are built and driven with the same game pad code through PadBuild /
// PadPoint / PadUpdate below; pad 2 itself is unchanged.
// Pad 2 (2020 build). Built with the game's own pad code; driven from the game's
// frame by player2_input.cpp (phase 6b), or from a thread as a fallback.
//
// Pad class (GOG 1.3, verified; found by pattern on Steam):
//   size 0xD8, constructed by the pad array's static constructor
//   ctor        mov [ecx], vtable; mov eax, ecx; ret
//   SetIndex(i) [+8] = i (XInput user index), [+9] = 0 (connected); ret 4
//   Probe()     XInputGetState once, sets [+9], clears vibration; ret
//   Update(dt)  XInputGetState, deadzones, processed sticks at +0x0C..+0x18,
//               button edges, XInputSetState; ret 4
//   +0x09 byte connected, +0xC4 WORD buttons (inside XINPUT_STATE at +0xC0)
// All of these are thiscall; they are called here through __fastcall pointers
// (ECX = this, EDX unused, stack arguments popped by the callee), which is the
// same call shape.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "pads.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

// Static constructor of the pad array:
//   push dtor; push ctor; push 1; push 0D8h; push <pad array>; call vector-ctor
const char* const kStaticCtor = "68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 01 68 D8 00 00 00 68 ?? ?? ?? ?? E8";
// Pad init: push 0; mov ecx, pad; call SetIndex; mov ecx, pad; call Probe;
//           mov ecx, pad; call IsConnected; test al, al
const char* const kInit =
    "6A 00 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
    "84 C0";
// Per-frame device update: ... push ecx; mov ecx, <device>; movss [esp], xmm0; call Update
const char* const kFrame = "55 8B EC F3 0F 10 45 08 51 B9 ?? ?? ?? ?? F3 0F 11 04 24 E8 ?? ?? ?? ??";

const char* const kCtorBody = "C7 01 ?? ?? ?? ?? 8B C1 C3";
const char* const kSetIndexBody = "55 8B EC 8A 45 08 88 41 08 C6 41 09 00 5D C2 04 00";
const char* const kProbeBody = "56 8B F1 8D 86 C0 00 00 00 50 0F B6 46 08 50 E8";
const char* const kUpdateBody = "53 56 8B F1 57 0F B6 46 08 8D BE C0 00 00 00 57 50 E8";
const char* const kIsConnectedBody = "8A 41 09 C3";

constexpr uint32_t kPadSize = 0xD8;
constexpr int kMaxLines = 400;
constexpr DWORD kUpdateMs = 16;
constexpr DWORD kReselectAfterMs = 2000;
constexpr DWORD kStickLogEveryMs = 500;
constexpr float kStickLogThreshold = 0.5f;

typedef void*(__fastcall* PadCtor_t)(void* self, void* edx);
typedef void(__fastcall* PadSetIndex_t)(void* self, void* edx, uint32_t index);
typedef void(__fastcall* PadProbe_t)(void* self, void* edx);
typedef void(__fastcall* PadUpdate_t)(void* self, void* edx, float dt);

PadCtor_t g_ctor = nullptr;
PadSetIndex_t g_setIndex = nullptr;
PadProbe_t g_probe = nullptr;
PadUpdate_t g_update = nullptr;
uintptr_t g_gamePad = 0;

alignas(16) unsigned char g_pad2[kPadSize];

int g_lines = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[pad2] line limit reached, going quiet");
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

// Target of the E8 at rva, following incremental-link E9 thunks.
uint32_t CallTarget(uint32_t rva)
{
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize))
        return 0;
    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
    for (int hop = 0; hop < 8; ++hop)
    {
        if (t < textRva || t >= textRva + textSize)
            return 0;
        if (*reinterpret_cast<const unsigned char*>(Rva(t)) != 0xE9)
            return t;
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    }
    return 0;
}

// Absolute pointer stored at rva -> RVA, following thunks if it points to code.
uint32_t PointerToRva(uint32_t rva)
{
    const uintptr_t v = ReadU32(Rva(rva));
    if (v < ExeBase())
        return 0;
    uint32_t t = static_cast<uint32_t>(v - ExeBase());
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    return t;
}

bool Check(const char* what, uint32_t rva, const char* body)
{
    if (rva && MatchPatternAt(rva, body))
    {
        Log("[pad2] %-12s rva 0x%08X  OK", what, rva);
        return true;
    }
    Log("[pad2] %-12s rva 0x%08X  code does NOT match - pad 2 disabled", what, rva);
    return false;
}

bool FindPadCode()
{
    uint32_t hits[8] = {};

    int n = FindPattern(kStaticCtor, hits, 8);
    if (n != 1)
    {
        Log("[pad2] pad array constructor: %d pattern hits (need 1) - pad 2 disabled", n);
        return false;
    }
    const uint32_t staticCtor = hits[0];
    const uint32_t padRva = static_cast<uint32_t>(ReadU32(Rva(staticCtor + 18)) - ExeBase());
    const uint32_t ctorRva = PointerToRva(staticCtor + 6);
    g_gamePad = Rva(padRva);
    Log("[pad2] game pad at rva 0x%08X (array of 1 x 0x%X)", padRva, static_cast<unsigned>(kPadSize));
    if (!Check("constructor", ctorRva, kCtorBody))
        return false;

    n = FindPattern(kInit, hits, 8);
    if (n != 1 || ReadU32(Rva(hits[0] + 3)) != g_gamePad)
    {
        Log("[pad2] pad init: %d pattern hits, or not on the game pad - pad 2 disabled", n);
        return false;
    }
    const uint32_t init = hits[0];
    const uint32_t setIndexRva = CallTarget(init + 7);
    const uint32_t probeRva = CallTarget(init + 17);
    const uint32_t isConnectedRva = CallTarget(init + 27);
    if (!Check("SetIndex", setIndexRva, kSetIndexBody) || !Check("Probe", probeRva, kProbeBody) ||
        !Check("IsConnected", isConnectedRva, kIsConnectedBody))
        return false;

    n = FindPattern(kFrame, hits, 8);
    uint32_t updateRva = 0;
    int onPad = 0;
    for (int i = 0; i < n && i < 8; ++i)
    {
        if (ReadU32(Rva(hits[i] + 10)) != g_gamePad)
            continue;
        ++onPad;
        updateRva = CallTarget(hits[i] + 19);
    }
    if (onPad != 1)
    {
        Log("[pad2] per-frame pad update: %d of %d frame-update hits act on the game pad (need 1) - "
            "pad 2 disabled",
            onPad, n);
        return false;
    }
    if (!Check("Update", updateRva, kUpdateBody))
        return false;

    g_ctor = reinterpret_cast<PadCtor_t>(Rva(ctorRva));
    g_setIndex = reinterpret_cast<PadSetIndex_t>(Rva(setIndexRva));
    g_probe = reinterpret_cast<PadProbe_t>(Rva(probeRva));
    g_update = reinterpret_cast<PadUpdate_t>(Rva(updateRva));
    return true;
}

// ---------------------------------------------------------------------------
// Picking pad 2's XInput index: the first connected pad that the game's own
// pad is not using.
// ---------------------------------------------------------------------------
struct XPad
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
};

struct XState
{
    DWORD dwPacketNumber;
    XPad Gamepad;
};

typedef DWORD(WINAPI* XInputGetState_t)(DWORD, XState*);
XInputGetState_t g_getState = nullptr;

void LoadXInput()
{
    const char* const names[] = { "xinput1_4.dll", "xinput9_1_0.dll", "xinput1_3.dll" };
    for (const char* name : names)
    {
        const HMODULE h = LoadLibraryA(name);
        if (!h)
            continue;
        g_getState = reinterpret_cast<XInputGetState_t>(
            reinterpret_cast<void*>(GetProcAddress(h, "XInputGetState")));
        if (g_getState)
            return;
    }
}

int GamePadIndex()
{
    return *reinterpret_cast<const volatile uint8_t*>(g_gamePad + 0x08);
}

int PickIndex()
{
    if (!g_getState)
        return -1;
    const int gameIndex = GamePadIndex();
    for (DWORD i = 0; i < 4; ++i)
    {
        if (static_cast<int>(i) == gameIndex)
            continue;
        XState st = {};
        if (g_getState(i, &st) == ERROR_SUCCESS)
            return static_cast<int>(i);
    }
    return -1;
}

void ButtonNames(WORD b, char* out, size_t outSize)
{
    struct Bit
    {
        WORD mask;
        const char* name;
    };
    static const Bit kBits[] = {
        { 0x0001, "Up" },    { 0x0002, "Down" }, { 0x0004, "Left" }, { 0x0008, "Right" },
        { 0x0010, "Start" }, { 0x0020, "Back" }, { 0x0040, "L3" },   { 0x0080, "R3" },
        { 0x0100, "LB" },    { 0x0200, "RB" },   { 0x1000, "A" },    { 0x2000, "B" },
        { 0x4000, "X" },     { 0x8000, "Y" },
    };
    out[0] = 0;
    size_t used = 0;
    for (const Bit& bit : kBits)
    {
        if (!(b & bit.mask))
            continue;
        const int w = snprintf(out + used, outSize - used, "%s%s", used ? "+" : "", bit.name);
        if (w < 0 || static_cast<size_t>(w) >= outSize - used)
            break;
        used += static_cast<size_t>(w);
    }
    if (!used)
        snprintf(out, outSize, "none");
}

float ReadFloat(size_t offset)
{
    float f = 0.0f;
    memcpy(&f, g_pad2 + offset, sizeof(f));
    return f;
}

bool Connected()
{
    return *reinterpret_cast<const volatile uint8_t*>(g_pad2 + 0x09) != 0;
}

WORD Buttons()
{
    WORD b = 0;
    memcpy(&b, g_pad2 + 0xC4, sizeof(b));
    return b;
}

// ---------------------------------------------------------------------------
// Driving pad 2
// ---------------------------------------------------------------------------
bool g_ready = false;
int g_index = -1;
bool g_needPick = true;
DWORD g_lastPick = 0;
bool g_waitingLogged = false;
bool g_wasConnected = false;
WORD g_lastButtons = 0;
DWORD g_disconnectedSince = 0;
DWORD g_lastStickLog = 0;

void PointAt(int index, const char* why)
{
    g_index = index;
    g_setIndex(g_pad2, nullptr, static_cast<uint32_t>(index));
    g_probe(g_pad2, nullptr);
    g_wasConnected = Connected();
    g_disconnectedSince = 0;
    if (Budget())
        Log("[pad2] %lu ms: pad 2 %s XInput index %d, connected %d (the game's pad uses index %d)",
            ScanMsSinceAttach(), why, index, g_wasConnected ? 1 : 0, GamePadIndex());
}

DWORD WINAPI Pad2Thread(LPVOID)
{
    LARGE_INTEGER freq;
    LARGE_INTEGER last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
    for (;;)
    {
        Sleep(kUpdateMs);
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        float dt = static_cast<float>(now.QuadPart - last.QuadPart) /
                   static_cast<float>(freq.QuadPart);
        last = now;
        if (dt < 0.0f || dt > 0.1f)
            dt = 0.016f;
        Pad2Tick(dt);
    }
}

} // namespace

bool Pad2Init()
{
    if (g_ready)
        return true;
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[pad2] not used on this build");
        return false;
    }
    Log("[pad2] pads.cpp file build p23a-r1");
    if (!FindPadCode())
        return false;

    LoadXInput();
    g_ctor(g_pad2, nullptr);
    uint32_t vtable = 0;
    memcpy(&vtable, g_pad2, sizeof(vtable));
    Log("[pad2] pad 2 built with the game's constructor (vtable 0x%08X)",
        static_cast<unsigned>(vtable));
    g_ready = true;
    return true;
}

void Pad2Tick(float dt)
{
    if (!g_ready)
        return;
    const DWORD tick = GetTickCount();

    // Pick / re-pick at most once a second.
    if (g_needPick && tick - g_lastPick >= 1000)
    {
        g_lastPick = tick;
        const int next = PickIndex();
        if (next >= 0 && next != g_index)
        {
            PointAt(next, g_index < 0 ? "->" : "re-pointed to");
            g_needPick = false;
        }
        else if (next < 0 && g_index < 0 && !g_waitingLogged && Budget())
        {
            Log("[pad2] %lu ms: waiting for a second XInput pad (the game's pad uses index %d)",
                ScanMsSinceAttach(), GamePadIndex());
            g_waitingLogged = true;
        }
        else if (next == g_index)
        {
            g_needPick = false;
        }
    }
    if (g_index < 0)
        return;

    g_update(g_pad2, nullptr, dt);

    const bool conn = Connected();
    if (conn != g_wasConnected && Budget())
        Log("[pad2] %lu ms: XInput index %d %s", ScanMsSinceAttach(), g_index,
            conn ? "connected" : "DISCONNECTED");
    g_wasConnected = conn;

    if (!conn)
    {
        if (!g_disconnectedSince)
            g_disconnectedSince = tick;
        else if (tick - g_disconnectedSince > kReselectAfterMs)
            g_needPick = true;
    }
    else
    {
        g_disconnectedSince = 0;
    }
    if (GamePadIndex() == g_index)
        g_needPick = true;

    const WORD b = conn ? Buttons() : 0;
    if (b != g_lastButtons && b != 0 && Budget())
    {
        char names[96];
        ButtonNames(b, names, sizeof(names));
        Log("[pad2] %lu ms: pad 2 (XInput %d) sees: %s", ScanMsSinceAttach(), g_index, names);
    }
    g_lastButtons = b;

    if (conn && tick - g_lastStickLog >= kStickLogEveryMs)
    {
        const float lx = ReadFloat(0x0C);
        const float ly = ReadFloat(0x10);
        const float rx = ReadFloat(0x14);
        const float ry = ReadFloat(0x18);
        if ((std::fabs(lx) > kStickLogThreshold || std::fabs(ly) > kStickLogThreshold ||
             std::fabs(rx) > kStickLogThreshold || std::fabs(ry) > kStickLogThreshold) &&
            Budget())
        {
            Log("[pad2] %lu ms: pad 2 sticks (after the game's deadzone): left %.2f %.2f, right "
                "%.2f %.2f",
                ScanMsSinceAttach(), lx, ly, rx, ry);
            g_lastStickLog = tick;
        }
    }
}

void Pad2StartThread()
{
    if (!g_ready)
        return;
    const HANDLE t = CreateThread(nullptr, 0, Pad2Thread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[pad2] driven by its own thread every %lu ms (fallback)",
            static_cast<unsigned long>(kUpdateMs));
    }
    else
    {
        Log("[pad2] thread failed to start (error %lu)", GetLastError());
    }
}

uintptr_t Pad2Address()
{
    return reinterpret_cast<uintptr_t>(g_pad2);
}

bool Pad2IsConnected()
{
    return g_ready && g_index >= 0 && Connected();
}

uintptr_t GamePadAddress()
{
    return g_gamePad;
}

// 20e: more pads of the game's class, for players 3 and 4.
bool PadBuild(void* mem)
{
    if (!g_ready || !g_ctor || !mem)
        return false;
    g_ctor(mem, nullptr);
    return true;
}

void PadPoint(void* pad, int index)
{
    if (!g_ready || !pad || index < 0 || index > 3)
        return;
    g_setIndex(pad, nullptr, static_cast<uint32_t>(index));
    g_probe(pad, nullptr);
}

void PadUpdate(void* pad, float dt)
{
    if (g_ready && pad)
        g_update(pad, nullptr, dt);
}

bool PadConnected(const void* pad)
{
    return pad && *(reinterpret_cast<const volatile uint8_t*>(pad) + 0x09) != 0;
}

uint16_t PadButtons(const void* pad)
{
    uint16_t b = 0;
    if (pad)
        memcpy(&b, reinterpret_cast<const unsigned char*>(pad) + 0xC4, sizeof(b));
    return b;
}

int GamePadXInputIndex()
{
    return g_gamePad ? GamePadIndex() : -1;
}

int Pad2XInputIndex()
{
    return g_index;
}

bool XInputPadPresent(int index)
{
    if (!g_getState || index < 0 || index > 3)
        return false;
    XState st = {};
    return g_getState(static_cast<DWORD>(index), &st) == ERROR_SUCCESS;
}

} // namespace bf1
