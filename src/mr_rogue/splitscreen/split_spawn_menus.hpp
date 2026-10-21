// BF1GameExt - split_spawn_menus.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was p2spawn_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r25: P2SpawnPauseP1 (player 1's pause menu as the game's Start opens it - menus.cpp's free camera).
// 21j-r15: P2SpawnMenuOwner answers 2 (the 2-player split: player 2's own half, [Splitscreen] PauseLikeP1P2); P2SpawnMenuP1Quarter
// (the 3-player layout: player 1's menu layer in his quarter, [Splitscreen] MenuBoxP1); P2SpawnMatchEnded (the end of a match,
// for players34.cpp's end shots); P2SpawnP2PauseLike (the 2-player script's switch, for menus.cpp).
// 21i-r1: P2SpawnMissionName, P2SpawnGameLocalPlayers (the bot / reinforcement scan).
// 21h-r11: P2SpawnStatsPauseOn - player 1's pause menu on the stats screen is on (menus.cpp hides its RESPAWN).
// 21h-r8: P2SpawnEndStatsFull - the end screens over the whole screen (spawn_ui.cpp).
// 21h-r3: P2SpawnMenuOwner - whose box the menu layer (popups, option screens) goes in (spawn_ui.cpp).
// 21h-r2: P2SpawnRealPauseReady / P2SpawnRealPauseAsk - players 3 / 4's real pause menus (ifs_pausemenu3 / 4, made by
// menus.cpp's BF1GameExt_pause34.lua run; what they ask through "BFXQ: rp ...").
// 20g-r12: P2SpawnRootGeom / P2SpawnGroupPlace / P2SpawnRootsHorizontal / P2SpawnHorizontalRoots (horizontal 2-player, for
// class_wheel.cpp).
// 20g-r10: P2SpawnScreensUp (players 2 - 4's camera shots come with their spawn screens).
// 20g-r6: P2SpawnP34MapFrame (players 3 and 4's maps on their spawn point screens).
// 20g-r5: P2SpawnP34Teardown.
// 20f-r2: P2SpawnP34Frame / P2SpawnP34Owns / P2SpawnP34Root - players 3 and 4's spawn screens (their own groups in
// their views' HUD roots), for players34.cpp.
// 19i: P2SpawnSettleManager (the IF manager's tick, for a screen switch made at once).
// Phase 18u: PLAYER 2 SPAWN SCREENS + MENU INPUT FIX (2020 build: GOG 1.3 / Steam). [p2spawn] lines.
// (1) The menu input fix: the interface's input step keeps each player's held buttons in a table with
// ONE 1Ah-byte slot on PC (the Xbox's has 4); player 2's slot ran over the Lua's current-manager
// index and the object after it, so his pause menu took no input. The step's 25 references to the
// table are moved to a 4-slot table in BF1GameExt (by pattern, each one checked, read back).
// (2) Player 2's own console spawn screens in split screen, by themselves: ifs_readyselect2 (side) ->
// ifs_mapselect2 (spawn point) -> ifs_charselect2 (class) on IF manager 1, with the game's own
// PushScreen / PopScreen (as it pushes player 2's pause menu), drawn by the game in player 2's half,
// driven by pad 2; A on the class screen spawns him through the game's own Spawn button calls.
// The 18s / 18t scan and the 30-frame screen dumps stay; the F5 preview is gone.
#pragma once

#include <cstddef>
#include <cstdint>

namespace bf1 {

// Finds everything by pattern (logged, each checked), applies the menu input fix (on the game
// thread: now if onGameThread, else on the first menu layer frame) and starts the watcher thread.
// Call after SpawnUiScanStart (it uses its PushScreen / PopScreen) and ClassWheelScanStart (texts,
// sounds). Does nothing on GOG 1.2.
void P2SpawnScanStart(bool onGameThread);

// The IF manager index (0 = player 1's, 1 = player 2's) of a manager object, or -1 if it is not
// one of the game's two managers (or they were not found). For the PushScreen / PopScreen log lines.
int P2SpawnManagerIndex(uintptr_t manager);

// Called by spawn_ui.cpp's menu layer hook every frame (game thread, before the game draws the
// IF root): player 2's spawn screens, the screen / root logs, the viewport line.
void P2SpawnMenuLayer(uintptr_t root);

// 18u: true while player 2's spawn screens look after his spawning (split screen, in a match, the
// screens loaded, [Splitscreen] P2SpawnScreen=1, everything found) - autospawn.cpp stands aside.
bool P2SpawnFlowOwnsSpawn();

// 19i: runs the IF manager's queued screen events now (its tick with dt 0: a PushScreen / PopScreen just
// made takes effect before the frame is drawn). False if the tick was not found or
// [Splitscreen] SpawnScreenSwitchNow=0 - the IF update then switches next frame, as before.
bool P2SpawnSettleManager(uintptr_t manager);
bool P2SpawnSettleReady(); // true if P2SpawnSettleManager would do it

// 20f-r2: players 3 and 4's spawn screens. Frame: every frame on the game thread before the views are drawn
// (players34.cpp's frame hook). Owns(k): his screens look after player k + 3's spawning (his pad connected, his
// group made) - the auto-spawn stands aside. Root(k): the root index (= renderer view) his shown screen is in, -1 while
// none is shown - that view's HUD flag is on while it is shown. Nothing happens with [Splitscreen] SpawnP34=0.
void P2SpawnP34Frame();
// 20g-r5: the match teardown (while the match is still loaded): players 3 and 4's spawn screens, rings and groups
// deleted. Game thread only.
void P2SpawnP34Teardown();
bool P2SpawnP34Owns(int k);
// 21h-r8: the end screens (ifs_teamstats / ifs_awardstats) are to be drawn over the whole screen ([Splitscreen]
// EndStatsFullScreen) - spawn_ui.cpp's menu layer
bool P2SpawnEndStatsFull();
int P2SpawnP34Root(int k);
// 20g-r6: players 3 and 4's own minimaps in their spawn screen look while their spawn point screens are up (and as
// they were after). players34.cpp's HudFrame calls it each frame after it has shown / hidden their HUD. Game thread.
void P2SpawnP34MapFrame();
// 20g-r10: player p's spawn screens (p 1 = player 2, 2 / 3 = players 3 / 4): 1 while his side, spawn point or class
// screen is up, 0 while not, -1 when BF1GameExt's spawn screens do not look after him. Game thread.
int P2SpawnScreensUp(int p);
// 20g-r12: player p's (0 / 1) HUD root: its size (units) and safe area (offset, scale) as the root draws with them - false
// if it is not known or not usable. Game thread.
bool P2SpawnRootGeom(int p, float* w, float* h, float* offX, float* offY, float* sx, float* sy);
// 20g-r12: the place (x, y) in `group` (a child's) that draws at (fx, fy) of its HUD root's view (fractions): the group's
// root anchor through the root's safe area when its flag is set, its own place, the root's size - false if the group is
// not in one of the HUD roots. Game thread.
bool P2SpawnGroupPlace(uintptr_t group, float fx, float fy, float* x, float* y);
// 20g-r12: both players' HUD roots are known, apart, and horizontal halves (at least twice as wide as tall)
bool P2SpawnRootsHorizontal();
// 20g-r12: horizontal 2-player ([Splitscreen] HorizontalSafeArea=1): both halves' HUD roots get the Classic Collection's
// safe area now (the menu layer does it each frame; class_wheel.cpp calls it before player 1's screens place anything,
// which runs earlier in the frame). Game thread.
void P2SpawnHorizontalRoots();
// 21h-r2: players 3 / 4's real pause menus are installed ([Splitscreen] RealPauseP34=1, the managers built, the thunks moved):
// menus.cpp makes ifs_pausemenu3 / 4 (BF1GameExt_pause34.lua) only then.
bool P2SpawnRealPauseReady();
// 21h-r11: player 1's pause menu on the stats screen is on ([Splitscreen] StatsPauseP1 > 0) - menus.cpp then hides
// its RESPAWN there in every match (the small script it runs while the game makes ifs_pausemenu)
bool P2SpawnStatsPauseOn();
// 21j-r25: player 1's pause menu opened as the game's own Start opens it (PauseState[0]::SetPaused(1)) - for his free camera,
// where Start did not pause (menus.cpp). 1 = paused now, 0 = already paused, -1 = not possible. Game thread.
int P2SpawnPauseP1();
// 21i-r1: the map being played (the mission queue's last loaded entry; false if not known) and the game's own local player
// count (1 / 2) - for menus.cpp's [bots] lines
bool P2SpawnMissionName(char* out, size_t size);
int P2SpawnGameLocalPlayers();
// 21h-r2: a question / request from players 3 / 4's real pause menus ("BFXQ: rp <what> <player 3 / 4> [i] [v]", without the
// "rp "): resume / suicide / restart / quit (1 = taken, done in his next frame), ctlget / ctlset / ctlsave / ctllive (his
// controller options). -1 for anything else. Game thread (inside the menu's own tick).
int P2SpawnRealPauseAsk(const char* what);

// 21h-r3 (spawn_ui.cpp's menu layer): whose box the IF root (popups, option screens) is drawn in - 1 player 2's quarter
// (3 / 4 players: his pause is on and player 1's is not), 0 player 1's box, -1 as before ([Splitscreen] MenuBoxP2=1); 21j-r15:
// 2 = player 2's own half (the 2-player split, [Splitscreen] PauseLikeP1P2=1: a 16:9 box in it, as player 1's)
int P2SpawnMenuOwner();
// 21j-r15 (spawn_ui.cpp): player 1's menu layer goes in his top left quarter - the 3-player layout ([Splitscreen] MenuBoxP1=1);
// the last view drawn is player 3's full-width bottom half, which MenuViewportBegin takes for the 2-player bottom half
bool P2SpawnMenuP1Quarter();
// 21j-r15 (players34.cpp): the match is over - the game showed VICTORY / DEFEAT ([Splitscreen] EndMatch34=1), until the next
// match starts
bool P2SpawnMatchEnded();
// 21j-r15 (menus.cpp): player 2's pause menu as player 1's in the 2-player split ([Splitscreen] PauseLikeP1P2=1) - its
// script runs while the game makes ifs_pausemenu2 in a 2-player match
bool P2SpawnP2PauseLike();

} // namespace bf1
