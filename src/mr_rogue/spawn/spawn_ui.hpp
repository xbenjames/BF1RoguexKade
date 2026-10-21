// BF1GameExt - spawn_ui.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was spawn_ui_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 18t: SpawnUiPushScreenFn / SpawnUiPopScreenFn. Nothing else changed.
// Phase 13a: spawn screen scan (2020 build: GOG 1.3 / Steam), log-only.
//
// In split screen player 1's spawn screen (the PC mouse screen
// "ifs_pc_SpawnSelect") is drawn over the whole screen, landing mostly on
// player 2's side. The game picks the spawn screen in one function (mode 1 ->
// ifs_pc_SpawnSelect, mode 2 -> the console per-player "ifs_readyselect1").
// This logs every PushScreen / PopScreen, every change the chooser sees (mode,
// object, top screen), a snapshot of the menu stack while split screen is on,
// and searches the game's memory once (read-only) for the console spawn
// screen's scripts - to decide between using the console per-player screens
// and confining the PC screen to player 1's half. [ifs] lines.
//
// Phase 13c: the console screens ARE loaded (ifs_readyselect1 / 2,
// ifs_pausemenu2). Mode 2 = "Spawn pressed" (13b forcing it spawned player 1
// at once), so instead: while choosing in split screen, the chooser builds
// "ifs_readyselect1" in place of "ifs_pc_SpawnSelect".
// BF1GameExt.ini [Display] SplitConsoleSpawn=1 (default 1; 0 = PC screen).
//
// Phase 13d: the console screen is an empty shell in the 2020 game (the C++
// spawn display only fills ifs_pc_SpawnSelect's parts), so the plan is to fit
// the PC screen into player 1's half. To find the code that draws menus, a
// frame is captured by itself 1.5 s after the PC spawn screen comes up in
// split screen (at most twice per session).
//
// Phase 13f: the menu layer fix. The frame-interface function draws each
// player's HUD inside the view loop, then the global interface root (all menu
// screens) once after it - with the viewport still on player 2's half. That one
// call now goes through BF1GameExt: in split screen, while player 1's menu stack
// is not empty, the viewport is set to player 1's box for it and put back after.
// BF1GameExt.ini [Display] SplitMenuFit=1 (16:9 box in player 1's half; 2 = the
// whole half, squeezed; 0 = off).
//
// Phase 13g: the panels moved, but the command post map and the soldier did not:
// they belong to player 1's own HUD layer (the view loop's per-view call, same
// function). While player 1's PC spawn screen is up, that call gets the same box
// - only when the current viewport is player 1's own, so player 2's HUD never
// moves. BF1GameExt.ini [Display] SplitSpawnMapFit=1 (0 = off).
#pragma once

#include <cstdint>

namespace bf1 {

// Call after the code is unlocked, on the game thread.
void SpawnUiScanStart(bool onGameThread);

// 18t: the game's PushScreen(name, player, arg3) and PopScreen(arg) (thiscall on an IF manager),
// found by pattern and byte-checked at start; their entries go through the [ifs] logging stubs, so
// a call through these addresses is logged like the game's own. 0 if not found / not hooked.
uintptr_t SpawnUiPushScreenFn();
uintptr_t SpawnUiPopScreenFn();

} // namespace bf1
