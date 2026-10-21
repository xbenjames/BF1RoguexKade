// BF1GameExt - split_core.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was split_test.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 2: split screen test.
//
// 1. Lua wrappers. The game registers its ScriptCB_ functions from a static
//    { name, function } table in .data. Before that happens (DllMain), four
//    split screen entries are pointed at wrappers here. Each wrapper calls the
//    game's original function, then logs what the menu scripts did.
//
// 2. Sticky split. F9 turns split ON: 2 local players, and if a menu script
//    calls ScriptCB_SetSplitscreen(false) while it's on, the wrapper puts the
//    count back to 2 and logs that. F10 turns it OFF.
//
// Game code patched: none. Game data changed: 4 Lua registration slots, and
// the local player count when you press F9/F10.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "render_hooks.hpp"
#include "log.hpp"
#include "player2_input.hpp"
#include "game_check.hpp"
#include "split_core.hpp"

namespace bf1 {
namespace {

// ---------------------------------------------------------------------------
// Shared state
// ---------------------------------------------------------------------------
volatile LONG g_splitOn = 0; // set by F9, cleared by F10

uintptr_t PlayerCountAddr()
{
    return Rva(CurrentBuild()->localPlayerCountRva);
}

// On Steam the addresses are only known once the scan has found them in memory.
bool ValuesKnown()
{
    const BuildInfo* b = CurrentBuild();
    return b && b->localPlayerCountRva && b->wasSplitRva;
}

uint32_t PlayerCount()
{
    return *reinterpret_cast<volatile const uint32_t*>(PlayerCountAddr());
}

void WritePlayerCount(uint32_t players)
{
    *reinterpret_cast<volatile uint32_t*>(PlayerCountAddr()) = players;
}

uint8_t WasSplit()
{
    return *reinterpret_cast<volatile const uint8_t*>(Rva(CurrentBuild()->wasSplitRva));
}

// ---------------------------------------------------------------------------
// Lua wrappers
// ---------------------------------------------------------------------------
typedef int(__cdecl* LuaCFunction)(void* L);

LuaCFunction g_origSetSplit = nullptr;
LuaCFunction g_origIsSplit = nullptr;
LuaCFunction g_origWasSplit = nullptr;
LuaCFunction g_origSetHoriz = nullptr;

constexpr int kMaxSetLines = 200;
constexpr int kMaxQueryLines = 40;
int g_setLines = 0;
int g_isLines = 0;
int g_wasLines = 0;
int g_horizLines = 0;
int g_lastIsResult = -1;
int g_lastWasResult = -1;

int __cdecl Wrap_SetSplitscreen(void* L)
{
    if (!ValuesKnown())
        return g_origSetSplit(L);
    const uint32_t before = PlayerCount();
    const int ret = g_origSetSplit(L);
    const uint32_t after = PlayerCount();

    const bool keep = g_splitOn && after < 2;
    if (keep)
        WritePlayerCount(2);

    if (g_setLines < kMaxSetLines)
    {
        ++g_setLines;
        if (keep)
            Log("[lua] %lu ms: ScriptCB_SetSplitscreen(false): game set %lu -> %lu, "
                "F9 split is ON so the DLL kept 2",
                ScanMsSinceAttach(), static_cast<unsigned long>(before),
                static_cast<unsigned long>(after));
        else
            Log("[lua] %lu ms: ScriptCB_SetSplitscreen(%s): local players %lu -> %lu",
                ScanMsSinceAttach(), after > 1 ? "true" : "false",
                static_cast<unsigned long>(before), static_cast<unsigned long>(after));
    }
    return ret;
}

// Queries can run often: log the first few calls, then only when the answer changes.
int __cdecl Wrap_IsSplitscreen(void* L)
{
    const int ret = g_origIsSplit(L);
    if (!ValuesKnown())
        return ret;
    const int result = PlayerCount() > 1 ? 1 : 0;
    if (g_isLines < kMaxQueryLines && (g_isLines < 5 || result != g_lastIsResult))
    {
        ++g_isLines;
        Log("[lua] %lu ms: ScriptCB_IsSplitscreen() -> %s", ScanMsSinceAttach(),
            result ? "true" : "false");
    }
    g_lastIsResult = result;
    return ret;
}

int __cdecl Wrap_WasSplitscreen(void* L)
{
    const int ret = g_origWasSplit(L);
    if (!ValuesKnown())
        return ret;
    const int result = WasSplit() ? 1 : 0;
    if (g_wasLines < kMaxQueryLines && (g_wasLines < 5 || result != g_lastWasResult))
    {
        ++g_wasLines;
        Log("[lua] %lu ms: ScriptCB_WasSplitscreen() -> %s", ScanMsSinceAttach(),
            result ? "true" : "false");
    }
    g_lastWasResult = result;
    return ret;
}

int __cdecl Wrap_SetHorizontalSplitScreen(void* L)
{
    const int ret = g_origSetHoriz(L);
    if (g_horizLines < kMaxQueryLines)
    {
        ++g_horizLines;
        Log("[lua] %lu ms: ScriptCB_SetHorizontalSplitScreen() called (does nothing in the "
            "PC exe)",
            ScanMsSinceAttach());
    }
    return ret;
}

// The function each slot must hold comes from the build table (game_build.cpp).
struct LuaHook
{
    const char* name;
    LuaCFunction wrapper;
    LuaCFunction* original;
};

const LuaHook kLuaHooks[] = {
    { "ScriptCB_SetSplitscreen", Wrap_SetSplitscreen, &g_origSetSplit },
    { "ScriptCB_IsSplitscreen", Wrap_IsSplitscreen, &g_origIsSplit },
    { "ScriptCB_WasSplitscreen", Wrap_WasSplitscreen, &g_origWasSplit },
    { "ScriptCB_SetHorizontalSplitScreen", Wrap_SetHorizontalSplitScreen, &g_origSetHoriz },
};

// ---------------------------------------------------------------------------
// Hotkeys
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

constexpr WORD kPadStart = 0x0010;
constexpr WORD kPadBack = 0x0020;
constexpr WORD kPadLB = 0x0100;
constexpr WORD kPadRB = 0x0200;
constexpr WORD kPadY = 0x8000;
constexpr WORD kPadX = 0x4000;

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

bool GameWindowInFront()
{
    const HWND fg = GetForegroundWindow();
    if (!fg)
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    return pid == GetCurrentProcessId();
}

bool KeyDown(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool PadCombo(WORD buttons)
{
    if (!g_getState)
        return false;
    for (DWORD pad = 0; pad < 4; ++pad)
    {
        XState st = {};
        if (g_getState(pad, &st) == ERROR_SUCCESS && (st.Gamepad.wButtons & buttons) == buttons)
            return true;
    }
    return false;
}

void SplitOn(const char* why)
{
    const uint32_t before = PlayerCount();
    const bool wasOn = InterlockedExchange(&g_splitOn, 1) != 0;
    WritePlayerCount(2);
    if (!wasOn || before != 2)
        Log("[split] %lu ms: %s: split ON (sticky), local players %lu -> 2", ScanMsSinceAttach(),
            why, static_cast<unsigned long>(before));
}

void SplitOff(const char* why)
{
    const uint32_t before = PlayerCount();
    const bool wasOn = InterlockedExchange(&g_splitOn, 0) != 0;
    WritePlayerCount(1);
    if (wasOn || before != 1)
        Log("[split] %lu ms: %s: split OFF, local players %lu -> 1", ScanMsSinceAttach(), why,
            static_cast<unsigned long>(before));
}

DWORD WINAPI HotkeyThread(LPVOID)
{
    LoadXInput();
    Log("[split] ready: F9 or LB+RB+Back = split ON (stays on), F10 or LB+RB+Start = OFF. "
        "Use them in the menus, before starting a match.%s",
        g_getState ? "" : " XInput not found, keyboard only.");

    bool onWasDown = false;
    bool offWasDown = false;
    bool f11WasDown = false;
    bool insWasDown = false;
    bool findWasDown = false;
    bool numWasDown[10] = {};
    bool starWasDown = false;
    bool slashWasDown = false;
    bool minusWasDown = false;
    for (;;)
    {
        Sleep(50);
        if (!GameWindowInFront())
        {
            onWasDown = false;
            offWasDown = false;
            continue;
        }

        const bool f9 = KeyDown(VK_F9);
        const bool f10 = KeyDown(VK_F10);
        const bool onDown = f9 || PadCombo(kPadLB | kPadRB | kPadBack);
        const bool offDown = f10 || PadCombo(kPadLB | kPadRB | kPadStart);

        if (onDown && !onWasDown)
            SplitOn(f9 ? "F9" : "LB+RB+Back");
        if (offDown && !offWasDown)
            SplitOff(f10 ? "F10" : "LB+RB+Start");

        onWasDown = onDown;
        offWasDown = offDown;

        const bool f11 = KeyDown(VK_F11);
        if (f11 && !f11WasDown)
            Player2RedirectToggle();
        f11WasDown = f11;

        // Frame capture: Insert, Home or LB+RB+Y (not F12: Steam's screenshot key
        // swallows it; some keyboards hide Insert behind Fn)
        const bool ins = KeyDown(VK_INSERT) || KeyDown(VK_HOME) || PadCombo(kPadLB | kPadRB | kPadY);
        if (ins && !insWasDown)
            FrameCaptureArm();
        insWasDown = ins;

        // Draw-group switches (11e): Num 1-9 flip one draw group off / on, Num 0
        // turns everything back on (NumLock must be on); LB+RB+X cycles.
        for (int k = 0; k <= 9; ++k)
        {
            const bool down = KeyDown(VK_NUMPAD0 + k);
            if (down && !numWasDown[k])
            {
                if (k == 0)
                    FrameCaptureAllOn();
                else
                    FrameCaptureToggleSource(k - 1);
            }
            numWasDown[k] = down;
        }
        const bool star = KeyDown(VK_MULTIPLY);
        if (star && !starWasDown)
            FrameCaptureToggleScreenSampling();
        starWasDown = star;
        const bool slash = KeyDown(VK_DIVIDE);
        if (slash && !slashWasDown)
            FrameCaptureToggleCacheSync();
        slashWasDown = slash;
        const bool minus = KeyDown(VK_SUBTRACT);
        if (minus && !minusWasDown)
            FrameCaptureToggleBlur();
        minusWasDown = minus;
        const bool find = PadCombo(kPadLB | kPadRB | kPadX);
        if (find && !findWasDown)
            FrameCaptureCycleSkip();
        findWasDown = find;
    }
}

} // namespace

void SplitTestInstallLuaHooks()
{
    if (!CurrentBuild())
    {
        Log("[lua] wrappers NOT installed - unknown exe");
        return;
    }

    int installed = 0;
    for (const LuaHook& h : kLuaHooks)
    {
        const uintptr_t slot = ScanLuaFuncSlot(h.name);
        if (!slot)
        {
            Log("[lua] %s: registration slot NOT FOUND - not wrapped", h.name);
            continue;
        }

        const uint32_t expectedRva = LuaFuncRva(h.name);
        const uint32_t expected = static_cast<uint32_t>(Rva(expectedRva));
        uint32_t current = 0;
        memcpy(&current, reinterpret_cast<const void*>(slot), sizeof(current));
        if (!expectedRva || current != expected)
        {
            Log("[lua] %s: slot holds rva 0x%08X, expected rva 0x%08X - not wrapped", h.name,
                static_cast<unsigned>(current - ExeBase()), static_cast<unsigned>(expectedRva));
            continue;
        }

        *h.original = reinterpret_cast<LuaCFunction>(static_cast<uintptr_t>(current));
        const uint32_t wrapper = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(h.wrapper));
        memcpy(reinterpret_cast<void*>(slot), &wrapper, sizeof(wrapper));
        ++installed;
        Log("[lua] %s: slot rva 0x%08X check OK (function rva 0x%08X), now logged", h.name,
            static_cast<unsigned>(slot - ExeBase()), static_cast<unsigned>(expectedRva));
    }
    Log("[lua] %d of %d split screen Lua functions wrapped", installed,
        static_cast<int>(sizeof(kLuaHooks) / sizeof(kLuaHooks[0])));
}

// BF1GameExt.ini, next to the game's exe:
//   [Splitscreen]
//   Enabled=1   split screen on automatically (the sticky F9 mode from the start)
// It does NOT write 2 players at startup: with the count already at 2 while the
// game first loads its menus, the developer "boot into split screen" switch
// would trip. The menus' first SetSplitscreen(false) call (about 10 s in) is
// held at 2 by the Lua wrapper, exactly like pressing F9.
static void ApplyIniSplit()
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return;
    *slash = 0;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);

    const bool exists = GetFileAttributesA(ini) != INVALID_FILE_ATTRIBUTES;
    const UINT enabled = GetPrivateProfileIntA("Splitscreen", "Enabled", 0, ini);
    if (enabled)
    {
        InterlockedExchange(&g_splitOn, 1);
        Log("[split] %lu ms: BF1GameExt.ini [Splitscreen] Enabled=1: split screen ON automatically "
            "(sticky) - the menus' first SetSplitscreen call will set 2 players. F10 turns it off.",
            ScanMsSinceAttach());
    }
    else
    {
        Log("[split] BF1GameExt.ini %s, [Splitscreen] Enabled=0: split screen starts OFF (F9 turns "
            "it on)",
            exists ? "found" : "not found");
    }
}

void SplitTestStart()
{
    ApplyIniSplit();
    const HANDLE t = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
    else
        Log("[split] hotkey thread failed to start (error %lu)", GetLastError());
}

} // namespace bf1
