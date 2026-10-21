// BF1GameExt - spawn_ui.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was spawn_ui_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 22e: its memory search (log only) runs to the top of the address space Windows gives the game (4 GB with the exe's 4 GB
// flag - BF1GameExt_4GB.bat), not to a fixed 2 GB. Nothing else changed.
// 21j-r32: THE NAMED POP GUARD (the Galactic Conquest end crash). The game's PopScreen(name) (GOG 1.3 0x005347D0, the
// one this file already hooks for its log; ScriptCB_PopScreen("...") reaches it on the current manager) walks the menu
// stack top down - entry i at [mgr+0F8h+i*4], i from depth-1 to 0, each one's name at entry+8 - and _stricmp's each name
// with the one asked for. The bottom entry (0) is always empty (nothing was up before the shell's first push: "top was
// '', depth 0"), so a name that is not on the stack ends in _stricmp(8, name) - the read of address 8. Dave's r31 log: at
// the end of a won conquest, after both profiles were saved, ifs_meta_movie's ScriptCB_PopScreen("ifs_sp") (back to the
// game modes) crashed there - his shell's game modes screen is "ifs_sp2" (also ifs_instant_top2, ifs_missionselect2,
// ifs_legion: a modded shell), so ifs_sp was never pushed. Now, before the walk: a name it finds above the first empty
// entry - the game's own; one it would not - the same name with a 2 ("ifs_sp" -> "ifs_sp2") or without its last 2, then
// "ifs_main", the first of them on the stack is popped to instead; none of them - a plain one-screen PopScreen() (the
// game's own pop, which handles an empty entry). The bytes it relies on are checked at start ([ifs] 21j-r32 line); every
// named pop logs the whole stack (80 lines at most). Nothing else changed; a name the game finds is untouched.
// 21j-r15: the menu layer (popups, option screens) - (1) the 2-player split: while player 2's pause is the one on
// (split_spawn_menus.cpp's P2SpawnMenuOwner answers 2, [Splitscreen] PauseLikeP1P2=1) it is drawn in a 16:9 box of his own half
// (render_hooks.cpp MenuViewportBeginOwn), as player 1's is in his - his OPTIONS were stretched over his whole half; (2) the
// 3-player layout: player 1's goes in his top left quarter (MenuViewportBeginRegion, P2SpawnMenuP1Quarter, [Splitscreen]
// MenuBoxP1=1) - it was the whole top half, across player 2's quarter. Nothing else changed.
// 21j-r11: the frame-rate scan's parts (render_hooks.cpp, [perf] lines; log only): the menu layer hook's own work
// (kPerfOurMenu) and the game's menu tree draw in it (kPerfMenuDraw), the HUD layer hook's game draw (kPerfHudDraw), the
// chooser hook (kPerfOurChooser). Nothing else changed.
// 21h-r8: the end of a match with 3 / 4 players: the stats / awards screens (ifs_teamstats / ifs_awardstats) drawn over the
// whole screen as the Xbox's, not in player 1's quarter (split_spawn_menus.cpp's P2SpawnEndStatsFull, [Splitscreen]
// EndStatsFullScreen). Nothing else changed.
// 21h-r3: the menu layer in player 2's quarter. In 3 / 4-player matches, while P2SpawnMenuOwner (split_spawn_menus.cpp) says it is
// player 2's (his pause is on, player 1's is not), the menu layer - the IF root: his "are you sure" popups and option
// screens - is drawn in his box (render_hooks.cpp MenuViewportBeginP2): they were drawn in the last view's place (player
// 4's quarter) or player 1's box. [Splitscreen] MenuBoxP2=1. 2-player and everything else unchanged.
// 19i: the chooser's switch made at once. The chooser (mode 1) pops the top screen and pushes the one
// it builds when they differ; both only queue (a screen is up), and the IF update's tick runs them -
// after the chooser, before the draw - so the new console screen drew one frame as the game left it
// (its Lua Enter after ClassWheelFrame's setup: the game's own soldier, HINTS, the old panels; 19h-r2's
// video). Now, when the console flow wants another screen than the top, the hook makes the same
// PopScreen / PushScreen(name, 0, 0) itself and settles the manager (P2SpawnSettleManager: the
// manager's tick with dt 0 runs Exit, SetScreen, Enter and the activation at once) BEFORE
// ClassWheelFrame sets the screen up; the chooser's body then finds the top as it wants it and does
// nothing. The side screen's own elements go one frame later (class_wheel.cpp), after that switch.
// 18w: in split screen the chooser shows the class wheel's screen too (ClassWheelSplitWanted).
// 18t: SpawnUiPushScreenFn / SpawnUiPopScreenFn - the game's PushScreen / PopScreen (found and
// byte-checked here, entries hooked for logging), for the player 2 screen preview. Nothing else changed.
// 18s: the PushScreen / PopScreen log lines say which IF manager (0 = player 1's, 1 = player 2's)
// the call is on, and the menu layer hook hands the root to the player 2 spawn scan
// (P2SpawnMenuLayer, log-only) before the game draws it. Nothing else changed.
// 18e: the chooser hook also runs the class wheel's console driver (ClassWheelFrame) every frame
// the spawn screen is up, on the game thread, with the screen the chooser is about to show.
// 18d: the chooser's screen-name swap takes any screen: in 1-player games the class wheel scan's
// F6 preview picks it (class_wheel.cpp ClassWheelPreviewScreen); split screen keeps 13c's
// SplitConsoleSpawn. With nothing picked, the PC screen as the game has it.
// Phase 13a (2020 build): what the game does with menu screens in split screen.
//
// The spawn screen is chosen in one function (GOG 1.3 0x00476D40, thiscall,
// ret 4): mode [obj+18h] 1 -> push "ifs_pc_SpawnSelect" (the PC mouse screen,
// drawn over the whole screen), mode 2 -> push "ifs_readyselect%i" with the
// player number hard-coded to 1 (the console per-player screen). Screens go
// through the IF manager ([0x0085E430]; top screen name at +34h, stack depth at
// +178h, PushScreen's 2nd argument stored at +18Ch):
//   PushScreen(name, arg2, arg3)  GOG 0x005348D0  thiscall, ret 0Ch
//   PopScreen(arg)                GOG 0x005347D0  thiscall, ret 4
// Log-only: each of the three gets a 27-byte stub (save ecx, call a logger with
// a pointer to the stack arguments, restore ecx, run the function's own first
// 6 bytes - pinned by the pattern, no relative operands, nothing jumps into
// them - and jump back). Plus: a snapshot of the menu stack while split screen
// is on, and a one-time read-only search of the game's memory for the console
// spawn screen's scripts (are "ifs_readyselect" screens loaded at all?).
// [ifs] lines.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "class_wheel.hpp"
#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "split_spawn_menus.hpp"
#include "game_check.hpp"
#include "spawn_ui.hpp"

namespace bf1 {
namespace {

const char* const kPushScreen = "55 8B EC 8B 45 0C 56 8B F1 57 85 C0 78 06 89 86 8C 01 00 00 8B BE 78 01 00 00 83 FF "
                                "20 7D";
const char* const kPopScreen = "55 8B EC 53 8B D9 56 8B B3 78 01 00 00 85 F6 0F 8E ?? ?? ?? ?? 4E 83 7D 08 00 75";
const char* const kChooser = "55 8B EC 83 EC 20 56 8B F1 8B 0D ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 8B 56 18 8B F8 83 EA "
                             "01 0F 84";
constexpr uint32_t kChooserMgrOperand = 11;
// 13f: the menu layer. After the view loop the frame-interface function (GOG
// 0x0068EB60) draws the global interface root (the menu screens) with
//   mov ecx,imm ; call A ; push 1 ; call B ; add esp,4 ; call GetRoot ;
//   mov ecx,eax ; call DrawTree ; push 0 ; call B ; add esp,4 ; call C
// DrawTree (GOG 0x00692570: thiscall, no stack arguments, plain ret) is the same
// function the view loop uses for each player's HUD, 42 bytes earlier.
const char* const kMenuLayer = "B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? "
                               "?? ?? 6A 00 E8 ?? ?? ?? ?? 83 C4 04 E8";
constexpr int kMenuCall = 27;      // call DrawTree (the menu layer)
constexpr int kHudCall = -42;      // call DrawTree (each view's HUD), same target
constexpr int kTwoDOn = 12;        // call B (2D on)
constexpr int kTwoDOff = 34;       // call B (2D off), same target
constexpr uint32_t kPrologue = 6;
constexpr int kMaxEventLines = 300;

uintptr_t g_mgrGlobal = 0; // address of the IF manager pointer
// 13c: mode 2 means "Spawn pressed" (13b forced it and player 1 spawned at
// once), so the mode is left alone. Instead, while player 1 is choosing
// (mode 1) in split screen, the chooser's sprintf gets "ifs_readyselect1"
// (the console per-player spawn screen, made for split screen and the pad)
// in place of "ifs_pc_SpawnSelect" (the PC mouse screen drawn over the whole
// screen): the one 'push imm32' of that text inside the chooser is swapped,
// and swapped back outside split screen.
bool g_consoleSpawn = false;
// 13f: 0 = off, 1 = the menu layer in a 16:9 box centred in player 1's half,
// 2 = the menu layer in player 1's whole half (squeezed)
int g_menuFit = 1;
typedef void(__fastcall* DrawTree_t)(void* self, void* unusedEdx);
DrawTree_t g_drawTree = nullptr;
unsigned g_menuFitLines = 0;
bool g_menuWasMoved = false;
bool g_menuWasP2 = false; // 21h-r3: the last frame's menu layer was in player 2's box
// 13g: while player 1's PC spawn screen is up, player 1's own HUD layer (drawn
// in the view loop: the command post map, the score bar) gets the same box, so
// it lines up with the panels as in single player
bool g_spawnHudFit = true;
unsigned g_hudFitLines = 0;
bool g_hudWasMoved = false;
uintptr_t g_namePushImm = 0;   // address of the imm32 of 'push "ifs_pc_SpawnSelect"'
uint32_t g_namePcText = 0;     // its original value
const char kConsoleScreen[] = "ifs_readyselect1";
unsigned g_swapLines = 0;

// 18d: name = the screen to show while choosing, or null for the game's own (ifs_pc_SpawnSelect)
void SetChooserScreen(const char* name)
{
    if (!g_namePushImm)
        return;
    const uint32_t want = name ? static_cast<uint32_t>(reinterpret_cast<uintptr_t>(name)) : g_namePcText;
    if (*reinterpret_cast<const volatile uint32_t*>(g_namePushImm) == want)
        return;
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(g_namePushImm), 4, PAGE_EXECUTE_READWRITE, &oldProt))
        return;
    *reinterpret_cast<volatile uint32_t*>(g_namePushImm) = want;
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(g_namePushImm), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(g_namePushImm - 1), 5);
    if (g_swapLines++ < 40)
        Log("[ifs] %lu ms: spawn screen while choosing is now \"%s\"%s", ScanMsSinceAttach(),
            name ? name : "ifs_pc_SpawnSelect",
            name ? " (console screen)" : " (the PC screen, as the game has it)");
}
int g_eventLines = 0;
unsigned char* g_stubs = nullptr;
uintptr_t g_pushScreenFn = 0; // 18t: the game's PushScreen / PopScreen (their hooked entries: calls are logged)
uintptr_t g_popScreenFn = 0;

uint32_t ReadU32Safe(uintptr_t va)
{
    uint32_t v = 0;
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), &v, sizeof(v), &got))
        return 0;
    return v;
}

// A printable copy of a C string at 'va' (safe even for a bad pointer).
void ReadName(uintptr_t va, char* out, size_t outSize)
{
    out[0] = 0;
    if (!va)
    {
        snprintf(out, outSize, "(null)");
        return;
    }
    char buf[64] = {};
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), buf, sizeof(buf) - 1, &got))
    {
        snprintf(out, outSize, "(unreadable 0x%08X)", static_cast<unsigned>(va));
        return;
    }
    size_t n = 0;
    while (n < got && buf[n] && n + 1 < outSize)
    {
        out[n] = (buf[n] >= 32 && buf[n] < 127) ? buf[n] : '?';
        ++n;
    }
    out[n] = 0;
}

uintptr_t Manager()
{
    return g_mgrGlobal ? ReadU32Safe(g_mgrGlobal) : 0;
}

void TopName(char* out, size_t outSize)
{
    const uintptr_t m = Manager();
    if (!m)
    {
        snprintf(out, outSize, "(no manager)");
        return;
    }
    ReadName(m + 0x34, out, outSize);
}

uint32_t Caller(const uint32_t* args)
{
    const uintptr_t ra = args[-1];
    return (ra >= ExeBase()) ? static_cast<uint32_t>(ra - ExeBase()) : 0;
}

bool EventBudget()
{
    if (g_eventLines >= kMaxEventLines)
        return false;
    if (++g_eventLines == kMaxEventLines)
        Log("[ifs] %d screen events logged, going quiet", kMaxEventLines);
    return true;
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 0;
    return ReadU32Safe(Rva(b->localPlayerCountRva));
}

// ---- the three loggers (cdecl, called from the stubs on the game thread) ----
void __cdecl LogPush(void* self, const uint32_t* args)
{
    if (!EventBudget())
        return;
    char name[64], top[64];
    ReadName(args[0], name, sizeof(name));
    TopName(top, sizeof(top));
    const uintptr_t m = reinterpret_cast<uintptr_t>(self);
    Log("[ifs] %lu ms: PushScreen \"%s\" (arg2 %ld, arg3 %ld) on manager %d - top was \"%s\", depth %lu, mgr+18Ch %ld, "
        "from rva 0x%08X, %lu local player(s)",
        ScanMsSinceAttach(), name, static_cast<long>(args[1]), static_cast<long>(args[2]), P2SpawnManagerIndex(m), top,
        static_cast<unsigned long>(ReadU32Safe(m + 0x178)), static_cast<long>(ReadU32Safe(m + 0x18C)),
        Caller(args), static_cast<unsigned long>(LocalPlayers()));
}

// ---- 21j-r32: the named pop guard (see the file's head) ----
constexpr uint32_t kMgrEntries = 0xF8; // the stack: entry i at [mgr+0F8h+i*4]
constexpr uint32_t kMgrDepth = 0x178;  // its depth (the current screen is entry [depth])
constexpr uint32_t kEntryName = 8;     // an entry's name: entry+8 (the getter: lea eax,[ecx+8])
constexpr long kStackMax = 32;         // PushScreen refuses at depth 32
constexpr int kPopGuardLines = 80;
bool g_popGuard = false; // the bytes it relies on checked at start
int g_popGuardLines = 0;

bool PopGuardBudget()
{
    if (g_popGuardLines >= kPopGuardLines)
        return false;
    if (++g_popGuardLines == kPopGuardLines)
        Log("[ifs] %d named PopScreen lines logged, going quiet (the guard stays on)", kPopGuardLines);
    return true;
}

// entry i's name; false when the entry is empty (namePtr: where the game keeps the name)
bool StackName(uintptr_t m, long i, char* out, size_t outSize, uintptr_t* namePtr)
{
    const uintptr_t e = ReadU32Safe(m + kMgrEntries + static_cast<uint32_t>(i) * 4);
    if (namePtr)
        *namePtr = e ? e + kEntryName : 0;
    if (!e)
    {
        out[0] = 0;
        return false;
    }
    ReadName(e + kEntryName, out, outSize);
    return true;
}

// the topmost entry above 'stop' named 'want' (as the game's walk would find it); -1: none
long StackFind(uintptr_t m, long depth, long stop, const char* want, uintptr_t* namePtr)
{
    for (long i = depth - 1; i > stop; --i)
    {
        char n[64];
        uintptr_t p = 0;
        if (StackName(m, i, n, sizeof(n), &p) && _stricmp(n, want) == 0)
        {
            *namePtr = p;
            return i;
        }
    }
    return -1;
}

// the stack top down, the current screen first: "9 ifs_meta_movie (up) | 8 ifs_meta_main | ... | 0 -"
void StackList(uintptr_t m, long depth, char* out, size_t outSize)
{
    size_t n = 0;
    out[0] = 0;
    for (long i = depth; i >= 0 && n + 1 < outSize; --i)
    {
        char name[64];
        const bool real = StackName(m, i, name, sizeof(name), nullptr);
        const int w = snprintf(out + n, outSize - n, "%s%ld %s%s", i == depth ? "" : " | ", i, real ? name : "-",
                               i == depth ? " (up)" : "");
        if (w < 0)
            break;
        n += static_cast<size_t>(w);
        if (n >= outSize)
        {
            out[outSize - 1] = 0;
            break;
        }
    }
}

// runs before the game's PopScreen(arg): a name the walk would not find before an empty entry is swapped for one it
// finds - the same name with a 2 / without its last 2, then "ifs_main" - or for 0 (a plain one-screen pop)
void PopNameGuard(uintptr_t m, uint32_t* args)
{
    if (!g_popGuard || !args[0])
        return;
    char want[64];
    ReadName(args[0], want, sizeof(want));
    const long depth = static_cast<long>(ReadU32Safe(m + kMgrDepth));
    if (depth <= 0 || depth > kStackMax)
    {
        if (PopGuardBudget())
            Log("[ifs] %lu ms: PopScreen(\"%s\") on manager %d: depth %ld - nothing to walk (the game's own)",
                ScanMsSinceAttach(), want, P2SpawnManagerIndex(m), depth);
        return;
    }
    long stop = -1;  // the first empty entry from the top: where the game's walk would read address 8
    long found = -1; // the entry the game's walk stops at
    for (long i = depth - 1; i >= 0; --i)
    {
        char n[64];
        if (!StackName(m, i, n, sizeof(n), nullptr))
        {
            stop = i;
            break;
        }
        if (_stricmp(n, want) == 0)
        {
            found = i;
            break;
        }
    }
    char list[600];
    StackList(m, depth, list, sizeof(list));
    if (found >= 0 || stop < 0)
    {
        if (PopGuardBudget())
            Log("[ifs] %lu ms: PopScreen(\"%s\") on manager %d, depth %ld: %s - the game's own; the stack: %s",
                ScanMsSinceAttach(), want, P2SpawnManagerIndex(m), depth,
                found >= 0 ? "on the stack" : "not on the stack, no empty entry (nothing popped)", list);
        return;
    }
    // not above the empty entry: the game would compare entry [stop]'s name - address 8 - and crash
    char cand[3][72];
    int nc = 0;
    snprintf(cand[nc++], sizeof(cand[0]), "%s2", want);
    const size_t len = strlen(want);
    if (len > 1 && want[len - 1] == '2')
    {
        snprintf(cand[nc], sizeof(cand[0]), "%s", want);
        cand[nc++][len - 1] = 0;
    }
    if (_stricmp(want, "ifs_main") != 0)
        snprintf(cand[nc++], sizeof(cand[0]), "%s", "ifs_main");
    for (int c = 0; c < nc; ++c)
    {
        uintptr_t p = 0;
        const long at = StackFind(m, depth, stop, cand[c], &p);
        if (at < 0)
            continue;
        args[0] = static_cast<uint32_t>(p);
        Log("[ifs] %lu ms: PopScreen(\"%s\") on manager %d, depth %ld: NOT on the stack above its empty entry %ld (the "
            "game would read address 8 and crash) - popped to \"%s\" (entry %ld) instead; the stack: %s",
            ScanMsSinceAttach(), want, P2SpawnManagerIndex(m), depth, stop, cand[c], at, list);
        return;
    }
    args[0] = 0;
    Log("[ifs] %lu ms: PopScreen(\"%s\") on manager %d, depth %ld: NOT on the stack above its empty entry %ld (the game "
        "would read address 8 and crash), nor %s%s%s%s%s - a plain PopScreen() instead (one screen back); the stack: %s",
        ScanMsSinceAttach(), want, P2SpawnManagerIndex(m), depth, stop, cand[0], nc > 1 ? " / " : "",
        nc > 1 ? cand[1] : "", nc > 2 ? " / " : "", nc > 2 ? cand[2] : "", list);
}

void __cdecl LogPop(void* self, uint32_t* args)
{
    PopNameGuard(reinterpret_cast<uintptr_t>(self), args); // 21j-r32: first, so the line below shows what the game gets
    if (!EventBudget())
        return;
    char top[64];
    TopName(top, sizeof(top));
    const uintptr_t m = reinterpret_cast<uintptr_t>(self);
    Log("[ifs] %lu ms: PopScreen(%ld) on manager %d - top was \"%s\", depth %lu, from rva 0x%08X", ScanMsSinceAttach(),
        static_cast<long>(args[0]), P2SpawnManagerIndex(m), top, static_cast<unsigned long>(ReadU32Safe(m + 0x178)),
        Caller(args));
}

// 19i: what the chooser's mode-1 body does when the screen it builds is not the top (and the top is not
// the pause menu): PopScreen(0) if a screen is up, PushScreen(name, 0, 0) - made here first, and settled,
// so the screen is switched before ClassWheelFrame sets it up and before the frame is drawn
typedef void(__fastcall* ChooserPush_t)(void* mgr, void* unusedEdx, const char* name, int player, int arg3);
typedef void(__fastcall* ChooserPop_t)(void* mgr, void* unusedEdx, int arg);
unsigned g_switchLines = 0;

void SwitchNow(uintptr_t obj, const char* want)
{
    // only when the switch can be settled here (else the chooser's own queued switch stands, as before) and
    // the chooser's name is swapped (else its body would push the PC screen over ours every frame)
    if (!want || !g_pushScreenFn || !g_popScreenFn || !g_namePushImm || !P2SpawnSettleReady() || ReadU32Safe(obj + 0x18) != 1)
        return;
    const uintptr_t m = Manager();
    if (!m)
        return;
    char top[64];
    ReadName(m + 0x34, top, sizeof(top));
    if (strcmp(top, want) == 0 || strcmp(top, "ifs_pausemenu") == 0)
        return;
    if (top[0])
        reinterpret_cast<ChooserPop_t>(g_popScreenFn)(reinterpret_cast<void*>(m), nullptr, 0);
    reinterpret_cast<ChooserPush_t>(g_pushScreenFn)(reinterpret_cast<void*>(m), nullptr, want, 0, 0);
    const bool settled = P2SpawnSettleManager(m);
    char now[64];
    ReadName(m + 0x34, now, sizeof(now));
    if (g_switchLines++ < 40)
        Log("[ifs] %lu ms: the chooser's switch to \"%s\" made here (top was \"%s\")%s: top now \"%s\", depth %lu",
            ScanMsSinceAttach(), want, top, settled ? " and settled" : " (the IF update settles it)", now,
            static_cast<unsigned long>(ReadU32Safe(m + 0x178)));
}

struct ChooserState
{
    uintptr_t obj;
    uint32_t mode;
    uint32_t player;
    char top[64];
};
ChooserState g_lastChoice[4];
int g_choiceCount = 0;
unsigned g_chooserCalls = 0;

void __cdecl LogChooser(void* self, const uint32_t* args)
{
    const FramePerfScope perf(kPerfOurChooser); // 21j-r11: the frame-rate scan
    ++g_chooserCalls;
    const uintptr_t obj = reinterpret_cast<uintptr_t>(self);
    // runs on the game thread, right before the chooser builds the screen name
    const uint32_t players = LocalPlayers();
    const char* want = nullptr;
    if (players < 2 || ClassWheelSplitWanted())
        want = ClassWheelPreviewScreen(); // 18d: the F6 preview (null = the PC screen); 18w: split screen too
    else if (g_consoleSpawn)
        want = kConsoleScreen; // 13c: split screen
    SetChooserScreen(want);
    SwitchNow(obj, want);       // 19i: the chooser's pop / push and the switch itself, before the setup
    ClassWheelFrame(obj, want); // 18e: the console driver (game thread)
    const uint32_t mode = ReadU32Safe(obj + 0x18);
    const uint32_t player = ReadU32Safe(obj + 0x238C);
    char top[64];
    TopName(top, sizeof(top));
    int slot = -1;
    for (int i = 0; i < g_choiceCount; ++i)
    {
        if (g_lastChoice[i].obj == obj)
        {
            slot = i;
            break;
        }
    }
    if (slot >= 0 && g_lastChoice[slot].mode == mode && g_lastChoice[slot].player == player &&
        strcmp(g_lastChoice[slot].top, top) == 0)
        return; // unchanged - log changes only
    if (slot < 0)
    {
        if (g_choiceCount >= 4)
            slot = 3;
        else
            slot = g_choiceCount++;
    }
    g_lastChoice[slot].obj = obj;
    g_lastChoice[slot].mode = mode;
    g_lastChoice[slot].player = player;
    snprintf(g_lastChoice[slot].top, sizeof(g_lastChoice[slot].top), "%s", top);
    if (!EventBudget())
        return;
    Log("[ifs] %lu ms: spawn-screen chooser for object 0x%08X (arg %ld): mode %lu (%s), obj+238Ch %lu, top screen "
        "\"%s\", %lu local player(s), call #%u",
        ScanMsSinceAttach(), static_cast<unsigned>(obj), static_cast<long>(args[0]),
        static_cast<unsigned long>(mode),
        mode == 1 ? "choosing" : mode == 2 ? "Spawn pressed" : "other",
        static_cast<unsigned long>(player), top, static_cast<unsigned long>(LocalPlayers()), g_chooserCalls);
}

// ---- 13f: the menu layer, drawn in player 1's box while a menu is up --------
void __fastcall DrawMenuLayer(void* root, void* unusedEdx)
{
    const FramePerfScope perf(kPerfOurMenu); // 21j-r11: the frame-rate scan (the game's draw below is its own part)
    MenuViewportSave save;
    bool moved = false;
    P2SpawnMenuLayer(reinterpret_cast<uintptr_t>(root)); // 18s: log-only (the viewport as the game left it, the root's tree)
    bool p2 = false;
    const bool endFull = P2SpawnEndStatsFull(); // 21h-r8: the end screens over the whole screen
    if (endFull && g_menuFit)
        moved = MenuViewportBeginFull(g_menuFit, &save); // 21h-r8
    if (g_menuFit && LocalPlayers() >= 2 && !endFull)
    {
        // 21h-r3: -1 unless player 2's pause (3 / 4 players: 1; 21j-r15: the 2-player split with PauseLikeP1P2: 2)
        const int owner = P2SpawnMenuOwner();
        if (owner == 1)
            p2 = moved = MenuViewportBeginP2(g_menuFit, &save); // 21h-r3: player 2's popups / option screens in his quarter
        else if (owner == 2)
            p2 = moved = MenuViewportBeginOwn(g_menuFit, &save); // 21j-r15: in a 16:9 box of his own half, as player 1's
        if (!moved && owner != 1 && owner != 2)
        {
            const uintptr_t m = Manager();
            if (m && ReadU32Safe(m + 0x178) > 0) // player 1's menu stack is not empty
                moved = P2SpawnMenuP1Quarter() ? MenuViewportBeginRegion(g_menuFit, 0.0f, 0.0f, 0.5f, 0.5f, &save) // 21j-r15
                                               : MenuViewportBegin(g_menuFit, false, &save);
        }
    }
    {
        const FramePerfScope perfDraw(kPerfMenuDraw); // 21j-r11
        g_drawTree(root, unusedEdx);
    }
    if (moved)
        MenuViewportEnd(&save);
    if ((moved != g_menuWasMoved || p2 != g_menuWasP2) && g_menuFitLines < 40)
    {
        ++g_menuFitLines;
        char top[64];
        TopName(top, sizeof(top));
        if (moved)
            Log("[menufit] %lu ms: menu \"%s\" drawn in %s box x %lu y %lu w %lu h %lu (was the view's viewport x %lu y %lu "
                "w %lu h %lu)",
                ScanMsSinceAttach(), top,
                endFull ? "THE WHOLE SCREEN'S (21h-r8: the end screens)"
                        : p2 ? "PLAYER 2'S (21h-r3 / 21j-r15: his popups / option screens)"
                             : (P2SpawnMenuP1Quarter() ? "player 1's (21j-r15: his 3-player quarter)" : "player 1's"),
                save.boxX, save.boxY, save.boxW, save.boxH, save.x, save.y, save.w, save.h);
        else
            Log("[menufit] %lu ms: %s - the menu layer is drawn as the game has it", ScanMsSinceAttach(),
                endFull ? "21h-r8: the end screens (stats / awards) - over the whole screen" : "no menu up (or not split screen)");
    }
    g_menuWasMoved = moved;
    g_menuWasP2 = p2;
}

// 13g: each view's HUD layer; only player 1's, only while the PC spawn screen is up
void __fastcall DrawHudLayer(void* root, void* unusedEdx)
{
    const FramePerfScope perf(kPerfOurMenu); // 21j-r11: the frame-rate scan (the game's draw below is its own part)
    MenuViewportSave save;
    bool moved = false;
    if (g_menuFit && g_spawnHudFit && LocalPlayers() >= 2)
    {
        char top[64];
        TopName(top, sizeof(top));
        if (strcmp(top, "ifs_pc_SpawnSelect") == 0)
            moved = MenuViewportBegin(g_menuFit, true, &save); // only if this is player 1's own viewport
    }
    {
        const FramePerfScope perfDraw(kPerfHudDraw); // 21j-r11
        g_drawTree(root, unusedEdx);
    }
    if (moved)
        MenuViewportEnd(&save);
    if (moved && !g_hudWasMoved && g_hudFitLines < 8)
    {
        ++g_hudFitLines;
        Log("[menufit] %lu ms: player 1's HUD layer (spawn screen map) drawn in the same box x %lu y %lu w %lu h "
            "%lu",
            ScanMsSinceAttach(), save.boxX, save.boxY, save.boxW, save.boxH);
    }
    if (moved)
        g_hudWasMoved = true;
}

bool RedirectCall(const unsigned char* call, void* to)
{
    const int32_t rel =
        static_cast<int32_t>(reinterpret_cast<uintptr_t>(to) - (reinterpret_cast<uintptr_t>(call) + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(const_cast<unsigned char*>(call + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(const_cast<unsigned char*>(call + 1), &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(const_cast<unsigned char*>(call + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    return true;
}

bool InstallMenuFit(const BuildInfo* build)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kMenuLayer, hits, 4);
    if (n != 1)
    {
        Log("[menufit] %d menu-layer pattern hits (need 1) - NOT installed", n);
        return false;
    }
    const uintptr_t a = Rva(hits[0]);
    const unsigned char* menu = reinterpret_cast<const unsigned char*>(a + kMenuCall);
    const unsigned char* hud = reinterpret_cast<const unsigned char*>(a + kHudCall);
    const unsigned char* on = reinterpret_cast<const unsigned char*>(a + kTwoDOn);
    const unsigned char* off = reinterpret_cast<const unsigned char*>(a + kTwoDOff);
    if (menu[0] != 0xE8 || hud[0] != 0xE8 || on[0] != 0xE8 || off[0] != 0xE8)
    {
        Log("[menufit] the calls around the menu layer are not where expected - NOT installed");
        return false;
    }
    auto target = [](const unsigned char* call) -> uintptr_t {
        int32_t rel = 0;
        memcpy(&rel, call + 1, 4);
        return reinterpret_cast<uintptr_t>(call) + 5 + rel;
    };
    const uintptr_t drawTree = target(menu);
    if (drawTree != target(hud) || target(on) != target(off))
    {
        Log("[menufit] the menu layer's call does not match the HUD's (0x%08X vs 0x%08X) - NOT installed",
            static_cast<unsigned>(drawTree - ExeBase()), static_cast<unsigned>(target(hud) - ExeBase()));
        return false;
    }
    if (drawTree < ExeBase() || drawTree >= ExeBase() + build->sizeOfImage)
    {
        Log("[menufit] the menu layer's call leaves the exe - NOT installed");
        return false;
    }
    g_drawTree = reinterpret_cast<DrawTree_t>(drawTree);
    if (!RedirectCall(menu, reinterpret_cast<void*>(&DrawMenuLayer)))
        return false;
    Log("[menufit] menu layer call at rva 0x%08X (draws the interface root with rva 0x%08X, as each HUD does) "
        "now goes through BF1GameExt",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(menu) - ExeBase()),
        static_cast<unsigned>(drawTree - ExeBase()));
    if (g_spawnHudFit)
    {
        if (RedirectCall(hud, reinterpret_cast<void*>(&DrawHudLayer)))
            Log("[menufit] each view's HUD layer call at rva 0x%08X now goes through BF1GameExt (player 1's "
                "only moves, and only while the PC spawn screen is up)",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(hud) - ExeBase()));
    }
    return true;
}

// ---- the hook stub: save ecx, call logger(this, &args), restore, prologue, back
bool HookEntry(uintptr_t fn, unsigned char* stub, void* logger)
{
    unsigned char code[27];
    code[0] = 0x51;                                   // push ecx
    code[1] = 0x8D;                                   // lea eax, [esp+8]  (first stack argument)
    code[2] = 0x44;
    code[3] = 0x24;
    code[4] = 0x08;
    code[5] = 0x50;                                   // push eax
    code[6] = 0x51;                                   // push ecx
    code[7] = 0xE8;                                   // call logger
    const int32_t toLogger =
        static_cast<int32_t>(reinterpret_cast<uintptr_t>(logger) - (reinterpret_cast<uintptr_t>(stub) + 12));
    memcpy(code + 8, &toLogger, 4);
    code[12] = 0x83;                                  // add esp, 8
    code[13] = 0xC4;
    code[14] = 0x08;
    code[15] = 0x59;                                  // pop ecx
    memcpy(code + 16, reinterpret_cast<const void*>(fn), kPrologue); // the function's own first 6 bytes
    code[22] = 0xE9;                                  // jmp fn + 6
    const int32_t back =
        static_cast<int32_t>((fn + kPrologue) - (reinterpret_cast<uintptr_t>(stub) + sizeof(code)));
    memcpy(code + 23, &back, 4);
    memcpy(stub, code, sizeof(code));
    FlushInstructionCache(GetCurrentProcess(), stub, sizeof(code));

    unsigned char jmp[6];
    jmp[0] = 0xE9;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(stub) - (fn + 5));
    memcpy(jmp + 1, &rel, 4);
    jmp[5] = 0x90;
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(fn), sizeof(jmp), PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(reinterpret_cast<void*>(fn), jmp, sizeof(jmp));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(fn), sizeof(jmp), oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(fn), sizeof(jmp));
    return true;
}

// ---- read-only search of the game's memory for the menu scripts ------------
struct Needle
{
    const char* text;
    int hits;
};

void LogContext(const unsigned char* buf, size_t len, size_t at, const char* what)
{
    // the printable strings around the hit (Lua bytecode keeps names as plain text)
    const size_t from = at > 160 ? at - 160 : 0;
    const size_t to = (at + 200 < len) ? at + 200 : len;
    char line[420];
    size_t n = 0;
    size_t run = 0;
    for (size_t i = from; i < to && n + 2 < sizeof(line); ++i)
    {
        const unsigned char c = buf[i];
        if (c >= 32 && c < 127)
        {
            line[n++] = static_cast<char>(c);
            ++run;
        }
        else if (run)
        {
            line[n++] = '|';
            run = 0;
        }
    }
    line[n] = 0;
    Log("[ifs]     near \"%s\": %s", what, line);
}

void SearchMemory()
{
    Needle needles[] = { { "ifs_readyselect", 0 }, { "ifs_pc_SpawnSelect", 0 }, { "ifs_pausemenu2", 0 },
                         { "SetIFScreenViewport", 0 }, { "GetPausingViewport", 0 } };
    const int kNeedles = static_cast<int>(sizeof(needles) / sizeof(needles[0]));
    const BuildInfo* b = CurrentBuild();
    const uintptr_t exeLo = ExeBase();
    const uintptr_t exeHi = ExeBase() + (b ? b->sizeOfImage : 0);
    const size_t kChunk = 1 << 20;
    unsigned char* buf = static_cast<unsigned char*>(VirtualAlloc(nullptr, kChunk + 64, MEM_COMMIT, PAGE_READWRITE));
    if (!buf)
        return;
    uintptr_t addr = 0x10000;
    MEMORY_BASIC_INFORMATION mbi;
    unsigned long scannedMb = 0;
    SYSTEM_INFO si; // 22e: to the top of the address space (4 GB with the exe's 4 GB flag), not a fixed 2 GB
    GetSystemInfo(&si);
    const uintptr_t top = reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress);
    while (addr < top && VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)))
    {
        const uintptr_t regionLo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        const uintptr_t regionHi = regionLo + mbi.RegionSize;
        const DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
                               PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
        const bool skip = mbi.State != MEM_COMMIT || !(mbi.Protect & readable) || (mbi.Protect & PAGE_GUARD) ||
                          (regionLo < exeHi && regionHi > exeLo) || reinterpret_cast<unsigned char*>(regionLo) == buf;
        if (!skip)
        {
            for (uintptr_t p = regionLo; p < regionHi; p += kChunk)
            {
                const size_t want = (regionHi - p < kChunk + 32) ? static_cast<size_t>(regionHi - p) : kChunk + 32;
                SIZE_T got = 0;
                if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(p), buf, want, &got) || !got)
                    continue;
                scannedMb += static_cast<unsigned long>(got >> 20);
                for (int k = 0; k < kNeedles; ++k)
                {
                    const size_t nl = strlen(needles[k].text);
                    for (size_t i = 0; i + nl <= got && i < kChunk; ++i)
                    {
                        if (buf[i] == static_cast<unsigned char>(needles[k].text[0]) &&
                            memcmp(buf + i, needles[k].text, nl) == 0)
                        {
                            if (needles[k].hits < 3)
                            {
                                Log("[ifs]   \"%s\" found at 0x%08X", needles[k].text,
                                    static_cast<unsigned>(p + i));
                                LogContext(buf, got, i, needles[k].text);
                            }
                            ++needles[k].hits;
                        }
                    }
                }
            }
        }
        if (regionHi <= addr)
            break; // 22e: the last region (no wrap past 4 GB)
        addr = regionHi;
    }
    VirtualFree(buf, 0, MEM_RELEASE);
    Log("[ifs] memory search done (%lu MB outside the exe): ifs_readyselect %d, ifs_pc_SpawnSelect %d, "
        "ifs_pausemenu2 %d, SetIFScreenViewport %d, GetPausingViewport %d",
        scannedMb, needles[0].hits, needles[1].hits, needles[2].hits, needles[3].hits, needles[4].hits);
}

// ---- background: menu stack snapshot in split screen + one memory search ----
DWORD WINAPI WatchThread(LPVOID)
{
    char lastTop[64] = "";
    uint32_t lastDepth = 0xFFFFFFFF;
    uint32_t last18c = 0xFFFFFFFF;
    DWORD splitSince = 0;
    bool searched = false;
    int snapshots = 0;
    // 13d: capture a frame of the PC spawn screen in split screen by itself
    // (1.5 s after it comes up, at most twice) - finds the code that draws menus
    DWORD spawnScreenSince = 0;
    int spawnCaptures = 0;
    for (;;)
    {
        Sleep(500);
        const bool split = LocalPlayers() >= 2;
        if (split && !splitSince)
            splitSince = GetTickCount();
        if (!split)
            splitSince = 0;
        const uintptr_t m = Manager();
        if (m && snapshots < 120)
        {
            char top[64];
            TopName(top, sizeof(top));
            const uint32_t depth = ReadU32Safe(m + 0x178);
            const uint32_t v18c = ReadU32Safe(m + 0x18C);
            if (strcmp(top, lastTop) != 0 || depth != lastDepth || v18c != last18c)
            {
                ++snapshots;
                Log("[ifs] %lu ms: menu stack now: top \"%s\", depth %lu, mgr+18Ch %ld, %lu local player(s)",
                    ScanMsSinceAttach(), top, static_cast<unsigned long>(depth), static_cast<long>(v18c),
                    static_cast<unsigned long>(LocalPlayers()));
                snprintf(lastTop, sizeof(lastTop), "%s", top);
                lastDepth = depth;
                last18c = v18c;
            }
        }
        if (m && split && spawnCaptures < 2)
        {
            char top[64];
            TopName(top, sizeof(top));
            if (strcmp(top, "ifs_pc_SpawnSelect") == 0)
            {
                if (!spawnScreenSince)
                    spawnScreenSince = GetTickCount();
                else if (spawnScreenSince != 1 && GetTickCount() - spawnScreenSince >= 1500)
                {
                    ++spawnCaptures;
                    spawnScreenSince = 1; // once per appearance
                    Log("[ifs] %lu ms: the PC spawn screen is up in split screen - capturing a frame of it (%d of 2)",
                        ScanMsSinceAttach(), spawnCaptures);
                    FrameCaptureArm();
                }
            }
            else
                spawnScreenSince = 0;
        }
        // once, 20 s into split screen (the match's menu scripts are loaded by then)
        if (!searched && splitSince && GetTickCount() - splitSince > 20000)
        {
            searched = true;
            Log("[ifs] %lu ms: searching the game's memory for the console spawn screen scripts (read-only)...",
                ScanMsSinceAttach());
            SearchMemory();
        }
    }
}

} // namespace

uintptr_t SpawnUiPushScreenFn()
{
    return g_pushScreenFn;
}

uintptr_t SpawnUiPopScreenFn()
{
    return g_popScreenFn;
}

void SpawnUiScanStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[ifs] not used on this build");
        return;
    }
    if (!onGameThread)
    {
        Log("[ifs] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }
    uint32_t push[4] = {}, pop[4] = {}, chooser[4] = {};
    const int np = FindPattern(kPushScreen, push, 4);
    const int nq = FindPattern(kPopScreen, pop, 4);
    const int nc = FindPattern(kChooser, chooser, 4);
    if (np != 1 || nq != 1 || nc != 1)
    {
        Log("[ifs] PushScreen %d / PopScreen %d / chooser %d pattern hits (need 1 each) - NOT installed", np, nq, nc);
        return;
    }
    const uint32_t mgr = ReadU32Safe(Rva(chooser[0]) + kChooserMgrOperand);
    if (mgr < ExeBase() || mgr >= ExeBase() + build->sizeOfImage)
    {
        Log("[ifs] the IF manager pointer is outside the exe - NOT installed");
        return;
    }
    g_mgrGlobal = mgr;
    {
        char exe[MAX_PATH] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
            {
                *slash = 0;
                char ini[MAX_PATH + 32];
                snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
                g_consoleSpawn = GetPrivateProfileIntA("Display", "SplitConsoleSpawn", 0, ini) != 0;
                g_menuFit = static_cast<int>(GetPrivateProfileIntA("Display", "SplitMenuFit", 1, ini));
                g_spawnHudFit = GetPrivateProfileIntA("Display", "SplitSpawnMapFit", 1, ini) != 0;
                if (g_menuFit < 0 || g_menuFit > 2)
                    g_menuFit = 1;
            }
        }
        Log("[menufit] BF1GameExt.ini [Display] SplitMenuFit=%d%s", g_menuFit,
            g_menuFit == 1   ? ": in split screen, menus are drawn in a 16:9 box in player 1's half"
            : g_menuFit == 2 ? ": in split screen, menus fill player 1's half (squeezed)"
                             : ": menus are drawn where the game has them");
        Log("[menufit] BF1GameExt.ini [Display] SplitSpawnMapFit=%d%s", g_spawnHudFit ? 1 : 0,
            g_spawnHudFit ? ": while player 1's spawn screen is up, player 1's HUD layer (map) uses the same box"
                          : ": player 1's HUD layer stays as the game has it");
        Log("[ifs] BF1GameExt.ini [Display] SplitConsoleSpawn=%d%s", g_consoleSpawn ? 1 : 0,
            g_consoleSpawn ? ": in split screen player 1 chooses on the console spawn screen (ifs_readyselect1)"
                           : ": the PC spawn screen as the game has it");
    }
    {
        // the one 'push imm32' of the text "ifs_pc_SpawnSelect" inside the chooser (sprintf's format)
        const uintptr_t c = Rva(chooser[0]);
        int found = 0;
        for (uint32_t off = 0; off + 5 <= 0x240; ++off)
        {
            const unsigned char* b = reinterpret_cast<const unsigned char*>(c + off);
            if (b[0] != 0x68)
                continue;
            uint32_t v = 0;
            memcpy(&v, b + 1, 4);
            if (v < ExeBase() || v >= ExeBase() + build->sizeOfImage)
                continue;
            char text[64];
            ReadName(v, text, sizeof(text));
            if (strcmp(text, "ifs_pc_SpawnSelect") == 0)
            {
                if (!found)
                {
                    g_namePushImm = c + off + 1;
                    g_namePcText = v;
                }
                ++found;
            }
        }
        if (found != 1)
        {
            g_namePushImm = 0;
            Log("[ifs] %d pushes of \"ifs_pc_SpawnSelect\" in the chooser (need 1) - console spawn screen NOT used",
                found);
        }
        else
            Log("[ifs] chooser's spawn screen name at rva 0x%08X (push of \"ifs_pc_SpawnSelect\")",
                static_cast<unsigned>(g_namePushImm - 1 - ExeBase()));
    }
    g_stubs = static_cast<unsigned char*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE,
                                                        PAGE_EXECUTE_READWRITE));
    if (!g_stubs)
    {
        Log("[ifs] VirtualAlloc failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    {
        // 21j-r32: the named pop guard's bytes - the game's PopScreen(name) walk (GOG 1.3 0x005347D0; the 2020 Steam exe
        // has the same function, pattern kPopScreen): offsets from its entry, checked before the hook goes in
        const unsigned char* b = reinterpret_cast<const unsigned char*>(Rva(pop[0]));
        static const unsigned char kTopEntry[] = {0x8B, 0x8C, 0xB3, 0xF8, 0x00, 0x00, 0x00};     // +22h mov ecx,[ebx+esi*4+0F8h]
        static const unsigned char kWalkStart[] = {0x8D, 0xBB, 0xF8, 0x00, 0x00, 0x00, 0x8D, 0x3C, 0xB7}; // +63h lea edi, ...
        static const unsigned char kWalkName[] = {0x8B, 0x0F, 0xE8};                             // +70h mov ecx,[edi]; call
        static const unsigned char kWalkCmp[] = {0xFF, 0x75, 0x08, 0x50, 0xE8};                  // +77h push arg; push eax; call
        static const unsigned char kWalkNext[] = {0x83, 0xEF, 0x04, 0x83, 0xEE, 0x01, 0x79, 0xE1}; // +87h the next one down
        static const unsigned char kGetName[] = {0x8D, 0x41, 0x08, 0xC3};                        // lea eax,[ecx+8]; ret
        const bool walkOk = memcmp(b + 0x22, kTopEntry, sizeof(kTopEntry)) == 0 &&
                            memcmp(b + 0x63, kWalkStart, sizeof(kWalkStart)) == 0 &&
                            memcmp(b + 0x70, kWalkName, sizeof(kWalkName)) == 0 &&
                            memcmp(b + 0x77, kWalkCmp, sizeof(kWalkCmp)) == 0 &&
                            memcmp(b + 0x87, kWalkNext, sizeof(kWalkNext)) == 0;
        bool getterOk = false;
        uint32_t getterRva = 0;
        if (walkOk)
        {
            int32_t rel = 0;
            memcpy(&rel, b + 0x73, 4);
            const unsigned char* g = b + 0x77 + rel; // the name getter (or its jump thunk)
            if (g[0] == 0xE9)
            {
                memcpy(&rel, g + 1, 4);
                g = g + 5 + rel;
            }
            getterRva = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g) - ExeBase());
            getterOk = memcmp(g, kGetName, sizeof(kGetName)) == 0;
        }
        g_popGuard = walkOk && getterOk;
        Log("[ifs] 21j-r32: named PopScreen guard %s - the stack walk (rva 0x%08X +22h / +63h / +70h / +77h / +87h) %s, "
            "the name getter (rva 0x%08X: lea eax,[ecx+8]) %s; a name the walk would not find before the stack's empty "
            "bottom entry (where the game read address 8) goes to the same name with a 2 / without its last 2, then "
            "ifs_main, else a plain pop; every named pop logs the stack",
            g_popGuard ? "ON" : "OFF", pop[0], walkOk ? "OK" : "NOT as GOG 1.3 has it", getterRva,
            getterOk ? "OK" : (walkOk ? "NOT as GOG 1.3 has it" : "not checked"));
    }
    const bool ok1 = HookEntry(Rva(push[0]), g_stubs, reinterpret_cast<void*>(&LogPush));
    const bool ok2 = HookEntry(Rva(pop[0]), g_stubs + 0x40, reinterpret_cast<void*>(&LogPop));
    const bool ok3 = HookEntry(Rva(chooser[0]), g_stubs + 0x80, reinterpret_cast<void*>(&LogChooser));
    if (ok1 && ok2)
    {
        g_pushScreenFn = Rva(push[0]); // 18t
        g_popScreenFn = Rva(pop[0]);
    }
    Log("[ifs] PushScreen rva 0x%08X %s, PopScreen rva 0x%08X %s, spawn-screen chooser rva 0x%08X %s; IF manager "
        "pointer rva 0x%08X - logging every menu screen change (log-only, the named pop guard aside)",
        push[0], ok1 ? "hooked" : "FAILED", pop[0], ok2 ? "hooked" : "FAILED", chooser[0],
        ok3 ? "hooked" : "FAILED", static_cast<unsigned>(mgr - ExeBase()));
    if (g_menuFit)
        InstallMenuFit(build);
    const HANDLE t = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
}

} // namespace bf1
