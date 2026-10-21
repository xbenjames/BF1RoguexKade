// BF1GameExt - menus.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/menus/ (was controller_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r27: ControllerAudioFrame - the audio scanner's frame ([Splitscreen] AudioScan=1, [audio] lines; called from
// split_spawn_menus.cpp's menu layer frame).
// 21i-r4: ControllerBotFrame - the bleed scan's reinforcement lines (called from split_spawn_menus.cpp).
// 21g-r3: LockedSideP34 - players 3 and 4's side while sides are locked (campaign: player 1's; Galactic Conquest: the side
// each picked on PICK YOUR SIDE), for their spawn screens (split_spawn_menus.cpp).
// 21f-r1: SidesLockedNow / PlayerSideNow - the game's side state, for the spawn screens (class_wheel.cpp,
// split_spawn_menus.cpp): no side screen while sides are locked (campaign, Galactic Conquest).
// 21e-r1: no interface change - ControllerLuaTraceInstall also loads the split screen menu script (BF1GameExt_split.lua,
// [Splitscreen] MainMenuSplit=1); ControllerScanStart also finds the game's pushnumber the script's questions need.
// 21d-r2: no interface change (the RESET TO DEFAULTS work is all inside menus.cpp).
// Phase 21d: ControllerLuaTraceInstall also loads the controller page script (BF1GameExt_ctrlpage.lua) that makes the
// game's controller page changeable ([Controls] ControllerPage=1).
// Phase 21c: ControllerLuaTraceInstall - the controller page scan (the options screens' Lua calls, log only).
// Phase C0: read-only scan of BF1's control bindings, ahead of porting
// BF2GameExt's controller support.
//
// BF1 keeps bindings inside each player profile: three tables of 43 entries
// (one per action), copied into the player's controller object when the
// profile is applied. This logs those tables, the profile's joystick-enabled
// flag and the two local player slots, then logs every entry that changes,
// so binding something in Options -> Controls shows how entries are encoded.
#pragma once

namespace bf1 {

// Byte-checks the code that defines the layout, then starts the watcher.
// Call only after ScanIdentityOk() is true.
void ControllerScanStart();

// 21c: call from DllMain (before the game registers its Lua functions), after ScanOnAttach. Puts a logging stub in
// the registration entries of the ScriptCB functions the options / controller screens use ([Controls] LuaTrace=1).
// The stubs call the game's functions unchanged; ControllerScanStart finds the Lua helpers that read their arguments.
void ControllerLuaTraceInstall();

// 21f: 1 = sides are locked - the game's "can switch sides" is off (the shell turns it off for the campaign and Galactic
// Conquest before the match: ScriptCB_SetCanSwitchSides(nil)), or its metagame rules are on (the game's own check says no
// switching then too), and [Splitscreen] SideLock=1: the spawn screens of players
// 1 and 2 skip the side screen (their side is the game's). 0 = sides can be switched (or SideLock=0). -1 = not known
// (the byte was not found / the trace is off): the side screens come up as before. Any thread.
int SidesLockedNow();
// 21f: the side the game has for local player 0 / 1 (the table ScriptCB_SetPlayerSide and ScriptCB_SetMetagameTeams
// write): 1 / 2 (0: none set); -1 = not known.
int PlayerSideNow(int player);
// 21g-r3: player k + 3's side (k = 0 / 1) while sides are locked, as a relation to player 1's: in Galactic Conquest (its rules
// on) his pick on PICK YOUR SIDE against player 1's pick (the conquest's side numbers are not a battle's: a battle's team 1
// is the side its map was made for), else - the campaign, or no pick - player 1's side. 1 = player 1's side, -1 = the other
// one, 0 = not known. *why gets a short text for the log. Any thread.
int LockedSideP34(int k, const char** why);
// 21i-r4: the bleed scan's reinforcement lines ([Splitscreen] BotScanIA=1; log only) - call each frame on the game thread
// (split_spawn_menus.cpp EndScanFrame); logs at most every 5 s while the numbers change.
void ControllerBotFrame();
// 21j-r27: the audio scanner ([Splitscreen] AudioScan=1; log only) - call each frame on the game thread in the shell and in a
// match (split_spawn_menus.cpp P2SpawnMenuLayer); the sound state every 250 ms, a line when it changes.
void ControllerAudioFrame();

} // namespace bf1
