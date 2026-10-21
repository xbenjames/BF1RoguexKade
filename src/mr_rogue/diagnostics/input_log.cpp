// BF1GameExt - input_log.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was input_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 5 scan (2020 build: GOG 1.3 / Steam). Read-only.
//
// Structures (GOG 1.3 RVAs from static analysis; found by pattern at runtime so
// the same code works on Steam):
//   pad array          GetPad(i) = array + i * 0xD8, built with ONE slot
//                      pad +0x08 byte  XInput user index
//                      pad +0x09 byte  connected
//                      pad +0xC0       XINPUT_STATE (+0xC4 buttons, +0xC6/+0xC7
//                                      triggers, +0xC8.. sticks)
//   active pad table   2 DWORDs: the pad each local player uses (written by
//                      the assignment code from the pad loop)
//   player input       object of 0x718 bytes; +0x00 = pointer to its pad
//   pad index list     DWORDs written alongside the player input objects

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "input_log.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

// GetPad(i): push ebp; mov ebp, esp; movzx eax, byte [ebp+8]; imul eax, eax, 0D8h;
//            add eax, <pad array>; pop ebp; ret
const char* const kGetPad = "55 8B EC 0F B6 45 08 69 C0 D8 00 00 00 05 ?? ?? ?? ?? 5D C3";
// pad loop that fills the active pad table: xor edi, edi; mov esi, <table>; ...
const char* const kActive =
    "33 FF BE ?? ?? ?? ?? FF 75 FC E8 ?? ?? ?? ?? 83 C4 04 8B C8 E8 ?? ?? ?? ?? 84 C0 74 05 89 3E "
    "83 C6 04";
// push esi; call GetPad; mov [<player input>], eax; ... mov [<pad index list>], eax
const char* const kPlayerInput =
    "56 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? 83 C4 04 0F B6 45 FE BF 01 00 00 00 A3 ?? ?? ?? ??";

constexpr uint32_t kPadSize = 0xD8;
constexpr int kMaxLines = 600;
constexpr int kMaxButtonLines = 250;
constexpr DWORD kPollMs = 100;

uintptr_t g_padArray = 0;
uintptr_t g_activeTable = 0;
uintptr_t g_playerInput = 0;
uintptr_t g_padIndexList = 0;

int g_lines = 0;
int g_buttonLines = 0;

bool Budget(bool isButtonLine)
{
    if (g_lines >= kMaxLines)
        return false;
    if (isButtonLine)
    {
        if (g_buttonLines >= kMaxButtonLines)
            return false;
        ++g_buttonLines;
    }
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[input] line limit reached, input scan going quiet");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uint8_t ReadU8(uintptr_t va)
{
    return *reinterpret_cast<const volatile uint8_t*>(va);
}

uint16_t ReadU16(uintptr_t va)
{
    uint16_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

// Absolute address stored at 'at' in game code, checked to be inside the exe.
uintptr_t AbsAt(uintptr_t at)
{
    const uintptr_t v = ReadU32(at);
    const BuildInfo* b = CurrentBuild();
    if (!b || v < ExeBase() || v >= ExeBase() + b->sizeOfImage)
        return 0;
    return v;
}

bool FindOne(const char* what, const char* pattern, uint32_t* rva)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pattern, hits, 4);
    if (n != 1)
    {
        Log("[input] %s: %d pattern hits (need exactly 1) - input scan disabled", what, n);
        return false;
    }
    *rva = hits[0];
    return true;
}

// ---------------------------------------------------------------------------
// XInput, loaded by name (our own reads, to see every physical pad)
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

void ButtonNames(WORD b, char* out, size_t outSize)
{
    struct Bit
    {
        WORD mask;
        const char* name;
    };
    static const Bit kBits[] = {
        { 0x0001, "Up" },    { 0x0002, "Down" }, { 0x0004, "Left" },  { 0x0008, "Right" },
        { 0x0010, "Start" }, { 0x0020, "Back" }, { 0x0040, "L3" },    { 0x0080, "R3" },
        { 0x0100, "LB" },    { 0x0200, "RB" },   { 0x1000, "A" },     { 0x2000, "B" },
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

// ---------------------------------------------------------------------------
// Snapshot of the game's side
// ---------------------------------------------------------------------------
struct GameSide
{
    uint8_t padIndex;
    uint8_t padConnected;
    uint16_t padButtons;
    uint32_t active[2];
    uint32_t playerPad;
    uint32_t padList[2];
    uint32_t players;
};

void TakeGame(GameSide& g)
{
    memset(&g, 0, sizeof(g));
    g.padIndex = ReadU8(g_padArray + 0x08);
    g.padConnected = ReadU8(g_padArray + 0x09);
    g.padButtons = ReadU16(g_padArray + 0xC4);
    g.active[0] = ReadU32(g_activeTable);
    g.active[1] = ReadU32(g_activeTable + 4);
    g.playerPad = ReadU32(g_playerInput);
    g.padList[0] = ReadU32(g_padIndexList);
    g.padList[1] = ReadU32(g_padIndexList + 4);
    const BuildInfo* b = CurrentBuild();
    g.players = (b && b->localPlayerCountRva) ? ReadU32(Rva(b->localPlayerCountRva)) : 0;
}

void DescribePadPtr(uint32_t p, char* out, size_t outSize)
{
    if (p == 0)
        snprintf(out, outSize, "none");
    else if (p == static_cast<uint32_t>(g_padArray))
        snprintf(out, outSize, "game pad 0");
    else
        snprintf(out, outSize, "0x%08X (not the game's pad)", static_cast<unsigned>(p));
}

void LogGameSide(const char* why, const GameSide& g)
{
    if (!Budget(false))
        return;
    char pp[48];
    DescribePadPtr(g.playerPad, pp, sizeof(pp));
    Log("[input] %lu ms: %s game pad: XInput index %u, connected %u | active pad table [%ld, %ld] | "
        "player input uses %s | pad index list [%ld, %ld] | local players %lu",
        ScanMsSinceAttach(), why, static_cast<unsigned>(g.padIndex),
        static_cast<unsigned>(g.padConnected), static_cast<long>(static_cast<int32_t>(g.active[0])),
        static_cast<long>(static_cast<int32_t>(g.active[1])), pp,
        static_cast<long>(static_cast<int32_t>(g.padList[0])),
        static_cast<long>(static_cast<int32_t>(g.padList[1])),
        static_cast<unsigned long>(g.players));
}

bool GameSideDiffers(const GameSide& a, const GameSide& b)
{
    return a.padIndex != b.padIndex || a.padConnected != b.padConnected ||
           a.active[0] != b.active[0] || a.active[1] != b.active[1] ||
           a.playerPad != b.playerPad || a.padList[0] != b.padList[0] ||
           a.padList[1] != b.padList[1] || a.players != b.players;
}

DWORD WINAPI InputScanThread(LPVOID)
{
    LoadXInput();

    GameSide last;
    TakeGame(last);
    LogGameSide("start:", last);

    WORD lastPhys[4] = {};
    bool lastConn[4] = {};
    if (g_getState)
    {
        for (DWORD i = 0; i < 4; ++i)
        {
            XState st = {};
            lastConn[i] = g_getState(i, &st) == ERROR_SUCCESS;
            lastPhys[i] = lastConn[i] ? st.Gamepad.wButtons : 0;
            if (Budget(false))
                Log("[input] %lu ms: XInput pad %lu %s", ScanMsSinceAttach(),
                    static_cast<unsigned long>(i), lastConn[i] ? "CONNECTED" : "not connected");
        }
    }
    else if (Budget(false))
    {
        Log("[input] XInput not found - physical pad lines disabled");
    }

    uint16_t lastGameButtons = last.padButtons;
    for (;;)
    {
        Sleep(kPollMs);

        GameSide now;
        TakeGame(now);
        if (GameSideDiffers(now, last))
            LogGameSide("changed:", now);

        if (now.padButtons != lastGameButtons && now.padButtons != 0 && Budget(true))
        {
            char names[96];
            ButtonNames(now.padButtons, names, sizeof(names));
            Log("[input] %lu ms: game pad (XInput index %u) sees: %s", ScanMsSinceAttach(),
                static_cast<unsigned>(now.padIndex), names);
        }
        lastGameButtons = now.padButtons;
        last = now;

        if (!g_getState)
            continue;
        for (DWORD i = 0; i < 4; ++i)
        {
            XState st = {};
            const bool conn = g_getState(i, &st) == ERROR_SUCCESS;
            const WORD b = conn ? st.Gamepad.wButtons : 0;
            if (conn != lastConn[i] && Budget(false))
                Log("[input] %lu ms: XInput pad %lu %s", ScanMsSinceAttach(),
                    static_cast<unsigned long>(i), conn ? "CONNECTED" : "disconnected");
            if (b != lastPhys[i] && b != 0 && Budget(true))
            {
                char names[96];
                ButtonNames(b, names, sizeof(names));
                Log("[input] %lu ms: physical XInput pad %lu pressed: %s", ScanMsSinceAttach(),
                    static_cast<unsigned long>(i), names);
            }
            lastConn[i] = conn;
            lastPhys[i] = b;
        }
    }
}

} // namespace

void InputScanStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build)
    {
        Log("[input] DISABLED - unknown exe");
        return;
    }
    if (build->loadsMemExt)
    {
        Log("[input] not used on %s (it reads pads through DirectInput only)", build->name);
        return;
    }

    uint32_t getPad = 0;
    uint32_t active = 0;
    uint32_t playerInput = 0;
    if (!FindOne("GetPad (pad array)", kGetPad, &getPad))
        return;
    if (!FindOne("active pad table loop", kActive, &active))
        return;
    if (!FindOne("player input assignment", kPlayerInput, &playerInput))
        return;

    g_padArray = AbsAt(Rva(getPad + 14));
    g_activeTable = AbsAt(Rva(active + 3));
    g_playerInput = AbsAt(Rva(playerInput + 7));
    g_padIndexList = AbsAt(Rva(playerInput + 24));
    if (!g_padArray || !g_activeTable || !g_playerInput || !g_padIndexList)
    {
        Log("[input] an address read from the code points outside the exe - input scan disabled");
        return;
    }

    Log("[input] GetPad at rva 0x%08X: pad array rva 0x%08X (1 slot of 0x%X bytes)", getPad,
        static_cast<unsigned>(g_padArray - ExeBase()), static_cast<unsigned>(kPadSize));
    Log("[input] active pad loop at rva 0x%08X: active pad table rva 0x%08X", active,
        static_cast<unsigned>(g_activeTable - ExeBase()));
    Log("[input] player input assignment at rva 0x%08X: player input rva 0x%08X, pad index list "
        "rva 0x%08X",
        playerInput, static_cast<unsigned>(g_playerInput - ExeBase()),
        static_cast<unsigned>(g_padIndexList - ExeBase()));

    const HANDLE t = CreateThread(nullptr, 0, InputScanThread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[input] scan started, polling every %lu ms", static_cast<unsigned long>(kPollMs));
    }
    else
    {
        Log("[input] thread failed to start (error %lu)", GetLastError());
    }
}

} // namespace bf1
