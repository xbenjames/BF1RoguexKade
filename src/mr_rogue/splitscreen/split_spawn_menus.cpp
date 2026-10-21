// BF1GameExt - split_spawn_menus.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was p2spawn_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r27: the menu layer's frame also runs menus.cpp's audio scanner (ControllerAudioFrame, [Splitscreen] AudioScan;
// log only). Nothing else changed.
// 21j-r25: P2SpawnPauseP1 - player 1's pause menu opened as the game's own Start opens it (PauseState[0]::SetPaused(1), through
// PsSetPausedWrap as StatsPauseFrame does), for menus.cpp's free camera: its Start did not pause. Nothing else changed.
// 21j-r16: THE MOUSE CURSOR IS PLAYER 1'S ([Splitscreen] CursorP1Only=1; CursorFrame, just above the end of the file's own
// namespace; [p2spawn] cursor lines). The game shows its one mouse cursor with every PushScreen and hides it only when a menu
// stack runs empty; the menu layer draws it in player 1's box while his menus are up, else where the last view left the viewport -
// player 2's half. r15's video: player 2 paused on his spawn screen, his pause menu's pop left that screen on his stack, so the
// cursor stayed on, and BF1GameExt's spawn screen switches keep it as they find it: player 1's mouse cursor stood in the corner of
// player 2's half while he played. In a split screen match it is now hidden whenever player 1 has no menu up and player 2's pause
// menu is not on. Nothing else changed.
// 21j-r15: (1) THE 2-PLAYER SPLIT - PLAYER 2'S PAUSE MENU AS PLAYER 1'S ([Splitscreen] PauseLikeP1P2=1; the block above
// P2SpawnMenuOwner's globals): P2SpawnMenuOwner answers 2 while his pause is the one on (spawn_ui.cpp draws the menu layer
// in a 16:9 box of his half), "rp p2dx / p2dy" (menus.cpp's script moves his menu where player 1's stands in his
// half), P2SpawnP2PauseLike; players 1 / 2's pause objects found on their own too (PauseObjsFind). (2) The 3-player layout:
// P2SpawnMenuP1Quarter ([Splitscreen] MenuBoxP1=1) - player 1's menu layer in his quarter. (3) P2SpawnMatchEnded (players34.cpp:
// players 3 / 4's views on players 1 / 2's cameras at the end). (4) EndStatsFullScreen 2 by default (the 2-player stats over the
// whole screen, centred). Nothing else changed.
// 21j-r13: the 3 / 4-player spawn point windows a little further apart (kPwGap 12 -> 24 of the game's units: r11's video had
// the VEHICLES bar standing right on the COMMAND POST box). Nothing else changed.
// 21j-r11: the frame-rate scan's part in RpIfUpdateWrap (render_hooks.cpp, kPerfOurMenus34; [perf] lines): players 3 / 4's
// menu managers (input, tick) and the sound under their pause, after the game's IF update. Log only; nothing else changed.
// 21j-r9: THE CONSOLE WINDOWS ON THE 3 / 4-PLAYER SPAWN SCREENS ([Splitscreen] SpawnPanels34=1, [panels] lines). The spawn
// point and class screens in players 3 / 4's look (players 1 - 4 in quarters, player 3's bottom half with 3 players) had
// texts standing in for the game's windows (the post's name with "<" / ">" and "2 / 5", its vehicles, the class name with
// "<" / ">"). They now have the game's windows, as the 1-player screens: COMMAND POST (the post's name), VEHICLES: (its
// vehicles), OBJECTIVES: (his side's, the game's own getter) and the class window (the class's name on its orange bar,
// EQUIPMENT and the weapons in its box) - NewButtonWindow's orange title bar (a copy of the game's own bar) over its blue box
// (9 bitmaps of "border_3_pieces") with the text in it, built in the game's units in a group of their own scaled to the view
// (F34WindowsMake / PwWindow). The class ring of a quarter stands left of the class window (kPwRingX). Players 3 / 4 and
// player 1 (Side12Make) as before otherwise; player 2's spawn point / class screens get theirs from Side12Frame (g_f12[1] in
// his HUD root), his own screen keeping its grid, map, ring and prompt (g_flow.pwLook). A window that cannot be made: the
// texts as before (that player, that match). 2-player and 1-player screens are not touched.
// 21i-r4: EndScanFrame also calls ControllerBotFrame (menus.cpp: the bleed scan's reinforcement lines). Nothing
// else changed.
// 21i-r1: P2SpawnMissionName (the map being played, from the mission queue found in r12) and P2SpawnGameLocalPlayers, for
// menus.cpp's bot / reinforcement scan lines. Nothing else changed.
// 21h-r14: THE BLACK SCREEN AFTER QUIT ON THE STATS SCREEN FIXED. PauseState +22h is the game's "leave the stats" flag (set by
// ScriptCB_QuitFromStats / DONE), not "Start pressed": r9 - r13 took it as Start (and cleared it). After QUIT the game went to
// the shell with player 1's stats mode (+21h) still on, saw +22h, and paused the shell - ifs_pausemenu over the menus, nothing
// drawn. Now +22h is left to the game, and the menu opens only while ifs_teamstats / ifs_awardstats / ifs_personalstats is the
// top of player 1's manager (Start on his pad or Esc).
// 21h-r13: RESTART MISSION ON THE STATS SCREEN plays the same map again ("rp statsrestart": the mission queue's index back by
// one, to the map just played; then the stats are left as DONE does - the game loads it as its next mission). EXIT TO WINDOWS
// as the game has it.
// 21h-r12: QUIT ON THE STATS SCREEN GOES TO THE MAIN MENU. After a win the game is already on its way out of the match, so its
// QuitToShell does nothing there (it only acts while the game is in a match state) - leaving the stats loads the next mission
// in its queue, or goes to the shell when there is none. "rp statsquit" (the stats pause's QUIT -> YES): the queue's index past
// its count and its loop flag 0, then the script leaves the stats as DONE does (the game's ScriptCB_QuitFromStats) -> the game
// finds no next mission -> the main menu. The queue is found by a pattern (Ps34Install, [pause34] line). OPTIONS hidden there.
// 21h-r11: THE PAUSE MENU ON THE STATS SCREEN, FIXED, AND IN EVERY MATCH ([Splitscreen] StatsPauseP1=2 now: 1 player, 2, 3, 4).
// r9 waited for the game's own "Start pressed on the stats" (PauseState +22h), but the r10 log showed it never comes after a
// win: the game's PauseState Update only reads Start while its game-over flag (GOG 1.3 0x008A4D86) is 0. Now Start on player
// 1's pad (the game's pad object, +C4h) or Esc (the game's window in front) - a new press while the stats are up - opens it.
// StatsPauseP1=1 means real 3 / 4-player matches only (it let 1-player matches through before). RESPAWN is hidden there in
// every match now (menus.cpp runs that part for ifs_pausemenu itself; "rp statspause" as r10).
// 21h-r10: "rp statspause" (P2SpawnRealPauseAsk): 1 while player 1's pause menu is up over the stats screen, so
// BF1GameExt_pause34.lua hides RESPAWN there. Nothing else changed; no new hooks.
// 21h-r9: (1) PLAYERS 3 / 4'S VICTORY / DEFEAT AS PLAYERS 1 / 2'S: the game's own image - for each of them a group and an image
// element made with the game's constructors (as its victory / defeat display makes players 1 / 2's: the group anchored at
// the middle of his view's root, the image "ig_victory_defeat" 256 x 64), shown with the same colour and half of the texture
// the game's Show picks for his team (green VICTORY / red DEFEAT), hidden when the game hides players 1 / 2's; taken out of
// his root and deleted at the match teardown. The r8 text stays as the fallback. (2) THE PAUSE MENU ON THE STATS SCREEN
// ([Splitscreen] StatsPauseP1=1): Start there only set a flag the PC game never reads (PauseState +22h, its stats mode +21h);
// now it opens player 1's pause menu (the game's SetPaused) over the stats - drawn over the whole screen with them (its screen's
// viewport -1 while it is up there) - RESUME GAME back to the stats, QUIT to the main menu.
// 21h-r8: THE END OF A MATCH WITH 3 / 4 PLAYERS AS THE XBOX'S ([Splitscreen] EndMatch34=1, EndStatsFullScreen=1; [end34] lines).
// r7's scan: the game's victory / defeat display is made at match start (room for 2 - its count was 2, nothing written past
// it) and switched on at the end (+4A0h = 1, +4A4h = 5 s) by its Show (GOG 1.3 0x000CA730, one jump thunk, one caller), which
// gets the winners as a team mask: a player whose team's bit is clear gets VICTORY (green, the texture's top half), set:
// DEFEAT (red). Then ifs_teamstats / ifs_awardstats on player 1's manager - while players 2 - 4's spawn screens went on (the
// text glitches: four sets of spawn screen texts with the stats in one font). Now, from the moment it is switched on until the
// next match: (1) player 2's spawn screens close and stay closed; players 3 / 4's spawn screens and pause menus close and
// stay closed (no Start), their soldiers get the calm packet, the auto-spawn stands aside; their views go to player 1's
// camera shot. (2) Players 3 / 4 get VICTORY / DEFEAT in their quarters for the 5 s the game shows player 1's / 2's - their
// team's result from Show's mask (its jump thunk goes through BF1GameExt, the game's function runs first, unchanged); for
// now in the game's font, green / red, at its place (the game's own image next). (3) The stats / awards screens are drawn
// over the whole screen, not in player 1's quarter (spawn_ui.cpp asks P2SpawnEndStatsFull) - player 1's pad drives them.
// 21h-r7: THE MATCH END SCAN (log only, [Splitscreen] EndScan34=1, [end34] lines) - for players 3 / 4 at victory / defeat
// (the Xbox: every view stops on VICTORY / DEFEAT, then one stats board over the whole screen, player 1's pad). The game's
// victory / defeat display (GOG 1.3: made at 0x000CA3D0 - 4B0h bytes, its pointer at 0x0089C470; its constructor 0x000CA140
// fills arrays of 2 at +20h (B0h each) and +180h (100h each) for as many players as a count read at its start says) is
// watched each frame: when it is made / goes, that count (above 2 it would write past its arrays), its flags, and every
// player's screens (player 1's / 2's managers, player 2's and players 3 / 4's spawn screens, who has a soldier) when they
// change. Nothing in the game changes.
// 21h-r6: (1) PLAYERS 3 / 4 PAUSE ON THEIR SPAWN SCREENS ([Splitscreen] SpawnPauseP34=1): Start on his side, spawn point or
// class screen opens his real pause menu over them (the screen goes, his view shows the world behind the menu, as player 1's /
// 2's pause there); RESUME GAME / B / Start brings the same screen back with his buttons latched (the A that closed the menu
// is no press there - player 2's way). RESPAWN is not offered while he is down ("rp down"). (2) THEIR BUTTONS REMAPPED as
// players 1 / 2's ([Splitscreen] RemapP34=1): the game's own remap (ScriptCB_SetFunctionIdForButtonId, GOG 1.3 0x0016C370)
// only writes the button's slot of the map in the PlayerInput (0x0013E920: +12Ch + (mode * 32 + button) * 4 = the function
// table's id) and saves the profile; here the same slot is written in his own PlayerInput ("rp ctlfnset"; "rp ctlfnreset"
// puts a tab back to the game's default), kept in BF1GameExt.ini [Splitscreen] ControlMapP3 / P4 (57 numbers: the page's
// function number of buttons 1 - 19 for infantry, vehicle, starfighter; -1 = the game's) and written into his PlayerInput
// whenever it is a new one (each match) - [ctlmap] lines. The sticks stay as they are (the game's analog setter does nothing
// in this build, for players 1 / 2 too); BACK / START / the d-pad are fixed, as on player 1's page.
// 21h-r4: THE CONTROL MAP SCAN (log only, [ctlmap] lines): each player's button -> function map (his PlayerInput's, by mode:
// infantry / vehicle / starfighter, in the gamepad page's numbers) when players 3 / 4's real pause menu opens - for the next
// round's copy of player 1's gamepad page in their quarters; "rp ctlfn" answers it to their page. Nothing else changed.
// 21h-r3: PLAYERS 3 / 4'S PAUSE - SOUND, NO CROUCH; PLAYER 2'S POPUPS IN HIS QUARTER. (1) The sound runs while a pause of
// players 3 / 4 holds the world (PauseState::Update +18h's function, the game's pause runs it each frame - the world loop that
// runs it otherwise stands still with the pause byte on): their pause was as loud as the game (the in-game bus fade never went
// on) and their menus' sounds did not play. Run after the game's IF update, only while neither player 1's nor player 2's
// pause is on ([Splitscreen] PauseSoundP34=1). (2) His real pause menu up: players34.cpp gives his soldier a calm packet
// while the menu reads his pad, and holds it after the menu goes until every button is up (PlayerNMenuInput) - the B (crouch)
// that closed it no longer crouches him. (3) P2SpawnMenuOwner: in 3 / 4-player matches the menu layer (popups, option screens:
// the IF root) goes in player 2's quarter while his pause is on and player 1's is not (spawn_ui.cpp draws it there;
// [Splitscreen] MenuBoxP2=1). Their OPTIONS / popups in player 1's scheme are BF1GameExt_pause34.lua's (p21h-r3).
// 21h-r2: REAL PAUSE MENUS FOR PLAYERS 3 AND 4 ([Splitscreen] RealPauseP34=1, [rp34] lines; the section "21h-r2: REAL PAUSE MENUS"
// below): their pause screens are the game's own kind - ifs_pausemenu3 / 4 (BF1GameExt_pause34.lua, made by the game's script
// functions right after ifs_pausemenu2) on viewports 2 / 3, run on two IF managers of BF1GameExt's (the game's constructor;
// controller and Lua index 2 / 3), driven right after the game's IF update; the Lua index's 2-slot limits (current manager,
// active screen get / set, a manager's controller) answer 2 / 3 with ours; reset with the game's managers. Start (in the world)
// opens it; world pause, sound and HUD as before; RESUME GAME / OPTIONS (his CONTROLLER page) / RESPAWN / RESTART MISSION / QUIT
// done here when the screen asks ("BFXQ: rp ..." through menus.cpp). BF1GameExt's own pause screen stays as the
// fallback (RealPauseP34=0, or something not found / not made). The 21h-r1 pause scan stays (PauseScan34).
// 21h-r1: THE PAUSE SCAN (log only, [Splitscreen] PauseScan34=1, [pause34] lines) - the first step to real pause menus for
// players 3 and 4 (the game's own ifs_pausemenu3 / 4, or an equivalent, on viewports 2 / 3 in their quarters). How the game's
// pause reaches a player's IF manager (PauseState::Update -> the pausing viewport -> PauseState::SetPaused -> GetIFManager(player)
// -> PushScreen("ifs_pausemenu" / "ifs_pausemenu2", player, player)), the 2-slot limits on that way (the managers, the active
// screen per Lua index, the IF update's loops, the pause states), and where each pause screen is drawn (its record's viewport ->
// roots[viewport], each element's class and place). Three jump thunks go through BF1GameExt (SetPaused, a screen's activation and
// deactivation - the game's own function runs unchanged); everything else is read. Players 3 / 4's own pause screens (BF1GameExt's)
// are dumped too, to set them beside the game's. Nothing else changed. (The section "21h-r1: THE PAUSE SCAN" below.)
// 21g-r3: LOCKED SIDES FOR PLAYERS 3 AND 4 TOO - while the game says sides cannot be switched (SidesLockedNow, as player
// 2's in 21f) their spawn screens have no side screen: a match starts at the spawn point screen on the side they have to
// play (menus.cpp's LockedSideP34: the campaign - player 1's side; Galactic Conquest - the side each picked on
// PICK YOUR SIDE), and B there does not go back to a side screen. Nothing else changed.
// 21f-r1: LOCKED SIDES - while the game says sides cannot be switched (the shell turns it off for the campaign and
// Galactic Conquest; menus.cpp's SidesLockedNow, [Splitscreen] SideLock=1) player 2's spawn screens have no side
// screen: a new match starts at his spawn point screen on the side the game gave him (his character's team, else the
// game's side table), and B there does not go back to it. Players 3 and 4's screens are unchanged. Nothing else changed.
// 20g-r13: 3-PLAYER STEP 1 (players34.cpp's Views=3: players 1 / 2 in the top quarters, player 3 in the bottom half).
// Player 3's screens in his half take player 2's horizontal places (F34Hz: his root a horizontal half, [Splitscreen]
// MenusHorizontal=1): the side screen's title, soldiers and names (no SELECT SIDE), his map in its own lent layout 2 at
// Classic's place and size (F34MapLayoutHz; layout 1 stays players 3 / 4's and player 2's quarter one), the post's texts
// on the right where Classic's panels stand, the ring in Classic's arrangement, the class's name and EQUIPMENT where
// Classic's class window stands, B BACK, the prompts bottom right. His HUD root gets Classic's safe area too (HzRootsFrame,
// HorizontalSafeArea=1). HzPanelsScan (log only, once): player 2's spawn point panels and class window trees and their
// first bitmaps' raw fields - to copy them into player 3's screens next round. Quarters (4-player) unchanged.
// 20g-r12 (from the user's horizontal shots against the Classic Collection's video): HORIZONTAL 2-PLAYER. (1) Both halves'
// HUD roots get Classic's safe area down - 10% at the top and the bottom of each half (offset 0.10, scale 0.80) - in place
// of the game's 10% / 2% (player 1) and 2% / 10% (player 2): player 2's score bar no longer sits on the split line, both
// halves alike, and the game's own spawn screens and HUD stand where Classic has them ([Splitscreen] HorizontalSafeArea=1,
// HzRootsFrame each frame). (2) RootPlace takes the root's safe area into account in horizontal (GroupFrac): his side
// screen's title, soldiers and names, his prompts, the blue grid at Classic's places; his panels, class window and
// "B BACK" left as the game lays them out (FlowLayoutFrame does nothing); his class ring in Classic's arrangement, in his
// class screen's Helptext_Back group (the Info group stays turned, as Classic's window). (3) P2SpawnRootGeom /
// P2SpawnGroupPlace / P2SpawnRootsHorizontal for class_wheel.cpp (player 1's places, the maps).
// 20g-r11 (from r10's video: "player 1's mini map and class wheel to match the rest of the players"): PLAYER 1'S SPAWN
// POINT AND CLASS SCREENS IN PLAYERS 3 / 4'S LOOK (4-player, MenusLikeP34=1), as r10 did his side screen: Side12Frame
// draws all three of his screens from class_wheel.cpp's ClassWheelP1Look with F34's elements, places and sizes (the
// side's name, "<" / ">", the post, "1 / 3", its vehicles, SELECT SPAWNPOINT beside his map in the left; the class's
// name, "<" / ">", his side's classes on a ring as theirs - class_wheel.cpp's ring 3 - SELECT CHARACTER); his score
// bar is off on all three (theirs have none). F34Ring: the ring of a Flow34 (player 1's look: 3).
// 20g-r10 (from r9's test: "fix the choose sides screen for players 1 and 2 to match players 3 and 4"): (1) PLAYERS 1
// AND 2'S SIDE SCREENS IN PLAYERS 3 / 4'S LOOK (4-player, MenusLikeP34=1): player 1's was the Xbox's half layout
// (class_wheel.cpp's soldiers / texts in the PC spawn screen's groups) and player 2's sat in ifs_readyselect2's Info
// group with the root's safe area applied (smaller, shifted). Both are now drawn by Side12Frame exactly as players 3 /
// 4's: the same group kind in their HUD root (AddToRoot(root, group, 0, 0, 0)), the same elements at the same places
// (blue grid, CHOOSE YOUR SIDE, the two sides' soldiers and names, SELECT SIDE), the picked side lit. Their own side
// screens still run the choice with none of their elements made (player 1's spawn screen grid hidden while it is up);
// if the group cannot be made, r9's side screens. [side12] lines. (2) P2SpawnScreensUp(p) for players34.cpp: players
// 2 - 4's camera shots come in the frame their spawn screen comes up (their respawn timer ran out, as player 1's).
// 20g-r9 (from r8's test): (1) PLAYERS 3 AND 4'S PAUSE SCREEN IN THE GAME'S LOOK: the orange title bar and the blue item
// bars are the game's FlashyTextElements (a text with a bar texture behind it). When his pause screen comes up, player
// 2's pause menu (ifs_pausemenu2; else player 1's ifs_pausemenu) is searched for them (their places from its top-level
// elements' anchors in their root, logged once a session as [wheel] tree lines) and copies are made (ClassWheelFlashyLike:
// the game's constructor, its bar resource, the bar textures, margins, flags) - GAME PAUSED at the topmost one's place,
// the five items at the others' places (in order down); the item picked is drawn at full colour, the others dimmed.
// Without them: r8's texts on the blue grid. [Splitscreen] PauseLookP34=1. (2) THE GAME'S SOUND IS MUTED while his pause
// screen holds the world pause - what the game's own pause does (its SetPaused, GOG 1.3 0x001E6B30: the sound bus
// 0x985C8F54's volume to 0 over 1 s, back to 1 after; both functions taken from that code, byte-checked). [Splitscreen]
// PauseMuteP34=1. (3) SOUNDS: the select sound when it comes up, the back sound when it goes (B, Start, RESUME GAME) -
// each play's result logged for a while. (4) THE CLASS RING like Battlefront II's 4-player wheel (players 3 / 4 and player
// 2's in a quarter): the picked soldier in the middle of the quarter, his feet two thirds down, the ring twice as wide
// (radius 0.26 of the quarter), the picked one 0.40. Droidekas on it are mapped around the game's own soldier's place
// (512, 520 in the root's units - r7 took 520 / 624 of the quarter's height, 260: the droideka stood off).
// 20g-r8: PLAYERS 3 AND 4'S OPTIONS. OPTIONS on their pause screens is no longer greyed: it opens a CONTROLLER page in
// its place (the title becomes CONTROLLER): INVERT LOOK (on foot and in ground vehicles), INVERT FLYING (on = the
// game's default) and SENSITIVITY (look, 1-10, 5 = the game's default). Up / down picks, left / right (or A) changes,
// B = back to the pause items, Start = closes the pause screen. Each change takes effect in his PlayerInput at once
// (players34.cpp's PlayerNControlsSet - the game's own per-mode Y flip and look scale) and they are saved to
// BF1GameExt.ini [Splitscreen] InvertLookP3 / P4, InvertFlyingP3 / P4, SensitivityP3 / P4 when the page goes. OPTIONS
// stays greyed if his PlayerInput cannot take them (pads 3 / 4 off, or the game's reads not found). [p34spawn] lines.
// 20g-r7 (from r6's test): PLAYERS 3 AND 4'S PAUSE SCREENS AS PLAYERS 1 AND 2'S. Start on his pad while he is in the
// world: GAME PAUSED (orange) - RESUME GAME / OPTIONS (greyed: the options screen is players 1 / 2's) / RESPAWN /
// RESTART MISSION / QUIT, "B BACK" at the bottom left, over his own (paused) view - no blue grid. The game pauses
// while it is up, as for players 1 / 2's (the offline world pause the game's SetPaused calls, GOG 1.3 0x0013E180 /
// 0x0013E8F0, found through SetPaused's own calls; held while a pause screen of theirs is up). RESTART MISSION and QUIT
// ask "A YES / B NO" first, then do what the game's ScriptCB_RestartMission / ScriptCB_QuitToShell do (their bodies
// byte-checked; the functions they call). Up / down, A; B or Start = back. [Splitscreen] PauseP34=1, PauseWorldP34=1.
// 20g-r7: 4-PLAYER - PLAYER 2'S SCREENS AS PLAYERS 3 AND 4'S: his root is a quarter of the same size as theirs, so his
// side screen (title, names, soldiers, SELECT SIDE), spawn point screen (his map in their layout 1, the post's name,
// count and vehicles beside it; the game's panels, made for a half, hidden) and class screen (their ring; the class
// name over it; the class window hidden) take their places and sizes. [Splitscreen] MenusLikeP34=1.
// 20g-r6 (from r5's test):
//  - NO MORE "WAITING FOR PLAYER 1": players 2, 3 and 4 spawn when they press A on their class screen, whether player 1
//    is in the world or not ([Splitscreen] SpawnWaitP1=0; 1 = the old rule).
//  - PLAYERS 3 AND 4'S SPAWN POINT SCREEN HAS THEIR MAP: their own MapDisplay (players34.cpp's) shown on it in the
//    spawn screen look, as player 1's: SetFlag(1) + SetMode(1), drawn on top (layer 1), in a layout of its own - the
//    layout picker (MapPickWrap) answers layout 1 for it (the 1-player spawn map's, which split screen never uses; its
//    values lent while one of theirs is up, the game's put back after), placed in the left of his quarter with the
//    post's name, count and vehicles beside it; his chosen post lights up (his post / side lent to the spawn screen
//    object for each of its Update calls, through the vtable copy player 2's map uses). Back to the HUD minimap
//    (SetMode(0), SetFlag(0)) when the screen goes. [Splitscreen] SpawnMapP34=1 (needs P2SpawnMap=1).
//  - PLAYERS 3 AND 4'S PAUSE SCREENS: Start on his pad while he is in the world: PAUSED - RESUME / RESPAWN / CHANGE
//    SIDE in his quarter (up / down, A; B or Start = resume). While it is up his soldier stands still (his PlayerInput
//    updates without its pad - players34.cpp). RESPAWN kills his soldier through its death function (as player 2's
//    Respawn); CHANGE SIDE does too, then his side screen comes up. [Splitscreen] PauseP34=1.
// 20g-r5: THE CRASH AFTER THE FIRST ROUND. Players 3 and 4's spawn screen group (made in the runtime heap of the match)
// was kept over a match change and deleted in the next match (F34NewMatch / F34GroupMake): by then the old match's
// heap was gone and its memory belonged to the new match - the free broke the heap and the game shut itself down 140 ms
// into the second match. Now the match teardown deletes the group, its elements and the ring while the match is still
// loaded (P2SpawnP34Teardown, from players34.cpp's HUD teardown), and a group from an old match is only forgotten,
// never deleted.
// 20f-r4: THE REAL CAUSE OF THE r2 / r3 CRASH. r3's heap log: the runtime heap was the current one when our frame began
// (so r2's guess was wrong) and had one free block of 75 MB; right after player 3's group was made and put in roots[2]
// that block's size read 0 - and the game's next allocation failed (GOG 1.3 0x00287BFF). The group was a plain
// RedGroupElement (0xA0 bytes), and the game's AddToRoot writes the root anchor and flag at +A0h, +A4h, +A8h - fields of
// a RedScreenGroupElement (0xB0 bytes, the class a HUD root's top-level children are) - so it wrote 0, 0, 0 over the
// header of the heap block right after the group: the free block's size. class_wheel.cpp's ClassWheelGroupNew now
// makes a RedScreenGroupElement. The r3 heap code stays (our frame still switches to the runtime heap if another one is
// current, and checks it has 256 KB free) - in r3's log it was a no-op.
// 20f-r3: THE CRASH WHEN PLAYERS 3 AND 4'S SCREENS CAME UP (p20f-r2, Steam). Player 3's group was made, then every one of
// his elements and player 4's group came back 0 from the game's operator new, and the game's own new(4Ch) in the
// renderer (GOG 1.3 0x00287BFF) came back 0 and it wrote through it. The game's new / delete use the CURRENT heap (an
// index the game switches with SetHeap; free puts the block back into the current heap's free list), and the heap that
// was current in our frame had only about one group's worth left. Now players 3 and 4's screens make and delete
// everything with the game's runtime heap current (SetHeap(runtime) for our frame, the old heap put back after; the
// memory set-up, SetHeap, the heap table and the game's failed-allocation record found by pattern and byte-checked);
// their elements are made only while the runtime heap has 256 KB free (else the auto-spawn looks after him that match);
// a failed element logs the game's failed-allocation record; the heaps are logged at start and at each match start.
// 20f-r2: PLAYERS 3 AND 4'S SPAWN SCREENS ([Splitscreen] SpawnP34=1, with Players=3 / 4 and PadsP34=1; [p34spawn]
// lines). 20f-r1's scan found no console screens for players 3 and 4 on PC and two IF managers only, so theirs are
// BF1GameExt's own: an empty group of the game's (class_wheel.cpp's ClassWheelGroupNew) in the HUD root of his
// quarter's view (the game's AddToRoot, found and byte-checked with the setup's root table: anchored at the view's top
// left, flag 0), shown while he chooses - players34.cpp turns that view's HUD flag on while it is shown. In it player
// 2's screens' parts: the texts, the side screen's two soldiers, the class ring (class_wheel.cpp's ring 1 / 2, the
// Xbox's arrangement, no EQUIPMENT box) and the blue grid; the same flow as player 2's (side -> spawn point -> class,
// his respawn timer after a death; d-pad / left stick, A, B on his own pad), and A on the class screen spawns him as the
// PC Spawn button does: SetSpawnChoice(side, class, ValidPost(post, side)) + SetReady(1) on his character (player index
// 2 / 3), only while player 1 is in the world. At a match end the screen's elements are left as they are (their
// classes go with the match) and the group is deleted (it only takes them out). Without his pad, or if no group could
// be made, the auto-spawn (AutoSpawnP34) spawns him as before. Player 2's screens are unchanged (Pad2Read is now
// PadReadAt(pad 2)).
// 20f-r1 (log only, players34.cpp's 4-player work): the match scan also looks for players 3 and 4's console
// screens - ifs_readyselect3 / 4, ifs_charselect3 / 4, ifs_mapselect3 / 4, ifs_pausemenu3 / 4 - and their
// elements in the registry (the console paths with 3 / 4), and lists every record in the screen list (its hash,
// the name when it is one we know, its viewport and element count). [p2spawn] screens lines. Nothing else changed.
// 19i (19h-r2's log and video): (1) NO GLITCH FRAME. The game's PushScreen / PopScreen only queue the
// switch (a screen is up: IF manager +34h), and the IF update's tick (GOG 1.3 rva 0x00134D60: it queues
// its own update event and drains the manager's event list - Exit, SetScreen, Enter, activation, all in
// one pass) runs it the next frame, before that frame's draw; FlowFrame runs after the draw (the menu
// layer), so each new screen drew one frame as the game left it (the first time turned and big, after that
// without the blue grid and our texts). Now FlowShow / FlowClose call that tick themselves right after the
// push / pop (manager 1 only, dt 0; the tick found from the IF update's second loop and byte-checked), and
// the new screen is set up (texts, grid, flat layout) in the same call - the next frame drawn is the
// finished screen.
// spawn_ui.cpp does the same for player 1's console screens (P2SpawnSettleManager).
// [Splitscreen] SpawnScreenSwitchNow=1 (0: the switch is left to the IF update, as 19h).
// (2) HIS DEATH AS PLAYER 1'S: the game opens player 1's spawn screen when his character's respawn timer
// (character +C0h, counted down by the game after a death - SpawnDisplay::Update, GOG 1.3 0x0007998B)
// runs out, about 4 s after the death; his came up 1.5 s after. His spawn point screen now waits for
// that same timer (30 s at most).
// (3) HIS SOLDIERS PLAYER 1'S SIZE: the ring's picked soldier 0.508 (was 0.38), the ring where player 1's
// stands; the side screen's soldiers 0.54 (was 0.50) and where player 1's stand, the names and title too.
// (4) HIS MAP AS PLAYER 1'S: the game's spawn screen puts player 1's map in its spawn screen look with
// MapDisplay::SetFlag(1) (GOG 1.3 0x00188620: the +1F289h flag, the layout re-applied); his map had the
// flag clear (set for its Update calls only), which is the game's full-map look: the dark 600 x 600 quad
// (+1F400h), the icons group, the layout picked by the flag (GOG 1.3 0x001873C0: flag -> layout 5, player
// 1's). Now the layout picker's jump thunk goes through BF1GameExt (his map with the flag -> layout 4, his
// own), his map gets SetFlag(1) + SetMode(1) as player 1's does (SetMode(0) + SetFlag(0) after), and his
// Update runs with the spawn screen's post and team set to his (the SpawnDisplay's +23E0h / +23DCh, put
// back after each call - the map highlights GetSpawnDisplay()'s post, which is player 1's object): his
// chosen post lights up on his map. (5) His UI sounds: the tick name list starts with the names the sound
// banks have (shell_select_change ...); each play is logged a few times (class_wheel.cpp).
// (6) Log only: his root's shown elements once while his spawn point screen is up (the "OBJECTIVES:" over
// his map), the map's +1F510h / +1F740h in the map dump.
// 19h-r2 (19h's log and video): (1) HIS MAP: the dump had it set up, shown, at the Xbox's place - and
// the game calls his map's own Update every frame too (its HUD display loop), which, with his flag
// clear and no unit, drops the track and hides the element again after ours showed it. His map's
// vtable is copied while his spawn point screen is up, its Update slot BF1GameExt's: every call (the
// game's and MapUpdateP2's) runs with his flag set and his layout lent, so the element stays shown
// through the draw. The game's vtable goes back when the screen goes. (2) 19g's places (the groups
// pinned) on the first frame his spawn point / class screen is up, the flat layout (from the title
// bars now, class_wheel.cpp) over them.
// 19h: (1) HIS MAP ON TOP: 19g's log has his map element shown, in his root, at the Xbox's place, its
// Update running - yet nothing drew. The spawn screen puts player 1's map element on layer 1 (drawn
// last in his root, GOG 1.3 0x00079264); his stayed at 128. His goes to layer 1 while his spawn point
// screen is up (back after), and both maps are dumped side by side once a match (group, picture,
// raster, arrow, icons group, materials and textures) - log only. (2) FLAT: his spawn point screen's
// panels and his class window flat and placed from their own sizes (class_wheel.cpp's
// ClassWheelXbPanels / ClassWheelXbWindow; 19g's moves if a size does not check out); his class
// screen's Info group, now straight, also stands his ring and grid straight - the ring is placed in
// fractions of his half (ClassWheelFlatPlace) in the Xbox's arrangement. (3) No "B BACK" on his spawn
// point screen (icon and label hidden; his prompt is in that group). (4) His VEHICLES box: the game's
// own list for his post (ClassWheelVehiclesAt). (5) The score bar off on both players' side screens
// (their TeamDisplay's enabled byte +30h, put back after). (6) His side soldiers' feet 3% lower.
// 19g: PLAYER 2'S SCREENS LIKE THE XBOX'S VERTICAL SPLIT (from 19f's video against the Xbox shots).
// (1) THE BLUE GRID: each of his three screens gets a copy of the spawn screen's "shell_cube" (the grid
// room player 1 has behind his; the PC makes it for player 1 only) at his root's centre, in the
// screen's Info group, drawn first (class_wheel.cpp's ClassWheelCubeNew); the Info group draws
// before his HUD (layer 129), so the score bar stays over the grid. [Splitscreen] P2SpawnBackdrop=1.
// (2) HIS MAP DRAWS: the game updates only player 1's map every frame (GOG 1.3 0x0013F8F3:
// GetMap(0)->Update(frame time)) and MapDisplay::Update shows its element only at its end, so his
// map never drew. While his spawn point screen is up his map's own Update (its vtable slot 1, the
// function byte-checked) is called here every frame; it sits in split layout 4, which takes the
// Xbox's place above the panels while his screen is up (18z's anchor change was undone by SetMode).
// (3) HIS CLASS RING: the class screen shows all his side's classes on a ring that turns
// (ClassWheelRing2Frame) instead of one soldier. (4) The Xbox's layout: side screen soldiers smaller
// and where the Xbox has them, the title and names too (his texts drew 0.1 of his half left of 18z's
// numbers: his root has no x offset); the panels and window centred and lower; "B BACK" in and up
// (hidden on the side screen, as the Xbox has none there); the prompts on its row. (5) His side soldiers
// and ring are deleted only while the screen record and group they were made in are still the screen's
// (as the texts are); otherwise they are left to the game.
// 19f: RESPAWN DIES AT ONCE. The death took, but his half stays paused under his pause menu until he
// leaves it, so he only fell then. The wrapper now also unpauses him right after the kill, as the Lua
// ScriptCB_Unpause(player) does (GOG 1.3 rva 0x0016A960 -> SetPaused(player, 0) rva 0x00097D20: the
// per-player pause object's flag off, which pops his pause menu): he dies where he stands, and his
// spawn point screen follows.
// 19e: the Respawn's death goes through the vtable slot as it is (ragdoll.cpp's hook when it is in: the
// soldier gets his ragdoll and the death tracker sees him, as for any death), after the game's own
// function behind it is byte-checked as before. 19d called the original directly.
// 19d: THE RESPAWN'S SOLDIER. 19c's log: the unit a character holds (+B4h) is the controllable 250h bytes
// into the EntitySoldier (ragdoll.cpp's kUnitOff; its RTTI names EntitySoldier with that offset in the
// vtable's locator), so "unit + 8" was a float, not the +8 sub-object's vtable. The soldier is unit minus
// the locator's offset (checked = 250h); its +8 vtable slot 1 (or ragdoll.cpp's original when the ragdoll
// hook sits there) is the death function, byte-checked, called on soldier + 8.
// 19c: DIAGNOSTIC. The Respawn still found "death function does not match" with ragdoll.cpp's original:
// the wrapper now logs the unit's type, its +8 vtable, that slot 1, what ragdoll.cpp hands back and
// which check failed. The screens' placement logs the root the screen is in (size, offset, scale) and
// the group's anchor each time, for the panels that drew in different places with the same numbers.
// 19b: (1) the Respawn's death function: ragdoll.cpp hooks that vtable slot, so the slot held BF1GameExt's
// hook and 19a's check failed ("does not match"): the game's own is taken from ragdoll.cpp then
// (RagdollOrigDeathFn), byte-checked as before. (2) The panels' and the class window's parent groups are
// pinned at 0, 0 before the children are placed - the console script slides them in and left them at
// different places at match start and after a death (18z's and 19a's videos).
// 19a: (1) PLAYER 2'S RESPAWN KILLS HIM. The pause menu's Respawn runs the Lua ScriptCB_PlayerSuicide
// (GOG 1.3 rva 0x0017A220): the player's unit's primary vtable slot 1 - a removal, not a death (no
// death, no animation; 18y's log). Its registration slot now points at BF1GameExt's wrapper: in split
// screen, for a local player with a unit, not in multiplayer (the game's own check), the unit's death
// function is called instead - its +8 sub-object's vtable slot 1 (the one ragdoll.cpp hooks for
// ragdolls: it ends in SetState(3), byte-checked here too) - so he dies where he stands and his spawn
// point screen follows; anything else goes to the game's own function. [Splitscreen] P2RespawnDies=1.
// (2) The panels keep the script's spacing, shifted down (they overlapped).
// 18z: the Xbox layout for player 2's screens: the spawn point screen's panels centred in the lower
// half with his map above them (the map's drawn element is the group at map +30h, shown, its anchor
// moved up, layout 1 smaller through class_wheel.cpp), the class window centred under the soldier.
// 18x: player 2's 3D soldiers - the two sides' first classes on his side screen (the picked one white,
// the other dark), the picked class on his class screen - made with class_wheel.cpp's ring code
// (ClassWheelModelNew) as children of the screen's Info group in his half; his map element hidden
// while his side / class screen is up. [Splitscreen] P2SpawnSoldiers=1 (0 = texts only).
// 18w: player 2's "SELECT ..." prompts go into the screen's Helptext_Back group (the "B BACK" row at the
// bottom, unslanted) instead of the slanted Info window - they were drawn tilted. Nothing else changed.
// Phase 18v: PLAYER 2 MENU INPUT - THE INTERFACE'S PLAYER COUNT (2020 build: GOG 1.3 / Steam).
// What 18u's log showed: in player 2's pause menu the interface never asked for player 2's buttons at
// all (the input manager's GetBufB(1): 0 calls a second; the only reads of his buttons came from the
// game's pause check, which is how Start still worked) - and his held-button slot never changed. The
// IF update (GOG 1.3 rva 0x00134DE0) gives a manager input only while that manager's player (+18Ch)
// is below the input manager's player count ([input manager + 14h], read at +24h: its only reader) -
// and the PC game sets that count to 1 at boot (GOG 0x00146D1D: push 1; call InputMgrInit). So
// manager 1 (player 2's) never got any input. That one read now goes through BF1GameExt: 2 in split
// screen while the 6c redirect gives player 2's objects for index 1 (player2_input.cpp), the game's own
// value otherwise. Applied only once 18u's held-button table is moved (player 2's slot is used from
// now on). Both are [Splitscreen] P2MenuInputFix=1. Everything from 18u stays.
//
// Phase 18u: PLAYER 2 SPAWN SCREENS + MENU INPUT FIX (2020 build: GOG 1.3 / Steam). [p2spawn] lines.
//
// (1) THE MENU INPUT FIX. Why player 2's pause menu took no pad input (GOG 1.3 rvas, same code on
// Steam): the IF input step (rva 0x00133590; args: manager index, input buffer, player, dt) keeps the
// buttons each player holds in a table at rva 0x004A35D0, 1Ah bytes per player (index = the
// manager's player +18Ch * 1Ah) - and checks a button's flag before it posts its event, and writes
// all 1Ah bytes back at the end. On PC the table has ONE slot; the Xbox's (default.xbe, 0x00453050)
// has four. Player 2's slot (+1Ah..+33h) is the Lua's current-manager index (+1Ch, what
// ScriptCB_PushScreen pushes on) and the object at +24h - so a screen on manager 1 (his pause menu)
// wrote over both every frame and its own flags were never what it set: the menu never moved.
// The step's 25 references to the table (cmp / mov byte, movups, movq, mov word - all in that one
// function, checked in GOG 1.3: no other code reads the table) are found by pattern at start, each
// one checked (exactly 25, the write-back's three in their places, the table right before the Lua's
// index), and moved - on the game thread - to a 4-slot table in BF1GameExt (player 1's slot copied
// over first); every reference read back. Player 2's slot is logged when it changes while manager 1
// has a screen up (his presses reaching the interface). [Splitscreen] P2MenuInputFix=1 (0 = off).
//
// (2) PLAYER 2'S SPAWN SCREENS, by themselves, in split screen. While player 2's character (1) has no
// unit in a match (match start: at once; after a death: 1.5 s later), BF1GameExt puts his own console
// screens up on IF manager 1 with the game's PushScreen(name, 1, 1) - as the game pushes his pause
// menu - and the game draws them in his half (viewport 1 -> roots[1]): ifs_readyselect2 (side, the
// first time in a match) -> ifs_mapselect2 (spawn point; later spawns start here) ->
// ifs_charselect2 (class). One of them is up at a time (PopScreen, then PushScreen); the IF
// cursor's visibility is put back after each (PushScreen shows it, the last PopScreen hides it - player
// 1's own screen keeps its cursor as it had it). While one of them is manager 1's top, its countdown
// (+194h) is held at 2 so its console script takes no input: pad 2 drives it through BF1GameExt
// (pad 2's XInput state: d-pad / left stick, A, B; held repeats). A screen the game puts over it (his
// pause menu) is left alone; ours waits under it and goes on when it is back on top.
//   side:        left / right pick the side, A joins it. "CHOOSE YOUR SIDE" and the two sides' names
//                (the picked one white, the other grey), placed in his half by fractions of his root.
//   spawn point: left / up and right / down = the previous / next post his side can spawn at (the
//                game's NextSpawnPoint round, its cursor put back), A = on, B = back to the side.
//                The post's name in the COMMAND POST panel, his side's objective (the game's own text
//                function, as SelectTeam fills the PC's objective window) in OBJECTIVES, HINTS hidden,
//                "SELECT SPAWNPOINT"; his map in its spawn screen mode (SetMode(1), put back after;
//                [Splitscreen] P2SpawnMap=1).
//   class:       left / right = the class (the side's list, as the PC cycle), the class name in the
//                title bar and EQUIPMENT in the box (as player 1's soldier select), A = spawn, B = back.
// Spawn = exactly what the PC Spawn button does to its character, on character 1: SetSpawnChoice(team,
// class index, post) with the post through the game's ValidPost(post, team), then SetReady(1) - only
// while player 1 is in the world (the auto-spawn's rule; until then "WAITING FOR PLAYER 1", B
// cancels). The screen goes when his unit is in the world. autospawn.cpp stands aside while these
// screens look after player 2. [Splitscreen] P2SpawnScreen=1 (0 = the auto-spawn as before).
// Not yet: player 2's 3D soldiers (side screen, class ring) and the VEHICLES text.
//
// 18t: the screen dumps stay (each screen that comes up on either manager in a match: its record and
// root, 30 frames later); the F5 preview is gone.
//
// What 18s found on Steam in split screen, and the code behind it (GOG 1.3 rvas, same code on Steam):
//   - the game DOES load player 2's console screens in split screen: ifs_readyselect2,
//     ifs_charselect2, ifs_mapselect2 (and ifs_pausemenu2), every panel of them in the registry
//     (16 of 16), and each with viewport 1 at record +48h (player 1's "1" screens: viewport 0;
//     ifs_pc_SpawnSelect: -1).
//   - that viewport IS used on PC (18s said it was not - the search missed it): a screen's
//     activation (rva 0x002B9CA0) adds each of its top-level elements (record +4Ch, count +44Ch) to
//     roots[viewport] (AddToRoot, rva 0x0028BDD0) - or, with viewport -1, to the IF root (the menu
//     layer, rva 0x0028BE30). roots[0] / roots[1] are player 1's / player 2's HUD roots, which the
//     view loop draws inside each player's own viewport. So player 2's console screens are drawn in
//     player 2's half by the game itself, laid out by the console scripts - while
//     ifs_pc_SpawnSelect (viewport -1) goes to the IF root, the menu layer 13f had to box.
//   - the game pushes player 2's pause menu as PushScreen(GetIFManager(1), "ifs_pausemenu2", 1, 1),
//     then sets manager +194h = 3 (3 frames before the screen takes input) (rva 0x001E7181).
//   - a 2D root (the IF root, the HUD roots) keeps its children in a list at +18h ([head] = first
//     node, [node] = next, [node+0Ch] = the element); +0 / +4 its width / height (ints), +8 / +0Ch
//     offset, +10h / +14h scale. An IF group's children are at +80h, the same kind of list. An element
//     knows its list: [element + 1Ch + 8] = the list it is in. (18s walked roots at +80h: 0 children.)
//   - 18s's three misses, fixed here: the chooser is found with its first 6 bytes hooked (13a's
//     logging jump), the match scan waits until the spawn screen is set up, roots walked at +18h.
//
// 18t PREVIEW (gone in 18u - the screens come up by themselves) (diagnostic, for screenshots): in
// split screen, in a match, F5 cycled player 2's own console screen - none -> ifs_readyselect2 ->
// ifs_mapselect2 -> ifs_charselect2 -> none - on manager 1 (player 2's), exactly as the game pushes player 2's pause menu: the game's own
// PushScreen(manager 1, name, 1, 1) and countdown 3, and PopScreen(0) to take it down (only a
// screen BF1GameExt put up; a screen the game put up on manager 1 is left alone and F5 waits). Run
// on the game thread, in the menu layer hook, before the IF root is drawn. 30 frames after each
// screen comes up (and after any screen the game puts up on either manager in a match), the
// screen's record (viewport, elements, and which root each element is in) and that root's tree
// are logged. Nothing else in the game is changed; the screens are the console scripts' own
// shells (titles and panels, no soldiers / map / texts yet). Pad 2's buttons reach the screen's own
// script (as with the pause menu) - for the screenshots, look only, F5 moves on.
//
// Phase 18s: PLAYER 2 SPAWN SCAN, log-only (everything kept): the two IF managers (GetIFManager,
// rva 0x000784B0; both run every frame, each reading the input of the player at +18Ch), the one
// spawn screen (GetSpawnDisplay(player) ignores its player), the chooser's player-1 sites, the
// setup's root (push 0 -> RootIndex -> root table), the per-player map / display arrays, the
// screen list, the registry, the menu layer's IF root, the map table.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "class_wheel.hpp"
#include "menus.hpp"
#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "split_spawn_menus.hpp"
#include "pads.hpp"
#include "player2_input.hpp"
#include "players34.hpp"
#include "ragdoll.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"
#include "spawn_ui.hpp"

// 21h-r1: the return address of the current function (who called a wrapper) - declared directly, as player2_input.cpp does
#ifdef _MSC_VER
extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#define BF1_RETURN_ADDRESS() _ReturnAddress()
#else
#define BF1_RETURN_ADDRESS() __builtin_return_address(0)
#endif

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

// GetIFManager(player) (GOG 1.3 rva 0x000784B0): cmp eax,1; ja; imul eax,eax,244h; add eax,[base]
const char* const kGetIfManager = "55 8B EC 8B 45 08 83 F8 01 77 0E 69 C0 44 02 00 00 03 05 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kGetIfManagerBase = 0x13; // imm32 of 'add eax, [base]'
// the manager the Lua's ScriptCB_PushScreen pushes on (rva 0x00134380): the index at a global
const char* const kLuaManager = "A1 ?? ?? ?? ?? 83 F8 01 77 0D 69 C0 44 02 00 00 03 05 ?? ?? ?? ?? C3 33 C0 C3";
constexpr uint32_t kLuaManagerIndex = 0x01;
constexpr uint32_t kLuaManagerBase = 0x12;
// the IF update (rva 0x00134DE0): both managers every frame, the input from the player at +18Ch
const char* const kIfUpdate = "55 8B EC 83 EC 08 56 E8 ?? ?? ?? ?? 8B F0 8B CE 89 75 F8 E8 ?? ?? ?? ?? 84 C0 0F 85 ?? ?? ?? ?? "
                              "53 57 8B CE E8 ?? ?? ?? ?? 33 FF 89 45 FC 33 F6 83 FF 02 7D 30 8B 0D";
constexpr uint32_t kIfUpdatePlayerAt = 0x6A;   // mov ebx, [esi+eax+18Ch]
const char* const kIfUpdatePlayer = "8B 9C 06 8C 01 00 00";
constexpr uint32_t kIfUpdateInputAt = 0x7C;    // call [eax+20h] = GetPlayerInput(ebx)
const char* const kIfUpdateInput = "FF 50 20";
constexpr uint32_t kManagerSize = 0x244;
constexpr uint32_t kMgrTopName = 0x34;   // the top screen's name (inline text)
constexpr uint32_t kMgrActive = 0xB4;    // byte
constexpr uint32_t kMgrDepth = 0x178;
constexpr uint32_t kMgrPlayer = 0x18C;   // PushScreen's 2nd argument: whose input the manager reads
constexpr uint32_t kMgrIndex = 0x190;    // the manager's own index
constexpr uint32_t kMgrCountdown = 0x194;
constexpr uint32_t kMgrCallback = 0x19C;
// GetSpawnDisplay(player) (rva 0x00078510): the player is ignored
const char* const kGetSpawnDisplay = "55 8B EC 83 7D 08 FF 75 0A 6A 00 E8 ?? ?? ?? ?? 83 C4 04 A1 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kGetSpawnDisplayGlobal = 0x14;
// the code that makes the spawn screen (class_wheel.cpp, rva 0x00077030): the global at +23h,
// then "push 0; call GetIFManager; add esp,4; mov [chooser global], eax" at +44h
const char* const kMakeDisplay = "56 68 F0 23 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0D 6A 00 8B C8 E8 ?? ?? ?? ?? "
                                 "8B F0 EB 02 33 F6 89 35 ?? ?? ?? ??";
constexpr uint32_t kMakeGlobal = 35;
constexpr uint32_t kMakeMgrAt = 0x44;
const char* const kMakeMgr = "6A 00 E8 ?? ?? ?? ?? 83 C4 04 A3";
constexpr uint32_t kMakeMgrGlobal = 0x4F;
// the chooser (spawn_ui.cpp, rva 0x00076D40) and its player-1 sites
const char* const kChooser = "55 8B EC 83 EC 20 56 8B F1 8B 0D ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 8B 56 18 8B F8 83 EA "
                             "01 0F 84";
constexpr uint32_t kChooserMgrReads[5] = { 0x09, 0x7E, 0x8B, 0x1B4, 0x1C1 }; // 8B 0D imm32
constexpr uint32_t kChooserPush1 = 0x38;        // 6A 01: the "%i" of "ifs_readyselect%i"
constexpr uint32_t kChooserReadyFmt = 0x3D;     // 68 imm32 "ifs_readyselect%i"
constexpr uint32_t kChooserMapCall = 0xAF;      // E8 GetMap(player)
constexpr uint32_t kChooserDispBCall = 0xD9;    // E8 display B getter(player)
constexpr uint32_t kChooserDispCCall = 0x101;   // E8 display C getter(player)
constexpr uint32_t kChooserPcName = 0x143;      // 68 imm32 "ifs_pc_SpawnSelect"
constexpr uint32_t kChooserPauseName = 0x17F;   // B9 imm32 "ifs_pausemenu"
const char* const kArrayGetter = "55 8B EC 8B 45 08 8B 04 85 ?? ?? ?? ?? 5D C3"; // mov eax,[player*4 + array]
constexpr uint32_t kArrayGetterImm = 9;
// the setup (rva 0x00077920): "push 0" (+0Dh), GetRootsObject (+12h), RootIndex(player) (+19h),
// AddToRoot (+3Ch)
const char* const kSetup = "55 8B EC 81 EC A0 00 00 00 56 57 8B F9 6A 00 89 7D EC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? "
                           "6A 01 83 EC 08 8B F0 8D 87 00 03 00 00";
constexpr uint32_t kSetupPush0 = 0x0D;
constexpr uint32_t kSetupRootsCall = 0x12;
constexpr uint32_t kSetupIndexCall = 0x19;
constexpr uint32_t kSetupAddCall = 0x3C;
constexpr uint32_t kSetupFindCall = 0x15D;      // FindByPath (18b)
const char* const kGetterImm = "A1 ?? ?? ?? ?? C3";                                   // mov eax, [imm]; ret
const char* const kRootIndexBody = "55 8B EC 8B 45 08 8B 44 81 6C 5D C2 04 00";          // [this + 6Ch + player*4]
const char* const kAddToRootBody = "55 8B EC F3 0F 10 45 14 56 8B 75 0C 83 EC 08 8B CE F3 0F 11 44 24 04 F3 0F 10 45 10 "
                                   "F3 0F 11 04 24 E8 ?? ?? ?? ?? FF 75 18 8B CE E8 ?? ?? ?? ?? 8B 4D 08 56 8B 0C 8D";
constexpr uint32_t kAddToRootTable = 0x37;      // imm32 of 'mov ecx, [ecx*4 + table]'
constexpr uint32_t kRootsIndexOfs = 0x6C;
constexpr int kMaxRootIndex = 16;
// the registry (18b): setup +15Dh -> FindByPath (+17h -> FindByHash: registry imm at +07h)
const char* const kFindByPath =
    "55 8B EC 51 FF 75 08 8D 4D FC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 04 8B E5 5D C3";
constexpr uint32_t kFindByHashCall = 0x17;
const char* const kFindByHash = "55 8B EC FF 75 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kRegistryImm = 0x07;
constexpr uint32_t kRegSlots = 0x1000;
// the screen list (18c): ScreenByHash
const char* const kScreenByHash = "55 8B EC 83 EC 08 56 57 8D 45 F8 B9 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? B9 ?? ?? ?? ?? "
                                  "8B 00 89 45 FC 8D 45 F8 50 E8";
constexpr uint32_t kScreenListImm1 = 0x0C;
constexpr uint32_t kScreenListImm2 = 0x17;
constexpr uint32_t kRecordHash = 0x04;
constexpr uint32_t kRecordViewport = 0x48;      // ScriptCB_SetIFScreenViewport writes it; the activation reads it (18t)
// the menu layer (13f): its GetRoot ("mov eax, imm; ret") = the one IF root
const char* const kMenuLayer = "B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? "
                               "?? ?? 6A 00 E8 ?? ?? ?? ?? 83 C4 04 E8";
constexpr uint32_t kMenuGetRootCall = 20;
const char* const kGetRootBody = "B8 ?? ?? ?? ?? C3";
// the map (18m): SetMode by pattern, its layout table at +7Eh
const char* const kMapSetModeBody =
    "55 8B EC 8B 45 08 53 8B D9 56 57 39 83 8C F2 01 00 74 06 FF 05 ?? ?? ?? ?? 89 83 8C F2 01 00 E8";
const char* const kMapTableSite = "F3 0F 10 04 BD";
constexpr uint32_t kMapTableAt = 0x7E;
constexpr uint32_t kMapMode = 0x1F28C;
constexpr uint32_t kMapFlag = 0x1F289;
constexpr uint32_t kMapLayoutFloats = 7;
constexpr uint32_t kMapLayoutBytes = kMapLayoutFloats * 4; // 19g: one layout of the table
constexpr int kMapLayoutsLogged = 8;
// IF elements: +10h path hash, +14h flags (100h = shown), +60h / +64h position, +80h a group's
// children (a circular list: [list] = first node, [node] = next, [node+0Ch] = the child)
constexpr uint32_t kElemHash = 0x10;
constexpr uint32_t kElemFlags = 0x14;
constexpr uint32_t kElemX = 0x60;
constexpr uint32_t kElemY = 0x64;
constexpr uint32_t kElemChildren = 0x80;
// the spawn screen (class_wheel.cpp)
constexpr uint32_t kSdMode = 0x18;
constexpr uint32_t kSdPlayer = 0x238C;
constexpr uint32_t kSdTeamShown = 0x23DC;
constexpr uint32_t kSdCharacter = 0x2370;
// characters (spawn_manager.hpp)
constexpr uint32_t kCharTeam = 0xA0;
constexpr uint32_t kCharUnit = 0xB4;

constexpr DWORD kPollMs = 100;
constexpr int kMaxLines = 2600;

// 18t: the chooser with 13a's logging jump at its entry (E9 rel32, 90)
const char* const kChooserHooked = "E9 ?? ?? ?? ?? 90 56 8B F1 8B 0D ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 8B 56 18 8B F8 83 EA "
                                   "01 0F 84";
// 18t: a screen's activation (GOG 1.3 rva 0x002B9CA0): for each element (record +4Ch, count +44Ch):
// viewport (+48h) -1 -> AddToIFRoot (+4Fh), else AddToRoot(viewport, ...) (+7Ch)
const char* const kActivate = "55 8B EC 83 EC 08 8B C1 56 33 F6 89 45 F8 39 B0 4C 04 00 00 7E ?? 53 57 8D 78 4C";
constexpr uint32_t kActivateInnerAt = 0x20;
const char* const kActivateInner = "8B 40 48 8B 1F 8B CB 89 45 FC 83 F8 FF 75";
constexpr uint32_t kActivateIfRootCall = 0x4F;
constexpr uint32_t kActivateRootCall = 0x7C;
const char* const kAddToIfRootBody = "55 8B EC F3 0F 10 45 10 56 8B 75 08 83 EC 08 8B CE F3 0F 11 44 24 04";
// 18t: the game's per-player pause push (GOG 1.3 rva 0x001E7181): mov ecx,[edi+1Ch] (the player);
// cmp ecx,1; push ecx; push ecx; mov ecx,esi (GetIFManager(player)); jge; push "ifs_pausemenu";
// jmp; push "ifs_pausemenu2"; call PushScreen; push 3; mov ecx,esi; call SetCountdown
const char* const kPausePush = "8B 4F 1C 83 F9 01 51 51 8B CE 7D 07 68 ?? ?? ?? ?? EB 05 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
                               "6A 03 8B CE E8";
constexpr uint32_t kPausePushName1 = 0x0D;
constexpr uint32_t kPausePushName2 = 0x14;
constexpr uint32_t kPausePushCall = 0x18;
constexpr uint32_t kPausePushCountdownCall = 0x21;
const char* const kSetCountdownBody = "55 8B EC 8B 45 08 89 81 94 01 00 00 5D C2 04 00";
// 18t: 2D roots (the IF root, the HUD roots): +0 / +4 width / height (int), +8 / +0Ch offset,
// +10h / +14h scale (floats), +18h the children list
constexpr uint32_t kRootList = 0x18;
// elements: +1Ch their list node ([node + 8] = the list they are in); a root's child: +A0h / +A4h its
// position in the root, +A8h a flag (AddToRoot's last argument)
constexpr uint32_t kElemNodeOwner = 0x24;
constexpr uint32_t kElemRootX = 0xA0;
constexpr uint32_t kElemRootY = 0xA4;
constexpr uint32_t kElemRootFlag = 0xA8;
// screen records: +48h viewport, +4Ch the top-level elements, +44Ch their count
constexpr uint32_t kRecordElems = 0x4C;
constexpr uint32_t kRecordCount = 0x44C;
constexpr int kRootsLogged = 6;
constexpr int kDumpDelayFrames = 30;
constexpr int kMaxDumpedNames = 10;

const char* const kScreenNames[] = { "ifs_readyselect1", "ifs_readyselect2", "ifs_charselect1", "ifs_charselect2",
                                     "ifs_mapselect1",   "ifs_mapselect2",   "ifs_pausemenu",   "ifs_pausemenu2",
                                     "ifs_pc_SpawnSelect", "ifs_teamstats",  nullptr };
// the elements the console screens have for player 1 (18c), tried for player 2 ("%d" = 1 / 2)
const char* const kScreenPaths[] = { "ifs_readyselect%d.Info",
                                     "ifs_readyselect%d.Helptext_Back",
                                     "ifs_readyselect%d.Helptext_Back.btn.label",
                                     "ifs_charselect%d.Info",
                                     "ifs_charselect%d.Info.Window",
                                     "ifs_charselect%d.Info.Window.titleBarElement",
                                     "ifs_charselect%d.Info.Window.BoxText",
                                     "ifs_charselect%d.Helptext_Back",
                                     "ifs_mapselect%d.Info",
                                     "ifs_mapselect%d.SideBar",
                                     "ifs_mapselect%d.SideBar.Spawnpoint",
                                     "ifs_mapselect%d.SideBar.Spawnpoint.titleBarElement",
                                     "ifs_mapselect%d.SideBar.Vehicle",
                                     "ifs_mapselect%d.SideBar.Objectives",
                                     "ifs_mapselect%d.SideBar.Hints",
                                     "ifs_mapselect%d.Helptext_Back",
                                     nullptr };

// found at start (0 = not found; the part that needs it stays off)
uintptr_t g_mgrBaseAddr = 0;    // address of the managers' base pointer
uintptr_t g_luaIndexAddr = 0;   // the current manager index (the Lua pushes on that manager)
uintptr_t g_chooserMgrAddr = 0; // the chooser's manager global
uint32_t g_chooserRva = 0;
uint32_t g_getSpawnDisplayRva = 0;
uintptr_t g_sdGlobal = 0;       // the spawn screen pointer
uintptr_t g_rootsObjAddr = 0;   // [this] -> the roots object (+6Ch + player*4 = root index)
uintptr_t g_rootTable = 0;      // root index -> the HUD root element
// 20f-r2: the game's AddToRoot(root index, element, anchor x, anchor y, flag) (GOG 1.3 rva 0x0028BDD0, cdecl): the
// element's anchor (+A0h / +A4h, fractions of the view) and flag (+A8h: 1 = the root's offset / scale applied to the
// anchor), then roots[index]->Add(element) - players 3 and 4's spawn screens' group goes into their roots with it
typedef void(__cdecl* AddToRoot_t)(int root, void* elem, float ax, float ay, int flag);
AddToRoot_t g_addToRoot = nullptr;
uintptr_t g_mapArray = 0;       // GetMap(player)
uintptr_t g_dispBArray = 0;     // the chooser's display B (sets +31h)
uintptr_t g_dispCArray = 0;     // the chooser's display C (sets +29h)
uintptr_t g_mapTable = 0;       // the map's layout table
uintptr_t g_registry = 0;
uintptr_t g_screenList = 0;
uintptr_t g_ifRoot = 0;         // the one IF root (menu layer)
int g_lines = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    if (++g_lines == kMaxLines)
        Log("[p2spawn] line limit reached, player 2 spawn scan going quiet");
    return true;
}

bool ReadMem(uintptr_t va, void* out, size_t n)
{
    if (va < 0x10000)
        return false;
    SIZE_T got = 0;
    return ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), out, n, &got) && got == n;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    if (!ReadMem(va, &v, sizeof(v)))
        return 0;
    return v;
}

uint8_t ReadU8(uintptr_t va)
{
    uint8_t v = 0;
    if (!ReadMem(va, &v, sizeof(v)))
        return 0;
    return v;
}

float ReadF32(uintptr_t va)
{
    float f = 0.0f;
    if (!ReadMem(va, &f, sizeof(f)))
        return 0.0f;
    return f;
}

int32_t ReadI32At(uint32_t rva)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

uint32_t CallTarget(uint32_t rva)
{
    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    return t;
}

bool InExe(uintptr_t va)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va < ExeBase() + b->sizeOfImage;
}

unsigned RvaOf(uintptr_t va)
{
    return InExe(va) ? static_cast<unsigned>(va - ExeBase()) : 0u;
}

// A printable C string (3+ characters, ending with a 0 inside the 63 bytes read) at va.
bool ReadText(uintptr_t va, char* out, size_t outSize)
{
    out[0] = 0;
    if (va < 0x10000 || outSize < 4)
        return false;
    char buf[64] = {};
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), buf, sizeof(buf) - 1, &got) ||
        got < 4)
        return false;
    size_t n = 0;
    while (n < got && buf[n] >= 32 && buf[n] < 127)
        ++n;
    if (n < 3 || n >= got || buf[n] != 0)
        return false;
    if (n >= outSize)
        n = outSize - 1;
    memcpy(out, buf, n);
    out[n] = 0;
    return true;
}

// the manager's top screen name (inline at +34h), "" if none
void TopName(uintptr_t mgr, char* out, size_t outSize)
{
    out[0] = 0;
    char buf[64] = {};
    if (!mgr || !ReadMem(mgr + kMgrTopName, buf, sizeof(buf) - 1))
        return;
    size_t n = 0;
    while (n < 60 && buf[n] >= 32 && buf[n] < 127 && n + 1 < outSize)
    {
        out[n] = buf[n];
        ++n;
    }
    out[n] = 0;
}

// The C++ type of an object, from its vtable's RTTI (".?AVName@@" -> "Name").
bool TypeName(uintptr_t obj, char* out, size_t outSize)
{
    out[0] = 0;
    const uint32_t vt = ReadU32(obj);
    const uint32_t col = vt ? ReadU32(vt - 4) : 0;
    const uint32_t td = col ? ReadU32(col + 12) : 0;
    char raw[64];
    if (!td || !ReadText(td + 8, raw, sizeof(raw)) || strncmp(raw, ".?AV", 4) != 0)
        return false;
    size_t n = 0;
    while (raw[4 + n] && raw[4 + n] != '@' && n + 1 < outSize)
    {
        out[n] = raw[4 + n];
        ++n;
    }
    out[n] = 0;
    return n > 0;
}

const char* TypeOrDash(uintptr_t obj, char* buf, size_t n)
{
    if (!obj || !TypeName(obj, buf, n))
        snprintf(buf, n, "%s", obj ? "type ?" : "-");
    return buf;
}

uint32_t PathHash(const char* s)
{
    uint32_t h = 0x811C9DC5u;
    for (; *s; ++s)
        h = (h ^ (static_cast<uint32_t>(static_cast<unsigned char>(*s)) | 0x20u)) * 0x01000193u;
    return h;
}

// the registry, live (18b: keys at +4, 1000h slots, then the values; backwards probing)
uintptr_t RegLookup(const char* path)
{
    const uint32_t hash = PathHash(path);
    if (!g_registry || !hash)
        return 0;
    const uintptr_t keys = g_registry + 4;
    const uintptr_t vals = keys + kRegSlots * 4;
    uint32_t i = hash & (kRegSlots - 1);
    for (uint32_t n = 0; n < kRegSlots; ++n)
    {
        const uint32_t k = ReadU32(keys + 4 * i);
        if (k == hash)
            return ReadU32(vals + 4 * i);
        if (!k)
            return 0;
        i = i ? i - 1 : kRegSlots - 1;
    }
    return 0;
}

// the screen list, live: the record of a screen name, or 0
uintptr_t ScreenRecord(const char* name)
{
    if (!g_screenList)
        return 0;
    const uint32_t hash = PathHash(name);
    uintptr_t node = ReadU32(g_screenList);
    for (int guard = 0; node && node != g_screenList && guard < 1024; ++guard)
    {
        const uintptr_t record = ReadU32(node + 0x0C);
        if (record && ReadU32(record + kRecordHash) == hash)
            return record;
        node = ReadU32(node);
    }
    return 0;
}

int ScreenCount()
{
    if (!g_screenList)
        return 0;
    int n = 0;
    uintptr_t node = ReadU32(g_screenList);
    for (int guard = 0; node && node != g_screenList && guard < 1024; ++guard)
    {
        ++n;
        node = ReadU32(node);
    }
    return n;
}

uintptr_t Manager(int i)
{
    if (!g_mgrBaseAddr || i < 0 || i > 1)
        return 0;
    const uintptr_t base = ReadU32(g_mgrBaseAddr);
    return base ? base + static_cast<uintptr_t>(i) * kManagerSize : 0;
}

uint32_t LocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    return (b && b->localPlayerCountRva) ? ReadU32(Rva(b->localPlayerCountRva)) : 0;
}

// the element's name, if its hash is one of the screen names (screens' roots) - else its hash
const char* NameOfHash(uint32_t hash, char* buf, size_t n)
{
    for (int i = 0; kScreenNames[i]; ++i)
        if (PathHash(kScreenNames[i]) == hash)
            return kScreenNames[i];
    for (int i = 0; kScreenPaths[i]; ++i)
    {
        char p[96];
        for (int player = 1; player <= 2; ++player)
        {
            snprintf(p, sizeof(p), kScreenPaths[i], player);
            if (PathHash(p) == hash)
            {
                snprintf(buf, n, "%s", p);
                return buf;
            }
        }
    }
    snprintf(buf, n, "hash %08X", static_cast<unsigned>(hash));
    return buf;
}

// a circular list (the IF's): [head] = first node, [node] = next, [node+0Ch] = the element. Fills up
// to max, returns how many there are (may exceed max).
int ListChildren(uintptr_t head, uintptr_t* out, int max)
{
    uintptr_t node = ReadU32(head);
    int n = 0;
    for (int guard = 0; node && node != head && guard < 512; ++guard)
    {
        const uintptr_t child = ReadU32(node + 0x0C);
        if (child && n < max)
            out[n] = child;
        if (child)
            ++n;
        node = ReadU32(node);
    }
    return n;
}

// only a group has the +80h child list (a leaf's +80h is something else)
bool IsGroup(uintptr_t e)
{
    char type[64];
    return TypeName(e, type, sizeof(type)) && strstr(type, "Group") != nullptr;
}

int GroupChildren(uintptr_t group, uintptr_t* out, int max)
{
    return IsGroup(group) ? ListChildren(group + kElemChildren, out, max) : 0;
}

int RootChildren(uintptr_t root, uintptr_t* out, int max)
{
    return root ? ListChildren(root + kRootList, out, max) : 0;
}

// roots[k] (the table AddToRoot uses; k = a screen's viewport, or a player's root index)
uintptr_t RootAt(int k)
{
    if (!g_rootTable || k < 0 || k >= kMaxRootIndex)
        return 0;
    return ReadU32(g_rootTable + 4 * static_cast<uint32_t>(k));
}

// the root index of player p (roots object +6Ch + p*4), -1 if not known
int PlayerRootIndex(int p)
{
    const uintptr_t obj = g_rootsObjAddr ? ReadU32(g_rootsObjAddr) : 0;
    if (!obj || p < 0 || p > 1)
        return -1;
    const int32_t idx = static_cast<int32_t>(ReadU32(obj + kRootsIndexOfs + 4 * static_cast<uint32_t>(p)));
    return (idx >= 0 && idx < kMaxRootIndex) ? idx : -1;
}

// which root an element is in (its list node's owner), as text
const char* WhichRoot(uintptr_t e, char* buf, size_t n)
{
    const uintptr_t owner = ReadU32(e + kElemNodeOwner);
    if (!owner)
    {
        snprintf(buf, n, "in no list");
        return buf;
    }
    if (g_ifRoot && owner == g_ifRoot + kRootList)
    {
        snprintf(buf, n, "in the IF root");
        return buf;
    }
    for (int k = 0; k < kRootsLogged; ++k)
    {
        const uintptr_t r = RootAt(k);
        if (r && owner == r + kRootList)
        {
            snprintf(buf, n, "in roots[%d]%s", k,
                     k == PlayerRootIndex(0) ? " (player 1's HUD root)" : (k == PlayerRootIndex(1) ? " (player 2's HUD root)" : ""));
            return buf;
        }
    }
    snprintf(buf, n, "in the list at 0x%08X (a group's?)", static_cast<unsigned>(owner));
    return buf;
}

void LogElement(const char* indent, uintptr_t e)
{
    char type[64];
    char nb[96];
    uintptr_t kids[4];
    const bool group = IsGroup(e);
    const int nk = group ? GroupChildren(e, kids, 4) : 0;
    const uint32_t flags = ReadU32(e + kElemFlags);
    if (Budget())
        Log("[p2spawn] %s0x%08X %s: %s, %s, layer %u, at %.0f %.0f%s%d%s", indent, static_cast<unsigned>(e),
            TypeOrDash(e, type, sizeof(type)), NameOfHash(ReadU32(e + kElemHash), nb, sizeof(nb)),
            (flags & 0x100) ? "shown" : "hidden", static_cast<unsigned>(flags & 0xFF), ReadF32(e + kElemX),
            ReadF32(e + kElemY), group ? ", " : "", nk, group ? " child(ren)" : "");
}

// a root's own child: also its position in the root (+A0h / +A4h) and flag (+A8h)
void LogRootChild(uintptr_t e)
{
    char type[64];
    char nb[96];
    uintptr_t kids[4];
    const bool group = IsGroup(e);
    const int nk = group ? GroupChildren(e, kids, 4) : 0;
    const uint32_t flags = ReadU32(e + kElemFlags);
    if (Budget())
        Log("[p2spawn]   0x%08X %s: %s, %s, layer %u, at %.0f %.0f, in the root at %.1f %.1f flag %u%s%d%s",
            static_cast<unsigned>(e), TypeOrDash(e, type, sizeof(type)), NameOfHash(ReadU32(e + kElemHash), nb, sizeof(nb)),
            (flags & 0x100) ? "shown" : "hidden", static_cast<unsigned>(flags & 0xFF), ReadF32(e + kElemX),
            ReadF32(e + kElemY), ReadF32(e + kElemRootX), ReadF32(e + kElemRootY),
            static_cast<unsigned>(ReadU8(e + kElemRootFlag)), group ? ", " : "", nk, group ? " child(ren)" : "");
}

void LogRootFields(uintptr_t root, const char* label)
{
    char type[64];
    uintptr_t kids[4];
    const int n = RootChildren(root, kids, 4);
    if (Budget())
        Log("[p2spawn] %s 0x%08X (%s): size %ld x %ld, offset %.1f %.1f, scale %.4f %.4f, %d child(ren)", label,
            static_cast<unsigned>(root), TypeOrDash(root, type, sizeof(type)),
            static_cast<long>(static_cast<int32_t>(ReadU32(root))), static_cast<long>(static_cast<int32_t>(ReadU32(root + 4))),
            ReadF32(root + 8), ReadF32(root + 0x0C), ReadF32(root + 0x10), ReadF32(root + 0x14), n);
}

// a 2D root: its fields and its own children (+18h) - not their children (a HUD root holds the whole HUD)
void DumpRoot2D(uintptr_t root, const char* label)
{
    if (!root)
        return;
    LogRootFields(root, label);
    uintptr_t kids[40];
    const int n = RootChildren(root, kids, 40);
    for (int i = 0; i < n && i < 40; ++i)
        LogRootChild(kids[i]);
    if (n > 40 && Budget())
        Log("[p2spawn]   ... %d more children", n - 40);
}

// a screen record: its viewport, its top-level elements and the root each one is in; then that root
void DumpRecord(const char* name, int manager)
{
    const uintptr_t rec = ScreenRecord(name);
    if (!rec)
    {
        if (Budget())
            Log("[p2spawn] %lu ms: screen \"%s\" (manager %d): NOT in the screen list", ScanMsSinceAttach(), name, manager);
        return;
    }
    const int32_t vp = static_cast<int32_t>(ReadU32(rec + kRecordViewport));
    int32_t count = static_cast<int32_t>(ReadU32(rec + kRecordCount));
    if (Budget())
        Log("[p2spawn] %lu ms: SCREEN \"%s\" up on manager %d: record 0x%08X, viewport %ld (its elements go to %s), %ld "
            "top-level element(s), %lu local player(s); player root indexes %d / %d:",
            ScanMsSinceAttach(), name, manager, static_cast<unsigned>(rec), static_cast<long>(vp),
            vp < 0 ? "the IF root" : "roots[viewport]", static_cast<long>(count), static_cast<unsigned long>(LocalPlayers()),
            PlayerRootIndex(0), PlayerRootIndex(1));
    if (count < 0 || count > 256)
        count = 0;
    for (int32_t i = 0; i < count && i < 16; ++i)
    {
        const uintptr_t e = ReadU32(rec + kRecordElems + 4 * static_cast<uint32_t>(i));
        char type[64];
        char nb[96];
        char where[96];
        if (!e)
            continue;
        if (Budget())
            Log("[p2spawn]   element %ld: 0x%08X %s: %s, %s, %s, in the root at %.1f %.1f", static_cast<long>(i),
                static_cast<unsigned>(e), TypeOrDash(e, type, sizeof(type)), NameOfHash(ReadU32(e + kElemHash), nb, sizeof(nb)),
                (ReadU32(e + kElemFlags) & 0x100) ? "shown" : "hidden", WhichRoot(e, where, sizeof(where)),
                ReadF32(e + kElemRootX), ReadF32(e + kElemRootY));
        // its children (one level): the panels, titles and prompts the console script laid out
        uintptr_t sub[8];
        const int ns = GroupChildren(e, sub, 8);
        for (int k = 0; k < ns && k < 8; ++k)
            LogElement("      ", sub[k]);
        if (ns > 8 && Budget())
            Log("[p2spawn]       ... %d more", ns - 8);
    }
    char label[96];
    if (vp < 0)
    {
        snprintf(label, sizeof(label), "the IF root (with \"%s\" up)", name);
        DumpRoot2D(g_ifRoot, label);
    }
    else
    {
        snprintf(label, sizeof(label), "roots[%ld] (with \"%s\" up)", static_cast<long>(vp), name);
        DumpRoot2D(RootAt(vp), label);
    }
}

void LogManagers(const char* why)
{
    for (int i = 0; i < 2; ++i)
    {
        const uintptr_t m = Manager(i);
        char top[64];
        TopName(m, top, sizeof(top));
        if (Budget())
            Log("[p2spawn] %lu ms: manager %d at 0x%08X (%s): top \"%s\", active %u, depth %lu, player +18Ch %ld, "
                "index +190h %ld, countdown %lu, callback 0x%08X, %lu local player(s)",
                ScanMsSinceAttach(), i, static_cast<unsigned>(m), why, top, static_cast<unsigned>(ReadU8(m + kMgrActive)),
                static_cast<unsigned long>(ReadU32(m + kMgrDepth)), static_cast<long>(static_cast<int32_t>(ReadU32(m + kMgrPlayer))),
                static_cast<long>(static_cast<int32_t>(ReadU32(m + kMgrIndex))),
                static_cast<unsigned long>(ReadU32(m + kMgrCountdown)), static_cast<unsigned>(ReadU32(m + kMgrCallback)),
                static_cast<unsigned long>(LocalPlayers()));
    }
    if (g_luaIndexAddr && Budget())
        Log("[p2spawn]   the Lua's current manager index (ScriptCB_PushScreen pushes on that manager): %ld",
            static_cast<long>(static_cast<int32_t>(ReadU32(g_luaIndexAddr))));
}

// ---- once per match: roots, arrays, maps, screens, registry ------------------------------------
void LogRoots()
{
    if (!g_rootsObjAddr || !g_rootTable)
        return;
    const uintptr_t obj = ReadU32(g_rootsObjAddr);
    if (Budget())
        Log("[p2spawn] HUD roots: roots object 0x%08X, root table at rva 0x%08X; player 1 -> roots[%d], player 2 -> roots[%d]",
            static_cast<unsigned>(obj), RvaOf(g_rootTable), PlayerRootIndex(0), PlayerRootIndex(1));
    for (int k = 0; k < kRootsLogged; ++k)
    {
        const uintptr_t r = RootAt(k);
        if (!r)
            continue;
        char label[32];
        snprintf(label, sizeof(label), "  roots[%d]", k);
        LogRootFields(r, label);
    }
    if (g_ifRoot)
        LogRootFields(g_ifRoot, "  the IF root");
    // the spawn screen's own groups: which root they are in
    const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
    static const uint32_t kSdGroups[5] = { 0x1A0, 0x250, 0x300, 0x3B0, 0x500 };
    for (uint32_t off : kSdGroups)
    {
        if (!sd)
            break;
        char where[96];
        if (Budget())
            Log("[p2spawn]   spawn screen +%03Xh: %s, %s, in the root at %.1f %.1f", static_cast<unsigned>(off),
                WhichRoot(sd + off, where, sizeof(where)), (ReadU32(sd + off + kElemFlags) & 0x100) ? "shown" : "hidden",
                ReadF32(sd + off + kElemRootX), ReadF32(sd + off + kElemRootY));
    }
}

void LogArray(const char* what, uintptr_t arr)
{
    if (!arr)
        return;
    char t0[64];
    char t1[64];
    const uintptr_t a = ReadU32(arr);
    const uintptr_t b = ReadU32(arr + 4);
    if (Budget())
        Log("[p2spawn] %s (rva 0x%08X): player 1 0x%08X (%s), player 2 0x%08X (%s)", what, RvaOf(arr),
            static_cast<unsigned>(a), TypeOrDash(a, t0, sizeof(t0)), static_cast<unsigned>(b), TypeOrDash(b, t1, sizeof(t1)));
}

void LogMaps()
{
    if (!g_mapArray)
        return;
    for (int p = 0; p < 2; ++p)
    {
        const uintptr_t map = ReadU32(g_mapArray + 4 * static_cast<uint32_t>(p));
        if (!map)
            continue;
        char where[96];
        if (Budget())
            Log("[p2spawn]   map of player %d 0x%08X: mode %lu, flag %u, %s", p + 1, static_cast<unsigned>(map),
                static_cast<unsigned long>(ReadU32(map + kMapMode)), static_cast<unsigned>(ReadU8(map + kMapFlag)),
                WhichRoot(map, where, sizeof(where)));
    }
    if (g_mapTable)
    {
        for (int l = 0; l < kMapLayoutsLogged; ++l)
        {
            float f[kMapLayoutFloats] = {};
            ReadMem(g_mapTable + static_cast<uintptr_t>(l) * kMapLayoutFloats * 4, f, sizeof(f));
            if (Budget())
                Log("[p2spawn]   map layout %d: %.1f %.1f %.1f %.1f %.3f %.3f %.3f", l, f[0], f[1], f[2], f[3], f[4], f[5],
                    f[6]);
        }
    }
}

void LogScreens()
{
    if (Budget())
        Log("[p2spawn] screens (%d in the screen list, %lu local player(s)):", ScreenCount(),
            static_cast<unsigned long>(LocalPlayers()));
    for (int i = 0; kScreenNames[i]; ++i)
    {
        const uintptr_t rec = ScreenRecord(kScreenNames[i]);
        if (Budget())
            Log("[p2spawn]   %-20s %s%s%08X, viewport +48h %ld, %ld top-level element(s)", kScreenNames[i],
                rec ? "IN the list" : "not in the list", rec ? ", record 0x" : "", static_cast<unsigned>(rec),
                rec ? static_cast<long>(static_cast<int32_t>(ReadU32(rec + kRecordViewport))) : -1L,
                rec ? static_cast<long>(static_cast<int32_t>(ReadU32(rec + kRecordCount))) : 0L);
    }
    // 20f-r1: players 3 and 4's console screens, and every record in the list
    {
        static const char* const kBases[] = { "ifs_readyselect%d", "ifs_charselect%d", "ifs_mapselect%d",
                                              "ifs_pausemenu%d", "ifs_teamstats%d", nullptr };
        for (int player = 3; player <= 4; ++player)
            for (int b = 0; kBases[b]; ++b)
            {
                char name[64];
                snprintf(name, sizeof(name), kBases[b], player);
                const uintptr_t rec = ScreenRecord(name);
                if (Budget())
                    Log("[p2spawn]   %-20s %s%s%08X, viewport +48h %ld, %ld top-level element(s) (20f)", name,
                        rec ? "IN the list" : "not in the list", rec ? ", record 0x" : "", static_cast<unsigned>(rec),
                        rec ? static_cast<long>(static_cast<int32_t>(ReadU32(rec + kRecordViewport))) : -1L,
                        rec ? static_cast<long>(static_cast<int32_t>(ReadU32(rec + kRecordCount))) : 0L);
            }
        // every record: its name when the hash is a known one
        static const char* const kKnown[] = {
            "ifs_readyselect", "ifs_charselect", "ifs_mapselect", "ifs_pausemenu", "ifs_teamstats", "ifs_pc_SpawnSelect",
            "ifs_pc_spawnselect", "ifs_boot", "ifs_legal", "ifs_login", "ifs_saveop", "ifs_movietrans", "ifs_main",
            "ifs_sp", "ifs_instant_top", "ifs_missionselect", "ifs_instant_side", "ifs_opt_top", "ifs_opt_general",
            "ifs_opt_sound", "ifs_opt_controller", "ifs_opt_display", "ifs_opt_pccontrols", "ifs_opt_mp",
            "ifs_pc_controls", "ifs_controller", "ifs_vkeyboard", "ifs_endgame", "ifs_missionend", "ifs_quit",
            "ifs_quitconfirm", "ifs_ingame_opt", "ifs_ingame_quit", "ifs_mp_lobby", "ifs_mpgs_pause", "ifs_pc_chat",
            "ifs_pc_scoreboard", "ifs_pc_teamstats", "ifs_pc_pause", "ifs_freeform", "ifs_campaign", "ifs_split",
            "ifs_sp_briefing", "ifs_briefing", "ifs_loading", "ifs_load", "ifs_fakeconsole", "ifs_console", nullptr };
        uintptr_t node = g_screenList ? ReadU32(g_screenList) : 0;
        int idx = 0;
        for (int guard = 0; node && node != g_screenList && guard < 256; ++guard, ++idx)
        {
            const uintptr_t record = ReadU32(node + 0x0C);
            const uint32_t hash = record ? ReadU32(record + kRecordHash) : 0;
            char name[64] = "";
            for (int k = 0; kKnown[k] && !name[0]; ++k)
            {
                char c[64];
                for (int suffix = -1; suffix <= 4 && !name[0]; ++suffix)
                {
                    if (suffix < 0)
                        snprintf(c, sizeof(c), "%s", kKnown[k]);
                    else
                        snprintf(c, sizeof(c), "%s%d", kKnown[k], suffix);
                    if (PathHash(c) == hash)
                        snprintf(name, sizeof(name), "%s", c);
                }
            }
            if (Budget())
                Log("[p2spawn]   list %2d: record 0x%08X hash %08X %-20s viewport %ld, %ld element(s)", idx,
                    static_cast<unsigned>(record), static_cast<unsigned>(hash), name[0] ? name : "(not a known name)",
                    record ? static_cast<long>(static_cast<int32_t>(ReadU32(record + kRecordViewport))) : -1L,
                    record ? static_cast<long>(static_cast<int32_t>(ReadU32(record + kRecordCount))) : 0L);
            node = ReadU32(node);
        }
    }
    if (!g_registry)
        return;
    for (int player = 1; player <= 4; ++player) // 20f-r1: 3 and 4 too
    {
        int found = 0;
        int total = 0;
        for (int i = 0; kScreenPaths[i]; ++i)
        {
            char path[96];
            snprintf(path, sizeof(path), kScreenPaths[i], player);
            ++total;
            if (RegLookup(path))
                ++found;
        }
        if (Budget())
            Log("[p2spawn]   registry: %d of %d known console screen paths found for player %d", found, total, player);
    }
}

void LogSpawnScreen()
{
    const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
    if (!sd)
        return;
    const uintptr_t ch0 = SpawnScanCharacter(0);
    const uintptr_t ch1 = SpawnScanCharacter(1);
    if (Budget())
        Log("[p2spawn] spawn screen 0x%08X: mode %lu, player +238Ch %ld, team shown %lu, character +2370h 0x%08X | "
            "character 0 0x%08X (team %ld, unit 0x%08X), character 1 0x%08X (team %ld, unit 0x%08X)",
            static_cast<unsigned>(sd), static_cast<unsigned long>(ReadU32(sd + kSdMode)),
            static_cast<long>(static_cast<int32_t>(ReadU32(sd + kSdPlayer))), static_cast<unsigned long>(ReadU32(sd + kSdTeamShown)),
            static_cast<unsigned>(ReadU32(sd + kSdCharacter)), static_cast<unsigned>(ch0),
            ch0 ? static_cast<long>(static_cast<int32_t>(ReadU32(ch0 + kCharTeam))) : 0L, static_cast<unsigned>(ch0 ? ReadU32(ch0 + kCharUnit) : 0),
            static_cast<unsigned>(ch1), ch1 ? static_cast<long>(static_cast<int32_t>(ReadU32(ch1 + kCharTeam))) : 0L,
            static_cast<unsigned>(ch1 ? ReadU32(ch1 + kCharUnit) : 0));
}

void MatchScan()
{
    if (Budget())
        Log("[p2spawn] %lu ms: MATCH SCAN (%lu local player(s), the spawn screen set up) - roots, displays, maps, screens:",
            ScanMsSinceAttach(), static_cast<unsigned long>(LocalPlayers()));
    LogSpawnScreen();
    LogRoots();
    LogArray("maps (GetMap)", g_mapArray);
    LogArray("display B (chooser +D9h, its +31h byte)", g_dispBArray);
    LogArray("display C (chooser +101h, its +29h byte)", g_dispCArray);
    LogMaps();
    LogScreens();
    LogManagers("match scan");
}

// ---- the watcher (log only) ------------------------------------------------------------------------
struct MgrState
{
    char top[64];
    uint8_t active;
    uint32_t depth;
    uint32_t player;
    uint32_t index;
    bool have;
};

DWORD WINAPI WatchThread(LPVOID)
{
    MgrState st[2] = {};
    uintptr_t scannedFor = 0; // the SpawnManager the match scan ran for
    int menuLines = 0;        // manager changes outside a match (the shell's menus): the first 30
    for (;;)
    {
        Sleep(kPollMs);
        const uintptr_t sm = SpawnScanManager(); // 0 outside a match
        for (int i = 0; i < 2; ++i)
        {
            const uintptr_t m = Manager(i);
            if (!m)
                continue;
            MgrState now = {};
            TopName(m, now.top, sizeof(now.top));
            now.active = ReadU8(m + kMgrActive);
            now.depth = ReadU32(m + kMgrDepth);
            now.player = ReadU32(m + kMgrPlayer);
            now.index = ReadU32(m + kMgrIndex);
            now.have = true;
            MgrState& was = st[i];
            if (!was.have || strcmp(now.top, was.top) != 0 || now.active != was.active || now.depth != was.depth ||
                now.player != was.player || now.index != was.index)
            {
                if ((sm || menuLines++ < 30) && Budget())
                    Log("[p2spawn] %lu ms: manager %d: top \"%s\", active %u, depth %lu, player +18Ch %ld, index +190h %ld "
                        "(%lu local player(s)%s)",
                        ScanMsSinceAttach(), i, now.top, static_cast<unsigned>(now.active), static_cast<unsigned long>(now.depth),
                        static_cast<long>(static_cast<int32_t>(now.player)), static_cast<long>(static_cast<int32_t>(now.index)),
                        static_cast<unsigned long>(LocalPlayers()), sm ? ", in a match" : "");
                was = now;
            }
        }
        // 18t: the match scan once per match, when the spawn screen is set up (team shown 1 or 2, a character)
        if (!sm)
        {
            scannedFor = 0;
            continue;
        }
        const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
        const uint32_t team = sd ? ReadU32(sd + kSdTeamShown) : 0;
        if (sm != scannedFor && sd && (team == 1 || team == 2) && ReadU32(sd + kSdCharacter))
        {
            scannedFor = sm;
            MatchScan();
        }
    }
}

// ---- 18t: the game thread (the menu layer hook, every frame) ----------------------------------------
typedef void(__fastcall* PushScreen_t)(void* mgr, void* unusedEdx, const char* name, int player, int arg3);
typedef void(__fastcall* PopScreen_t)(void* mgr, void* unusedEdx, int arg);
PushScreen_t g_pushScreen = nullptr;
PopScreen_t g_popScreen = nullptr;
bool g_screensOk = false;  // PushScreen / PopScreen, the managers and the screen list found

struct GameThread
{
    unsigned long frame = 0;
    // scheduled dumps (game thread): a screen that came up on a manager, dumped kDumpDelayFrames later
    char lastTop[2][64] = {};
    char dueName[64] = {};
    int dueManager = -1;
    unsigned long dueFrame = 0;
    char dumped[kMaxDumpedNames][64] = {};
    int dumpedCount = 0;
    bool splitRootsDumped = false;
    // the viewport line
    unsigned long lastSig[6] = { 0xFFFFFFFFul, 0, 0, 0, 0, 0 };
    int sigLines = 0;
};
GameThread g_gt;

bool WasDumped(const char* name)
{
    for (int i = 0; i < g_gt.dumpedCount; ++i)
        if (strcmp(g_gt.dumped[i], name) == 0)
            return true;
    return false;
}

// a screen coming up on either manager in a match: its record and root, kDumpDelayFrames later
void ScheduledDumps()
{
    const bool inMatch = SpawnScanManager() != 0;
    for (int i = 0; i < 2; ++i)
    {
        char top[64];
        TopName(Manager(i), top, sizeof(top));
        if (strcmp(top, g_gt.lastTop[i]) == 0)
            continue;
        snprintf(g_gt.lastTop[i], sizeof(g_gt.lastTop[i]), "%s", top);
        if (inMatch && top[0] && !g_gt.dueName[0] && !WasDumped(top) && g_gt.dumpedCount < kMaxDumpedNames)
        {
            snprintf(g_gt.dueName, sizeof(g_gt.dueName), "%s", top);
            g_gt.dueManager = i;
            g_gt.dueFrame = g_gt.frame + kDumpDelayFrames;
        }
    }
    if (!g_gt.dueName[0] || g_gt.frame < g_gt.dueFrame)
        return;
    char top[64];
    TopName(Manager(g_gt.dueManager), top, sizeof(top));
    if (strcmp(top, g_gt.dueName) == 0)
    {
        snprintf(g_gt.dumped[g_gt.dumpedCount++], sizeof(g_gt.dumped[0]), "%s", g_gt.dueName);
        DumpRecord(g_gt.dueName, g_gt.dueManager);
        // once in split screen: both players' HUD roots as they are
        if (!g_gt.splitRootsDumped && LocalPlayers() >= 2)
        {
            g_gt.splitRootsDumped = true;
            const int r0 = PlayerRootIndex(0);
            const int r1 = PlayerRootIndex(1);
            if (r0 >= 0)
                DumpRoot2D(RootAt(r0), "player 1's HUD root");
            if (r1 >= 0)
                DumpRoot2D(RootAt(r1), "player 2's HUD root");
        }
    }
    g_gt.dueName[0] = 0;
    g_gt.dueManager = -1;
}

// the viewport the menu layer is drawn with, whenever the situation changes (the first 40 changes)
void ViewportLine(uintptr_t root)
{
    const uintptr_t m0 = Manager(0);
    const uintptr_t m1 = Manager(1);
    const unsigned long d0 = m0 ? ReadU32(m0 + kMgrDepth) : 0;
    const unsigned long d1 = m1 ? ReadU32(m1 + kMgrDepth) : 0;
    if (!d0 && !d1)
    {
        g_gt.lastSig[0] = 0xFFFFFFFFul; // nothing up: the next screen logs again
        return;
    }
    MenuViewportSave vp = {};
    const bool got = MenuViewportRead(&vp);
    const unsigned long sig[6] = { d0, d1, got ? vp.x : 0UL, got ? vp.y : 0UL, got ? vp.w : 0UL, got ? vp.h : 0UL };
    if (memcmp(sig, g_gt.lastSig, sizeof(sig)) == 0)
        return;
    memcpy(g_gt.lastSig, sig, sizeof(sig));
    char top0[64];
    char top1[64];
    TopName(m0, top0, sizeof(top0));
    TopName(m1, top1, sizeof(top1));
    if (g_gt.sigLines++ < 40 && Budget())
        Log("[p2spawn] %lu ms: menu layer draw: root 0x%08X%s, Direct3D viewport %s x %lu y %lu w %lu h %lu (target %lu x "
            "%lu) as the game left it, manager 0 depth %lu top \"%s\", manager 1 depth %lu top \"%s\", %lu local player(s)",
            ScanMsSinceAttach(), static_cast<unsigned>(root),
            root == g_ifRoot ? " (= GetRoot's IF root)" : " (NOT the IF root found at start)", got ? "" : "(not read)", sig[2],
            sig[3], sig[4], sig[5], got ? vp.boxW : 0UL, got ? vp.boxH : 0UL, d0, top0, d1, top1,
            static_cast<unsigned long>(LocalPlayers()));
}

// ---- 18u (1): THE MENU INPUT FIX ----------------------------------------------------------------
// the IF input step (GOG 1.3 rva 0x00133590): args manager index, input buffer, player, dt; its
// countdown test first (+194h > 0: one frame less, no input)
const char* const kIfInputHead = "55 8B EC 83 EC 1C 56 8B 75 10 57 8B F9 8B 87 94 01 00 00 85 C0 7E 0F";
constexpr uint32_t kIfInputScan = 0x700;   // bytes of the function searched for the table's references
constexpr uint32_t kHeldSlot = 0x1A;       // bytes per player (imul eax, player, 1Ah)
constexpr int kHeldSlots = 4;              // the Xbox's table (default.xbe 0x00453050: 4 x 1Ah)
constexpr int kHeldRefsExpected = 25;      // GOG 1.3: 25 references, all in this function
constexpr int kHeldRefsMax = 40;
constexpr uint32_t kHeldLuaIndexOfs = 0x1C; // the Lua's current-manager index: right after the table (in slot 1)
// the write-back at its end: movups xmm0,[ebp-1Ch]; movups [eax+T],xmm0; xorps xmm0,xmm0;
// movq [eax+T+10h],xmm0; mov word [eax+T+18h],0
const char* const kHeldWriteBack = "0F 10 45 E4 0F 11 80 ?? ?? ?? ?? 0F 57 C0 66 0F D6 80 ?? ?? ?? ?? 66 C7 80 ?? ?? ?? ?? 00 00";
constexpr uint32_t kHeldWbT = 0x07;
constexpr uint32_t kHeldWbT10 = 0x12;
constexpr uint32_t kHeldWbT18 = 0x19;
// the five ways the step addresses the table ([eax + T + k], eax = player * 1Ah)
struct HeldForm
{
    uint8_t op[4];
    int opLen;
    int disp;       // where the disp32 starts
    uint8_t tail[2];
    int tailLen;
    const char* what;
};
const HeldForm kHeldForms[5] = {
    { { 0x80, 0xB8, 0, 0 }, 2, 2, { 0x00, 0 }, 1, "cmp byte" },
    { { 0xC6, 0x80, 0, 0 }, 2, 2, { 0x00, 0 }, 1, "mov byte" },
    { { 0x0F, 0x11, 0x80, 0 }, 3, 3, { 0, 0 }, 0, "movups" },
    { { 0x66, 0x0F, 0xD6, 0x80 }, 4, 4, { 0, 0 }, 0, "movq" },
    { { 0x66, 0xC7, 0x80, 0 }, 3, 3, { 0x00, 0x00 }, 2, "mov word" },
};

alignas(16) uint8_t g_held[kHeldSlots * kHeldSlot + 6] = {};  // BF1GameExt's table: 4 players
uint32_t g_ifInputRva = 0;
uintptr_t g_heldOld = 0;               // the game's table (one slot)
uint32_t g_heldRef[kHeldRefsMax] = {}; // rva of each reference's disp32
uint8_t g_heldK[kHeldRefsMax] = {};    // its byte in the slot
int g_heldRefs = 0;
bool g_heldWanted = true;              // [Splitscreen] P2MenuInputFix
bool g_heldReady = false;              // found and checked, not moved yet
bool g_heldMoved = false;
uint8_t g_heldLast[kHeldSlot] = {};
int g_heldLines = 0;

void HeldFind()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kIfInputHead, hits, 4);
    if (n != 1)
    {
        Log("[p2spawn] MENU INPUT FIX OFF: the IF input step - %d pattern hits (need exactly 1)", n);
        return;
    }
    const uint32_t fn = hits[0];
    uint32_t wb = 0;
    int wbHits = 0;
    for (uint32_t i = 0; i + 32 <= kIfInputScan; ++i)
        if (MatchPatternAt(fn + i, kHeldWriteBack))
        {
            wb = fn + i;
            ++wbHits;
        }
    if (wbHits != 1)
    {
        Log("[p2spawn] MENU INPUT FIX OFF: the IF input step rva 0x%08X - its write-back found %d times (need exactly 1)", fn,
            wbHits);
        return;
    }
    const BuildInfo* build = CurrentBuild();
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = lo + (build ? build->sizeOfImage : 0);
    const uintptr_t t = ReadU32(Rva(wb + kHeldWbT));
    const bool wbOk = ReadU32(Rva(wb + kHeldWbT10)) == t + 0x10 && ReadU32(Rva(wb + kHeldWbT18)) == t + 0x18;
    if (!wbOk || t < lo || t + 0x40 > hi)
    {
        Log("[p2spawn] MENU INPUT FIX OFF: the write-back at rva 0x%08X: table 0x%08X, its +10h / +18h stores %s, %s", wb,
            static_cast<unsigned>(t), wbOk ? "OK" : "do NOT match", (t >= lo && t + 0x40 <= hi) ? "in the exe" : "NOT in the exe");
        return;
    }
    const unsigned char* code = reinterpret_cast<const unsigned char*>(Rva(fn));
    int counts[5] = {};
    int count = 0;
    char list[600] = "";
    for (uint32_t i = 0; i + 10 <= kIfInputScan; ++i)
    {
        for (int f = 0; f < 5; ++f)
        {
            const HeldForm& h = kHeldForms[f];
            if (memcmp(code + i, h.op, static_cast<size_t>(h.opLen)) != 0)
                continue;
            uint32_t d = 0;
            memcpy(&d, code + i + h.disp, 4);
            if (d < t || d >= t + kHeldSlot)
                continue;
            if (h.tailLen && memcmp(code + i + h.disp + 4, h.tail, static_cast<size_t>(h.tailLen)) != 0)
                continue;
            if (count < kHeldRefsMax)
            {
                g_heldRef[count] = fn + i + static_cast<uint32_t>(h.disp);
                g_heldK[count] = static_cast<uint8_t>(d - t);
            }
            ++count;
            ++counts[f];
            char one[24];
            snprintf(one, sizeof(one), "%s+%X:%X", count > 1 ? " " : "", i, static_cast<unsigned>(d - t));
            if (strlen(list) + strlen(one) + 1 < sizeof(list))
                strcat_s(list, one);
        }
    }
    const bool luaInside = g_luaIndexAddr && g_luaIndexAddr == t + kHeldLuaIndexOfs;
    Log("[p2spawn] menu input: the IF input step rva 0x%08X: held-button table at rva 0x%08X (1 slot of %Xh bytes; "
        "player 2's slot would be +%Xh..+%Xh), write-back at +%Xh OK; %d reference(s) in the step - %d cmp byte, %d mov byte, "
        "%d movups, %d movq, %d mov word (GOG 1.3 has %d): %s",
        fn, RvaOf(t), kHeldSlot, kHeldSlot, 2 * kHeldSlot - 1, wb - fn, count, counts[0], counts[1], counts[2], counts[3],
        counts[4], kHeldRefsExpected, list);
    Log("[p2spawn] menu input: the Lua's current-manager index (rva 0x%08X) %s", RvaOf(g_luaIndexAddr),
        luaInside ? "IS table +1Ch - inside player 2's slot: a screen on manager 1 wrote over it every frame (the bug)"
                  : "is NOT table +1Ch (not as GOG 1.3 has it)");
    if (count != kHeldRefsExpected || !luaInside)
    {
        Log("[p2spawn] MENU INPUT FIX OFF: %s", count != kHeldRefsExpected ? "not exactly 25 references" : "the table is not where GOG 1.3 has it");
        return;
    }
    g_ifInputRva = fn;
    g_heldOld = t;
    g_heldRefs = count;
    g_heldReady = true;
}

// game thread (no input step running): player 1's slot copied, then the 25 disp32s moved, read back
void HeldMove()
{
    if (!g_heldReady || g_heldMoved || !g_heldWanted)
        return;
    g_heldReady = false;
    memcpy(g_held, reinterpret_cast<const void*>(g_heldOld), kHeldSlot);
    const uintptr_t newT = reinterpret_cast<uintptr_t>(g_held);
    void* const at = reinterpret_cast<void*>(Rva(g_ifInputRva));
    DWORD old = 0;
    if (!VirtualProtect(at, kIfInputScan, PAGE_EXECUTE_READWRITE, &old))
    {
        Log("[p2spawn] MENU INPUT FIX: VirtualProtect failed (error %lu) - nothing changed", GetLastError());
        return;
    }
    for (int i = 0; i < g_heldRefs; ++i)
    {
        const uint32_t v = static_cast<uint32_t>(newT + g_heldK[i]);
        memcpy(reinterpret_cast<void*>(Rva(g_heldRef[i])), &v, 4);
    }
    DWORD ignored = 0;
    VirtualProtect(at, kIfInputScan, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, kIfInputScan);
    int back = 0;
    for (int i = 0; i < g_heldRefs; ++i)
        if (ReadU32(Rva(g_heldRef[i])) == static_cast<uint32_t>(newT + g_heldK[i]))
            ++back;
    g_heldMoved = back == g_heldRefs;
    memcpy(g_heldLast, g_held + kHeldSlot, kHeldSlot);
    Log("[p2spawn] %lu ms: MENU INPUT FIX %s: the IF input step's %d references moved from rva 0x%08X (1 slot) to "
        "BF1GameExt's table at 0x%08X (%d slots of %Xh: player 2 has his own); %d of %d read back%s",
        ScanMsSinceAttach(), g_heldMoved ? "ON" : "FAILED", g_heldRefs, RvaOf(g_heldOld), static_cast<unsigned>(newT),
        kHeldSlots, kHeldSlot, back, g_heldRefs, g_heldMoved ? " - player 2's menus take his pad" : "");
}

// player 2's slot: logged when it changes while manager 1 has a screen up (his presses reaching it)
void HeldWatch()
{
    if (!g_heldMoved || g_heldLines >= 40)
        return;
    const uint8_t* slot = g_held + kHeldSlot;
    if (memcmp(slot, g_heldLast, kHeldSlot) == 0)
        return;
    memcpy(g_heldLast, slot, kHeldSlot);
    char top[64];
    TopName(Manager(1), top, sizeof(top));
    char bytes[3 * kHeldSlot + 1] = "";
    for (uint32_t i = 0; i < kHeldSlot; ++i)
        snprintf(bytes + 3 * i, 4, "%02X ", slot[i]);
    ++g_heldLines;
    if (Budget())
        Log("[p2spawn] %lu ms: menu input: player 2's held buttons %s(manager 1 top \"%s\")", ScanMsSinceAttach(), bytes, top);
}

// ---- 18v: THE INTERFACE'S PLAYER COUNT -------------------------------------------------------------
// the IF update +22h: mov ecx,esi; call PlayerCount (+24h: [input manager + 14h]); xor edi,edi;
// mov [ebp-4],eax - and +71h: cmp ebx,[ebp-4]; jge (a manager whose player is not below it: no input)
constexpr uint32_t kIfUpdateCountSiteAt = 0x22;
const char* const kIfUpdateCountSite = "8B CE E8 ?? ?? ?? ?? 33 FF 89 45 FC";
constexpr uint32_t kIfUpdateCountCall = 0x24;
const char* const kInputCountBody = "8B 41 14 C3";
constexpr uint32_t kIfUpdateCompareAt = 0x71;
const char* const kIfUpdateCompare = "3B 5D FC 7D";

typedef int(__fastcall* InputCount_t)(void* inputMgr, void* unusedEdx);
InputCount_t g_inputCount = nullptr; // the game's: mov eax, [ecx+14h]; ret
uint32_t g_ifUpdateRva = 0;
uint32_t g_countCallRva = 0;         // the E8 at IF update +24h
bool g_countReady = false;           // found and checked, not yet routed through BF1GameExt
bool g_countMoved = false;
int g_countState = -1;               // the last answer logged: 0 the game's own, 1 raised to 2
int g_countLines = 0;

// the IF update's player count (game thread, every frame): 2 in split screen while index 1 gets
// player 2's objects, so manager 1 gets player 2's input; the game's own count otherwise
int __fastcall IfInputCount(void* inputMgr, void* unusedEdx)
{
    const int n = g_inputCount(inputMgr, unusedEdx);
    const bool raise = n < 2 && LocalPlayers() >= 2 && Player2RedirectActive();
    const int state = raise ? 1 : 0;
    if (state != g_countState)
    {
        g_countState = state;
        if (g_countLines++ < 30 && Budget())
            Log("[p2spawn] %lu ms: menu input: the interface's player count %s (the game's own: %d)", ScanMsSinceAttach(),
                raise ? "is 2 - manager 1 (player 2's menus) gets player 2's input" : "is the game's own", n);
    }
    return raise ? 2 : n;
}

void CountFind(uint32_t ifUpdate)
{
    g_ifUpdateRva = ifUpdate;
    const bool siteOk = MatchPatternAt(ifUpdate + kIfUpdateCountSiteAt, kIfUpdateCountSite);
    const bool cmpOk = MatchPatternAt(ifUpdate + kIfUpdateCompareAt, kIfUpdateCompare);
    const uint32_t fn = siteOk ? CallTarget(ifUpdate + kIfUpdateCountCall) : 0;
    const bool bodyOk = fn && MatchPatternAt(fn, kInputCountBody);
    Log("[p2spawn] menu input: the IF update's player count at +24h %s -> rva 0x%08X %s ([input manager + 14h]), its "
        "compare at +71h %s",
        siteOk ? "OK" : "does NOT match", fn, bodyOk ? "OK" : "does NOT match", cmpOk ? "OK" : "does NOT match");
    if (!siteOk || !cmpOk || !bodyOk)
    {
        Log("[p2spawn] MENU INPUT FIX: the player count is NOT routed - player 2's menus stay without input");
        return;
    }
    g_inputCount = reinterpret_cast<InputCount_t>(Rva(fn));
    g_countCallRva = ifUpdate + kIfUpdateCountCall;
    g_countReady = true;
}

// game thread (the IF update not running), after HeldMove: the call at +24h to IfInputCount, read back
void CountMove()
{
    if (!g_countReady || g_countMoved || !g_heldMoved)
        return;
    g_countReady = false;
    unsigned char* const call = reinterpret_cast<unsigned char*>(Rva(g_countCallRva));
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&IfInputCount) - (reinterpret_cast<uintptr_t>(call) + 5));
    DWORD old = 0;
    if (call[0] != 0xE8 || !VirtualProtect(call + 1, 4, PAGE_EXECUTE_READWRITE, &old))
    {
        Log("[p2spawn] MENU INPUT FIX: the player count call could not be written (error %lu) - nothing changed", GetLastError());
        return;
    }
    memcpy(call + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(call + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    int32_t back = 0;
    memcpy(&back, call + 1, 4);
    g_countMoved = back == rel;
    Log("[p2spawn] %lu ms: MENU INPUT FIX %s: the IF update's player count (rva 0x%08X) now goes through BF1GameExt - 2 in "
        "split screen while player 2's input objects are wired (the game's own value otherwise)%s",
        ScanMsSinceAttach(), g_countMoved ? "ON" : "FAILED", g_countCallRva, g_countMoved ? "" : " - read back wrong");
}

// ---- 19a: THE PAUSE MENU'S RESPAWN KILLS THE PLAYER --------------------------------------------------
// ScriptCB_PlayerSuicide (GOG 1.3 rva 0x0017A220, a Lua C function): isnumber(L, 1) (+16h), the player
// = tonumber(L, 1) (+26h, an int); player <= 1; Character(player) (+3Fh) -> its unit (+B4h); if not
// multiplayer (+51h: a flag) the unit's primary vtable slot 1; else the network path (+6Ch)
const char* const kSuicideBody = "55 8B EC E8 ?? ?? ?? ?? 3D 39 03 F6 8F 75 67 56 8B 75 08 6A 01 56 E8 ?? ?? ?? ?? 83 C4 08 84 C0 "
                                 "74 53 57 6A 01 56 E8 ?? ?? ?? ?? 8B F8 83 C4 08 83 FF 01 77 3F 57 33 F6 E8";
constexpr uint32_t kSuicideIsNumCall = 0x16;
constexpr uint32_t kSuicideToNumCall = 0x26;
constexpr uint32_t kSuicideMpCall = 0x51;
const char* const kSuicideToNumBody = "55 8B EC 51 FF 75 0C FF 75 08 E8 ?? ?? ?? ?? D9 5D FC F3 0F 2C 45 FC 83 C4 08 8B E5 5D C3";
const char* const kSuicideMpBody = "80 3D ?? ?? ?? ?? 00 0F B6 05 ?? ?? ?? ?? 0F B6 0D ?? ?? ?? ?? 0F 45 C1 C3";
// the death function (ragdoll.cpp's kDeathBody / kDeathSetDead): the unit's +8 sub-object, vtable slot 1
const char* const kDeathBody =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC B8 00 00 00 56 57 8B F9 C7 "
    "45 F8 02 00 00 00 89 7D F4 83 A7 A8 00 00 00 F7 8D 87 48 0A 00 00 89 45 FC";
const char* const kDeathSetDead = "83 BF 5C 0A 00 00 05 74 0A 6A 03 8D 4F F8 E8";
constexpr uint32_t kDeathWindow = 0x350;
constexpr uint32_t kUnitSub = 0x8;
constexpr uint32_t kUnitState = 0xA64;

bool g_respawnWanted = true;  // 19a: [Splitscreen] P2RespawnDies
bool g_waitP1 = false;        // 20g-r6: [Splitscreen] SpawnWaitP1 (1: players 2-4 spawn only while player 1 is in the world)
bool g_f34MapWanted = true;   // 20g-r6: [Splitscreen] SpawnMapP34
bool g_pauseWanted = true;    // 20g-r6: [Splitscreen] PauseP34
bool g_pauseLookWanted = true; // 20g-r9: [Splitscreen] PauseLookP34
typedef int(__cdecl* LuaCFn_t)(void* L);
typedef int(__cdecl* LuaIsNum_t)(void* L, int idx);
typedef int(__cdecl* LuaToInt_t)(void* L, int idx);
typedef unsigned char(__cdecl* MpFlag_t)();
typedef void(__fastcall* Death_t)(void* sub, void* unusedEdx);
// 19f: ScriptCB_Unpause (a Lua C function): isnumber(L,1) ? tonumber : 0 -> SetPaused(player, 0) (+42h)
const char* const kUnpauseBody = "55 8B EC 56 8B 75 08 57 56 33 FF E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 75 1B 50 56 E8 ?? ?? ?? ?? 83 C4 08 84 C0 "
                                 "74 24 6A 01 56 E8 ?? ?? ?? ?? 83 C4 08 8B F8 E8 ?? ?? ?? ?? 3D";
constexpr uint32_t kUnpauseSetCall = 0x42;
const char* const kSetPausedBody = "55 8B EC 8B 45 08 FF 75 0C 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 E8 ?? ?? ?? ?? 5D C3";
typedef void(__cdecl* SetPaused_t)(int player, int paused);
SetPaused_t g_setPaused = nullptr;
LuaCFn_t g_origSuicide = nullptr;
LuaIsNum_t g_luaIsNum = nullptr;
LuaToInt_t g_luaToInt = nullptr;
MpFlag_t g_mpFlag = nullptr;
bool g_suicideWrapped = false;
int g_suicideLines = 0;

uint32_t FollowJumps(uint32_t rva)
{
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(rva)) == 0xE9; ++hop)
        rva = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
    return rva;
}

// the death function of this unit (its +8 sub-object's vtable slot 1), byte-checked; 0 if not it
constexpr uint32_t kUnitOff = 0x250; // the controllable a character drives, inside the EntitySoldier (ragdoll.cpp)

// 19d: the vtable locator's offset: where this vtable's sub-object sits in the complete object
uint32_t RttiOffset(uintptr_t obj)
{
    const uint32_t vt = ReadU32(obj);
    const uint32_t col = vt ? ReadU32(vt - 4) : 0;
    return col ? ReadU32(col + 4) : 0xFFFFFFFFu;
}

uintptr_t UnitDeathFn(uintptr_t unit, uintptr_t* soldierOut)
{
    *soldierOut = 0;
    const uint32_t off = unit ? RttiOffset(unit) : 0xFFFFFFFFu;
    const uintptr_t soldier = (off == kUnitOff) ? unit - kUnitOff : 0;
    const uintptr_t vt8 = soldier ? ReadU32(soldier + kUnitSub) : 0;
    const uintptr_t slot1 = vt8 ? ReadU32(vt8 + 4) : 0;
    const uintptr_t fromRagdoll = RagdollOrigDeathFn();
    uintptr_t fn = slot1;
    if (!InExe(fn))
        fn = fromRagdoll; // 19b: the ragdoll hook sits in the slot: the game's own behind it
    const uint32_t rva = InExe(fn) ? FollowJumps(static_cast<uint32_t>(fn - ExeBase())) : 0;
    const bool bodyOk = rva && MatchPatternAt(rva, kDeathBody);
    int n = 0;
    for (uint32_t i = 0; bodyOk && i + 16 <= kDeathWindow; ++i)
        if (MatchPatternAt(rva + i, kDeathSetDead))
            ++n;
    // 19c: what was found, each time (the checks failed in 19a / 19b)
    char type[64];
    unsigned char head[8] = {};
    if (rva)
        memcpy(head, reinterpret_cast<const void*>(Rva(rva)), sizeof(head));
    if (g_suicideLines < 40 && Budget())
        Log("[p2spawn] respawn: unit 0x%08X (%s, locator offset %Xh%s) -> soldier 0x%08X: +8 vtable 0x%08X (rva 0x%08X), its slot 1 "
            "0x%08X (%s), ragdoll.cpp's original 0x%08X -> checked rva 0x%08X: head %02X %02X %02X %02X %02X %02X %02X %02X, body %s, "
            "SetState(3) x%d",
            static_cast<unsigned>(unit), TypeOrDash(unit, type, sizeof(type)), static_cast<unsigned>(off), off == kUnitOff ? "" : " - NOT 250h",
            static_cast<unsigned>(soldier), static_cast<unsigned>(vt8), RvaOf(vt8), static_cast<unsigned>(slot1),
            InExe(slot1) ? "in the exe" : "NOT in the exe (a hook / none)", static_cast<unsigned>(fromRagdoll), rva, head[0], head[1],
            head[2], head[3], head[4], head[5], head[6], head[7], bodyOk ? "OK" : "does NOT match", n);
    if (!(bodyOk && n == 1))
        return 0;
    *soldierOut = soldier;
    // 19e: called through the slot as it is (ragdoll.cpp's hook, which calls the game's own, when it is in)
    return slot1 ? slot1 : Rva(rva);
}

int __cdecl SuicideWrap(void* L)
{
    const bool split = LocalPlayers() >= 2;
    const int player = (g_luaIsNum(L, 1) != 0) ? g_luaToInt(L, 1) : -1;
    const uintptr_t ch = (split && player >= 0 && player <= 1) ? SpawnScanCharacter(player) : 0;
    const uintptr_t unit = ch ? ReadU32(ch + kCharUnit) : 0;
    const bool mp = g_mpFlag() != 0;
    uintptr_t soldier = 0;
    const uintptr_t death = (unit && !mp) ? UnitDeathFn(unit, &soldier) : 0;
    if (!death)
    {
        if (g_suicideLines++ < 40 && Budget())
            Log("[p2spawn] %lu ms: respawn: ScriptCB_PlayerSuicide(%d) - the game's own (%s)", ScanMsSinceAttach(), player,
                !split ? "not split screen" : (!ch ? "no character" : (!unit ? "no unit" : (mp ? "multiplayer" : "death function does not match"))));
        return g_origSuicide(L);
    }
    const int32_t state = static_cast<int32_t>(ReadU32(soldier + kUnitState));
    if (g_suicideLines++ < 40 && Budget())
        Log("[p2spawn] %lu ms: respawn: ScriptCB_PlayerSuicide(%d) - player %d's soldier 0x%08X (state %ld) KILLED through its death "
            "function (%s 0x%08X) (not removed)",
            ScanMsSinceAttach(), player, player + 1, static_cast<unsigned>(soldier), static_cast<long>(state),
            InExe(death) ? "the game's, rva" : "ragdoll.cpp's hook at", InExe(death) ? RvaOf(death) : static_cast<unsigned>(death));
    reinterpret_cast<Death_t>(death)(reinterpret_cast<void*>(soldier + kUnitSub), nullptr);
    // 19f: and out of his pause menu at once (his half stays paused under it otherwise: he would only fall
    // when he left the menu himself)
    if (g_setPaused)
    {
        g_setPaused(player, 0);
        if (g_suicideLines++ < 40 && Budget())
            Log("[p2spawn] %lu ms: respawn: player %d unpaused (SetPaused(%d, 0)) - he dies now", ScanMsSinceAttach(), player + 1, player);
    }
    return 0;
}

void SuicideWrapInstall()
{
    if (!g_respawnWanted)
    {
        Log("[p2spawn] respawn: [Splitscreen] P2RespawnDies=0 - the pause menu's Respawn as the game has it");
        return;
    }
    const uintptr_t slot = ScanLuaFuncSlot("ScriptCB_PlayerSuicide");
    const uintptr_t fn = slot ? ReadU32(slot) : 0;
    const uint32_t rva = InExe(fn) ? static_cast<uint32_t>(fn - ExeBase()) : 0;
    const bool bodyOk = rva && MatchPatternAt(rva, kSuicideBody);
    const uint32_t isNum = bodyOk ? CallTarget(rva + kSuicideIsNumCall) : 0;
    const uint32_t toNum = bodyOk ? CallTarget(rva + kSuicideToNumCall) : 0;
    const uint32_t mpFn = bodyOk ? CallTarget(rva + kSuicideMpCall) : 0;
    const bool toNumOk = toNum && MatchPatternAt(toNum, kSuicideToNumBody);
    const bool mpOk = mpFn && MatchPatternAt(mpFn, kSuicideMpBody);
    Log("[p2spawn] respawn: ScriptCB_PlayerSuicide slot rva 0x%08X -> function rva 0x%08X %s; isnumber 0x%08X, tonumber 0x%08X %s, "
        "multiplayer flag 0x%08X %s",
        RvaOf(slot), rva, bodyOk ? "OK" : "does NOT match", isNum, toNum, toNumOk ? "OK" : "does NOT match", mpFn,
        mpOk ? "OK" : "does NOT match");
    if (!bodyOk || !toNumOk || !mpOk || !isNum)
    {
        Log("[p2spawn] respawn: NOT wrapped - the pause menu's Respawn as the game has it");
        return;
    }
    // 19f: SetPaused through the Lua ScriptCB_Unpause's registration slot
    {
        const uintptr_t uslot = ScanLuaFuncSlot("ScriptCB_Unpause");
        const uintptr_t ufn = uslot ? ReadU32(uslot) : 0;
        const uint32_t urva = InExe(ufn) ? static_cast<uint32_t>(ufn - ExeBase()) : 0;
        const bool uOk = urva && MatchPatternAt(urva, kUnpauseBody);
        const uint32_t sp = uOk ? CallTarget(urva + kUnpauseSetCall) : 0;
        const bool spOk = sp && MatchPatternAt(sp, kSetPausedBody);
        if (spOk)
            g_setPaused = reinterpret_cast<SetPaused_t>(Rva(sp));
        Log("[p2spawn] respawn: ScriptCB_Unpause rva 0x%08X %s -> SetPaused(player, paused) rva 0x%08X %s%s", urva,
            uOk ? "OK" : "does NOT match", sp, spOk ? "OK" : "does NOT match",
            spOk ? " - the Respawn leaves his pause menu at once" : " - the Respawn leaves the pause menu as the game does");
    }
    g_origSuicide = reinterpret_cast<LuaCFn_t>(fn);
    g_luaIsNum = reinterpret_cast<LuaIsNum_t>(Rva(isNum));
    g_luaToInt = reinterpret_cast<LuaToInt_t>(Rva(toNum));
    g_mpFlag = reinterpret_cast<MpFlag_t>(Rva(mpFn));
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, PAGE_READWRITE, &old))
    {
        Log("[p2spawn] respawn: the slot could not be written (error %lu) - not wrapped", GetLastError());
        return;
    }
    const uint32_t w = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&SuicideWrap));
    memcpy(reinterpret_cast<void*>(slot), &w, 4);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, old, &ignored);
    g_suicideWrapped = ReadU32(slot) == w;
    Log("[p2spawn] respawn %s: in split screen the pause menu's Respawn kills the player's unit through its death function "
        "(the +8 vtable slot 1 ragdoll.cpp uses, byte-checked at the call) instead of removing it; otherwise the game's own",
        g_suicideWrapped ? "ON" : "FAILED (read back wrong)");
}

// ---- 18u (2): PLAYER 2'S SPAWN SCREENS ------------------------------------------------------------
// the Spawn button's sequence (autospawn.cpp's pattern, GOG 1.3 rva 0x00078B2C): the three argument
// getters before it (-15h: post, class index, team), SetSpawnChoice (+07h), SetReady (+14h)
const char* const kSpawnPressed = "8B 8E 70 23 00 00 50 E8 ?? ?? ?? ?? 8B 8E 70 23 00 00 6A 01 E8 ?? ?? ?? ??";
constexpr uint32_t kSpawnArgsBack = 0x15;
const char* const kSpawnArgs = "E8 ?? ?? ?? ?? 50 8B CE E8 ?? ?? ?? ?? 50 8B CE E8";
constexpr uint32_t kSpawnChoiceCall = 0x07;
constexpr uint32_t kSpawnReadyCall = 0x14;
const char* const kSetSpawnChoiceBody =
    "55 8B EC 56 FF 75 08 8B F1 E8 ?? ?? ?? ?? FF 75 0C 8B CE E8 ?? ?? ?? ?? FF 75 10 8B CE E8 ?? ?? ?? ?? 5E 5D C2 0C 00";
const char* const kSetReadyBody = "55 8B EC 8A 45 08 88 81 CC 00 00 00 5D C2 04 00";
// the post getter (GOG 0x00077320): push [sd+23DCh]; push [sd+23E0h]; call GetSpawnManager;
// mov ecx,eax; call ValidPost(post, team); ret
const char* const kPostGetterBody = "FF B1 DC 23 00 00 FF B1 E0 23 00 00 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? C3";
constexpr uint32_t kPostGetterSmCall = 0x0C;
constexpr uint32_t kPostGetterValidCall = 0x13;
const char* const kGetSmBody = "A1 ?? ?? ?? ?? C3";
const char* const kValidPostBody = "55 8B EC 83 EC 14 56 8B 75 08 85 F6 0F 84";
// NextSpawnPoint (class_wheel.cpp 18g): the PC next-post block, its call at +0Dh; the cursor +A8h
const char* const kNextPostBlock = "FF B3 DC 23 00 00 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 50 8B CB E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
                                   "D8 05 ?? ?? ?? ?? D9 9B AC 23 00 00";
constexpr uint32_t kNextPostCall = 0x0D;
const char* const kNextPostBody = "55 8B EC 81 EC 08 01 00 00 56 8D 45 FC 89 4D F8 50 33 F6 E8";
const char* const kNextPostCursor = "8B 55 F8 33 C9 8B 82 A8 00 00 00 3B C6";
const char* const kNextPostCursorSet = "89 8A A8 00 00 00";
constexpr uint32_t kNextPostCursorAt = 0x75;
constexpr uint32_t kNextPostCursorSetAt = 0x90;
// SelectTeam's objective text (GOG 0x000789F4 = SelectTeam +74h): lea eax,[ebp-400h]; push team;
// push 400h; push eax; push 0; call GetMap; add esp,4; mov ecx,eax; call GetObjective(buf, bytes, team)
const char* const kSelectTeamBody = "55 8B EC 81 EC 00 04 00 00 56 8B 75 08 57 56 8B F9 E8";
constexpr uint32_t kObjSiteAt = 0x74;
const char* const kObjSite = "8D 85 00 FC FF FF 56 68 00 04 00 00 50 6A 00 E8 ?? ?? ?? ?? 83 C4 04 8B C8 E8";
constexpr uint32_t kObjCall = 0x8D;
const char* const kGetObjectiveBody = "55 8B EC 53 8B 5D 0C 56 FF 75 10 D1 EB E8";
// PopScreen +3Bh: push 0; call CursorShow (cdecl: the IF cursor element's SetVisible); PushScreen
// calls it with 1
constexpr uint32_t kPopCursorAt = 0x3B;
const char* const kPopCursor = "6A 00 E8";
const char* const kCursorShowBody = "55 8B EC FF 75 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kCursorElemImm = 0x07;
// characters, teams, classes, posts
constexpr uint32_t kCharIndex = 0x9C;
constexpr uint32_t kCharClassIdx = 0xA8;
constexpr uint32_t kCharClass = 0xAC;
constexpr uint32_t kCharSpawnPoint = 0xB0;
constexpr uint32_t kCharPost = 0xC8;
constexpr uint32_t kCharReady = 0xCC;
constexpr uint32_t kSmTeams = 0x18;
constexpr uint32_t kSmPostCursor = 0xA8;
constexpr uint32_t kTeamName = 0x0C;
constexpr uint32_t kTeamCount = 0x3C;
constexpr uint32_t kTeamClasses = 0x44;
constexpr uint32_t kMaxClasses = 64;
constexpr uint32_t kPostName = 0x1C;
constexpr uint32_t kClassName = 0x30;
constexpr uint32_t kWeaponCountSlot = 0x2C;
constexpr uint32_t kWeaponGetSlot = 0x30;
// pad 2: XINPUT_STATE at +C0h
constexpr uint32_t kPadButtons = 0xC4;
constexpr uint32_t kPadLX = 0xC8;
constexpr uint32_t kPadLY = 0xCA;
constexpr int kStickPush = 16000;
// the flow
enum FlowScreen
{
    kScrNone = 0,
    kScrSide = 1,
    kScrPost = 2,
    kScrClass = 3
};
const char* const kFlowScreens[4] = { nullptr, "ifs_readyselect2", "ifs_mapselect2", "ifs_charselect2" };
const char* const kFlowGroupPref[4] = { nullptr, "ifs_readyselect2.Info", "ifs_mapselect2.Info", "ifs_charselect2.Info" };
// 20g-r12: horizontal - his class screen's ring and blue grid go in its Helptext_Back group ("B BACK", anchored bottom
// left, never turned): its Info group stays turned there, as the Classic Collection's class window
const char* const kHzClassGroup = "ifs_charselect2.Helptext_Back";
// 19i: after a death his spawn point screen waits for his character's respawn timer (+C0h) to run out,
// as the game waits for player 1's (SpawnDisplay::Update, GOG 1.3 0x0007998B: Timer(ch) > 0 -> wait);
// kRespawnMaxWaitMs is the limit if the timer never does, kRespawnDelayMs the wait when the timer
// cannot be read (not a number, or beyond kRespawnTimerMax)
constexpr DWORD kRespawnDelayMs = 1500;
constexpr DWORD kRespawnMaxWaitMs = 30000;
constexpr float kRespawnTimerMax = 120.0f;
constexpr uint32_t kCharTimer = 0xC0;     // character: the respawn timer (float, seconds; <= 0 = may spawn)
constexpr DWORD kReadyLogAfterMs = 8000;
constexpr int kFlowCountdown = 2;         // held while our screen is manager 1's top
constexpr int kMaxPosts = 64;
constexpr int kFlowTexts = 10;
// where our own texts stand in his half (fractions of his HUD root, y down; the Xbox split screen)
// 19g: 19f's video - his root has no x offset (player 1's has 0.1), so what RootPlace puts at x of his
// root draws at x - 0.10 of his half in the groups anchored at its right edge (the side screen's
// title at 0.50 drew at 40%); y draws where it is put. The Xbox's: the title at 47% across, the names
// at 30% / 65% and 66% down, the soldiers at 29% / 65%, 32% of the height tall (his were 47% at size
// 0.75), their feet at 62% - 64%.
// 19i: the same look as player 1's side screen (19h-r2's video, both halves measured): his soldiers
// 0.324 of the height tall (size 0.54) centred at 36.5% / 66.7%, feet at 63%, the names at 34.4% /
// 69.8% and 65.7% down, the title centred at 52%. What is put at x of his half draws 0.105 further
// left (19g; his root has no x offset), feet 0.015 higher.
constexpr float kSideTitleX = 0.625f, kSideTitleY = 0.107f;
constexpr float kSideModelX0 = 0.47f, kSideModelX1 = 0.772f, kSideModelY = 0.645f; // the side soldiers' feet
constexpr float kSideModelSize = 0.54f;   // = player 1's split side screen (class_wheel.cpp's SideLookNow)
// 19g: the class ring over the window (its group is anchored top right: 19f's class soldier at 0.50
// drew at 53% across with its feet 4.5% lower than put, size 0.85 stood 38% of the height tall). The
// Xbox's: the front soldier's feet at 47%, the ring from 33% to 75% across, the front one 31% tall.
// 19h: his class screen's Info group is set straight (ClassWheelXbWindow; it was turned 42 degrees,
// which turned his soldiers too and moved them in depth), so the ring is placed in fractions of his
// half (ClassWheelFlatPlace). The Xbox's ring for player 2: its centre at 50% across, radius 16.5% of
// the half, the picked soldier 23.5% of the height tall with its feet at 46.6% (19g's: put at 0.47 by
// that mapping, its feet drew at 46.8%). About 0.62 of the height per size flat (player 1's split
// ring: 0.604 in his soldiers' group, whose units are 0.88 of these; 19g's turned group: 0.416).
// 19i: player 1's ring (19h-r2's video): its picked soldier 0.287 of the height tall (size 0.508), the
// ring's centre at 55.7% across with its radius 14.7% of the half, feet at 46.3%. His drew with 0.85 of
// a half per 512 units (0.165 put drew 0.14) and 0.03 right of where put (0.50 drew at 0.53), feet 0.01
// higher - so 0.527 / 0.173 / 0.475 here draw where player 1's are.
constexpr float kRingX = 0.527f, kRingY = 0.475f; // the ring's centre, at its soldiers' feet
constexpr float kRingRadius = 0.173f;             // of the half
constexpr float kRingSize = 0.508f;               // the picked soldier (of the game's soldier's size) = player 1's
constexpr float kRingLift = 0.0f;                 // feet level
// 19g's ring, kept for a group that is still turned (ClassWheelXbWindow left it as it was)
constexpr float kRingX19g = 0.54f, kRingY19g = 0.42f, kRingWidth19g = 110.0f, kRingSize19g = 0.70f, kRingLift19g = 15.0f;
constexpr float kGameSoldierFx = 1.0f;          // the game's own soldier stands at x 512, y 520 of a 512 x 624 root
constexpr float kGameSoldierFy = 520.0f / 624.0f;
// 18z / 19a / 19g: the panels (their group anchored at the right edge; in 19f they moved about 0.67 of
// a root unit per unit across and 0.47 down): centred at 47% across (46% in 19f), their top at about
// 48% of the height (45% in 19f)
constexpr float kXbPanelX = -366.0f, kXbPostY = 305.0f, kXbVehY = 384.0f, kXbObjY = 529.0f;
constexpr float kXbWindowX = -364.0f, kXbWindowY = 375.0f; // the class window centred, its title at about 47% (41%)
// 19g: "B BACK" in and up to the Xbox's place (its group pinned; the script leaves it at 0, -15)
constexpr float kXbBackX = 53.0f, kXbBackY = -41.0f;
constexpr uint32_t kMapGroup = 0x30;   // the map's drawn element
constexpr float kSideNameX0 = 0.449f, kSideNameX1 = 0.803f, kSideNameY = 0.65f; // 19i: under player 1's places (drawn 0.007 lower, 0.105 left)
// 18w / 19g: the prompts on the "B BACK" row (Helptext_Back, anchored bottom left; RootPlace takes the
// group's pinned place into account): the Xbox's at 65% across, 89% down (19f's fy 1.0 drew at 96%)
constexpr float kPromptX = 0.637f, kPromptY = 0.932f;
constexpr float kTextScale = 1.0f;
constexpr int kInfoLayer = 129;        // 19g: his screens' Info group (the grid in it) drawn before his HUD's groups
// 20g-r7: 4-PLAYER - his root is a quarter (512 x 312, as players 3 / 4's): his side, spawn point and class screens take
// the places and sizes of theirs (F34's values below, in fractions of the root): the game's panels / class window
// (made for a half) hidden, our texts for the post, its count and vehicles and the class name; his map in their map
// layout (layout 1, F34's values). [Splitscreen] MenusLikeP34=1.
constexpr float kQTitleY = 0.10f, kQTitleScale = 0.80f, kQPromptY = 0.93f, kQPromptScale = 0.55f;
constexpr float kQSideX[2] = { 0.30f, 0.70f };
constexpr float kQSideFeetY = 0.74f, kQSideNameY = 0.83f, kQSideSize = 0.34f, kQNameScale = 0.60f;
constexpr float kQPostY = 0.40f, kQPostScale = 0.75f, kQCountY = 0.50f, kQSmallScale = 0.55f, kQVehY = 0.60f, kQVehScale = 0.40f;
constexpr float kQMapTextX = 0.72f;
constexpr float kQMapArrowX[2] = { 0.50f, 0.94f };
constexpr float kQArrowX[2] = { 0.08f, 0.92f };
constexpr float kQClassArrowY = 0.60f;
// 20g-r9: Battlefront II's 4-player wheel (players 3 / 4's ring): the picked soldier in the middle, feet 2/3 down
constexpr float kQRingRadius = 0.26f, kQRingX = 0.50f - kQRingRadius * 0.53f, kQRingY = 0.67f, kQRingSize = 0.40f;
constexpr float kGameSoldierFyQ = 520.0f / 312.0f; // 20g-r9: the game's soldier's y (520 root units) in a quarter's root
// 21j-r9: with the console windows on (SpawnPanels34) a quarter's ring stands left of the class window: radius 0.22 of the
// quarter's width, the picked soldier at 0.38 across - the ring's right-hand soldiers end before the window's left edge (0.535)
constexpr float kPwRingRadius = 0.22f, kPwRingX = 0.38f - kPwRingRadius * 0.53f;
bool PwReady();          // 21j-r9 (players 3 / 4's section): the console windows can be made
bool Side12PwWill();     // 21j-r9: player 2's spawn point / class screens get them (players 3 / 4's look, Side12Frame)
bool g_quarterWanted = true; // [Splitscreen] MenusLikeP34

bool P2Quarter()
{
    return g_quarterWanted && PlayerNLocalPlayers() >= 3;
}

int P2MapLayoutQ();      // (players 3 / 4's section): 4-player - his map in their layout (lent), its number
void P2MapLayoutQOff(); // ... given back

typedef void(__fastcall* SetSpawnChoice_t)(void* ch, void* unusedEdx, int team, int classIndex, uintptr_t post);
typedef void(__fastcall* SetReady_t)(void* ch, void* unusedEdx, int ready);
typedef uintptr_t(__cdecl* GetSm_t)();
typedef uintptr_t(__fastcall* ValidPost_t)(void* sm, void* unusedEdx, uintptr_t post, int team);
typedef uintptr_t(__fastcall* NextPost_t)(void* sm, void* unusedEdx, int team);
typedef void(__fastcall* GetObjective_t)(void* map, void* unusedEdx, wchar_t* buf, int bytes, int team);
typedef void(__cdecl* CursorShow_t)(int on);
typedef void(__fastcall* MapSetMode_t)(void* map, void* unusedEdx, int mode);
// 19g: MapDisplay::Update(frame time) - its vtable slot 1 (GOG 1.3 rva 0x00188B00): hides its element
// (+30h) first and shows it again only at its end; the game calls it for player 1's map alone
typedef bool(__fastcall* MapUpdate_t)(void* map, void* unusedEdx, float dt);
const char* const kMapUpdateBody = "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC B8 00 00 00 56 8B F1 "
                                   "57 6A 00 8D 4E 30 E8 ?? ?? ?? ?? 80 7E 20 00 0F 84";
constexpr uint32_t kMapVtUpdate = 0x04;   // its vtable slot
constexpr uint32_t kMapEnabled = 0x20;    // byte: 0 = Update does nothing (element left hidden)
constexpr uint32_t kMapTrack = 0x3F4;     // what it follows
constexpr uint32_t kMapHasTrack = 0x3F8;  // byte: 0 = Update ends early (element left hidden)
uint32_t g_mapUpdateRva = 0;
// 19h-r2: player 2's map's vtable, copied with its Update slot ours, while his spawn point screen is up.
// 19h's log: his map was set up and shown, yet never drew - the game calls his map's Update itself
// every frame (its HUD display loop), and with his flag (+1F289h) clear and no unit that call drops
// the track and leaves the element hidden; ours ran before it. Through the copy every call runs with
// the flag set (and his layout lent to 5). The copy: [RTTI locator][slot 0 ..] - the game's 5 slots
// (GOG 1.3 MapDisplay: deleting destructor, Update, AddToLayer, RemoveFromLayer, InLayer), only those
// that are code in the exe. The pointer in his map object is put back when the screen goes.
constexpr int kMapVtMax = 8;
uintptr_t g_mapVtCopy[kMapVtMax + 1] = {};
int g_mapVtSlots = 0;
uintptr_t g_mapVtOrig = 0;     // the game's vtable (VA) the copy was made from
int g_mapWrapGame = 0;         // the wrapper's calls from the game (not MapUpdateP2) since his screen came up
int g_mapWrapGameSeen = 0;     // ... as of the last MapUpdateP2
int g_mapWrapDepth = 0;        // (the original Update must not come back through the slot)
bool MapWrapInstall(uintptr_t map);               // defined with MapUpdateP2
void MapWrapRemove(uintptr_t map, const char* why);
// 19i: MapDisplay::SetFlag(byte) (GOG 1.3 rva 0x00188620): the spawn screen flag +1F289h, the picture and
// raster resized to the layout the flag picks, the layout re-applied (ApplyLayout 0x00188110: the dark
// quad +1F400h and the icons group +1F290h are shown only in mode 1 WITHOUT the flag - the full-map
// look), then SetMode(the mode as it is). The spawn screen calls it with 1 for player 1's map when it
// opens (0x00079419) and 0 when it closes (0x000777D7).
typedef void(__fastcall* MapSetFlag_t)(void* map, void* unusedEdx, int flag);
const char* const kMapSetFlagBody = "55 8B EC 8A 45 08 53 8B D9 56 57 3A 83 89 F2 01 00 74 06 FF 05 ?? ?? ?? ?? 88 83 89 F2 01 00 "
                                    "E8 ?? ?? ?? ?? 8B F0 E8";
MapSetFlag_t g_mapSetFlag = nullptr;
// 19i: the layout picker (GOG 1.3 rva 0x001873C0, thiscall, no arguments): 2 players: the flag -> 5
// (player 1's spawn screen layout), else mode 1 -> 4, mode 0 -> 3, else 6. Its 7 callers (Update,
// ApplyLayout, SetMode, SetFlag, the draw) all call one jump thunk (E9); that thunk's target becomes
// MapPickWrap: his map with the flag set -> 4 (his own layout, the Xbox's place), anything else the
// game's own answer. Found from Update's call at +3C8h, the picker byte-checked.
typedef int(__fastcall* MapPick_t)(void* map, void* unusedEdx);
constexpr uint32_t kMapUpdatePickAt = 0x3C8; // call thunk (E8) inside MapDisplay::Update
const char* const kMapPickBody = "53 56 8B F1 8A 9E 89 F2 01 00 E8 ?? ?? ?? ?? 83 F8 01 7E 2B 84 DB 74 08 5E B8 05 00 00 00 5B C3 "
                                 "8B 86 8C F2 01 00 83 F8 01 75 08 5E B8 04 00 00 00 5B C3 85 C0 75 29 5E B8 03 00 00 00 5B C3";
MapPick_t g_mapPick = nullptr;      // the game's picker
uintptr_t g_mapPickThunk = 0;       // the jump thunk (VA) now going to MapPickWrap
constexpr uint32_t kMapIconsGroup = 0x1F290;
constexpr uint32_t kMapDarkQuad = 0x1F400;
constexpr uint32_t kMapExtraGroup = 0x1F510; // shown in mode 1 (ApplyLayout) - what it holds is logged (the map dump)
constexpr uint32_t kMapVec = 0x1F740;
constexpr uint32_t kSdPost = 0x23E0;    // the spawn screen's post (the map highlights ValidPost(post, team); kSdTeamShown its team)
// 19i: the IF manager's tick (GOG 1.3 rva 0x00134D60: thiscall(dt), the IF update's second loop calls it
// for both managers): queues an update event (type 7, dt) and drains the manager's event list - a queued
// PushScreen / PopScreen (Exit, SetScreen: the old screen's elements off the root, Enter: the Lua Enter
// and the new screen's elements onto the root) runs in that one pass. Called here with dt 0 right after
// our own PushScreen / PopScreen so the switch is done before the frame is drawn.
typedef void(__fastcall* ManagerTick_t)(void* mgr, void* unusedEdx, float dt);
constexpr uint32_t kIfUpdateTickAt = 0xE7;
const char* const kIfUpdateTickSite = "E8 ?? ?? ?? ?? 81 C6 44 02 00 00 81 FE 88 04 00 00 7C DA 5E 8B E5 5D C3";
const char* const kManagerTickBody = "55 8B EC 51 56 8B F1 57 8B 3D ?? ?? ?? ?? 6A 4C 8B 86 90 01 00 00 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
                                     "83 C4 04 85 C0 74 09 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 F3 0F 10 45 08 8B CE C7 00 07 00 00 00 "
                                     "89 45 FC F3 0F 11 40 04 8D 45 FC 50 E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 89 3D ?? ?? ?? ?? 5F 5E "
                                     "8B E5 5D C2 04 00";
ManagerTick_t g_managerTick = nullptr;
bool g_switchNowWanted = true;  // [Splitscreen] SpawnScreenSwitchNow
int g_settleLines = 0;

SetSpawnChoice_t g_setSpawnChoice = nullptr;
SetReady_t g_setReady = nullptr;
GetSm_t g_getSm = nullptr;
ValidPost_t g_validPost = nullptr;
NextPost_t g_nextPost = nullptr;
GetObjective_t g_getObjective = nullptr;
CursorShow_t g_cursorShow = nullptr;
uintptr_t g_cursorElem = 0;
MapSetMode_t g_mapSetMode = nullptr;
bool g_flowWanted = true;     // [Splitscreen] P2SpawnScreen
bool g_mapWanted = true;      // [Splitscreen] P2SpawnMap
bool g_soldiersWanted = true; // 18x: [Splitscreen] P2SpawnSoldiers
bool g_backdropWanted = true; // 19g: [Splitscreen] P2SpawnBackdrop
bool g_flowOk = false;        // everything the flow needs was found
bool g_textsOk = false;       // class_wheel.cpp's texts are there
volatile LONG g_flowOwns = 0; // autospawn.cpp stands aside while 1

struct FlowText
{
    uintptr_t e = 0;
    const char* screen = nullptr; // the screen it is in
    uintptr_t rec = 0;            // that screen's record when the text was made (it must still be the one in the list)
    const char* panel = nullptr;  // the registry path of its group (null: a screen's top-level group)
    uintptr_t group = 0;
};

struct PadEdges
{
    bool aDown = false, bDown = false;
    bool sDown = false; // 20g-r6: Start
    bool lDown = false, rDown = false, uDown = false, dDown = false;
    DWORD lSince = 0, rSince = 0, uSince = 0, dSince = 0;
    DWORD lLast = 0, rLast = 0, uLast = 0, dLast = 0;
};

// 21h-r8: the end of the match (defined with the match end scan)
bool EndOver();
int EndResult(uintptr_t ch);
bool VdP34Make(int k, int root);          // 21h-r9
void VdP34Show(int k, int win, bool on);  // 21h-r9
void VdP34Drop(int k, const char* why);   // 21h-r9
void __fastcall VdShowWrap(void* self, void* edx, uint32_t mask, float secs);

struct Flow
{
    uintptr_t sm = 0;          // the match this belongs to
    bool matchReady = false;   // the spawn screen was set up in this match (not loading any more)
    bool loaded = false;       // the three screens are in the screen list (checked once a second)
    DWORD loadedAt = 0;
    int screen = kScrNone;     // our screen on manager 1 (kScrNone = none of ours)
    bool wasTop = false;       // it was manager 1's top last frame
    bool sideDone = false;     // a side was chosen this match: later spawn screens start at the spawn point
    int pick = 0;              // side screen: 0 = team 1, 1 = team 2
    int team = 0;              // the side chosen (1 / 2)
    uintptr_t post = 0;        // the post chosen
    int cls = 0;               // the class index chosen
    int ready = 0;             // 0 choosing, 1 A pressed - waiting for player 1, 2 SetReady(1) done
    DWORD readyAt = 0;
    bool readyLogged = false;
    bool inWorld = false;      // character 1 had a unit last frame
    DWORD leftWorldAt = 0;     // when he lost it (0: match start)
    PadEdges pad;
    FlowText t[kFlowTexts];
    int n = 0;
    int textsFor = kScrNone;   // the screen our texts were made for
    uintptr_t sideName[2] = {};
    uintptr_t sideModel[2] = {}; // 18x: the side screen's two soldiers
    bool ringShown = false;      // 19g: his class ring is up (class_wheel.cpp's ring 2)
    uintptr_t sideRec = 0;       // 19g: the record / group the side soldiers were made in (deleted only while
    uintptr_t sideGroup = 0;     //      both are still the screen's, as the texts)
    const char* sidePanel = nullptr;
    uintptr_t ringRec = 0;       // 19g: the same for his class ring
    uintptr_t ringGroup = 0;
    const char* ringPanel = nullptr; // 20g-r12: the registry path of its group (horizontal: his class screen's Helptext_Back)
    uintptr_t cube = 0;          // 19g: the blue grid's copy in the screen's Info group (one of t[])
    bool mapHidden = false;      // 18x: his map element hidden (side / class screen)
    uintptr_t prompt = 0;
    uintptr_t postText = 0;
    uintptr_t countText = 0;   // 20g-r7: 4-player: "2 / 5" (as players 3 / 4's)
    uintptr_t classText = 0;   // 20g-r7: 4-player: the class name (the class window is hidden)
    uintptr_t vehText = 0;     // 19h: his VEHICLES panel's text (ClassWheelVehiclesAt)
    uintptr_t vehPost = 0;     // 19h: the post it was filled for
    uintptr_t hints = 0;       // ifs_mapselect2.SideBar.Hints, hidden while the spawn point screen is up
    uintptr_t shownPost = 0;
    int shownCls = -1;
    int shownTeam = 0;
    int shownPick = -1;
    bool waitShown = false;
    uintptr_t map = 0;         // player 2's map, in its spawn screen mode while the spawn point screen is up
    uint32_t mapSaved = 0;
    bool mapSet = false;
    int mapLayerSaved = -1;    // 19h: his map element's layer before it went to 1 (as the spawn screen does player 1's)
    bool mapWrapped = false;   // 19h-r2: his map's vtable is the copy (its Update ours)
    bool mapWrapLogged = false;
    bool mapDumped = false;    // 19h: both maps dumped once this match
    bool mapFlagSet = false;   // 19i: his map has the spawn screen flag (SetFlag(1); the picker gives it layout 4)
    bool timerLogged = false;  // 19i: his respawn timer logged once per death
    float timerSeen = 0.0f;    // 19i: his respawn timer's last value, and when it last changed (a timer that does not
    DWORD timerSeenAt = 0;     //      run - not his, not this build's - falls back to 19h's 1.5 s)
    bool timerRuns = false;    // 19i: it was > 0 when he left the world (else 19h's 1.5 s)
    LARGE_INTEGER mapLast = {};  // 19g: his map's last Update (its frame time)
    int mapUpdates = 0;          // 19g: Updates since his spawn point screen came up (the first few logged)
    bool smDiffLogged = false;
    bool padWas = true;        // pad 2 was connected (logged when it changes)
    long sdDeletes = 0;        // spawn screens deleted when this match began (one per match end)
    int textTries = 0;         // tries to make the texts of the screen that is up (its groups may come a frame late)
    int layoutFor = 0;         // 19h: the screen whose flat layout is done (kScrNone: not yet)
    bool sideLook = false;     // 20g-r10: 4-player - his side screen drawn in players 3 / 4's look (Side12Frame), none of ours
    bool pwLook = false;       // 21j-r9: 4-player - his spawn point / class screen's console windows drawn by Side12Frame, none of ours
    bool pwBack = false;       // 21j-r9: they could not be drawn (Side12FlowBack) - his texts made again, r9's, in his own frame
    int layoutTries = 0;       // 19h: frames it was tried on
    bool layoutPinned = false; // 19h-r2: the screen's groups pinned (19g's places)
    DWORD postsAt = 0;         // when the spawn point list was last read
    int lines = 0;
};
Flow g_flow;

bool FlowBudget()
{
    return g_flow.lines++ < 600 && Budget();
}

bool InCode(uintptr_t va)
{
    uint32_t rva = 0;
    uint32_t size = 0;
    return va >= ExeBase() && CodeSection(&rva, &size) && va - ExeBase() >= rva && va - ExeBase() < rva + size;
}

// wide text (with its 0) at va, at most cap - 1 characters
bool ReadWideAt(uintptr_t va, wchar_t* out, size_t cap)
{
    out[0] = 0;
    if (va < 0x10000 || cap < 2)
        return false;
    wchar_t buf[128] = {};
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), buf, sizeof(buf) - sizeof(wchar_t), &got) &&
        !ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), buf, 32 * sizeof(wchar_t), &got))
        return false;
    const size_t max = got / sizeof(wchar_t);
    size_t n = 0;
    while (n < max && buf[n])
        ++n;
    if (!n || n >= max)
        return false;
    if (n >= cap)
        n = cap - 1;
    memcpy(out, buf, n * sizeof(wchar_t));
    out[n] = 0;
    return true;
}

// for the log: printable ASCII as it is, a line break as " / ", anything else as '?'; always ends in 0
const char* Narrow(const wchar_t* w, char* out, size_t cap)
{
    size_t n = 0;
    for (size_t i = 0; w && w[i] && n + 4 < cap; ++i)
    {
        if (w[i] == L'\n')
        {
            out[n++] = ' ';
            out[n++] = '/';
            out[n++] = ' ';
        }
        else
            out[n++] = (w[i] >= 0x20 && w[i] < 0x7F) ? static_cast<char>(w[i]) : '?';
    }
    out[n] = 0;
    return out;
}

// ---- pad 2 -------------------------------------------------------------------------------------
struct PadIn
{
    bool left = false, right = false, up = false, down = false, a = false, b = false;
    bool start = false; // 20g-r6
};

// 20f-r2: any pad of the game's class (pad 2, pads 3 / 4): the d-pad / left stick, A, B
PadIn PadReadAt(uintptr_t pad)
{
    PadIn in;
    if (!pad)
        return in;
    uint16_t buttons = 0;
    int16_t lx = 0;
    int16_t ly = 0;
    memcpy(&buttons, reinterpret_cast<const void*>(pad + kPadButtons), 2);
    memcpy(&lx, reinterpret_cast<const void*>(pad + kPadLX), 2);
    memcpy(&ly, reinterpret_cast<const void*>(pad + kPadLY), 2);
    in.up = (buttons & 0x0001) != 0 || ly > kStickPush;
    in.down = (buttons & 0x0002) != 0 || ly < -kStickPush;
    in.left = (buttons & 0x0004) != 0 || lx < -kStickPush;
    in.right = (buttons & 0x0008) != 0 || lx > kStickPush;
    in.a = (buttons & 0x1000) != 0;
    in.b = (buttons & 0x2000) != 0;
    in.start = (buttons & 0x0010) != 0; // 20g-r6
    return in;
}

PadIn Pad2Read()
{
    const uintptr_t pad = Pad2Address();
    return (pad && Pad2IsConnected()) ? PadReadAt(pad) : PadIn();
}

bool PressEdge(bool now, bool& down)
{
    const bool fire = now && !down;
    down = now;
    return fire;
}

// once per press, then repeating while held
bool StepEdge(bool now, bool& down, DWORD& since, DWORD& last)
{
    const DWORD t = GetTickCount();
    if (!now)
    {
        down = false;
        return false;
    }
    if (!down)
    {
        down = true;
        since = t;
        last = t;
        return true;
    }
    if (t - since > 450 && t - last > 180)
    {
        last = t;
        return true;
    }
    return false;
}

// buttons already held when a screen comes up (or comes back on top) do not count as presses
void PadLatch()
{
    const PadIn in = Pad2Read();
    g_flow.pad.aDown = in.a;
    g_flow.pad.bDown = in.b;
    g_flow.pad.lDown = in.left;
    g_flow.pad.rDown = in.right;
    g_flow.pad.uDown = in.up;
    g_flow.pad.dDown = in.down;
    const DWORD t = GetTickCount();
    g_flow.pad.lSince = g_flow.pad.rSince = g_flow.pad.uSince = g_flow.pad.dSince = t;
    g_flow.pad.lLast = g_flow.pad.rLast = g_flow.pad.uLast = g_flow.pad.dLast = t;
}

// ---- the game's data -----------------------------------------------------------------------------
uintptr_t TeamObject(uintptr_t sm, int team)
{
    return (sm && team >= 1 && team <= 7) ? ReadU32(sm + kSmTeams + 4 * static_cast<uint32_t>(team)) : 0;
}

// the classes the PC cycle goes through (count - the extra ones), 0 if none
int TeamCycle(uintptr_t sm, int team)
{
    const uintptr_t to = TeamObject(sm, team);
    if (!to)
        return 0;
    const uint32_t count = ReadU32(to + kTeamCount);
    const uint32_t extra = ReadU32(to + kTeamCount + 4);
    if (count > kMaxClasses || extra > count || !ReadU32(to + kTeamClasses))
        return 0;
    return static_cast<int>(count - extra);
}

uintptr_t TeamClass(uintptr_t sm, int team, int idx)
{
    const uintptr_t to = TeamObject(sm, team);
    const uintptr_t arr = to ? ReadU32(to + kTeamClasses) : 0;
    return (arr && idx >= 0 && static_cast<uint32_t>(idx) < kMaxClasses) ? ReadU32(arr + 4 * static_cast<uint32_t>(idx)) : 0;
}

void TeamNameW(uintptr_t sm, int team, wchar_t* out, size_t cap)
{
    const uintptr_t to = TeamObject(sm, team);
    if (!to || !ReadWideAt(ReadU32(to + kTeamName), out, cap))
        swprintf(out, cap, L"SIDE %d", team);
}

// the posts his side can spawn at, in the game's order: NextSpawnPoint round once from cursor 0,
// then the cursor put back as it was (player 1's PC next-post input carries on from it)
int TeamPosts(uintptr_t sm, int team, uintptr_t* posts, int cap)
{
    if (!sm || !g_nextPost)
        return 0;
    volatile int32_t* cursor = reinterpret_cast<volatile int32_t*>(sm + kSmPostCursor);
    const int32_t saved = *cursor;
    int n = 0;
    *cursor = 0;
    while (n < cap)
    {
        const uintptr_t p = g_nextPost(reinterpret_cast<void*>(sm), nullptr, team);
        if (!p || (n > 0 && p == posts[0]))
            break;
        posts[n++] = p;
    }
    *cursor = saved;
    return n;
}

void PostNameW(uintptr_t post, wchar_t* out, size_t cap)
{
    if (!post || !ReadWideAt(ReadU32(post + kPostName), out, cap))
        out[0] = 0;
}

// a class's name and its EQUIPMENT list (player 1's soldier select's texts)
bool ClassTextsW(uintptr_t cls, wchar_t* name, size_t nameCap, wchar_t* box, size_t boxCap, int* weapons)
{
    *weapons = 0;
    char type[64];
    if (!cls || !TypeName(cls, type, sizeof(type)) ||
        (strcmp(type, "EntitySoldierClass") != 0 && strcmp(type, "EntityDroidekaClass") != 0))
        return false;
    if (!ReadWideAt(ReadU32(cls + kClassName), name, nameCap))
        wcscpy_s(name, nameCap, L"?");
    wcscpy_s(box, boxCap, L"EQUIPMENT:");
    const uintptr_t vt = ReadU32(cls);
    const uintptr_t fnCount = vt ? ReadU32(vt + kWeaponCountSlot) : 0;
    const uintptr_t fnGet = vt ? ReadU32(vt + kWeaponGetSlot) : 0;
    if (!InCode(fnCount) || !InCode(fnGet))
        return true;
    typedef int(__fastcall * WeaponCount_t)(void* self, void* unusedEdx);
    typedef uintptr_t(__fastcall * WeaponGet_t)(void* self, void* unusedEdx, int k);
    int n = reinterpret_cast<WeaponCount_t>(fnCount)(reinterpret_cast<void*>(cls), nullptr);
    if (n < 0 || n > 16)
        n = 0;
    for (int k = 0; k < n; ++k)
    {
        const uintptr_t w = reinterpret_cast<WeaponGet_t>(fnGet)(reinterpret_cast<void*>(cls), nullptr, k);
        wchar_t wn[96];
        if (!w || !ReadWideAt(ReadU32(w + kClassName), wn, 96))
            continue;
        if (wcslen(box) + wcslen(wn) + 2 >= boxCap)
            break;
        wcscat_s(box, boxCap, L"\n");
        wcscat_s(box, boxCap, wn);
        ++*weapons;
    }
    return true;
}

bool PlayerInWorld(int k)
{
    const uintptr_t ch = SpawnScanCharacter(k);
    return ch && ReadU32(ch + kCharUnit) != 0;
}

// ---- 20g-r12: HORIZONTAL 2-PLAYER (the Classic Collection's) ------------------------------------------------------------
// A HUD root draws a child of it at (flag set: offset + scale x anchor, else the anchor) + (its place + a place in it) /
// the root's size - the anchor goes through the root's safe area, a place in units never does (the root's draw, GOG 1.3
// 0x00292897: anchor * [+10h] + [+8], anchor * [+14h] + [+0Ch]). The safe area is AddViewport's (GOG 1.3 0x0028C440): a
// margin of 5% of the SCREEN, clipped at the view's edges and then 0.01 in from an edge inside the screen - in the
// horizontal split player 1's half has 10% at its top and 2% at its bottom (offset 0.10, scale 0.88 down) and player 2's
// 2% / 10% (0.02, 0.88): his score bar on the split line. The Classic Collection's video: both halves alike, 10% at the
// top AND the bottom (0.10, 0.80 down; across 0.05 / 0.90 as ours) - with those, the game's own spawn screens ("B BACK",
// the spawn point map in its layout, the turned panels and class window) stand where Classic has them.
constexpr int kHudRoots = 4;           // the game makes 4 HUD roots (roots[0..3])
constexpr float kHzOffY = 0.10f;       // Classic's safe area down: offset, scale
constexpr float kHzScaleY = 0.80f;
bool g_hzSafeWanted = true;            // [Splitscreen] HorizontalSafeArea
bool g_hzMenusWanted = true;           // [Splitscreen] MenusHorizontal (20g-r13: read here too - player 3's half)
int g_hzLines = 0;
long g_hzFixes[kHudRoots] = {}; // 20g-r13: each root's safe area writes (the game writes its own again at each view set-up)

bool HzBudget()
{
    return g_hzLines++ < 60;
}

bool RootUsable(uintptr_t root, float* w, float* h)
{
    if (!root)
        return false;
    *w = static_cast<float>(static_cast<int32_t>(ReadU32(root)));
    *h = static_cast<float>(static_cast<int32_t>(ReadU32(root + 4)));
    return *w >= 64.0f && *h >= 64.0f && *w <= 8192.0f && *h <= 8192.0f;
}

// where `group`'s (0, 0) draws, in fractions of its root's view, and the root's size (its units); false if it is not in
// one of the HUD roots (or in a straight, unscaled group that is, up to 3 deep)
bool GroupFracDepth(uintptr_t group, float* bx, float* by, float* w, float* h, int depth);

bool GroupFrac(uintptr_t group, float* bx, float* by, float* w, float* h)
{
    return GroupFracDepth(group, bx, by, w, h, 0);
}

bool GroupFracDepth(uintptr_t group, float* bx, float* by, float* w, float* h, int depth)
{
    const uintptr_t owner = group ? ReadU32(group + kElemNodeOwner) : 0;
    if (!owner)
        return false;
    // a group in a group: its parent's place plus its own, when the parent is straight and unscaled (its rows identity)
    const uintptr_t parent = owner - kElemChildren;
    bool inRoot = false;
    for (int k = 0; k < kHudRoots && !inRoot; ++k)
        inRoot = RootAt(k) && owner == RootAt(k) + kRootList;
    if (!inRoot)
    {
        float m[12] = {};
        if (depth >= 3 || !IsGroup(parent) || !ReadMem(parent + 0x30, m, sizeof(m)))
            return false;
        const bool straight = m[0] > 0.999f && m[0] < 1.001f && m[5] > 0.999f && m[5] < 1.001f && m[1] > -0.001f && m[1] < 0.001f &&
                              m[4] > -0.001f && m[4] < 0.001f && m[2] > -0.001f && m[2] < 0.001f && m[8] > -0.001f && m[8] < 0.001f;
        float pbx = 0.0f;
        float pby = 0.0f;
        if (!straight || !GroupFracDepth(parent, &pbx, &pby, w, h, depth + 1))
            return false;
        *bx = pbx + ReadF32(group + kElemX) / *w;
        *by = pby + ReadF32(group + kElemY) / *h;
        return *bx > -100.0f && *bx < 100.0f && *by > -100.0f && *by < 100.0f;
    }
    for (int k = 0; k < kHudRoots; ++k)
    {
        const uintptr_t r = RootAt(k);
        if (!r || owner != r + kRootList)
            continue;
        if (!RootUsable(r, w, h))
            return false;
        float ox = 0.0f;
        float oy = 0.0f;
        float sx = 1.0f;
        float sy = 1.0f;
        if (ReadU8(group + kElemRootFlag))
        {
            ox = ReadF32(r + 8);
            oy = ReadF32(r + 0x0C);
            sx = ReadF32(r + 0x10);
            sy = ReadF32(r + 0x14);
            if (!(ox >= -1.0f && ox <= 1.0f && oy >= -1.0f && oy <= 1.0f && sx > 0.1f && sx <= 2.0f && sy > 0.1f && sy <= 2.0f))
                return false;
        }
        *bx = ox + sx * ReadF32(group + kElemRootX) + ReadF32(group + kElemX) / *w;
        *by = oy + sy * ReadF32(group + kElemRootY) + ReadF32(group + kElemY) / *h;
        return *bx > -100.0f && *bx < 100.0f && *by > -100.0f && *by < 100.0f;
    }
    return false;
}

// the place in `group` that draws at (fx, fy) of its root's view
bool GroupFracPlace(uintptr_t group, float fx, float fy, float* x, float* y)
{
    float bx = 0.0f;
    float by = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    if (!GroupFrac(group, &bx, &by, &w, &h))
        return false;
    *x = (fx - bx) * w;
    *y = (fy - by) * h;
    return true;
}

// horizontal 2-player (class_wheel.cpp's ClassWheelHorizontal: [Splitscreen] MenusHorizontal=1, Vertical=0, 2 local
// players, both HUD roots horizontal halves)
bool P2Hz()
{
    return ClassWheelHorizontal();
}

// one horizontal half's HUD root (roots[r], player p + 1's) with Classic's safe area down (logged when written)
void HzRootFix(int p, int r)
{
    {
        const uintptr_t root = r >= 0 && r < kHudRoots ? RootAt(r) : 0;
        float w = 0.0f;
        float h = 0.0f;
        if (!RootUsable(root, &w, &h) || w < 2.0f * h) // (a horizontal half only)
            return;
        const float oy = ReadF32(root + 0x0C);
        const float sy = ReadF32(root + 0x14);
        const float d1 = oy - kHzOffY;
        const float d2 = sy - kHzScaleY;
        if (d1 < 0.0005f && d1 > -0.0005f && d2 < 0.0005f && d2 > -0.0005f)
            return;
        // (a root's safe area is never outside these: AddViewport's margins are 0 - 0.1 of the view)
        const float ox = ReadF32(root + 8);
        const float sx = ReadF32(root + 0x10);
        if (!(oy >= 0.0f && oy <= 0.5f && sy >= 0.5f && sy <= 1.0f && ox >= 0.0f && ox <= 0.5f && sx >= 0.5f && sx <= 1.0f))
        {
            if (HzBudget())
                Log("[p2spawn] %lu ms: 20g-r12: horizontal: player %d's HUD root roots[%d]: safe area %.3f %.3f / %.3f %.3f not as "
                    "expected - left as it is",
                    ScanMsSinceAttach(), p + 1, r, ox, oy, sx, sy);
            return;
        }
        *reinterpret_cast<float*>(root + 0x0C) = kHzOffY;
        *reinterpret_cast<float*>(root + 0x14) = kHzScaleY;
        const long fixes = ++g_hzFixes[r]; // (the first 3 of each root and every 100th logged)
        if ((fixes <= 3 || fixes % 100 == 0) && HzBudget())
            Log("[p2spawn] %lu ms: 20g-r12: horizontal: player %d's HUD root roots[%d] 0x%08X (%.0f x %.0f): safe area down %.3f / %.3f "
                "-> %.3f / %.3f (Classic's: 10%% at the top and the bottom of his half); across %.3f / %.3f kept (write %ld)",
                ScanMsSinceAttach(), p + 1, r, static_cast<unsigned>(root), w, h, oy, sy, kHzOffY, kHzScaleY, ReadF32(root + 8),
                ReadF32(root + 0x10), fixes);
    }
}

// each frame (the menu layer, game thread; 20g-r13: and players 3 / 4's frame): the horizontal halves' HUD roots get
// Classic's safe area down (the game writes its own again only when it sets the views up) - both players' in the 2-player
// horizontal split, and (20g-r13) player 3's in the 3-player layout's bottom half
void HzRootsFrame()
{
    if (!g_hzSafeWanted)
        return;
    if (P2Hz())
        for (int p = 0; p < 2; ++p)
            HzRootFix(p, PlayerRootIndex(p));
    for (int k = 0; k < 2 && g_hzMenusWanted && k + 3 <= PlayerNLocalPlayers(); ++k)
        HzRootFix(k + 2, PlayerNSpecView(k)); // (a quarter is left as it is)
}

// ---- texts -----------------------------------------------------------------------------------------
// player 2's HUD root, and where an element of it stands: its anchor in the root (+A0h / +A4h,
// fractions) and its own position; a child at (x, y) of it sits at anchor * size + position + (x, y)
// 20g-r12: horizontal - the root's safe area taken into account (GroupFracPlace): (fx, fy) are fractions of his half as
// drawn
bool RootPlace(uintptr_t group, float fx, float fy, float* x, float* y)
{
    if (P2Hz())
    {
        const int r1 = PlayerRootIndex(1);
        const uintptr_t root1 = r1 >= 0 ? RootAt(r1) : 0;
        if (!root1 || !group || ReadU32(group + kElemNodeOwner) != root1 + kRootList)
            return false;
        return GroupFracPlace(group, fx, fy, x, y);
    }
    const int r = PlayerRootIndex(1);
    const uintptr_t root = r >= 0 ? RootAt(r) : 0;
    if (!root || !group || ReadU32(group + kElemNodeOwner) != root + kRootList)
        return false;
    const float w = static_cast<float>(static_cast<int32_t>(ReadU32(root)));
    const float h = static_cast<float>(static_cast<int32_t>(ReadU32(root + 4)));
    if (w < 64.0f || h < 64.0f || w > 8192.0f || h > 8192.0f)
        return false;
    *x = fx * w - ReadF32(group + kElemRootX) * w - ReadF32(group + kElemX);
    *y = fy * h - ReadF32(group + kElemRootY) * h - ReadF32(group + kElemY);
    return true;
}

// the screen's group our own texts go in: its preferred top-level group if it is shown and in his
// root, else the first such top-level element of its record
uintptr_t ScreenGroup(int scr, const char** panelOut)
{
    *panelOut = nullptr;
    const uintptr_t pref = RegLookup(kFlowGroupPref[scr]);
    float x = 0.0f;
    float y = 0.0f;
    if (pref && (ReadU32(pref + kElemFlags) & 0x100) && RootPlace(pref, 0.5f, 0.5f, &x, &y))
    {
        *panelOut = kFlowGroupPref[scr];
        return pref;
    }
    const uintptr_t rec = ScreenRecord(kFlowScreens[scr]);
    int32_t count = rec ? static_cast<int32_t>(ReadU32(rec + kRecordCount)) : 0;
    if (count < 0 || count > 256)
        count = 0;
    for (int32_t i = 0; i < count; ++i)
    {
        const uintptr_t e = ReadU32(rec + kRecordElems + 4 * static_cast<uint32_t>(i));
        if (e && (ReadU32(e + kElemFlags) & 0x100) && RootPlace(e, 0.5f, 0.5f, &x, &y))
            return e;
    }
    return 0;
}

uintptr_t FlowAdd(uintptr_t e, int scr, const char* panel, uintptr_t group)
{
    if (e && g_flow.n < kFlowTexts)
    {
        FlowText& t = g_flow.t[g_flow.n++];
        t.e = e;
        t.screen = kFlowScreens[scr];
        t.rec = ScreenRecord(kFlowScreens[scr]);
        t.panel = panel;
        t.group = group;
    }
    return e;
}

uintptr_t FlowTextAtS(int scr, const wchar_t* s, float fx, float fy, float scale, const char* what); // 20g-r7 (below)

// a text of ours at (fx, fy) of his half, in the screen's group
uintptr_t FlowTextAt(int scr, const wchar_t* s, float fx, float fy, const char* what)
{
    return FlowTextAtS(scr, s, fx, fy, kTextScale, what);
}

// 20g-r7: the same at its own scale
uintptr_t FlowTextAtS(int scr, const wchar_t* s, float fx, float fy, float scale, const char* what)
{
    const char* panel = nullptr;
    const uintptr_t group = ScreenGroup(scr, &panel);
    float x = 0.0f;
    float y = 0.0f;
    if (!group || !RootPlace(group, fx, fy, &x, &y))
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: %s text - no shown group of %s in player 2's root: not made", ScanMsSinceAttach(), what,
                kFlowScreens[scr]);
        return 0;
    }
    const uintptr_t e = ClassWheelTextNew(group, s, x, y, scale);
    char nb[96];
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s text 0x%08X in %s (0x%08X, anchor %.2f %.2f) at %.0f %.0f = %.2f %.2f of his half",
            ScanMsSinceAttach(), what, static_cast<unsigned>(e), panel ? panel : NameOfHash(ReadU32(group + kElemHash), nb, sizeof(nb)),
            static_cast<unsigned>(group), ReadF32(group + kElemRootX), ReadF32(group + kElemRootY), x, y, fx, fy);
    return FlowAdd(e, scr, panel, group);
}

uintptr_t FlowTextInGroupS(int scr, const char* panel, const wchar_t* s, float fx, float fy, float scale, const char* what);

// 18w: a text of ours in a named group of the screen (the registry path), at (fx, fy) of his half
uintptr_t FlowTextInGroup(int scr, const char* panel, const wchar_t* s, float fx, float fy, const char* what)
{
    return FlowTextInGroupS(scr, panel, s, fx, fy, kTextScale, what);
}

// 20g-r12: the same at its own scale
uintptr_t FlowTextInGroupS(int scr, const char* panel, const wchar_t* s, float fx, float fy, float scale, const char* what)
{
    const uintptr_t group = RegLookup(panel);
    float x = 0.0f;
    float y = 0.0f;
    if (!group || !(ReadU32(group + kElemFlags) & 0x100) || !RootPlace(group, fx, fy, &x, &y))
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: %s text - %s is missing / hidden / not in player 2's root: the Info group instead",
                ScanMsSinceAttach(), what, panel);
        return FlowTextAtS(scr, s, fx, fy, scale, what);
    }
    const uintptr_t e = ClassWheelTextNew(group, s, x, y, scale);
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s text 0x%08X in %s (0x%08X, anchor %.2f %.2f) at %.0f %.0f = %.2f %.2f of his half",
            ScanMsSinceAttach(), what, static_cast<unsigned>(e), panel, static_cast<unsigned>(group), ReadF32(group + kElemRootX),
            ReadF32(group + kElemRootY), x, y, fx, fy);
    return FlowAdd(e, scr, panel, group);
}

// 18x: a class's soldier at (fx, fy) of his half (its feet), in the screen's shown group
uintptr_t FlowModelAt(int scr, uintptr_t cls, float fx, float fy, float size, const char* what)
{
    if (!g_soldiersWanted)
        return 0;
    const char* panel = nullptr;
    const uintptr_t group = ScreenGroup(scr, &panel);
    float x = 0.0f;
    float y = 0.0f;
    if (!group || !RootPlace(group, fx, fy, &x, &y))
        return 0;
    bool droideka = false;
    const uintptr_t e = ClassWheelModelNew(group, cls, x, y, size, &droideka);
    if (e && scr == kScrSide) // 19g: where it lives (FlowModelsDrop deletes it only while that is still the screen's)
    {
        g_flow.sideRec = ScreenRecord(kFlowScreens[scr]);
        g_flow.sideGroup = group;
        g_flow.sidePanel = panel;
    }
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s soldier 0x%08X%s (class 0x%08X) in %s at %.0f %.0f = %.2f %.2f of his half%s",
            ScanMsSinceAttach(), what, static_cast<unsigned>(e), droideka ? " (droideka)" : "", static_cast<unsigned>(cls),
            panel ? panel : "the screen's first group", x, y, fx, fy, e ? "" : " - NOT made (ring functions / class)");
    return e;
}

void FlowModelsDrop()
{
    // 19g: deleted only while the screen's record and group they were made in are still the screen's (a
    // screen taken down by the game may be going); else left to the game, as the texts
    const bool sideAlive = g_flow.sideRec && ScreenRecord(kFlowScreens[kScrSide]) == g_flow.sideRec &&
                           (!g_flow.sidePanel || RegLookup(g_flow.sidePanel) == g_flow.sideGroup);
    for (int i = 0; i < 2; ++i)
    {
        if (g_flow.sideModel[i] && sideAlive)
            ClassWheelDeleteElement(g_flow.sideModel[i]);
        g_flow.sideModel[i] = 0;
    }
    g_flow.sideRec = 0;
    g_flow.sideGroup = 0;
    g_flow.sidePanel = nullptr;
    if (g_flow.ringShown) // 19g
    {
        const bool ringAlive = g_flow.ringRec && ScreenRecord(kFlowScreens[kScrClass]) == g_flow.ringRec &&
                               RegLookup(g_flow.ringPanel ? g_flow.ringPanel : kFlowGroupPref[kScrClass]) == g_flow.ringGroup;
        ClassWheelRing2Clear(ringAlive, ringAlive ? "his class screen is going" : "his class screen is gone - left to the game");
    }
    g_flow.ringShown = false;
    g_flow.ringRec = 0;
    g_flow.ringGroup = 0;
    g_flow.ringPanel = nullptr;
}

// his map element hidden on the side / class screens (the Xbox shows none there), shown again after
void MapElementHide(bool hide)
{
    const uintptr_t map = g_mapArray ? ReadU32(g_mapArray + 4) : 0;
    if (!map || hide == g_flow.mapHidden)
        return;
    g_flow.mapHidden = hide;
    ClassWheelShowElement(map + kMapGroup, !hide); // 18z: the drawn element is the group at +30h
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: player 2's map element 0x%08X (map +30h) %s", ScanMsSinceAttach(),
            static_cast<unsigned>(map + kMapGroup), hide ? "hidden" : "shown");
}

// a text inside a console panel, where ifs_charselect2's BoxText sits under its title bar (made like it)
uintptr_t FlowPanelText(int scr, const char* panel, const char* titlePath, const wchar_t* s)
{
    const uintptr_t group = RegLookup(panel);
    const uintptr_t title = RegLookup(titlePath);
    const uintptr_t like = RegLookup("ifs_charselect2.Info.Window.BoxText");
    const uintptr_t likeTitle = RegLookup("ifs_charselect2.Info.Window.titleBarElement");
    if (!group || !title || !like || !likeTitle)
    {
        if (FlowBudget())
            Log("[p2spawn] flow: %s - panel 0x%08X, title 0x%08X, BoxText 0x%08X / its title 0x%08X: MISSING", panel,
                static_cast<unsigned>(group), static_cast<unsigned>(title), static_cast<unsigned>(like),
                static_cast<unsigned>(likeTitle));
        return 0;
    }
    const float x = ReadF32(title + kElemX) + (ReadF32(like + kElemX) - ReadF32(likeTitle + kElemX));
    const float y = ReadF32(title + kElemY) + (ReadF32(like + kElemY) - ReadF32(likeTitle + kElemY));
    const uintptr_t e = ClassWheelTextLike(group, like, s, x, y);
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s text 0x%08X at %.1f %.1f (panel %s, title at %.1f %.1f)", ScanMsSinceAttach(), panel,
            static_cast<unsigned>(e), x, y, (ReadU32(group + kElemFlags) & 0x100) ? "shown" : "hidden", ReadF32(title + kElemX),
            ReadF32(title + kElemY));
    return FlowAdd(e, scr, panel, group);
}

// our texts deleted (del: only those whose screen and panel are still there, in this match) and forgotten
void FlowTextsDrop(bool del, const char* why)
{
    int deleted = 0;
    int left = 0;
    g_flow.sideLook = false; // 20g-r10
    g_flow.pwLook = false;   // 21j-r9
    if (del)
        FlowModelsDrop();
    else
    {
        g_flow.sideModel[0] = g_flow.sideModel[1] = 0;
        if (g_flow.ringShown) // 19g: left to the game (nothing touched)
            ClassWheelRing2Clear(false, why);
        g_flow.ringShown = false;
    }
    if (del)
        MapElementHide(false);
    else
        g_flow.mapHidden = false;
    for (int i = 0; i < g_flow.n; ++i)
    {
        FlowText& t = g_flow.t[i];
        if (!t.e)
            continue;
        const bool alive = del && t.rec && ScreenRecord(t.screen) == t.rec && (!t.panel || RegLookup(t.panel) == t.group);
        if (alive)
        {
            ClassWheelDeleteElement(t.e);
            ++deleted;
        }
        else
            ++left;
        t = FlowText();
    }
    if (g_flow.hints && del && RegLookup("ifs_mapselect2.SideBar.Hints") == g_flow.hints)
        ClassWheelShowElement(g_flow.hints, true);
    if ((deleted || left) && FlowBudget())
        Log("[p2spawn] %lu ms: flow: %d text(s) deleted%s (%s)", ScanMsSinceAttach(), deleted,
            left ? ", some left (their screen or panel is gone)" : "", why);
    g_flow.n = 0;
    g_flow.cube = 0; // 19g: (one of the texts: deleted or left with them)
    g_flow.textsFor = kScrNone;
    g_flow.textTries = 0;
    g_flow.layoutFor = kScrNone;
    g_flow.layoutTries = 0;
    g_flow.layoutPinned = false;
    g_flow.sideName[0] = g_flow.sideName[1] = 0;
    g_flow.prompt = 0;
    g_flow.postText = 0;
    g_flow.countText = 0;
    g_flow.classText = 0;
    g_flow.vehText = 0;
    g_flow.vehPost = 0;
    g_flow.hints = 0;
    g_flow.shownPost = 0;
    g_flow.shownCls = -1;
    g_flow.shownTeam = 0;
    g_flow.shownPick = -1;
    g_flow.waitShown = false;
}

bool Side12Will(int p); // 20g-r10 (players 3 / 4's section): player 1 / 2's side screen can be drawn in their look

// the texts of one screen (FlowTextsMake: once per time it is put up)
void HzPanelsScan(int which); // 20g-r13 (players 3 / 4's section): log only

void FlowTextsMakeOnce(int scr, uintptr_t sm)
{
    // 20g-r13 (3-player, log only, once a session): his panels / class window before anything of ours touches them
    if (g_flow.textTries == 1 && (scr == kScrPost || scr == kScrClass))
        HzPanelsScan(scr == kScrPost ? 0 : 1);
    // 19c: the root the screen is drawn in, each time its texts are made (its size / offset / scale)
    {
        const int r = PlayerRootIndex(1);
        char label[64];
        snprintf(label, sizeof(label), "layout: player 2's root (roots[%d]) with %s up:", r, kFlowScreens[scr]);
        LogRootFields(RootAt(r), label);
        const char* panel = nullptr;
        const uintptr_t g = ScreenGroup(scr, &panel);
        if (g && FlowBudget())
            Log("[p2spawn] layout:   its group %s 0x%08X: anchor %.2f %.2f, at %.0f %.0f, flag %u", panel ? panel : "?",
                static_cast<unsigned>(g), ReadF32(g + kElemRootX), ReadF32(g + kElemRootY), ReadF32(g + kElemX), ReadF32(g + kElemY),
                static_cast<unsigned>(ReadU8(g + kElemRootFlag)));
        // 20g-r12: the safe area exactly (the line above rounds it), and where the group's (0, 0) draws
        const uintptr_t root = (P2Hz() && g_flow.textTries == 1) ? RootAt(r) : 0; // (horizontal only, its first try)
        float bx = 0.0f, by = 0.0f, gw = 0.0f, gh = 0.0f;
        if (root && FlowBudget())
            Log("[p2spawn] layout:   %s: his root's safe area: offset %.3f %.3f, scale %.3f %.3f; the group's (0, 0) draws at %s",
                "horizontal", ReadF32(root + 8), ReadF32(root + 0x0C), ReadF32(root + 0x10), ReadF32(root + 0x14),
                (g && GroupFrac(g, &bx, &by, &gw, &gh)) ? "the place below" : "(not in a HUD root)");
        if (g && root && GroupFrac(g, &bx, &by, &gw, &gh) && FlowBudget())
            Log("[p2spawn] layout:   ... %.3f %.3f of his half (units %.0f x %.0f)", bx, by, gw, gh);
    }
    // 20g-r12: horizontal - his class screen's ring, grid and prompt go in its Helptext_Back group: nothing made until it is
    // shown in his root (FlowTextsMake tries again next frame while nothing is made)
    if (P2Hz() && scr == kScrClass)
    {
        const uintptr_t back = RegLookup(kHzClassGroup);
        float tx = 0.0f;
        float ty = 0.0f;
        if (!back || !(ReadU32(back + kElemFlags) & 0x100) || !RootPlace(back, 0.5f, 0.5f, &tx, &ty))
        {
            if ((g_flow.textTries == 1 || g_flow.textTries >= 30) && FlowBudget())
                Log("[p2spawn] %lu ms: flow: horizontal - %s 0x%08X not shown in his root yet (try %d)", ScanMsSinceAttach(), kHzClassGroup,
                    static_cast<unsigned>(back), g_flow.textTries);
            return;
        }
    }
    // 19g: the blue grid behind his screen (a copy of player 1's), at his root's centre in the screen's
    // Info group, which then draws before his HUD's groups (the score bar stays over the grid)
    // (20g-r10: not on the side screen in players 3 / 4's look - its own grid is in that look's group)
    const bool sideLook = scr == kScrSide && P2Quarter() && Side12Will(1);
    if (g_backdropWanted && !g_flow.cube && !sideLook)
    {
        const char* panel = nullptr;
        uintptr_t g = ScreenGroup(scr, &panel);
        if (P2Hz() && scr == kScrClass) // 20g-r12: his class screen's Info group stays turned (Classic's window): not in it
        {
            g = RegLookup(kHzClassGroup); // (shown and in his root: checked above)
            panel = kHzClassGroup;
        }
        float x = 0.0f;
        float y = 0.0f;
        if (g && RootPlace(g, 0.5f, 0.5f, &x, &y))
        {
            const uintptr_t cube = ClassWheelCubeNew(g, x, y);
            if (cube)
            {
                g_flow.cube = FlowAdd(cube, scr, panel, g);
                ClassWheelSetLayer(g, kInfoLayer);
            }
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: blue grid 0x%08X in %s (0x%08X) at %.0f %.0f (his root's centre)%s", ScanMsSinceAttach(),
                    static_cast<unsigned>(cube), panel ? panel : "the screen's first group", static_cast<unsigned>(g), x, y,
                    cube ? "; the group's layer 129" : " - NOT made");
        }
    }
    if (scr == kScrSide)
    {
        if (g_flow.textTries == 1) // 19g: the Xbox's side screen has no "B BACK"
        {
            const uintptr_t back = RegLookup("ifs_readyselect2.Helptext_Back");
            ClassWheelShowElement(back, false);
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: ifs_readyselect2.Helptext_Back 0x%08X %s", ScanMsSinceAttach(), static_cast<unsigned>(back),
                    back ? "hidden" : "MISSING");
        }
        if (sideLook) // 20g-r10: 4-player - players 3 / 4's own side screen (their group in his HUD root): Side12Frame
        {
            if (!g_flow.sideLook && FlowBudget())
                Log("[p2spawn] %lu ms: flow: his side screen in players 3 / 4's look (their group and places, in his HUD root) - "
                    "none of r9's texts made", ScanMsSinceAttach());
            g_flow.sideLook = true;
            MapElementHide(true);
            return;
        }
        if (P2Quarter()) // 20g-r7: 4-player - as players 3 / 4's side screens
        {
            FlowTextAtS(scr, L"CHOOSE YOUR SIDE", 0.5f, kQTitleY, kQTitleScale, "side title (4-player)");
            for (int i = 0; i < 2; ++i)
            {
                wchar_t name[48];
                TeamNameW(sm, i + 1, name, 48);
                g_flow.sideName[i] = FlowTextAtS(scr, name, kQSideX[i], kQSideNameY, kQNameScale, i == 0 ? "side 1 name" : "side 2 name");
                if (!g_flow.sideModel[i])
                    g_flow.sideModel[i] = FlowModelAt(scr, TeamClass(sm, i + 1, 0), kQSideX[i], kQSideFeetY, kQSideSize,
                                                      i == 0 ? "side 1" : "side 2");
            }
            g_flow.prompt = FlowTextAtS(scr, L"SELECT SIDE", 0.5f, kQPromptY, kQPromptScale, "prompt (4-player)");
            MapElementHide(true);
            return;
        }
        if (P2Hz()) // 20g-r12: horizontal - the Classic Collection's side screen, as player 1's (class_wheel.cpp's kHz*)
        {
            float w = 0.0f, h = 0.0f, ox = 0.0f, oy = 0.0f, kx = 0.0f, ky = 0.0f;
            const float size = P2SpawnRootGeom(1, &w, &h, &ox, &oy, &kx, &ky) ? kHzSideTall * h / kHzSoldierUnits : kSideModelSize;
            FlowTextAtS(scr, L"CHOOSE YOUR SIDE", kHzTitleX, kHzTitleY, kHzTextScale, "side title (horizontal)");
            for (int i = 0; i < 2; ++i)
            {
                wchar_t name[48];
                TeamNameW(sm, i + 1, name, 48);
                g_flow.sideName[i] = FlowTextAtS(scr, name, i == 0 ? kHzNameX0 : kHzNameX1, kHzNameY, kHzTextScale,
                                                 i == 0 ? "side 1 name (horizontal)" : "side 2 name (horizontal)");
                if (!g_flow.sideModel[i])
                    g_flow.sideModel[i] = FlowModelAt(scr, TeamClass(sm, i + 1, 0), i == 0 ? kHzSideX0 : kHzSideX1, kHzSideFeetY, size,
                                                      i == 0 ? "side 1 (horizontal)" : "side 2 (horizontal)");
            }
            MapElementHide(true);
            return;
        }
        FlowTextAt(scr, L"CHOOSE YOUR SIDE", kSideTitleX, kSideTitleY, "side title");
        for (int i = 0; i < 2; ++i)
        {
            wchar_t name[48];
            TeamNameW(sm, i + 1, name, 48);
            g_flow.sideName[i] = FlowTextAt(scr, name, i == 0 ? kSideNameX0 : kSideNameX1, kSideNameY, i == 0 ? "side 1 name" : "side 2 name");
            if (!g_flow.sideModel[i])
                g_flow.sideModel[i] = FlowModelAt(scr, TeamClass(sm, i + 1, 0), i == 0 ? kSideModelX0 : kSideModelX1, kSideModelY,
                                                  kSideModelSize, i == 0 ? "side 1" : "side 2");
        }
        MapElementHide(true);
    }
    else if (scr == kScrPost)
    {
        if (g_flow.textTries == 1) // 18z: the panels where the Xbox has them (19b: their group pinned first)
        {
            // 19h: the panels themselves: FlowLayoutFrame (flat, from their own sizes)
            // 20g-r12: horizontal - the group where the game has it (Classic's; the prompt in it is placed through it)
            if (!P2Hz())
                ClassWheelMoveElement(RegLookup("ifs_mapselect2.Helptext_Back"), kXbBackX, kXbBackY, "player 2's B BACK (spawn point)");
            // 19h: the Xbox's spawn point screen has no "B BACK": its icon and label hidden (the group stays:
            // the prompt is in it; B still goes back)
            const uintptr_t backIcon = RegLookup("ifs_mapselect2.Helptext_Back.Icon");
            const uintptr_t backBtn = RegLookup("ifs_mapselect2.Helptext_Back.btn");
            ClassWheelShowElement(backIcon, false);
            ClassWheelShowElement(backBtn, false);
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: ifs_mapselect2's \"B BACK\" (icon 0x%08X, label 0x%08X) hidden", ScanMsSinceAttach(),
                    static_cast<unsigned>(backIcon), static_cast<unsigned>(backBtn));
            // 19h: the Info group's turn (the blue grid is in it), logged
            const uintptr_t info = RegLookup("ifs_mapselect2.Info");
            float m[16] = {};
            if (info && ReadMem(info + 0x30, m, sizeof(m)) && FlowBudget())
                Log("[p2spawn] %lu ms: flow: ifs_mapselect2.Info 0x%08X rows %.3f %.3f %.3f / %.3f %.3f %.3f / %.3f %.3f %.3f (left as it is)",
                    ScanMsSinceAttach(), static_cast<unsigned>(info), m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);
        }
        if (P2Quarter()) // 20g-r7: 4-player - as players 3 / 4's spawn point screens (the panels are made for a half)
        {
            ClassWheelShowElement(RegLookup("ifs_mapselect2.SideBar"), false);
            if (Side12PwWill()) // 21j-r9: the console windows (players 3 / 4's, Side12Frame) - only his prompt of ours
            {
                if (!g_flow.pwLook && FlowBudget())
                    Log("[p2spawn] %lu ms: flow: his spawn point screen's console windows drawn by Side12Frame (players 3 / 4's) - only "
                        "SELECT SPAWNPOINT of r9's texts", ScanMsSinceAttach());
                g_flow.pwLook = true;
                g_flow.prompt = FlowTextAtS(scr, L"SELECT SPAWNPOINT", 0.5f, kQPromptY, kQPromptScale, "prompt (4-player)");
                return;
            }
            wchar_t side[48];
            TeamNameW(sm, g_flow.team, side, 48);
            FlowTextAtS(scr, side, 0.5f, kQTitleY, kQTitleScale, "side name (4-player)");
            FlowTextAtS(scr, L"<", kQMapArrowX[0], kQPostY, kQPostScale, "left arrow");
            FlowTextAtS(scr, L">", kQMapArrowX[1], kQPostY, kQPostScale, "right arrow");
            g_flow.pwLook = false; // 21j-r9 (r9's texts: no windows)
            g_flow.postText = FlowTextAtS(scr, L"", kQMapTextX, kQPostY, kQPostScale, "post name");
            g_flow.countText = FlowTextAtS(scr, L"", kQMapTextX, kQCountY, kQSmallScale, "post count");
            g_flow.vehText = FlowTextAtS(scr, L"", kQMapTextX, kQVehY, kQVehScale, "vehicles");
            g_flow.vehPost = 0;
            g_flow.shownPost = 0; // (the post's texts written again)
            g_flow.prompt = FlowTextAtS(scr, L"SELECT SPAWNPOINT", 0.5f, kQPromptY, kQPromptScale, "prompt (4-player)");
            return;
        }
        g_flow.postText = FlowPanelText(scr, "ifs_mapselect2.SideBar.Spawnpoint", "ifs_mapselect2.SideBar.Spawnpoint.titleBarElement", L"");
        // 19h: his VEHICLES box (19g left it empty): the game's own list for his post (PostFrame)
        g_flow.vehText = FlowPanelText(scr, "ifs_mapselect2.SideBar.Vehicle", "ifs_mapselect2.SideBar.Vehicle.titleBarElement", L"");
        g_flow.vehPost = 0;
        wchar_t obj[0x200] = {};
        const uintptr_t map0 = g_mapArray ? ReadU32(g_mapArray) : 0;
        if (g_getObjective && map0)
            g_getObjective(reinterpret_cast<void*>(map0), nullptr, obj, static_cast<int>(sizeof(obj)), g_flow.team);
        obj[0x1FF] = 0;
        FlowPanelText(scr, "ifs_mapselect2.SideBar.Objectives", "ifs_mapselect2.SideBar.Objectives.titleBarElement", obj);
        char a[300];
        if (g_flow.textTries == 1 && FlowBudget())
            Log("[p2spawn] %lu ms: flow: side %d's objective \"%s\"", ScanMsSinceAttach(), g_flow.team, Narrow(obj, a, sizeof(a)));
        g_flow.hints = RegLookup("ifs_mapselect2.SideBar.Hints");
        if (g_flow.hints)
            ClassWheelShowElement(g_flow.hints, false);
        g_flow.prompt = P2Hz() ? FlowTextInGroupS(scr, "ifs_mapselect2.Helptext_Back", L"SELECT SPAWNPOINT", kHzPromptPostX, kHzPromptY,
                                                  kHzPromptScale, "prompt (horizontal)") // 20g-r12
                               : FlowTextInGroup(scr, "ifs_mapselect2.Helptext_Back", L"SELECT SPAWNPOINT", kPromptX, kPromptY, "prompt");
    }
    else if (scr == kScrClass)
    {
        if (g_flow.textTries == 1) // 18z: the class window centred under the soldier (19b: its group pinned first)
        {
            // 19h: the window itself: FlowLayoutFrame (flat, from its own size)
            // 20g-r12: horizontal - "B BACK" where the game has it (Classic's)
            if (!P2Hz())
                ClassWheelMoveElement(RegLookup("ifs_charselect2.Helptext_Back"), kXbBackX, kXbBackY, "player 2's B BACK (class)");
        }
        if (P2Quarter()) // 20g-r7: 4-player - as players 3 / 4's class screens (the class window is made for a half)
        {
            ClassWheelShowElement(RegLookup("ifs_charselect2.Info.Window"), false);
            if (Side12PwWill()) // 21j-r9: the console window (players 3 / 4's, Side12Frame) - only his prompt of ours
            {
                if (!g_flow.pwLook && FlowBudget())
                    Log("[p2spawn] %lu ms: flow: his class screen's console window drawn by Side12Frame (players 3 / 4's) - only SELECT "
                        "CHARACTER of r9's texts; his ring left of it", ScanMsSinceAttach());
                g_flow.pwLook = true;
                g_flow.prompt = FlowTextAtS(scr, L"SELECT CHARACTER", 0.5f, kQPromptY, kQPromptScale, "prompt (4-player)");
                MapElementHide(true);
                return;
            }
            g_flow.pwLook = false; // 21j-r9 (r9's texts: no windows)
            g_flow.classText = FlowTextAtS(scr, L"", 0.5f, kQTitleY, kQTitleScale, "class name (4-player)");
            FlowTextAtS(scr, L"<", kQArrowX[0], kQClassArrowY, kQPostScale, "left arrow");
            FlowTextAtS(scr, L">", kQArrowX[1], kQClassArrowY, kQPostScale, "right arrow");
            g_flow.shownCls = -1; // (the class name written again)
            g_flow.prompt = FlowTextAtS(scr, L"SELECT CHARACTER", 0.5f, kQPromptY, kQPromptScale, "prompt (4-player)");
            MapElementHide(true);
            return;
        }
        g_flow.prompt = P2Hz() ? FlowTextInGroupS(scr, "ifs_charselect2.Helptext_Back", L"SELECT CHARACTER", kHzPromptClassX, kHzPromptY,
                                                  kHzPromptScale, "prompt (horizontal)") // 20g-r12
                               : FlowTextInGroup(scr, "ifs_charselect2.Helptext_Back", L"SELECT CHARACTER", kPromptX, kPromptY, "prompt");
        MapElementHide(true);
    }
}

// the texts of the screen that is up: once per time it is put up, tried again for up to 30 frames
// while none could be made (its groups may join his root a frame late)
void FlowTextsMake(int scr, uintptr_t sm)
{
    // 21j-r9: his console windows could not be drawn (Side12FlowBack): what was made for them (his prompt, grid) dropped here, in
    // his own frame, and his screen's texts made again as r9's (Side12PwWill is false for the rest of the match)
    if (g_flow.pwBack)
    {
        g_flow.pwBack = false;
        if (scr == kScrPost || scr == kScrClass)
            FlowTextsDrop(true, "his console windows could not be drawn - r9's texts instead");
    }
    if (!g_textsOk || g_flow.textsFor == scr || g_flow.textTries >= 30)
        return;
    ++g_flow.textTries;
    FlowTextsMakeOnce(scr, sm);
    const int texts = g_flow.n - (g_flow.cube ? 1 : 0); // 19g: the grid is not a text (tried again until one is made)
    const bool look = scr == kScrSide && g_flow.sideLook; // 20g-r10: players 3 / 4's look draws it (none of ours)
    if (texts <= 0 && g_flow.textTries < 30 && !look)
        return;
    g_flow.textsFor = scr;
    if (texts <= 0 && !look && FlowBudget())
        Log("[p2spawn] %lu ms: flow: no text could be made on %s (30 tries)", ScanMsSinceAttach(), kFlowScreens[scr]);
    // the texts that follow the choice are set again
    g_flow.shownPost = 0;
    g_flow.shownCls = -1;
    g_flow.shownTeam = 0;
    g_flow.shownPick = -1;
    if (g_flow.waitShown)
        ClassWheelSetText(g_flow.prompt, L"WAITING FOR PLAYER 1");
}

// 19h: the screen's panels (spawn point) / window (class) flat where the Xbox has them, from their
// title bars (ClassWheelXbPanels / ClassWheelXbWindow) - tried on a few frames after the screen comes
// up (its elements can join his root late). 19h-r2: 19g's places (the group pinned) on the first frame
// first, so the game's own layout never slides in; they stay if no title is ever found. Setting his
// class screen's Info group straight also stands his ring's soldiers and the blue grid straight.
void FlowLayoutFrame(int scr)
{
    constexpr int kFlowLayoutTries = 30; // (class_wheel.cpp's ClassWheelLayoutTryNow: the last try)
    if ((scr != kScrPost && scr != kScrClass) || g_flow.layoutFor == scr)
        return;
    if (P2Hz()) // 20g-r12: Classic's horizontal split has the game's own (turned) panels and class window
    {
        g_flow.layoutFor = scr;
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: %s laid out as the game does it (horizontal - Classic's)", ScanMsSinceAttach(), kFlowScreens[scr]);
        return;
    }
    ++g_flow.layoutTries;
    const bool lastTry = g_flow.layoutTries >= kFlowLayoutTries;
    const uintptr_t panels[3] = { RegLookup("ifs_mapselect2.SideBar.Spawnpoint"), RegLookup("ifs_mapselect2.SideBar.Vehicle"),
                                  RegLookup("ifs_mapselect2.SideBar.Objectives") };
    if (!g_flow.layoutPinned) // (on every frame until the elements are all there)
    {
        if (scr == kScrPost)
        {
            const uintptr_t bar = RegLookup("ifs_mapselect2.SideBar");
            g_flow.layoutPinned = bar && panels[0] && panels[1] && panels[2];
            ClassWheelMoveElement(bar, 0.0f, 0.0f, "player 2's SideBar group");
            ClassWheelMoveElement(panels[0], kXbPanelX, kXbPostY, "player 2's COMMAND POST panel");
            ClassWheelMoveElement(panels[1], kXbPanelX, kXbVehY, "player 2's VEHICLES panel");
            ClassWheelMoveElement(panels[2], kXbPanelX, kXbObjY, "player 2's OBJECTIVES panel");
        }
        else
        {
            const uintptr_t info = RegLookup("ifs_charselect2.Info");
            const uintptr_t window = RegLookup("ifs_charselect2.Info.Window");
            g_flow.layoutPinned = info && window;
            ClassWheelMoveElement(info, 0.0f, 0.0f, "player 2's Info group");
            ClassWheelMoveElement(window, kXbWindowX, kXbWindowY, "player 2's class window");
        }
    }
    if (!ClassWheelLayoutTryNow(g_flow.layoutTries))
        return;
    bool flat = false;
    if (scr == kScrPost)
    {
        const uintptr_t titles[3] = { RegLookup("ifs_mapselect2.SideBar.Spawnpoint.titleBarElement"),
                                      RegLookup("ifs_mapselect2.SideBar.Vehicle.titleBarElement"),
                                      RegLookup("ifs_mapselect2.SideBar.Objectives.titleBarElement") };
        const uintptr_t skins[3] = { RegLookup("ifs_mapselect2.SideBar.Spawnpoint.Skin"), RegLookup("ifs_mapselect2.SideBar.Vehicle.Skin"),
                                     RegLookup("ifs_mapselect2.SideBar.Objectives.Skin") };
        flat = ClassWheelXbPanels(1, RegLookup("ifs_mapselect2.SideBar"), panels, titles, skins, "player 2's", lastTry);
    }
    else
        flat = ClassWheelXbWindow(1, RegLookup("ifs_charselect2.Info"), RegLookup("ifs_charselect2.Info.Window"),
                                  RegLookup("ifs_charselect2.Info.Window.titleBarElement"), RegLookup("ifs_charselect2.Info.Window.Skin"),
                                  "player 2's", lastTry);
    if (flat && scr == kScrClass && g_flow.cube)
    {
        // the blue grid was put at his root's centre by the turned group's reckoning: the straight one's
        // (where player 1's grid stands in his half)
        float cx = 0.0f;
        float cy = 0.0f;
        if (ClassWheelFlatPlace(1, RegLookup("ifs_charselect2.Info"), 0.5f, 0.5f, &cx, &cy))
            ClassWheelMoveElement(g_flow.cube, cx, cy, "player 2's blue grid (the Info group straight)");
    }
    if (flat || g_flow.layoutTries >= kFlowLayoutTries)
    {
        g_flow.layoutFor = scr;
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: %s laid out %s (try %d)", ScanMsSinceAttach(), kFlowScreens[scr],
                flat ? "flat (the Xbox's)" : "as 19g (no title found)", g_flow.layoutTries);
    }
}

// ---- the screens -------------------------------------------------------------------------------------
int CursorShown()
{
    if (!g_cursorElem)
        return -1;
    return (ReadU32(g_cursorElem + kElemFlags) & 0x100) ? 1 : 0;
}

void CursorPutBack(int was)
{
    if (was >= 0 && g_cursorShow && CursorShown() != was)
        g_cursorShow(was);
}

// player 2's map in its spawn screen mode (the spawn point screen), and back
void MapSpawnMode(bool on)
{
    if (!g_mapWanted || !g_mapSetMode || !g_mapArray)
        return;
    if (on && !g_flow.mapSet)
    {
        const uintptr_t map = ReadU32(g_mapArray + 4);
        if (!map)
            return;
        g_flow.map = map;
        g_flow.mapSaved = ReadU32(map + kMapMode);
        g_flow.mapSet = true;
        // 18z: shown (the game keeps player 2's map element hidden); 19g: his layout (4, in mode 1) takes
        // the Xbox's place first and SetMode(1) applies it (its anchor and size: 18z's anchor change was
        // undone by it); MapUpdateP2 runs his map's Update every frame from here
        g_flow.mapHidden = true;
        MapElementHide(false);
        ClassWheelSplitMapBig(1, true);
        // 19i: as the spawn screen does player 1's map (GOG 1.3 0x00079419 / the chooser): SetFlag(1) - the
        // spawn screen look (no dark quad, no HUD icons; the picker gives his map layout 4 with the flag) -
        // then SetMode(1). Without the wrapped picker the flag stays clear (19h-r2's way: set for each
        // Update call, his layout lent).
        g_flow.mapFlagSet = false;
        if (g_mapSetFlag && g_mapPickThunk)
        {
            g_flow.mapFlagSet = true;
            g_mapSetFlag(reinterpret_cast<void*>(map), nullptr, 1);
        }
        g_mapSetMode(reinterpret_cast<void*>(map), nullptr, 1);
        g_flow.mapUpdates = 0;
        g_flow.mapLast.QuadPart = 0;
        // 19h: drawn last in his root, as the spawn screen does with player 1's map (SetLayer(map +30h, 1),
        // GOG 1.3 0x00079264); his stayed at 128 in 19g and never showed
        g_flow.mapLayerSaved = static_cast<int>(ReadU32(map + kMapGroup + kElemFlags) & 0xFF);
        ClassWheelSetLayer(map + kMapGroup, 1);
        g_mapWrapGame = 0; // 19h-r2: the game's Update calls on his map through the copied vtable
        g_mapWrapGameSeen = 0;
        g_flow.mapWrapLogged = false;
        g_flow.mapWrapped = MapWrapInstall(map);
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X %sSetMode(1) (was mode %lu, flag now %u) - its spawn screen mode; "
                "its element's layer %d -> %lu; dark quad +1F400h %s, icons group +1F290h %s",
                ScanMsSinceAttach(), static_cast<unsigned>(map), g_flow.mapFlagSet ? "SetFlag(1) + " : "",
                static_cast<unsigned long>(g_flow.mapSaved), static_cast<unsigned>(ReadU8(map + kMapFlag)), g_flow.mapLayerSaved,
                static_cast<unsigned long>(ReadU32(map + kMapGroup + kElemFlags) & 0xFF),
                (ReadU32(map + kMapDarkQuad + kElemFlags) & 0x100) ? "shown" : "hidden",
                (ReadU32(map + kMapIconsGroup + kElemFlags) & 0x100) ? "shown" : "hidden");
    }
    else if (!on && g_flow.mapSet)
    {
        if (g_flow.map && g_flow.map == ReadU32(g_mapArray + 4))
        {
            MapWrapRemove(g_flow.map, "his spawn point screen is down"); // 19h-r2
            // 19i: as the spawn screen closes player 1's (GOG 1.3 0x000777C5): SetMode(back), then SetFlag(0);
            // his layout 4 is put back before them (the picker's answer for his map while mapSet and the flag hold)
            ClassWheelSplitMapBig(1, false);
            g_mapSetMode(reinterpret_cast<void*>(g_flow.map), nullptr, static_cast<int>(g_flow.mapSaved));
            if (g_flow.mapFlagSet && g_mapSetFlag)
                g_mapSetFlag(reinterpret_cast<void*>(g_flow.map), nullptr, 0);
            g_flow.mapFlagSet = false;
            if (g_flow.mapLayerSaved >= 0) // 19h: its layer as it was
                ClassWheelSetLayer(g_flow.map + kMapGroup, g_flow.mapLayerSaved);
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: player 2's map back to mode %lu (flag %u), layer %d", ScanMsSinceAttach(),
                    static_cast<unsigned long>(g_flow.mapSaved), static_cast<unsigned>(ReadU8(g_flow.map + kMapFlag)),
                    g_flow.mapLayerSaved);
        }
        else
            ClassWheelSplitMapBig(1, false);
        g_flow.mapSet = false;
        g_flow.mapFlagSet = false;
        g_flow.mapWrapped = false;
        g_flow.mapLayerSaved = -1;
        g_flow.map = 0;
        P2MapLayoutQOff(); // 20g-r7
    }
}

// 19i: the layout picker, for every call the game makes (through its jump thunk): his map with the spawn
// screen flag gets layout 4 (his own, the Xbox's place), anything else the game's own answer
int F34MapLayoutFor(uintptr_t map); // 20g-r6 (players 3 / 4's section): their spawn point screen's map -> its layout, or -1

int __fastcall MapPickWrap(void* map, void* unusedEdx)
{
    const uintptr_t m = reinterpret_cast<uintptr_t>(map);
    if (m && m == g_flow.map && g_flow.mapSet && g_flow.mapFlagSet && ReadU8(m + kMapFlag))
        return P2Quarter() ? P2MapLayoutQ() : 4; // 20g-r7: 4-player - players 3 / 4's map layout
    if (m)
    {
        const int lay = F34MapLayoutFor(m); // 20g-r6: player 3's / 4's map on his spawn point screen
        if (lay >= 0)
            return lay;
    }
    return g_mapPick ? g_mapPick(map, unusedEdx) : 0;
}

// 19i: a jump thunk (E9 rel32) retargeted; false if it is not one
bool RetargetJump(uintptr_t thunk, const void* to)
{
    unsigned char* const p = reinterpret_cast<unsigned char*>(thunk);
    if (!p || p[0] != 0xE9)
        return false;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(to) - (thunk + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(p + 1, 4, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(p + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(p + 1, 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), p, 5);
    return true;
}

// 19h: both maps side by side, once a match (at his map's 3rd Update): what player 1's has that his has
// not. Each fixed member of a map (GOG 1.3: the picture +E0h is a RedMaskedBitmapElement with its
// material at +100h; the others are bitmaps with theirs at +F8h; a material's texture at +1Ch - the
// draw skips a bitmap without one), its group +30h, the icons group +1F290h.
void MapDumpElem(const char* what, uintptr_t e)
{
    char type[64];
    char where[96];
    float m[16] = {};
    ReadMem(e + 0x30, m, sizeof(m));
    const uint32_t flags = ReadU32(e + kElemFlags);
    const uintptr_t mat = ReadU32(e + 0xF8);
    const uintptr_t mat2 = ReadU32(e + 0x100);
    Log("[p2spawn] map dump:     %s 0x%08X %s: %s, layer %u, colour 0x%08X, at %.1f %.1f %.2f, rows %.3f %.3f %.3f, corners %.1f %.1f / "
        "%.1f %.1f, +F8h 0x%08X (texture 0x%08X), +100h 0x%08X (texture 0x%08X, +24h 0x%08X, +28h 0x%08X), %s",
        what, static_cast<unsigned>(e), TypeOrDash(e, type, sizeof(type)), (flags & 0x100) ? "shown" : "hidden",
        static_cast<unsigned>(flags & 0xFF), ReadU32(e + 0x70), m[12], m[13], m[14], m[0], m[5], m[10], ReadF32(e + 0x88),
        ReadF32(e + 0x8C), ReadF32(e + 0xC0), ReadF32(e + 0xC4), static_cast<unsigned>(mat),
        static_cast<unsigned>(mat ? ReadU32(mat + 0x1C) : 0), static_cast<unsigned>(mat2), static_cast<unsigned>(mat2 ? ReadU32(mat2 + 0x1C) : 0),
        static_cast<unsigned>(mat2 ? ReadU32(mat2 + 0x24) : 0), static_cast<unsigned>(mat2 ? ReadU32(mat2 + 0x28) : 0),
        WhichRoot(e, where, sizeof(where)));
}

void MapDumpOne(int p)
{
    const uintptr_t map = g_mapArray ? ReadU32(g_mapArray + 4 * static_cast<uint32_t>(p)) : 0;
    if (!map)
    {
        Log("[p2spawn] map dump: player %d's map - none", p + 1);
        return;
    }
    const uintptr_t g = map + kMapGroup;
    char where[96];
    float m[16] = {};
    ReadMem(g + 0x30, m, sizeof(m));
    uintptr_t kids[512];
    const int n = GroupChildren(g, kids, 512);
    int shown = 0;
    int shownBitmaps = 0;
    for (int i = 0; i < n && i < 512; ++i)
    {
        if (!(ReadU32(kids[i] + kElemFlags) & 0x100))
            continue;
        ++shown;
        char t[64];
        if (TypeName(kids[i], t, sizeof(t)) && strstr(t, "Bitmap"))
            ++shownBitmaps;
    }
    Log("[p2spawn] map dump: player %d's map 0x%08X: player +18h %ld, root index +1Ch %ld, enabled %u, follows 0x%08X (has it %u), "
        "mode %lu, flag %u; mode-1 view %.1f %.1f zoom %.5f / %.5f",
        p + 1, static_cast<unsigned>(map), static_cast<long>(static_cast<int32_t>(ReadU32(map + 0x18))),
        static_cast<long>(static_cast<int32_t>(ReadU32(map + 0x1C))), static_cast<unsigned>(ReadU8(map + kMapEnabled)),
        static_cast<unsigned>(ReadU32(map + kMapTrack)), static_cast<unsigned>(ReadU8(map + kMapHasTrack)),
        static_cast<unsigned long>(ReadU32(map + kMapMode)), static_cast<unsigned>(ReadU8(map + kMapFlag)), ReadF32(map + 0x404),
        ReadF32(map + 0x410), ReadF32(map + 0x41C), ReadF32(map + 0x428));
    Log("[p2spawn] map dump:   its group +30h 0x%08X: %s, layer %u, colour 0x%08X, rows %.3f %.3f %.3f / %.3f %.3f %.3f / %.3f %.3f %.3f, "
        "at %.1f %.1f %.2f, anchor %.3f %.3f flag %u, %s; %d child(ren), %d shown (%d bitmaps)",
        static_cast<unsigned>(g), (ReadU32(g + kElemFlags) & 0x100) ? "shown" : "hidden", static_cast<unsigned>(ReadU32(g + kElemFlags) & 0xFF),
        ReadU32(g + 0x70), m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14], ReadF32(g + kElemRootX),
        ReadF32(g + kElemRootY), static_cast<unsigned>(ReadU8(g + kElemRootFlag)), WhichRoot(g, where, sizeof(where)), n, shown,
        shownBitmaps);
    MapDumpElem("+E0h (the picture)", map + 0xE0);
    MapDumpElem("+1F0h (his arrow)", map + 0x1F0);
    MapDumpElem("+2F0h (raster)", map + 0x2F0);
    MapDumpElem("+1F400h", map + 0x1F400);
    MapDumpElem("+1F290h (icons group)", map + 0x1F290);
    MapDumpElem("+1F510h (shown in mode 1)", map + kMapExtraGroup); // 19i
    {
        // 19i: what +1F510h holds (a group: its first children)
        uintptr_t xk[8];
        const int nx = GroupChildren(map + kMapExtraGroup, xk, 8);
        for (int i = 0; i < nx && i < 8; ++i)
        {
            char what[48];
            snprintf(what, sizeof(what), "+1F510h's child %d", i);
            MapDumpElem(what, xk[i]);
        }
        char vt[64];
        Log("[p2spawn] map dump:     +1F740h 0x%08X %s: %.2f %.2f %.2f %.2f", static_cast<unsigned>(map + kMapVec),
            TypeOrDash(map + kMapVec, vt, sizeof(vt)), ReadF32(map + kMapVec), ReadF32(map + kMapVec + 4), ReadF32(map + kMapVec + 8),
            ReadF32(map + kMapVec + 12));
    }
    // the first shown children of +30h that are none of those
    int listed = 0;
    for (int i = 0; i < n && i < 512 && listed < 6; ++i)
    {
        const uintptr_t c = kids[i];
        if (c == map + 0xE0 || c == map + 0x1F0 || c == map + 0x2F0 || c == map + 0x1F400 || !(ReadU32(c + kElemFlags) & 0x100))
            continue;
        char what[48];
        snprintf(what, sizeof(what), "child %d (+%Xh)", i, static_cast<unsigned>(c - map));
        MapDumpElem(what, c);
        ++listed;
    }
}

// 19g: his map's Update every frame his spawn point screen is up (the game updates only player 1's map:
// without it his map's element never showed). Only for his map in its spawn screen mode, through its
// own vtable slot, which must be the function found and byte-checked at start.
uintptr_t MapVtOurs()
{
    return reinterpret_cast<uintptr_t>(&g_mapVtCopy[1]);
}

// his map's Update with his flag set and his layout (4) lent to 5 for the call (the reasons in
// MapUpdateP2's old comment, kept below); `fromGame` = the game's own call through the copied vtable
bool MapUpdateFlagged(uintptr_t map, float dt, bool fromGame)
{
    const bool logIt = g_flow.mapUpdates < 3 && FlowBudget();
    const uintptr_t grp = map + kMapGroup;
    const unsigned shownBefore = (ReadU32(grp + kElemFlags) & 0x100) ? 1u : 0u;
    // with its flag clear and no soldier to follow, Update drops what it follows and leaves the map hidden
    // (GOG 1.3 0x00188BAA); with the flag (what the spawn screen sets on player 1's) it follows his
    // character as player 1's does. 19i: the flag is set for good while his screen is up when the layout
    // picker is wrapped (MapSpawnMode: SetFlag(1), his map -> layout 4); else, as 19h-r2, set for the
    // call only, with layout 4's values (his) lent to layout 5 for the call (game thread: nothing else
    // reads the table meanwhile).
    uint8_t* const flag = reinterpret_cast<uint8_t*>(map + kMapFlag);
    const bool lend = !g_flow.mapFlagSet;
    float* const lay4 = (lend && g_mapTable) ? reinterpret_cast<float*>(g_mapTable + kMapLayoutBytes * 4) : nullptr;
    float* const lay5 = (lend && g_mapTable) ? reinterpret_cast<float*>(g_mapTable + kMapLayoutBytes * 5) : nullptr;
    float saved5[kMapLayoutFloats] = {};
    if (lay5)
    {
        memcpy(saved5, lay5, sizeof(saved5));
        memcpy(lay5, lay4, sizeof(saved5));
    }
    const uint8_t flagWas = *flag;
    *flag = 1;
    // 19i: the map lights up the post of GetSpawnDisplay(its player) - the one spawn screen object, player
    // 1's: its post (+23E0h) and team (+23DCh) are his for the call (the icons' team and the highlight both
    // read them: GOG 1.3 0x001893EA, 0x0018966E), put back right after
    const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
    const bool swap = sd != 0 && g_flow.post != 0 && g_flow.team >= 1 && g_flow.team <= 2;
    uint32_t sdPostWas = 0;
    uint32_t sdTeamWas = 0;
    if (swap)
    {
        sdPostWas = ReadU32(sd + kSdPost);
        sdTeamWas = ReadU32(sd + kSdTeamShown);
        *reinterpret_cast<volatile uint32_t*>(sd + kSdPost) = static_cast<uint32_t>(g_flow.post);
        *reinterpret_cast<volatile uint32_t*>(sd + kSdTeamShown) = static_cast<uint32_t>(g_flow.team);
    }
    const bool r = reinterpret_cast<MapUpdate_t>(Rva(g_mapUpdateRva))(reinterpret_cast<void*>(map), nullptr, dt);
    if (swap)
    {
        *reinterpret_cast<volatile uint32_t*>(sd + kSdPost) = sdPostWas;
        *reinterpret_cast<volatile uint32_t*>(sd + kSdTeamShown) = sdTeamWas;
    }
    *flag = flagWas;
    if (lay5)
        memcpy(lay5, saved5, sizeof(saved5));
    if (g_flow.mapUpdates == 2 && !g_flow.mapDumped) // 19h
    {
        g_flow.mapDumped = true;
        Log("[p2spawn] %lu ms: map dump (once a match, his spawn point screen up; player 1's map wherever it is now):",
            ScanMsSinceAttach());
        MapDumpOne(0);
        MapDumpOne(1);
        // 19i: his root's shown elements (what else draws over his spawn point screen: the "OBJECTIVES:"
        // over his map in 19h-r2's video)
        const uintptr_t root = RootAt(PlayerRootIndex(1));
        uintptr_t kids[96];
        const int n = RootChildren(root, kids, 96);
        int shown = 0;
        Log("[p2spawn] root dump: player 2's root 0x%08X (%d child(ren)) - the shown ones, his spawn point screen up:",
            static_cast<unsigned>(root), n);
        for (int i = 0; i < n && i < 96; ++i)
        {
            if (!(ReadU32(kids[i] + kElemFlags) & 0x100))
                continue;
            ++shown;
            if (shown <= 48)
                LogRootChild(kids[i]);
        }
        Log("[p2spawn] root dump: %d shown", shown);
    }
    if (logIt)
        Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X Update(%.4f) #%d (%s): enabled %u, follows 0x%08X, has it %u, mode %lu, "
            "flag %u; its element %s -> %s (anchor %.3f %.3f, at %.1f %.1f)",
            ScanMsSinceAttach(), static_cast<unsigned>(map), dt, g_flow.mapUpdates + 1, fromGame ? "the game's call" : "called here",
            static_cast<unsigned>(ReadU8(map + kMapEnabled)), static_cast<unsigned>(ReadU32(map + kMapTrack)),
            static_cast<unsigned>(ReadU8(map + kMapHasTrack)), static_cast<unsigned long>(ReadU32(map + kMapMode)),
            static_cast<unsigned>(ReadU8(map + kMapFlag)), shownBefore ? "shown" : "hidden",
            (ReadU32(grp + kElemFlags) & 0x100) ? "SHOWN" : "HIDDEN (Update ended early)", ReadF32(grp + kElemRootX),
            ReadF32(grp + kElemRootY), ReadF32(grp + kElemX), ReadF32(grp + kElemY));
    ++g_flow.mapUpdates;
    return r;
}

bool F34MapUpdate(uintptr_t map, float dt, bool* result); // 20g-r6 (players 3 / 4's section)

// 19h-r2: slot 1 of the copied vtable: the game's own Update calls on his map (and MapUpdateP2's)
bool __fastcall MapUpdateWrap(void* self, void* unusedEdx, float dt)
{
    const uintptr_t map = reinterpret_cast<uintptr_t>(self);
    if (!g_mapUpdateRva)
        return true;
    bool f34Result = true;
    if (map != g_flow.map && !g_mapWrapDepth && F34MapUpdate(map, dt, &f34Result)) // 20g-r6: player 3's / 4's map
        return f34Result;
    if (map != g_flow.map || !g_flow.mapSet || g_mapWrapDepth)
        return reinterpret_cast<MapUpdate_t>(Rva(g_mapUpdateRva))(self, unusedEdx, dt); // another map, his screen down, or re-entered
    ++g_mapWrapDepth;
    ++g_mapWrapGame;
    const bool r = MapUpdateFlagged(map, dt, true);
    --g_mapWrapDepth;
    return r;
}

// the copy in his map (true: in, or already in), the game's vtable checked first (its Update slot
// must be the function found and byte-checked at start)
bool MapWrapInstall(uintptr_t map)
{
    if (!map || !g_mapUpdateRva)
        return false;
    const uintptr_t vt = ReadU32(map);
    if (vt == MapVtOurs())
        return true;
    const uintptr_t fn = vt ? ReadU32(vt + kMapVtUpdate) : 0;
    const uint32_t rva = InExe(fn) ? FollowJumps(static_cast<uint32_t>(fn - ExeBase())) : 0;
    if (!InExe(vt) || rva != g_mapUpdateRva)
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X: its vtable 0x%08X / Update slot rva 0x%08X is not MapDisplay's "
                "(0x%08X) - not wrapped",
                ScanMsSinceAttach(), static_cast<unsigned>(map), static_cast<unsigned>(vt), rva, g_mapUpdateRva);
        return false;
    }
    int n = 0;
    while (n < kMapVtMax && InCode(ReadU32(vt + 4 * static_cast<uint32_t>(n))))
        ++n;
    if (n < 2 || n >= kMapVtMax) // (n = kMapVtMax: more slots than the copy holds, or no end found - not wrapped)
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X: its vtable has %d code slot(s) (%s) - not wrapped", ScanMsSinceAttach(),
                static_cast<unsigned>(map), n, n < 2 ? "too few" : "too many for the copy");
        return false;
    }
    g_mapVtCopy[0] = ReadU32(vt - 4); // the RTTI locator (TypeName reads it)
    for (int i = 0; i < kMapVtMax; ++i)
        g_mapVtCopy[1 + i] = i < n ? ReadU32(vt + 4 * static_cast<uint32_t>(i)) : 0;
    g_mapVtCopy[2] = reinterpret_cast<uintptr_t>(&MapUpdateWrap); // slot 1
    g_mapVtSlots = n;
    g_mapVtOrig = vt;
    *reinterpret_cast<volatile uintptr_t*>(map) = MapVtOurs();
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X: its vtable (rva 0x%08X, %d slot(s)) copied, slot 1 (Update) now "
            "BF1GameExt's - every call keeps his map shown",
            ScanMsSinceAttach(), static_cast<unsigned>(map), RvaOf(vt), n);
    return true;
}

// the game's vtable back in his map (only while it is still his map and still has the copy)
void MapWrapRemove(uintptr_t map, const char* why)
{
    if (!map || !g_mapArray || ReadU32(g_mapArray + 4) != map || ReadU32(map) != MapVtOurs() || !g_mapVtOrig)
        return;
    *reinterpret_cast<volatile uintptr_t*>(map) = g_mapVtOrig;
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X: its own vtable back (%s)", ScanMsSinceAttach(), static_cast<unsigned>(map),
            why);
}

// 19g: his map's Update every frame his spawn point screen is up (the game updates only player 1's map:
// without it his map's element never showed). 19h-r2: the game does call his map's Update (through
// the copied vtable now, with his flag set); this calls it only while the game has not since the last
// frame (no copy, or the game's loop skips it).
void MapUpdateP2()
{
    if (!g_mapUpdateRva || !g_flow.mapSet || !g_flow.map || g_flow.map != ReadU32(g_mapArray + 4))
        return;
    const uintptr_t map = g_flow.map;
    if (g_flow.mapWrapped && ReadU32(map) == MapVtOurs())
    {
        const int gameCalls = g_mapWrapGame - g_mapWrapGameSeen;
        g_mapWrapGameSeen = g_mapWrapGame;
        if (gameCalls > 0)
        {
            if (!g_flow.mapWrapLogged && FlowBudget())
                Log("[p2spawn] %lu ms: flow: the game calls player 2's map's Update itself (%d call(s) since the last frame) - "
                    "not called here",
                    ScanMsSinceAttach(), gameCalls);
            g_flow.mapWrapLogged = true;
            return;
        }
    }
    const uintptr_t vt = ReadU32(map);
    const uintptr_t fn = vt ? ReadU32(vt + kMapVtUpdate) : 0;
    const uint32_t rva = InExe(fn) ? FollowJumps(static_cast<uint32_t>(fn - ExeBase())) : 0;
    if (vt != MapVtOurs() && rva != g_mapUpdateRva)
    {
        if (g_flow.mapUpdates++ == 0 && FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2's map 0x%08X: its Update slot is rva 0x%08X, not MapDisplay::Update (0x%08X) - "
                "not called",
                ScanMsSinceAttach(), static_cast<unsigned>(map), rva, g_mapUpdateRva);
        return;
    }
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    float dt = 1.0f / 60.0f;
    if (g_flow.mapLast.QuadPart && freq.QuadPart)
        dt = static_cast<float>(static_cast<double>(now.QuadPart - g_flow.mapLast.QuadPart) / static_cast<double>(freq.QuadPart));
    g_flow.mapLast = now;
    if (dt < 0.001f)
        dt = 0.001f;
    if (dt > 0.1f)
        dt = 0.1f;
    MapUpdateFlagged(map, dt, false);
}

// 19i: the manager's queued screen events run now (the IF manager's tick with dt 0): a PushScreen /
// PopScreen just made takes effect at once, not at the next IF update. False if the tick is not there / not
// wanted.
bool FlowSettle(uintptr_t mgr)
{
    if (!g_managerTick || !g_switchNowWanted || !mgr)
        return false;
    g_managerTick(reinterpret_cast<void*>(mgr), nullptr, 0.0f);
    return true;
}

// our screen down (it must be manager 1's top), texts deleted first, the map as it was
void FlowClose(const char* why)
{
    if (g_flow.screen == kScrNone)
        return;
    const uintptr_t mgr1 = Manager(1);
    const char* mine = kFlowScreens[g_flow.screen];
    char top[64];
    TopName(mgr1, top, sizeof(top));
    FlowTextsDrop(true, why);
    MapSpawnMode(false);
    const bool isTop = mgr1 && strcmp(top, mine) == 0;
    bool settled = false;
    if (isTop)
    {
        const int cur = CursorShown();
        g_popScreen(reinterpret_cast<void*>(mgr1), nullptr, 0);
        settled = FlowSettle(mgr1); // 19i: off the root now, not at the next IF update
        CursorPutBack(cur);
    }
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s %s (%s); manager 1 depth %lu%s", ScanMsSinceAttach(), mine,
            isTop ? "taken down (PopScreen)" : "NOT taken down - not manager 1's top", why,
            static_cast<unsigned long>(mgr1 ? ReadU32(mgr1 + kMgrDepth) : 0), settled ? ", settled now" : "");
    g_flow.screen = kScrNone;
    g_flow.wasTop = false;
}

void FlowTopSetup(uintptr_t sm); // defined with FlowFrame
void FlowScreenFrame(uintptr_t sm, uintptr_t ch, bool left, bool right, bool up, bool down, bool a, bool b);

// put one of our screens up (ours down first): the game's PushScreen(name, 1, 1), as it pushes his pause menu
void FlowShow(int scr, const char* why)
{
    const uintptr_t mgr1 = Manager(1);
    if (!mgr1)
        return;
    FlowTextsDrop(true, "screen changed");
    if (scr != kScrPost)
        MapSpawnMode(false);
    const int cur = CursorShown();
    const char* was = g_flow.screen ? kFlowScreens[g_flow.screen] : nullptr;
    if (g_flow.screen)
        g_popScreen(reinterpret_cast<void*>(mgr1), nullptr, 0);
    g_pushScreen(reinterpret_cast<void*>(mgr1), nullptr, kFlowScreens[scr], 1, 1);
    *reinterpret_cast<volatile uint32_t*>(mgr1 + kMgrCountdown) = kFlowCountdown;
    // 19i: the switch now (the old screen off the root, the new one entered and on it) and the new screen
    // set up below in this same call: the next frame drawn is the finished screen (this runs after this
    // frame's HUD draw; without it the IF update switched next frame and that frame drew the screen as the
    // game left it, before FlowFrame could set it up)
    const bool settled = FlowSettle(mgr1);
    CursorPutBack(cur);
    g_flow.screen = scr;
    g_flow.wasTop = false;
    char top[64];
    TopName(mgr1, top, sizeof(top));
    const bool topNow = strcmp(top, kFlowScreens[scr]) == 0;
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s%s%s -> PushScreen(\"%s\", 1, 1) on manager 1 (%s): top now \"%s\"%s, depth %lu, cursor %s",
            ScanMsSinceAttach(), was ? "PopScreen(\"" : "", was ? was : "", was ? "\") " : "", kFlowScreens[scr], why, top,
            settled ? (topNow ? " (settled now)" : " (settled, but NOT ours on top?)") : " (the IF update switches)",
            static_cast<unsigned long>(ReadU32(mgr1 + kMgrDepth)), cur < 0 ? "?" : (cur ? "shown (kept)" : "hidden (kept)"));
    if (scr == kScrPost)
        MapSpawnMode(true);
    if (settled && topNow)
    {
        // its texts, grid and flat layout, and the screen's own first frame without input (the post name
        // and vehicles, the class name and ring, the picked side lit) - all before the next frame is drawn
        const uintptr_t sm = SpawnScanManager();
        FlowTopSetup(sm);
        FlowScreenFrame(sm, SpawnScanCharacter(1), false, false, false, false, false, false);
    }
}

// 19h: the score bar (a player's TeamDisplay - display B - its +40h group: "200 | 200") off while that
// player's side screen is up, as the Xbox's: its enabled byte +30h cleared (TeamDisplay::Update hides
// +40h / +780h first and stops there when it is 0 - GOG 1.3 0x002200A7), put back after (only while it
// is still the 0 written here). Both players, split screen only. A new match forgets it (nothing written).
struct ScoreBar
{
    uintptr_t disp = 0;
    bool off = false;
    uint8_t was = 1;
};
ScoreBar g_score[2];
int g_scoreLines = 0;
constexpr uint32_t kTeamEnabled = 0x30;
constexpr uint32_t kTeamBar = 0x40;
constexpr uint32_t kTeamBar2 = 0x780;

void ScoreBarFrame(uintptr_t sm)
{
    for (int p = 0; p < 2; ++p)
    {
        ScoreBar& b = g_score[p];
        const uintptr_t disp = g_dispBArray ? ReadU32(g_dispBArray + 4 * static_cast<uint32_t>(p)) : 0;
        if (b.off && b.disp != disp)
            b = ScoreBar(); // another display (its own state): the old one forgotten
        const uintptr_t mgr = Manager(p);
        char top[64] = "";
        if (mgr)
            TopName(mgr, top, sizeof(top));
        // 20g-r11: 4-player, players 3 / 4's look - player 1's off on all three of his screens (theirs have none: his
        // spawn point / class screens' titles stand where it is)
        const bool p1Look = p == 0 && P2Quarter() && Side12Will(0) &&
                            (strcmp(top, "ifs_mapselect1") == 0 || strcmp(top, "ifs_charselect1") == 0);
        const bool want =
            sm && disp && LocalPlayers() >= 2 && (strcmp(top, p == 0 ? "ifs_readyselect1" : "ifs_readyselect2") == 0 || p1Look);
        if (want)
        {
            if (!b.off)
            {
                char t[64];
                if (!TypeName(disp, t, sizeof(t)) || strcmp(t, "TeamDisplay") != 0)
                    continue;
                b.disp = disp;
                b.was = ReadU8(disp + kTeamEnabled);
                b.off = true;
                if (g_scoreLines++ < 40)
                    Log("[p2spawn] %lu ms: score bar: player %d's TeamDisplay 0x%08X off (its %s screen is up; enabled was %u)",
                        ScanMsSinceAttach(), p + 1, static_cast<unsigned>(disp), p1Look ? top : "side", static_cast<unsigned>(b.was));
            }
            *reinterpret_cast<volatile uint8_t*>(disp + kTeamEnabled) = 0;
            ClassWheelShowElement(disp + kTeamBar, false);
            ClassWheelShowElement(disp + kTeamBar2, false);
        }
        else if (b.off)
        {
            const bool back = ReadU8(disp + kTeamEnabled) == 0;
            if (back)
                *reinterpret_cast<volatile uint8_t*>(disp + kTeamEnabled) = b.was;
            if (g_scoreLines++ < 40)
                Log("[p2spawn] %lu ms: score bar: player %d's TeamDisplay 0x%08X %s (his spawn screens are down)", ScanMsSinceAttach(),
                    p + 1, static_cast<unsigned>(disp), back ? "back on" : "left as the game set it");
            b = ScoreBar();
        }
    }
}

// a new match (or none): everything of the old one forgotten - its screens and texts are the game's to
// tear down (nothing deleted, nothing popped)
void FlowNewMatch(uintptr_t sm, long sdDeletes)
{
    P2MapLayoutQOff(); // 20g-r7: (the table only)
    // 19h: a score bar turned off here goes back on if its display is still the one in the game's array
    // (the game clears the array's entries when it deletes its displays); otherwise nothing is written
    for (int p = 0; p < 2; ++p)
    {
        ScoreBar& b = g_score[p];
        char t[64];
        if (b.off && g_dispBArray && ReadU32(g_dispBArray + 4 * static_cast<uint32_t>(p)) == b.disp && TypeName(b.disp, t, sizeof(t)) &&
            strcmp(t, "TeamDisplay") == 0 && ReadU8(b.disp + kTeamEnabled) == 0)
            *reinterpret_cast<volatile uint8_t*>(b.disp + kTeamEnabled) = b.was;
        b = ScoreBar();
    }
    const int lines = g_flow.lines;
    if (g_flow.screen && FlowBudget())
        Log("[p2spawn] %lu ms: flow: the match ended with %s up - left to the game", ScanMsSinceAttach(),
            kFlowScreens[g_flow.screen]);
    if (g_flow.ringShown) // 19g: his ring's soldiers forgotten (nothing touched)
        ClassWheelRing2Clear(false, "a new match");
    if (g_flow.mapSet && g_flow.map)
        MapWrapRemove(g_flow.map, "a new match"); // 19h-r2: (only while it is still his map)
    g_flow = Flow();
    g_flow.lines = lines;
    g_flow.sm = sm;
    g_flow.sdDeletes = sdDeletes;
}

// ---- each screen, every frame it is manager 1's top -----------------------------------------------
void SideFrame(uintptr_t sm, bool left, bool right, bool a)
{
    if (left && g_flow.pick != 0)
    {
        g_flow.pick = 0;
        ClassWheelPlaySound(0);
    }
    if (right && g_flow.pick != 1)
    {
        g_flow.pick = 1;
        ClassWheelPlaySound(0);
    }
    if (g_flow.pick != g_flow.shownPick)
    {
        g_flow.shownPick = g_flow.pick;
        ClassWheelTextGrey(g_flow.sideName[0], g_flow.pick != 0);
        ClassWheelTextGrey(g_flow.sideName[1], g_flow.pick != 1);
        ClassWheelModelGrey(g_flow.sideModel[0], g_flow.pick != 0);
        ClassWheelModelGrey(g_flow.sideModel[1], g_flow.pick != 1);
    }
    if (!a)
        return;
    const int team = g_flow.pick + 1;
    wchar_t name[48];
    char a8[96];
    TeamNameW(sm, team, name, 48);
    if (team != g_flow.team)
    {
        g_flow.post = 0; // another side: its own posts and classes
        g_flow.cls = 0;
    }
    g_flow.team = team;
    g_flow.sideDone = true;
    ClassWheelPlaySound(1);
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: A on the side screen - side %d \"%s\"", ScanMsSinceAttach(), team, Narrow(name, a8, sizeof(a8)));
    FlowShow(kScrPost, "side chosen");
}

void PostFrame(uintptr_t sm, bool prev, bool next, bool a, bool b)
{
    // (his map's Update: FlowTopSetup, every frame the screen is up)
    if (b && SidesLockedNow() == 1) // 21f: no side screen to go back to
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: B on the spawn point screen - sides are locked, no side screen to go back to",
                ScanMsSinceAttach());
        b = false;
    }
    if (b)
    {
        ClassWheelPlaySound(2);
        FlowShow(kScrSide, "B on the spawn point screen");
        return;
    }
    // the list (NextSpawnPoint round once) when the screen is up, on a press, and twice a second (a
    // post can be lost while he looks)
    const DWORD now = GetTickCount();
    if (!prev && !next && !a && g_flow.shownPost == g_flow.post && g_flow.post && now - g_flow.postsAt < 500)
        return;
    g_flow.postsAt = now;
    uintptr_t posts[kMaxPosts];
    const int n = TeamPosts(sm, g_flow.team, posts, kMaxPosts);
    int k = -1;
    for (int i = 0; i < n; ++i)
        if (posts[i] == g_flow.post)
            k = i;
    bool lost = false;
    if (n && k < 0)
    {
        lost = g_flow.post != 0;
        k = 0;
        g_flow.post = posts[0];
    }
    if (n && (prev || next))
    {
        k = (k + (next ? 1 : n - 1)) % n;
        g_flow.post = posts[k];
        ClassWheelPlaySound(0);
    }
    if (P2Quarter() && (g_flow.countText || g_flow.pwLook)) // 20g-r7: 4-player: the game's panels stay hidden (its script may show them)
        ClassWheelShowElement(RegLookup("ifs_mapselect2.SideBar"), false);
    if (g_flow.post != g_flow.shownPost)
    {
        g_flow.shownPost = g_flow.post;
        wchar_t name[96];
        char a8[200];
        PostNameW(g_flow.post, name, 96);
        ClassWheelSetText(g_flow.postText, name);
        if (g_flow.countText) // 20g-r7
        {
            wchar_t c[24];
            swprintf(c, 24, L"%d / %d", k + 1, n);
            ClassWheelSetText(g_flow.countText, c);
        }
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: spawn point %d of %d \"%s\" (0x%08X)", ScanMsSinceAttach(), k + 1, n,
                Narrow(name, a8, sizeof(a8)), static_cast<unsigned>(g_flow.post));
    }
    // 19h: the vehicles at his post, as the game writes them for player 1's (its own PopupFill)
    if (g_flow.vehText && g_flow.post && g_flow.post != g_flow.vehPost)
    {
        g_flow.vehPost = g_flow.post;
        wchar_t veh[256];
        if (ClassWheelVehiclesAt(g_flow.post, veh, 256))
            ClassWheelSetText(g_flow.vehText, veh);
    }
    if (a)
    {
        if (!n)
        {
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: A on the spawn point screen - side %d has no post to spawn at", ScanMsSinceAttach(),
                    g_flow.team);
            return;
        }
        if (lost)
        {
            // his post was lost: the screen shows the one that took its place; A again to take it
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: A on the spawn point screen - the post he had is gone, now showing the first; "
                    "A again to take it",
                    ScanMsSinceAttach());
            return;
        }
        ClassWheelPlaySound(1);
        FlowShow(kScrClass, "spawn point chosen");
    }
}

void ShowClass(uintptr_t sm)
{
    if (g_flow.cls == g_flow.shownCls && g_flow.team == g_flow.shownTeam)
        return;
    g_flow.shownCls = g_flow.cls;
    g_flow.shownTeam = g_flow.team;
    const uintptr_t cls = TeamClass(sm, g_flow.team, g_flow.cls);
    wchar_t name[96];
    wchar_t box[600];
    int weapons = 0;
    char a8[200];
    if (!ClassTextsW(cls, name, 96, box, 600, &weapons))
    {
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: class %d of side %d (0x%08X) is not a soldier class - texts left", ScanMsSinceAttach(),
                g_flow.cls, g_flow.team, static_cast<unsigned>(cls));
        return;
    }
    if (g_flow.classText) // 20g-r7: 4-player
        ClassWheelSetText(g_flow.classText, name);
    const bool t1 = ClassWheelSetText(RegLookup("ifs_charselect2.Info.Window.titleBarElement"), name);
    const bool t2 = ClassWheelSetText(RegLookup("ifs_charselect2.Info.Window.BoxText"), box);
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: class %d \"%s\" (0x%08X), EQUIPMENT: %d weapon(s)%s", ScanMsSinceAttach(), g_flow.cls,
            Narrow(name, a8, sizeof(a8)), static_cast<unsigned>(cls), weapons, (t1 && t2) ? "" : " - title / BoxText MISSING");
}

// 19g: his side's classes on a ring over the class window (class_wheel.cpp's ring 2), turned to
// the one he has
void RingFrame(uintptr_t sm, int cycle)
{
    if (P2Quarter() && (g_flow.classText || g_flow.pwLook)) // 20g-r7: 4-player - the class window (made for a half) stays hidden
        ClassWheelShowElement(RegLookup("ifs_charselect2.Info.Window"), false);
    if (!g_soldiersWanted || cycle <= 0)
        return;
    uintptr_t classes[8] = {};
    const int n = cycle < 8 ? cycle : 8;
    for (int i = 0; i < n; ++i)
        classes[i] = TeamClass(sm, g_flow.team, i);
    // 20g-r12: horizontal - the Classic Collection's ring, in his class screen's Helptext_Back group (his Info group stays
    // turned there, as Classic's class window: soldiers in it would be turned too), placed through his HUD root
    if (P2Hz())
    {
        const uintptr_t hgrp = RegLookup(kHzClassGroup);
        float w = 0.0f, h = 0.0f, ox = 0.0f, oy = 0.0f, kx = 0.0f, ky = 0.0f;
        float hx = 0.0f;
        float hy = 0.0f;
        if (!hgrp || !(ReadU32(hgrp + kElemFlags) & 0x100) || !P2SpawnRootGeom(1, &w, &h, &ox, &oy, &kx, &ky) ||
            !RootPlace(hgrp, kHzRingX, kHzRingFeetY, &hx, &hy))
            return;
        if (g_flow.ringShown && hgrp != g_flow.ringGroup)
            return; // (another group would leave the first one's soldiers behind)
        const int hsel = g_flow.cls < n ? g_flow.cls : 0;
        const float hsize = kHzRingTall * h / kHzSoldierUnits;
        const int hplaced = ClassWheelRing2Frame(hgrp, classes, n, hsel, hx, hy, kHzRingRadius * w, hsize, kHzRingLift * h, hx, hy,
                                                 kRingStyleClassic);
        if (!g_flow.ringShown && FlowBudget())
            Log("[p2spawn] %lu ms: flow: class ring (horizontal, Classic's) in %s (0x%08X): %d of %d class(es) placed, centre at %.0f %.0f "
                "(%.3f %.3f of his half), radius %.0f, back ones %.0f higher, size %.3f",
                ScanMsSinceAttach(), kHzClassGroup, static_cast<unsigned>(hgrp), hplaced, n, hx, hy, kHzRingX, kHzRingFeetY,
                kHzRingRadius * w, kHzRingLift * h, hsize);
        if (!g_flow.ringShown)
        {
            g_flow.ringRec = ScreenRecord(kFlowScreens[kScrClass]);
            g_flow.ringGroup = hgrp;
            g_flow.ringPanel = kHzClassGroup;
        }
        g_flow.ringShown = true;
        return;
    }
    // its own group only (ifs_charselect2.Info, shown, in his root): the ring never moves to another
    const char* const panel = kFlowGroupPref[kScrClass];
    const uintptr_t grp = RegLookup(panel);
    float x = 0.0f;
    float y = 0.0f;
    float fx = 0.0f;
    float fy = 0.0f;
    if (!grp || !(ReadU32(grp + kElemFlags) & 0x100) ||
        !RootPlace(grp, kGameSoldierFx, P2Quarter() ? kGameSoldierFyQ : kGameSoldierFy, &fx, &fy)) // 20g-r9: 520 root units
        return;
    // 19h: in his half's fractions when the group is straight (ClassWheelXbWindow), 19g's place if not
    // 20g-r7: 4-player - players 3 / 4's ring, placed in his root (a quarter: the flat place's units are a half's)
    const bool flat = ClassWheelFlatPlace(1, grp, kRingX, kRingY, &x, &y);
    float qx = 0.0f;
    float qy = 0.0f;
    const bool quarter = P2Quarter() && RootPlace(grp, g_flow.pwLook ? kPwRingX : kQRingX, kQRingY, &qx, &qy); // 21j-r9: windows
    if (quarter)
    {
        x = qx;
        y = qy;
    }
    else if (!flat && !RootPlace(grp, kRingX19g, kRingY19g, &x, &y))
        return;
    const float radius = quarter ? (g_flow.pwLook ? kPwRingRadius : kQRingRadius) * 512.0f : (flat ? kRingRadius * 512.0f : kRingWidth19g);
    const float size = quarter ? kQRingSize : (flat ? kRingSize : kRingSize19g);
    if (g_flow.ringShown && grp != g_flow.ringGroup)
        return; // (another group would leave the first one's soldiers behind)
    const int sel = g_flow.cls < n ? g_flow.cls : 0;
    const int placed =
        ClassWheelRing2Frame(grp, classes, n, sel, x, y, radius, size, (flat || quarter) ? kRingLift : kRingLift19g, fx, fy,
                             flat || quarter); // 20g-r9: a quarter's always in the Xbox arrangement (as players 3 / 4's)
    if (!g_flow.ringShown && FlowBudget())
        Log("[p2spawn] %lu ms: flow: class ring in %s (0x%08X, %s): %d of %d class(es) placed, centre at %.0f %.0f (%.3f %.3f of his "
            "half), radius %.0f, size %.2f, droidekas around %.0f %.0f",
            ScanMsSinceAttach(), panel, static_cast<unsigned>(grp), flat ? "straight" : "STILL TURNED - 19g's place", placed, n, x, y,
            flat ? kRingX : kRingX19g, flat ? kRingY : kRingY19g, radius, size, fx, fy);
    if (!g_flow.ringShown)
    {
        g_flow.ringRec = ScreenRecord(kFlowScreens[kScrClass]);
        g_flow.ringGroup = grp;
        g_flow.ringPanel = panel;
    }
    g_flow.ringShown = true;
}

void LogCharacter(const char* what, uintptr_t ch)
{
    if (!ch || !FlowBudget())
        return;
    Log("[p2spawn] %lu ms: flow: %s: character 1 team %ld, class index %ld, class 0x%08X, spawn point 0x%08X, post %ld, "
        "ready %u, unit 0x%08X",
        ScanMsSinceAttach(), what, static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharTeam))),
        static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharClassIdx))), static_cast<unsigned>(ReadU32(ch + kCharClass)),
        static_cast<unsigned>(ReadU32(ch + kCharSpawnPoint)), static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharPost))),
        static_cast<unsigned>(ReadU8(ch + kCharReady)), static_cast<unsigned>(ReadU32(ch + kCharUnit)));
}

void PromptText(const wchar_t* s)
{
    ClassWheelSetText(g_flow.prompt, s);
}

void ClassFrame(uintptr_t sm, uintptr_t ch, bool left, bool right, bool a, bool b)
{
    const int cycle = TeamCycle(sm, g_flow.team);
    if (g_flow.cls >= cycle)
        g_flow.cls = 0;
    if (g_flow.ready == 0 && cycle > 0 && (left || right))
    {
        g_flow.cls = left ? (g_flow.cls + cycle - 1) % cycle : (g_flow.cls + 1) % cycle;
        ClassWheelPlaySound(0);
        left = right = false;
    }
    ShowClass(sm); // the class name and EQUIPMENT (again after his pause menu)
    RingFrame(sm, cycle); // 19g: his class ring (every frame: it turns)
    if (g_flow.ready == 0)
    {
        if (g_flow.waitShown)
        {
            g_flow.waitShown = false;
            PromptText(L"SELECT CHARACTER");
        }
        if (a && cycle > 0)
        {
            // what the PC Spawn button does to its character: SetSpawnChoice(team, class index,
            // ValidPost(post, team)), then SetReady(1)
            ClassWheelPlaySound(1);
            const uintptr_t gsm = g_getSm();
            const uintptr_t post = g_validPost(reinterpret_cast<void*>(gsm), nullptr, g_flow.post, g_flow.team);
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: A on the class screen - SetSpawnChoice(side %d, class %d, post 0x%08X (ValidPost of "
                    "0x%08X)) on character 1 0x%08X",
                    ScanMsSinceAttach(), g_flow.team, g_flow.cls, static_cast<unsigned>(post), static_cast<unsigned>(g_flow.post),
                    static_cast<unsigned>(ch));
            g_setSpawnChoice(reinterpret_cast<void*>(ch), nullptr, g_flow.team, g_flow.cls, post);
            if (static_cast<int32_t>(ReadU32(ch + kCharTeam)) != g_flow.team && FlowBudget())
                Log("[p2spawn] %lu ms: flow: the game kept player 2 on side %ld (side %d full?)", ScanMsSinceAttach(),
                    static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharTeam))), g_flow.team);
            g_flow.ready = 1;
            g_flow.readyAt = GetTickCount();
            g_flow.readyLogged = false;
            LogCharacter("after SetSpawnChoice", ch);
        }
        else if (b)
        {
            ClassWheelPlaySound(2);
            FlowShow(kScrPost, "B on the class screen");
            return;
        }
    }
    if (g_flow.ready == 1)
    {
        if (!g_waitP1 || PlayerInWorld(0)) // 20g-r6: no waiting for player 1 (SpawnWaitP1=0)
        {
            g_setReady(reinterpret_cast<void*>(ch), nullptr, 1);
            g_flow.ready = 2;
            g_flow.readyAt = GetTickCount();
            LogCharacter(g_waitP1 ? "SetReady(1) - player 1 is in the world" : "SetReady(1)", ch);
            if (g_flow.waitShown)
            {
                g_flow.waitShown = false;
                PromptText(L"SELECT CHARACTER");
            }
        }
        else if (!g_flow.waitShown)
        {
            g_flow.waitShown = true;
            PromptText(L"WAITING FOR PLAYER 1");
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: player 1 is not in the world - player 2 spawns when he is (B cancels)",
                    ScanMsSinceAttach());
        }
    }
    if (g_flow.ready && b)
    {
        const int was = g_flow.ready;
        if (was == 2)
            g_setReady(reinterpret_cast<void*>(ch), nullptr, 0);
        g_flow.ready = 0;
        ClassWheelPlaySound(2);
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: B - spawn cancelled%s", ScanMsSinceAttach(), was == 2 ? " (SetReady(0))" : "");
        return;
    }
    if (g_flow.ready == 2 && !g_flow.readyLogged && GetTickCount() - g_flow.readyAt > kReadyLogAfterMs)
    {
        g_flow.readyLogged = true;
        LogCharacter("READY but the game has not spawned him yet", ch);
    }
}

// our screen forgotten without touching the game (a match being torn down): texts not deleted, the map
// left, nothing popped
void FlowForget(const char* why)
{
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s left to the game (%s)", ScanMsSinceAttach(), kFlowScreens[g_flow.screen], why);
    FlowTextsDrop(false, why);
    if (g_flow.mapSet)
        ClassWheelSplitMapReset(1); // 19g: his split map layout as the game had it (the table only)
    if (g_flow.mapSet && g_flow.map)
        MapWrapRemove(g_flow.map, why); // 19h-r2: (only while it is still his map)
    g_flow.mapSet = false;
    g_flow.mapFlagSet = false; // 19i: (the map object goes with the match; nothing written)
    g_flow.mapWrapped = false;
    g_flow.map = 0;
    P2MapLayoutQOff(); // 20g-r7: layout 1 back as the game has it (the table only)
    g_flow.screen = kScrNone;
    g_flow.wasTop = false;
}

// one of our screens by name (1..3), 0 if not ours - the game never puts these on manager 1 itself
int OurScreen(const char* name)
{
    for (int i = kScrSide; i <= kScrClass; ++i)
        if (strcmp(name, kFlowScreens[i]) == 0)
            return i;
    return kScrNone;
}

// a screen of ours left on manager 1 (closed while his pause menu was over it), now its top again
void PopOrphan(uintptr_t mgr1, const char* name, const char* why)
{
    const int cur = CursorShown();
    g_popScreen(reinterpret_cast<void*>(mgr1), nullptr, 0);
    CursorPutBack(cur);
    if (FlowBudget())
        Log("[p2spawn] %lu ms: flow: %s was left on manager 1 - taken down now (%s); depth %lu", ScanMsSinceAttach(), name, why,
            static_cast<unsigned long>(ReadU32(mgr1 + kMgrDepth)));
}

// our screen is manager 1's top (just put up, or every frame after): its countdown held, and - the first
// frame it is up - the pad latched, its texts, grid and flat layout made. 19i: also called from FlowShow
// right after the switch, so the first drawn frame of the screen is the finished one.
void FlowTopSetup(uintptr_t sm)
{
    const uintptr_t mgr1 = Manager(1);
    if (!mgr1 || g_flow.screen == kScrNone)
        return;
    *reinterpret_cast<volatile uint32_t*>(mgr1 + kMgrCountdown) = kFlowCountdown;
    if (!g_flow.wasTop)
    {
        g_flow.wasTop = true;
        PadLatch();
        // (back) on top: the texts that follow the choice set again
        g_flow.shownPost = 0;
        g_flow.shownCls = -1;
        g_flow.shownTeam = 0;
        g_flow.shownPick = -1;
        if (g_flow.waitShown)
            PromptText(L"WAITING FOR PLAYER 1");
        if (g_flow.screen == kScrPost)
            MapSpawnMode(true);
    }
    FlowTextsMake(g_flow.screen, sm); // once per time it is put up (tried again for a few frames)
    FlowLayoutFrame(g_flow.screen);   // 19h: its panels / window flat (tried on a few frames)
    if (g_flow.screen == kScrPost)
        MapUpdateP2(); // 19g: his map, every frame (19i: on the first frame too, before the draw)
}

// every frame (game thread, the menu layer)
void FlowFrame()
{
    if (!g_flowOk)
        return;
    const uintptr_t sm = SpawnScanManager();
    bool countKnown = false; // (logged at start)
    const long deletes = ClassWheelSpawnScreenDeletes(&countKnown);
    if (sm != g_flow.sm || deletes != g_flow.sdDeletes)
        FlowNewMatch(sm, deletes); // a new match (or none): the old one's screens and texts are the game's
    ScoreBarFrame(sm); // 19h: both players' side screens without the score bar
    const uintptr_t mgr1 = Manager(1);
    const uintptr_t ch = (sm && LocalPlayers() >= 2) ? SpawnScanCharacter(1) : 0;
    const bool eligible = mgr1 && ch && static_cast<int32_t>(ReadU32(ch + kCharIndex)) == 1;
    if (eligible && !g_flow.matchReady)
    {
        const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
        const uint32_t teamShown = sd ? ReadU32(sd + kSdTeamShown) : 0;
        if (sd && (teamShown == 1 || teamShown == 2) && ReadU32(sd + kSdCharacter))
        {
            g_flow.matchReady = true;
            if (g_getSm() != sm && !g_flow.smDiffLogged && FlowBudget())
            {
                g_flow.smDiffLogged = true;
                Log("[p2spawn] flow: GetSpawnManager 0x%08X differs from spawn_scan's 0x%08X", static_cast<unsigned>(g_getSm()),
                    static_cast<unsigned>(sm));
            }
        }
    }
    if (eligible && g_flow.matchReady && GetTickCount() - g_flow.loadedAt > 1000)
    {
        g_flow.loadedAt = GetTickCount();
        const bool loaded = ScreenRecord(kFlowScreens[1]) && ScreenRecord(kFlowScreens[2]) && ScreenRecord(kFlowScreens[3]);
        if (loaded != g_flow.loaded && FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2's screens %s - %s", ScanMsSinceAttach(), loaded ? "loaded" : "NOT loaded",
                loaded ? "his spawn screens look after his spawning (auto-spawn stands aside)" : "the auto-spawn spawns him");
        g_flow.loaded = loaded;
    }
    // without pad 2 he could not choose: the auto-spawn looks after him then
    const bool pad = Pad2IsConnected();
    if (eligible && g_flow.matchReady && g_flow.loaded && pad != g_flow.padWas)
    {
        g_flow.padWas = pad;
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: pad 2 %s", ScanMsSinceAttach(),
                pad ? "connected - player 2's spawn screens on" : "NOT connected - his spawn screens off, the auto-spawn spawns him");
    }
    const bool on = eligible && g_flow.matchReady && g_flow.loaded && pad;
    InterlockedExchange(&g_flowOwns, on ? 1 : 0);
    char top[64] = "";
    uint32_t depth = 0;
    if (mgr1)
    {
        TopName(mgr1, top, sizeof(top));
        depth = ReadU32(mgr1 + kMgrDepth);
    }
    // a screen of ours that was closed under his pause menu, on top again (same match, set up)
    const int orphan = (eligible && g_flow.matchReady && g_flow.screen == kScrNone) ? OurScreen(top) : kScrNone;
    if (!on)
    {
        if (!eligible)
        {
            // no character 1 / manager 1 / split screen any more: a match being torn down - its screens are
            // the game's (nothing popped, nothing deleted)
            if (g_flow.screen)
                FlowForget("no character 1 / split screen - the match is ending?");
        }
        else if (g_flow.screen)
            FlowClose(!pad ? "pad 2 not connected" : "his screens are not loaded");
        else if (orphan)
            PopOrphan(mgr1, kFlowScreens[orphan], "his screens are off");
        return;
    }
    if (EndOver()) // 21h-r8: the match is over - his spawn screens close and stay closed
    {
        if (g_flow.screen)
            FlowClose("the match is over");
        else if (orphan)
            PopOrphan(mgr1, kFlowScreens[orphan], "the match is over");
        return;
    }

    // in the world: no screen
    if (ReadU32(ch + kCharUnit))
    {
        if (!g_flow.inWorld)
        {
            g_flow.inWorld = true;
            LogCharacter("PLAYER 2 IS IN THE WORLD", ch);
        }
        g_flow.ready = 0;
        if (g_flow.screen)
            FlowClose("player 2 is in the world");
        else if (orphan)
            PopOrphan(mgr1, kFlowScreens[orphan], "player 2 is in the world");
        return;
    }
    if (g_flow.inWorld)
    {
        g_flow.inWorld = false;
        g_flow.leftWorldAt = GetTickCount();
        g_flow.timerLogged = false;
        const float timer = ReadF32(ch + kCharTimer);
        g_flow.timerRuns = timer == timer && timer > 0.0f && timer < kRespawnTimerMax;
        g_flow.timerSeen = timer;
        g_flow.timerSeenAt = g_flow.leftWorldAt;
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: player 2 left the world - his spawn point screen %s", ScanMsSinceAttach(),
                g_flow.timerRuns ? "when his respawn timer runs out, as player 1's (the game's SpawnDisplay waits for it)"
                                 : "in 1500 ms (his respawn timer +C0h is not running)");
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: his respawn timer (character +C0h) reads %.2f s", ScanMsSinceAttach(), timer);
    }

    if (orphan)
    {
        // he is down again and it is on top: it is ours again
        g_flow.screen = orphan;
        g_flow.wasTop = false;
        g_flow.ready = 0;
        if (FlowBudget())
            Log("[p2spawn] %lu ms: flow: %s (left on manager 1 under his pause menu) is ours again", ScanMsSinceAttach(),
                kFlowScreens[orphan]);
        return;
    }
    if (g_flow.screen == kScrNone)
    {
        if (depth > 0)
            return; // a screen of the game's (his pause menu) is up: after it
        if (g_flow.leftWorldAt)
        {
            // 19i: his respawn timer (the game counts it down after a death; player 1's spawn screen opens
            // when it is <= 0 - GOG 1.3 0x0007998B), kRespawnMaxWaitMs at most. 19h's 1.5 s when it was not
            // running when he left the world, or stops changing for 2 s (not his timer on this build)
            const DWORD now = GetTickCount();
            const float timer = ReadF32(ch + kCharTimer);
            const DWORD since = now - g_flow.leftWorldAt;
            if (timer != g_flow.timerSeen)
            {
                g_flow.timerSeen = timer;
                g_flow.timerSeenAt = now;
            }
            if (!g_flow.timerRuns && since < 500 && timer == timer && timer > 0.0f && timer < kRespawnTimerMax)
            {
                // set by the game a frame or two after the unit went (then it is his timer after all)
                g_flow.timerRuns = true;
                if (FlowBudget())
                    Log("[p2spawn] %lu ms: flow: his respawn timer started %lu ms after he left the world (%.2f s) - waited for",
                        ScanMsSinceAttach(), static_cast<unsigned long>(since), timer);
            }
            const bool stuck = now - g_flow.timerSeenAt > 2000;
            const bool byTimer = g_flow.timerRuns && !stuck && timer == timer;
            const bool wait = byTimer ? (timer > 0.0f && since < kRespawnMaxWaitMs) : since < kRespawnDelayMs;
            if (wait)
                return;
            if (!g_flow.timerLogged && FlowBudget())
            {
                g_flow.timerLogged = true;
                Log("[p2spawn] %lu ms: flow: %s, %lu ms after his death - his spawn point screen now", ScanMsSinceAttach(),
                    byTimer ? (timer <= 0.0f ? "his respawn timer ran out" : "his respawn timer still runs (the 30 s limit)")
                            : (g_flow.timerRuns ? "his respawn timer stopped changing - 19h's 1.5 s" : "19h's 1.5 s"),
                    static_cast<unsigned long>(since));
            }
        }
        const int32_t t = static_cast<int32_t>(ReadU32(ch + kCharTeam));
        if (!g_flow.sideDone)
        {
            g_flow.pick = t == 2 ? 1 : 0;
            g_flow.team = t == 2 ? 2 : 1;
            // 21f: sides locked (campaign / Galactic Conquest): no side screen - the side the game gave him
            const int locked = SidesLockedNow();
            if (locked == 1)
            {
                const int table = PlayerSideNow(1);
                const int side = (t == 1 || t == 2) ? static_cast<int>(t) : ((table == 1 || table == 2) ? table : 1);
                g_flow.pick = side == 2 ? 1 : 0;
                g_flow.team = side;
                g_flow.sideDone = true;
                if (FlowBudget())
                    Log("[p2spawn] %lu ms: flow: sides are locked (campaign / Galactic Conquest) - no side screen: player 2 on "
                        "side %d (his character's team %ld, the game's side table: player 1 %d, player 2 %d)",
                        ScanMsSinceAttach(), side, static_cast<long>(t), PlayerSideNow(0), table);
            }
            else if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: sides %s - the side screen (his character's team %ld, the game's side table: "
                    "player 1 %d, player 2 %d)",
                    ScanMsSinceAttach(), locked == 0 ? "can be switched" : "NOT known", static_cast<long>(t),
                    PlayerSideNow(0), PlayerSideNow(1));
        }
        else if ((t == 1 || t == 2) && t != g_flow.team)
        {
            // the game kept him on the other side: his spawn point screen shows that side's posts
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: player 2 is on side %ld, not %d - his screens follow the game", ScanMsSinceAttach(),
                    static_cast<long>(t), g_flow.team);
            g_flow.team = t;
            g_flow.post = 0;
            g_flow.cls = 0;
        }
        g_flow.ready = 0;
        FlowShow(g_flow.sideDone ? kScrPost : kScrSide, g_flow.sideDone ? "player 2 is down - the spawn point screen" : "match start - the side screen");
        return;
    }
    if (strcmp(top, kFlowScreens[g_flow.screen]) != 0)
    {
        if (depth == 0)
        {
            // taken down by someone else: up again next frame
            if (FlowBudget())
                Log("[p2spawn] %lu ms: flow: %s is gone from manager 1 (not by BF1GameExt) - put up again", ScanMsSinceAttach(),
                    kFlowScreens[g_flow.screen]);
            FlowTextsDrop(true, "screen gone");
            MapSpawnMode(false);
            g_flow.screen = kScrNone;
            g_flow.wasTop = false;
            return;
        }
        if (g_flow.wasTop && FlowBudget())
            Log("[p2spawn] %lu ms: flow: \"%s\" is over %s - waiting under it", ScanMsSinceAttach(), top, kFlowScreens[g_flow.screen]);
        g_flow.wasTop = false;
        return;
    }

    // ours is the top: its script takes no input, pad 2 drives it
    FlowTopSetup(sm);
    const PadIn in = Pad2Read();
    PadEdges& p = g_flow.pad;
    const bool left = StepEdge(in.left, p.lDown, p.lSince, p.lLast);
    const bool right = StepEdge(in.right, p.rDown, p.rSince, p.rLast);
    const bool up = StepEdge(in.up, p.uDown, p.uSince, p.uLast);
    const bool down = StepEdge(in.down, p.dDown, p.dSince, p.dLast);
    const bool a = PressEdge(in.a, p.aDown);
    const bool b = PressEdge(in.b, p.bDown);
    FlowScreenFrame(sm, ch, left, right, up, down, a, b);
}

// the screen that is up, one frame (19i: also once from FlowShow, without input, right after the switch)
void FlowScreenFrame(uintptr_t sm, uintptr_t ch, bool left, bool right, bool up, bool down, bool a, bool b)
{
    if (g_flow.screen == kScrSide)
        SideFrame(sm, left, right, a);
    else if (g_flow.screen == kScrPost)
        PostFrame(sm, left || up, right || down, a, b);
    else if (g_flow.screen == kScrClass && ch)
        ClassFrame(sm, ch, left, right, a, b);
}

// at start: the flow's functions, each checked
void FlowFind()
{
    uint32_t hits[4] = {};
    bool ok = true;
    // SetSpawnChoice / SetReady / ValidPost through the Spawn button's sequence
    int n = FindPattern(kSpawnPressed, hits, 4);
    if (n == 1)
    {
        const uint32_t sp = hits[0];
        const uint32_t choice = CallTarget(sp + kSpawnChoiceCall);
        const uint32_t ready = CallTarget(sp + kSpawnReadyCall);
        const bool argsOk = MatchPatternAt(sp - kSpawnArgsBack, kSpawnArgs);
        const uint32_t getter = argsOk ? CallTarget(sp - kSpawnArgsBack) : 0;
        const bool getterOk = getter && MatchPatternAt(getter, kPostGetterBody);
        const uint32_t gsm = getterOk ? CallTarget(getter + kPostGetterSmCall) : 0;
        const uint32_t valid = getterOk ? CallTarget(getter + kPostGetterValidCall) : 0;
        const bool choiceOk = MatchPatternAt(choice, kSetSpawnChoiceBody);
        const bool readyOk = MatchPatternAt(ready, kSetReadyBody);
        const bool gsmOk = gsm && MatchPatternAt(gsm, kGetSmBody);
        const bool validOk = valid && MatchPatternAt(valid, kValidPostBody);
        Log("[p2spawn] flow: the Spawn button's calls at rva 0x%08X: SetSpawnChoice 0x%08X %s, SetReady 0x%08X %s, its post "
            "argument's getter 0x%08X %s -> GetSpawnManager 0x%08X %s, ValidPost 0x%08X %s",
            sp, choice, choiceOk ? "OK" : "does NOT match", ready, readyOk ? "OK" : "does NOT match", getter,
            getterOk ? "OK" : "does NOT match", gsm, gsmOk ? "OK" : "does NOT match", valid, validOk ? "OK" : "does NOT match");
        if (choiceOk && readyOk && gsmOk && validOk)
        {
            g_setSpawnChoice = reinterpret_cast<SetSpawnChoice_t>(Rva(choice));
            g_setReady = reinterpret_cast<SetReady_t>(Rva(ready));
            g_getSm = reinterpret_cast<GetSm_t>(Rva(gsm));
            g_validPost = reinterpret_cast<ValidPost_t>(Rva(valid));
        }
        else
            ok = false;
    }
    else
    {
        Log("[p2spawn] flow: the Spawn button's calls - %d pattern hits (need exactly 1)", n);
        ok = false;
    }
    // NextSpawnPoint
    n = FindPattern(kNextPostBlock, hits, 4);
    if (n == 1)
    {
        const uint32_t next = CallTarget(hits[0] + kNextPostCall);
        const bool bodyOk = MatchPatternAt(next, kNextPostBody);
        const bool curOk = bodyOk && MatchPatternAt(next + kNextPostCursorAt, kNextPostCursor) &&
                           MatchPatternAt(next + kNextPostCursorSetAt, kNextPostCursorSet);
        Log("[p2spawn] flow: NextSpawnPoint 0x%08X %s, its cursor (+A8h) read / write %s", next, bodyOk ? "OK" : "does NOT match",
            curOk ? "OK" : "do NOT match");
        if (bodyOk && curOk)
            g_nextPost = reinterpret_cast<NextPost_t>(Rva(next));
        else
            ok = false;
    }
    else
    {
        Log("[p2spawn] flow: NextSpawnPoint's block - %d pattern hits (need exactly 1)", n);
        ok = false;
    }
    // the objective text (optional)
    n = FindPattern(kSelectTeamBody, hits, 4);
    if (n == 1 && MatchPatternAt(hits[0] + kObjSiteAt, kObjSite))
    {
        const uint32_t obj = CallTarget(hits[0] + kObjCall);
        if (MatchPatternAt(obj, kGetObjectiveBody))
            g_getObjective = reinterpret_cast<GetObjective_t>(Rva(obj));
        Log("[p2spawn] flow: SelectTeam rva 0x%08X: the objective text function 0x%08X %s", hits[0], obj,
            g_getObjective ? "OK" : "does NOT match - no OBJECTIVES text");
    }
    else
        Log("[p2spawn] flow: SelectTeam's objective site - %d pattern hits / site does not match - no OBJECTIVES text", n);
    // the IF cursor (PopScreen +3Bh) (optional: without it the cursor is as PushScreen / PopScreen leave it)
    const uintptr_t popFn = SpawnUiPopScreenFn();
    if (popFn && InExe(popFn))
    {
        const uint32_t popRva = static_cast<uint32_t>(popFn - ExeBase());
        if (MatchPatternAt(popRva + kPopCursorAt, kPopCursor))
        {
            const uint32_t cur = CallTarget(popRva + kPopCursorAt + 2);
            if (MatchPatternAt(cur, kCursorShowBody))
            {
                g_cursorShow = reinterpret_cast<CursorShow_t>(Rva(cur));
                g_cursorElem = ReadU32(Rva(cur + kCursorElemImm));
            }
            Log("[p2spawn] flow: the IF cursor: PopScreen +3Bh -> CursorShow 0x%08X %s, cursor element at rva 0x%08X", cur,
                g_cursorShow ? "OK" : "does NOT match", RvaOf(g_cursorElem));
        }
        else
            Log("[p2spawn] flow: the IF cursor: PopScreen +3Bh does NOT match - the cursor is left as the game leaves it");
    }
    g_textsOk = ClassWheelTextsReady();
    const bool padOk = Pad2Address() != 0;
    bool countKnown = false;
    ClassWheelSpawnScreenDeletes(&countKnown);
    Log("[p2spawn] flow: match ends %s", countKnown ? "counted through the spawn screen's deleting destructor (class_wheel.cpp's hook)"
                                                      : "NOT counted (that hook is not installed) - a new match is told by its SpawnManager only");
    Log("[p2spawn] flow: texts (class_wheel.cpp) %s, pad 2 %s, map SetMode %s, GetMap's array %s, managers %s, screen list "
        "%s, registry %s, PushScreen / PopScreen %s",
        g_textsOk ? "OK" : "NOT found (no texts)", padOk ? "OK" : "NOT built", g_mapSetMode ? "OK" : "NOT found",
        g_mapArray ? "OK" : "NOT found", g_mgrBaseAddr ? "OK" : "NOT found", g_screenList ? "OK" : "NOT found",
        g_registry ? "OK" : "NOT found", g_screensOk ? "OK" : "NOT found");
    g_flowOk = ok && padOk && g_screensOk && g_registry && g_sdGlobal && g_flowWanted;
}

// ---- 20f-r3: THE GAME'S HEAPS --------------------------------------------------------------------------
// (r4: the crash this was written for was the group's size - see the top of the file; this stays as a guard.)
// p20f-r2's crash (Steam, the first frame players 3 and 4's screens came up): every element after player 3's group
// came back 0 from the game's operator new, player 4's group too, and then the game's own new(4Ch) in the renderer
// (GOG 1.3 0x00287BFF) came back 0 and it wrote through it. The game's new / delete (GOG 1.3 0x0027D890 / 0x0027D8B0)
// allocate from and free into the CURRENT heap - an index at a global (GOG 1.3 0x00AA356C) that SetHeap (0x0027DF40)
// sets; each heap is a free list in a table of 1Ch-byte entries (0x00AA3588: +4 the first free block, a block = size,
// previous, next), and a failed allocation leaves its heap and size at 0x00AA3578 / +4. The heap current when our
// frame ran had about one group's worth left: our group took it and the game's next allocation failed. Now everything
// players 3 and 4's screens make or delete is done with the game's RUNTIME heap current (the one its memory set-up makes
// current at start, GOG 1.3 0x00140B2B: [0x008A7140]) - SetHeap(runtime) before, the heap that was current put back
// after - so they never take from the heap the game is using at that moment, and a free always goes to the heap the
// block came from. Elements are made only while the runtime heap has 256 KB free. The heaps are logged once a match.
const char* const kMemInitSite =
    "A3 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 50 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 68 00 00 20 00 FF 35 ?? ?? ?? ??";
constexpr uint32_t kMemInitOther = 0x01;    // mov [the 16 MB heap made first], eax
constexpr uint32_t kMemInitRuntime = 0x0D;  // mov [runtime heap], eax
constexpr uint32_t kMemInitSetHeap = 0x11;  // call SetHeap(runtime heap)
constexpr uint32_t kMemInitPush = 0x25;     // push [runtime heap] (the same global)
const char* const kSetHeapBody = "55 8B EC A1 ?? ?? ?? ?? 8B 4D 08 89 0D ?? ?? ?? ?? 5D C3";
constexpr uint32_t kSetHeapCurA = 0x04;
constexpr uint32_t kSetHeapCurB = 0x0D;
const char* const kHeapAllocBody =
    "55 8B EC 51 80 3D ?? ?? ?? ?? 00 75 05 E8 ?? ?? ?? ?? 8B 45 08 53 56 57 8D 0C C5 00 00 00 00 2B C8 A1 ?? ?? ?? ??";
constexpr uint32_t kHeapAllocTable = 0x22;
constexpr uint32_t kHeapAllocFailAt = 0x76; // mov [failed heap], eax; mov [failed size], edi
const char* const kHeapAllocFail = "A3 ?? ?? ?? ?? 89 3D ?? ?? ?? ??";
constexpr uint32_t kHeapEntry = 0x1C;
constexpr uint32_t kHeapFirstFree = 0x04;
constexpr uint32_t kBlockNext = 0x08;
constexpr int kMaxHeapIndex = 64;
constexpr int kHeapWalkMax = 4096;          // free blocks read at most (a walk stops there)
constexpr uint32_t kF34MinFree = 256 * 1024;

typedef int(__cdecl* SetHeap_t)(int heap);
SetHeap_t g_setHeap = nullptr;
uintptr_t g_curHeapVa = 0;      // the current heap index (the game's new / delete use it)
uintptr_t g_runtimeHeapVa = 0;  // the runtime heap's index
uintptr_t g_otherHeapVa = 0;    // the 16 MB heap the memory set-up makes just before it
uintptr_t g_heapTableVa = 0;    // -> the heap entries
uintptr_t g_heapFailVa = 0;     // the last failed allocation: heap index, then size
long g_heapSwitches = 0;        // frames our work ran with another heap current (switched to the runtime heap)
int g_heapLines = 0;
int g_heapAtEntry = -1;         // the heap that was current when this frame's work began

int HeapIndexAt(uintptr_t va)
{
    const int32_t v = va ? static_cast<int32_t>(ReadU32(va)) : -1;
    return (v >= 0 && v < kMaxHeapIndex) ? v : -1;
}

// a heap's free blocks: their total, the largest, how many (a walk stops after kHeapWalkMax blocks, or once the total
// reaches `enough` when it is not 0); false if the heap cannot be read
bool HeapFreeStats(int idx, uint32_t enough, uint32_t* total, uint32_t* largest, int* blocks)
{
    *total = 0;
    *largest = 0;
    *blocks = 0;
    const uintptr_t table = g_heapTableVa ? ReadU32(g_heapTableVa) : 0;
    if (!table || idx < 0 || idx >= kMaxHeapIndex)
        return false;
    uintptr_t b = ReadU32(table + static_cast<uint32_t>(idx) * kHeapEntry + kHeapFirstFree);
    while (b && *blocks < kHeapWalkMax)
    {
        const uint32_t size = ReadU32(b);
        *total += size;
        if (size > *largest)
            *largest = size;
        ++*blocks;
        if (enough && *total >= enough)
            break;
        b = ReadU32(b + kBlockNext);
    }
    return true;
}

// "heap 3 (runtime): 12345 KB free in 210 blocks, largest 9000 KB"
const char* HeapText(int idx, char* out, size_t n)
{
    uint32_t total = 0;
    uint32_t largest = 0;
    int blocks = 0;
    const char* name = idx < 0 ? "?" : (idx == HeapIndexAt(g_runtimeHeapVa) ? "runtime" : (idx == HeapIndexAt(g_otherHeapVa) ? "16 MB" : "other"));
    if (!HeapFreeStats(idx, 0, &total, &largest, &blocks))
        snprintf(out, n, "heap %d (%s): not readable", idx, name);
    else
        snprintf(out, n, "heap %d (%s): %lu KB free in %d%s block(s), largest %lu KB", idx, name,
                 static_cast<unsigned long>(total / 1024), blocks, blocks >= kHeapWalkMax ? "+" : "",
                 static_cast<unsigned long>(largest / 1024));
    return out;
}

// the game's record of its last failed allocation
const char* HeapFailText(char* out, size_t n)
{
    if (!g_heapFailVa)
        snprintf(out, n, "the game's failed-allocation record not found");
    else
        snprintf(out, n, "the game's last failed allocation: heap %ld, %lu bytes",
                 static_cast<long>(static_cast<int32_t>(ReadU32(g_heapFailVa))), static_cast<unsigned long>(ReadU32(g_heapFailVa + 4)));
    return out;
}

void HeapFind()
{
    uint32_t hits[4] = {};
    int n = FindPattern(kMemInitSite, hits, 4);
    uint32_t setHeap = 0;
    if (n == 1)
    {
        const uintptr_t runtime = ReadU32(Rva(hits[0] + kMemInitRuntime));
        const uintptr_t pushed = ReadU32(Rva(hits[0] + kMemInitPush));
        setHeap = CallTarget(hits[0] + kMemInitSetHeap);
        const bool setOk = setHeap && MatchPatternAt(setHeap, kSetHeapBody) &&
                           ReadU32(Rva(setHeap + kSetHeapCurA)) == ReadU32(Rva(setHeap + kSetHeapCurB));
        if (runtime && runtime == pushed && setOk && InExe(runtime))
        {
            g_runtimeHeapVa = runtime;
            g_otherHeapVa = ReadU32(Rva(hits[0] + kMemInitOther));
            g_curHeapVa = ReadU32(Rva(setHeap + kSetHeapCurA));
            g_setHeap = reinterpret_cast<SetHeap_t>(Rva(setHeap));
        }
        Log("[p2spawn] 20f-r3: the memory set-up rva 0x%08X: runtime heap global rva 0x%08X %s, SetHeap rva 0x%08X %s (current heap "
            "global rva 0x%08X)",
            hits[0], RvaOf(runtime), runtime == pushed ? "OK" : "does NOT match its push", setHeap, setOk ? "OK" : "does NOT match",
            RvaOf(g_curHeapVa));
    }
    else
        Log("[p2spawn] 20f-r3: the memory set-up: %d pattern hits (need exactly 1)", n);
    n = FindPattern(kHeapAllocBody, hits, 4);
    if (n == 1 && MatchPatternAt(hits[0] + kHeapAllocFailAt, kHeapAllocFail))
    {
        g_heapTableVa = ReadU32(Rva(hits[0] + kHeapAllocTable));
        g_heapFailVa = ReadU32(Rva(hits[0] + kHeapAllocFailAt + 1));
        if (ReadU32(Rva(hits[0] + kHeapAllocFailAt + 7)) != g_heapFailVa + 4)
            g_heapFailVa = 0;
    }
    Log("[p2spawn] 20f-r3: the heap allocator: %d pattern hit(s), heap table at rva 0x%08X, failed-allocation record at rva 0x%08X%s", n,
        RvaOf(g_heapTableVa), RvaOf(g_heapFailVa), (g_heapTableVa && g_heapFailVa) ? " OK" : " (NOT read - log only)");
    if (g_setHeap)
    {
        char a[160];
        char b[160];
        Log("[p2spawn] 20f-r3: heaps now: current %d; %s; %s", HeapIndexAt(g_curHeapVa), HeapText(HeapIndexAt(g_runtimeHeapVa), a, sizeof(a)),
            HeapText(HeapIndexAt(g_otherHeapVa), b, sizeof(b)));
    }
}

// ---- 20f-r2: PLAYERS 3 AND 4'S SPAWN SCREENS -------------------------------------------------------------
// 20f-r1's scan: the PC exe has no console screens for players 3 and 4 (no ifs_*3 / ifs_*4 in the screen list, none
// of their paths in the registry) and two IF managers only - so their screens are BF1GameExt's own. Each one is an
// empty RedGroupElement of the game's (class_wheel.cpp's ClassWheelGroupNew) put in the HUD root of his quarter's
// renderer view with the game's own AddToRoot(view, group, 0, 0, flag 0) - anchored at the view's top left, the root's
// safe-area offset / scale not applied (the root draw, GOG 1.3 0x0029288C): a child at x, y of the group draws at
// x / root width, y / root height of his quarter. The renderer draws roots[view] in view `view` only while that view's
// HUD flag is on (GOG 1.3 0x0028F0C8); players34.cpp turns it on while the group is shown (P2SpawnP34Root).
// In the group: player 2's screens' texts, side soldiers, class ring and blue grid (class_wheel.cpp; his ring is
// ring 1 / 2), the same flow as player 2's: side (first time in a match) -> spawn point -> class (after a death: the
// spawn point, when his respawn timer runs out), driven by his pad (players34.cpp's pads 3 and 4: d-pad / left
// stick, A, B). A on the class screen = what the PC Spawn button does, on his character (player index 2 / 3):
// SetSpawnChoice(side, class, ValidPost(post, side)), then SetReady(1) - at once (20g-r6; SpawnWaitP1=1: only while
// player 1 is in the world) (B cancels). The screen goes when his unit is in the world. Without his pad (or the group) the auto-spawn
// (AutoSpawnP34) looks after him, as before. [p34spawn] lines. [Splitscreen] SpawnP34=1.
constexpr int kF34Elems = 80; // 20g-r8: (was 16) his pause screen with its OPTIONS page has 17; 21j-r9: (was 24) 3 console windows - 36
constexpr uint32_t kCharBytes = 0xE8;     // a character (spawn_manager.cpp)
constexpr int kF34MaxLines = 250;
// places in his quarter (fractions of the view)
constexpr float kF34TitleY = 0.10f;
constexpr float kF34TitleScale = 0.80f;
constexpr float kF34PromptY = 0.93f;
constexpr float kF34PromptScale = 0.55f;
constexpr float kF34SideX[2] = { 0.30f, 0.70f };
constexpr float kF34SideFeetY = 0.74f;    // the side soldiers' feet
constexpr float kF34SideNameY = 0.83f;
constexpr float kF34SideSize = 0.34f;     // of the game's soldier (player 2's 0.54 in a root twice as tall)
constexpr float kF34NameScale = 0.60f;
constexpr float kF34PostY = 0.40f;
constexpr float kF34PostScale = 0.75f;
constexpr float kF34ArrowX[2] = { 0.08f, 0.92f };
constexpr float kF34CountY = 0.50f;
constexpr float kF34SmallScale = 0.55f;
constexpr float kF34VehY = 0.64f;
constexpr float kF34VehScale = 0.45f;
constexpr float kF34ClassArrowY = 0.60f;
// 20g-r9: Battlefront II's 4-player wheel: the picked soldier in the quarter's middle (the Xbox arrangement stands him
// sin(32 deg) of the radius right of the ring's centre), his feet two thirds down, the ring about half the quarter wide
constexpr float kRingPickSin = 0.53f;       // sin(32 deg): class_wheel.cpp's kXbRingPhase
constexpr float kF34RingRadius = 0.26f;     // of the quarter's width (r7: 0.13)
constexpr float kF34RingX = 0.50f - kF34RingRadius * kRingPickSin; // the ring's centre at the soldiers' feet
constexpr float kF34RingY = 0.67f;          // (r7: 0.80)
constexpr float kF34RingSize = 0.40f;       // the picked soldier (r7: 0.36)
constexpr float kGameSoldierRootX = 512.0f; // 20g-r9: the game's own soldier's place in root units - droidekas map around it
constexpr float kGameSoldierRootY = 520.0f;
// 20g-r6: his pause screen (a screen of F34's only). 20g-r7: the game's pause menu's look (its shots: the title and the
// items left-aligned at about 31% of the quarter, "B BACK" at the bottom left)
constexpr int kScrPause34 = 4;
constexpr int kPauseItems = 5;            // RESUME GAME, OPTIONS, RESPAWN, RESTART MISSION, QUIT
constexpr int kPauseOptions = 1;          // 20g-r8: his CONTROLLER page (greyed and skipped if his controls cannot be set)
constexpr float kF34PauseX = 0.30f;       // left edge (left-justified)
constexpr float kF34PauseTitleY = 0.25f;
constexpr float kF34PauseItemY[kPauseItems] = { 0.37f, 0.46f, 0.55f, 0.64f, 0.73f };
constexpr float kF34PauseTitleScale = 0.80f;
constexpr float kF34PauseItemScale = 0.70f;
constexpr float kF34PauseBackX = 0.07f, kF34PauseBackY = 0.92f, kF34PauseBackScale = 0.60f;
constexpr float kF34ConfirmY[2] = { 0.40f, 0.49f };
constexpr float kF34ConfirmScale = 0.60f;
constexpr float kF34ConfirmKeysY = 0.62f;
constexpr uint32_t kPauseTitleArgb = 0xFFFF8C1Eu;   // orange
constexpr uint32_t kPauseItemArgb = 0xFFDCE6F0u;    // white-blue
constexpr uint32_t kPauseSelArgb = 0xFFFFC832u;     // the selected item
constexpr uint32_t kPauseOffArgb = 0xFF6E6E6Eu;     // greyed (OPTIONS)
// 20g-r8: his OPTIONS page (CONTROLLER): the labels left at 20% of his quarter, the values centred at 80%
constexpr int kOptItems = 3;              // INVERT LOOK, INVERT FLYING, SENSITIVITY
constexpr float kF34OptLabelX = 0.20f;
constexpr float kF34OptValueX = 0.80f;
constexpr float kF34OptY[kOptItems] = { 0.40f, 0.50f, 0.60f };
constexpr float kF34OptScale = 0.62f;
constexpr float kF34OptHintY = 0.74f;
constexpr float kF34OptHintScale = 0.50f;
constexpr uint32_t kOptHintArgb = 0xFFA0A8B0u;
// 20g-r9: the game's menu bars copied (their own colours): the item picked full, the others dimmed, a greyed one darker
constexpr uint32_t kLookSelArgb = 0xFFFFFFFFu;
constexpr uint32_t kLookItemArgb = 0xFFA8A8A8u;
constexpr uint32_t kLookOffArgb = 0xFF606060u;
// 20g-r6: his spawn point screen with his map: the map in the left of his quarter, the post's texts beside it
constexpr int kF34MapLayout = 1;          // the 1-player spawn screen's layout (split screen never uses it)
constexpr float kF34MapW = 138.0f;        // its size (player 1's split layout 157 x 168, a little smaller)
constexpr float kF34MapH = 148.0f;
constexpr float kF34MapAnchorX = 0.28f;   // its centre, fractions of his quarter
constexpr float kF34MapAnchorY = 0.53f;
constexpr float kF34MapTextX = 0.72f;     // the post's name, count and vehicles
constexpr float kF34MapArrowX[2] = { 0.50f, 0.94f };
constexpr float kF34MapVehScale = 0.40f;
// 20g-r13: his screens in a horizontal half (the 3-player bottom half) - where player 2's horizontal screens have their
// pieces (fractions of the half; class_wheel.cpp's kHz* for the side screen, the ring, the map and the prompts). The
// post's and the class's texts stand on the right where the Classic Collection's panels / class window stand (0.565 -
// 0.96 across): copies of those come in a later round (HzPanelsScan logs them).
constexpr float kHzPanelX = 0.76f;                              // the panels' middle
constexpr float kHzPostY = 0.30f, kHzCountY = 0.40f, kHzVehY = 0.50f;
constexpr float kHzPostArrowX0 = 0.60f, kHzPostArrowX1 = 0.92f;
constexpr float kHzClassNameY = 0.24f;                          // Classic's class window title bar
constexpr float kHzBoxX = 0.585f, kHzBoxY = 0.50f, kHzBoxScale = 0.45f; // EQUIPMENT (left-justified, centred down)
constexpr float kHzBackX = 0.035f, kHzBackScale = 0.60f;        // "B BACK" (Classic's at 0.036 .. 0.115, 0.856 down)

struct Flow34
{
    int k = 0;                 // 0 = player 3, 1 = player 4
    uintptr_t sm = 0;          // the match this belongs to
    long sdDeletes = 0;
    bool matchReady = false;   // the spawn screen was set up in this match
    uintptr_t group = 0;       // our group, in roots[root]
    int root = -1;
    bool groupFailed = false;  // no group could be made: the auto-spawn looks after him
    bool shown = false;        // the group is shown (his view's HUD flag on)
    int screen = kScrNone;
    bool sideDone = false;     // a side was chosen this match: later screens start at the spawn point
    int pick = 0;              // side screen: 0 = side 1, 1 = side 2
    int team = 0;              // the side chosen (1 / 2)
    uintptr_t post = 0;        // the post chosen
    int cls = 0;               // the class index chosen
    int ready = 0;             // 0 choosing, 1 A pressed - waiting for player 1, 2 SetReady(1) done
    DWORD readyAt = 0;
    bool readyLogged = false;
    bool inWorld = false;      // his character had a unit last frame
    DWORD leftWorldAt = 0;     // when he lost it (0: match start)
    bool timerRuns = false;    // his respawn timer (+C0h) runs (else 1.5 s, as player 2's)
    float timerSeen = 0.0f;
    DWORD timerSeenAt = 0;
    bool timerLogged = false;
    PadEdges pad;
    bool padWas = false;       // his pad was connected (logged when it changes)
    bool owns = false;         // his screens look after his spawning (the auto-spawn stands aside)
    uintptr_t e[kF34Elems] = {};
    int n = 0;
    uintptr_t name[2] = {};    // side screen: the two sides' names
    uintptr_t model[2] = {};   //              and soldiers
    uintptr_t main = 0;        // spawn point screen: the post's name; class screen: the class name
    uintptr_t count = 0;       // spawn point screen: "2 / 5"
    uintptr_t veh = 0;         // spawn point screen: the vehicles at the post
    uintptr_t box = 0;         // 20g-r13: class screen in a horizontal half: the EQUIPMENT list (Classic's class window has it)
    bool pwOn = false;         // 21j-r9: this screen's console windows are up (main / veh / box are their texts; the ring moves left)
    bool pwFailed = false;     // 21j-r9: a window could not be made this match - the texts from then on
    uintptr_t prompt = 0;
    bool ringShown = false;    // his class ring (class_wheel.cpp's ring k + 1)
    int shownPick = -1;
    uintptr_t shownPost = 0;
    int shownCount = -1;
    int shownCls = -1;
    int shownTeam = 0;
    bool waitShown = false;
    uintptr_t vehPost = 0;
    DWORD postsAt = 0;
    bool lowHeap = false;      // 20f-r3: the runtime heap had less than 256 KB free - the auto-spawn looks after him
    bool heapsLogged = false;  // 20f-r3: the heaps logged this match
    int shows[5] = {};         // how many times each screen came up this match (20g-r6: 4 = pause)
    bool verbose = true;       // its elements are logged (its first 2 times a match)
    int lines = 0;
    // 20g-r6: his pause screen. 20g-r7: the game's items, a confirm step, the world paused while it is up
    int pauseSel = 0;          // 0 resume, 1 options (greyed), 2 respawn, 3 restart mission, 4 quit
    int shownPauseSel = -1;
    uintptr_t item[kPauseItems] = {};
    int pauses = 0;
    int confirm = 0;           // 0 none, 3 restart mission, 4 quit: "A YES / B NO" up
    uintptr_t confirmText[3] = {};
    bool worldPaused = false;  // he holds the game's world pause
    bool real = false;         // 21h-r2: his real pause menu (ifs_pausemenu3 / 4 on our manager) is up
    int pauseFrom = kScrNone;  // 21h-r6: his pause menu came up over this spawn screen (side / spawn point / class) - back to it
    uintptr_t endText = 0;     // 21h-r8: his VICTORY / DEFEAT
    bool endDone = false;      // 21h-r8: the end of the match handled (his screens closed, the calm packet on)
    // 20g-r8: his OPTIONS page (CONTROLLER), made the first time it is opened while his pause screen is up
    uintptr_t pauseTitle = 0;  // "GAME PAUSED" / "CONTROLLER"
    bool pauseLook = false;    // 20g-r9: the title and items are copies of the game's menu bars
    int sounds = 0;            // 20g-r9: his pause screen's sounds logged
    bool optsOn = false;       // OPTIONS can be chosen (his controls are written into his PlayerInput)
    bool opts = false;         // the page is up (in place of the pause items)
    bool optsMade = false;
    bool optsDirty = false;    // changed since it came up (saved to BF1GameExt.ini when it goes)
    int optSel = 0;            // 0 invert look, 1 invert flying, 2 sensitivity
    int shownOptSel = -1;
    uintptr_t optLabel[kOptItems] = {};
    uintptr_t optValue[kOptItems] = {};
    uintptr_t optHint = 0;
    PlayerNControls ctl;       // what the page shows
    // 20g-r6: his map on his spawn point screen
    uintptr_t mapObj = 0;      // his MapDisplay while it is in the spawn screen look
    bool mapOn = false;
    bool mapHz = false;        // 20g-r13: in the horizontal half's layout (kF34MapLayoutHz), not players 3 / 4's quarter one
    uint32_t mapModeWas = 0;
    int mapLayerWas = -1;
    bool mapVt = false;        // the vtable copy is in it (his post lit up)
    int mapReshows = 0;
    int mapUpdates = 0;
};
Flow34 g_f34[2];
bool g_f34Wanted = true; // [Splitscreen] SpawnP34
bool g_f34Ok = false;    // the spawn calls, NextSpawnPoint, the texts, AddToRoot and the roots were found

bool F34Budget(Flow34& f)
{
    return f.lines++ < kF34MaxLines;
}

// the character of player index p (2 / 3), or 0
uintptr_t CharOfPlayer(int p)
{
    uintptr_t chars = 0;
    uint32_t count = 0;
    if (!SpawnScanCharacters(&chars, &count))
        return 0;
    for (uint32_t c = 0; c < count && c < 64; ++c)
    {
        const uintptr_t ch = chars + static_cast<uintptr_t>(c) * kCharBytes;
        if (static_cast<int32_t>(ReadU32(ch + kCharIndex)) == p)
            return ch;
    }
    return 0;
}

// buttons already held when a screen comes up do not count as presses
void F34PadLatch(Flow34& f)
{
    const PadIn in = PadReadAt(PlayerNPadAddress(f.k + 2));
    f.pad.aDown = in.a;
    f.pad.bDown = in.b;
    f.pad.sDown = in.start; // 20g-r6
    f.pad.lDown = in.left;
    f.pad.rDown = in.right;
    f.pad.uDown = in.up;
    f.pad.dDown = in.down;
    const DWORD t = GetTickCount();
    f.pad.lSince = f.pad.rSince = f.pad.uSince = f.pad.dSince = t;
    f.pad.lLast = f.pad.rLast = f.pad.uLast = f.pad.dLast = t;
}

// (fx, fy) of his quarter in his group's units
bool F34Place(const Flow34& f, float fx, float fy, float* x, float* y)
{
    const uintptr_t root = f.root >= 0 ? RootAt(f.root) : 0;
    const int32_t w = root ? static_cast<int32_t>(ReadU32(root)) : 0;
    const int32_t h = root ? static_cast<int32_t>(ReadU32(root + 4)) : 0;
    if (w < 64 || h < 64 || w > 8192 || h > 8192)
        return false;
    *x = fx * static_cast<float>(w);
    *y = fy * static_cast<float>(h);
    return true;
}

// 20g-r13: his screens are in a horizontal half (the 3-player layout's bottom half: his root is 1024 x 312) - they take the
// Classic Collection's horizontal places (class_wheel.cpp's kHz* fractions: player 2's horizontal screens'). His group
// is anchored at his view's top left with no safe area, so the fractions are of his half as they are. [Splitscreen]
// MenusHorizontal=1.
bool F34Hz(const Flow34& f)
{
    float w = 0.0f;
    float h = 0.0f;
    return g_hzMenusWanted && f.root >= 0 && RootUsable(RootAt(f.root), &w, &h) && w >= 2.0f * h;
}

// 20g-r13 (log only, once a session each, 3-player): player 2's spawn point screen panels / class window as the game lays
// them out - what player 3's horizontal screens will copy (their trees: classes, places, turns; the first bitmaps' raw
// fields). From player 2's first time on that screen (FlowTextsMakeOnce, before the 4-player look hides / moves them).
bool g_hzPanelsScanned[2] = {};

void HzPanelsScan(int which)
{
    if (which < 0 || which > 1 || g_hzPanelsScanned[which] || PlayerNLocalPlayers() < 3)
        return;
    g_hzPanelsScanned[which] = true;
    static const char* const kPaths[2] = { "ifs_mapselect2.SideBar", "ifs_charselect2.Info" };
    {
        const char* const path = kPaths[which];
        const uintptr_t g = RegLookup(path);
        if (!g)
        {
            Log("[p34spawn] 20g-r13 scan: %s NOT in the registry", path);
            return;
        }
        float m[16] = {};
        ReadMem(g + 0x30, m, sizeof(m));
        char where[64];
        Log("[p34spawn] 20g-r13 scan: %s 0x%08X %s, anchor %.3f %.3f flag %u, at %.1f %.1f, rows %.3f %.3f %.3f / %.3f %.3f %.3f / %.3f "
            "%.3f %.3f",
            path, static_cast<unsigned>(g), WhichRoot(g, where, sizeof(where)), ReadF32(g + kElemRootX), ReadF32(g + kElemRootY),
            static_cast<unsigned>(ReadU8(g + kElemRootFlag)), ReadF32(g + kElemX), ReadF32(g + kElemY), m[0], m[1], m[2], m[4], m[5],
            m[6], m[8], m[9], m[10]);
        char what[96];
        snprintf(what, sizeof(what), "20g-r13 scan: %s (player 2's)", path);
        ClassWheelDumpTree(g, what, 0.0f, 0.0f);
        uintptr_t bm[2] = {};
        float bx[2] = {};
        float by[2] = {};
        const int nb = ClassWheelFindKind(g, "RedBitmapElement", 0.0f, 0.0f, bm, bx, by, 2);
        for (int i = 0; i < nb; ++i)
        {
            uint32_t d[56] = {};
            ReadMem(bm[i] + 0x30, d, sizeof(d));
            for (int row = 0; row < 4; ++row)
                Log("[p34spawn] 20g-r13 scan: bitmap 0x%08X (vtable rva 0x%08X) +%03Xh: %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X "
                    "%08X %08X %08X %08X",
                    static_cast<unsigned>(bm[i]), RvaOf(ReadU32(bm[i])), static_cast<unsigned>(0x30 + row * 56), d[row * 14 + 0], d[row * 14 + 1], d[row * 14 + 2],
                    d[row * 14 + 3], d[row * 14 + 4], d[row * 14 + 5], d[row * 14 + 6], d[row * 14 + 7], d[row * 14 + 8], d[row * 14 + 9],
                    d[row * 14 + 10], d[row * 14 + 11], d[row * 14 + 12], d[row * 14 + 13]);
        }
    }
}

uintptr_t F34Add(Flow34& f, uintptr_t e)
{
    if (!e)
        return 0;
    if (f.n >= kF34Elems)
    {
        ClassWheelDeleteElement(e); // (never: a screen has 10 at most)
        return 0;
    }
    f.e[f.n++] = e;
    return e;
}

uintptr_t F34Text(Flow34& f, const wchar_t* s, float fx, float fy, float scale, const char* what)
{
    float x = 0.0f;
    float y = 0.0f;
    if (!f.group || !F34Place(f, fx, fy, &x, &y))
        return 0;
    const uintptr_t e = F34Add(f, ClassWheelTextNew(f.group, s, x, y, scale));
    char fail[120] = "";
    if (!e)
        HeapFailText(fail, sizeof(fail));
    if ((f.verbose || !e) && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s text 0x%08X at %.0f %.0f (%.2f %.2f of his quarter), scale %.2f%s%s", ScanMsSinceAttach(),
            f.k + 3, what, static_cast<unsigned>(e), x, y, fx, fy, scale, e ? "" : " - NOT made; ", fail);
    return e;
}

// 20g-r7: a text with its own justify (0 left, 2 centre) and colour (0xAARRGGBB) - his pause screen
uintptr_t F34TextEx(Flow34& f, const wchar_t* s, float fx, float fy, float scale, int justify, uint32_t argb, const char* what)
{
    float x = 0.0f;
    float y = 0.0f;
    if (!f.group || !F34Place(f, fx, fy, &x, &y))
        return 0;
    const uintptr_t e = F34Add(f, ClassWheelTextNewEx(f.group, s, x, y, scale, justify, argb));
    if ((f.verbose || !e) && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s text 0x%08X at %.0f %.0f (%.2f %.2f of his quarter), scale %.2f, %s, colour %08X%s",
            ScanMsSinceAttach(), f.k + 3, what, static_cast<unsigned>(e), x, y, fx, fy, scale, justify == 0 ? "left" : "centred",
            argb, e ? "" : " - NOT made");
    return e;
}

uintptr_t F34Model(Flow34& f, uintptr_t cls, float fx, float fy, float size, const char* what)
{
    float x = 0.0f;
    float y = 0.0f;
    if (!g_soldiersWanted || !f.group || !cls || !F34Place(f, fx, fy, &x, &y))
        return 0;
    bool droideka = false;
    const uintptr_t e = F34Add(f, ClassWheelModelNew(f.group, cls, x, y, size, &droideka));
    char fail[120] = "";
    if (!e)
        HeapFailText(fail, sizeof(fail));
    if ((f.verbose || !e) && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s soldier 0x%08X%s (class 0x%08X) at %.0f %.0f (%.2f %.2f, feet), size %.2f%s%s",
            ScanMsSinceAttach(), f.k + 3, what, static_cast<unsigned>(e), droideka ? " (droideka)" : "", static_cast<unsigned>(cls), x,
            y, fx, fy, size, e ? "" : " - NOT made (ring functions / class / memory); ", fail);
    return e;
}

// his class ring's number in class_wheel.cpp (players 3 / 4: rings 1 / 2; 20g-r11: player 1's look, k -2: ring 3)
int F34Ring(const Flow34& f)
{
    return f.k >= 0 ? f.k + 1 : (f.k == -2 ? 3 : -1);
}

// our elements deleted (del: the group and the match are alive) or forgotten (left in the group, not touched)
void F34ElemsDrop(Flow34& f, bool del, const char* why)
{
    int n = 0;
    for (int i = 0; i < f.n; ++i)
    {
        if (f.e[i])
        {
            if (del)
                ClassWheelDeleteElement(f.e[i]);
            ++n;
        }
        f.e[i] = 0;
    }
    f.n = 0;
    if (f.ringShown)
        ClassWheelRingClear(F34Ring(f), del, why);
    f.ringShown = false;
    f.name[0] = f.name[1] = 0;
    f.model[0] = f.model[1] = 0;
    f.main = f.count = f.veh = f.prompt = 0;
    f.box = 0; // 20g-r13
    f.pwOn = false; // 21j-r9
    f.shownPick = -1;
    f.shownPost = 0;
    f.shownCount = -1;
    f.shownCls = -1;
    f.shownTeam = 0;
    f.waitShown = false;
    f.vehPost = 0;
    f.item[0] = f.item[1] = f.item[2] = 0; // 20g-r6
    f.shownPauseSel = -1;
    f.endText = 0; // 21h-r8
    if (n && (f.verbose || !del) && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %d element(s) %s (%s)", ScanMsSinceAttach(), f.k + 3, n,
            del ? "deleted" : "forgotten (left in the group)", why);
}

bool F34GroupLive(const Flow34& f)
{
    const uintptr_t root = (f.group && f.root >= 0) ? RootAt(f.root) : 0;
    return root && ReadU32(f.group + kElemNodeOwner) == root + kRootList;
}

// the group deleted: its destructor (GOG 1.3 0x0029FF90) only takes what is left in it out (not deleted), then
// takes itself out of its root
void F34GroupDrop(Flow34& f, const char* why)
{
    if (!f.group)
        return;
    const uintptr_t g = f.group;
    ClassWheelDeleteElement(g);
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his group 0x%08X deleted (%s)", ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(g), why);
    f.group = 0;
    f.root = -1;
    f.shown = false;
}

// 20g-r5: the group forgotten, NOT deleted: it was made in a match that is gone (its heap with it) - deleting it then
// freed memory the next match had already taken (r4: the game shut itself down right after the second match began)
void F34GroupForget(Flow34& f, const char* why)
{
    if (!f.group)
        return;
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his group 0x%08X forgotten, not deleted (%s)", ScanMsSinceAttach(), f.k + 3,
            static_cast<unsigned>(f.group), why);
    f.group = 0;
    f.root = -1;
    f.shown = false;
}

// screen and elements forgotten without touching them (a match being torn down: its soldiers' classes go with it);
// the group (ours) deleted - the elements stay out of every root. 20g-r5: the group is deleted only when mayDelete
// (the match it was made in is still loaded) and it is still in its root; otherwise it is only forgotten.
void F34MapOff(Flow34& f, const char* why, bool touch); // 20g-r6 (below)

void F34WorldPause(Flow34& f, bool on); // 20g-r7 (below)

void F34Forget(Flow34& f, const char* why, bool mayDelete)
{
    F34WorldPause(f, false); // 20g-r7: (the world pause his pause screen held)
    f.confirm = 0;
    f.pauseFrom = kScrNone;  // 21h-r6
    if (f.optsDirty) // 20g-r8: his OPTIONS page was up with changes (they are in use already; kept for next time)
        PlayerNControlsSave(f.k);
    f.optsDirty = false;
    f.opts = false;
    if (f.screen != kScrNone && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his %s screen forgotten (%s)", ScanMsSinceAttach(), f.k + 3,
            f.screen == kScrSide ? "side" : (f.screen == kScrPost ? "spawn point" : (f.screen == kScrPause34 ? "pause" : "class")),
            why);
    PlayerNSetPaused(f.k, false);     // 20g-r6
    F34MapOff(f, why, mayDelete);     // 20g-r6: his map as the HUD has it (not touched when the match is gone)
    const bool any = f.n || f.ringShown || f.screen != kScrNone;
    F34ElemsDrop(f, false, why);
    if (!mayDelete)
        F34GroupForget(f, why);
    else if (any && f.group)
    {
        if (F34GroupLive(f))
            F34GroupDrop(f, why);
        else
            F34GroupForget(f, why);
    }
    f.screen = kScrNone;
    f.shown = false;
}

// 20f-r3: room in the runtime heap (our frame has it current) for his screen's elements; if not, the auto-spawn looks
// after him this match
bool F34HeapRoom(Flow34& f, const char* what)
{
    uint32_t total = 0;
    uint32_t largest = 0;
    int blocks = 0;
    const int rt = HeapIndexAt(g_runtimeHeapVa);
    if (HeapFreeStats(rt, kF34MinFree, &total, &largest, &blocks) && total >= kF34MinFree)
        return true;
    f.lowHeap = true;
    char h[160];
    char fail[120];
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s NOT made - the runtime heap has less than %lu KB free (%s; %s) - the auto-spawn looks "
            "after him this match",
            ScanMsSinceAttach(), f.k + 3, what, static_cast<unsigned long>(kF34MinFree / 1024), HeapText(rt, h, sizeof(h)),
            HeapFailText(fail, sizeof(fail)));
    return false;
}

// his group in roots[view] (made once; again if his view moved or his root was made again)
bool F34GroupMake(Flow34& f, int view)
{
    if (f.group && (f.root != view || !F34GroupLive(f)))
    {
        // 20g-r5: deleted only while it is still in its root (memory of this match); a group that is not in its root
        // any more went with its root's children (a match ending) and is only forgotten, its elements too
        if (F34GroupLive(f))
        {
            F34ElemsDrop(f, true, "his view moved");
            F34GroupDrop(f, "his view moved");
        }
        else
        {
            F34ElemsDrop(f, false, "his group is not in its root any more");
            F34GroupForget(f, "not in its root any more");
        }
        f.screen = kScrNone;
    }
    if (f.group)
        return true;
    if (f.groupFailed || !g_addToRoot)
        return false;
    const uintptr_t root = RootAt(view);
    if (!root)
        return false;
    if (!F34HeapRoom(f, "his group"))
        return false;
    const uintptr_t g = ClassWheelGroupNew();
    if (!g)
    {
        f.groupFailed = true;
        if (F34Budget(f))
            Log("[p34spawn] player %d: no group could be made (class_wheel.cpp's [wheel] 20f line) - the auto-spawn looks after him",
                f.k + 3);
        return false;
    }
    ClassWheelShowElement(g, false);
    g_addToRoot(view, reinterpret_cast<void*>(g), 0.0f, 0.0f, 0);
    f.group = g;
    f.root = view;
    const bool live = F34GroupLive(f);
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his group 0x%08X put in roots[%d] 0x%08X (%ld x %ld, offset %.3f %.3f, scale %.3f %.3f) "
            "with AddToRoot(%d, group, 0, 0, 0): anchor %.2f %.2f, flag %u - %s",
            ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(g), view, static_cast<unsigned>(root),
            static_cast<long>(static_cast<int32_t>(ReadU32(root))), static_cast<long>(static_cast<int32_t>(ReadU32(root + 4))),
            ReadF32(root + 8), ReadF32(root + 0xC), ReadF32(root + 0x10), ReadF32(root + 0x14), view, ReadF32(g + kElemRootX),
            ReadF32(g + kElemRootY), static_cast<unsigned>(ReadU8(g + kElemRootFlag)),
            live ? "in the root OK" : "NOT in the root (left as it is) - the auto-spawn looks after him");
    if (!live)
    {
        f.groupFailed = true;
        f.group = 0;
        f.root = -1;
        return false;
    }
    return true;
}


// ---- 20g-r6: his map on his spawn point screen ---------------------------------------------------------------------
// His own MapDisplay (players34.cpp made and set it up as his HUD minimap) in the spawn screen look while his spawn
// point screen is up, as the spawn screen does player 1's: SetFlag(1) + SetMode(1), its element drawn on top of his
// root (layer 1). The layout picker (MapPickWrap) answers layout 1 for it - the 1-player spawn screen layout, which no
// split screen map uses (2+ local players: 3, 4 and 5) - and layout 1 holds our values (his quarter's left) while one
// of theirs is up, the game's own put back when none is. Its Update calls (the game's HUD task) go through the vtable
// copy player 2's map uses: his chosen post and side are lent to the spawn screen object for each call, so his post
// lights up. When the screen goes: SetMode(as it was), SetFlag(0), its layer and vtable as they were, and hidden with
// the rest of his HUD while a screen of his is still up.
float g_lay1Saved[kMapLayoutFloats] = {};
bool g_lay1Lent = false;
int g_mapLines34 = 0;

bool Map34Budget()
{
    return g_mapLines34++ < 60;
}

bool F34MapReady()
{
    return g_f34MapWanted && g_mapSetMode && g_mapSetFlag && g_mapPickThunk && g_mapTable && g_mapUpdateRva;
}

bool F34MapWill(const Flow34& f)
{
    return F34MapReady() && PlayerNHudMap(f.k) != 0;
}

bool g_p2Lay1 = false; // 20g-r7: player 2's map (4-player) is in layout 1 too

// layout 1 = ours while one of their maps is on, the game's own otherwise
void F34MapLayout1(bool on)
{
    float* const ent = reinterpret_cast<float*>(g_mapTable + kMapLayoutBytes * kF34MapLayout);
    if (on && !g_lay1Lent)
    {
        memcpy(g_lay1Saved, ent, sizeof(g_lay1Saved));
        float d[kMapLayoutFloats];
        memcpy(d, g_lay1Saved, sizeof(d));
        d[0] = kF34MapW;
        d[1] = kF34MapH;
        d[2] = d[3] = 0.0f;
        d[4] = kF34MapAnchorX;
        d[5] = kF34MapAnchorY;
        memcpy(ent, d, sizeof(d));
        g_lay1Lent = true;
        if (Map34Budget())
            Log("[p34spawn] %lu ms: map layout %d lent to players 3 / 4's spawn point screens: %.0f x %.0f at %.2f %.2f of the quarter "
                "(the game's: %.0f x %.0f %+.0f %+.0f, anchor %.3f %.3f)",
                ScanMsSinceAttach(), kF34MapLayout, d[0], d[1], d[4], d[5], g_lay1Saved[0], g_lay1Saved[1], g_lay1Saved[2],
                g_lay1Saved[3], g_lay1Saved[4], g_lay1Saved[5]);
    }
    else if (!on && g_lay1Lent && !(g_f34[0].mapOn && !g_f34[0].mapHz) && !(g_f34[1].mapOn && !g_f34[1].mapHz) && !g_p2Lay1)
    {
        memcpy(ent, g_lay1Saved, sizeof(g_lay1Saved));
        g_lay1Lent = false;
        if (Map34Budget())
            Log("[p34spawn] %lu ms: map layout %d back as the game has it", ScanMsSinceAttach(), kF34MapLayout);
    }
}

// 20g-r13: player 3's map in a horizontal half (the 3-player bottom half) - a layout of its own, lent like layout 1: layout
// 2 (the 1-player big map's other layout; with more than one local player the game's picker never gives it), set to the
// Classic Collection's map (player 2's horizontal one: 128 x 140, centred at 0.320 / 0.432 of the half) through his
// root's safe area (a root draws a map at its offset + scale x the anchor)
constexpr int kF34MapLayoutHz = 2;
float g_lay2Saved[kMapLayoutFloats] = {};
bool g_lay2Lent = false;

void F34MapLayoutHz(bool on, const Flow34* f)
{
    float* const ent = reinterpret_cast<float*>(g_mapTable + kMapLayoutBytes * kF34MapLayoutHz);
    if (on && f)
    {
        const uintptr_t root = f->root >= 0 ? RootAt(f->root) : 0;
        float ox = ReadF32(root + 8);
        float oy = ReadF32(root + 0x0C);
        float kx = ReadF32(root + 0x10);
        float ky = ReadF32(root + 0x14);
        if (!root || !(kx > 0.1f && kx <= 2.0f && ky > 0.1f && ky <= 2.0f && ox > -1.0f && ox < 1.0f && oy > -1.0f && oy < 1.0f))
        {
            ox = 0.05f; // (unreadable: Classic's)
            oy = kHzOffY;
            kx = 0.9f;
            ky = kHzScaleY;
        }
        if (!g_lay2Lent)
            memcpy(g_lay2Saved, ent, sizeof(g_lay2Saved));
        float d[kMapLayoutFloats];
        memcpy(d, g_lay2Saved, sizeof(d));
        d[0] = kHzMapW;
        d[1] = kHzMapH;
        d[2] = d[3] = 0.0f;
        d[4] = (kHzMapX - ox) / kx;
        d[5] = (kHzMapY - oy) / ky;
        memcpy(ent, d, sizeof(d));
        if (!g_lay2Lent && Map34Budget())
        {
            const float* const lay4 = reinterpret_cast<const float*>(g_mapTable + kMapLayoutBytes * 4);
            Log("[p34spawn] %lu ms: map layout %d lent to player %d's spawn point screen in his half: %.0f x %.0f, anchor %.3f %.3f (Classic's "
                "place %.3f %.3f through his root's safe area %.3f %.3f / %.3f %.3f) (the game's: %.0f x %.0f %+.0f %+.0f, anchor %.3f %.3f, "
                "[6] %.3f; layout 4's [6] %.3f)",
                ScanMsSinceAttach(), kF34MapLayoutHz, f->k + 3, d[0], d[1], d[4], d[5], kHzMapX, kHzMapY, ox, oy, kx, ky, g_lay2Saved[0],
                g_lay2Saved[1], g_lay2Saved[2], g_lay2Saved[3], g_lay2Saved[4], g_lay2Saved[5], g_lay2Saved[6], lay4[6]);
        }
        g_lay2Lent = true;
    }
    else if (!on && g_lay2Lent && !(g_f34[0].mapOn && g_f34[0].mapHz) && !(g_f34[1].mapOn && g_f34[1].mapHz))
    {
        memcpy(ent, g_lay2Saved, sizeof(g_lay2Saved));
        g_lay2Lent = false;
        if (Map34Budget())
            Log("[p34spawn] %lu ms: map layout %d back as the game has it", ScanMsSinceAttach(), kF34MapLayoutHz);
    }
}

// 20g-r7: 4-player - player 2's map on his spawn point screen in players 3 / 4's layout (the picker's answer for it)
int P2MapLayoutQ()
{
    if (!g_mapTable)
        return 4; // (no layout table: his own layout, as 2-player)
    if (!g_p2Lay1)
    {
        g_p2Lay1 = true;
        F34MapLayout1(true);
        if (Map34Budget())
            Log("[p34spawn] %lu ms: player 2's map (4-player) in layout %d - as players 3 / 4's", ScanMsSinceAttach(), kF34MapLayout);
    }
    return kF34MapLayout;
}

void P2MapLayoutQOff()
{
    if (!g_p2Lay1)
        return;
    g_p2Lay1 = false;
    F34MapLayout1(false);
}

int F34MapLayoutFor(uintptr_t map)
{
    for (const Flow34& f : g_f34)
        if (f.mapOn && f.mapObj == map && ReadU8(map + kMapFlag))
            return f.mapHz ? kF34MapLayoutHz : kF34MapLayout; // 20g-r13
    return -1;
}

// the game's Update on his map (through the vtable copy) with his post and side lent to the spawn screen object; false:
// not one of theirs (the caller goes on as before)
bool F34MapUpdate(uintptr_t map, float dt, bool* result)
{
    for (Flow34& f : g_f34)
    {
        if (!f.mapOn || f.mapObj != map)
            continue;
        const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
        const bool swap = sd != 0 && f.post != 0 && f.team >= 1 && f.team <= 2;
        uint32_t postWas = 0;
        uint32_t teamWas = 0;
        if (swap)
        {
            postWas = ReadU32(sd + kSdPost);
            teamWas = ReadU32(sd + kSdTeamShown);
            *reinterpret_cast<volatile uint32_t*>(sd + kSdPost) = static_cast<uint32_t>(f.post);
            *reinterpret_cast<volatile uint32_t*>(sd + kSdTeamShown) = static_cast<uint32_t>(f.team);
        }
        ++g_mapWrapDepth;
        *result = reinterpret_cast<MapUpdate_t>(Rva(g_mapUpdateRva))(reinterpret_cast<void*>(map), nullptr, dt);
        --g_mapWrapDepth;
        if (swap)
        {
            *reinterpret_cast<volatile uint32_t*>(sd + kSdPost) = postWas;
            *reinterpret_cast<volatile uint32_t*>(sd + kSdTeamShown) = teamWas;
        }
        if (f.mapUpdates++ < 2 && Map34Budget())
            Log("[p34spawn] %lu ms: player %d's map 0x%08X Update(%.4f) (the game's call): his post 0x%08X / side %d %s; mode %lu, flag %u, "
                "its element %s",
                ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(map), dt, static_cast<unsigned>(f.post), f.team,
                swap ? "lent to the spawn screen" : "not lent (none yet)", static_cast<unsigned long>(ReadU32(map + kMapMode)),
                static_cast<unsigned>(ReadU8(map + kMapFlag)), (ReadU32(map + kMapGroup + kElemFlags) & 0x100) ? "shown" : "hidden");
        return true;
    }
    return false;
}

// the vtable copy (player 2's map uses the same one: one class) in his map
bool F34MapWrapInstall(Flow34& f, uintptr_t map)
{
    const uintptr_t vt = ReadU32(map);
    if (vt == MapVtOurs())
        return true;
    const uintptr_t fn = vt ? ReadU32(vt + kMapVtUpdate) : 0;
    const uint32_t rva = InExe(fn) ? FollowJumps(static_cast<uint32_t>(fn - ExeBase())) : 0;
    if (!InExe(vt) || rva != g_mapUpdateRva || (g_mapVtOrig && vt != g_mapVtOrig))
    {
        if (Map34Budget())
            Log("[p34spawn] player %d's map 0x%08X: its vtable 0x%08X (Update rva 0x%08X) is not the MapDisplay one the copy is made of - "
                "his post is not lit up",
                f.k + 3, static_cast<unsigned>(map), static_cast<unsigned>(vt), rva);
        return false;
    }
    if (!g_mapVtOrig)
    {
        int n = 0;
        while (n < kMapVtMax && InCode(ReadU32(vt + 4 * static_cast<uint32_t>(n))))
            ++n;
        if (n < 2 || n >= kMapVtMax)
            return false;
        g_mapVtCopy[0] = ReadU32(vt - 4);
        for (int i = 0; i < kMapVtMax; ++i)
            g_mapVtCopy[1 + i] = i < n ? ReadU32(vt + 4 * static_cast<uint32_t>(i)) : 0;
        g_mapVtCopy[2] = reinterpret_cast<uintptr_t>(&MapUpdateWrap);
        g_mapVtSlots = n;
        g_mapVtOrig = vt;
    }
    *reinterpret_cast<volatile uintptr_t*>(map) = MapVtOurs();
    return true;
}

void F34MapWrapRemove(uintptr_t map)
{
    if (map && g_mapVtOrig && ReadU32(map) == MapVtOurs())
        *reinterpret_cast<volatile uintptr_t*>(map) = g_mapVtOrig;
}

void F34MapOn(Flow34& f, uintptr_t map)
{
    f.mapHz = F34Hz(f); // 20g-r13: in a horizontal half: its own layout (Classic's place)
    if (f.mapHz)
        F34MapLayoutHz(true, &f);
    else
        F34MapLayout1(true);
    f.mapObj = map;
    f.mapModeWas = ReadU32(map + kMapMode);
    f.mapOn = true; // (the picker answers layout 1 for it from here)
    f.mapUpdates = 0;
    f.mapReshows = 0;
    const bool shown = PlayerNHudMapShow(f.k, true);
    f.mapVt = F34MapWrapInstall(f, map);
    g_mapSetFlag(reinterpret_cast<void*>(map), nullptr, 1);
    g_mapSetMode(reinterpret_cast<void*>(map), nullptr, 1);
    f.mapLayerWas = static_cast<int>(ReadU32(map + kMapGroup + kElemFlags) & 0xFF);
    ClassWheelSetLayer(map + kMapGroup, 1);
    if (Map34Budget())
        Log("[p34spawn] %lu ms: player %d's map 0x%08X ON his spawn point screen: %s, SetFlag(1) + SetMode(1) (was mode %lu), layout %d, "
            "layer %d -> 1, %s",
            ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(map), shown ? "shown (ForPlayer)" : "NOT shown (no ForPlayer)",
            static_cast<unsigned long>(f.mapModeWas), f.mapHz ? kF34MapLayoutHz : kF34MapLayout, f.mapLayerWas,
            f.mapVt ? "his post lit up (vtable copy)" : "his post NOT lit up");
}

// touch = his map is still this match's (false: the match is gone - only forgotten)
void F34MapOff(Flow34& f, const char* why, bool touch)
{
    if (!f.mapOn)
        return;
    const uintptr_t map = f.mapObj;
    const bool hz = f.mapHz;
    f.mapOn = false; // (the picker gives the game's own answers again)
    f.mapHz = false;
    f.mapObj = 0;
    const bool same = touch && map && PlayerNHudMap(f.k) == map;
    if (same)
    {
        F34MapWrapRemove(map);
        g_mapSetMode(reinterpret_cast<void*>(map), nullptr, static_cast<int>(f.mapModeWas));
        g_mapSetFlag(reinterpret_cast<void*>(map), nullptr, 0);
        if (f.mapLayerWas >= 0)
            ClassWheelSetLayer(map + kMapGroup, f.mapLayerWas);
        if (P2SpawnP34Root(f.k) >= 0)
            PlayerNHudMapShow(f.k, false); // a screen of his is still up: hidden with the rest of his HUD
    }
    if (hz)
        F34MapLayoutHz(false, nullptr); // 20g-r13
    else
        F34MapLayout1(false);
    if (Map34Budget())
        Log("[p34spawn] %lu ms: player %d's map 0x%08X OFF (%s): %s", ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(map), why,
            same ? "SetMode(as it was), SetFlag(0), its layer and vtable back" : "left alone (not this match's any more)");
}

// each frame after his HUD was shown / hidden (players34.cpp's HudFrame)
void F34MapFrameOne(Flow34& f)
{
    const uintptr_t map = (f.shown && f.screen == kScrPost && F34MapReady()) ? PlayerNHudMap(f.k) : 0;
    if (f.mapOn && map != f.mapObj)
        F34MapOff(f, map ? "another map" : "his spawn point screen is down", true);
    if (map && !f.mapOn)
        F34MapOn(f, map);
    if (f.mapOn && !ReadU8(map + kMapEnabled))
    {
        // hidden again (his HUD hidden on the frame the screen came up): shown once more
        PlayerNHudMapShow(f.k, true);
        if (f.mapReshows++ < 3 && Map34Budget())
            Log("[p34spawn] %lu ms: player %d's map was hidden (his HUD) - shown again", ScanMsSinceAttach(), f.k + 3);
    }
}

// ---- 20g-r9: the game's pause menu's look (its FlashyTextElements: the orange title bar, the blue item bars) ----------------
struct PauseLook
{
    const char* screen = nullptr;
    int n = 0;
    uintptr_t e[8] = {};
    float x[8] = {};          // in the screen's root's units
    float y[8] = {};
    float w = 512.0f;         // that root's size
    float h = 312.0f;
};
bool g_pauseTreeLogged = false;

// player 2's pause menu (else player 1's): its FlashyTextElements, top to bottom
bool PauseLookFind(PauseLook* out)
{
    static const char* const kNames[2] = { "ifs_pausemenu2", "ifs_pausemenu" };
    for (const char* name : kNames)
    {
        const uintptr_t rec = ScreenRecord(name);
        const int32_t count = rec ? static_cast<int32_t>(ReadU32(rec + kRecordCount)) : 0;
        if (count <= 0 || count > 256)
            continue;
        PauseLook t;
        t.screen = name;
        for (int32_t i = 0; i < count && t.n < 8; ++i)
        {
            const uintptr_t top = ReadU32(rec + kRecordElems + 4 * static_cast<uint32_t>(i));
            if (!top)
                continue;
            // its root's size while it is in one (else a quarter's, 512 x 312)
            const uintptr_t owner = ReadU32(top + kElemNodeOwner);
            if (owner > kRootList)
            {
                const int32_t rw = static_cast<int32_t>(ReadU32(owner - kRootList));
                const int32_t rh = static_cast<int32_t>(ReadU32(owner - kRootList + 4));
                if (rw >= 64 && rh >= 64 && rw <= 8192 && rh <= 8192)
                {
                    t.w = static_cast<float>(rw);
                    t.h = static_cast<float>(rh);
                }
            }
            const float ax = ReadF32(top + kElemRootX) * t.w;
            const float ay = ReadF32(top + kElemRootY) * t.h;
            if (!g_pauseTreeLogged)
            {
                char what[96];
                snprintf(what, sizeof(what), "%s top-level element %d (root %.0f x %.0f)", name, static_cast<int>(i),
                         static_cast<double>(t.w), static_cast<double>(t.h));
                ClassWheelDumpTree(top, what, ax, ay);
            }
            t.n += ClassWheelFindKind(top, "FlashyTextElement", ax, ay, t.e + t.n, t.x + t.n, t.y + t.n, 8 - t.n);
        }
        // top to bottom
        for (int a = 0; a < t.n; ++a)
            for (int b = a + 1; b < t.n; ++b)
                if (t.y[b] < t.y[a])
                {
                    const uintptr_t te = t.e[a];
                    const float tx = t.x[a];
                    const float ty = t.y[a];
                    t.e[a] = t.e[b];
                    t.x[a] = t.x[b];
                    t.y[a] = t.y[b];
                    t.e[b] = te;
                    t.x[b] = tx;
                    t.y[b] = ty;
                }
        if (t.n >= 2)
        {
            *out = t;
            g_pauseTreeLogged = true;
            return true;
        }
    }
    g_pauseTreeLogged = true;
    return false;
}

// a sound of his pause screen (0 tick, 1 select, 2 back), its result logged for a while
void F34Sound(Flow34& f, int which, const char* what)
{
    const bool played = ClassWheelPlaySound(which);
    if (f.sounds++ < 24 && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: sound %s (%s) - %s", ScanMsSinceAttach(), f.k + 3,
            which == 1 ? "select" : (which == 2 ? "back" : "tick"), what, played ? "played" : "NOT played");
}

// the blue grid at his quarter's centre (as the game's screens' backdrop)
void F34Grid(Flow34& f)
{
    if (!g_backdropWanted)
        return;
    float x = 0.0f;
    float y = 0.0f;
    if (!F34Place(f, 0.5f, 0.5f, &x, &y))
        return;
    const uintptr_t cube = F34Add(f, ClassWheelCubeNew(f.group, x, y));
    if ((f.verbose || !cube) && F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: blue grid 0x%08X at %.0f %.0f (his quarter's centre)%s", ScanMsSinceAttach(), f.k + 3,
            static_cast<unsigned>(cube), x, y, cube ? "" : " - NOT made");
}

// ---- 21j-r9: THE CONSOLE WINDOWS ON THE 3 / 4-PLAYER SPAWN SCREENS ([Splitscreen] SpawnPanels34=1, [panels] lines) ------
// What the spawn point and class screens show in the game's own look (1-player and the 2-player split): its windows -
// NewButtonWindow (ifs_mapSelect.lua's COMMAND POST, VEHICLES and OBJECTIVES, ifs_charselect.lua's class window): an
// orange title bar (a FlashyTextElement: a copy of the game's own, ifs_mapselect1's COMMAND POST bar - ClassWheelFlashyLike,
// its bar as wide as the box less 37) over a blue box (9 RedBitmapElements of "border_3_pieces": 16-unit corners, the edges
// and the middle - NewButtonWindow's skin) with the text in it (left / top, wrapped in the box - ClassWheelBoxTextNew).
// Each window is built in the game's own units in a RedGroupElement of its own (the game's constructor: the class the
// game's windows are) whose rows scale it to the view (kPwScale - 19h's flat panels scale the game's the same way), its
// title bar's top left at fractions of his view. On the 3 / 4-player spawn point and class screens - players 3 / 4 (their
// quarters, player 3's bottom half with 3 players) and players 1 / 2 (their quarters, Side12Frame) - they take the place
// of the texts that stood in for them (the post's name with "<" / ">" and "2 / 5", its vehicles, the class name with "<" /
// ">"); the class ring stands left of the class window (kPwRingX). A window that cannot be made: the texts as before.
// The functions: the bitmap's constructor, its texture and its UVs as players 3 / 4's VICTORY / DEFEAT image takes them
// (the victory / defeat display's own code, 21h-r9's patterns), its SetRect from its SetSize (GOG 1.3 0x002A94E0 +9Dh:
// 0x002A92A0 - the four corners +88h / +A4h / +C0h / +DCh), RedGroupElement's constructor from RedScreenGroupElement's
// (GOG 1.3 0x00291F50 +0Ah: 0x0029FC90) - each body, vtable (RTTI) and deleting destructor's size checked.
typedef void(__fastcall* PwCtor_t)(void* self, void* unusedEdx);
typedef void(__fastcall* PwTex_t)(void* bmp, void* unusedEdx, const char* name);
typedef void(__fastcall* PwRect_t)(void* bmp, void* unusedEdx, float l, float t, float r, float b);
typedef void(__fastcall* PwUv_t)(void* bmp, void* unusedEdx, float u0, float v0, float u1, float v1, int diag);
typedef void*(__cdecl* PwNew_t)(uint32_t size);
struct PwFns
{
    PwNew_t gameNew = nullptr;
    PwCtor_t groupCtor = nullptr; // RedGroupElement::RedGroupElement (A0h bytes)
    PwCtor_t imageCtor = nullptr; // RedBitmapElement::RedBitmapElement (100h bytes)
    PwTex_t tex = nullptr;        // its texture, by name
    PwRect_t rect = nullptr;      // its corners (left, top, right, bottom)
    PwUv_t uv = nullptr;          // its texture's corners
    bool ok = false;
};
PwFns g_pw;
bool g_pwWanted = true; // [Splitscreen] SpawnPanels34
int g_pwLines = 0;
constexpr uint32_t kPwGroupSize = 0xA0;
constexpr uint32_t kPwImageSize = 0x100;
const char* const kPwSkinTexture = "border_3_pieces";
// the bar copied: COMMAND POST's (any of the game's windows' bars - the same textures, margins and font)
const char* const kPwBarLike[4] = { "ifs_mapselect1.SideBar.Spawnpoint.titleBarElement", "ifs_mapselect2.SideBar.Spawnpoint.titleBarElement",
                                    "ifs_charselect1.Info.Window.titleBarElement", "ifs_charselect2.Info.Window.titleBarElement" };
// NewButtonWindow's numbers (the game's units)
constexpr float kPwCorner = 16.0f;      // the skin's corners
constexpr float kPwTitleInset = 18.0f;  // the title's x from the box's left edge
constexpr float kPwTitleUp = 29.0f;     // the title's top above the box's top (gamefont_medium's 24 + 5)
constexpr float kPwBarLess = 37.0f;     // the bar is as wide as the box less this (its bg_width)
constexpr float kPwTextX = 10.0f;       // the box's text: its top left in the box (ifs_charselect's BoxText)
constexpr float kPwTextY = 5.0f;
const char* const kPwFont = "gamefont_small"; // (the 1-player windows' text font)
// the windows here: their boxes' width (a quarter's / a horizontal half's) and heights, the gap between two, in the game's
// units; drawn kPwScale as big; the left edge and the first title bar's top in fractions of his view
constexpr float kPwScale = 0.56f;
constexpr float kPwWQuarter = 400.0f;   // 224 of a quarter's 512: 0.535 .. 0.97 across
constexpr float kPwWHalf = 700.0f;      // 392 of a horizontal half's 1024: 0.565 .. 0.95 (where Classic's windows stand)
constexpr float kPwHPost = 30.0f;       // COMMAND POST: one line
constexpr float kPwHVeh = 60.0f;        // VEHICLES: up to three
constexpr float kPwHObj = 90.0f;        // OBJECTIVES
constexpr float kPwHClass = 130.0f;     // the class window: EQUIPMENT: and the weapons
constexpr float kPwGap = 24.0f;        // 21j-r13: 12 -> 24 (r11's video: the next window's bar stood right on the box above)
constexpr float kPwLeftQuarter = 0.535f;
constexpr float kPwLeftHalf = 0.565f;
constexpr float kPwTop = 0.17f;
// layers: the window over the soldiers, in it the box first, its title, then its text
constexpr int kPwLayerWindow = 0x20;
constexpr int kPwLayerSkin = 0x30;
constexpr int kPwLayerTitle = 0x20;
constexpr int kPwLayerText = 0x10;

bool PwBudget()
{
    return g_pwLines++ < 120;
}

bool PwReady()
{
    return g_pwWanted && g_pw.ok && ClassWheelTextsReady();
}

// the game's own window bar to copy (the first of kPwBarLike in the registry)
uintptr_t PwBarLike()
{
    for (const char* path : kPwBarLike)
    {
        const uintptr_t e = RegLookup(path);
        if (e && ClassWheelIsFlashy(e))
            return e;
    }
    return 0;
}

// f's elements from `first` on deleted (a window that could not be made whole)
void F34ElemsTrim(Flow34& f, int first)
{
    for (int i = f.n - 1; i >= first && i >= 0; --i)
    {
        if (f.e[i])
            ClassWheelDeleteElement(f.e[i]);
        f.e[i] = 0;
    }
    if (first >= 0 && first < f.n)
        f.n = first;
}

// a bitmap of the skin: (x0, y0) .. (x1, y1) of the box, the texture's (u0, v0) .. (u1, v1)
uintptr_t PwChunk(Flow34& f, uintptr_t g, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1)
{
    void* mem = g_pw.gameNew(kPwImageSize);
    if (!mem)
        return 0;
    g_pw.imageCtor(mem, nullptr);
    const uintptr_t e = reinterpret_cast<uintptr_t>(mem);
    char t[64] = "";
    if (!TypeName(e, t, sizeof(t)) || strcmp(t, "RedBitmapElement") != 0)
    {
        ClassWheelDeleteElement(e);
        return 0;
    }
    g_pw.tex(mem, nullptr, kPwSkinTexture);
    g_pw.rect(mem, nullptr, x0, y0, x1, y1);
    g_pw.uv(mem, nullptr, u0, v0, u1, v1, 0);
    ClassWheelSetLayer(e, kPwLayerSkin);
    if (!ClassWheelAddChild(g, e))
    {
        ClassWheelDeleteElement(e);
        return 0;
    }
    ClassWheelShowElement(e, true);
    return F34Add(f, e);
}

// one window in f's group: its box w x h (the game's units), its title bar's top left at (fx, fy) of his view, the title and
// the text in it; *titleOut / *textOut = its title bar and text. False (nothing of it left) if a piece could not be made.
bool PwWindow(Flow34& f, float fx, float fy, float w, float h, const wchar_t* title, const wchar_t* text, uintptr_t* titleOut,
              uintptr_t* textOut, const char* what)
{
    *titleOut = 0;
    *textOut = 0;
    const uintptr_t like = PwBarLike();
    float x = 0.0f;
    float y = 0.0f;
    const int first = f.n;
    if (!f.group || !like || !F34Place(f, fx, fy, &x, &y) || f.n + 12 > kF34Elems)
    {
        if (PwBudget())
            Log("[panels] %lu ms: player %d: %s NOT made - %s", ScanMsSinceAttach(), f.k + 3, what,
                !f.group ? "no group" : (!like ? "none of the game's window bars in the registry to copy" : (f.n + 12 > kF34Elems ? "no room in his list" : "no place in his view")));
        return false;
    }
    void* mem = g_pw.gameNew(kPwGroupSize);
    if (!mem)
        return false;
    g_pw.groupCtor(mem, nullptr);
    const uintptr_t g = reinterpret_cast<uintptr_t>(mem);
    char t[64] = "";
    if (!TypeName(g, t, sizeof(t)) || strcmp(t, "RedGroupElement") != 0)
    {
        ClassWheelDeleteElement(g);
        g_pw.ok = false; // (not what was checked at start: no more windows)
        if (PwBudget())
            Log("[panels] %lu ms: the new window group is a \"%s\", not a RedGroupElement - no console windows any more", ScanMsSinceAttach(), t);
        return false;
    }
    F34Add(f, g);
    // the box's top left at the title bar's top + its height (scaled); the game's units inside
    ClassWheelPlaceScaled(g, x, y + kPwScale * kPwTitleUp, kPwScale, kPwScale);
    ClassWheelSetLayer(g, kPwLayerWindow);
    bool ok = ClassWheelAddChild(f.group, g);
    // the skin: NewButtonWindow's 9 pieces (16-unit corners; a box under 32 tall has no middle row, as the game's)
    const float xs[4] = { 0.0f, kPwCorner, w - kPwCorner, w };
    float ys[4] = { 0.0f, kPwCorner, h - kPwCorner, h };
    if (ys[2] < ys[1])
        ys[1] = ys[2] = h * 0.5f;
    static const float kUv[4] = { 0.0f, 0.25f, 0.75f, 1.0f };
    int chunks = 0;
    for (int r = 0; r < 3 && ok; ++r)
        for (int c = 0; c < 3 && ok; ++c)
        {
            if (PwChunk(f, g, xs[c], ys[r], xs[c + 1], ys[r + 1], kUv[c], kUv[r], kUv[c + 1], kUv[r + 1]))
                ++chunks;
            else
                ok = false;
        }
    // the title bar (a copy of the game's: its font, black text, bar textures and margins), as wide as the box less 37
    uintptr_t bar = 0;
    if (ok)
    {
        bar = F34Add(f, ClassWheelFlashyLike(g, like, title, kPwTitleInset, -kPwTitleUp));
        ok = bar && ClassWheelFlashyBarWidth(bar, static_cast<int>(w - kPwBarLess));
        if (bar)
            ClassWheelSetLayer(bar, kPwLayerTitle);
    }
    // its text: left / top in the box, wrapped in it
    uintptr_t tx = 0;
    if (ok)
    {
        tx = F34Add(f, ClassWheelBoxTextNew(g, kPwFont, text, kPwTextX, kPwTextY, 1.0f, static_cast<int>(w - 2.0f * kPwTextX),
                                            static_cast<int>(h), kPwLayerText));
        ok = tx != 0;
    }
    if (!ok)
    {
        char fail[120] = "";
        HeapFailText(fail, sizeof(fail));
        F34ElemsTrim(f, first);
        if (PwBudget())
            Log("[panels] %lu ms: player %d: %s NOT made (%d of 9 box pieces, bar 0x%08X, text 0x%08X) - the texts instead; %s",
                ScanMsSinceAttach(), f.k + 3, what, chunks, static_cast<unsigned>(bar), static_cast<unsigned>(tx), fail);
        return false;
    }
    *titleOut = bar;
    *textOut = tx;
    if (f.verbose && PwBudget())
        Log("[panels] %lu ms: player %d: %s 0x%08X at %.0f %.0f (%.3f %.3f of his view, its bar's top left), box %.0f x %.0f of the game's "
            "units drawn %.2f as big (%.0f x %.0f), bar 0x%08X (copied from 0x%08X, %d wide), text 0x%08X",
            ScanMsSinceAttach(), f.k + 3, what, static_cast<unsigned>(g), x, y, fx, fy, w, h, kPwScale, w * kPwScale, h * kPwScale,
            static_cast<unsigned>(bar), static_cast<unsigned>(like), static_cast<int>(w - kPwBarLess), static_cast<unsigned>(tx));
    return true;
}

// 21j-r9: the spawn point / class screen's windows and prompts (F34Make, Side12Make); false = none made (the texts instead).
// Spawn point: COMMAND POST (f.main: the post's name), VEHICLES: (f.veh: its vehicles), OBJECTIVES: (his side's, the game's
// own getter), SELECT SPAWNPOINT. Class: the class window (f.main: its bar - the class's name; f.box: EQUIPMENT and the
// weapons), SELECT CHARACTER (and "B BACK" in a horizontal half, as before). prompt = false: no prompts (player 2's: his own).
bool F34WindowsMake(Flow34& f, uintptr_t sm, bool hz, bool prompts)
{
    if (!PwReady() || f.pwFailed || (f.screen != kScrPost && f.screen != kScrClass))
        return false;
    float vw = 0.0f;
    float vh = 0.0f;
    if (!F34Place(f, 1.0f, 1.0f, &vw, &vh) || vh < 32.0f)
        return false;
    const float w = hz ? kPwWHalf : kPwWQuarter;
    const float left = hz ? kPwLeftHalf : kPwLeftQuarter;
    const int first = f.n;
    if (f.screen == kScrPost)
    {
        wchar_t obj[0x200] = {};
        const uintptr_t map0 = g_mapArray ? ReadU32(g_mapArray) : 0;
        if (g_getObjective && map0 && f.team > 0)
            g_getObjective(reinterpret_cast<void*>(map0), nullptr, obj, static_cast<int>(sizeof(obj)), f.team);
        obj[0x1FF] = 0;
        static const wchar_t* const kTitles[3] = { L"COMMAND POST", L"VEHICLES:", L"OBJECTIVES:" };
        static const char* const kWhat[3] = { "COMMAND POST window", "VEHICLES window", "OBJECTIVES window" };
        const float hs[3] = { kPwHPost, kPwHVeh, kPwHObj };
        uintptr_t texts[3] = {};
        float top = kPwTop;
        for (int i = 0; i < 3; ++i)
        {
            uintptr_t bar = 0;
            if (!PwWindow(f, left, top, w, hs[i], kTitles[i], i == 2 ? obj : L"", &bar, &texts[i], kWhat[i]))
            {
                F34ElemsTrim(f, first);
                f.pwFailed = true;
                return false;
            }
            top += kPwScale * (kPwTitleUp + hs[i] + kPwGap) / vh;
        }
        f.main = texts[0];
        f.veh = texts[1];
        f.count = 0;
        if (f.verbose && PwBudget())
        {
            char a[300];
            Log("[panels] %lu ms: player %d: spawn point screen - COMMAND POST, VEHICLES, OBJECTIVES (side %d: \"%s\") at %.3f .. %.3f "
                "across, %.3f .. %.3f down%s",
                ScanMsSinceAttach(), f.k + 3, f.team, Narrow(obj, a, sizeof(a)), left, left + w * kPwScale / vw, kPwTop, top - kPwScale * kPwGap / vh,
                hz ? " (his horizontal half)" : "");
        }
        if (prompts)
            f.prompt = hz ? F34Text(f, L"SELECT SPAWNPOINT", kHzPromptPostX, kHzPromptY, kHzPromptScale, "prompt")
                          : F34Text(f, L"SELECT SPAWNPOINT", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    else
    {
        uintptr_t bar = 0;
        uintptr_t text = 0;
        if (!PwWindow(f, left, kPwTop, w, kPwHClass, L"", L"", &bar, &text, "class window"))
        {
            F34ElemsTrim(f, first);
            f.pwFailed = true;
            return false;
        }
        f.main = bar;
        f.box = text;
        if (prompts && hz)
        {
            F34TextEx(f, L"B BACK", kHzBackX, kHzPromptY, kHzBackScale, 0, 0xFFFFFFFFu, "B back");
            f.prompt = F34Text(f, L"SELECT CHARACTER", kHzPromptClassX, kHzPromptY, kHzPromptScale, "prompt");
        }
        else if (prompts)
            f.prompt = F34Text(f, L"SELECT CHARACTER", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    f.pwOn = true;
    return true;
}

// 21j-r9: a quarter's class ring - left of the class window while it is up
float F34RingCx(const Flow34& f)
{
    return f.pwOn ? kPwRingX : kF34RingX;
}

float F34RingR(const Flow34& f)
{
    return f.pwOn ? kPwRingRadius : kF34RingRadius;
}

// the elements of the screen that is up (once each time it comes up)
void F34Make(Flow34& f, uintptr_t sm)
{
    // 20g-r7: his pause screen is over his own view, as the game's. 20g-r9: the blue grid only when the game's look is not there
    // (on the pause screen it is made after the copies, so that copies that could not be made still get it)
    PauseLook look;
    const bool lookOk = f.screen == kScrPause34 && g_pauseLookWanted && PauseLookFind(&look);
    if (f.screen != kScrPause34)
        F34Grid(f);
    const bool hz = F34Hz(f); // 20g-r13: a horizontal half (3-player) - the Classic Collection's horizontal places
    float hw = 0.0f;
    float hh = 0.0f;
    if (hz)
        F34Place(f, 1.0f, 1.0f, &hw, &hh);
    // 21j-r9: the console windows on the spawn point and class screens (the texts below if they cannot be made)
    if ((f.screen == kScrPost || f.screen == kScrClass) && F34WindowsMake(f, sm, hz, true))
        return;
    if (f.screen == kScrSide && hz)
    {
        // as player 2's horizontal side screen: the title, the two sides' soldiers and names (no SELECT SIDE: Classic has none)
        F34Text(f, L"CHOOSE YOUR SIDE", kHzTitleX, kHzTitleY, kHzTextScale, "side title (horizontal)");
        for (int i = 0; i < 2; ++i)
        {
            wchar_t name[48];
            TeamNameW(sm, i + 1, name, 48);
            f.name[i] = F34Text(f, name, i == 0 ? kHzNameX0 : kHzNameX1, kHzNameY, kHzTextScale, i == 0 ? "side 1 name" : "side 2 name");
            f.model[i] = F34Model(f, TeamClass(sm, i + 1, 0), i == 0 ? kHzSideX0 : kHzSideX1, kHzSideFeetY,
                                  kHzSideTall * hh / kHzSoldierUnits, i == 0 ? "side 1" : "side 2");
        }
    }
    else if (f.screen == kScrPost && hz)
    {
        // as player 2's horizontal spawn point screen: his map in the left (F34MapOn, its own layout), the post on the right
        // where Classic's panels stand (the panels themselves: a later round), SELECT SPAWNPOINT bottom right
        F34Text(f, L"<", kHzPostArrowX0, kHzPostY, kF34PostScale, "left arrow");
        F34Text(f, L">", kHzPostArrowX1, kHzPostY, kF34PostScale, "right arrow");
        f.main = F34Text(f, L"", kHzPanelX, kHzPostY, kF34PostScale, "post name");
        f.count = F34Text(f, L"", kHzPanelX, kHzCountY, kF34SmallScale, "post count");
        f.veh = F34Text(f, L"", kHzPanelX, kHzVehY, kF34VehScale, "vehicles");
        f.prompt = F34Text(f, L"SELECT SPAWNPOINT", kHzPromptPostX, kHzPromptY, kHzPromptScale, "prompt");
    }
    else if (f.screen == kScrClass && hz)
    {
        // as player 2's horizontal class screen: the ring in the left (F34ClassFrame, Classic's arrangement), the class's
        // name and EQUIPMENT on the right where Classic's class window stands, B BACK bottom left, SELECT CHARACTER bottom right
        f.main = F34Text(f, L"", kHzPanelX, kHzClassNameY, kF34TitleScale, "class name");
        f.box = F34TextEx(f, L"", kHzBoxX, kHzBoxY, kHzBoxScale, 0, 0xFFFFFFFFu, "EQUIPMENT");
        F34TextEx(f, L"B BACK", kHzBackX, kHzPromptY, kHzBackScale, 0, 0xFFFFFFFFu, "B back");
        f.prompt = F34Text(f, L"SELECT CHARACTER", kHzPromptClassX, kHzPromptY, kHzPromptScale, "prompt");
    }
    else if (f.screen == kScrSide)
    {
        F34Text(f, L"CHOOSE YOUR SIDE", 0.5f, kF34TitleY, kF34TitleScale, "side title");
        for (int i = 0; i < 2; ++i)
        {
            wchar_t name[48];
            TeamNameW(sm, i + 1, name, 48);
            f.name[i] = F34Text(f, name, kF34SideX[i], kF34SideNameY, kF34NameScale, i == 0 ? "side 1 name" : "side 2 name");
            f.model[i] = F34Model(f, TeamClass(sm, i + 1, 0), kF34SideX[i], kF34SideFeetY, kF34SideSize, i == 0 ? "side 1" : "side 2");
        }
        f.prompt = F34Text(f, L"SELECT SIDE", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    else if (f.screen == kScrPost)
    {
        wchar_t side[48];
        TeamNameW(sm, f.team, side, 48);
        F34Text(f, side, 0.5f, kF34TitleY, kF34TitleScale, "side name");
        // 20g-r6: with his map (left of his quarter) the post's texts stand beside it
        const bool map = F34MapWill(f);
        const float tx = map ? kF34MapTextX : 0.5f;
        F34Text(f, L"<", map ? kF34MapArrowX[0] : kF34ArrowX[0], kF34PostY, kF34PostScale, "left arrow");
        F34Text(f, L">", map ? kF34MapArrowX[1] : kF34ArrowX[1], kF34PostY, kF34PostScale, "right arrow");
        f.main = F34Text(f, L"", tx, kF34PostY, kF34PostScale, "post name");
        f.count = F34Text(f, L"", tx, kF34CountY, kF34SmallScale, "post count");
        f.veh = F34Text(f, L"", tx, map ? (kF34CountY + kF34VehY) * 0.5f + 0.03f : kF34VehY, map ? kF34MapVehScale : kF34VehScale,
                        "vehicles");
        f.prompt = F34Text(f, L"SELECT SPAWNPOINT", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    else if (f.screen == kScrPause34) // 20g-r6; 20g-r7: the game's pause menu
    {
        // 20g-r9: the game's look - copies of its menu bars at their places (the root's units scaled to his)
        f.pauseLook = false;
        f.pauseTitle = 0;
        static const wchar_t* const kItems[kPauseItems] = { L"RESUME GAME", L"OPTIONS", L"RESPAWN", L"RESTART MISSION", L"QUIT" };
        static const char* const kWhat[kPauseItems] = { "resume game", "options", "respawn", "restart mission", "quit" };
        if (lookOk)
        {
            float qx = 0.0f;
            float qy = 0.0f;
            const bool places = look.n == kPauseItems + 1 && F34Place(f, 1.0f, 1.0f, &qx, &qy);
            const float sx = places ? qx / look.w : 1.0f;
            const float sy = places ? qy / look.h : 1.0f;
            float x = 0.0f;
            float y = 0.0f;
            if (places)
            {
                x = look.x[0] * sx;
                y = look.y[0] * sy;
            }
            else
                F34Place(f, kF34PauseX, kF34PauseTitleY, &x, &y);
            f.pauseTitle = F34Add(f, ClassWheelFlashyLike(f.group, look.e[0], L"GAME PAUSED", x, y));
            int made = f.pauseTitle ? 1 : 0;
            for (int i = 0; i < kPauseItems; ++i)
            {
                const int j = places ? i + 1 : (look.n > 1 ? 1 : 0);
                if (places)
                {
                    x = look.x[j] * sx;
                    y = look.y[j] * sy;
                }
                else
                    F34Place(f, kF34PauseX, kF34PauseItemY[i], &x, &y);
                f.item[i] = F34Add(f, ClassWheelFlashyLike(f.group, look.e[j], kItems[i], x, y));
                if (f.item[i])
                    ++made;
            }
            f.pauseLook = made == kPauseItems + 1;
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: his pause screen in the game's look (%s: %d menu bar(s) found, %s) - %d of %d "
                    "copies made%s",
                    ScanMsSinceAttach(), f.k + 3, look.screen, look.n,
                    places ? "at their places" : "not the 6 expected: at r8's places", made, kPauseItems + 1,
                    f.pauseLook ? "" : " - r8's texts instead");
            if (!f.pauseLook)
            {
                for (int i = 0; i < kPauseItems; ++i)
                    if (f.item[i])
                        ClassWheelShowElement(f.item[i], false);
                if (f.pauseTitle)
                    ClassWheelShowElement(f.pauseTitle, false);
            }
        }
        else if (g_pauseLookWanted && F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: the game's pause menu bars NOT found (ifs_pausemenu2 / ifs_pausemenu) - r8's texts on "
                "the blue grid", ScanMsSinceAttach(), f.k + 3);
        if (!f.pauseLook)
            F34Grid(f); // 20g-r9: r8's look (texts on the blue grid) when the game's could not be copied
        if (!f.pauseLook)
            f.pauseTitle =
                F34TextEx(f, L"GAME PAUSED", kF34PauseX, kF34PauseTitleY, kF34PauseTitleScale, 0, kPauseTitleArgb, "pause title");
        // 20g-r8: OPTIONS = his CONTROLLER page (made when first opened)
        f.optsOn = PlayerNControlsLive();
        f.opts = false;
        f.optsMade = false;
        f.optsDirty = false;
        for (int i = 0; i < kOptItems; ++i)
            f.optLabel[i] = f.optValue[i] = 0;
        f.optHint = 0;
        if (!f.pauseLook)
            for (int i = 0; i < kPauseItems; ++i)
                f.item[i] = F34TextEx(f, kItems[i], kF34PauseX, kF34PauseItemY[i], kF34PauseItemScale, 0,
                                      i == kPauseOptions && !f.optsOn ? kPauseOffArgb : kPauseItemArgb, kWhat[i]);
        f.prompt = F34TextEx(f, L"B BACK", kF34PauseBackX, kF34PauseBackY, kF34PauseBackScale, 0, 0xFFFFFFFFu, "B back");
        // the "are you sure" texts, hidden until RESTART MISSION / QUIT is chosen
        for (int i = 0; i < 3; ++i)
        {
            f.confirmText[i] = F34TextEx(f, L" ", 0.5f, i < 2 ? kF34ConfirmY[i] : kF34ConfirmKeysY, kF34ConfirmScale, 2,
                                         i < 2 ? 0xFFFFFFFFu : kPauseSelArgb, i < 2 ? "confirm line" : "confirm keys");
            ClassWheelShowElement(f.confirmText[i], false);
        }
        f.confirm = 0;
        f.shownPauseSel = -1;
    }
    else if (f.screen == kScrClass)
    {
        f.main = F34Text(f, L"", 0.5f, kF34TitleY, kF34TitleScale, "class name");
        F34Text(f, L"<", kF34ArrowX[0], kF34ClassArrowY, kF34PostScale, "left arrow");
        F34Text(f, L">", kF34ArrowX[1], kF34ClassArrowY, kF34PostScale, "right arrow");
        f.prompt = F34Text(f, L"SELECT CHARACTER", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
}

// ---- 20g-r7: the game's world pause, restart and quit (players 3 and 4's pause screens) --------------------------------
// The game's per-player SetPaused (GOG 1.3 0x001E70F0) pauses the world offline through 0x0013E180 (pause byte = 1) and
// lets it go through 0x0013E8F0 (= 0), each behind its "offline" test (0x001E6AD0) - found here through those calls.
// ScriptCB_RestartMission (0x0016A920) posts the restart event unless the game is in one state; ScriptCB_QuitToShell
// (0x0016AB90) calls the quit - both bodies byte-checked, the functions they call taken from them.
const char* const kPatPauseOnCalls = "E8 ?? ?? ?? ?? 84 C0 74 0A E8 ?? ?? ?? ?? E9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 85";
const char* const kPatPauseOffCalls = "E8 ?? ?? ?? ?? 84 C0 74 07 E8 ?? ?? ?? ?? EB 13 E8 ?? ?? ?? ?? 84 C0 75 0A 6A 01 E8";
const char* const kOfflineBody = "E8 ?? ?? ?? ?? 34 01 C3";
const char* const kPauseByteOnBody = "C6 05 ?? ?? ?? ?? 01 C3";
const char* const kPauseByteOffBody = "C6 05 ?? ?? ?? ?? 00 C3";
const char* const kRestartBody = "55 8B EC FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 3D ?? ?? ?? ?? 74 0D 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
                                 "83 C4 04 33 C0 5D C3";
const char* const kQuitBody = "55 8B EC FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 E8 ?? ?? ?? ?? 3D ?? ?? ?? ?? 74 05 E8 ?? ?? ?? ?? 33 C0 5D C3";
typedef bool(__cdecl* Offline_t)();
typedef void(__cdecl* PauseByte_t)();
typedef uint32_t(__cdecl* GameState_t)();
typedef void(__cdecl* PostEvent_t)(uint32_t event);
typedef void(__cdecl* Quit_t)();
bool g_worldPauseWanted = true;          // [Splitscreen] PauseWorldP34
Offline_t g_offline = nullptr;
PauseByte_t g_pauseByteOn = nullptr;
PauseByte_t g_pauseByteOff = nullptr;
uintptr_t g_pauseByte = 0;               // its address
GameState_t g_gameState = nullptr;
PostEvent_t g_postEvent = nullptr;
uint32_t g_restartEvent = 0;
uint32_t g_restartNotIn = 0;
Quit_t g_quit = nullptr;
uint32_t g_quitNotIn = 0;
long g_worldPauses = 0;
bool g_worldByteOurs = false;            // the pause byte was set by us (players 1 / 2's own pauses are left alone)
uintptr_t g_pauseObjs = 0;               // the game's per-player pause objects (pointer to them, 24h bytes each; +18h on)
const char* const kPatSetPausedObj = "55 8B EC 57 8B F9 E8 ?? ?? ?? ?? 8A 4D 08 3A C8 0F 84 ?? ?? ?? ?? 53 56 88 4F 18";
const char* const kPatSetPausedCall = "55 8B EC 8B 45 08 FF 75 0C 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 E8";
constexpr uint32_t kPauseObjBytes = 0x24;
constexpr uint32_t kPauseObjOn = 0x18;
// 20g-r9: the game's pause fades the in-game sound out and back (its SetPaused, GOG 1.3 0x001E6B30): bus = 0x00304E30
// (0x985C8F54) - cdecl; bus volume 0x00304D70 (thiscall: 1.0, volume, -1.0; ret 0Ch) - 0 at the pause (right before the
// pause byte's "on", 0x0013E180), 1 after it
const char* const kPatSndBusSite = "68 54 8F 5C 98 E8 ?? ?? ?? ?? 83 C4 04 8B F8 E8 ?? ?? ?? ?? 84 C0 0F 84";
const char* const kPatSndMuteSite =
    "83 EC 0C 8B CF C7 44 24 08 00 00 80 BF C7 44 24 04 00 00 00 00 C7 04 24 00 00 80 3F E8 ?? ?? ?? ?? E8";
const char* const kSndBusBody = "55 8B EC FF 75 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3";
const char* const kSndVolBody = "55 8B EC F3 0F 10 45 10 83 EC 0C F3 0F 11 44 24 08 F3 0F 10 45 0C F3 0F 11 44 24 04 F3 0F 10 45 08 "
                                "F3 0F 11 04 24 E8 ?? ?? ?? ?? 5D C2 0C 00";
constexpr uint32_t kSndBusHash = 0x985C8F54u;
constexpr uint32_t kSndBusCall = 5, kSndVolCall = 0x1C, kSndByteOnCall = 0x21;
typedef uintptr_t(__cdecl* SndBus_t)(uint32_t hash);
typedef void(__fastcall* SndBusVol_t)(void* bus, void* edx, float a, float volume, float c);
SndBus_t g_sndBusGet = nullptr;
SndBusVol_t g_sndBusVol = nullptr;
bool g_muteWanted = true;   // [Splitscreen] PauseMuteP34
bool g_mutedByUs = false;
long g_mutes = 0;

// player 1 / 2 (p) has the game's own pause menu up
bool GamePlayerPaused(int p)
{
    const uintptr_t objs = g_pauseObjs ? ReadU32(g_pauseObjs) : 0;
    return objs && ReadU8(objs + static_cast<uintptr_t>(p) * kPauseObjBytes + kPauseObjOn) != 0;
}

// the game's per-player pause objects (players 1 / 2's pause menus): SetPaused(player, on)'s array (20g-r7; 21j-r15: on its
// own too - the 2-player split's menu layer needs it without players 3 / 4's screens)
void PauseObjsFind()
{
    if (g_pauseObjs)
        return;
    uint32_t obj[4] = {};
    uint32_t calls[4] = {};
    const uint32_t setObj = FindPattern(kPatSetPausedObj, obj, 4) == 1 ? obj[0] : 0;
    const int nc = FindPattern(kPatSetPausedCall, calls, 4);
    for (int i = 0; i < nc && setObj && !g_pauseObjs; ++i)
        if (CallTarget(calls[i] + 20) == setObj)
            g_pauseObjs = ReadU32(Rva(calls[i] + 13));
}

// 21j-r15: this match is the game's own 2-player split (no players 3 / 4 in it)
bool P2Two()
{
    return LocalPlayers() == 2 && PlayerNLocalPlayers() < 3;
}

void F34GameFind()
{
    uint32_t h[4] = {};
    const uint32_t on = FindPattern(kPatPauseOnCalls, h, 4) == 1 ? h[0] : 0;
    const uint32_t off = FindPattern(kPatPauseOffCalls, h, 4) == 1 ? h[0] : 0;
    const uint32_t offline = on ? CallTarget(on) : 0;
    const uint32_t offline2 = off ? CallTarget(off) : 0;
    const uint32_t byteOn = on ? CallTarget(on + 9) : 0;
    const uint32_t byteOff = off ? CallTarget(off + 9) : 0;
    const bool pauseOk = offline && offline == offline2 && MatchPatternAt(offline, kOfflineBody) && byteOn &&
                         MatchPatternAt(byteOn, kPauseByteOnBody) && byteOff && MatchPatternAt(byteOff, kPauseByteOffBody) &&
                         ReadU32(Rva(byteOn + 2)) == ReadU32(Rva(byteOff + 2));
    if (pauseOk)
    {
        g_offline = reinterpret_cast<Offline_t>(Rva(offline));
        g_pauseByteOn = reinterpret_cast<PauseByte_t>(Rva(byteOn));
        g_pauseByteOff = reinterpret_cast<PauseByte_t>(Rva(byteOff));
        g_pauseByte = ReadU32(Rva(byteOn + 2));
    }
    // the game's per-player pause objects (players 1 / 2's pause menus): SetPaused(player, on)'s array
    PauseObjsFind();
    // 20g-r9: the sound bus fade the game's pause does
    {
        uint32_t b[4] = {};
        uint32_t m[4] = {};
        const uint32_t busSite = FindPattern(kPatSndBusSite, b, 4) == 1 ? b[0] : 0;
        const uint32_t muteSite = FindPattern(kPatSndMuteSite, m, 4) == 1 ? m[0] : 0;
        const uint32_t bus = busSite ? CallTarget(busSite + kSndBusCall) : 0;
        const uint32_t vol = muteSite ? CallTarget(muteSite + kSndVolCall) : 0;
        const uint32_t on = muteSite ? CallTarget(muteSite + kSndByteOnCall) : 0;
        const bool sndOk = bus && vol && MatchPatternAt(bus, kSndBusBody) && MatchPatternAt(vol, kSndVolBody) && g_pauseByteOn &&
                           Rva(on) == reinterpret_cast<uintptr_t>(g_pauseByteOn);
        if (sndOk)
        {
            g_sndBusGet = reinterpret_cast<SndBus_t>(Rva(bus));
            g_sndBusVol = reinterpret_cast<SndBusVol_t>(Rva(vol));
        }
        Log("[p2spawn] 20g-r9: the game's pause sound fade: bus site rva 0x%08X (bus rva 0x%08X), mute site rva 0x%08X (volume rva "
            "0x%08X, then the pause byte's on rva 0x%08X) - %s%s",
            busSite, bus, muteSite, vol, on, sndOk ? "OK" : "NOT OK (players 3 / 4's pause leaves the sound on)",
            g_muteWanted ? "" : " (PauseMuteP34=0: not used)");
    }
    const uint32_t restart = FindPattern(kRestartBody, h, 4) == 1 ? h[0] : 0;
    if (restart)
    {
        g_gameState = reinterpret_cast<GameState_t>(Rva(CallTarget(restart + 14)));
        g_restartNotIn = ReadU32(Rva(restart + 20));
        g_restartEvent = ReadU32(Rva(restart + 27));
        g_postEvent = reinterpret_cast<PostEvent_t>(Rva(CallTarget(restart + 31)));
    }
    const uint32_t quit = FindPattern(kQuitBody, h, 4) == 1 ? h[0] : 0;
    if (quit && CallTarget(quit + 14) == (restart ? CallTarget(restart + 14) : 0))
    {
        g_quit = reinterpret_cast<Quit_t>(Rva(CallTarget(quit + 26)));
        g_quitNotIn = ReadU32(Rva(quit + 20));
    }
    Log("[p2spawn] 20g-r7: players 3 and 4's pause screens: the world pause (SetPaused's calls rva 0x%08X / 0x%08X: offline test rva "
        "0x%08X, pause byte on rva 0x%08X / off rva 0x%08X, byte va 0x%08X) %s%s; players 1 / 2's pause objects va 0x%08X %s; "
        "RESTART MISSION (ScriptCB_RestartMission rva 0x%08X, event %08X) %s; QUIT (ScriptCB_QuitToShell rva 0x%08X) %s",
        on, off, offline, byteOn, byteOff, static_cast<unsigned>(g_pauseByte), pauseOk ? "OK" : "NOT OK",
        g_worldPauseWanted ? "" : " (PauseWorldP34=0: not used)", static_cast<unsigned>(g_pauseObjs),
        g_pauseObjs ? "OK (their pauses are left alone)" : "NOT found (the world pause is NOT used)", restart, g_restartEvent,
        g_postEvent ? "OK" : "NOT found",
        quit, g_quit ? "OK" : "NOT found");
}

// 20g-r9: the in-game sound faded out / back in as the game's pause does
void F34Mute(bool on, const char* why)
{
    if (!g_muteWanted || !g_sndBusGet || !g_sndBusVol)
        return;
    const uintptr_t bus = g_sndBusGet(kSndBusHash);
    if (!bus)
        return;
    g_sndBusVol(reinterpret_cast<void*>(bus), nullptr, 1.0f, on ? 0.0f : 1.0f, -1.0f);
    g_mutedByUs = on;
    if (g_mutes++ < 40)
        Log("[p34spawn] %lu ms: the game's sound %s (%s)", ScanMsSinceAttach(), on ? "MUTED" : "back", why);
}

// his pause screen holds the world pause (offline only, as the game's); let go when neither player 3's nor 4's holds it
void F34WorldPause(Flow34& f, bool on)
{
    if (!g_worldPauseWanted || !g_pauseByteOn || !g_pauseByte || !g_pauseObjs)
        return;
    if (on)
    {
        if (f.worldPaused || !g_offline())
            return;
        f.worldPaused = true;
        if (!ReadU8(g_pauseByte))
        {
            g_pauseByteOn();
            g_worldByteOurs = true;
        }
        if (!g_mutedByUs)
            F34Mute(true, "his pause screen"); // 20g-r9
        ++g_worldPauses;
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: THE GAME IS PAUSED (his pause screen, as players 1 / 2's)", ScanMsSinceAttach(), f.k + 3);
        return;
    }
    if (!f.worldPaused)
        return;
    f.worldPaused = false;
    const bool other = g_f34[1 - f.k].worldPaused;
    const bool theirs = GamePlayerPaused(0) || GamePlayerPaused(1); // players 1 / 2's own pause menus keep it
    if (!other && g_worldByteOurs)
    {
        if (!theirs && g_offline() && ReadU8(g_pauseByte))
            g_pauseByteOff();
        g_worldByteOurs = false;
    }
    if (!other && g_mutedByUs) // 20g-r9: (players 1 / 2's own pause brings it back when theirs goes)
    {
        if (!theirs)
            F34Mute(false, "his pause screen went");
        g_mutedByUs = false;
    }
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: the game %s", ScanMsSinceAttach(), f.k + 3,
            other ? "stays paused (player 3 / 4's other pause screen is up)"
                  : (theirs ? "stays paused (player 1's / 2's pause menu is up)" : "runs again"));
}

// each frame: a world pause of theirs held (players 1 / 2's own SetPaused(0) lets the byte go whoever set it)
void F34WorldPauseHold()
{
    if (!g_pauseByteOn || !g_pauseByte)
        return;
    if ((g_f34[0].worldPaused || g_f34[1].worldPaused) && !ReadU8(g_pauseByte) && g_offline())
    {
        g_pauseByteOn();
        g_worldByteOurs = true;
        F34Mute(true, "player 1's / 2's pause went, his is still up"); // 20g-r9: their SetPaused(0) faded it back in
    }
}

// RESTART MISSION / QUIT: what the game's script callbacks do
void F34GameAction(Flow34& f, int what)
{
    const uint32_t state = g_gameState ? g_gameState() : 0;
    if (what == 3 && g_postEvent && state != g_restartNotIn)
        g_postEvent(g_restartEvent);
    else if (what == 4 && g_quit && state != g_quitNotIn)
        g_quit();
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s (the game's state %08X)", ScanMsSinceAttach(), f.k + 3,
            what == 3 ? (g_postEvent ? "RESTART MISSION - the restart event posted" : "RESTART MISSION - not found")
                      : (g_quit ? "QUIT - quit to the shell" : "QUIT - not found"),
            state);
}

const char* F34ScreenName(int scr)
{
    return scr == kScrSide ? "side"
                           : (scr == kScrPost ? "spawn point" : (scr == kScrClass ? "class" : (scr == kScrPause34 ? "pause" : "none")));
}

void F34Close(Flow34& f, const char* why); // (below)
void RpCloseScreen(Flow34& f);              // 21h-r2 (below): his real pause menu taken off his manager

void F34Show(Flow34& f, int scr, uintptr_t sm, const char* why)
{
    f.pauseFrom = kScrNone; // 21h-r6
    if (f.optsDirty) // 20g-r8: his OPTIONS page went with the screen
        PlayerNControlsSave(f.k);
    f.optsDirty = false;
    f.opts = false;
    F34ElemsDrop(f, true, "the next screen");
    if (!F34HeapRoom(f, "his screen"))
    {
        F34Close(f, "no room in the runtime heap");
        return;
    }
    f.screen = scr;
    f.verbose = scr >= 0 && scr < 5 && ++f.shows[scr] <= 2;
    F34PadLatch(f);
    F34Make(f, sm);
    ClassWheelShowElement(f.group, true);
    f.shown = true;
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his %s screen is up in his quarter (%s) - %d element(s)", ScanMsSinceAttach(), f.k + 3,
            F34ScreenName(scr), why, f.n);
}

void F34Close(Flow34& f, const char* why)
{
    PlayerNSetPaused(f.k, false); // 20g-r6: (his pause screen, if it was the one up)
    F34WorldPause(f, false);      // 20g-r7: (the world pause his pause screen held)
    RpCloseScreen(f);             // 21h-r2: (his real pause menu, if it was the one up)
    f.confirm = 0;
    if (f.optsDirty) // 20g-r8: his OPTIONS page went with the pause screen
        PlayerNControlsSave(f.k);
    f.optsDirty = false;
    f.opts = false;
    if (f.screen == kScrNone && !f.shown)
        return;
    const int was = f.screen;
    F34ElemsDrop(f, true, why);
    if (f.group)
        ClassWheelShowElement(f.group, false);
    f.shown = false;
    f.screen = kScrNone;
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his %s screen closed (%s)", ScanMsSinceAttach(), f.k + 3, F34ScreenName(was), why);
}


// ---- 21h-r1: THE PAUSE SCAN (log only) - [Splitscreen] PauseScan34=1, [pause34] lines --------------------------------------
// What the game's pause does, per player, and where it draws (GOG 1.3 rvas; the Steam exe found by pattern at start):
//   PauseState::Update (0x001E6B30, one per player - the pause loops run it for players 1 / 2 only): Start on that player's
//   pad -> the pausing viewport (0x0053FDC4, what ScriptCB_GetPausingViewport answers) = his player index -> SetPaused.
//   PauseState::SetPaused (0x001E70F0, thiscall, its jump thunk the only way in): GetIFManager(player) -> that manager's
//   PushScreen(player < 1 ? "ifs_pausemenu" : "ifs_pausemenu2", player, player) - so player 3 or 4 would get NULL (GetIFManager
//   answers NULL past 1) and "ifs_pausemenu2".
//   A screen's activation (0x002B9CA0, from the manager's event drain): each top-level element of its record goes to
//   roots[viewport] (the record's +48h, ScriptCB_SetIFScreenViewport) - or the IF root when the viewport is -1. Deactivation
//   (0x002B9EF0) takes them out again.
//   The 2-slot limits on the way: GetIFManager and the Lua's current-manager getter (index <= 1), the active screen per Lua
//   index (get 0x00133E90 / set 0x00134BC0, an array of 2 at 0x006E9740, NULL / ignored past 1), a manager's controller
//   (0x00133F00, the options' ScriptCB calls), the IF update (0x00134DE0: input for managers 0 / 1 - cmp edi, 2 - and the tick
//   loop to 2 x 244h), PauseState[2] (0x24 each). The renderer has 4 HUD roots: a screen with viewport 2 / 3 would be drawn in
//   views 3 / 4.
// This scan: the sites above found and checked; the pause's jump thunk (SetPaused) and the activation / deactivation thunks
// go through BF1GameExt (each calls the game's function first / after, unchanged); every pause and unpause logged with both
// managers, the active screens and the pausing viewport; each pause screen's record (viewport, elements: class, root, place)
// when it comes up and 12 frames later; the 4 HUD roots (size, safe area, whose); the menu input objects of controllers 0 - 3;
// players 3 / 4's own pause screens (BF1GameExt's) with their places, to set side by side.
const char* const kPsSetPausedBody = "55 8B EC 57 8B F9 E8 ?? ?? ?? ?? 8A 4D 08 3A C8 0F 84 ?? ?? ?? ?? 53 56 88 4F 18 E8 ?? ?? ?? ?? "
                                     "FF 77 1C 8B C8 89 45 08 8B 10 FF 52 18";
// SetPaused +6Eh: push [edi+1Ch]; call GetIFManager; push bus; mov esi, eax; call; add esp, 8; mov ebx, eax; cmp [edi+18h], 0;
// je; mov ecx, [edi+1Ch]; cmp ecx, 1; push ecx; push ecx; mov ecx, esi; jge; push "ifs_pausemenu"; jmp; push "ifs_pausemenu2";
// call PushScreen
const char* const kPsNameChoice = "FF 77 1C E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B F0 E8 ?? ?? ?? ?? 83 C4 08 8B D8 80 7F 18 00 0F 84 "
                                  "?? ?? ?? ?? 8B 4F 1C 83 F9 01 51 51 8B CE 7D 07 68 ?? ?? ?? ?? EB 05 68 ?? ?? ?? ?? E8";
constexpr uint32_t kPsNameChoiceAt = 0x6E;    // from SetPaused's start
constexpr uint32_t kPsNameChoiceMgrCall = 0x03;
constexpr uint32_t kPsNameChoiceName1 = 0x30;
constexpr uint32_t kPsNameChoiceName2 = 0x37;
constexpr uint32_t kPsNameChoicePush = 0x3B;
const char* const kPsUpdateBody = "55 8B EC 56 8B F1 57 80 7E 18 00 74 13 F3 0F 10 45 08 51 F3 0F 11 04 24 E8 ?? ?? ?? ?? 83 C4 04 "
                                  "53 E8 ?? ?? ?? ?? FF 76 1C 8B F8 8B C8 8B 17 FF 52 1C";
// Update +C8h: mov eax, [esi+1Ch]; mov ecx, edi; mov [pausing viewport], eax; mov eax, [edi]; push [esi+1Ch]; call [eax+20h]
const char* const kPsViewportWrite = "8B 46 1C 8B CF A3 ?? ?? ?? ?? 8B 07 FF 76 1C FF 50 20";
constexpr uint32_t kPsViewportWriteAt = 0xC8;
constexpr uint32_t kPsViewportVar = 0x06;
// Update +10Fh: cmp byte [esi+18h], 0; mov ecx, esi; sete al; movzx eax, al; push eax; call SetPaused (its jump thunk)
const char* const kPsSetPausedCall = "80 7E 18 00 8B CE 0F 94 C0 0F B6 C0 50 E8 ?? ?? ?? ?? E8";
constexpr uint32_t kPsSetPausedCallAt = 0x0D;
const char* const kPsActivateBody = "55 8B EC 83 EC 08 8B C1 56 33 F6 89 45 F8 39 B0 4C 04 00 00 7E 7F 53 57 8D 78 4C 0F 1F 44 00 00 "
                                    "8B 40 48 8B 1F 8B CB 89 45 FC 83 F8 FF 75 2A";
const char* const kPsDeactivateBody = "53 8B D9 56 33 F6 39 B3 4C 04 00 00 7E 2F 57 8D 7B 4C 8B 4B 48 8B 07 50 83 F9 FF 75 0A E8";
// the manager's event drain (event 2): push [esi+190h]; call ActiveGet; add esp, 4; test; je; mov ecx, eax; call Activate;
// mov [esi+0F4h], 2
const char* const kPsActivateCall = "FF B6 90 01 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 07 8B C8 E8 ?? ?? ?? ?? C7 86 F4 00 00 00 "
                                    "02 00 00 00";
constexpr uint32_t kPsActivateCallAt = 0x14;
// the manager's SetScreen: mov [ebp+8], eax; call ActiveSet; mov eax, [ebp+8]; add esp, 0Ch; test; je; mov ecx, eax;
// call Deactivate; mov [edi+0F4h], 0
const char* const kPsDeactivateCall = "89 45 08 E8 ?? ?? ?? ?? 8B 45 08 83 C4 0C 85 C0 74 07 8B C8 E8 ?? ?? ?? ?? C7 87 F4 00 00 00 "
                                      "00 00 00 00";
constexpr uint32_t kPsDeactivateCallAt = 0x14;
const char* const kPsFindScreen = "55 8B EC 8B 45 08 85 C0 74 19 50 8D 4D 08 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 89 45 08 5D E9 ?? "
                                  "?? ?? ?? 33 C0 89 45 08 5D E9";
const char* const kPsActiveGet = "55 8B EC 8B 45 08 83 F8 01 77 09 8B 04 85 ?? ?? ?? ?? 5D C3 33 C0";
constexpr uint32_t kPsActiveGetArray = 0x0E;
const char* const kPsActiveSet = "55 8B EC 8B 4D 08 83 F9 01 77 0A 8B 45 0C 89 04 8D ?? ?? ?? ?? 5D C3";
constexpr uint32_t kPsActiveSetArray = 0x11;
const char* const kPsMgrCtrl = "55 8B EC 8B 45 08 83 F8 01 77 14 69 C8 44 02 00 00 A1 ?? ?? ?? ?? 8B 84 01 8C 01 00 00 5D C3";
const char* const kPsMgrInput = "55 8B EC 83 EC 1C 56 8B 75 10 57 8B F9 8B 87 94 01 00 00 85 C0 7E 0F";
// the IF update: +30h cmp edi, 2; jge (the input loop over managers 0 / 1); +BDh the input loop's end (add esi, 244h; inc edi;
// cmp esi, 488h; jl)
constexpr uint32_t kPsIfInputHeadAt = 0x30;
const char* const kPsIfInputHead = "83 FF 02 7D 30";
constexpr uint32_t kPsIfInputEndAt = 0xBD;
const char* const kPsIfInputEnd = "81 C6 44 02 00 00 47 81 FE 88 04 00 00 0F 8C";
constexpr uint32_t kPsIfInputMgrCall = 0x07; // the IF update's first call: the input manager (mov eax, imm32; ret)
const char* const kPsMovEaxImmRet = "B8 ?? ?? ?? ?? C3";
// SetPaused(player, on) / (player, x): lea ecx, [eax+eax*8]; mov eax, [pointer]; lea ecx, [eax+ecx*4] - PauseState[2]
const char* const kPsPauseCall = "55 8B EC 8B 45 08 FF 75 0C 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kPsPauseCallPtr = 0x0D;
constexpr uint32_t kPsStateSize = 0x24;
constexpr uint32_t kPsStatePaused = 0x18;   // byte
constexpr uint32_t kPsStatePlayer = 0x1C;
constexpr uint32_t kMgrState = 0xF4;        // the manager's screen state (0 set, 2 activated, 3 / 4 leaving)
constexpr uint32_t kRecName = 0x08;         // a screen record: +8 its name (inline), +48h viewport
constexpr uint32_t kRecViewport = 0x48;
constexpr int kPs34MaxLines = 1500;
constexpr int kPsOtherScreenLines = 150;    // the other screens' activations (one line each)
constexpr unsigned long kPsRedumpFrames = 12;

typedef void(__fastcall* PsSetPaused_t)(void* state, void* unusedEdx, int on);
typedef void(__fastcall* PsScreenFn_t)(void* record, void* unusedEdx);
typedef uintptr_t(__cdecl* PsFindScreen_t)(const char* name);
typedef uintptr_t(__fastcall* PsInputGet_t)(void* inputMgr, void* unusedEdx, int index);

bool g_ps34Wanted = true;          // [Splitscreen] PauseScan34
bool g_ps34On = false;
int g_ps34Lines = 0;
int g_psOtherLines = 0;
// where each thunk went before it came here (the game's function, or a chain) - written before the thunk moves (never called while 0)
volatile uintptr_t g_psSetPausedFn = 0;
volatile uintptr_t g_psActivateFn = 0;
volatile uintptr_t g_psDeactivateFn = 0;
PsFindScreen_t g_psFindScreen = nullptr;
uintptr_t g_psViewportVar = 0;     // the pausing viewport (int)
uintptr_t g_psActiveArray = 0;     // the active screen per Lua index [2]
uintptr_t g_psInputMgr = 0;        // the input manager (its vtable +20h: a controller's menu input)
uintptr_t g_psStatePtr = 0;        // -> PauseState[2]
char g_psName[2][32] = {};         // the two names SetPaused pushes
long g_psPauses = 0;
unsigned long g_psRecDue = 0;      // a pause screen's record dumped again at this frame (game thread)
char g_psRecName[64] = {};
unsigned long g_psSceneDue = 0;    // roots, inputs, the pause states and the 3 / 4 records at this frame
unsigned long g_psP34Due[2] = {};  // players 3 / 4's own pause screens, dumped at this frame
bool g_psMatchLogged = false;

bool Ps34Budget()
{
    if (g_ps34Lines >= kPs34MaxLines)
        return false;
    if (++g_ps34Lines == kPs34MaxLines)
    {
        Log("[pause34] line limit reached (%d), the pause scan goes quiet", kPs34MaxLines);
        return false;
    }
    return true;
}

int32_t PsI32(uintptr_t va)
{
    return static_cast<int32_t>(ReadU32(va));
}

// a code byte of the exe (0 outside it)
uint8_t PsCodeByte(uint32_t rva)
{
    return InExe(Rva(rva)) ? *reinterpret_cast<const unsigned char*>(Rva(rva)) : 0;
}

// an absolute address read out of the code, 0 if it is not in the exe
uintptr_t PsOperand(uint32_t rva)
{
    if (!InExe(Rva(rva)))
        return 0;
    const uintptr_t v = ReadU32(Rva(rva));
    return InExe(v) ? v : 0;
}

// jump thunks followed from rva while they stay inside the exe, stopping at stopAt (a function whose own entry another part may
// have turned into a jump out of the exe)
uint32_t PsFollow(uint32_t rva, uint32_t stopAt)
{
    for (int hop = 0; hop < 8 && rva != stopAt && PsCodeByte(rva) == 0xE9; ++hop)
    {
        const uint32_t next = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
        if (!InExe(Rva(next)))
            break;
        rva = next;
    }
    return rva;
}

// a call's target, thunks followed (see PsFollow)
uint32_t PsCallTarget(uint32_t site, uint32_t stopAt)
{
    if (PsCodeByte(site) != 0xE8)
        return 0;
    return PsFollow(static_cast<uint32_t>(static_cast<int64_t>(site) + 5 + ReadI32At(site + 1)), stopAt);
}

void PsRecordName(uintptr_t rec, char* out, size_t n)
{
    out[0] = 0;
    if (!rec || !ReadText(rec + kRecName, out, n))
        snprintf(out, n, "?");
}

// one element: its class, name, shown, layer, place in its parent; a root's own child also its place in the root and flag
void PsLogElem(const char* indent, uintptr_t e, bool rootChild)
{
    char type[64];
    char nb[96];
    char where[96];
    uintptr_t kids[2];
    const bool group = IsGroup(e);
    const int nk = group ? GroupChildren(e, kids, 2) : 0;
    const uint32_t flags = ReadU32(e + kElemFlags);
    if (!Ps34Budget())
        return;
    if (rootChild)
        Log("[pause34] %s0x%08X %s: %s, %s, layer %u, at %.1f %.1f, %s at %.1f %.1f flag %u%s%d%s", indent, static_cast<unsigned>(e),
            TypeOrDash(e, type, sizeof(type)), NameOfHash(ReadU32(e + kElemHash), nb, sizeof(nb)), (flags & 0x100) ? "shown" : "hidden",
            static_cast<unsigned>(flags & 0xFF), ReadF32(e + kElemX), ReadF32(e + kElemY), WhichRoot(e, where, sizeof(where)),
            ReadF32(e + kElemRootX), ReadF32(e + kElemRootY), static_cast<unsigned>(ReadU8(e + kElemRootFlag)), group ? ", " : "", nk,
            group ? " child(ren)" : "");
    else
        Log("[pause34] %s0x%08X %s: %s, %s, layer %u, at %.1f %.1f%s%d%s", indent, static_cast<unsigned>(e),
            TypeOrDash(e, type, sizeof(type)), NameOfHash(ReadU32(e + kElemHash), nb, sizeof(nb)), (flags & 0x100) ? "shown" : "hidden",
            static_cast<unsigned>(flags & 0xFF), ReadF32(e + kElemX), ReadF32(e + kElemY), group ? ", " : "", nk,
            group ? " child(ren)" : "");
}

// a group's tree, `depth` levels down (up to 16 children a level)
void PsLogTree(uintptr_t group, int depth, int level)
{
    if (depth <= 0 || !IsGroup(group))
        return;
    uintptr_t kids[16];
    const int n = GroupChildren(group, kids, 16);
    static const char* const kIndent[] = { "      ", "        ", "          ", "            " };
    const char* const ind = kIndent[level < 3 ? level : 3];
    for (int i = 0; i < n && i < 16; ++i)
    {
        PsLogElem(ind, kids[i], false);
        PsLogTree(kids[i], depth - 1, level + 1);
    }
    if (n > 16 && Ps34Budget())
        Log("[pause34] %s(%d more)", ind, n - 16);
}

// a screen record: viewport, its top-level elements (where they went), their children 3 levels down
void PsLogRecord(uintptr_t rec, const char* why)
{
    if (!rec)
        return;
    char name[64];
    PsRecordName(rec, name, sizeof(name));
    const int vp = PsI32(rec + kRecViewport);
    const int n = PsI32(rec + kRecordCount);
    if (Ps34Budget())
        Log("[pause34] %lu ms: screen \"%s\" (record 0x%08X) %s: viewport %d (%s), %d top-level element(s)", ScanMsSinceAttach(), name,
            static_cast<unsigned>(rec), why, vp, vp < 0 ? "the IF root - every player's" : "its elements go to roots[viewport]", n);
    for (int i = 0; i < n && i < 24; ++i)
    {
        const uintptr_t e = ReadU32(rec + kRecordElems + 4 * static_cast<uint32_t>(i));
        if (!e)
            continue;
        PsLogElem("    ", e, true);
        PsLogTree(e, 3, 0);
    }
}

// the 4 HUD roots: size, safe area, children, whose
void PsLogRoots(const char* why)
{
    if (Ps34Budget())
        Log("[pause34] %lu ms: HUD roots (%s): player 1 -> roots[%d], player 2 -> roots[%d], player 3's view %d (his screens in "
            "roots[%d]), player 4's view %d (roots[%d]); %d local player(s)",
            ScanMsSinceAttach(), why, PlayerRootIndex(0), PlayerRootIndex(1), PlayerNSpecView(0), P2SpawnP34Root(0), PlayerNSpecView(1),
            P2SpawnP34Root(1), PlayerNLocalPlayers());
    for (int k = 0; k < 4; ++k)
    {
        const uintptr_t r = RootAt(k);
        if (!r)
            continue;
        uintptr_t kids[2];
        const int n = RootChildren(r, kids, 2);
        if (Ps34Budget())
            Log("[pause34]   roots[%d] 0x%08X: size %ld x %ld, safe area offset %.3f %.3f scale %.3f %.3f, %d child(ren)", k,
                static_cast<unsigned>(r), static_cast<long>(PsI32(r)), static_cast<long>(PsI32(r + 4)), ReadF32(r + 8),
                ReadF32(r + 0x0C), ReadF32(r + 0x10), ReadF32(r + 0x14), n);
    }
}

void PsLogManagers(const char* why)
{
    char top[2][64];
    char act[2][64];
    for (int i = 0; i < 2; ++i)
    {
        TopName(Manager(i), top[i], sizeof(top[i]));
        const uintptr_t a = g_psActiveArray ? ReadU32(g_psActiveArray + 4 * static_cast<uint32_t>(i)) : 0;
        if (a)
            PsRecordName(a, act[i], sizeof(act[i]));
        else
            snprintf(act[i], sizeof(act[i]), "-");
    }
    const uintptr_t m0 = Manager(0);
    const uintptr_t m1 = Manager(1);
    if (Ps34Budget())
        Log("[pause34]   managers (%s): 0 top \"%s\" depth %ld controller %ld index %ld state %ld countdown %ld | 1 top \"%s\" depth %ld "
            "controller %ld index %ld state %ld countdown %ld | active screen per Lua index: 0 \"%s\", 1 \"%s\" | the Lua's current "
            "index %ld | pausing viewport %ld",
            why, top[0], static_cast<long>(PsI32(m0 + kMgrDepth)), static_cast<long>(PsI32(m0 + kMgrPlayer)),
            static_cast<long>(PsI32(m0 + kMgrIndex)), static_cast<long>(PsI32(m0 + kMgrState)), static_cast<long>(PsI32(m0 + kMgrCountdown)),
            top[1], static_cast<long>(PsI32(m1 + kMgrDepth)), static_cast<long>(PsI32(m1 + kMgrPlayer)),
            static_cast<long>(PsI32(m1 + kMgrIndex)), static_cast<long>(PsI32(m1 + kMgrState)), static_cast<long>(PsI32(m1 + kMgrCountdown)),
            act[0], act[1], g_luaIndexAddr ? static_cast<long>(PsI32(g_luaIndexAddr)) : -99L,
            g_psViewportVar ? static_cast<long>(PsI32(g_psViewportVar)) : -99L);
}

// the pause states (2 of the game's) and what a third would read
void PsLogStates()
{
    const uintptr_t arr = g_psStatePtr ? ReadU32(g_psStatePtr) : 0;
    if (!arr)
        return;
    for (int i = 0; i < 3; ++i)
    {
        const uintptr_t s = arr + static_cast<uintptr_t>(i) * kPsStateSize;
        if (Ps34Budget())
            Log("[pause34]   PauseState[%d]%s 0x%08X: paused %u, +19h %u, player %ld, +20h %08X", i,
                i < 2 ? "" : " (past the game's 2: what player 3's would read)", static_cast<unsigned>(s),
                static_cast<unsigned>(ReadU8(s + kPsStatePaused)), static_cast<unsigned>(ReadU8(s + kPsStatePaused + 1)),
                static_cast<long>(PsI32(s + kPsStatePlayer)), static_cast<unsigned>(ReadU32(s + 0x20)));
    }
}

// the menu input of controllers 0 - 3 (the input manager's vtable +20h - the IF update reads a manager's controller this way;
// BF1GameExt answers 1 - 3 with players 2 - 4's objects while that is on)
void PsLogInputs()
{
    if (!g_psInputMgr)
        return;
    const uintptr_t vt = ReadU32(g_psInputMgr);
    const uintptr_t fn = vt ? ReadU32(vt + 0x20) : 0;
    if (!fn)
        return;
    const bool exeFn = fn >= ExeBase() && RvaOf(fn) != 0;
    char line[512];
    size_t at = 0;
    for (int i = 0; i < 4; ++i)
    {
        const uintptr_t in = reinterpret_cast<PsInputGet_t>(fn)(reinterpret_cast<void*>(g_psInputMgr), nullptr, i);
        const bool own = in >= g_psInputMgr && in < g_psInputMgr + 0x8BC;
        const int w = snprintf(line + at, sizeof(line) - at, "%s%d: 0x%08X%s start %.2f / %.2f back %.2f", i ? " | " : "", i,
                               static_cast<unsigned>(in), own ? (i == 0 ? " (the game's)" : " (inside the game's 1-slot array: past it)") : " (BF1GameExt's)",
                               ReadF32(in + 0x70), ReadF32(in + 0xD8), ReadF32(in + 0x74));
        if (w > 0 && static_cast<size_t>(w) < sizeof(line) - at)
            at += static_cast<size_t>(w);
    }
    if (Ps34Budget())
        Log("[pause34]   menu input per controller (input manager vtable +20h %s 0x%08X): %s", exeFn ? "-> the game's, rva" : "-> BF1GameExt",
            exeFn ? RvaOf(fn) : static_cast<unsigned>(fn), line);
}

void PsLogScene(const char* why)
{
    PsLogRoots(why);
    PsLogStates();
    PsLogInputs();
    if (!g_psFindScreen)
        return;
    static const char* const kNames[] = { "ifs_pausemenu", "ifs_pausemenu2", "ifs_pausemenu3", "ifs_pausemenu4" };
    for (const char* nm : kNames)
    {
        const uintptr_t rec = g_psFindScreen(nm);
        if (!rec)
        {
            if (Ps34Budget())
                Log("[pause34]   screen \"%s\": no record in the screen list", nm);
            continue;
        }
        PsLogRecord(rec, why);
    }
}

// PauseState::SetPaused through its jump thunk: the game's first, then what it did
void __fastcall PsSetPausedWrap(void* state, void* unusedEdx, int on)
{
    const uintptr_t s = reinterpret_cast<uintptr_t>(state);
    const uint32_t from = RvaOf(reinterpret_cast<uintptr_t>(BF1_RETURN_ADDRESS()));
    const int player = static_cast<int>(PsI32(s + kPsStatePlayer));
    const int was = ReadU8(s + kPsStatePaused);
    const long vp = g_psViewportVar ? static_cast<long>(PsI32(g_psViewportVar)) : -99L;
    reinterpret_cast<PsSetPaused_t>(g_psSetPausedFn)(state, unusedEdx, on);
    ++g_psPauses;
    const bool pauseOn = (on & 0xFF) != 0;
    if (Ps34Budget())
        Log("[pause34] %lu ms: PauseState[player %d]::SetPaused(%d) from rva 0x%08X (paused was %d, now %u; pausing viewport %ld) - "
            "the game pushes / pops \"%s\" on manager %d",
            ScanMsSinceAttach(), player, pauseOn ? 1 : 0, from, was, static_cast<unsigned>(ReadU8(s + kPsStatePaused)), vp,
            player < 1 ? g_psName[0] : g_psName[1], player);
    PsLogManagers(pauseOn ? "after the pause" : "after the unpause");
    if (pauseOn)
        g_psSceneDue = g_gt.frame + kPsRedumpFrames;
}

bool PsIsPauseScreen(const char* name)
{
    return strncmp(name, "ifs_pause", 9) == 0;
}

// a screen's activation: the game's first (its elements go to their root), then where they went
void __fastcall PsActivateWrap(void* record, void* unusedEdx)
{
    reinterpret_cast<PsScreenFn_t>(g_psActivateFn)(record, unusedEdx);
    const uintptr_t rec = reinterpret_cast<uintptr_t>(record);
    char name[64];
    PsRecordName(rec, name, sizeof(name));
    if (PsIsPauseScreen(name))
    {
        PsLogRecord(rec, "activated");
        snprintf(g_psRecName, sizeof(g_psRecName), "%s", name);
        g_psRecDue = g_gt.frame + kPsRedumpFrames;
        return;
    }
    if (g_psOtherLines < kPsOtherScreenLines && Ps34Budget())
    {
        ++g_psOtherLines;
        Log("[pause34] %lu ms: screen \"%s\" activated: viewport %ld, %ld top-level element(s)", ScanMsSinceAttach(), name,
            static_cast<long>(PsI32(rec + kRecViewport)), static_cast<long>(PsI32(rec + kRecordCount)));
    }
}

void __fastcall PsDeactivateWrap(void* record, void* unusedEdx)
{
    const uintptr_t rec = reinterpret_cast<uintptr_t>(record);
    char name[64];
    PsRecordName(rec, name, sizeof(name));
    if (PsIsPauseScreen(name) && Ps34Budget())
        Log("[pause34] %lu ms: screen \"%s\" deactivated (viewport %ld): its elements leave their root", ScanMsSinceAttach(), name,
            static_cast<long>(PsI32(rec + kRecViewport)));
    reinterpret_cast<PsScreenFn_t>(g_psDeactivateFn)(record, unusedEdx);
}

// a call site's jump thunk retargeted to `wrap`; *orig = where the thunk went (the game's function, or a chain). The thunk must
// lead to `fn` (or out of the exe: another part's wrapper, chained).
bool PsHookThunk(const char* what, uint32_t site, uint32_t fn, const void* wrap, volatile uintptr_t* orig)
{
    if (!site || !fn || PsCodeByte(site) != 0xE8)
    {
        Log("[pause34] %s: no call at the site - NOT hooked", what);
        return false;
    }
    const uint32_t thunk = static_cast<uint32_t>(static_cast<int64_t>(site) + 5 + ReadI32At(site + 1));
    if (PsCodeByte(thunk) != 0xE9)
    {
        Log("[pause34] %s: the call at rva 0x%08X does not go to a jump thunk (rva 0x%08X) - NOT hooked", what, site, thunk);
        return false;
    }
    const uintptr_t dest = Rva(thunk) + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(ReadI32At(thunk + 1)));
    const bool inExe = InExe(dest);
    const uint32_t end = inExe ? PsFollow(RvaOf(dest), fn) : 0;
    if (inExe && end != fn)
    {
        Log("[pause34] %s: the thunk rva 0x%08X leads to rva 0x%08X, not the function rva 0x%08X - NOT hooked", what, thunk, end, fn);
        return false;
    }
    *orig = dest;
    if (!RetargetJump(Rva(thunk), wrap))
    {
        *orig = 0;
        Log("[pause34] %s: the thunk rva 0x%08X could not be written - NOT hooked", what, thunk);
        return false;
    }
    Log("[pause34] %s: call at rva 0x%08X -> thunk rva 0x%08X now goes through BF1GameExt (then %s) OK", what, site, thunk,
        inExe ? "the game's function" : "the wrapper that was there: CHAINED");
    return true;
}

// 21h-r12: the game's mission queue (the next map after the stats: Instant Action's list, the campaign's next mission). Its
// "load the next one" (GOG 1.3 0x0018F8B0) starts: mov eax, [index]; cmp eax, [count]; jl; cmp byte [loop], 0; je; xor eax,
// eax; mov [index], eax - with the index past the count and the loop flag 0 it answers "none", and the game goes to the shell
// state (the main menu) once the stats are left (GOG 1.3 0x00140968 -> 0x00140999). QUIT on the stats screen sets that.
const char* const kMqNextBody = "A1 ?? ?? ?? ?? 3B 05 ?? ?? ?? ?? 7C 22 80 3D ?? ?? ?? ?? 00 74 ?? 33 C0 A3";
constexpr uint32_t kMqIndex = 0x01; // mov eax, [index]
constexpr uint32_t kMqCount = 0x07; // cmp eax, [count]
constexpr uint32_t kMqLoop = 0x0F;  // cmp byte ptr [loop], 0
constexpr uint32_t kMqStore = 0x19; // mov [index], eax
uintptr_t g_mqIndex = 0;
uintptr_t g_mqCount = 0;
uintptr_t g_mqLoop = 0;
// 21i-r5: the game's game-over flag (PauseState::Update +9Ah calls its getter "mov al, [flag] / ret"; the win sets it to 1, the
// next mission's start puts it back to 0). The victory / defeat display can be made again at the same address (a RESTART /
// DONE), so its address alone does not tell a new match: the end is over when this flag goes back to 0.
constexpr uint32_t kPsGameOverCallAt = 0x9A;
const char* const kPsGameOverCall = "84 C0 0F 85 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 75"; // at +92h: the call at +8 of it
constexpr uint32_t kPsGameOverCallIn = 8;
const char* const kGameOverGetter = "A0 ?? ?? ?? ?? C3";
uintptr_t g_gameOverFlag = 0;
bool g_gameOverSeen = false;          // the flag seen at 1 since the end went on

void Ps34Install()
{
    if (!g_ps34Wanted)
    {
        Log("[pause34] [Splitscreen] PauseScan34=0 - the pause scan is not installed");
        return;
    }
    Log("[pause34] split_spawn_menus.cpp file build %s - THE PAUSE SCAN (21h-r1, log only): how the game's pause reaches a player's IF "
        "manager and where each pause screen is drawn - for players 3 / 4's real pause menus. [Splitscreen] PauseScan34=1",
        kFileBuild);
    uint32_t hits[4] = {};
    auto one = [&](const char* what, const char* pat) -> uint32_t {
        const int n = FindPattern(pat, hits, 4);
        if (n == 1)
            return hits[0];
        Log("[pause34] %s: %d pattern hit(s) (need exactly 1) - NOT FOUND", what, n);
        return 0u;
    };
    const uint32_t setPaused = one("PauseState::SetPaused", kPsSetPausedBody);
    const uint32_t update = one("PauseState::Update", kPsUpdateBody);
    const uint32_t activate = one("screen activation", kPsActivateBody);
    const uint32_t deactivate = one("screen deactivation", kPsDeactivateBody);
    const uint32_t findScreen = one("FindScreen", kPsFindScreen);
    const uint32_t activeGet = one("active screen get", kPsActiveGet);
    const uint32_t activeSet = one("active screen set", kPsActiveSet);
    const uint32_t mgrCtrl = one("a manager's controller", kPsMgrCtrl);
    const uint32_t mgrInput = one("a manager's input step", kPsMgrInput);
    const uint32_t actCall = one("the drain's activation call", kPsActivateCall);
    const uint32_t deactCall = one("SetScreen's deactivation call", kPsDeactivateCall);
    const int np = FindPattern(kPsPauseCall, hits, 4);
    if (np == 2 && PsOperand(hits[0] + kPsPauseCallPtr) == PsOperand(hits[1] + kPsPauseCallPtr))
        g_psStatePtr = PsOperand(hits[0] + kPsPauseCallPtr);
    Log("[pause34] (5) the pause states: SetPaused(player, ...) %d of 2 pattern hits, pointer %s rva 0x%08X -> PauseState[2] (0x%X each)",
        np, g_psStatePtr ? "OK" : "NOT FOUND", RvaOf(g_psStatePtr), kPsStateSize);
    // 21h-r12: the mission queue (QUIT on the stats screen -> the main menu): every hit must give the same index / count / loop,
    // the store must be the index, and they must sit as the game has them (index = count + 4, loop = count + 9)
    {
        const int nq = FindPattern(kMqNextBody, hits, 4);
        bool same = nq >= 1 && nq <= 4;
        uintptr_t ix = 0;
        uintptr_t ct = 0;
        uintptr_t lp = 0;
        for (int i = 0; i < nq && i < 4 && same; ++i)
        {
            const uintptr_t a = PsOperand(hits[i] + kMqIndex);
            const uintptr_t b = PsOperand(hits[i] + kMqCount);
            const uintptr_t c = PsOperand(hits[i] + kMqLoop);
            if (!a || !b || !c || PsOperand(hits[i] + kMqStore) != a)
                same = false;
            else if (i == 0)
            {
                ix = a;
                ct = b;
                lp = c;
            }
            else if (a != ix || b != ct || c != lp)
                same = false;
        }
        if (same && ix == ct + 4 && lp == ct + 9)
        {
            g_mqIndex = ix;
            g_mqCount = ct;
            g_mqLoop = lp;
        }
        Log("[pause34] 21h-r12: the mission queue: %d pattern hit(s), count rva 0x%08X, index rva 0x%08X, loop flag rva 0x%08X - %s", nq,
            RvaOf(ct), RvaOf(ix), RvaOf(lp),
            g_mqIndex ? "OK: QUIT on the stats screen goes to the main menu" : "NOT FOUND - QUIT on the stats screen does nothing more than the game's");
    }

    // 21i-r5: the game's game-over flag, from PauseState::Update's call to its getter
    {
        const uint32_t at = update ? update + kPsGameOverCallAt - kPsGameOverCallIn : 0;
        const bool siteOk = at && MatchPatternAt(at, kPsGameOverCall);
        const uint32_t getter = siteOk ? PsCallTarget(at + kPsGameOverCallIn, 0) : 0;
        const bool getterOk = getter && MatchPatternAt(getter, kGameOverGetter);
        g_gameOverFlag = getterOk ? PsOperand(getter + 1) : 0;
        Log("[pause34] 21i-r5: the game-over flag: PauseState::Update +%02Xh %s -> getter rva 0x%08X %s -> flag rva 0x%08X - %s",
            kPsGameOverCallAt, siteOk ? "OK" : "does NOT match", getter, getterOk ? "OK" : "does NOT match", RvaOf(g_gameOverFlag),
            g_gameOverFlag ? "OK: a RESTART / DONE starts the next match clean (players 2 - 4 spawn again)"
                           : "NOT FOUND - the end is only cleared at the match teardown");
    }

    // (1) PauseState::Update: Start -> the pausing viewport -> SetPaused
    const bool vpOk = update && MatchPatternAt(update + kPsViewportWriteAt, kPsViewportWrite);
    if (vpOk)
        g_psViewportVar = PsOperand(update + kPsViewportWriteAt + kPsViewportVar);
    const bool callOk = update && MatchPatternAt(update + kPsViewportWriteAt + 0x47, kPsSetPausedCall);
    const uint32_t spSite = callOk ? update + kPsViewportWriteAt + 0x47 + kPsSetPausedCallAt : 0;
    const bool spCallOk = spSite && setPaused && PsCallTarget(spSite, setPaused) == setPaused;
    Log("[pause34] (1) PauseState::Update rva 0x%08X: Start on the player's pad -> the pausing viewport (rva 0x%08X, %s) = his player "
        "index (+1Ch) -> SetPaused(not paused) at rva 0x%08X %s; the game's pause loops run it for players 1 / 2 only",
        update, RvaOf(g_psViewportVar), vpOk ? "OK" : "does NOT match", spSite, spCallOk ? "-> SetPaused OK" : "does NOT check out");

    // (2) SetPaused: GetIFManager(player) -> PushScreen(name by player, player, player)
    const bool choiceOk = setPaused && MatchPatternAt(setPaused + kPsNameChoiceAt, kPsNameChoice);
    bool namesOk = false;
    if (choiceOk)
    {
        ReadText(ReadU32(Rva(setPaused + kPsNameChoiceAt + kPsNameChoiceName1)), g_psName[0], sizeof(g_psName[0]));
        ReadText(ReadU32(Rva(setPaused + kPsNameChoiceAt + kPsNameChoiceName2)), g_psName[1], sizeof(g_psName[1]));
        namesOk = strcmp(g_psName[0], "ifs_pausemenu") == 0 && strcmp(g_psName[1], "ifs_pausemenu2") == 0;
    }
    const uintptr_t pushFn = SpawnUiPushScreenFn();
    const uint32_t getMgr = choiceOk ? PsCallTarget(setPaused + kPsNameChoiceAt + kPsNameChoiceMgrCall, 0) : 0;
    const uint32_t push = choiceOk ? PsCallTarget(setPaused + kPsNameChoiceAt + kPsNameChoicePush, RvaOf(pushFn)) : 0;
    Log("[pause34] (2) PauseState::SetPaused rva 0x%08X: GetIFManager(player) rva 0x%08X %s -> PushScreen(player < 1 ? \"%s\" : \"%s\", "
        "player, player) rva 0x%08X %s - names %s; player 3 / 4 would get no manager (GetIFManager: NULL past 1) and \"%s\"",
        setPaused, getMgr, (getMgr && MatchPatternAt(getMgr, kGetIfManager)) ? "OK" : "does NOT match", g_psName[0], g_psName[1], push,
        (pushFn && push == RvaOf(pushFn)) ? "= spawn_ui_scan's PushScreen OK" : "(not spawn_ui_scan's PushScreen?)",
        namesOk ? "OK" : "do NOT match", g_psName[1]);

    // (3) the managers' 2-slot limits
    const uint32_t ifUpdate = g_ifUpdateRva;
    const bool inHeadOk = ifUpdate && MatchPatternAt(ifUpdate + kPsIfInputHeadAt, kPsIfInputHead);
    const bool inEndOk = ifUpdate && MatchPatternAt(ifUpdate + kPsIfInputEndAt, kPsIfInputEnd);
    const bool tickOk = ifUpdate && MatchPatternAt(ifUpdate + kIfUpdateTickAt, kIfUpdateTickSite);
    const uint32_t imFn = ifUpdate ? PsCallTarget(ifUpdate + kPsIfInputMgrCall, 0) : 0;
    if (imFn && MatchPatternAt(imFn, kPsMovEaxImmRet))
    {
        const uintptr_t im = ReadU32(Rva(imFn + 1));
        if (InExe(im))
            g_psInputMgr = im;
    }
    Log("[pause34] (3) the IF update rva 0x%08X: input for managers 0 / 1 (cmp edi, 2 at +30h %s; the loop's end at +BDh, 2 x 244h, "
        "%s), each manager ticked to 2 x 244h (+E7h %s); the input manager rva 0x%08X %s; a manager's input step rva 0x%08X, its "
        "controller getter rva 0x%08X (NULL past 1)",
        ifUpdate, inHeadOk ? "OK" : "does NOT match", inEndOk ? "OK" : "does NOT match", tickOk ? "OK" : "does NOT match",
        RvaOf(g_psInputMgr), g_psInputMgr ? "OK" : "NOT FOUND", mgrInput, mgrCtrl);
    const uintptr_t arrGet = activeGet ? PsOperand(activeGet + kPsActiveGetArray) : 0;
    const uintptr_t arrSet = activeSet ? PsOperand(activeSet + kPsActiveSetArray) : 0;
    if (arrGet && arrGet == arrSet)
        g_psActiveArray = arrGet;
    Log("[pause34] (4) the active screen per Lua index: get rva 0x%08X / set rva 0x%08X, the array rva 0x%08X [2] %s (NULL / ignored past "
        "index 1); FindScreen rva 0x%08X",
        activeGet, activeSet, RvaOf(arrGet), g_psActiveArray ? "OK (the same in both)" : "does NOT check out", findScreen);
    if (findScreen)
        g_psFindScreen = reinterpret_cast<PsFindScreen_t>(Rva(findScreen));

    // (5) the hooks (each original kept before its thunk moves)
    if (setPaused && spCallOk)
        PsHookThunk("PauseState::SetPaused", spSite, setPaused, reinterpret_cast<const void*>(&PsSetPausedWrap), &g_psSetPausedFn);
    if (activate && actCall)
        PsHookThunk("screen activation", actCall + kPsActivateCallAt, activate, reinterpret_cast<const void*>(&PsActivateWrap),
                    &g_psActivateFn);
    if (deactivate && deactCall)
        PsHookThunk("screen deactivation", deactCall + kPsDeactivateCallAt, deactivate,
                    reinterpret_cast<const void*>(&PsDeactivateWrap), &g_psDeactivateFn);
    g_ps34On = true;
    Log("[pause34] pause scan ON: SetPaused %s, activation %s, deactivation %s; each pause / unpause, each pause screen's record (and %lu "
        "frames later), the roots, the pause states, the menu input of controllers 0 - 3, players 3 / 4's own pause screens",
        g_psSetPausedFn ? "logged" : "NOT logged", g_psActivateFn ? "logged" : "NOT logged", g_psDeactivateFn ? "logged" : "NOT logged",
        kPsRedumpFrames);
}

// game thread (the menu layer, every frame): the dumps that wait a few frames
void Ps34Frame()
{
    if (!g_ps34On)
        return;
    const unsigned long now = g_gt.frame;
    if (g_psRecDue && now >= g_psRecDue)
    {
        g_psRecDue = 0;
        if (g_psFindScreen && g_psRecName[0])
            PsLogRecord(g_psFindScreen(g_psRecName), "12 frames after it came up");
    }
    if (g_psSceneDue && now >= g_psSceneDue)
    {
        g_psSceneDue = 0;
        PsLogManagers("12 frames into the pause");
        PsLogScene(g_psMatchLogged ? "a pause" : "the first pause of the session");
        g_psMatchLogged = true;
    }
    for (int k = 0; k < 2; ++k)
    {
        if (!g_psP34Due[k] || now < g_psP34Due[k])
            continue;
        g_psP34Due[k] = 0;
        const Flow34& f = g_f34[k];
        if (f.screen != kScrPause34 || !f.group)
            continue;
        if (Ps34Budget())
            Log("[pause34] %lu ms: PLAYER %d's PAUSE SCREEN (BF1GameExt's, not the game's): group 0x%08X in roots[%d] (his view %d), "
                "item %d picked, game look %s",
                ScanMsSinceAttach(), k + 3, static_cast<unsigned>(f.group), f.root, PlayerNSpecView(k), f.pauseSel,
                f.pauseLook ? "on (copies of the game's menu bars)" : "off (texts)");
        PsLogElem("    ", f.group, true);
        PsLogTree(f.group, 2, 0);
        PsLogRoots("his pause screen");
        PsLogManagers("his pause screen");
        PsLogInputs();
    }
}

// players 3 / 4's own pause screen came up (F34PauseOpen): dumped 2 frames later
void Ps34P34Opened(int k)
{
    if (g_ps34On && k >= 0 && k <= 1)
        g_psP34Due[k] = g_gt.frame + 2;
}

void F34PauseClose(Flow34& f, const char* why); // (below)
bool F34Kill(Flow34& f, uintptr_t ch);          // (below)

// ---- 21h-r2: REAL PAUSE MENUS FOR PLAYERS 3 AND 4 - [Splitscreen] RealPauseP34=1, [rp34] lines ----------------------------
// Players 3 / 4's pause screens are the game's own kind now: ifs_pausemenu3 / 4, made by the game's script functions in
// BF1GameExt_pause34.lua (menus.cpp runs it while the game makes ifs_pausemenu2) on viewports 2 / 3, so the game draws
// them in their quarters exactly as it draws ifs_pausemenu / 2 in players 1 / 2's. They run on two IF managers of BF1GameExt's
// (the game has two): built with the game's own constructor (GOG 1.3 0x00132E20) at start, controller 2 / 3 and Lua index 2 /
// 3 in them, and given their input step (0x00133590) and tick (0x00134D60) right after the game's own IF update each frame
// while one of them has a screen. The 2-slot limits the Lua index meets on the way answer indexes 2 / 3 with ours: the Lua's
// current manager (0x00134380), the active screen get / set (0x00133E90 / 0x00134BC0) and a manager's controller (0x00133F00) -
// each through its jump thunk, the game's function for 0 / 1. When the game resets its managers (its Lua state closes:
// 0x00134B70) ours are reset with them. Start on his pad (in the world) pushes his screen on his manager; the world pause,
// the sound and his HUD as before; his pad is NOT taken off his PlayerInput while it is up (the menu reads his pad through
// it; the world pause holds his soldier). What the screen asks for ("BFXQ: rp ..."): resume, respawn, restart, quit (each
// closes it; the last three after its YES), and his controller options (ctlget / ctlset / ctlsave / ctllive -
// players34.cpp's PlayerNControls*). Without the world pause (PauseWorldP34=0), the managers, the screen or his view at
// viewport k + 2: BF1GameExt's own pause screen as before. Players 1 / 2 and 2-player: unchanged.
const char* const kRpCtorCall = "68 ?? ?? ?? ?? 6A 02 68 44 02 00 00 68 ?? ?? ?? ?? E8"; // the managers' vector ctor call
constexpr uint32_t kRpCtorCallThunk = 0x01;
const char* const kRpCtorBody = "56 6A 40 8B F1 E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B C6 5E C3";
constexpr uint32_t kRpCtorInitCall = 0x0C;
const char* const kRpInitBody = "56 8B F1 57 68 C0 00 00 00 6A 00 8D 46 34 C7 86 F4 00 00 00 00 00 00 00 50 C7 86 8C 01 00 00 00 00 00 00";
// the managers' reset (the Lua state going): init each of the two; [flag] = 0; [Lua state] = 0
const char* const kRpResetBody = "56 33 F6 0F 1F 40 00 66 0F 1F 84 00 00 00 00 00 8B 0D ?? ?? ?? ?? 8D 0C 0E E8 ?? ?? ?? ?? 81 C6 44 02 "
                                 "00 00 81 FE 88 04 00 00 7C E4 C6 05 ?? ?? ?? ?? 00 C7 05 ?? ?? ?? ?? 00 00 00 00 5E C3";
constexpr uint32_t kRpResetInitCall = 0x19;
constexpr uint32_t kRpResetLuaState = 0x35;
const char* const kRpCurMgrBody = "A1 ?? ?? ?? ?? 83 F8 01 77 0D 69 C0 44 02 00 00 03 05 ?? ?? ?? ?? C3";
constexpr int kRpMaxLines = 600;

typedef void(__fastcall* RpCtor_t)(void* mgr, void* unusedEdx);
typedef void(__fastcall* RpInit_t)(void* mgr, void* unusedEdx);
typedef void(__fastcall* RpInput_t)(void* mgr, void* unusedEdx, int index, void* input, int controller, float dt);
typedef uintptr_t(__cdecl* RpCurMgr_t)();
typedef uintptr_t(__cdecl* RpActiveGet_t)(int index);
typedef void(__cdecl* RpActiveSet_t)(int index, uintptr_t screen);
typedef int(__cdecl* RpCtrl_t)(int index);
typedef void(__cdecl* RpIfUpdate_t)(float dt);
typedef void(__cdecl* RpReset_t)();

bool g_rpWanted = true;            // [Splitscreen] RealPauseP34
// 21h-r3: the sound while players 3 / 4's pause holds the world. The game's own pause runs the sound each frame from
// PauseState::Update while its player is paused (+18h: call 0x00144400(dt), GOG 1.3) - the world loop that runs it otherwise
// stands still while the pause byte is on. Players 3 / 4's pauses had nothing running it: the fade-out of the in-game bus never
// went on (their pause was as loud as the game) and the menu sounds did not play. Now the same function runs after the game's
// IF update while one of their pauses holds the world and neither player 1's nor player 2's pause is on (theirs run it).
// [Splitscreen] PauseSoundP34=1.
const char* const kPsSndUpdateBody = "55 8B EC E8 ?? ?? ?? ?? 84 C0 74 09 E8 ?? ?? ?? ?? 84 C0 75 52 E8 ?? ?? ?? ?? 84 C0 74 49 B9 ?? ?? ?? ?? E8";
constexpr uint32_t kPsUpdateSndCall = 0x18;  // PauseState::Update +18h: call the sound's paused update (cdecl, float dt)
typedef void(__cdecl* PsSndUpdate_t)(float dt);
PsSndUpdate_t g_psSndUpdate = nullptr;
bool g_sndRunWanted = true;        // [Splitscreen] PauseSoundP34
long g_sndRuns = 0;
bool g_sndRunning = false;         // (for the log: one line as it starts / stops)
// 21h-r3: player 2's popups and option screens in his quarter. They are IF-root screens (viewport -1), drawn by the menu layer
// (spawn_ui.cpp), which moves it to player 1's box only while player 1's manager has a screen - so in 3 / 4-player
// matches player 2's options were drawn in the last view's place (player 4's quarter). P2SpawnMenuOwner says whose box it is
// (player 2's while his pause is on and player 1's is not; with both on, whose manager has the popup or a page pushed over
// its pause menu). The manager's popup name: +74h (its OpenPopup copies 40h bytes there; ClosePopup clears it) - checked.
// [Splitscreen] MenuBoxP2=1.
const char* const kMgrOpenPopupBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 84 C0 75 2D 6A 40 FF 75 08 83 C6 74 56 E8";
constexpr uint32_t kMgrPopup = 0x74;
bool g_menuBoxP2Wanted = true;     // [Splitscreen] MenuBoxP2
bool g_popupOk = false;            // +74h checked
int g_ownerWas = -2;
int g_ownerLines = 0;
// 21j-r15: THE 2-PLAYER SPLIT - PLAYER 2'S PAUSE MENU AS PLAYER 1'S ([Splitscreen] PauseLikeP1P2). the videos after r14 (vertical):
// player 2's OPTIONS (an IF-root screen, drawn by the menu layer) came out stretched over his whole half - player 1's are in a
// 16:9 box of his (spawn_ui.cpp, SplitMenuFit); his OPTIONS had VIDEO OPTIONS and his CONTROL OPTIONS the keyboard page;
// his pause menu stood further left in his half than player 1's in his (the game's safe areas: player 1's half 10% at its
// left and 2% at its right, player 2's 2% / 10% - the menu's container is anchored at 0.6 of it), and it made no sounds. Now
// while his pause is the one on his popups and option screens go in a 16:9 box of his own half (P2SpawnMenuOwner 2,
// render_hooks.cpp MenuViewportBeginOwn), and menus.cpp's script (run while the game makes ifs_pausemenu2 in a
// 2-player match) gives his OPTIONS no VIDEO OPTIONS and CONTROL OPTIONS straight to the gamepad page (as OptionsP2 does with 3 /
// 4 players), player 1's menu sounds, and moves his menu's container by "rp p2dx / p2dy" - where player 1's stands in his
// half. The 3-PLAYER LAYOUT ([Splitscreen] MenuBoxP1): player 1's menu layer in his top left quarter (the video after r14: across the
// top half - the last view is player 3's full-width bottom half, which MenuViewportBegin took for the 2-player bottom half).
bool g_p2LikeWanted = true;        // [Splitscreen] PauseLikeP1P2
bool g_menuBoxP1Wanted = true;     // [Splitscreen] MenuBoxP1
bool g_cursorP1Wanted = true;      // [Splitscreen] CursorP1Only (21j-r16: CursorFrame)
int g_cursorLines = 0;
int g_p2ShiftWas[2] = { -99999, -99999 };
int g_p2LikeLines = 0;
// 21h-r4: THE CONTROL MAP SCAN (log only, [ctlmap] lines) - for players 3 / 4's copy of player 1's gamepad page. A player's
// button -> function map is in his PlayerInput: +12Ch + (mode * 32 + button) * 4 (GOG 1.3 0x0013D540, what the page's
// ScriptCB_GetFunctionIdForButtonId reads for the pausing player). The page's button numbers (1 - 19) go through the game's
// button table (0x0016FBC0: 19 internal ids) and the function ids back through its function table (0x0016F810: 20 ids, the
// index = the page's function number). Logged for players 1 - 4 (each mode) when a real pause menu of players 3 / 4 opens;
// "rp ctlfn <player> <mode> <button>" answers the page's function number (for the next round's page).
const char* const kCmGetBody = "55 8B EC 8B C1 8B 48 04 C1 E1 05 03 4D 0C 8B 8C 88 2C 01 00 00 8B 45 08 89 08 5D C2 08 00";
constexpr uint32_t kCmGetBase = 0x11;   // the disp32 (+12Ch) of "mov ecx, [eax+ecx*4+12Ch]"
const char* const kCmBtnBody = "55 8B EC 8B 45 08 83 F8 12 77 09 8B 04 85 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kCmBtnTable = 0x0E;
constexpr int kCmButtons = 19;
const char* const kCmFnBody = "55 8B EC 8B 4D 08 33 C0 0F 1F 84 00 00 00 00 00 3B 0C 85 ?? ?? ?? ?? 74 08 40 83 F8 14 7C F1 33 C0 5D C3";
constexpr uint32_t kCmFnTable = 0x13;
constexpr int kCmFns = 20;
constexpr int kCmModes = 3;
uint32_t g_cmBase = 0;            // +12Ch
uintptr_t g_cmBtn = 0;            // the button table (19 ints)
uintptr_t g_cmFn = 0;             // the function table (20 ints)
int g_cmLogs = 0;
// 21h-r6: players 3 / 4's own button maps ([Splitscreen] RemapP34, ControlMapP3 / P4)
bool g_remapWanted = true;           // [Splitscreen] RemapP34
bool g_spawnPauseWanted = true;      // [Splitscreen] SpawnPauseP34
char g_cmIni[MAX_PATH + 32] = {};    // BF1GameExt.ini
int g_cmWant[2][kCmModes][kCmButtons + 1]; // the page's function number per button (-1: the game's)
bool g_cmDirty[2] = {};              // changed since the last save
uintptr_t g_cmApplied[2] = {};       // the PlayerInput it was last written into
DWORD g_cmCheckAt[2] = {};
int g_cmApplyLines = 0;
// the buttons a player may change (the page's numbers): LT LB X A B Y RB RT L3 R3 - not the sticks (7 / 10), BACK / START
// (8 / 9), the d-pad (3 - 6) or 17; and the game's default function for each (the same in the three modes)
constexpr int kCmPick[] = { 1, 2, 11, 12, 13, 14, 15, 16, 18, 19 };
int CmDefault(int b)
{
    switch (b)
    {
    case 1: return 6;
    case 2: return 4;
    case 11: return 9;
    case 12: return 2;
    case 13: return 1;
    case 14: return 7;
    case 15: return 3;
    case 16: return 5;
    case 18: return 15;
    case 19: return 8;
    default: return -1;
    }
}
// 21h-r7: the match end scan
bool g_endScanWanted = true;          // [Splitscreen] EndScan34
const char* const kVdMakeBody = "68 B0 04 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0D 8B C8 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? C3";
constexpr uint32_t kVdMakeCtorCall = 0x13;   // call the constructor
constexpr uint32_t kVdMakeGlobal = 0x19;     // its pointer (mov [g], eax)
constexpr uint32_t kVdCtorHolderCall = 0x62; // constructor +62h: call (mov eax, [holder]; ret)
constexpr uint32_t kVdCtorCountCall = 0x6C;  // constructor +6Ch: call (mov eax, [ecx+20h]; ret) - the count
uintptr_t g_vdGlobal = 0;
uintptr_t g_vdHolder = 0;
uint32_t g_vdCountOfs = 0;
uintptr_t g_vdObj = 0;
DWORD g_vdSince = 0;
DWORD g_vdLastLog = 0;
char g_vdState[400] = {};
int g_endLines = 0;
constexpr int kEndMaxLines = 240;
// 21h-r8: the end of the match
bool g_endFixWanted = true;           // [Splitscreen] EndMatch34
int g_endFullWanted = 2;              // [Splitscreen] EndStatsFullScreen (1: 3 / 4 players, 2: 2 players too, 0: off; 21j-r15: 2)
const char* const kVdShowBody = "55 8B EC 83 EC 0C 53 56 8B 75 08 8B D9 89 5D FC 57 85 F6 0F 84";
volatile uintptr_t g_vdShowFn = 0;    // the game's Show (its jump thunk goes through VdShowWrap)
volatile LONG g_endOver = 0;          // switched on this match (until the display is made again / goes)
volatile LONG g_endMaskKnown = 0;
volatile uint32_t g_endMask = 0;      // Show's mask: a team's bit set = DEFEAT
DWORD g_endAt = 0;
// 21h-r9: players 3 / 4's VICTORY / DEFEAT image, made as the game makes players 1 / 2's
typedef void*(__cdecl* VdNew_t)(uint32_t size);
typedef void(__fastcall* VdCtor_t)(void* self, void* edx);
typedef void(__cdecl* VdRootAdd_t)(int root, void* group, float x, float y, int flag);
typedef void(__cdecl* VdRootRemove_t)(int root, void* group);
typedef char(__fastcall* VdGroupAdd_t)(void* group, void* edx, void* elem);
typedef void(__fastcall* VdGroupClear_t)(void* group, void* edx);
typedef void(__fastcall* VdElemVis_t)(void* elem, void* edx, int on);
typedef void(__fastcall* VdElemTex_t)(void* elem, void* edx, const char* name);
typedef void(__fastcall* VdElemSize_t)(void* elem, void* edx, float w, float h, int hMode, int vMode);
typedef void(__fastcall* VdElemColor_t)(void* elem, void* edx, uint32_t rgba);
typedef void(__fastcall* VdElemUv_t)(void* elem, void* edx, float u0, float v0, float u1, float v1, int flag);
typedef void(__fastcall* VdDelete_t)(void* self, void* edx, int flags);
const char* const kVdInitBody = "56 57 8D B1 80 01 00 00 BF 02 00 00 00 0F 1F 00 6A 00 8B CE E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B CE E8 "
                                "?? ?? ?? ?? 66 0F 6E 05 ?? ?? ?? ?? 8B CE 6A 01";
struct VdImg
{
    VdNew_t gameNew = nullptr;
    VdCtor_t groupCtor = nullptr;
    VdCtor_t imageCtor = nullptr;
    VdRootAdd_t rootAdd = nullptr;
    VdRootRemove_t rootRemove = nullptr;
    VdGroupAdd_t groupAdd = nullptr;
    VdGroupClear_t groupClear = nullptr;
    VdElemVis_t vis = nullptr;
    VdElemTex_t tex = nullptr;
    VdElemSize_t size = nullptr;
    VdElemColor_t color = nullptr;
    VdElemUv_t uv = nullptr;
    const char* texName = nullptr;   // "ig_victory_defeat"
    uintptr_t sizeW = 0;             // int 256
    uintptr_t sizeH = 0;             // int 64
    uintptr_t colors = 0;            // 2 x RGBA: [0] red DEFEAT, [1] green VICTORY
    uintptr_t uvs = 0;               // 2 x 4 floats: [0] the bottom half (DEFEAT), [1] the top half (VICTORY)
    bool ok = false;
};
VdImg g_vdi;
struct VdP34
{
    uintptr_t group = 0;
    uintptr_t elem = 0;
    int root = -1;
    bool shown = false;
};
VdP34 g_vdp[2];
// 21h-r9: the pause menu on the stats screen
int g_statsPauseWanted = 2;           // [Splitscreen] StatsPauseP1 (1: 3 / 4-player matches, 2: every match, 0: off)
bool g_statsSeen = false;             // the stats mode seen on (logged once each time)
bool g_statsPaused = false;           // player 1's pause menu is up over the stats (ours)
uintptr_t g_statsRec = 0;             // ifs_pausemenu's record (its viewport -1 while it is up there)
int32_t g_statsVpWas = 0;
int g_statsLines = 0;
bool g_statsPadWas = false;          // 21h-r11: Start on player 1's pad last frame
bool g_statsEscWas = false;          // 21h-r11: Esc last frame (the game's window in front)
bool CmPickable(int b)
{
    for (int x : kCmPick)
        if (x == b)
            return true;
    return false;
}
bool g_rpOn = false;               // everything found, the managers built and the thunks moved
int g_rpLines = 0;
uintptr_t g_rpMgr[2] = {};         // players 3 / 4's IF managers (ours)
uintptr_t g_rpLuaState = 0;        // the game's Lua state pointer (0 while none)
RpInit_t g_rpInit = nullptr;
RpInput_t g_rpInput = nullptr;
ManagerTick_t g_rpTick = nullptr;
uintptr_t g_rpInputMgr = 0;
// where each moved thunk went before (the game's function), written before the thunk moves
volatile uintptr_t g_rpCurMgrFn = 0;
volatile uintptr_t g_rpActiveGetFn = 0;
volatile uintptr_t g_rpActiveSetFn = 0;
volatile uintptr_t g_rpCtrlFn = 0;
volatile uintptr_t g_rpIfUpdateFn = 0;
volatile uintptr_t g_rpResetFn = 0;
uintptr_t g_rpActive[2] = {};      // the active screen of Lua index 2 / 3
uintptr_t g_rpActiveL[2] = {};     // the Lua state it was set in (a screen from another Lua state is gone)
bool g_rpRun[2] = {};              // our manager k is driven each frame (a screen up, or going)
int g_rpSkip[2] = {};              // input steps skipped right after it opens (the Start that opened it)
volatile LONG g_rpReq[2] = {};     // what his screen asked for: 1 resume, 2 respawn, 3 restart, 4 quit (done in his frame)
long g_rpOpens = 0;
long g_rpTicks[2] = {};
int g_rpInLines = 0;

bool RpBudget()
{
    if (g_rpLines >= kRpMaxLines)
        return false;
    if (++g_rpLines == kRpMaxLines)
    {
        Log("[rp34] line limit reached (%d), going quiet", kRpMaxLines);
        return false;
    }
    return true;
}

uintptr_t RpLuaState()
{
    return g_rpLuaState ? ReadU32(g_rpLuaState) : 0;
}

// the one incremental-link jump thunk going to fn (an E9 with another E9 next to it), 0 unless exactly one; direct calls
// counted
uint32_t RpThunkOf(uint32_t fn, int* direct)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    *direct = 0;
    if (!fn || !CodeSection(&codeRva, &codeSize) || codeSize < 10)
        return 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    uint32_t found = 0;
    int n = 0;
    for (uint32_t i = 0; i + 5 <= codeSize; ++i)
    {
        if (p[i] != 0xE9 && p[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, p + i + 1, 4);
        if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) != fn)
            continue;
        if (p[i] == 0xE8)
        {
            ++*direct;
            continue;
        }
        const bool table = (i >= 5 && p[i - 5] == 0xE9) || (i + 10 <= codeSize && p[i + 5] == 0xE9);
        if (table)
        {
            found = codeRva + i;
            ++n;
        }
    }
    return n == 1 ? found : 0;
}

// fn's jump thunk to `wrap`, the function's address kept in *orig first
bool RpMoveThunk(const char* what, uint32_t fn, const void* wrap, volatile uintptr_t* orig)
{
    int direct = 0;
    const uint32_t thunk = RpThunkOf(fn, &direct);
    if (!thunk || direct)
    {
        Log("[rp34] %s rva 0x%08X: %s - NOT moved", what, fn, !thunk ? "not exactly one jump thunk" : "direct calls too");
        return false;
    }
    *orig = Rva(fn);
    if (!RetargetJump(Rva(thunk), wrap))
    {
        *orig = 0;
        Log("[rp34] %s: the thunk rva 0x%08X could not be written - NOT moved", what, thunk);
        return false;
    }
    Log("[rp34] %s rva 0x%08X: its jump thunk rva 0x%08X now goes through BF1GameExt OK", what, fn, thunk);
    return true;
}

// the wrappers: Lua index 2 / 3 -> ours, else the game's
uintptr_t __cdecl RpCurMgrWrap()
{
    const int32_t i = g_luaIndexAddr ? PsI32(g_luaIndexAddr) : -1;
    if ((i == 2 || i == 3) && g_rpMgr[i - 2])
        return g_rpMgr[i - 2];
    return reinterpret_cast<RpCurMgr_t>(g_rpCurMgrFn)();
}

uintptr_t __cdecl RpActiveGetWrap(int index)
{
    if (index == 2 || index == 3)
    {
        const int k = index - 2;
        return (g_rpActive[k] && g_rpActiveL[k] == RpLuaState()) ? g_rpActive[k] : 0;
    }
    return reinterpret_cast<RpActiveGet_t>(g_rpActiveGetFn)(index);
}

void __cdecl RpActiveSetWrap(int index, uintptr_t screen)
{
    if (index == 2 || index == 3)
    {
        g_rpActive[index - 2] = screen;
        g_rpActiveL[index - 2] = RpLuaState();
        return;
    }
    reinterpret_cast<RpActiveSet_t>(g_rpActiveSetFn)(index, screen);
}

int __cdecl RpCtrlWrap(int index)
{
    if ((index == 2 || index == 3) && g_rpMgr[index - 2])
        return PsI32(g_rpMgr[index - 2] + kMgrPlayer);
    return reinterpret_cast<RpCtrl_t>(g_rpCtrlFn)(index);
}

// our managers to their start state (after the game's own reset, or at start): the game's init, then controller / index
void RpResetOurs(const char* why)
{
    for (int k = 0; k < 2; ++k)
    {
        const uintptr_t m = g_rpMgr[k];
        if (!m)
            continue;
        g_rpInit(reinterpret_cast<void*>(m), nullptr);
        *reinterpret_cast<int32_t*>(m + kMgrPlayer) = k + 2;
        *reinterpret_cast<int32_t*>(m + kMgrIndex) = k + 2;
        g_rpActive[k] = 0;
        g_rpActiveL[k] = 0;
        g_rpRun[k] = false;
        g_rpReq[k] = 0;
    }
    if (RpBudget())
        Log("[rp34] %lu ms: players 3 / 4's IF managers reset (%s)", ScanMsSinceAttach(), why);
}

void __cdecl RpResetWrap()
{
    reinterpret_cast<RpReset_t>(g_rpResetFn)();
    RpResetOurs("the game reset its managers - its Lua state goes");
}

// the menu input of controller c (the input manager's vtable +20h: BF1GameExt answers 2 / 3 with players 3 / 4's)
uintptr_t RpMenuInput(int c)
{
    const uintptr_t vt = g_rpInputMgr ? ReadU32(g_rpInputMgr) : 0;
    const uintptr_t fn = vt ? ReadU32(vt + 0x20) : 0;
    return fn ? reinterpret_cast<PsInputGet_t>(fn)(reinterpret_cast<void*>(g_rpInputMgr), nullptr, c) : 0;
}

// after the game's IF update (game thread): our managers' input step and tick, while one has a screen
void __cdecl RpIfUpdateWrap(float dt)
{
    reinterpret_cast<RpIfUpdate_t>(g_rpIfUpdateFn)(dt);
    const FramePerfScope perf(kPerfOurMenus34); // 21j-r11: the frame-rate scan (players 3 / 4's menu managers, the sound)
    for (int k = 0; k < 2; ++k)
    {
        if (!g_rpRun[k] || !g_rpMgr[k])
            continue;
        void* const m = reinterpret_cast<void*>(g_rpMgr[k]);
        const uintptr_t in = RpMenuInput(k + 2);
        if (g_rpSkip[k] > 0)
            --g_rpSkip[k];
        else if (in)
        {
            g_rpInput(m, nullptr, k + 2, reinterpret_cast<void*>(in), k + 2, dt);
            // the first presses that reach his menu (accept / back / start: current, previous)
            const float acc = ReadF32(in + 0x68), back = ReadF32(in + 0x6C), st = ReadF32(in + 0x70);
            if ((acc >= 0.5f || back >= 0.5f || st >= 0.5f) && g_rpInLines < 24 && RpBudget())
            {
                ++g_rpInLines;
                Log("[rp34] %lu ms: player %d's menu input (controller %d): accept %.2f back %.2f start %.2f", ScanMsSinceAttach(), k + 3,
                    k + 2, static_cast<double>(acc), static_cast<double>(back), static_cast<double>(st));
            }
        }
        g_rpTick(m, nullptr, dt);
        ++g_rpTicks[k];
    }
    // 21h-r3: the sound runs while a pause of players 3 / 4 holds the world (players 1 / 2's own pauses run it themselves)
    const bool ours = g_f34[0].worldPaused || g_f34[1].worldPaused;
    const bool run = g_psSndUpdate && g_sndRunWanted && ours && !GamePlayerPaused(0) && !GamePlayerPaused(1) && g_pauseByte &&
                     ReadU8(g_pauseByte) != 0;
    if (run)
    {
        g_psSndUpdate(dt);
        ++g_sndRuns;
    }
    if (run != g_sndRunning)
    {
        g_sndRunning = run;
        if (RpBudget())
            Log("[rp34] %lu ms: the sound %s (%ld frames run so far)", ScanMsSinceAttach(),
                run ? "RUNS while player 3's / 4's pause holds the world (as the game's pause runs it)"
                    : (ours ? "is run by player 1's / 2's pause now" : "is the world's again"),
                g_sndRuns);
    }
}

// his screen ("ifs_pausemenu3" / "4") exists in this Lua state's screen list
bool RpScreenExists(int k)
{
    if (!g_psFindScreen)
        return false;
    char name[24];
    snprintf(name, sizeof(name), "ifs_pausemenu%d", k + 3);
    return g_psFindScreen(name) != 0;
}

// can his real pause screen come up now? (else BF1GameExt's own)
bool RpCanOpen(const Flow34& f, const char** why)
{
    *why = "";
    if (!g_rpWanted)
        *why = "[Splitscreen] RealPauseP34=0";
    else if (!g_rpOn)
        *why = "the real pause menus are not installed (see the start lines)";
    else if (!g_worldPauseWanted || !g_pauseByteOn || !g_offline || !g_offline())
        *why = "no world pause (PauseWorldP34=0, or not offline): his pad would move his soldier";
    else if (PlayerNSpecView(f.k) != f.k + 2)
        *why = "his view is not viewport k + 2";
    else if (!RpScreenExists(f.k))
        *why = "his screen was not made this match (BF1GameExt_pause34.lua)";
    else if (PsI32(g_rpMgr[f.k] + kMgrDepth) != 0)
        *why = "his manager still has a screen";
    return (*why)[0] == 0;
}

void CmLog(const char* why); // 21h-r4: the control map scan (below)

void RpOpen(Flow34& f)
{
    const int k = f.k;
    char name[24];
    snprintf(name, sizeof(name), "ifs_pausemenu%d", k + 3);
    ++f.pauses;
    ++g_rpOpens;
    f.pauseSel = 0;
    f.confirm = 0;
    f.screen = kScrPause34;
    f.real = true;
    g_rpReq[k] = 0;
    g_rpSkip[k] = 2;
    g_rpRun[k] = true;
    F34PadLatch(f);
    PlayerNMenuInput(k, true); // 21h-r3: his soldier gets a calm packet while the menu reads his pad
    g_pushScreen(reinterpret_cast<void*>(g_rpMgr[k]), nullptr, name, k + 2, k + 2);
    F34WorldPause(f, true);
    if (f.pauses <= 2) // 21h-r4: the control map scan
        CmLog(k == 0 ? "player 3's pause menu opened" : "player 4's pause menu opened");
    if (RpBudget())
        Log("[rp34] %lu ms: player %d: Start - his REAL pause menu: \"%s\" pushed on his manager (controller %d, Lua index %d, viewport "
            "%d) - open %ld",
            ScanMsSinceAttach(), k + 3, name, k + 2, k + 2, k + 2, g_rpOpens);
}

// his screen taken off his manager (F34Close)
void RpCloseScreen(Flow34& f)
{
    if (!f.real)
        return;
    f.real = false;
    PlayerNMenuInput(f.k, false); // 21h-r3: the calm packet held until his buttons are up (the B that closed it is crouch)
    const uintptr_t m = g_rpMgr[f.k];
    if (m && PsI32(m + kMgrDepth) > 0)
        g_popScreen(reinterpret_cast<void*>(m), nullptr, 0);
    if (RpBudget())
        Log("[rp34] %lu ms: player %d: his real pause menu popped (manager depth %ld, %ld ticks so far)", ScanMsSinceAttach(), f.k + 3,
            static_cast<long>(m ? PsI32(m + kMgrDepth) : -1), g_rpTicks[f.k]);
}

// his frame while it is up (game thread): his pad latched (the Start that closes it does not open it again), what it asked for
void RpFrame(Flow34& f, uintptr_t ch, uintptr_t pad)
{
    const PadIn in = PadReadAt(pad);
    f.pad.aDown = in.a;
    f.pad.bDown = in.b;
    f.pad.sDown = in.start;
    const int k = f.k;
    const LONG req = InterlockedExchange(&g_rpReq[k], 0);
    if (!req)
        return;
    static const char* const kWhat[] = { "", "RESUME GAME", "RESPAWN - yes", "RESTART MISSION - yes", "QUIT - yes" };
    F34PauseClose(f, req >= 1 && req <= 4 ? kWhat[req] : "?");
    if (req == 2)
        F34Kill(f, ch);
    else if (req == 3 || req == 4)
        F34GameAction(f, req);
}

// once a popped screen is gone, our manager rests
void RpIdleCheck()
{
    for (int k = 0; k < 2; ++k)
    {
        if (!g_rpRun[k] || g_f34[k].real)
            continue;
        const uintptr_t m = g_rpMgr[k];
        char top[64];
        TopName(m, top, sizeof(top));
        if (PsI32(m + kMgrDepth) == 0 && top[0] == 0)
        {
            g_rpRun[k] = false;
            if (RpBudget())
                Log("[rp34] %lu ms: player %d's manager is idle again", ScanMsSinceAttach(), k + 3);
        }
    }
}

// 21h-r4: player p's (0 - 3) PlayerInput: players 1 / 2's through the input manager (vtable +18h, as the game's page reads
// it), players 3 / 4's their own (BF1GameExt's)
uintptr_t CmInput(int p)
{
    if (p >= 2)
        return p <= 3 ? reinterpret_cast<uintptr_t>(PlayerNInputObject(p, 0)) : 0;
    if (p < 0)
        return 0;
    const uintptr_t vt = g_rpInputMgr ? ReadU32(g_rpInputMgr) : 0;
    const uintptr_t fn = vt ? ReadU32(vt + 0x18) : 0;
    return fn ? reinterpret_cast<PsInputGet_t>(fn)(reinterpret_cast<void*>(g_rpInputMgr), nullptr, p) : 0;
}

// the page's function number (0 - 19, -1 unknown) for the page's button b (1 - 19) in mode m, from player p's map
int CmFn(uintptr_t input, int m, int b)
{
    if (!input || !g_cmBase || !g_cmBtn || !g_cmFn || m < 0 || m >= kCmModes || b < 1 || b > kCmButtons)
        return -1;
    const int ib = PsI32(g_cmBtn + static_cast<uintptr_t>(b - 1) * 4);
    if (ib < 0 || ib >= 32)
        return -1;
    const int32_t fn = PsI32(input + g_cmBase + static_cast<uintptr_t>(m * 32 + ib) * 4);
    for (int i = 0; i < kCmFns; ++i)
        if (PsI32(g_cmFn + static_cast<uintptr_t>(i) * 4) == fn)
            return i;
    return 0;
}

// 21h-r6: button b (1 - 19) of mode m in this PlayerInput set to the page's function number fn (0 - 19) - what the game's
// ScriptCB_SetFunctionIdForButtonId writes (0x0013E920), in the given input, not the pausing player's
bool CmSet(uintptr_t input, int m, int b, int fn)
{
    if (!input || !g_cmBase || !g_cmBtn || !g_cmFn || m < 0 || m >= kCmModes || b < 1 || b > kCmButtons || fn < 0 || fn >= kCmFns)
        return false;
    const int ib = PsI32(g_cmBtn + static_cast<uintptr_t>(b - 1) * 4);
    if (ib < 0 || ib >= 32)
        return false;
    *reinterpret_cast<volatile int32_t*>(input + g_cmBase + static_cast<uintptr_t>(m * 32 + ib) * 4) =
        PsI32(g_cmFn + static_cast<uintptr_t>(fn) * 4);
    return true;
}

// [Splitscreen] ControlMapP3 / P4 read (at start)
void CmLoad()
{
    for (int k = 0; k < 2; ++k)
    {
        for (int m = 0; m < kCmModes; ++m)
            for (int b = 0; b <= kCmButtons; ++b)
                g_cmWant[k][m][b] = -1;
        if (!g_cmIni[0])
            continue;
        char key[24];
        char val[400] = {};
        snprintf(key, sizeof(key), "ControlMapP%d", k + 3);
        GetPrivateProfileStringA("Splitscreen", key, "", val, sizeof(val), g_cmIni);
        const char* q = val;
        int set = 0;
        for (int m = 0; m < kCmModes; ++m)
            for (int b = 1; b <= kCmButtons; ++b)
            {
                char* end = nullptr;
                const long x = strtol(q, &end, 10);
                if (end == q)
                    break;
                q = end;
                if (CmPickable(b) && x >= 0 && x < kCmFns)
                {
                    g_cmWant[k][m][b] = static_cast<int>(x);
                    ++set;
                }
            }
        Log("[ctlmap] 21h-r6: [Splitscreen] %s: %d button(s) of player %d's own map%s", key, set, k + 3,
            set ? " - written into his PlayerInput each match" : " (the game's default map)");
    }
}

// [Splitscreen] ControlMapP3 / P4 written (his page went with changes)
bool CmSave(int k)
{
    if (k < 0 || k > 1 || !g_cmDirty[k])
        return true;
    g_cmDirty[k] = false;
    if (!g_cmIni[0])
        return false;
    char key[24];
    char val[400];
    size_t at = 0;
    val[0] = 0;
    for (int m = 0; m < kCmModes; ++m)
        for (int b = 1; b <= kCmButtons && at + 8 < sizeof(val); ++b)
        {
            const int w = snprintf(val + at, sizeof(val) - at, "%s%d", at ? " " : "", g_cmWant[k][m][b]);
            if (w > 0)
                at += static_cast<size_t>(w);
        }
    snprintf(key, sizeof(key), "ControlMapP%d", k + 3);
    const bool ok = WritePrivateProfileStringA("Splitscreen", key, val, g_cmIni) != 0;
    Log("[ctlmap] %lu ms: player %d's button map saved to BF1GameExt.ini [Splitscreen] %s%s", ScanMsSinceAttach(), k + 3, key,
        ok ? "" : " - NOT written (the game folder may be read-only): it lasts this session only");
    return ok;
}

// each frame (his flow): his map written into his PlayerInput when it is a new one; checked every 2 s (rewritten if the game
// put its own back)
void CmApplyFrame(int k)
{
    if (!g_remapWanted || k < 0 || k > 1 || !g_cmBase || !g_cmBtn || !g_cmFn)
        return;
    const uintptr_t input = CmInput(k + 2);
    if (!input)
        return;
    const DWORD now = GetTickCount();
    const bool fresh = input != g_cmApplied[k];
    if (!fresh && now - g_cmCheckAt[k] < 2000)
        return;
    g_cmCheckAt[k] = now;
    int written = 0;
    int differed = 0;
    for (int m = 0; m < kCmModes; ++m)
        for (int b = 1; b <= kCmButtons; ++b)
        {
            const int want = g_cmWant[k][m][b];
            if (want < 0)
                continue;
            if (CmFn(input, m, b) != want)
            {
                ++differed;
                if (CmSet(input, m, b, want))
                    ++written;
            }
        }
    g_cmApplied[k] = input;
    if ((fresh || differed) && written && g_cmApplyLines < 40)
    {
        ++g_cmApplyLines;
        Log("[ctlmap] %lu ms: player %d's own button map %s his PlayerInput 0x%08X: %d button(s) set", ScanMsSinceAttach(), k + 3,
            fresh ? "written into" : "put back into (the game had its own there)", static_cast<unsigned>(input), written);
    }
}

void CmLog(const char* why)
{
    if (!g_cmBase || !g_cmBtn || !g_cmFn || g_cmLogs >= 12)
        return;
    ++g_cmLogs;
    Log("[ctlmap] %lu ms: the players' button -> function maps (%s; the page's numbers: button 1 - 19 -> function 0 - 19, "
        "gButtonFunctionStringsGamePad[function + 19 * mode])",
        ScanMsSinceAttach(), why);
    for (int p = 0; p < 4; ++p)
    {
        const uintptr_t in = CmInput(p);
        if (!in)
        {
            Log("[ctlmap]   player %d: no PlayerInput", p + 1);
            continue;
        }
        Log("[ctlmap]   player %d: PlayerInput 0x%08X%s, mode now %d", p + 1, static_cast<unsigned>(in),
            p >= 2 ? " (his own, BF1GameExt's)" : " (the game's, as its gamepad page reads it)", PsI32(in + 4));
        for (int m = 0; m < kCmModes; ++m)
        {
            char line[400];
            size_t at = 0;
            for (int b = 1; b <= kCmButtons && at + 12 < sizeof(line); ++b)
            {
                const int w = snprintf(line + at, sizeof(line) - at, " %d:%d", b, CmFn(in, m, b));
                if (w > 0)
                    at += static_cast<size_t>(w);
            }
            char raw[400];
            size_t rt = 0;
            for (int i = 0; i < 32 && rt + 12 < sizeof(raw); ++i)
            {
                const int w = snprintf(raw + rt, sizeof(raw) - rt, " %d", PsI32(in + g_cmBase + static_cast<uintptr_t>(m * 32 + i) * 4));
                if (w > 0)
                    rt += static_cast<size_t>(w);
            }
            Log("[ctlmap]     mode %d (%s): page buttons%s | raw slots%s", m, m == 0 ? "infantry" : m == 1 ? "vehicle" : "starfighter",
                line, raw);
        }
    }
}

void RpInstall()
{
    if (!g_rpWanted)
    {
        Log("[rp34] [Splitscreen] RealPauseP34=0 - players 3 / 4 keep BF1GameExt's own pause screens");
        return;
    }
    uint32_t hits[4] = {};
    auto one = [&](const char* what, const char* pat) -> uint32_t {
        const int n = FindPattern(pat, hits, 4);
        if (n == 1)
            return hits[0];
        Log("[rp34] %s: %d pattern hit(s) (need exactly 1) - NOT FOUND", what, n);
        return 0u;
    };
    // the managers' constructor (through their vector constructor call) and init
    uint32_t ctor = 0;
    {
        const int n = FindPattern(kRpCtorCall, hits, 4);
        for (int i = 0; i < n && i < 4 && !ctor; ++i)
        {
            const uintptr_t arr = PsOperand(hits[i] + 13);
            if (!arr || !g_mgrBaseAddr || arr != ReadU32(g_mgrBaseAddr))
                continue;
            const uintptr_t thunkVa = PsOperand(hits[i] + kRpCtorCallThunk);
            const uint32_t c = thunkVa ? PsFollow(RvaOf(thunkVa), 0) : 0;
            if (c && MatchPatternAt(c, kRpCtorBody))
                ctor = c;
        }
    }
    const uint32_t init = ctor ? PsCallTarget(ctor + kRpCtorInitCall, 0) : 0;
    const bool initOk = init && MatchPatternAt(init, kRpInitBody);
    const uint32_t reset = one("the managers' reset", kRpResetBody);
    const bool resetOk = reset && init && PsCallTarget(reset + kRpResetInitCall, 0) == init;
    g_rpLuaState = reset ? PsOperand(reset + kRpResetLuaState) : 0;
    if (!g_psFindScreen) // (the pause scan finds it too, with PauseScan34=1)
    {
        const uint32_t fs = one("FindScreen", kPsFindScreen);
        if (fs)
            g_psFindScreen = reinterpret_cast<PsFindScreen_t>(Rva(fs));
    }
    const uint32_t curMgr = one("the Lua's current manager", kRpCurMgrBody);
    const uint32_t actGet = one("active screen get", kPsActiveGet);
    const uint32_t actSet = one("active screen set", kPsActiveSet);
    const uint32_t ctrl = one("a manager's controller", kPsMgrCtrl);
    const uint32_t input = one("a manager's input step", kPsMgrInput);
    const uint32_t ifUpdate = g_ifUpdateRva;
    const uint32_t tick = ifUpdate && MatchPatternAt(ifUpdate + kIfUpdateTickAt, kIfUpdateTickSite)
                              ? PsCallTarget(ifUpdate + kIfUpdateTickAt, 0)
                              : 0;
    const bool tickOk = tick && MatchPatternAt(tick, kManagerTickBody);
    const uint32_t imFn = ifUpdate ? PsCallTarget(ifUpdate + kPsIfInputMgrCall, 0) : 0;
    if (imFn && MatchPatternAt(imFn, kPsMovEaxImmRet))
        g_rpInputMgr = PsOperand(imFn + 1);
    const bool curOk = curMgr && g_luaIndexAddr && PsOperand(curMgr + 1) == g_luaIndexAddr;
    const bool screensOk = g_pushScreen && g_popScreen && g_psFindScreen;
    // 21h-r3: the sound's paused update (PauseState::Update +18h)
    {
        const uint32_t upd = one("PauseState::Update (21h-r3: the sound)", kPsUpdateBody);
        const uint32_t snd = upd ? PsCallTarget(upd + kPsUpdateSndCall, 0) : 0;
        const bool sndOk = snd && MatchPatternAt(snd, kPsSndUpdateBody);
        if (sndOk && g_sndRunWanted)
            g_psSndUpdate = reinterpret_cast<PsSndUpdate_t>(Rva(snd));
        Log("[rp34] 21h-r3: the sound's paused update: PauseState::Update rva 0x%08X, its call +%02Xh -> rva 0x%08X %s - %s", upd,
            kPsUpdateSndCall, snd, sndOk ? "OK" : "does NOT match",
            !g_sndRunWanted ? "[Splitscreen] PauseSoundP34=0: not run (players 3 / 4's pauses leave the sound standing)"
                            : (sndOk ? "it runs while players 3 / 4's pause holds the world"
                                     : "NOT run (players 3 / 4's pauses leave the sound standing)"));
    }
    // 21h-r3: the manager's popup name (+74h), for P2SpawnMenuOwner
    {
        const uint32_t op = one("a manager's OpenPopup (21h-r3)", kMgrOpenPopupBody);
        g_popupOk = op != 0;
        Log("[rp34] 21h-r3: a manager's OpenPopup rva 0x%08X: the popup's name at +%02Xh %s; [Splitscreen] MenuBoxP2=%d (%s)", op,
            kMgrPopup, g_popupOk ? "OK" : "NOT checked (not read)", g_menuBoxP2Wanted ? 1 : 0,
            g_menuBoxP2Wanted ? "3 / 4 players: player 2's popups and option screens in his quarter"
                              : "player 2's popups and option screens as before");
    }
    // 21h-r4: the control map scan's three finds
    {
        const uint32_t get = one("PlayerInput's button -> function read (21h-r4)", kCmGetBody);
        const uint32_t btn = one("the page's button table (21h-r4)", kCmBtnBody);
        const uint32_t fnt = one("the page's function table (21h-r4)", kCmFnBody);
        g_cmBase = get ? ReadU32(Rva(get + kCmGetBase)) : 0;
        g_cmBtn = btn ? PsOperand(btn + kCmBtnTable) : 0;
        g_cmFn = fnt ? PsOperand(fnt + kCmFnTable) : 0;
        const bool cmOk = g_cmBase == 0x12C && g_cmBtn && g_cmFn;
        if (!cmOk)
        {
            g_cmBase = 0;
            g_cmBtn = 0;
            g_cmFn = 0;
        }
        Log("[ctlmap] 21h-r4: the control map scan (log only): read rva 0x%08X (+%03Xh, 32 slots a mode), button table rva 0x%08X "
            "(va 0x%08X), function table rva 0x%08X (va 0x%08X) - %s",
            get, get ? ReadU32(Rva(get + kCmGetBase)) : 0, btn, static_cast<unsigned>(g_cmBtn), fnt, static_cast<unsigned>(g_cmFn),
            cmOk ? "OK: the maps are logged when players 3 / 4's real pause menu opens" : "NOT all found - no [ctlmap] lines");
    }
    // 21h-r7: the match end scan's finds (21h-r8: the end of the match's too)
    if (g_endScanWanted || g_endFixWanted)
    {
        const uint32_t make = one("the victory / defeat display's maker (21h-r7)", kVdMakeBody);
        const uint32_t vdCtor = make ? CallTarget(make + kVdMakeCtorCall) : 0;
        g_vdGlobal = make ? PsOperand(make + kVdMakeGlobal) : 0;
        const uint32_t hold = vdCtor ? CallTarget(vdCtor + kVdCtorHolderCall) : 0;
        const uint32_t cnt = vdCtor ? CallTarget(vdCtor + kVdCtorCountCall) : 0;
        const bool holdOk = hold && PsCodeByte(vdCtor + kVdCtorHolderCall) == 0xE8 && PsCodeByte(hold) == 0xA1 && PsCodeByte(hold + 5) == 0xC3;
        const bool cntOk = cnt && PsCodeByte(vdCtor + kVdCtorCountCall) == 0xE8 && PsCodeByte(cnt) == 0x8B && PsCodeByte(cnt + 1) == 0x41 &&
                           PsCodeByte(cnt + 3) == 0xC3;
        g_vdHolder = holdOk ? PsOperand(hold + 1) : 0;
        g_vdCountOfs = cntOk ? PsCodeByte(cnt + 2) : 0;
        if (!g_vdHolder || !cntOk)
            g_vdGlobal = 0;
        // 21h-r8: its Show - the winners' mask
        if (g_endFixWanted && g_vdGlobal)
        {
            const uint32_t show = one("the victory / defeat display's Show (21h-r8)", kVdShowBody);
            if (show && !RpMoveThunk("[end34] the victory / defeat Show", show, reinterpret_cast<const void*>(&VdShowWrap), &g_vdShowFn))
                g_vdShowFn = 0;
            Log("[end34] 21h-r8: [Splitscreen] EndMatch34=1, EndStatsFullScreen=%d: Show rva 0x%08X %s", g_endFullWanted, show,
                g_vdShowFn ? "goes through BF1GameExt (the winners' mask for players 3 / 4's VICTORY / DEFEAT)"
                           : "NOT moved - players 3 / 4's screens still close at the end, no VICTORY / DEFEAT of theirs");
            // 21h-r9: what the game makes players 1 / 2's image with (its maker, constructor, Show, init and destructor)
            const uint32_t init = one("the victory / defeat display's image setup (21h-r9)", kVdInitBody);
            const uintptr_t vt = (vdCtor && PsCodeByte(vdCtor + 0x22) == 0xC7 && PsCodeByte(vdCtor + 0x23) == 0x06) ? PsOperand(vdCtor + 0x24) : 0;
            const uintptr_t slot0 = vt ? ReadU32(vt) : 0;
            uint32_t dtor = 0;
            if (InExe(slot0))
            {
                uint32_t sr = static_cast<uint32_t>(slot0 - ExeBase());
                for (int hop = 0; hop < 8 && PsCodeByte(sr) == 0xE9; ++hop)
                    sr = static_cast<uint32_t>(static_cast<int64_t>(sr) + 5 + ReadI32At(sr + 1));
                if (PsCodeByte(sr + 6) == 0xE8)
                    dtor = CallTarget(sr + 6);
            }
            auto imm = [&](uint32_t at) -> uintptr_t {
                const uintptr_t t = ReadU32(Rva(at));
                if (!InExe(t))
                    return 0;
                uint32_t r = static_cast<uint32_t>(t - ExeBase());
                for (int hop = 0; hop < 8 && PsCodeByte(r) == 0xE9; ++hop)
                    r = static_cast<uint32_t>(static_cast<int64_t>(r) + 5 + ReadI32At(r + 1));
                return Rva(r);
            };
            const bool ctorOk = vdCtor && PsCodeByte(vdCtor + 0x13) == 0x68 && PsCodeByte(vdCtor + 0x2E) == 0x68 &&
                                PsCodeByte(vdCtor + 0x9A) == 0xE8 && PsCodeByte(vdCtor + 0xA6) == 0xE8 && PsCodeByte(vdCtor + 0xAE) == 0xE8;
            const bool showOk = show && PsCodeByte(show + 0xF1) == 0x8D && PsCodeByte(show + 0x100) == 0xE8 && PsCodeByte(show + 0x10F) == 0x0F &&
                                PsCodeByte(show + 0x11A) == 0xE8;
            const bool initOk = init && PsCodeByte(init + 0x20) == 0xE8 && PsCodeByte(init + 0x4F) == 0xE8;
            const bool dtorOk = dtor && PsCodeByte(dtor + 0x3A) == 0xE8 && PsCodeByte(dtor + 0x44) == 0xE8;
            const bool makeOk = make && PsCodeByte(make + 5) == 0xE8;
            if (ctorOk && showOk && initOk && dtorOk && makeOk)
            {
                g_vdi.gameNew = reinterpret_cast<VdNew_t>(Rva(CallTarget(make + 5)));
                g_vdi.groupCtor = reinterpret_cast<VdCtor_t>(imm(vdCtor + 0x14));
                g_vdi.imageCtor = reinterpret_cast<VdCtor_t>(imm(vdCtor + 0x2F));
                g_vdi.rootAdd = reinterpret_cast<VdRootAdd_t>(Rva(CallTarget(vdCtor + 0x9A)));
                g_vdi.vis = reinterpret_cast<VdElemVis_t>(Rva(CallTarget(vdCtor + 0xA6)));
                g_vdi.groupAdd = reinterpret_cast<VdGroupAdd_t>(Rva(CallTarget(vdCtor + 0xAE)));
                g_vdi.color = reinterpret_cast<VdElemColor_t>(Rva(CallTarget(show + 0x100)));
                g_vdi.uv = reinterpret_cast<VdElemUv_t>(Rva(CallTarget(show + 0x11A)));
                g_vdi.colors = PsOperand(show + 0xF4);
                g_vdi.uvs = PsOperand(show + 0x112);
                g_vdi.tex = reinterpret_cast<VdElemTex_t>(Rva(CallTarget(init + 0x20)));
                g_vdi.size = reinterpret_cast<VdElemSize_t>(Rva(CallTarget(init + 0x4F)));
                g_vdi.texName = reinterpret_cast<const char*>(PsOperand(init + 0x1A));
                g_vdi.sizeH = PsOperand(init + 0x29);
                g_vdi.sizeW = PsOperand(init + 0x43);
                g_vdi.rootRemove = reinterpret_cast<VdRootRemove_t>(Rva(CallTarget(dtor + 0x3A)));
                g_vdi.groupClear = reinterpret_cast<VdGroupClear_t>(Rva(CallTarget(dtor + 0x44)));
                char name[32] = {};
                const bool nameOk = g_vdi.texName && ReadText(reinterpret_cast<uintptr_t>(g_vdi.texName), name, sizeof(name)) &&
                                    strcmp(name, "ig_victory_defeat") == 0;
                g_vdi.ok = nameOk && g_vdi.groupCtor && g_vdi.imageCtor && g_vdi.colors && g_vdi.uvs && g_vdi.sizeW && g_vdi.sizeH &&
                           PsI32(g_vdi.sizeW) == 256 && PsI32(g_vdi.sizeH) == 64;
            }
            Log("[end34] 21h-r9: players 3 / 4's VICTORY / DEFEAT as players 1 / 2's: the image setup rva 0x%08X, destructor rva 0x%08X, "
                "constructor %s, Show %s - %s", init, dtor, ctorOk ? "OK" : "does NOT match", showOk ? "OK" : "does NOT match",
                g_vdi.ok ? "OK: the game's own image (ig_victory_defeat 256 x 64) in their quarters" : "NOT all found - the r8 text instead");
        }
        Log("[end34] 21h-r7: the match end scan (log only, [Splitscreen] EndScan34=1): the victory / defeat display's maker rva 0x%08X, "
            "its pointer va 0x%08X, constructor rva 0x%08X, its count: holder va 0x%08X +%02Xh - %s",
            make, static_cast<unsigned>(g_vdGlobal), vdCtor, static_cast<unsigned>(g_vdHolder), g_vdCountOfs,
            g_vdGlobal ? "OK: [end34] lines when it is made, while it is up and when it goes" : "NOT all found - no [end34] lines");
    }
    Log("[rp34] found: manager ctor rva 0x%08X %s, init rva 0x%08X %s, reset rva 0x%08X %s (Lua state rva 0x%08X), current manager rva "
        "0x%08X %s, active get / set rva 0x%08X / 0x%08X, controller rva 0x%08X, input step rva 0x%08X, tick rva 0x%08X %s, IF update rva "
        "0x%08X, input manager rva 0x%08X, PushScreen / PopScreen / FindScreen %s",
        ctor, ctor ? "OK" : "NOT FOUND", init, initOk ? "OK" : "does NOT match", reset, resetOk ? "OK" : "does NOT check out",
        RvaOf(g_rpLuaState), curMgr, curOk ? "OK (the same index)" : "does NOT check out", actGet, actSet, ctrl, input, tick,
        tickOk ? "OK" : "does NOT match", ifUpdate, RvaOf(g_rpInputMgr), screensOk ? "OK" : "NOT all found");
    if (!ctor || !initOk || !resetOk || !g_rpLuaState || !curOk || !actGet || !actSet || !ctrl || !input || !tickOk || !ifUpdate ||
        !g_rpInputMgr || !screensOk)
    {
        Log("[rp34] REAL PAUSE MENUS NOT INSTALLED: something above did not check out - players 3 / 4 keep BF1GameExt's own");
        return;
    }
    // the two managers, built with the game's constructor (its arrays from the heap current now)
    unsigned char* const mem =
        static_cast<unsigned char*>(VirtualAlloc(nullptr, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!mem)
    {
        Log("[rp34] VirtualAlloc failed (error %lu) - REAL PAUSE MENUS NOT INSTALLED", GetLastError());
        return;
    }
    const int heap = HeapIndexAt(g_curHeapVa);
    for (int k = 0; k < 2; ++k)
    {
        g_rpMgr[k] = reinterpret_cast<uintptr_t>(mem + 0x100 + static_cast<size_t>(k) * 0x400);
        reinterpret_cast<RpCtor_t>(Rva(ctor))(reinterpret_cast<void*>(g_rpMgr[k]), nullptr);
    }
    g_rpInit = reinterpret_cast<RpInit_t>(Rva(init));
    g_rpInput = reinterpret_cast<RpInput_t>(Rva(input));
    g_rpTick = reinterpret_cast<ManagerTick_t>(Rva(tick));
    RpResetOurs("built");
    // the thunks (each original kept first); the IF update and the reset last
    bool ok = RpMoveThunk("the Lua's current manager", curMgr, reinterpret_cast<const void*>(&RpCurMgrWrap), &g_rpCurMgrFn);
    ok = ok && RpMoveThunk("active screen get", actGet, reinterpret_cast<const void*>(&RpActiveGetWrap), &g_rpActiveGetFn);
    ok = ok && RpMoveThunk("active screen set", actSet, reinterpret_cast<const void*>(&RpActiveSetWrap), &g_rpActiveSetFn);
    ok = ok && RpMoveThunk("a manager's controller", ctrl, reinterpret_cast<const void*>(&RpCtrlWrap), &g_rpCtrlFn);
    ok = ok && RpMoveThunk("the managers' reset", reset, reinterpret_cast<const void*>(&RpResetWrap), &g_rpResetFn);
    ok = ok && RpMoveThunk("the IF update", ifUpdate, reinterpret_cast<const void*>(&RpIfUpdateWrap), &g_rpIfUpdateFn);
    g_rpOn = ok;
    Log("[rp34] split_spawn_menus.cpp file build %s - REAL PAUSE MENUS FOR PLAYERS 3 AND 4 %s: managers at 0x%08X / 0x%08X (built with heap %d "
        "current), controller / Lua index 2 / 3; ifs_pausemenu3 / 4 from BF1GameExt_pause34.lua on viewports 2 / 3%s",
        kFileBuild, ok ? "ON" : "NOT complete (a thunk did not move) - OFF", static_cast<unsigned>(g_rpMgr[0]),
        static_cast<unsigned>(g_rpMgr[1]), heap, ok ? "" : "; the thunks that moved answer 0 / 1 with the game's own");
}

// ---- 20g-r6: his pause screen ----------------------------------------------------------------------------------------
// 20g-r7: as the game's pause menu: Start on his pad while he is in the world: GAME PAUSED - RESUME GAME / OPTIONS
// / RESPAWN / RESTART MISSION / QUIT (up / down, A; B or Start = back), the game paused while it is up. His soldier
// stands still while it is up (players34.cpp: his PlayerInput updates without its pad). 20g-r8: OPTIONS opens his
// CONTROLLER page (F34Opts, below).
void F34PauseClose(Flow34& f, const char* why)
{
    F34Close(f, why); // (unpauses him and the world)
}

void F34PauseOpen(Flow34& f, uintptr_t sm)
{
    const char* why = "";
    if (RpCanOpen(f, &why)) // 21h-r2: his real pause menu
    {
        RpOpen(f);
        return;
    }
    if (g_rpWanted && RpBudget())
        Log("[rp34] %lu ms: player %d: BF1GameExt's own pause screen (%s)", ScanMsSinceAttach(), f.k + 3, why);
    ++f.pauses;
    f.pauseSel = 0;
    f.confirm = 0;
    F34Show(f, kScrPause34, sm, "Start - his pause screen");
    if (f.screen == kScrPause34)
    {
        PlayerNSetPaused(f.k, true);
        F34WorldPause(f, true); // 20g-r7
        F34Sound(f, 1, "his pause screen came up"); // 20g-r9
        Ps34P34Opened(f.k); // 21h-r1: the pause scan dumps it 2 frames later
    }
}

// his soldier killed through its death function (as player 2's pause menu Respawn): false if it could not be
bool F34Kill(Flow34& f, uintptr_t ch)
{
    const uintptr_t unit = ch ? ReadU32(ch + kCharUnit) : 0;
    uintptr_t soldier = 0;
    const uintptr_t death = unit ? UnitDeathFn(unit, &soldier) : 0;
    if (!death || !soldier)
    {
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: his soldier could NOT be killed (unit 0x%08X: no death function that checks out)",
                ScanMsSinceAttach(), f.k + 3, static_cast<unsigned>(unit));
        return false;
    }
    reinterpret_cast<Death_t>(death)(reinterpret_cast<void*>(soldier + kUnitSub), nullptr);
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: his soldier 0x%08X killed through its death function (%s 0x%08X)", ScanMsSinceAttach(),
            f.k + 3, static_cast<unsigned>(soldier), InExe(death) ? "the game's, rva" : "ragdoll.cpp's hook at",
            InExe(death) ? RvaOf(death) : static_cast<unsigned>(death));
    return true;
}

void F34PauseColours(Flow34& f)
{
    for (int i = 0; i < kPauseItems; ++i)
    {
        const bool off = i == kPauseOptions && !f.optsOn;
        if (f.pauseLook) // 20g-r9: the game's bars keep their colours, dimmed but the one picked
            ClassWheelTextColor(f.item[i], off ? kLookOffArgb : (i == f.pauseSel ? kLookSelArgb : kLookItemArgb));
        else
            ClassWheelTextColor(f.item[i], off ? kPauseOffArgb : (i == f.pauseSel ? kPauseSelArgb : kPauseItemArgb));
    }
}

// ---- 20g-r8: his OPTIONS page (CONTROLLER) -------------------------------------------------------------------------------
// room in the runtime heap for its texts (F34HeapRoom's test without its "the auto-spawn looks after him" side)
bool F34OptsHeapOk()
{
    uint32_t total = 0;
    uint32_t largest = 0;
    int blocks = 0;
    const int rt = HeapIndexAt(g_runtimeHeapVa);
    return HeapFreeStats(rt, kF34MinFree, &total, &largest, &blocks) && total >= kF34MinFree;
}

void F34OptsValues(Flow34& f)
{
    ClassWheelSetText(f.optValue[0], f.ctl.invert ? L"ON" : L"OFF");
    ClassWheelSetText(f.optValue[1], f.ctl.invertFly ? L"ON" : L"OFF");
    wchar_t t[24];
    swprintf(t, 24, L"<  %d  >", f.ctl.sens);
    ClassWheelSetText(f.optValue[2], t);
}

void F34OptsColours(Flow34& f)
{
    for (int i = 0; i < kOptItems; ++i)
    {
        const uint32_t c = i == f.optSel ? kPauseSelArgb : kPauseItemArgb;
        ClassWheelTextColor(f.optLabel[i], c);
        ClassWheelTextColor(f.optValue[i], c);
    }
}

// on: the page in place of the pause items (made the first time); off: back to the items (saved if changed)
void F34Opts(Flow34& f, bool on)
{
    if (on && !f.optsMade)
    {
        if (!f.group || !F34OptsHeapOk())
        {
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: his OPTIONS page NOT made (%s)", ScanMsSinceAttach(), f.k + 3,
                    f.group ? "the runtime heap is low" : "no group");
            ClassWheelPlaySound(2);
            return;
        }
        f.optsMade = true;
        static const wchar_t* const kLabels[kOptItems] = { L"INVERT LOOK", L"INVERT FLYING", L"SENSITIVITY" };
        static const char* const kWhat[kOptItems] = { "invert look", "invert flying", "sensitivity" };
        for (int i = 0; i < kOptItems; ++i)
        {
            f.optLabel[i] = F34TextEx(f, kLabels[i], kF34OptLabelX, kF34OptY[i], kF34OptScale, 0, kPauseItemArgb, kWhat[i]);
            f.optValue[i] = F34TextEx(f, L" ", kF34OptValueX, kF34OptY[i], kF34OptScale, 2, kPauseItemArgb, "option value");
        }
        f.optHint = F34TextEx(f, L"LEFT / RIGHT  CHANGE", 0.5f, kF34OptHintY, kF34OptHintScale, 2, kOptHintArgb, "options hint");
    }
    if (on)
    {
        f.ctl = PlayerNControlsGet(f.k);
        f.optSel = 0;
        f.shownOptSel = -1;
        f.optsDirty = false;
        F34OptsValues(f);
    }
    else if (f.optsDirty)
    {
        PlayerNControlsSave(f.k);
        f.optsDirty = false;
    }
    f.opts = on;
    for (int i = 0; i < kPauseItems; ++i)
        ClassWheelShowElement(f.item[i], !on);
    for (int i = 0; i < kOptItems; ++i)
    {
        ClassWheelShowElement(f.optLabel[i], on);
        ClassWheelShowElement(f.optValue[i], on);
    }
    ClassWheelShowElement(f.optHint, on);
    ClassWheelSetText(f.pauseTitle, on ? L"CONTROLLER" : L"GAME PAUSED");
    if (!on)
    {
        f.shownPauseSel = -1; // (colours again)
    }
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s (invert look %s, invert flying %s, sensitivity %d)", ScanMsSinceAttach(), f.k + 3,
            on ? "OPTIONS - his CONTROLLER page is up" : "his CONTROLLER page closed - back to the pause items",
            f.ctl.invert ? "ON" : "off", f.ctl.invertFly ? "ON" : "off", f.ctl.sens);
}

// up / down pick; left = off / less, right = on / more, A = toggle / more (10 -> 1); back: B
void F34OptsFrame(Flow34& f, bool up, bool down, bool left, bool right, bool a, bool back)
{
    if (back)
    {
        F34Sound(f, 2, "B - back from his OPTIONS page");
        F34Opts(f, false);
        return;
    }
    if (up || down)
    {
        f.optSel = (f.optSel + (down ? 1 : kOptItems - 1)) % kOptItems;
        F34Sound(f, 0, "options up / down");
    }
    PlayerNControls c = f.ctl;
    if (f.optSel == 0 || f.optSel == 1)
    {
        int& v = f.optSel == 0 ? c.invert : c.invertFly;
        if (a)
            v = v ? 0 : 1;
        else if (left)
            v = 0;
        else if (right)
            v = 1;
    }
    else
    {
        if (left && c.sens > 1)
            --c.sens;
        if (right && c.sens < 10)
            ++c.sens;
        if (a)
            c.sens = c.sens >= 10 ? 1 : c.sens + 1;
    }
    if (c.invert != f.ctl.invert || c.invertFly != f.ctl.invertFly || c.sens != f.ctl.sens)
    {
        f.ctl = c;
        PlayerNControlsSet(f.k, c);
        f.optsDirty = true;
        F34OptsValues(f);
        ClassWheelPlaySound(0);
    }
    if (f.optSel != f.shownOptSel)
    {
        f.shownOptSel = f.optSel;
        F34OptsColours(f);
    }
}

// 20g-r7: "are you sure" (what = 3 restart mission, 4 quit; 0 = back to the items)
void F34Confirm(Flow34& f, int what)
{
    f.confirm = what;
    for (int i = 0; i < kPauseItems; ++i)
        ClassWheelShowElement(f.item[i], what == 0);
    if (what)
    {
        ClassWheelSetText(f.confirmText[0], L"DO YOU REALLY WANT TO");
        ClassWheelSetText(f.confirmText[1], what == 3 ? L"RESTART THIS BATTLE?" : L"QUIT THIS BATTLE?");
        ClassWheelSetText(f.confirmText[2], L"A YES      B NO");
    }
    for (int i = 0; i < 3; ++i)
        ClassWheelShowElement(f.confirmText[i], what != 0);
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: %s", ScanMsSinceAttach(), f.k + 3,
            what == 3 ? "RESTART MISSION - are you sure?" : (what == 4 ? "QUIT - are you sure?" : "back to the pause items"));
}

void F34PauseFrame(Flow34& f, uintptr_t ch, uintptr_t pad)
{
    if (f.real) // 21h-r2: his real pause menu - the game's screen runs it
    {
        RpFrame(f, ch, pad);
        return;
    }
    const PadIn in = PadReadAt(pad);
    PadEdges& p = f.pad;
    const bool up = StepEdge(in.up, p.uDown, p.uSince, p.uLast);
    const bool down = StepEdge(in.down, p.dDown, p.dSince, p.dLast);
    const bool left = StepEdge(in.left, p.lDown, p.lSince, p.lLast);    // 20g-r8: (his OPTIONS page)
    const bool right = StepEdge(in.right, p.rDown, p.rSince, p.rLast);
    const bool a = PressEdge(in.a, p.aDown);
    const bool b = PressEdge(in.b, p.bDown);
    const bool st = PressEdge(in.start, p.sDown);
    if (f.opts) // 20g-r8: his OPTIONS page - B back to the items, Start closes the pause screen
    {
        if (st)
        {
            F34Sound(f, 2, "Start - out (from his OPTIONS page)");
            F34PauseClose(f, "Start - back (from his OPTIONS page)");
            return;
        }
        F34OptsFrame(f, up, down, left, right, a, b);
        return;
    }
    if (f.confirm)
    {
        if (a)
        {
            F34Sound(f, 1, "A - yes");
            const int what = f.confirm;
            F34PauseClose(f, what == 3 ? "RESTART MISSION - yes" : "QUIT - yes");
            F34GameAction(f, what);
        }
        else if (b || st)
        {
            F34Sound(f, 2, "B - no");
            F34Confirm(f, 0);
        }
        return;
    }
    if (up || down)
    {
        for (int step = 0; step < kPauseItems; ++step)
        {
            f.pauseSel = (f.pauseSel + (down ? 1 : kPauseItems - 1)) % kPauseItems;
            if (f.pauseSel != kPauseOptions || f.optsOn)
                break;
        }
        F34Sound(f, 0, "up / down");
    }
    if (f.pauseSel != f.shownPauseSel)
    {
        f.shownPauseSel = f.pauseSel;
        F34PauseColours(f);
    }
    if (b || st || (a && f.pauseSel == 0))
    {
        F34Sound(f, 2, b ? "B - back (out)" : (st ? "Start - out" : "RESUME GAME - out")); // 20g-r9: the back sound going out
        F34PauseClose(f, b ? "B - back" : (st ? "Start - back" : "RESUME GAME"));
        return;
    }
    if (!a)
        return;
    F34Sound(f, 1, "A");
    if (f.pauseSel == kPauseOptions) // 20g-r8
    {
        if (f.optsOn)
            F34Opts(f, true);
        return;
    }
    if (f.pauseSel == 3 || f.pauseSel == 4)
    {
        F34Confirm(f, f.pauseSel);
        return;
    }
    // RESPAWN
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: RESPAWN on his pause screen", ScanMsSinceAttach(), f.k + 3);
    F34PauseClose(f, "RESPAWN");
    F34Kill(f, ch);
}

void F34LogCharacter(Flow34& f, const char* what, uintptr_t ch)
{
    if (!ch || !F34Budget(f))
        return;
    Log("[p34spawn] %lu ms: player %d: %s: his character team %ld, class index %ld, class 0x%08X, spawn point 0x%08X, post %ld, "
        "ready %u, unit 0x%08X",
        ScanMsSinceAttach(), f.k + 3, what, static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharTeam))),
        static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharClassIdx))), static_cast<unsigned>(ReadU32(ch + kCharClass)),
        static_cast<unsigned>(ReadU32(ch + kCharSpawnPoint)), static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharPost))),
        static_cast<unsigned>(ReadU8(ch + kCharReady)), static_cast<unsigned>(ReadU32(ch + kCharUnit)));
}

void F34NewMatch(Flow34& f, uintptr_t sm, long sdDeletes)
{
    // 20g-r5: nothing of the old match is deleted here - the group too is only forgotten (r4 kept it, then deleted it
    // in the new match: the old match's heap was gone and its memory reused). The match teardown deletes it while the
    // old match is still loaded (P2SpawnP34Teardown).
    F34Forget(f, "a new match (or none)", false);
    const int k = f.k;
    const int lines = f.lines;
    const bool failed = f.groupFailed;
    f = Flow34();
    f.k = k;
    f.lines = lines;
    f.groupFailed = failed;
    f.sm = sm;
    f.sdDeletes = sdDeletes;
}

void F34SideFrame(Flow34& f, uintptr_t sm, bool left, bool right, bool a)
{
    if (left && f.pick != 0)
    {
        f.pick = 0;
        ClassWheelPlaySound(0);
    }
    if (right && f.pick != 1)
    {
        f.pick = 1;
        ClassWheelPlaySound(0);
    }
    if (f.pick != f.shownPick)
    {
        f.shownPick = f.pick;
        ClassWheelTextGrey(f.name[0], f.pick != 0);
        ClassWheelTextGrey(f.name[1], f.pick != 1);
        ClassWheelModelGrey(f.model[0], f.pick != 0);
        ClassWheelModelGrey(f.model[1], f.pick != 1);
    }
    if (!a)
        return;
    const int team = f.pick + 1;
    if (team != f.team)
    {
        f.post = 0; // another side: its own posts and classes
        f.cls = 0;
    }
    f.team = team;
    f.sideDone = true;
    ClassWheelPlaySound(1);
    wchar_t name[48];
    char a8[96];
    TeamNameW(sm, team, name, 48);
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: A on the side screen - side %d \"%s\"", ScanMsSinceAttach(), f.k + 3, team,
            Narrow(name, a8, sizeof(a8)));
    F34Show(f, kScrPost, sm, "side chosen");
}

void F34PostFrame(Flow34& f, uintptr_t sm, bool prev, bool next, bool a, bool b)
{
    if (b && SidesLockedNow() == 1 && f.sideDone) // 21g-r3: no side screen to go back to
    {
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: B on the spawn point screen - sides are locked, no side screen to go back to",
                ScanMsSinceAttach(), f.k + 3);
        b = false;
    }
    if (b)
    {
        ClassWheelPlaySound(2);
        F34Show(f, kScrSide, sm, "B on the spawn point screen");
        return;
    }
    // the list (NextSpawnPoint round once) when the screen is up, on a press, and twice a second
    const DWORD now = GetTickCount();
    if (!prev && !next && !a && f.shownPost == f.post && f.post && now - f.postsAt < 500)
        return;
    f.postsAt = now;
    uintptr_t posts[kMaxPosts];
    const int n = TeamPosts(sm, f.team, posts, kMaxPosts);
    int k = -1;
    for (int i = 0; i < n; ++i)
        if (posts[i] == f.post)
            k = i;
    bool lost = false;
    if (n && k < 0)
    {
        lost = f.post != 0;
        k = 0;
        f.post = posts[0];
    }
    if (n && (prev || next))
    {
        k = (k + (next ? 1 : n - 1)) % n;
        f.post = posts[k];
        ClassWheelPlaySound(0);
    }
    if (!n)
        f.post = 0;
    if (f.post != f.shownPost || n != f.shownCount)
    {
        f.shownPost = f.post;
        f.shownCount = n;
        wchar_t name[96];
        wchar_t cnt[32];
        char a8[200];
        if (n)
        {
            PostNameW(f.post, name, 96);
            swprintf(cnt, 32, L"%d / %d", k + 1, n);
        }
        else
        {
            wcscpy_s(name, 96, L"NO SPAWN POINT");
            cnt[0] = 0;
        }
        ClassWheelSetText(f.main, name);
        ClassWheelSetText(f.count, cnt);
        if (f.verbose && F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: spawn point %d of %d \"%s\" (0x%08X)", ScanMsSinceAttach(), f.k + 3, k + 1, n,
                Narrow(name, a8, sizeof(a8)), static_cast<unsigned>(f.post));
    }
    // the vehicles at his post, as the game writes them for player 1's (its own PopupFill)
    if (f.veh && f.post && f.post != f.vehPost)
    {
        f.vehPost = f.post;
        wchar_t veh[256];
        ClassWheelSetText(f.veh, ClassWheelVehiclesAt(f.post, veh, 256) ? veh : L"");
    }
    if (!a)
        return;
    if (!n)
    {
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: A on the spawn point screen - side %d has no post to spawn at", ScanMsSinceAttach(),
                f.k + 3, f.team);
        return;
    }
    if (lost)
    {
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: A on the spawn point screen - the post he had is gone, now showing the first; A again "
                "to take it",
                ScanMsSinceAttach(), f.k + 3);
        return;
    }
    ClassWheelPlaySound(1);
    F34Show(f, kScrClass, sm, "spawn point chosen");
}

void F34ClassFrame(Flow34& f, uintptr_t sm, uintptr_t ch, bool left, bool right, bool a, bool b)
{
    const int cycle = TeamCycle(sm, f.team);
    if (f.cls >= cycle)
        f.cls = 0;
    if (f.ready == 0 && cycle > 0 && (left || right))
    {
        f.cls = left ? (f.cls + cycle - 1) % cycle : (f.cls + 1) % cycle;
        ClassWheelPlaySound(0);
    }
    // the class name (no EQUIPMENT box: the Xbox's 4-player screen has none; 21j-r9: with the console windows the class window's
    // bar has the name, its box EQUIPMENT and the weapons)
    if (f.cls != f.shownCls || f.team != f.shownTeam)
    {
        f.shownCls = f.cls;
        f.shownTeam = f.team;
        const uintptr_t cls = TeamClass(sm, f.team, f.cls);
        wchar_t name[96];
        wchar_t box[600];
        int weapons = 0;
        char a8[200];
        const bool ok = ClassTextsW(cls, name, 96, box, 600, &weapons);
        if (ok)
            ClassWheelSetText(f.main, name);
        if (ok && f.box) // 20g-r13: a horizontal half - his EQUIPMENT list; 21j-r9: the class window's box
            ClassWheelSetText(f.box, box);
        if ((f.verbose || !ok) && F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: class %d of %d \"%s\" (0x%08X)%s", ScanMsSinceAttach(), f.k + 3, f.cls, cycle,
                ok ? Narrow(name, a8, sizeof(a8)) : "?", static_cast<unsigned>(cls), ok ? "" : " - not a soldier class");
    }
    // his side's classes on a ring (class_wheel.cpp's ring k + 1, the Xbox's arrangement), turned to his
    if (g_soldiersWanted && cycle > 0)
    {
        uintptr_t classes[8] = {};
        const int n = cycle < 8 ? cycle : 8;
        for (int i = 0; i < n; ++i)
            classes[i] = TeamClass(sm, f.team, i);
        float cx = 0.0f;
        float cy = 0.0f;
        float fx = 0.0f;
        float fy = 0.0f;
        float w = 0.0f;
        float h = 0.0f;
        // 20g-r13: a horizontal half - the Classic Collection's ring (player 2's horizontal one: the picked soldier in front in
        // the middle, the others round it, the back ones higher)
        const bool hz = F34Hz(f);
        if (F34Place(f, hz ? kHzRingX : F34RingCx(f), hz ? kHzRingFeetY : kF34RingY, &cx, &cy) && F34Place(f, 1.0f, 1.0f, &w, &h))
        {
            fx = kGameSoldierRootX; // 20g-r9: his group's origin is his root's (F34Place), so root units directly
            fy = kGameSoldierRootY;
            const int sel = f.cls < n ? f.cls : 0;
            const float radius = (hz ? kHzRingRadius : F34RingR(f)) * w; // 21j-r9: left of the class window while it is up
            const float size = hz ? kHzRingTall * h / kHzSoldierUnits : kF34RingSize;
            const float lift = hz ? kHzRingLift * h : 0.0f;
            const int placed = ClassWheelRingFrame(f.k + 1, f.group, classes, n, sel, cx, cy, radius, size, lift, fx, fy,
                                                   hz ? kRingStyleClassic : kRingStyleXbox);
            if (!f.ringShown && (f.verbose || !placed) && F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: class ring%s: %d of %d class(es) placed, centre at %.0f %.0f (%.3f %.3f of his "
                    "%s), radius %.0f, size %.3f, lift %.0f, droidekas around %.0f %.0f",
                    ScanMsSinceAttach(), f.k + 3, hz ? " (horizontal, Classic's)" : "", placed, n, cx, cy, hz ? kHzRingX : F34RingCx(f),
                    hz ? kHzRingFeetY : kF34RingY, hz ? "half" : "quarter", radius, size, lift, fx, fy);
            f.ringShown = true;
        }
    }
    if (f.ready == 0)
    {
        if (f.waitShown)
        {
            f.waitShown = false;
            ClassWheelSetText(f.prompt, L"SELECT CHARACTER");
        }
        if (a && cycle > 0)
        {
            // what the PC Spawn button does to its character: SetSpawnChoice(team, class index,
            // ValidPost(post, team)), then SetReady(1)
            ClassWheelPlaySound(1);
            const uintptr_t gsm = g_getSm();
            const uintptr_t post = g_validPost(reinterpret_cast<void*>(gsm), nullptr, f.post, f.team);
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: A on the class screen - SetSpawnChoice(side %d, class %d, post 0x%08X (ValidPost "
                    "of 0x%08X)) on his character 0x%08X",
                    ScanMsSinceAttach(), f.k + 3, f.team, f.cls, static_cast<unsigned>(post), static_cast<unsigned>(f.post),
                    static_cast<unsigned>(ch));
            g_setSpawnChoice(reinterpret_cast<void*>(ch), nullptr, f.team, f.cls, post);
            if (static_cast<int32_t>(ReadU32(ch + kCharTeam)) != f.team && F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: the game kept him on side %ld (side %d full?)", ScanMsSinceAttach(), f.k + 3,
                    static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharTeam))), f.team);
            f.ready = 1;
            f.readyAt = GetTickCount();
            f.readyLogged = false;
            F34LogCharacter(f, "after SetSpawnChoice", ch);
        }
        else if (b)
        {
            ClassWheelPlaySound(2);
            F34Show(f, kScrPost, sm, "B on the class screen");
            return;
        }
    }
    if (f.ready == 1)
    {
        if (!g_waitP1 || PlayerInWorld(0)) // 20g-r6: no waiting for player 1 (SpawnWaitP1=0)
        {
            g_setReady(reinterpret_cast<void*>(ch), nullptr, 1);
            f.ready = 2;
            f.readyAt = GetTickCount();
            F34LogCharacter(f, g_waitP1 ? "SetReady(1) - player 1 is in the world" : "SetReady(1)", ch);
            if (f.waitShown)
            {
                f.waitShown = false;
                ClassWheelSetText(f.prompt, L"SELECT CHARACTER");
            }
        }
        else if (!f.waitShown)
        {
            f.waitShown = true;
            ClassWheelSetText(f.prompt, L"WAITING FOR PLAYER 1");
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: player 1 is not in the world - he spawns when player 1 is (B cancels)",
                    ScanMsSinceAttach(), f.k + 3);
        }
    }
    if (f.ready && b)
    {
        const int was = f.ready;
        if (was == 2)
            g_setReady(reinterpret_cast<void*>(ch), nullptr, 0);
        f.ready = 0;
        ClassWheelPlaySound(2);
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: B - spawn cancelled%s", ScanMsSinceAttach(), f.k + 3, was == 2 ? " (SetReady(0))" : "");
        return;
    }
    if (f.ready == 2 && !f.readyLogged && GetTickCount() - f.readyAt > kReadyLogAfterMs)
    {
        f.readyLogged = true;
        F34LogCharacter(f, "READY but the game has not spawned him yet", ch);
    }
}

// player 3 / 4, every frame (game thread, players34.cpp's frame hook, before the views are drawn)
// 21h-r8: the match is over: his spawn screens / pause menu closed for good, his soldier still (the calm packet), his
// VICTORY / DEFEAT while the game shows players 1 / 2's
void F34EndFrame(Flow34& f, uintptr_t ch)
{
    if (!f.endDone)
    {
        f.endDone = true;
        if (f.screen != kScrNone)
            F34Close(f, "the match is over");
        f.pauseFrom = kScrNone;
        PlayerNMenuInput(f.k, true);
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: THE MATCH IS OVER - his screens closed, no Start, his soldier still (%s)",
                ScanMsSinceAttach(), f.k + 3, ch && ReadU32(ch + kCharUnit) ? "in the world" : "no soldier");
    }
    const uintptr_t obj = g_vdObj;
    const bool showing = obj && ReadU8(obj + 0x4A0);
    // 21h-r9: the game's own image (as players 1 / 2's), the text if it could not be made
    if (g_vdi.ok && (g_vdp[f.k].group || VdP34Make(f.k, f.root)))
    {
        const int r = showing ? EndResult(ch) : -1;
        if (showing && r >= 0 && !g_vdp[f.k].shown)
        {
            ClassWheelShowElement(f.group, true); // (his root drawn, his HUD hidden)
            f.shown = true;
            VdP34Show(f.k, r, true);
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: %s - the game's image, as players 1 / 2's (his team %ld, the winners' mask %08X)",
                    ScanMsSinceAttach(), f.k + 3, r ? "VICTORY" : "DEFEAT", static_cast<long>(ReadU32(ch + kCharTeam)),
                    static_cast<unsigned>(g_endMask));
        }
        else if (!showing && g_vdp[f.k].shown)
            VdP34Show(f.k, 0, false);
        return;
    }
    if (showing && !f.endText)
    {
        const int r = EndResult(ch);
        if (r < 0)
            return;
        F34ElemsDrop(f, true, "his VICTORY / DEFEAT");
        f.endText = F34TextEx(f, r ? L"VICTORY" : L"DEFEAT", 0.5f, 0.55f, 2.6f, 2, r ? 0xFF40C040u : 0xFFC04040u,
                              r ? "VICTORY" : "DEFEAT");
        ClassWheelShowElement(f.group, true);
        f.shown = true;
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: %s (his team %ld, the winners' mask %08X)", ScanMsSinceAttach(), f.k + 3,
                r ? "VICTORY" : "DEFEAT", static_cast<long>(ReadU32(ch + kCharTeam)), static_cast<unsigned>(g_endMask));
    }
    else if (!showing && f.endText)
    {
        F34ElemsDrop(f, true, "the game's VICTORY / DEFEAT went (the stats)");
        f.endText = 0;
    }
}

// 21h-r6: Start on his side / spawn point / class screen: his real pause menu over it (the screen goes - his view shows the
// world behind the menu, as players 1 / 2's pause there); it comes back when the menu goes
void F34SpawnPause(Flow34& f, uintptr_t sm)
{
    (void)sm;
    const char* why = "";
    if (!RpCanOpen(f, &why))
    {
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: Start on his %s screen - no pause menu there (%s)", ScanMsSinceAttach(), f.k + 3,
                F34ScreenName(f.screen), why);
        return;
    }
    const int from = f.screen;
    F34Close(f, "Start - his pause menu over his spawn screens");
    RpOpen(f);
    f.pauseFrom = from;
    if (F34Budget(f))
        Log("[p34spawn] %lu ms: player %d: Start on his %s screen - his pause menu over it (the screen comes back when it goes)",
            ScanMsSinceAttach(), f.k + 3, F34ScreenName(from));
}

void F34FrameOne(Flow34& f, uintptr_t sm, long deletes)
{
    CmApplyFrame(f.k); // 21h-r6: his own button map in his PlayerInput
    if (sm != f.sm || deletes != f.sdDeletes)
        F34NewMatch(f, sm, deletes); // a new match (or none): the old one's elements are forgotten
    const uintptr_t ch = sm ? CharOfPlayer(f.k + 2) : 0;
    if (ch && !f.matchReady)
    {
        const uintptr_t sd = g_sdGlobal ? ReadU32(g_sdGlobal) : 0;
        const uint32_t teamShown = sd ? ReadU32(sd + kSdTeamShown) : 0;
        if (sd && (teamShown == 1 || teamShown == 2) && ReadU32(sd + kSdCharacter))
        {
            f.matchReady = true;
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: the match is set up (his character 0x%08X, his view %d, his pad %s)", ScanMsSinceAttach(),
                    f.k + 3, static_cast<unsigned>(ch), PlayerNSpecView(f.k), PlayerNPadAddress(f.k + 2) ? "connected" : "NOT connected");
            if (f.k == 0 && !f.heapsLogged && F34Budget(f))
            {
                // 20f-r3: the heaps at match start (the one current when this frame began, the runtime heap, the 16 MB one)
                f.heapsLogged = true;
                char a[160];
                char b[160];
                char c[160];
                char d[120];
                Log("[p34spawn] %lu ms: heaps at match start: current when this frame began: %s; %s; %s; %s", ScanMsSinceAttach(),
                    HeapText(g_heapAtEntry, a, sizeof(a)), HeapText(HeapIndexAt(g_runtimeHeapVa), b, sizeof(b)),
                    HeapText(HeapIndexAt(g_otherHeapVa), c, sizeof(c)), HeapFailText(d, sizeof(d)));
            }
        }
    }
    const int view = PlayerNSpecView(f.k);
    const uintptr_t pad = PlayerNPadAddress(f.k + 2);
    const bool eligible = ch && view >= 0 && f.matchReady;
    if (eligible && (pad != 0) != f.padWas)
    {
        f.padWas = pad != 0;
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d: pad %d %s", ScanMsSinceAttach(), f.k + 3, f.k + 3,
                pad ? "connected - his spawn screens on" : "NOT connected - his spawn screens off, the auto-spawn spawns him");
    }
    f.owns = eligible && pad && !f.groupFailed && !f.lowHeap;
    if (!f.owns)
    {
        if (f.screen != kScrNone || f.shown)
        {
            if (!ch)
                F34Forget(f, "no character - the match is ending?", true);
            else
                F34Close(f, !pad ? "his pad is not connected" : (view < 0 ? "his view is gone" : "his screens are off"));
        }
        return;
    }
    if (!F34GroupMake(f, view))
    {
        F34Close(f, "no group");
        f.owns = false;
        return;
    }
    if (EndOver()) // 21h-r8
    {
        F34EndFrame(f, ch);
        return;
    }

    // in the world: no screen
    if (ReadU32(ch + kCharUnit))
    {
        if (!f.inWorld)
        {
            f.inWorld = true;
            char what[48];
            snprintf(what, sizeof(what), "PLAYER %d IS IN THE WORLD", f.k + 3);
            F34LogCharacter(f, what, ch);
        }
        f.ready = 0;
        if (f.screen == kScrPause34) // 20g-r6: his pause screen
        {
            F34PauseFrame(f, ch, pad);
            return;
        }
        if (f.screen != kScrNone)
            F34Close(f, "he is in the world");
        if (g_pauseWanted)
        {
            const PadIn in = PadReadAt(pad);
            if (PressEdge(in.start, f.pad.sDown))
                F34PauseOpen(f, sm);
        }
        return;
    }
    if (f.screen == kScrPause34 && f.pauseFrom != kScrNone) // 21h-r6: his pause menu over his spawn screens
    {
        const int from = f.pauseFrom;
        F34PauseFrame(f, ch, pad);
        if (f.screen == kScrNone)
        {
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: his pause menu went - his %s screen back (his buttons latched)", ScanMsSinceAttach(),
                    f.k + 3, F34ScreenName(from));
            F34Show(f, from, sm, "back from his pause menu");
        }
        return;
    }
    if (f.screen == kScrPause34) // 20g-r6: down while his pause screen was up
        F34PauseClose(f, "he is down");
    if (f.inWorld)
    {
        f.inWorld = false;
        f.leftWorldAt = GetTickCount();
        f.timerLogged = false;
        const float timer = ReadF32(ch + kCharTimer);
        f.timerRuns = timer == timer && timer > 0.0f && timer < kRespawnTimerMax;
        f.timerSeen = timer;
        f.timerSeenAt = f.leftWorldAt;
        if (F34Budget(f))
            Log("[p34spawn] %lu ms: player %d left the world - his spawn point screen %s (his respawn timer +C0h reads %.2f s)",
                ScanMsSinceAttach(), f.k + 3, f.timerRuns ? "when his respawn timer runs out" : "in 1500 ms", timer);
    }
    if (f.screen == kScrNone)
    {
        if (f.leftWorldAt)
        {
            // his respawn timer, as player 2's (19i): 30 s at most; 1.5 s when it does not run
            const DWORD now = GetTickCount();
            const float timer = ReadF32(ch + kCharTimer);
            const DWORD since = now - f.leftWorldAt;
            if (timer != f.timerSeen)
            {
                f.timerSeen = timer;
                f.timerSeenAt = now;
            }
            if (!f.timerRuns && since < 500 && timer == timer && timer > 0.0f && timer < kRespawnTimerMax)
                f.timerRuns = true; // set by the game a frame or two after the unit went
            const bool stuck = now - f.timerSeenAt > 2000;
            const bool byTimer = f.timerRuns && !stuck && timer == timer;
            const bool wait = byTimer ? (timer > 0.0f && since < kRespawnMaxWaitMs) : since < kRespawnDelayMs;
            if (wait)
                return;
            if (!f.timerLogged && F34Budget(f))
            {
                f.timerLogged = true;
                Log("[p34spawn] %lu ms: player %d: %s, %lu ms after his death - his spawn point screen now", ScanMsSinceAttach(), f.k + 3,
                    byTimer ? (timer <= 0.0f ? "his respawn timer ran out" : "his respawn timer still runs (the 30 s limit)")
                            : (f.timerRuns ? "his respawn timer stopped changing - 1.5 s" : "1.5 s"),
                    static_cast<unsigned long>(since));
            }
        }
        const int32_t t = static_cast<int32_t>(ReadU32(ch + kCharTeam));
        if (!f.sideDone)
        {
            f.pick = t == 2 ? 1 : 0;
            f.team = t == 2 ? 2 : 1;
            // 21g-r3: sides locked (campaign / Galactic Conquest): no side screen - the side he has to play
            const int locked = SidesLockedNow();
            if (locked == 1)
            {
                // his side as a relation to player 1's (Galactic Conquest: the conquest's side numbers are not the battle's -
                // a battle's team 1 is the side its map was made for); player 1's team in this battle: his character's,
                // else the game's side table
                const char* why = "";
                const int rel = LockedSideP34(f.k, &why);
                const uintptr_t ch0 = SpawnScanCharacter(0);
                const int32_t t0 = ch0 ? static_cast<int32_t>(ReadU32(ch0 + kCharTeam)) : 0;
                const int table0 = PlayerSideNow(0);
                const int p1team = (t0 == 1 || t0 == 2) ? static_cast<int>(t0) : ((table0 == 1 || table0 == 2) ? table0 : 0);
                const int side = (p1team && rel) ? (rel > 0 ? p1team : 3 - p1team) : 0;
                if (side == 1 || side == 2)
                {
                    f.pick = side == 2 ? 1 : 0;
                    f.team = side;
                    f.sideDone = true;
                    if (F34Budget(f))
                        Log("[p34spawn] %lu ms: player %d: sides are locked (campaign / Galactic Conquest) - no side screen: team "
                            "%d (%s; player 1's team %d - his character's %ld, the side table's %d; player %d's character's "
                            "team %ld)",
                            ScanMsSinceAttach(), f.k + 3, side, why, p1team, static_cast<long>(t0), table0, f.k + 3,
                            static_cast<long>(t));
                }
                else if (F34Budget(f))
                    Log("[p34spawn] %lu ms: player %d: sides are locked but his team is NOT known (%s; player 1's character's "
                        "team %ld, the side table's %d) - the side screen",
                        ScanMsSinceAttach(), f.k + 3, why, static_cast<long>(t0), table0);
            }
            else if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d: sides %s - the side screen (his character's team %ld)", ScanMsSinceAttach(),
                    f.k + 3, locked == 0 ? "can be switched" : "NOT known", static_cast<long>(t));
        }
        else if ((t == 1 || t == 2) && t != f.team)
        {
            if (F34Budget(f))
                Log("[p34spawn] %lu ms: player %d is on side %ld, not %d - his screens follow the game", ScanMsSinceAttach(), f.k + 3,
                    static_cast<long>(t), f.team);
            f.team = t;
            f.post = 0;
            f.cls = 0;
        }
        f.ready = 0;
        F34Show(f, f.sideDone ? kScrPost : kScrSide, sm, f.sideDone ? "he is down - the spawn point screen" : "match start - the side screen");
        return;
    }

    // his screen is up: his pad drives it
    const PadIn in = PadReadAt(pad);
    PadEdges& p = f.pad;
    const bool left = StepEdge(in.left, p.lDown, p.lSince, p.lLast);
    const bool right = StepEdge(in.right, p.rDown, p.rSince, p.rLast);
    const bool up = StepEdge(in.up, p.uDown, p.uSince, p.uLast);
    const bool down = StepEdge(in.down, p.dDown, p.dSince, p.dLast);
    const bool a = PressEdge(in.a, p.aDown);
    const bool b = PressEdge(in.b, p.bDown);
    const bool st = PressEdge(in.start, p.sDown); // 21h-r6
    if (st && g_spawnPauseWanted && g_pauseWanted && f.ready == 0 &&
        (f.screen == kScrSide || f.screen == kScrPost || f.screen == kScrClass))
    {
        F34SpawnPause(f, sm);
        return;
    }
    if (f.screen == kScrSide)
        F34SideFrame(f, sm, left, right, a);
    else if (f.screen == kScrPost)
        F34PostFrame(f, sm, left || up, right || down, a, b);
    else if (f.screen == kScrClass)
        F34ClassFrame(f, sm, ch, left, right, a, b);
}

// ---- 21j-r9: the console windows' functions (at start, with players 3 / 4's screens; [panels] lines) -----------------------
// RedGroupElement's constructor: the call at +0Ah of RedScreenGroupElement's (the victory / defeat display's group); its
// vtable's RTTI and its deleting destructor (frees A0h) checked. RedBitmapElement's: the display's image constructor; its
// RTTI and deleting destructor (frees 100h). Its texture (by name) from the display's image setup (+20h), its SetRect from
// the setup's SetSize (+4Fh) at +9Dh, its UVs from the display's Show (+11Ah) - each body checked.
const char* const kPwGroupCtorBody =
    "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? 8D 8E 80 00 00 00 C7 06 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C6 5E 8B E5 5D C3"; // GOG 1.3 0x0029FC90
constexpr uint32_t kPwGroupCtorVt = 0x17;
const char* const kPwScreenGroupCtorBody = "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? C7 06 ?? ?? ?? ?? 8B C6 C7 86 A4 00 00 00 00 00 00 00"; // 0x00291F50
constexpr uint32_t kPwScreenGroupBaseCall = 0x0A;
const char* const kPwGroupDeleteBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? F6 45 08 01 74 0E 68 A0 00 00 00 56 E8";      // 0x0029FCF0
const char* const kPwImageCtorBody = "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? 6A 70 8D 86 88 00 00 00 C7 06";  // 0x002A8F80
constexpr uint32_t kPwImageCtorVt = 0x19;
// (its vector deleting destructor: one element - flag 2 clear - its destructor, then 100h freed; an array - 100h each)
const char* const kPwImageDeleteBody =
    "55 8B EC 8B 45 08 53 8B D8 83 E3 01 56 8B F1 A8 02 74 36 57 68 ?? ?? ?? ?? FF 76 F0 8D 7E F0 68 00 01 00 00 56 E8 ?? ?? ?? ?? "
    "85 DB 74 12 8B 0F C1 E1 08 83 C1 10 51 57 E8 ?? ?? ?? ?? 83 C4 08 8B C7 5F 5E 5B 5D C2 04 00 E8 ?? ?? ?? ?? 85 DB 74 0E 68 00 01 "
    "00 00 56 E8"; // GOG 1.3 0x0015FA10
const char* const kPwTexBody = "55 8B EC 56 57 FF 75 08 8B F9 8D 4D 08 8B 37 E8";                                // 0x00096440
const char* const kPwSizeBody = "55 8B EC 8B 45 10 0F 57 C0";                                                    // 0x002A94E0
constexpr uint32_t kPwSizeRectCall = 0x9D;
const char* const kPwRectBody = "55 8B EC F3 0F 10 45 08 F3 0F 10 55 0C F3 0F 10 4D 14 F3 0F 11 81 88 00 00 00"; // 0x002A92A0
const char* const kPwUvBody = "55 8B EC 80 7D 18 00 F3 0F 10 45 14 F3 0F 10 5D 08";                              // 0x002A9320
constexpr uint32_t kVdCtorGroupCtor = 0x14;  // the display's constructor: push imm32 (13h: 68) - RedScreenGroupElement's constructor
constexpr uint32_t kVdCtorImageCtor = 0x2F;  // push imm32 (2Eh: 68) - RedBitmapElement's constructor
constexpr uint32_t kVdCtorImageSize = 0x33;
const char* const kVdCtorImageSizeBytes = "6A 02 68 00 01 00 00"; // (2 of them, 100h bytes each)
constexpr uint32_t kVdInitTexCall = 0x20;
constexpr uint32_t kVdInitSizeCall = 0x4F;
constexpr uint32_t kVdShowUvCall = 0x11A;

// an address pushed in the code, through its jump thunks, as an rva (0: not in the exe's code)
uint32_t PwPushed(uint32_t at)
{
    const uintptr_t v = PsOperand(at);
    if (!v)
        return 0;
    uint32_t r = static_cast<uint32_t>(v - ExeBase());
    for (int hop = 0; hop < 8 && PsCodeByte(r) == 0xE9; ++hop)
        r = static_cast<uint32_t>(static_cast<int64_t>(r) + 5 + ReadI32At(r + 1));
    return r;
}

// a vtable's class (RTTI) and its deleting destructor's body
bool PwVtableIs(uintptr_t vt, const char* cls, const char* deleteBody)
{
    if (!vt || !InExe(vt))
        return false;
    uintptr_t fake = vt; // an "object" whose vtable is vt, for TypeName
    char t[64] = "";
    if (!TypeName(reinterpret_cast<uintptr_t>(&fake), t, sizeof(t)) || strcmp(t, cls) != 0)
        return false;
    const uintptr_t del = ReadU32(vt);
    if (!InExe(del))
        return false;
    uint32_t r = static_cast<uint32_t>(del - ExeBase());
    for (int hop = 0; hop < 8 && PsCodeByte(r) == 0xE9; ++hop)
        r = static_cast<uint32_t>(static_cast<int64_t>(r) + 5 + ReadI32At(r + 1));
    return MatchPatternAt(r, deleteBody);
}

void PwFind()
{
    auto one = [](const char* pat) -> uint32_t {
        uint32_t hits[4] = {};
        return FindPattern(pat, hits, 4) == 1 ? hits[0] : 0;
    };
    const uint32_t make = one(kVdMakeBody);
    const uint32_t vdCtor = (make && PsCodeByte(make + kVdMakeCtorCall) == 0xE8) ? CallTarget(make + kVdMakeCtorCall) : 0;
    const uint32_t show = one(kVdShowBody);
    const uint32_t init = one(kVdInitBody);
    // the group
    const uint32_t screenGroup = (vdCtor && PsCodeByte(vdCtor + kVdCtorGroupCtor - 1) == 0x68) ? PwPushed(vdCtor + kVdCtorGroupCtor) : 0;
    const bool screenGroupOk = screenGroup && MatchPatternAt(screenGroup, kPwScreenGroupCtorBody);
    const uint32_t group = screenGroupOk ? CallTarget(screenGroup + kPwScreenGroupBaseCall) : 0;
    const bool groupOk = group && MatchPatternAt(group, kPwGroupCtorBody) &&
                         PwVtableIs(PsOperand(group + kPwGroupCtorVt), "RedGroupElement", kPwGroupDeleteBody);
    // the bitmap
    const uint32_t image = (vdCtor && PsCodeByte(vdCtor + kVdCtorImageCtor - 1) == 0x68 &&
                            MatchPatternAt(vdCtor + kVdCtorImageSize, kVdCtorImageSizeBytes))
                               ? PwPushed(vdCtor + kVdCtorImageCtor)
                               : 0;
    const bool imageOk = image && MatchPatternAt(image, kPwImageCtorBody) &&
                         PwVtableIs(PsOperand(image + kPwImageCtorVt), "RedBitmapElement", kPwImageDeleteBody);
    const uint32_t tex = (init && PsCodeByte(init + kVdInitTexCall) == 0xE8) ? CallTarget(init + kVdInitTexCall) : 0;
    const bool texOk = tex && MatchPatternAt(tex, kPwTexBody);
    const uint32_t size = (init && PsCodeByte(init + kVdInitSizeCall) == 0xE8) ? CallTarget(init + kVdInitSizeCall) : 0;
    const uint32_t rect = (size && MatchPatternAt(size, kPwSizeBody) && PsCodeByte(size + kPwSizeRectCall) == 0xE8)
                              ? CallTarget(size + kPwSizeRectCall)
                              : 0;
    const bool rectOk = rect && MatchPatternAt(rect, kPwRectBody);
    const uint32_t uv = (show && PsCodeByte(show + kVdShowUvCall) == 0xE8) ? CallTarget(show + kVdShowUvCall) : 0;
    const bool uvOk = uv && MatchPatternAt(uv, kPwUvBody);
    const bool newOk = make && PsCodeByte(make + 5) == 0xE8;
    if (newOk && groupOk && imageOk && texOk && rectOk && uvOk)
    {
        g_pw.gameNew = reinterpret_cast<PwNew_t>(Rva(CallTarget(make + 5)));
        g_pw.groupCtor = reinterpret_cast<PwCtor_t>(Rva(group));
        g_pw.imageCtor = reinterpret_cast<PwCtor_t>(Rva(image));
        g_pw.tex = reinterpret_cast<PwTex_t>(Rva(tex));
        g_pw.rect = reinterpret_cast<PwRect_t>(Rva(rect));
        g_pw.uv = reinterpret_cast<PwUv_t>(Rva(uv));
        g_pw.ok = true;
    }
    Log("[panels] 21j-r9: [Splitscreen] SpawnPanels34=1 - the console windows on the 3 / 4-player spawn point and class screens: "
        "new rva 0x%08X %s, RedGroupElement ctor rva 0x%08X %s (from 0x%08X), RedBitmapElement ctor rva 0x%08X %s, texture rva 0x%08X %s, "
        "SetRect rva 0x%08X %s, UVs rva 0x%08X %s - %s",
        newOk ? CallTarget(make + 5) : 0u, newOk ? "OK" : "NOT found", group, groupOk ? "OK" : "does NOT check out", screenGroup, image,
        imageOk ? "OK" : "does NOT check out", tex, texOk ? "OK" : "does NOT match", rect, rectOk ? "OK" : "does NOT match", uv,
        uvOk ? "OK" : "does NOT match",
        g_pw.ok ? "ON (the bars copied from the game's own; [panels] lines as they are made)" : "OFF: the texts as before (21j-r8)");
}

// at start (after FlowFind): what players 3 and 4's screens need
void F34Find()
{
    g_f34[0].k = 0;
    g_f34[1].k = 1;
    HeapFind(); // 20f-r3
    const bool calls = g_setSpawnChoice && g_setReady && g_getSm && g_validPost && g_nextPost;
    const bool heaps = g_setHeap && g_curHeapVa && g_runtimeHeapVa && g_heapTableVa;
    g_f34Ok = g_f34Wanted && calls && g_textsOk && g_addToRoot && g_rootTable && g_sdGlobal && heaps;
    if (g_f34Ok && g_pauseWanted)
        F34GameFind(); // 20g-r7
    if (g_f34Ok && g_pwWanted)
        PwFind(); // 21j-r9
    else
        Log("[panels] 21j-r9: no console windows on the 3 / 4-player spawn screens (%s) - the texts as before",
            g_pwWanted ? "players 3 / 4's screens are off" : "[Splitscreen] SpawnPanels34=0");
    Log("[p2spawn] 20f: PLAYERS 3 AND 4'S SPAWN SCREENS %s: the Spawn button's calls and NextSpawnPoint %s, texts %s, AddToRoot %s, the "
        "HUD roots %s, the spawn screen %s, the heaps %s - in their own groups in their views' HUD roots, made and deleted with the "
        "runtime heap current, pads 3 and 4; [p34spawn] lines%s",
        g_f34Ok ? "ON" : "OFF", calls ? "OK" : "NOT found", g_textsOk ? "OK" : "NOT found", g_addToRoot ? "OK" : "NOT found",
        g_rootTable ? "OK" : "NOT found", g_sdGlobal ? "OK" : "NOT found", heaps ? "OK" : "NOT found",
        g_f34Wanted ? (g_f34Ok ? "" : " - the auto-spawn (AutoSpawnP34) spawns them") : " ([Splitscreen] SpawnP34=0)");
}

// ---- 20g-r10: PLAYERS 1 AND 2'S SIDE SCREENS IN PLAYERS 3 / 4'S LOOK ---------------------------------------------------
// 4-player (MenusLikeP34=1): player 1's side screen was the Xbox's half layout (class_wheel.cpp's soldiers and texts
// in the PC spawn screen's groups, reference units) and player 2's had players 3 / 4's fractions but in
// ifs_readyselect2's Info group - the root's safe area applied (offset, scale 0.88), so it sat smaller and shifted. Now
// both are drawn exactly as players 3 / 4's: the same kind of group (ClassWheelGroupNew, AddToRoot(his HUD root, group,
// 0, 0, 0): anchored at the view's top left, no safe area - the root's 512 x 312 is his quarter) with the same elements
// at the same places and sizes (F34Grid, F34Text, F34Model with F34's values): the blue grid, CHOOSE YOUR SIDE, the two
// sides' soldiers and names, SELECT SIDE. Their own side screens still run the choice (pads, SelectTeam / the flow's A)
// with none of their own elements made (player 1's spawn screen grid hidden while it is up). [side12] lines.
// 20g-r11: player 1's spawn point and class screens too (class_wheel.cpp's ClassWheelP1Look: his screen, side, post
// and class): the side's name, his map in the left (class_wheel.cpp puts it in players 3 / 4's place), "<" / ">", the
// post's name, "2 / 5" and its vehicles, SELECT SPAWNPOINT; the class's name, "<" / ">", his side's classes on a ring
// (class_wheel.cpp's ring 3, players 3 / 4's arrangement and sizes), SELECT CHARACTER - all as F34Make / F34PostFrame /
// F34ClassFrame make them. His console screens still run the choice; their panels, window and "B BACK" are not shown.
Flow34 g_f12[2];    // [0] player 1, [1] player 2 (k = -2 / -1: F34's own lines say "player k + 3")
int g_side12Lines = 0;

bool Side12Budget()
{
    return g_side12Lines++ < 80;
}

// player p (0 / 1) can have his side screen drawn in players 3 / 4's look: MenusLikeP34=1, their screens found, his group
// neither failed nor short of heap nor without a usable HUD root this match (4-player: the callers' P2Quarter /
// class_wheel.cpp's QuarterLook)
bool Side12Will(int p)
{
    if (!g_f34Ok || !g_quarterWanted || p < 0 || p > 1)
        return false;
    const Flow34& f = g_f12[p];
    return !f.groupFailed && !f.lowHeap;
}

// 21j-r9: player 2's spawn point / class screens (4-player) get their console windows from Side12Frame (his own screen keeps
// its grid, map, ring and prompt; none of the texts that stood in for the windows)
bool Side12PwWill()
{
    return PwReady() && P2Quarter() && Side12Will(1) && !g_f12[1].pwFailed;
}

// his HUD root, if it is usable for the look (as F34Place needs it), else -1
int Side12Root(int p)
{
    const int r = PlayerRootIndex(p);
    const uintptr_t root = r >= 0 ? RootAt(r) : 0;
    const int32_t w = root ? static_cast<int32_t>(ReadU32(root)) : 0;
    const int32_t h = root ? static_cast<int32_t>(ReadU32(root + 4)) : 0;
    return (w >= 64 && h >= 64 && w <= 8192 && h <= 8192) ? r : -1;
}

// the manager's top screen is this one (his pause menu over it: not); a manager that cannot be read does not hold it back
bool Side12Top(int mgr, const char* screen)
{
    const uintptr_t m = Manager(mgr);
    if (!m)
        return true;
    char top[64] = "";
    TopName(m, top, sizeof(top));
    return strcmp(top, screen) == 0;
}

void Side12Close(Flow34& f, const char* why)
{
    if (f.screen == kScrNone && !f.shown)
        return;
    F34ElemsDrop(f, true, why);
    if (f.group)
        ClassWheelShowElement(f.group, false);
    f.shown = false;
    f.screen = kScrNone;
    if (Side12Budget())
        Log("[side12] %lu ms: player %d's side screen (players 3 / 4's look) closed (%s)", ScanMsSinceAttach(), f.k + 3, why);
}

// a new match (or none): the old one's group and elements only forgotten (the match teardown deleted them while it was
// loaded), as players 3 / 4's
void Side12NewMatch(Flow34& f, uintptr_t sm, long sdDeletes)
{
    F34ElemsDrop(f, false, "a new match (or none)");
    F34GroupForget(f, "a new match (or none)");
    const int k = f.k;
    const int lines = f.lines;
    f = Flow34(); // (a failure is tried again each match)
    f.k = k;
    f.lines = lines;
    f.sm = sm;
    f.sdDeletes = sdDeletes;
}

// his screen's elements, as F34Make's (side: both players; spawn point / class: player 1's)
void Side12Make(Flow34& f, uintptr_t sm, int root, int scr, int team)
{
    F34ElemsDrop(f, true, "made again");
    if (!F34HeapRoom(f, "his screen"))
        return;
    f.screen = scr;
    f.team = team;
    f.verbose = ++f.shows[scr] <= 2;
    // 21j-r9: player 2's spawn point / class screens: only their console windows here (his own screen has its grid, map, ring
    // and prompt)
    const bool p2Windows = f.k == -1 && scr != kScrSide;
    if (!p2Windows)
        F34Grid(f);
    if (p2Windows)
    {
        if (!F34WindowsMake(f, sm, false, false))
        {
            f.screen = kScrNone; // (Side12Frame gives his flow its texts back)
            return;
        }
    }
    else if ((scr == kScrPost || scr == kScrClass) && F34WindowsMake(f, sm, false, true)) // 21j-r9: player 1's console windows
    {
    }
    else if (scr == kScrSide)
    {
        F34Text(f, L"CHOOSE YOUR SIDE", 0.5f, kF34TitleY, kF34TitleScale, "side title");
        for (int i = 0; i < 2; ++i)
        {
            wchar_t name[48];
            TeamNameW(sm, i + 1, name, 48);
            f.name[i] = F34Text(f, name, kF34SideX[i], kF34SideNameY, kF34NameScale, i == 0 ? "side 1 name" : "side 2 name");
            f.model[i] = F34Model(f, TeamClass(sm, i + 1, 0), kF34SideX[i], kF34SideFeetY, kF34SideSize, i == 0 ? "side 1" : "side 2");
        }
        f.prompt = F34Text(f, L"SELECT SIDE", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    else if (scr == kScrPost) // 20g-r11: as F34Make's spawn point screen with his map (class_wheel.cpp put it in the left)
    {
        wchar_t side[48];
        TeamNameW(sm, team, side, 48);
        F34Text(f, side, 0.5f, kF34TitleY, kF34TitleScale, "side name");
        F34Text(f, L"<", kF34MapArrowX[0], kF34PostY, kF34PostScale, "left arrow");
        F34Text(f, L">", kF34MapArrowX[1], kF34PostY, kF34PostScale, "right arrow");
        f.main = F34Text(f, L"", kF34MapTextX, kF34PostY, kF34PostScale, "post name");
        f.count = F34Text(f, L"", kF34MapTextX, kF34CountY, kF34SmallScale, "post count");
        f.veh = F34Text(f, L"", kF34MapTextX, (kF34CountY + kF34VehY) * 0.5f + 0.03f, kF34MapVehScale, "vehicles");
        f.prompt = F34Text(f, L"SELECT SPAWNPOINT", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    else if (scr == kScrClass) // 20g-r11: as F34Make's class screen (the ring each frame, Side12ClassFrame)
    {
        f.main = F34Text(f, L"", 0.5f, kF34TitleY, kF34TitleScale, "class name");
        F34Text(f, L"<", kF34ArrowX[0], kF34ClassArrowY, kF34PostScale, "left arrow");
        F34Text(f, L">", kF34ArrowX[1], kF34ClassArrowY, kF34PostScale, "right arrow");
        f.prompt = F34Text(f, L"SELECT CHARACTER", 0.5f, kF34PromptY, kF34PromptScale, "prompt");
    }
    f.shownPick = -1;
    f.shownPost = 0;
    f.shownCount = -1;
    f.shownCls = -1;
    f.shownTeam = 0;
    f.vehPost = 0;
    f.postsAt = 0;
    ClassWheelShowElement(f.group, true);
    f.shown = true;
    if (Side12Budget())
        Log("[side12] %lu ms: player %d's %s screen in players 3 / 4's look: %d element(s) in his group 0x%08X in roots[%d]",
            ScanMsSinceAttach(), f.k + 3, F34ScreenName(scr), f.n, static_cast<unsigned>(f.group), root);
}

// 20g-r11: player 1's spawn point screen, each frame: his post's name, "k / n" and vehicles (as F34PostFrame shows his)
void Side12PostFrame(Flow34& f, uintptr_t sm, uintptr_t post)
{
    const DWORD now = GetTickCount();
    if (post == f.shownPost && f.shownCount >= 0 && now - f.postsAt < 500)
        return;
    f.postsAt = now;
    uintptr_t posts[kMaxPosts];
    const int n = TeamPosts(sm, f.team, posts, kMaxPosts);
    int k = -1;
    for (int i = 0; i < n; ++i)
        if (posts[i] == post)
            k = i;
    if (post != f.shownPost || n != f.shownCount)
    {
        f.shownPost = post;
        f.shownCount = n;
        wchar_t name[96];
        wchar_t cnt[32];
        if (n && post)
        {
            PostNameW(post, name, 96);
            if (k >= 0)
                swprintf(cnt, 32, L"%d / %d", k + 1, n);
            else
                cnt[0] = 0;
        }
        else
        {
            wcscpy_s(name, 96, L"NO SPAWN POINT");
            cnt[0] = 0;
        }
        ClassWheelSetText(f.main, name);
        ClassWheelSetText(f.count, cnt);
        if (f.verbose && Side12Budget())
        {
            char a8[200];
            Log("[side12] %lu ms: player %d: spawn point %d of %d \"%s\" (0x%08X)", ScanMsSinceAttach(), f.k + 3, k + 1, n,
                Narrow(name, a8, sizeof(a8)), static_cast<unsigned>(post));
        }
    }
    if (f.veh && post && post != f.vehPost)
    {
        f.vehPost = post;
        wchar_t veh[256];
        ClassWheelSetText(f.veh, ClassWheelVehiclesAt(post, veh, 256) ? veh : L"");
    }
}

// 20g-r11: player 1's class screen, each frame: the class's name and his side's classes on ring 3 (as F34ClassFrame's)
void Side12ClassFrame(Flow34& f, uintptr_t sm, int cls)
{
    const int cycle = TeamCycle(sm, f.team);
    if (cls < 0 || cls >= cycle)
        cls = 0;
    if (cls != f.shownCls)
    {
        f.shownCls = cls;
        const uintptr_t c = TeamClass(sm, f.team, cls);
        wchar_t name[96];
        wchar_t box[600];
        int weapons = 0;
        if (ClassTextsW(c, name, 96, box, 600, &weapons))
        {
            ClassWheelSetText(f.main, name);
            ClassWheelSetText(f.box, box); // 21j-r9: the class window's box (0 without the windows)
        }
    }
    if (!g_soldiersWanted || cycle <= 0 || F34Ring(f) < 0) // 21j-r9: (player 2's ring is his own screen's)
        return;
    uintptr_t classes[8] = {};
    const int n = cycle < 8 ? cycle : 8;
    for (int i = 0; i < n; ++i)
        classes[i] = TeamClass(sm, f.team, i);
    float cx = 0.0f;
    float cy = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    if (!F34Place(f, F34RingCx(f), kF34RingY, &cx, &cy) || !F34Place(f, 1.0f, 1.0f, &w, &h))
        return;
    const int sel = cls < n ? cls : 0;
    const float radius = F34RingR(f) * w; // 21j-r9: left of the class window while it is up
    const int placed = ClassWheelRingFrame(F34Ring(f), f.group, classes, n, sel, cx, cy, radius, kF34RingSize, 0.0f, cx, cy, true);
    if (!f.ringShown && Side12Budget())
        Log("[side12] %lu ms: player 1: class ring (ring %d): %d of %d class(es) placed, centre at %.0f %.0f (%.2f %.2f of his quarter), "
            "radius %.0f, size %.2f - droidekas at their slots as soldiers (20g-r11)",
            ScanMsSinceAttach(), F34Ring(f), placed, n, cx, cy, F34RingCx(f), kF34RingY, radius, kF34RingSize);
    f.ringShown = true;
}

// player 2's side screen gives the look back: the flow makes r9's texts on it (made again next frame)
void Side12FlowBack(const char* why)
{
    // 21j-r9: his spawn point / class screen's windows too (they could not be made: r9's texts this match - pwFailed)
    if (g_flow.pwLook)
    {
        g_flow.pwLook = false;
        g_flow.pwBack = true; // (his flow drops what it made for them and makes r9's texts, in its own frame)
        g_f12[1].pwFailed = true;
        if (Side12Budget())
            Log("[side12] %lu ms: player 2's console windows NOT drawn (%s) - r9's texts on his screens this match", ScanMsSinceAttach(), why);
    }
    if (!g_flow.sideLook)
        return;
    g_flow.sideLook = false;
    if (g_flow.screen == kScrSide)
    {
        g_flow.textsFor = kScrNone;
        g_flow.textTries = 0;
    }
    if (Side12Budget())
        Log("[side12] %lu ms: player 2's side screen NOT in players 3 / 4's look (%s) - r9's texts on it", ScanMsSinceAttach(), why);
}

// every frame (P2SpawnP34Frame: game thread, the runtime heap current)
void Side12Frame(uintptr_t sm, long deletes)
{
    ClassWheelP1LookAllow(Side12Will(0));
    for (int p = 0; p < 2; ++p)
    {
        Flow34& f = g_f12[p];
        f.k = p - 2;
        if (sm != f.sm || deletes != f.sdDeletes)
            Side12NewMatch(f, sm, deletes);
        int pick = 0;
        int scr = kScrNone;
        int team = 0;
        uintptr_t post = 0;
        int cls = 0;
        if (Side12Will(p) && sm)
        {
            if (p == 0)
            {
                // 20g-r11: his side, spawn point or class screen (the console flow's 1 / 3 / 2), on top of his manager
                int flow = 0;
                if (ClassWheelP1Look(&flow, &team, &post, &cls, &pick))
                {
                    static const char* const kP1Screens[4] = { nullptr, "ifs_readyselect1", "ifs_mapselect1", "ifs_charselect1" };
                    const int s2 = flow == 1 ? kScrSide : (flow == 3 ? kScrPost : (flow == 2 ? kScrClass : kScrNone));
                    if (s2 != kScrNone && (s2 == kScrSide || team == 1 || team == 2) && Side12Top(0, kP1Screens[s2]))
                        scr = s2;
                }
            }
            else
            {
                // (his flow's state of this match - it runs in the menu layer, after this frame hook - and his side
                // screen on top: not under his pause menu)
                if (g_flowOwns && g_flow.sm == sm && g_flow.sdDeletes == deletes && g_flow.matchReady && g_flow.screen == kScrSide &&
                    g_flow.sideLook && Side12Top(1, kFlowScreens[kScrSide]))
                    scr = kScrSide;
                // 21j-r9: his spawn point / class screen's console windows (the rest of those screens is his flow's)
                else if (g_flowOwns && g_flow.sm == sm && g_flow.sdDeletes == deletes && g_flow.matchReady &&
                         (g_flow.screen == kScrPost || g_flow.screen == kScrClass) && g_flow.pwLook && g_flow.textsFor == g_flow.screen &&
                         (g_flow.team == 1 || g_flow.team == 2) && Side12Top(1, kFlowScreens[g_flow.screen]))
                {
                    scr = g_flow.screen;
                    team = g_flow.team;
                    post = g_flow.post;
                    cls = g_flow.cls;
                }
                pick = g_flow.pick;
            }
        }
        const bool up = scr != kScrNone;
        if (p == 1 && (g_flow.sideLook || g_flow.pwLook) && (!Side12Will(1) || !P2Quarter())) // (not 4-player any more, or his group failed)
            Side12FlowBack("not drawn in players 3 / 4's look any more");
        if (!up)
        {
            Side12Close(f, "his side screen is down");
            continue;
        }
        const int root = Side12Root(p);
        if (root < 0)
        {
            Side12Close(f, "no usable HUD root for him");
            f.groupFailed = true; // (this match: his own side screen as r9)
            if (Side12Budget())
                Log("[side12] %lu ms: player %d: no usable HUD root (roots[%d]) - his own side screen as r9 this match",
                    ScanMsSinceAttach(), p + 1, PlayerRootIndex(p));
            if (p == 1)
                Side12FlowBack("no HUD root");
            continue;
        }
        if (!F34GroupMake(f, root)) // (it sets groupFailed / lowHeap: his own side screen as r9 this match)
        {
            Side12Close(f, "no group");
            if (Side12Budget())
                Log("[side12] %lu ms: player %d: no group in his HUD root (roots[%d]) - his own side screen as r9 this match",
                    ScanMsSinceAttach(), p + 1, root);
            if (p == 1)
                Side12FlowBack("no group");
            continue;
        }
        if (f.screen != scr || (scr != kScrSide && team != f.team))
            Side12Make(f, sm, root, scr, team);
        if (f.screen != scr) // (no room in the runtime heap: F34HeapRoom set lowHeap; 21j-r9: or player 2's windows not made)
        {
            if (p == 1)
                Side12FlowBack(f.pwFailed ? "his console windows could not be made" : "no room in the runtime heap");
            continue;
        }
        if (scr == kScrPost)
            Side12PostFrame(f, sm, post);
        else if (scr == kScrClass)
            Side12ClassFrame(f, sm, cls);
        else if (pick != f.shownPick)
        {
            f.shownPick = pick;
            ClassWheelTextGrey(f.name[0], pick != 0);
            ClassWheelTextGrey(f.name[1], pick != 1);
            ClassWheelModelGrey(f.model[0], pick != 0);
            ClassWheelModelGrey(f.model[1], pick != 1);
        }
    }
}

// the match teardown (while it is still loaded, the runtime heap current): their groups deleted, as players 3 / 4's
void Side12Teardown()
{
    for (int p = 0; p < 2; ++p)
    {
        Flow34& f = g_f12[p];
        if (!f.group && !f.n)
            continue;
        if (f.group && F34GroupLive(f))
        {
            F34ElemsDrop(f, true, "the match teardown");
            F34GroupDrop(f, "the match teardown");
        }
        else
        {
            F34ElemsDrop(f, false, "the match teardown");
            F34GroupForget(f, "the match teardown");
        }
        f.screen = kScrNone;
        f.shown = false;
    }
}

// 21h-r7: the match end scan, each frame (game thread)
bool EndBudget()
{
    if (g_endLines >= kEndMaxLines)
        return false;
    if (++g_endLines == kEndMaxLines)
    {
        Log("[end34] line limit reached (%d), the match end scan goes quiet", kEndMaxLines);
        return false;
    }
    return true;
}

int EndCount()
{
    const uintptr_t h = g_vdHolder ? ReadU32(g_vdHolder) : 0;
    return h ? static_cast<int>(ReadU32(h + g_vdCountOfs)) : -1;
}

bool EndOver()
{
    return g_endFixWanted && g_endOver != 0;
}

// 21h-r9: his group and image in his view's root (the runtime heap current - his flow's frame); false if they could not be made
bool VdP34Make(int k, int root)
{
    VdP34& v = g_vdp[k];
    if (v.group)
        return v.root == root;
    if (!g_vdi.ok || root < 0 || root > 3)
        return false;
    void* g = g_vdi.gameNew(0xB0);
    void* e = g ? g_vdi.gameNew(0x100) : nullptr;
    if (!g || !e)
    {
        if (EndBudget())
            Log("[end34] %lu ms: player %d's VICTORY / DEFEAT image NOT made (no memory) - the text instead", ScanMsSinceAttach(), k + 3);
        return false; // (the one made stays unused: a few bytes until the heap goes)
    }
    g_vdi.groupCtor(g, nullptr);
    g_vdi.imageCtor(e, nullptr);
    g_vdi.rootAdd(root, g, 0.5f, 0.5f, 1);
    g_vdi.vis(e, nullptr, 0);
    g_vdi.groupAdd(g, nullptr, e);
    g_vdi.tex(e, nullptr, g_vdi.texName);
    g_vdi.size(e, nullptr, static_cast<float>(PsI32(g_vdi.sizeW)), static_cast<float>(PsI32(g_vdi.sizeH)), 1, 1);
    v.group = reinterpret_cast<uintptr_t>(g);
    v.elem = reinterpret_cast<uintptr_t>(e);
    v.root = root;
    v.shown = false;
    if (EndBudget())
        Log("[end34] %lu ms: player %d's VICTORY / DEFEAT image made as players 1 / 2's: group 0x%08X in roots[%d] at 0.50 0.50, image "
            "0x%08X \"%s\" %ld x %ld", ScanMsSinceAttach(), k + 3, static_cast<unsigned>(v.group), root, static_cast<unsigned>(v.elem),
            g_vdi.texName, static_cast<long>(PsI32(g_vdi.sizeW)), static_cast<long>(PsI32(g_vdi.sizeH)));
    return true;
}

// shown with the colour and the half of the texture the game's Show picks (win: [1], lose: [0]) / hidden
void VdP34Show(int k, int win, bool on)
{
    VdP34& v = g_vdp[k];
    if (!v.elem)
        return;
    void* e = reinterpret_cast<void*>(v.elem);
    if (on)
    {
        const uintptr_t i = win ? 1u : 0u;
        const float* uv = reinterpret_cast<const float*>(g_vdi.uvs + i * 16);
        g_vdi.color(e, nullptr, ReadU32(g_vdi.colors + i * 4));
        g_vdi.uv(e, nullptr, uv[0], uv[1], uv[2], uv[3], 0);
    }
    g_vdi.vis(e, nullptr, on ? 1 : 0);
    v.shown = on;
}

// out of his root and deleted (the match teardown, the runtime heap current)
void VdP34Drop(int k, const char* why)
{
    VdP34& v = g_vdp[k];
    if (!v.group)
        return;
    void* g = reinterpret_cast<void*>(v.group);
    void* e = reinterpret_cast<void*>(v.elem);
    g_vdi.rootRemove(v.root, g);
    g_vdi.groupClear(g, nullptr);
    reinterpret_cast<VdDelete_t>(ReadU32(ReadU32(v.elem)))(e, nullptr, 1);
    reinterpret_cast<VdDelete_t>(ReadU32(ReadU32(v.group)))(g, nullptr, 1);
    if (EndBudget())
        Log("[end34] %lu ms: player %d's VICTORY / DEFEAT image out of roots[%d] and deleted (%s)", ScanMsSinceAttach(), k + 3, v.root, why);
    v = VdP34();
}

// 21h-r9: ifs_pausemenu's viewport back (its pause over the stats went / the match went)
void StatsPauseRestore(const char* why)
{
    if (!g_statsPaused)
        return;
    g_statsPaused = false;
    if (g_statsRec)
        *reinterpret_cast<volatile int32_t*>(g_statsRec + kRecViewport) = g_statsVpWas;
    if (g_statsLines++ < 40)
        Log("[end34] %lu ms: player 1's pause menu over the stats went (%s) - ifs_pausemenu's viewport back to %ld", ScanMsSinceAttach(), why,
            static_cast<long>(g_statsVpWas));
    g_statsRec = 0;
}

// 21h-r11: Start on player 1's pad (the game's own pad object: its XInput buttons at +C4h, START = 0010h) and Esc (only
// while the game's window is in front) - read every frame, a new press counts
void StatsStartKeys(bool& pad, bool& esc)
{
    pad = false;
    esc = false;
    const uintptr_t gp = GamePadAddress();
    if (gp && (PadButtons(reinterpret_cast<const void*>(gp)) & 0x0010) != 0)
        pad = true;
    const HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (fg)
        GetWindowThreadProcessId(fg, &pid);
    if (pid == GetCurrentProcessId() && (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
        esc = true;
}

// each frame: Start on the stats screen (the game's PauseState +21h stats mode) -> player 1's pause menu (SetPaused) over the
// stats, drawn over the whole screen with them. 21h-r11: the r10 test (1 player) showed +22h is never set at the end: the
// game's PauseState Update only looks at Start while its game-over flag (GOG 1.3 0x008A4D86, set by the mission update when
// a side wins, read at 0x001E6BCA) is 0 - so after a win it never gets there. Now BF1GameExt reads Start / Esc itself.
void StatsPauseFrame()
{
    if (g_statsPauseWanted <= 0 || !g_psStatePtr || !g_psSetPausedFn || !g_psFindScreen)
        return;
    // 21h-r11: 1 = only real 3 / 4-player matches (PlayerNLocalPlayers is the match's count only in split screen)
    if (g_statsPauseWanted == 1 && (PlayerNLocalPlayers() < 3 || LocalPlayers() < 2))
        return;
    const uintptr_t ps = ReadU32(g_psStatePtr); // PauseState[0]: player 1's
    if (!ps)
        return;
    const bool stats = ReadU8(ps + 0x21) != 0;
    if (stats != g_statsSeen)
    {
        g_statsSeen = stats;
        if (g_statsLines++ < 40)
            Log("[end34] %lu ms: player 1's stats mode %s (PauseState +21h)%s", ScanMsSinceAttach(), stats ? "ON" : "off",
                stats ? " - Start there opens his pause menu" : "");
    }
    // 21h-r14: +22h is NOT "Start pressed" - it is "leave the stats" (the game's ScriptCB_QuitFromStats sets it for every player
    // in the stats mode; GOG 1.3 0x001E6750 waits for all of them, then the next mission or the shell). r9 - r13 read it as Start
    // and cleared it: after r12's QUIT the stats mode (+21h) was still on in the shell, so it paused the shell (black screen).
    // Now it is never read or written here, and the menu only opens while a stats screen is the top of player 1's manager.
    const bool paused = ReadU8(ps + kPsStatePaused) != 0;
    if (g_statsPaused && !paused)
        StatsPauseRestore("the menu closed");
    bool padNow = false;
    bool escNow = false;
    StatsStartKeys(padNow, escNow);
    const bool padPress = padNow && !g_statsPadWas;
    const bool escPress = escNow && !g_statsEscWas;
    g_statsPadWas = padNow;
    g_statsEscWas = escNow;
    if (!stats || (!padPress && !escPress) || paused)
        return;
    {
        char top[64];
        TopName(Manager(0), top, sizeof(top));
        if (strcmp(top, "ifs_teamstats") != 0 && strcmp(top, "ifs_awardstats") != 0 && strcmp(top, "ifs_personalstats") != 0)
        {
            if (g_statsLines++ < 40)
                Log("[end34] %lu ms: Start / Esc with the stats mode on, but player 1's top screen is \"%s\" (not a stats screen) - no "
                    "pause menu", ScanMsSinceAttach(), top);
            return;
        }
    }
    const char* const how = padPress ? "Start on his pad" : "Esc";
    const uintptr_t rec = g_psFindScreen("ifs_pausemenu");
    if (rec)
    {
        g_statsRec = rec;
        g_statsVpWas = PsI32(rec + kRecViewport);
        *reinterpret_cast<volatile int32_t*>(rec + kRecViewport) = -1; // the interface root: over the whole screen with the stats
    }
    g_statsPaused = true;
    PsSetPausedWrap(reinterpret_cast<void*>(ps), nullptr, 1);
    if (g_statsLines++ < 40)
        Log("[end34] %lu ms: %s on the stats screen - player 1's pause menu over it (SetPaused; ifs_pausemenu's viewport %ld -> -1 while "
            "it is up there; %lu local player(s)): QUIT goes to the main menu, RESUME GAME back to the stats", ScanMsSinceAttach(), how,
            static_cast<long>(g_statsVpWas), static_cast<unsigned long>(LocalPlayers()));
}

// 21h-r8: the game's Show (thiscall: the winners' mask, the seconds) - its own code first, then the mask kept
using VdShow_t = void(__fastcall*)(void*, void*, uint32_t, float);
void __fastcall VdShowWrap(void* self, void* edx, uint32_t mask, float secs)
{
    reinterpret_cast<VdShow_t>(g_vdShowFn)(self, edx, mask, secs);
    if (!mask)
        return;
    g_endMask = mask;
    InterlockedExchange(&g_endMaskKnown, 1);
    if (!g_endOver)
    {
        InterlockedExchange(&g_endOver, 1);
        g_endAt = GetTickCount();
    }
    if (EndBudget())
        Log("[end34] %lu ms: the game's victory / defeat Show: winners mask %08X for %.1f s - team 1 %s, team 2 %s (a set bit = DEFEAT)",
            ScanMsSinceAttach(), mask, secs, (mask >> 1) & 1 ? "DEFEAT" : "VICTORY", (mask >> 2) & 1 ? "DEFEAT" : "VICTORY");
}

// a player's result from Show's mask (1 VICTORY, 0 DEFEAT, -1 not known)
int EndResult(uintptr_t ch)
{
    if (!g_endMaskKnown || !ch)
        return -1;
    const int32_t team = static_cast<int32_t>(ReadU32(ch + kCharTeam));
    if (team < 0 || team > 31)
        return -1;
    return ((g_endMask >> team) & 1) ? 0 : 1;
}

// 21j-r16: THE MOUSE CURSOR IS PLAYER 1'S ([Splitscreen] CursorP1Only). The game has one mouse cursor (the IF cursor element,
// CursorShow - found at PopScreen +3Bh): every PushScreen (and a popup) shows it, PopScreen hides it only when that manager's
// menu stack runs empty. The menu layer draws it in player 1's box while his menus are up, else where the last view left the
// viewport - player 2's half (with 3 / 4 players the last quarter). r15's video: player 2 paused on his spawn screen; the pop of
// his pause menu left that screen on his stack, so the cursor stayed on, and BF1GameExt's own spawn screen switches keep it as
// they find it (CursorPutBack) - player 1's mouse cursor stood in the corner of player 2's half through his spawn screens and his
// play. Each frame of a split screen match: shown while player 1 has no menu (nor popup) up and player 2's pause menu is not on
// -> hidden (the game's own CursorShow(0), what its PopScreen does when the last screen goes). Player 1's menus and player 2's
// pause menu keep it as the game has it. Game thread (the menu layer hook).
void CursorFrame()
{
    if (!g_cursorP1Wanted || !g_cursorShow || CursorShown() != 1 || LocalPlayers() < 2 || !SpawnScanManager())
        return;
    const uintptr_t m0 = Manager(0);
    if (!m0 || ReadU32(m0 + kMgrDepth) > 0 || (g_popupOk && ReadU8(m0 + kMgrPopup) != 0))
        return; // player 1's menus are up: the cursor is his
    if (g_pauseObjs && GamePlayerPaused(1))
        return; // player 2's pause menu is on: as the game has it
    g_cursorShow(0);
    if (g_cursorLines < 16)
    {
        ++g_cursorLines;
        char top1[64];
        const uintptr_t m1 = Manager(1);
        TopName(m1, top1, sizeof(top1));
        Log("[p2spawn] %lu ms: 21j-r16: the mouse cursor hidden - it was on with no menu of player 1's up (player 2's manager: top "
            "\"%s\", depth %lu; %lu local player(s)) - it would have stood over the last view",
            ScanMsSinceAttach(), top1, static_cast<unsigned long>(m1 ? ReadU32(m1 + kMgrDepth) : 0),
            static_cast<unsigned long>(LocalPlayers()));
    }
}

} // namespace

// 20g-r12: horizontal 2-player - the HUD roots' geometry and places in their groups, for class_wheel.cpp
bool P2SpawnRootGeom(int p, float* w, float* h, float* offX, float* offY, float* sx, float* sy)
{
    const int r = PlayerRootIndex(p);
    const uintptr_t root = r >= 0 && r < kHudRoots ? RootAt(r) : 0;
    float rw = 0.0f;
    float rh = 0.0f;
    if (!w || !h || !offX || !offY || !sx || !sy || !RootUsable(root, &rw, &rh))
        return false;
    const float ox = ReadF32(root + 8);
    const float oy = ReadF32(root + 0x0C);
    const float kx = ReadF32(root + 0x10);
    const float ky = ReadF32(root + 0x14);
    if (!(ox >= -1.0f && ox <= 1.0f && oy >= -1.0f && oy <= 1.0f && kx > 0.1f && kx <= 2.0f && ky > 0.1f && ky <= 2.0f))
        return false;
    *w = rw;
    *h = rh;
    *offX = ox;
    *offY = oy;
    *sx = kx;
    *sy = ky;
    return true;
}

bool P2SpawnGroupPlace(uintptr_t group, float fx, float fy, float* x, float* y)
{
    return x && y && GroupFracPlace(group, fx, fy, x, y);
}

void P2SpawnHorizontalRoots()
{
    HzRootsFrame();
}

bool P2SpawnRootsHorizontal()
{
    for (int p = 0; p < 2; ++p)
    {
        const int r = PlayerRootIndex(p);
        const uintptr_t root = r >= 0 && r < kHudRoots ? RootAt(r) : 0;
        float w = 0.0f;
        float h = 0.0f;
        if (!RootUsable(root, &w, &h) || w < 2.0f * h)
            return false;
    }
    return PlayerRootIndex(0) != PlayerRootIndex(1);
}

int P2SpawnManagerIndex(uintptr_t manager)
{
    const uintptr_t base = g_mgrBaseAddr ? ReadU32(g_mgrBaseAddr) : 0;
    if (!base || !manager)
        return -1;
    if (manager == base)
        return 0;
    if (manager == base + kManagerSize)
        return 1;
    return -1;
}

// 21h-r3: whose box the menu layer (the IF root: popups, option screens) goes in - 1 player 2's, 0 player 1's, -1 as before
// (spawn_ui.cpp: player 1's box while his manager has a screen). Only in 3 / 4-player matches ([Splitscreen] MenuBoxP2).
// 21j-r15: and the 2-player split ([Splitscreen] PauseLikeP1P2): 2 = player 2's (his own half's 16:9 box), by the same rules.
int P2SpawnMenuOwner()
{
    int owner = -1;
    const char* why = "";
    const bool two = g_p2LikeWanted && P2Two();                         // 21j-r15
    const bool p34 = g_menuBoxP2Wanted && PlayerNLocalPlayers() >= 3;
    if ((p34 || two) && g_pauseObjs && g_mgrBaseAddr)
    {
        const bool p1 = GamePlayerPaused(0);
        const bool p2 = GamePlayerPaused(1);
        if (p2 && !p1)
        {
            owner = 1;
            why = "player 2's pause is on";
        }
        else if (p2 && p1)
        {
            const uintptr_t base = ReadU32(g_mgrBaseAddr);
            auto busy = [](uintptr_t m) -> bool {
                return m && (ReadU32(m + kMgrDepth) > 1 || (g_popupOk && ReadU8(m + kMgrPopup) != 0));
            };
            const bool b0 = busy(base);
            const bool b1 = busy(base ? base + kManagerSize : 0);
            if (b1 && !b0)
            {
                owner = 1;
                why = "both paused - player 2's manager has the popup / page";
            }
            else if (b0 && !b1)
            {
                owner = 0;
                why = "both paused - player 1's manager has the popup / page";
            }
            else
            {
                owner = (g_psViewportVar && PsI32(g_psViewportVar) == 1) ? 1 : 0;
                why = "both paused - the pausing viewport";
            }
        }
        if (owner == 1 && two)
            owner = 2; // 21j-r15: the 2-player split - his own half
    }
    if (owner != g_ownerWas)
    {
        g_ownerWas = owner;
        if (g_ownerLines < 40)
        {
            ++g_ownerLines;
            Log("[rp34] %lu ms: 21h-r3: the menu layer (popups, option screens) %s%s%s", ScanMsSinceAttach(),
                owner == 2   ? "goes in PLAYER 2'S own half (21j-r15: a 16:9 box in it, as player 1's in his)"
                : owner == 1 ? "goes in PLAYER 2'S quarter"
                : owner == 0 ? "goes in player 1's box"
                             : "as before (player 1's box while his manager has a screen)",
                why[0] ? " - " : "", why);
        }
    }
    return owner;
}

// 21j-r15: the 3-player layout - player 1's menu layer in his top left quarter ([Splitscreen] MenuBoxP1)
bool P2SpawnMenuP1Quarter()
{
    return g_menuBoxP1Wanted && PlayerNLocalPlayers() == 3;
}

// 21j-r15: the match is over (VICTORY / DEFEAT shown, until the next match starts) - players34.cpp's end shots
bool P2SpawnMatchEnded()
{
    return EndOver();
}

// 21j-r15: player 2's pause menu as player 1's in the 2-player split - menus.cpp's script
bool P2SpawnP2PauseLike()
{
    return g_p2LikeWanted;
}

void P2SpawnMenuLayer(uintptr_t root)
{
    if (!g_gt.frame)
        Log("[p2spawn] %lu ms: the menu layer hook is running (game thread) - player 2's spawn screens %s", ScanMsSinceAttach(),
            g_flowOk ? "are live" : "are OFF (see the start lines)");
    ++g_gt.frame;
    ControllerAudioFrame(); // 21j-r27: the audio scanner (menus.cpp, log only)
    HeldMove();       // 18u: the menu input fix, if it was not applied at start (not on the game thread then)
    CountMove();      // 18v: the interface's player count, likewise
    HeldWatch();      // 18u: player 2's held buttons, when they change
    HzRootsFrame();   // 20g-r12: horizontal - both halves' HUD roots with Classic's safe area (before his screens place anything)
    FlowFrame();      // 18u: player 2's spawn screens on manager 1
    CursorFrame();    // 21j-r16: the mouse cursor only while player 1's menus (or player 2's pause menu) are up
    ScheduledDumps(); // 18t: a screen's record and root, 30 frames after it comes up
    Ps34Frame();      // 21h-r1: the pause scan's dumps that wait a few frames
    ViewportLine(root);
}

bool P2SpawnFlowOwnsSpawn()
{
    if (EndOver())
        return true; // 21h-r8: nobody spawns after the end
    return g_flowOwns != 0;
}

// 21i-r5: the end of a match is over (a new match / none): every player's end state cleared (factored out of EndScanFrame)
void EndNewMatch(const char* why)
{
    if (g_endOver && EndBudget())
        Log("[end34] %lu ms: the end of the match is over (%s)", ScanMsSinceAttach(), why);
    InterlockedExchange(&g_endOver, 0);
    InterlockedExchange(&g_endMaskKnown, 0);
    g_gameOverSeen = false;
    StatsPauseRestore(why); // 21h-r9
    for (int k = 0; k < 2; ++k)
        if (g_vdp[k].group)
        {
            // (the teardown did not take it out: its heap went with the match - forgotten, never touched again)
            if (EndBudget())
                Log("[end34] %lu ms: player %d's VICTORY / DEFEAT image forgotten (the old match's)", ScanMsSinceAttach(), k + 3);
            g_vdp[k] = VdP34();
        }
    for (int k = 0; k < 2; ++k)
        if (g_f34[k].endDone)
        {
            g_f34[k].endDone = false;
            PlayerNMenuInput(k, false);
        }
}

// 20f-r2: players 3 and 4's spawn screens (players34.cpp: game thread, each frame before the views are drawn)
void EndScanFrame()
{
    StatsPauseFrame(); // 21h-r9
    ControllerBotFrame(); // 21i-r4 (log only)
    if ((!g_endScanWanted && !g_endFixWanted) || !g_vdGlobal)
        return;
    const uintptr_t obj = ReadU32(g_vdGlobal);
    const DWORD now = GetTickCount();
    if (obj != g_vdObj)
    {
        if (obj)
        {
            g_vdSince = now;
            const int n = EndCount();
            if (EndBudget())
                Log("[end34] %lu ms: THE VICTORY / DEFEAT DISPLAY MADE at 0x%08X (vtable rva 0x%08X): its player count %d (it has room "
                    "for 2: arrays of 2 at +20h / +180h)%s; local players %d",
                    ScanMsSinceAttach(), static_cast<unsigned>(obj), RvaOf(ReadU32(obj)), n,
                    n > 2 ? " - MORE THAN 2: its constructor wrote past its arrays" : "", static_cast<int>(LocalPlayers()));
        }
        else if (EndBudget())
            Log("[end34] %lu ms: the victory / defeat display went (it was up %lu ms)", ScanMsSinceAttach(),
                static_cast<unsigned long>(now - g_vdSince));
        g_vdObj = obj;
        g_vdState[0] = 0;
        // 21h-r8: a new match (or none): the end is over
        EndNewMatch(obj ? "a new match" : "the match is gone");
    }
    // 21i-r5: the game's game-over flag back to 0 (the next mission started - the display may sit at the same address)
    if (g_gameOverFlag && g_endOver)
    {
        if (ReadU8(g_gameOverFlag))
            g_gameOverSeen = true;
        else if (g_gameOverSeen)
            EndNewMatch("the game's game-over flag back to 0: a new match");
    }
    // 21h-r8: switched on (also without Show's thunk): the match is over (21i-r5: and the game says so, when its flag is known -
    // the old display's memory can still read as on in the shell)
    if (obj && ReadU8(obj + 0x4A0) && !g_endOver && (!g_gameOverFlag || ReadU8(g_gameOverFlag)))
    {
        InterlockedExchange(&g_endOver, 1);
        g_endAt = now;
        if (EndBudget())
            Log("[end34] %lu ms: THE MATCH IS OVER (the victory / defeat display is on)%s", ScanMsSinceAttach(),
                g_endFixWanted ? " - every player's spawn screens / pause menus close, players 3 / 4 get theirs" : " (EndMatch34=0: nothing done)");
    }
    if (!obj || now - g_vdLastLog < 250)
        return;
    g_vdLastLog = now;
    char top0[64];
    char top1[64];
    char top2[64];
    char top3[64];
    TopName(Manager(0), top0, sizeof(top0));
    TopName(Manager(1), top1, sizeof(top1));
    TopName(g_rpMgr[0], top2, sizeof(top2));
    TopName(g_rpMgr[1], top3, sizeof(top3));
    char st[400];
    int units[4] = { -1, -1, -1, -1 };
    for (int p = 0; p < 4; ++p)
    {
        const uintptr_t ch = p < 2 ? SpawnScanCharacter(p) : CharOfPlayer(p);
        units[p] = ch ? (ReadU32(ch + kCharUnit) ? 1 : 0) : -1;
    }
    snprintf(st, sizeof(st),
             "flags +4A0 %u +4A4 %lu +4A8 %u | +3A0 %u +3E0 %u +420 %u +460 %u | managers: 1 \"%s\" 2 \"%s\" 3 \"%s\" 4 \"%s\" | "
             "spawn screens: player 2 %s, player 3 %s, player 4 %s | soldiers %d %d %d %d | count %d",
             ReadU8(obj + 0x4A0), static_cast<unsigned long>(ReadU32(obj + 0x4A4)), ReadU8(obj + 0x4A8), ReadU8(obj + 0x3A0),
             ReadU8(obj + 0x3E0), ReadU8(obj + 0x420), ReadU8(obj + 0x460), top0, top1, top2, top3,
             g_flow.screen >= 0 && g_flow.screen < 4 && kFlowScreens[g_flow.screen] ? kFlowScreens[g_flow.screen] : "-",
             F34ScreenName(g_f34[0].screen), F34ScreenName(g_f34[1].screen), units[0], units[1], units[2], units[3], EndCount());
    if (strcmp(st, g_vdState) != 0)
    {
        snprintf(g_vdState, sizeof(g_vdState), "%s", st);
        if (EndBudget())
            Log("[end34] %lu ms (+%lu ms): %s", ScanMsSinceAttach(), static_cast<unsigned long>(now - g_vdSince), st);
    }
}

void P2SpawnP34Frame()
{
    EndScanFrame(); // 21h-r7 (log only)
    if (!g_f34Ok)
        return;
    HzRootsFrame(); // 20g-r13: player 3's half (3-player) with Classic's safe area before his screens place anything (his map)
    // 20f-r3: everything below makes and deletes its elements with the game's runtime heap current (the heap that was
    // current is put back at the end)
    const int rt = HeapIndexAt(g_runtimeHeapVa);
    if (rt < 0)
        return; // (the game's heaps are not set up yet)
    g_heapAtEntry = HeapIndexAt(g_curHeapVa);
    if (g_heapAtEntry != rt)
    {
        ++g_heapSwitches;
        if (g_heapLines < 8 || g_heapSwitches % 1000 == 0)
        {
            ++g_heapLines;
            char a[160];
            Log("[p34spawn] %lu ms: our frame began with heap %d current, not the runtime heap %d (%s) - switched to the runtime heap "
                "for it (%ld such frame(s))",
                ScanMsSinceAttach(), g_heapAtEntry, rt, HeapText(g_heapAtEntry, a, sizeof(a)), g_heapSwitches);
        }
    }
    const int was = g_setHeap(rt);
    const uintptr_t sm = SpawnScanManager();
    bool known = false;
    const long deletes = ClassWheelSpawnScreenDeletes(&known);
    const int players = PlayerNPadPlayers(); // 3 / 4 while pads 3 and 4 are on, else 2
    for (int k = 0; k < 2; ++k)
    {
        Flow34& f = g_f34[k];
        if (k + 3 > players)
        {
            f.owns = false;
            if (f.screen != kScrNone || f.shown)
                F34Close(f, "pads 3 and 4 are off");
            continue;
        }
        F34FrameOne(f, sm, deletes);
    }
    F34WorldPauseHold(); // 20g-r7
    RpIdleCheck();       // 21h-r2: our managers rest once a popped real pause menu is gone
    Side12Frame(sm, deletes); // 20g-r10: players 1 and 2's side screens in players 3 / 4's look
    g_setHeap(was);
}

// 20g-r5: the match teardown (players34.cpp, the first HUD display destroyed): players 3 and 4's screens, ring and
// group deleted while their match is still loaded, with the runtime heap current (as our frame makes them). The next
// match finds nothing to delete.
void P2SpawnP34Teardown()
{
    if (!g_gameOverFlag && g_endOver)
        EndNewMatch("the match teardown"); // 21i-r5: without the game's flag the teardown ends it
    if (!g_f34Ok)
        return;
    if (g_mutedByUs) // 20g-r9: a restart / quit from his pause screen brings the sound back (not left muted)
        F34Mute(false, "the match teardown");
    g_mutedByUs = false;
    const int rt = HeapIndexAt(g_runtimeHeapVa);
    if (rt < 0)
    {
        // 20g-r13: the map layouts lent (1 / 2) put back all the same (their maps not touched)
        for (Flow34& f : g_f34)
            F34MapOff(f, "the match teardown (no runtime heap)", false);
        return;
    }
    const int was = g_setHeap(rt);
    for (int k = 0; k < 2; ++k)
        VdP34Drop(k, "the match teardown"); // 21h-r9
    for (int k = 0; k < 2; ++k)
    {
        Flow34& f = g_f34[k];
        F34MapOff(f, "the match teardown", true); // 20g-r6: before players34.cpp deletes his map
        PlayerNSetPaused(k, false);
        f.real = false;        // 21h-r2: his real pause menu goes with the Lua state (our managers are reset with the game's)
        g_rpReq[k] = 0;
        f.worldPaused = false; // 20g-r7: the game's own mission change looks after its pause byte (not held any more)
        f.confirm = 0;
        g_worldByteOurs = false;
        if (!f.group && !f.n && !f.ringShown)
            continue;
        if (f.group && F34GroupLive(f))
        {
            F34ElemsDrop(f, true, "the match teardown");
            F34GroupDrop(f, "the match teardown");
        }
        else
        {
            F34ElemsDrop(f, false, "the match teardown");
            F34GroupForget(f, "the match teardown");
        }
        f.screen = kScrNone;
        f.shown = false;
        f.owns = false;
    }
    Side12Teardown(); // 20g-r10: players 1 and 2's side screen groups (players 3 / 4's look)
    g_setHeap(was);
}

// 20g-r6: players 3 and 4's maps on their spawn point screens (players34.cpp's HudFrame, after their HUD)
void P2SpawnP34MapFrame()
{
    if (!g_f34Ok)
        return;
    if (!g_f34[0].mapOn && !g_f34[1].mapOn && !(g_f34[0].shown && g_f34[0].screen == kScrPost) &&
        !(g_f34[1].shown && g_f34[1].screen == kScrPost))
        return; // (nothing to do: no heap switch)
    // with the runtime heap current, as their screens (SetFlag resizes the map's picture)
    const int rt = HeapIndexAt(g_runtimeHeapVa);
    if (rt < 0)
        return;
    const int was = g_setHeap(rt);
    for (int k = 0; k < 2; ++k)
        F34MapFrameOne(g_f34[k]);
    g_setHeap(was);
}

bool P2SpawnP34Owns(int k)
{
    if (EndOver() && k >= 0 && k < 2)
        return true; // 21h-r8: nobody spawns after the end
    return g_f34Ok && k >= 0 && k < 2 && g_f34[k].owns;
}

// 21h-r8: the end screens (ifs_teamstats / ifs_awardstats on player 1's manager) are drawn over the whole screen, not in
// his quarter ([Splitscreen] EndStatsFullScreen: 1 = with 3 / 4 players, 2 = with 2 players too)
bool P2SpawnEndStatsFull()
{
    if (g_endFullWanted <= 0)
        return false;
    if (g_endFullWanted == 1 && PlayerNLocalPlayers() < 3)
        return false;
    if (LocalPlayers() < 2)
        return false; // 21j-r15: split screen only (with 2 the 1-player stats would have been boxed too)
    char top[64];
    TopName(Manager(0), top, sizeof(top));
    if (g_statsPaused && strcmp(top, "ifs_pausemenu") == 0)
        return true; // 21h-r9: his pause menu over the stats
    return strcmp(top, "ifs_teamstats") == 0 || strcmp(top, "ifs_awardstats") == 0;
}

// 21i-r1: the map being played - the mission queue's last loaded entry (its name, the entry's first 32 bytes); false if the
// queue was not found or holds none
bool P2SpawnMissionName(char* out, size_t size)
{
    if (!out || size == 0)
        return false;
    out[0] = 0;
    if (!g_mqIndex || !g_mqCount)
        return false;
    const int32_t cnt = PsI32(g_mqCount);
    const int32_t idx = PsI32(g_mqIndex);
    if (cnt < 1 || cnt > 64 || idx < 1 || idx > cnt)
        return false;
    const uintptr_t e = g_mqCount + 0x4C + static_cast<uintptr_t>(idx - 1) * 0x4C; // the entries follow the count by 4Ch
    size_t i = 0;
    for (; i + 1 < size && i < 32; ++i)
    {
        const char c = static_cast<char>(ReadU8(e + i));
        if (!c)
            break;
        out[i] = (c >= 32 && c < 127) ? c : '?';
    }
    out[i] = 0;
    return i > 0;
}

// 21i-r1: the game's own local player count (1 single player, 2 split screen - players 3 / 4 are BF1GameExt's)
int P2SpawnGameLocalPlayers()
{
    return static_cast<int>(LocalPlayers());
}

// 21h-r11: player 1's pause menu on the stats screen is on (StatsPauseP1 > 0)
bool P2SpawnStatsPauseOn()
{
    return g_statsPauseWanted > 0;
}

// 21j-r25: player 1's pause menu, opened the way the game's own Start opens it (PauseState[0]::SetPaused(1) - the game pushes
// ifs_pausemenu, as StatsPauseFrame does over the stats) - for player 1's free camera, where Start did not pause
// (menus.cpp FreeCamPauseFrame). 1 = paused now, 0 = already paused, -1 = not possible (the pause states or SetPaused
// not found - [Splitscreen] PauseScan34=0, or a pattern). Game thread.
int P2SpawnPauseP1()
{
    if (!g_psStatePtr || !g_psSetPausedFn)
        return -1;
    const uintptr_t ps = ReadU32(g_psStatePtr); // PauseState[0]: player 1's
    if (!ps)
        return -1;
    if (ReadU8(ps + kPsStatePaused) != 0)
        return 0;
    PsSetPausedWrap(reinterpret_cast<void*>(ps), nullptr, 1);
    return ReadU8(ps + kPsStatePaused) != 0 ? 1 : -1;
}

// 21h-r2: players 3 / 4's real pause menus are installed (menus.cpp makes their screens only then)
bool P2SpawnRealPauseReady()
{
    return g_rpOn && g_rpWanted;
}

// 21h-r2: what players 3 / 4's real pause menus ask (BF1GameExt_pause34.lua, "BFXQ: rp <what> <player 3 / 4> [i] [v]"; game
// thread, inside our manager's tick): resume / suicide / restart / quit (done in his next frame; 1 = taken), ctlget (a value:
// 0 invert look, 1 invert flying, 2 sensitivity), ctlset, ctlsave, ctllive
// 21j-r15: "rp p2dx / p2dy <anchor * 1000>" - the 2-player split ([Splitscreen] PauseLikeP1P2): how far (his HUD root's units,
// + 1000) player 2's pause menu container, anchored at that fraction of his root, moves to stand where player 1's (anchored the
// same, unmoved) stands in his half - each root draws an anchor at offset + scale x anchor (its safe area). 0 = leave it.
static int P2MenuShift(bool across, long a1000)
{
    if (!g_p2LikeWanted || !P2Two() || a1000 < 0 || a1000 > 1000)
        return 0;
    float w[2] = {};
    float h[2] = {};
    float ox[2] = {};
    float oy[2] = {};
    float sx[2] = {};
    float sy[2] = {};
    for (int p = 0; p < 2; ++p)
        if (!P2SpawnRootGeom(p, &w[p], &h[p], &ox[p], &oy[p], &sx[p], &sy[p]))
            return 0;
    const float a = static_cast<float>(a1000) / 1000.0f;
    const float d = across ? (ox[0] + sx[0] * a) - (ox[1] + sx[1] * a) : (oy[0] + sy[0] * a) - (oy[1] + sy[1] * a);
    const float units = d * (across ? w[1] : h[1]);
    if (!(units > -400.0f && units < 400.0f))
        return 0;
    const int r = static_cast<int>(units >= 0.0f ? units + 0.5f : units - 0.5f);
    int& was = g_p2ShiftWas[across ? 0 : 1];
    if (r != was && g_p2LikeLines < 40)
    {
        ++g_p2LikeLines;
        Log("[p2spawn] %lu ms: 21j-r15: player 2's pause menu %s by %d unit(s) of his root (%.0f x %.0f) - his safe area %s %.3f / %.3f, "
            "player 1's %.3f / %.3f (anchor %.3f): it stands where player 1's stands in his half",
            ScanMsSinceAttach(), across ? "across" : "down", r, static_cast<double>(w[1]), static_cast<double>(h[1]),
            across ? "across" : "down", static_cast<double>(across ? ox[1] : oy[1]), static_cast<double>(across ? sx[1] : sy[1]),
            static_cast<double>(across ? ox[0] : oy[0]), static_cast<double>(across ? sx[0] : sy[0]), static_cast<double>(a));
    }
    was = r;
    return 1000 + r;
}

int P2SpawnRealPauseAsk(const char* what)
{
    char cmd[16] = {};
    size_t n = 0;
    while (what[n] && what[n] != ' ' && n + 1 < sizeof(cmd))
    {
        cmd[n] = what[n];
        ++n;
    }
    long v[3] = { -1, -1, -1 };
    const char* q = what + n;
    for (int i = 0; i < 3; ++i)
    {
        char* end = nullptr;
        const long x = strtol(q, &end, 10);
        if (end == q)
            break;
        v[i] = x;
        q = end;
    }
    // 21h-r4: "rp ctlfn <player 1 - 4> <mode> <button>" - the page's function number from that player's map
    if (strcmp(cmd, "ctlfn") == 0)
        return (v[0] >= 1 && v[0] <= 4) ? CmFn(CmInput(static_cast<int>(v[0]) - 1), static_cast<int>(v[1]), static_cast<int>(v[2]))
                                        : -1;
    // 21h-r10: "rp statspause" - 1 while player 1's pause menu is up over the stats (ifs_pausemenu then has no RESPAWN)
    if (strcmp(cmd, "statspause") == 0)
        return g_statsPaused ? 1 : 0;
    // 21j-r15: "rp p2dx / p2dy <anchor * 1000>" - player 2's pause menu where player 1's stands (2-player split)
    if (strcmp(cmd, "p2dx") == 0 || strcmp(cmd, "p2dy") == 0)
        return P2MenuShift(cmd[3] == 'x', v[0]);
    // 21h-r12: "rp statsquit" - QUIT (YES) in that menu: the mission queue says "no next mission" (its index past its count, no
    // loop), so leaving the stats (the script then unpauses and calls the game's ScriptCB_QuitFromStats) goes to the main menu
    if (strcmp(cmd, "statsquit") == 0)
    {
        if (!g_statsPaused)
            return 0;
        if (!g_mqIndex)
            return -1;
        const int32_t cnt = PsI32(g_mqCount);
        const int32_t was = PsI32(g_mqIndex);
        const uint8_t loopWas = ReadU8(g_mqLoop);
        if (cnt >= 0 && cnt <= 64)
            *reinterpret_cast<volatile int32_t*>(g_mqIndex) = cnt;
        *reinterpret_cast<volatile uint8_t*>(g_mqLoop) = 0;
        if (g_statsLines++ < 40)
            Log("[end34] %lu ms: QUIT on the stats screen - the mission queue: %ld mission(s), next was %ld (loop %u) -> %ld, no loop: "
                "leaving the stats goes to the main menu", ScanMsSinceAttach(), static_cast<long>(cnt), static_cast<long>(was),
                static_cast<unsigned>(loopWas), static_cast<long>(cnt));
        return (cnt >= 0 && cnt <= 64) ? 1 : -1;
    }
    // 21h-r13: "rp statsrestart" - RESTART MISSION (YES) in that menu: the map just played is the queue's last loaded one (its
    // index - 1: the game's "load the next one" loads [index] and then adds 1), so the index goes back by one and leaving the
    // stats loads the same map again (the game's own RestartMission does nothing after a win, as its QuitToShell)
    if (strcmp(cmd, "statsrestart") == 0)
    {
        if (!g_statsPaused)
            return 0;
        if (!g_mqIndex)
            return -1;
        const int32_t cnt = PsI32(g_mqCount);
        const int32_t was = PsI32(g_mqIndex);
        if (cnt < 1 || cnt > 64 || was < 1 || was > cnt)
        {
            if (g_statsLines++ < 40)
                Log("[end34] %lu ms: RESTART MISSION on the stats screen - the mission queue (%ld mission(s), next %ld) does not say which "
                    "map was played: the game's own RESTART", ScanMsSinceAttach(), static_cast<long>(cnt), static_cast<long>(was));
            return -1;
        }
        *reinterpret_cast<volatile int32_t*>(g_mqIndex) = was - 1;
        if (g_statsLines++ < 40)
            Log("[end34] %lu ms: RESTART MISSION on the stats screen - the mission queue: %ld mission(s), next was %ld -> %ld (the map just "
                "played): leaving the stats loads it again", ScanMsSinceAttach(), static_cast<long>(cnt), static_cast<long>(was),
                static_cast<long>(was - 1));
        return 1;
    }
    const int k = (v[0] == 3 || v[0] == 4) ? static_cast<int>(v[0]) - 3 : -1;
    if (k < 0)
        return -1;
    static const char* const kActs[] = { "resume", "suicide", "restart", "quit" };
    for (int a = 0; a < 4; ++a)
    {
        if (strcmp(cmd, kActs[a]) != 0)
            continue;
        if (!g_f34[k].real)
            return 0;
        InterlockedExchange(&g_rpReq[k], a + 1);
        if (RpBudget())
            Log("[rp34] %lu ms: player %d's pause menu: %s", ScanMsSinceAttach(), k + 3, a == 0 ? "RESUME GAME" : a == 1 ? "RESPAWN - yes" : a == 2 ? "RESTART MISSION - yes" : "QUIT - yes");
        return 1;
    }
    if (strcmp(cmd, "ctllive") == 0)
        return PlayerNControlsLive() ? 1 : 0;
    // 21h-r6: "rp down <3 / 4>" - 1 while his pause menu is over his spawn screens (no RESPAWN then)
    if (strcmp(cmd, "down") == 0)
        return g_f34[k].pauseFrom != kScrNone ? 1 : 0;
    // 21h-r6: "rp remap <3 / 4>" - 1 if his buttons can be changed here
    if (strcmp(cmd, "remap") == 0)
        return (g_remapWanted && g_cmBase && g_cmBtn && g_cmFn && CmInput(k + 2)) ? 1 : 0;
    // 21h-r6: "rp ctlfnset <3 / 4> <mode> <button> <function>" - his button changed (and kept: ControlMapP3 / P4)
    if (strcmp(cmd, "ctlfnset") == 0)
    {
        const int m = static_cast<int>(v[1]);
        const int b = static_cast<int>(v[2]);
        long fnl = -1;
        {
            char* end = nullptr;
            const long x = strtol(q, &end, 10);
            if (end != q)
                fnl = x;
        }
        const int fn = static_cast<int>(fnl);
        if (!g_remapWanted || !CmPickable(b) || m < 0 || m >= kCmModes || fn < 0 || fn >= kCmFns)
            return 0;
        const uintptr_t input = CmInput(k + 2);
        const int was = CmFn(input, m, b);
        if (!CmSet(input, m, b, fn))
            return 0;
        g_cmWant[k][m][b] = fn;
        g_cmDirty[k] = true;
        g_cmApplied[k] = input;
        if (RpBudget())
            Log("[ctlmap] %lu ms: player %d: tab %d, button %d: function %d -> %d (his own PlayerInput 0x%08X)", ScanMsSinceAttach(),
                k + 3, m, b, was, fn, static_cast<unsigned>(input));
        return 1;
    }
    // 21h-r6: "rp ctlfnreset <3 / 4> <mode>" - a tab's buttons back to the game's default
    if (strcmp(cmd, "ctlfnreset") == 0)
    {
        const int m = static_cast<int>(v[1]);
        if (!g_remapWanted || m < 0 || m >= kCmModes)
            return 0;
        const uintptr_t input = CmInput(k + 2);
        int n2 = 0;
        for (int b : kCmPick)
            if (CmSet(input, m, b, CmDefault(b)))
            {
                g_cmWant[k][m][b] = -1;
                ++n2;
            }
        g_cmDirty[k] = true;
        if (RpBudget())
            Log("[ctlmap] %lu ms: player %d: tab %d back to the game's default buttons (%d set)", ScanMsSinceAttach(), k + 3, m, n2);
        return n2 ? 1 : 0;
    }
    PlayerNControls c = PlayerNControlsGet(k);
    const int i = static_cast<int>(v[1]);
    if (strcmp(cmd, "ctlget") == 0)
        return i == 0 ? c.invert : i == 1 ? c.invertFly : i == 2 ? c.sens : -1;
    if (strcmp(cmd, "ctlset") == 0 && i >= 0 && i <= 2)
    {
        const int x = static_cast<int>(v[2]);
        if (i == 0)
            c.invert = x ? 1 : 0;
        else if (i == 1)
            c.invertFly = x ? 1 : 0;
        else
            c.sens = x;
        PlayerNControlsSet(k, c);
        g_f34[k].optsDirty = true;
        if (RpBudget())
            Log("[rp34] %lu ms: player %d's CONTROLLER page: %s = %d", ScanMsSinceAttach(), k + 3,
                i == 0 ? "invert look" : i == 1 ? "invert flying" : "sensitivity", x);
        return 1;
    }
    if (strcmp(cmd, "ctlsave") == 0)
    {
        const bool saved = !g_f34[k].optsDirty || PlayerNControlsSave(k);
        g_f34[k].optsDirty = false;
        const bool mapSaved = CmSave(k); // 21h-r6
        return (saved && mapSaved) ? 1 : 0;
    }
    return -1;
}

int P2SpawnP34Root(int k)
{
    if (!g_f34Ok || k < 0 || k > 1)
        return -1;
    const Flow34& f = g_f34[k];
    if (f.real) // 21h-r2: his real pause menu is in his view's root (its viewport): his HUD hidden, the root drawn
        return PlayerNSpecView(k);
    return (f.shown && F34GroupLive(f)) ? f.root : -1;
}

// 20g-r10: player p's spawn screens (p 1 = player 2, 2 / 3 = players 3 / 4): 1 while his side, spawn point or class
// screen is up (from the frame his respawn timer ran out, as player 1's), 0 while not, -1 when BF1GameExt's screens do
// not look after him (off, his pad not connected, not found) - players34.cpp's camera shots
int P2SpawnScreensUp(int p)
{
    if (EndOver() && p >= 1 && p <= 3)
        return 1; // 21h-r8: the match is over - their views on player 1's camera shot
    if (p == 1)
    {
        if (!g_flowOk || !g_flowOwns)
            return -1;
        return g_flow.screen != kScrNone ? 1 : 0;
    }
    const int k = p - 2;
    if (!g_f34Ok || k < 0 || k > 1 || !g_f34[k].owns)
        return -1;
    const int s = g_f34[k].screen;
    if (s == kScrPause34 && g_f34[k].pauseFrom != kScrNone) // 21h-r6: his pause menu over his spawn screens
        return 1;
    return (s == kScrSide || s == kScrPost || s == kScrClass) ? 1 : 0;
}

// 19i: for spawn_ui.cpp (player 1's console screens, the chooser's switch made at once)
bool P2SpawnSettleReady()
{
    return g_managerTick != nullptr && g_switchNowWanted;
}

bool P2SpawnSettleManager(uintptr_t manager)
{
    const bool done = FlowSettle(manager);
    if (done && g_settleLines++ < 6)
    {
        char top[64];
        TopName(manager, top, sizeof(top));
        Log("[p2spawn] %lu ms: manager %d settled now (its screen events run): top \"%s\", depth %lu", ScanMsSinceAttach(),
            P2SpawnManagerIndex(manager), top, static_cast<unsigned long>(ReadU32(manager + kMgrDepth)));
    }
    return done;
}

void P2SpawnScanStart(bool onGameThread)
{
    (void)onGameThread;
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[p2spawn] not used on this build");
        return;
    }
    Log("[p2spawn] split_spawn_menus.cpp file build %s - PLAYER 2 SPAWN SCREENS + MENU INPUT FIX (no hooks of its own: the IF "
        "input step's 25 references to its held-button table moved; player 2's console screens on manager 1 with the game's "
        "own PushScreen / PopScreen)",
        kFileBuild);
    {
        char exe[MAX_PATH] = {};
        char ini[MAX_PATH + 32] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
                *slash = 0;
            snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
            g_heldWanted = GetPrivateProfileIntA("Splitscreen", "P2MenuInputFix", 1, ini) != 0;
            g_flowWanted = GetPrivateProfileIntA("Splitscreen", "P2SpawnScreen", 1, ini) != 0;
            g_mapWanted = GetPrivateProfileIntA("Splitscreen", "P2SpawnMap", 1, ini) != 0;
            g_soldiersWanted = GetPrivateProfileIntA("Splitscreen", "P2SpawnSoldiers", 1, ini) != 0;
            g_respawnWanted = GetPrivateProfileIntA("Splitscreen", "P2RespawnDies", 1, ini) != 0;
            g_backdropWanted = GetPrivateProfileIntA("Splitscreen", "P2SpawnBackdrop", 1, ini) != 0;
            g_switchNowWanted = GetPrivateProfileIntA("Splitscreen", "SpawnScreenSwitchNow", 1, ini) != 0; // 19i
            g_f34Wanted = GetPrivateProfileIntA("Splitscreen", "SpawnP34", 1, ini) != 0; // 20f-r2
            g_waitP1 = GetPrivateProfileIntA("Splitscreen", "SpawnWaitP1", 0, ini) != 0;          // 20g-r6
            g_f34MapWanted = GetPrivateProfileIntA("Splitscreen", "SpawnMapP34", 1, ini) != 0;    // 20g-r6
            g_pauseWanted = GetPrivateProfileIntA("Splitscreen", "PauseP34", 1, ini) != 0;        // 20g-r6
            g_worldPauseWanted = GetPrivateProfileIntA("Splitscreen", "PauseWorldP34", 1, ini) != 0; // 20g-r7
            g_quarterWanted = GetPrivateProfileIntA("Splitscreen", "MenusLikeP34", 1, ini) != 0;     // 20g-r7
            g_muteWanted = GetPrivateProfileIntA("Splitscreen", "PauseMuteP34", 1, ini) != 0;        // 20g-r9
            g_pauseLookWanted = GetPrivateProfileIntA("Splitscreen", "PauseLookP34", 1, ini) != 0;   // 20g-r9
            g_hzSafeWanted = GetPrivateProfileIntA("Splitscreen", "HorizontalSafeArea", 1, ini) != 0; // 20g-r12
            g_hzMenusWanted = GetPrivateProfileIntA("Splitscreen", "MenusHorizontal", 1, ini) != 0;   // 20g-r13
            g_ps34Wanted = GetPrivateProfileIntA("Splitscreen", "PauseScan34", 1, ini) != 0;         // 21h-r1
            g_rpWanted = GetPrivateProfileIntA("Splitscreen", "RealPauseP34", 1, ini) != 0;          // 21h-r2
            g_sndRunWanted = GetPrivateProfileIntA("Splitscreen", "PauseSoundP34", 1, ini) != 0;     // 21h-r3
            g_menuBoxP2Wanted = GetPrivateProfileIntA("Splitscreen", "MenuBoxP2", 1, ini) != 0;      // 21h-r3
            g_p2LikeWanted = GetPrivateProfileIntA("Splitscreen", "PauseLikeP1P2", 1, ini) != 0;     // 21j-r15
            g_menuBoxP1Wanted = GetPrivateProfileIntA("Splitscreen", "MenuBoxP1", 1, ini) != 0;      // 21j-r15
            g_cursorP1Wanted = GetPrivateProfileIntA("Splitscreen", "CursorP1Only", 1, ini) != 0;    // 21j-r16
            g_spawnPauseWanted = GetPrivateProfileIntA("Splitscreen", "SpawnPauseP34", 1, ini) != 0; // 21h-r6
            g_remapWanted = GetPrivateProfileIntA("Splitscreen", "RemapP34", 1, ini) != 0;           // 21h-r6
            g_endScanWanted = GetPrivateProfileIntA("Splitscreen", "EndScan34", 1, ini) != 0;        // 21h-r7
            g_endFixWanted = GetPrivateProfileIntA("Splitscreen", "EndMatch34", 1, ini) != 0;        // 21h-r8
            g_endFullWanted = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "EndStatsFullScreen", 2, ini)); // 21h-r8 (21j-r15: 2)
            g_statsPauseWanted = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "StatsPauseP1", 2, ini)); // 21h-r9 (r11: 2)
            g_pwWanted = GetPrivateProfileIntA("Splitscreen", "SpawnPanels34", 1, ini) != 0;       // 21j-r9
            snprintf(g_cmIni, sizeof(g_cmIni), "%s", ini);
        }
        Log("[p2spawn] BF1GameExt.ini [Splitscreen] P2MenuInputFix=%d, P2SpawnScreen=%d, P2SpawnMap=%d, P2SpawnSoldiers=%d, "
            "P2RespawnDies=%d, P2SpawnBackdrop=%d, SpawnScreenSwitchNow=%d, SpawnP34=%d, SpawnWaitP1=%d (%s), SpawnMapP34=%d, "
            "PauseP34=%d, PauseWorldP34=%d, MenusLikeP34=%d, PauseMuteP34=%d, PauseLookP34=%d",
            g_heldWanted ? 1 : 0, g_flowWanted ? 1 : 0, g_mapWanted ? 1 : 0, g_soldiersWanted ? 1 : 0, g_respawnWanted ? 1 : 0,
            g_backdropWanted ? 1 : 0, g_switchNowWanted ? 1 : 0, g_f34Wanted ? 1 : 0, g_waitP1 ? 1 : 0,
            g_waitP1 ? "players 2-4 spawn only while player 1 is in the world" : "players 2-4 spawn without waiting for player 1",
            g_f34MapWanted ? 1 : 0, g_pauseWanted ? 1 : 0, g_worldPauseWanted ? 1 : 0, g_quarterWanted ? 1 : 0,
            g_muteWanted ? 1 : 0, g_pauseLookWanted ? 1 : 0);
        Log("[p2spawn] 21h-r6: [Splitscreen] SpawnPauseP34=%d (%s), RemapP34=%d (%s)", g_spawnPauseWanted ? 1 : 0,
            g_spawnPauseWanted ? "Start on players 3 / 4's side, spawn point and class screens opens their pause menu"
                               : "players 3 / 4 pause only in the world",
            g_remapWanted ? 1 : 0,
            g_remapWanted ? "players 3 / 4 change their buttons on their gamepad page (ControlMapP3 / P4)" : "their buttons as the game's");
        CmLoad(); // 21h-r6
        Log("[p2spawn] 21h-r9 / r11: [Splitscreen] StatsPauseP1=%d (%s; r11: Start on player 1's pad or Esc, read by BF1GameExt)", g_statsPauseWanted,
            g_statsPauseWanted == 1 ? "3 / 4-player matches: Start on the stats screen opens player 1's pause menu (QUIT to the main menu)"
            : g_statsPauseWanted >= 2 ? "every match - 1, 2, 3 or 4 players: Start on the stats screen opens player 1's pause menu, no RESPAWN"
                                      : "off");
        Log("[p2spawn] 20g-r12 / r13: [Splitscreen] MenusHorizontal=%d (20g-r13: player 3's screens in his 3-player half at player 2's "
            "horizontal places), HorizontalSafeArea=%d (%s)", g_hzMenusWanted ? 1 : 0, g_hzSafeWanted ? 1 : 0,
            g_hzSafeWanted ? "horizontal 2-player with MenusHorizontal=1: both halves' HUD roots get the Classic Collection's safe area "
                             "- 10% at the top and the bottom of each half"
                           : "the game's safe area: player 1's half 10% top / 2% bottom, player 2's 2% / 10%");
    }
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = lo + build->sizeOfImage;
    uint32_t hits[4] = {};

    // 1. the IF managers
    int n = FindPattern(kGetIfManager, hits, 4);
    if (n == 1)
    {
        const uintptr_t base = ReadU32(Rva(hits[0] + kGetIfManagerBase));
        if (base >= lo && base < hi)
            g_mgrBaseAddr = base;
        Log("[p2spawn] GetIFManager(player) rva 0x%08X: managers at [rva 0x%08X] + player * 244h%s", hits[0], RvaOf(base),
            g_mgrBaseAddr ? "  OK" : "  (base outside the exe - managers NOT read)");
    }
    else
        Log("[p2spawn] GetIFManager(player): %d pattern hits (need exactly 1) - managers NOT read", n);
    n = FindPattern(kLuaManager, hits, 4);
    if (n == 1)
    {
        const uintptr_t idx = ReadU32(Rva(hits[0] + kLuaManagerIndex));
        const uintptr_t base = ReadU32(Rva(hits[0] + kLuaManagerBase));
        if (idx >= lo && idx < hi)
            g_luaIndexAddr = idx;
        Log("[p2spawn] the Lua's manager getter rva 0x%08X: current manager index at rva 0x%08X, base at rva 0x%08X%s", hits[0],
            RvaOf(idx), RvaOf(base), base == g_mgrBaseAddr ? " (= GetIFManager's)  OK" : " (differs from GetIFManager's!)");
    }
    else
        Log("[p2spawn] the Lua's manager getter: %d pattern hits (need exactly 1)", n);
    // 18u: the menu input fix - found and checked now, applied on the game thread
    HeldFind();
    if (g_heldReady && !g_heldWanted)
    {
        g_heldReady = false;
        Log("[p2spawn] MENU INPUT FIX OFF: [Splitscreen] P2MenuInputFix=0 (found and checked, not applied)");
    }
    else if (g_heldReady && onGameThread)
        HeldMove();
    else if (g_heldReady)
        Log("[p2spawn] menu input fix: applied on the first menu layer frame (game thread)");
    n = FindPattern(kIfUpdate, hits, 4);
    if (n == 1)
    {
        const bool playerOk = MatchPatternAt(hits[0] + kIfUpdatePlayerAt, kIfUpdatePlayer);
        const bool inputOk = MatchPatternAt(hits[0] + kIfUpdateInputAt, kIfUpdateInput);
        Log("[p2spawn] IF update rva 0x%08X: both managers every frame; the input player read from manager +18Ch at +6Ah %s, "
            "GetPlayerInput (vfunc 20h) at +7Ch %s",
            hits[0], playerOk ? "OK" : "does NOT match", inputOk ? "OK" : "does NOT match");
        // 18v: its player count (the interface's) - routed through BF1GameExt with the held-button table
        if (g_heldWanted)
        {
            CountFind(hits[0]);
            if (onGameThread)
                CountMove();
        }
        // 19i: the manager's tick (the second loop's call at +E7h), byte-checked: our own PushScreen /
        // PopScreen are settled with it in the same frame
        {
            const bool siteOk = MatchPatternAt(hits[0] + kIfUpdateTickAt, kIfUpdateTickSite);
            const uint32_t tick = siteOk ? CallTarget(hits[0] + kIfUpdateTickAt) : 0;
            const bool bodyOk = tick && MatchPatternAt(tick, kManagerTickBody);
            if (bodyOk && g_switchNowWanted)
                g_managerTick = reinterpret_cast<ManagerTick_t>(Rva(tick));
            Log("[p2spawn] IF update +E7h: the manager tick call %s -> rva 0x%08X %s%s", siteOk ? "OK" : "does NOT match", tick,
                bodyOk ? "OK (queues its update event, drains the manager's screen events)" : "does NOT match",
                g_managerTick ? " - a screen switch of ours is settled in the frame it is made"
                              : (bodyOk ? " - NOT used ([Splitscreen] SpawnScreenSwitchNow=0): the IF update switches" : ""));
        }
    }
    else
        Log("[p2spawn] IF update: %d pattern hits (need exactly 1)", n);

    // 2. the spawn screen: its global, GetSpawnDisplay(player), the maker's GetIFManager(0)
    n = FindPattern(kMakeDisplay, hits, 4);
    const uint32_t make = n == 1 ? hits[0] : 0;
    if (make)
    {
        const uintptr_t global = ReadU32(Rva(make + kMakeGlobal));
        if (global >= lo && global < hi)
            g_sdGlobal = global;
        const bool mgrSiteOk = MatchPatternAt(make + kMakeMgrAt, kMakeMgr);
        const uint32_t mgrFn = mgrSiteOk ? CallTarget(make + kMakeMgrAt + 2) : 0;
        const uintptr_t chooserGlobal = mgrSiteOk ? ReadU32(Rva(make + kMakeMgrGlobal)) : 0;
        if (chooserGlobal >= lo && chooserGlobal < hi)
            g_chooserMgrAddr = chooserGlobal;
        Log("[p2spawn] spawn screen maker rva 0x%08X: pointer at rva 0x%08X; its \"push 0; call GetIFManager; mov [chooser "
            "global], eax\" at +44h %s (GetIFManager rva 0x%08X %s, chooser global at rva 0x%08X)",
            make, RvaOf(global), mgrSiteOk ? "OK" : "does NOT match", mgrFn,
            (mgrFn && MatchPatternAt(mgrFn, kGetIfManager)) ? "OK" : "NOT GetIFManager", RvaOf(chooserGlobal));
    }
    else
        Log("[p2spawn] spawn screen maker: %d pattern hits (need exactly 1)", n);
    n = FindPattern(kGetSpawnDisplay, hits, 4);
    if (n == 1)
    {
        g_getSpawnDisplayRva = hits[0];
        const uintptr_t global = ReadU32(Rva(hits[0] + kGetSpawnDisplayGlobal));
        // direct call sites (E8 rel32, one jump thunk followed) in the code section
        uint32_t codeRva = 0;
        uint32_t codeSize = 0;
        int sites = 0;
        int thunks = 0;
        if (CodeSection(&codeRva, &codeSize))
        {
            const unsigned char* code = reinterpret_cast<const unsigned char*>(Rva(codeRva));
            // the thunks first (a "jmp GetSpawnDisplay")
            uint32_t thunkRva[8] = {};
            for (uint32_t i = 0; i + 5 <= codeSize && i < 0x40000; ++i)
            {
                if (code[i] != 0xE9)
                    continue;
                int32_t rel = 0;
                memcpy(&rel, code + i + 1, 4);
                if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) == hits[0] && thunks < 8)
                    thunkRva[thunks++] = codeRva + i;
            }
            for (uint32_t i = 0; i + 5 <= codeSize; ++i)
            {
                if (code[i] != 0xE8)
                    continue;
                int32_t rel = 0;
                memcpy(&rel, code + i + 1, 4);
                const uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel);
                bool hit = t == hits[0];
                for (int k = 0; k < thunks && !hit; ++k)
                    hit = t == thunkRva[k];
                if (hit)
                    ++sites;
            }
        }
        Log("[p2spawn] GetSpawnDisplay(player) rva 0x%08X: ignores its player, returns [rva 0x%08X]%s; %d call site(s) in the "
            "code (%d jump thunk(s)) - GOG 1.3 has 24",
            hits[0], RvaOf(global), global == g_sdGlobal ? " (= the maker's global)  OK" : " (NOT the maker's global!)", sites,
            thunks);
    }
    else
        Log("[p2spawn] GetSpawnDisplay(player): %d pattern hits (need exactly 1)", n);

    // 3. the chooser's player-1 sites
    n = FindPattern(kChooser, hits, 4);
    bool chooserHooked = false;
    if (n == 0)
    {
        // 18t: 13a's logging jump is at its entry by now (spawn_ui.cpp starts first)
        n = FindPattern(kChooserHooked, hits, 4);
        chooserHooked = n == 1;
    }
    if (n == 1)
    {
        g_chooserRva = hits[0];
        const uint32_t c = hits[0];
        int reads = 0;
        for (uint32_t off : kChooserMgrReads)
        {
            const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(c + off));
            if (p[0] == 0x8B && p[1] == 0x0D && ReadU32(Rva(c + off + 2)) == g_chooserMgrAddr)
                ++reads;
        }
        const unsigned char* p1 = reinterpret_cast<const unsigned char*>(Rva(c + kChooserPush1));
        const bool push1 = p1[0] == 0x6A && p1[1] == 0x01;
        char ready[32] = "";
        char pc[32] = "";
        char pause[32] = "";
        const unsigned char* pf = reinterpret_cast<const unsigned char*>(Rva(c + kChooserReadyFmt));
        const unsigned char* pp = reinterpret_cast<const unsigned char*>(Rva(c + kChooserPcName));
        const unsigned char* pm = reinterpret_cast<const unsigned char*>(Rva(c + kChooserPauseName));
        if (pf[0] == 0x68)
            ReadText(ReadU32(Rva(c + kChooserReadyFmt + 1)), ready, sizeof(ready));
        if (pp[0] == 0x68)
            ReadText(ReadU32(Rva(c + kChooserPcName + 1)), pc, sizeof(pc));
        if (pm[0] == 0xB9)
            ReadText(ReadU32(Rva(c + kChooserPauseName + 1)), pause, sizeof(pause));
        const uint32_t calls[3] = { kChooserMapCall, kChooserDispBCall, kChooserDispCCall };
        uintptr_t arrays[3] = {};
        uint32_t fns[3] = {};
        for (int i = 0; i < 3; ++i)
        {
            const unsigned char* pc2 = reinterpret_cast<const unsigned char*>(Rva(c + calls[i]));
            if (pc2[0] != 0xE8)
                continue;
            fns[i] = CallTarget(c + calls[i]);
            if (MatchPatternAt(fns[i], kArrayGetter))
            {
                const uintptr_t arr = ReadU32(Rva(fns[i] + kArrayGetterImm));
                if (arr >= lo && arr < hi)
                    arrays[i] = arr;
            }
        }
        g_mapArray = arrays[0];
        g_dispBArray = arrays[1];
        g_dispCArray = arrays[2];
        Log("[p2spawn] chooser rva 0x%08X%s: %d of 5 manager reads are the chooser global, \"push 1\" at +38h %s, format "
            "\"%s\" at +3Dh, \"%s\" at +143h, \"%s\" at +17Fh",
            c, chooserHooked ? " (found with 13a's logging jump at its entry)" : "", reads, push1 ? "OK" : "NOT FOUND", ready,
            pc, pause);
        Log("[p2spawn] chooser's per-player arrays (from the screen's own player +238Ch): GetMap rva 0x%08X -> maps at rva "
            "0x%08X, display B getter 0x%08X -> rva 0x%08X, display C getter 0x%08X -> rva 0x%08X%s",
            fns[0], RvaOf(arrays[0]), fns[1], RvaOf(arrays[1]), fns[2], RvaOf(arrays[2]),
            (arrays[0] && arrays[1] && arrays[2]) ? "  all OK" : "  (one or more do NOT match)");
    }
    else
        Log("[p2spawn] chooser: %d pattern hits (need exactly 1)", n);

    // 4. the setup's root, the registry
    n = FindPattern(kSetup, hits, 4);
    if (n == 1)
    {
        const uint32_t s = hits[0];
        const unsigned char* p0 = reinterpret_cast<const unsigned char*>(Rva(s + kSetupPush0));
        const bool push0 = p0[0] == 0x6A && p0[1] == 0x00;
        const uint32_t rootsFn = p0[5] == 0xE8 ? CallTarget(s + kSetupRootsCall) : 0; // +12h = 0Dh + 5
        const uint32_t indexFn = *reinterpret_cast<const unsigned char*>(Rva(s + kSetupIndexCall)) == 0xE8 ? CallTarget(s + kSetupIndexCall) : 0;
        const uint32_t addFn = *reinterpret_cast<const unsigned char*>(Rva(s + kSetupAddCall)) == 0xE8 ? CallTarget(s + kSetupAddCall) : 0;
        const bool rootsOk = rootsFn && MatchPatternAt(rootsFn, kGetterImm);
        const bool indexOk = indexFn && MatchPatternAt(indexFn, kRootIndexBody);
        const bool addOk = addFn && MatchPatternAt(addFn, kAddToRootBody);
        const uintptr_t rootsObj = rootsOk ? ReadU32(Rva(rootsFn + 1)) : 0;
        const uintptr_t table = addOk ? ReadU32(Rva(addFn + kAddToRootTable)) : 0;
        if (rootsObj >= lo && rootsObj < hi && table >= lo && table < hi && push0 && indexOk)
        {
            g_rootsObjAddr = rootsObj;
            g_rootTable = table;
            g_addToRoot = reinterpret_cast<AddToRoot_t>(Rva(addFn)); // 20f-r2 (byte-checked above: kAddToRootBody)
        }
        Log("[p2spawn] setup rva 0x%08X: \"push 0\" (the player) at +0Dh %s, GetRootsObject 0x%08X %s (object pointer at rva "
            "0x%08X), RootIndex(player) 0x%08X %s (+6Ch + player*4), AddToRoot 0x%08X %s (root table at rva 0x%08X)%s",
            s, push0 ? "OK" : "NOT FOUND", rootsFn, rootsOk ? "OK" : "does NOT match", RvaOf(rootsObj), indexFn,
            indexOk ? "OK" : "does NOT match", addFn, addOk ? "OK" : "does NOT match", RvaOf(table),
            g_rootTable ? "  all OK" : "  (roots NOT read)");
        // the registry (18b's chain, the first two steps)
        if (*reinterpret_cast<const unsigned char*>(Rva(s + kSetupFindCall)) == 0xE8)
        {
            const uint32_t findByPath = CallTarget(s + kSetupFindCall);
            const uint32_t findByHash = MatchPatternAt(findByPath, kFindByPath) ? CallTarget(findByPath + kFindByHashCall) : 0;
            const uintptr_t reg = (findByHash && MatchPatternAt(findByHash, kFindByHash)) ? ReadU32(Rva(findByHash + kRegistryImm)) : 0;
            if (reg >= lo && reg + 4 + 2 * kRegSlots * 4 <= hi)
                g_registry = reg;
            Log("[p2spawn] registry: FindByPath 0x%08X, FindByHash 0x%08X, registry at rva 0x%08X%s", findByPath, findByHash,
                RvaOf(reg), g_registry ? "  OK" : "  (NOT read)");
        }
    }
    else
        Log("[p2spawn] setup: %d pattern hits (need exactly 1)", n);

    // 5. the screen list, the IF root, the map table
    n = FindPattern(kScreenByHash, hits, 4);
    if (n == 1)
    {
        const uintptr_t list = ReadU32(Rva(hits[0] + kScreenListImm1));
        if (list == ReadU32(Rva(hits[0] + kScreenListImm2)) && list >= lo && list < hi)
            g_screenList = list;
        Log("[p2spawn] screen list: ScreenByHash rva 0x%08X, list at rva 0x%08X%s", hits[0], RvaOf(list),
            g_screenList ? "  OK" : "  (NOT read)");
    }
    else
        Log("[p2spawn] screen list: %d pattern hits (need exactly 1)", n);
    n = FindPattern(kMenuLayer, hits, 4);
    if (n == 1 && *reinterpret_cast<const unsigned char*>(Rva(hits[0] + kMenuGetRootCall)) == 0xE8)
    {
        const uint32_t getRoot = CallTarget(hits[0] + kMenuGetRootCall);
        const uintptr_t root = MatchPatternAt(getRoot, kGetRootBody) ? ReadU32(Rva(getRoot + 1)) : 0;
        if (root >= lo && root < hi)
            g_ifRoot = root;
        char type[64];
        Log("[p2spawn] menu layer rva 0x%08X: GetRoot 0x%08X -> the one IF root at rva 0x%08X (%s)%s", hits[0], getRoot,
            RvaOf(root), TypeOrDash(root, type, sizeof(type)), g_ifRoot ? "  OK" : "  (NOT found)");
    }
    else
        Log("[p2spawn] menu layer: %d pattern hits (need exactly 1)", n);
    n = FindPattern(kMapSetModeBody, hits, 4);
    if (n == 1 && MatchPatternAt(hits[0] + 0x79, kMapTableSite))
    {
        const uintptr_t table = ReadU32(Rva(hits[0] + kMapTableAt));
        if (table >= lo && table < hi)
            g_mapTable = table;
        g_mapSetMode = reinterpret_cast<MapSetMode_t>(Rva(hits[0]));
        Log("[p2spawn] map: SetMode rva 0x%08X, layout table at rva 0x%08X%s", hits[0], RvaOf(table), g_mapTable ? "  OK" : "");
    }
    else
        Log("[p2spawn] map: SetMode %d pattern hits / table site - the layout table is NOT read", n);
    // 19g: MapDisplay::Update (his map's, called while his spawn point screen is up)
    n = FindPattern(kMapUpdateBody, hits, 4);
    if (n == 1 && MatchPatternAt(hits[0] + 0x25, "E8") && MatchPatternAt(hits[0] + 0x2A, "80 7E 20 00 0F 84"))
    {
        g_mapUpdateRva = hits[0];
        Log("[p2spawn] map: MapDisplay::Update rva 0x%08X OK (hides its element +30h first, its enabled byte +20h checked at +2Ah) - "
            "player 2's map is updated every frame his spawn point screen is up",
            hits[0]);
    }
    else
        Log("[p2spawn] map: MapDisplay::Update: %d pattern hits (need exactly 1) - player 2's map NOT updated", n);
    // 19i: MapDisplay::SetFlag (the spawn screen look) and the layout picker's jump thunk (Update's call at
    // +3C8h), the picker byte-checked; the thunk then goes to MapPickWrap (his map with the flag -> layout 4)
    if (g_mapUpdateRva && g_mapWanted)
    {
        uint32_t flagHits[4] = {};
        const int nf = FindPattern(kMapSetFlagBody, flagHits, 4);
        if (nf == 1)
            g_mapSetFlag = reinterpret_cast<MapSetFlag_t>(Rva(flagHits[0]));
        const uint32_t site = g_mapUpdateRva + kMapUpdatePickAt;
        const bool callOk = *reinterpret_cast<const unsigned char*>(Rva(site)) == 0xE8;
        const uint32_t thunk = callOk ? static_cast<uint32_t>(static_cast<int64_t>(site) + 5 + ReadI32At(site + 1)) : 0;
        const bool thunkOk = thunk && *reinterpret_cast<const unsigned char*>(Rva(thunk)) == 0xE9;
        const uint32_t pick = thunkOk ? FollowJumps(thunk) : 0;
        const bool pickOk = pick && MatchPatternAt(pick, kMapPickBody);
        bool wrapped = false;
        if (pickOk && g_mapSetFlag)
        {
            g_mapPick = reinterpret_cast<MapPick_t>(Rva(pick));
            wrapped = RetargetJump(Rva(thunk), reinterpret_cast<const void*>(&MapPickWrap));
            if (wrapped)
                g_mapPickThunk = Rva(thunk);
            else
                g_mapPick = nullptr;
        }
        Log("[p2spawn] map: SetFlag %s (%d pattern hit(s), rva 0x%08X); the layout picker: Update +3C8h call %s -> thunk rva 0x%08X "
            "%s -> picker rva 0x%08X %s; the thunk %s",
            g_mapSetFlag ? "OK" : "NOT found", nf, nf == 1 ? flagHits[0] : 0u, callOk ? "OK" : "is not a call", thunk,
            thunkOk ? "(a jump)" : "(NOT a jump)", pick, pickOk ? "OK" : "does NOT match",
            wrapped ? "now goes through BF1GameExt: his map with the spawn screen flag gets layout 4 (his own) - his map "
                      "gets SetFlag(1) + SetMode(1) as player 1's does, its spawn screen look"
                    : "left alone - his map's flag is set for its Update calls only (19h-r2)");
    }

    // 18t: a screen's activation - elements to roots[viewport] (or the IF root with viewport -1)
    n = FindPattern(kActivate, hits, 4);
    if (n == 1)
    {
        const uint32_t a = hits[0];
        const bool innerOk = MatchPatternAt(a + kActivateInnerAt, kActivateInner);
        const unsigned char* c1 = reinterpret_cast<const unsigned char*>(Rva(a + kActivateIfRootCall));
        const unsigned char* c2 = reinterpret_cast<const unsigned char*>(Rva(a + kActivateRootCall));
        const uint32_t toIf = (c1[0] == 0xE8) ? CallTarget(a + kActivateIfRootCall) : 0;
        const uint32_t toRoot = (c2[0] == 0xE8) ? CallTarget(a + kActivateRootCall) : 0;
        const bool ifOk = toIf && MatchPatternAt(toIf, kAddToIfRootBody);
        const bool rootOk = toRoot && MatchPatternAt(toRoot, kAddToRootBody) && ReadU32(Rva(toRoot + kAddToRootTable)) == g_rootTable;
        Log("[p2spawn] screen activation rva 0x%08X: reads the viewport (+48h) at +20h %s; viewport -1 -> AddToIFRoot 0x%08X %s, "
            "else AddToRoot 0x%08X %s (the same root table as the setup's)%s",
            a, innerOk ? "OK" : "does NOT match", toIf, ifOk ? "OK" : "does NOT match", toRoot, rootOk ? "OK" : "does NOT match",
            (innerOk && ifOk && rootOk) ? " - a screen with a viewport is drawn in that player's root" : "");
    }
    else
        Log("[p2spawn] screen activation: %d pattern hits (need exactly 1)", n);

    // 18t: the preview's PushScreen / PopScreen (spawn_ui.cpp found and checked them), and the game's
    // own per-player pause push as the model for the call
    const uintptr_t pushFn = SpawnUiPushScreenFn();
    const uintptr_t popFn = SpawnUiPopScreenFn();
    bool pauseOk = false;
    n = FindPattern(kPausePush, hits, 4);
    if (n == 1 && pushFn)
    {
        const uint32_t p = hits[0];
        char n1[32] = "";
        char n2[32] = "";
        ReadText(ReadU32(Rva(p + kPausePushName1)), n1, sizeof(n1));
        ReadText(ReadU32(Rva(p + kPausePushName2)), n2, sizeof(n2));
        // the call's target: follow incremental-link jumps until PushScreen's (hooked) entry
        uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(p + kPausePushCall) + 5 + ReadI32At(p + kPausePushCall + 1));
        const uint32_t pushRva = static_cast<uint32_t>(pushFn - ExeBase());
        for (int hop = 0; hop < 8 && t != pushRva && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
            t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
        const uint32_t cd = CallTarget(p + kPausePushCountdownCall);
        pauseOk = strcmp(n1, "ifs_pausemenu") == 0 && strcmp(n2, "ifs_pausemenu2") == 0 && t == pushRva &&
                  MatchPatternAt(cd, kSetCountdownBody);
        Log("[p2spawn] the game's pause push rva 0x%08X: \"%s\" / \"%s\", PushScreen(name, player, player) %s, then "
            "SetCountdown(3) 0x%08X %s",
            p, n1, n2, t == pushRva ? "= spawn_ui_scan's PushScreen OK" : "is NOT spawn_ui_scan's PushScreen", cd,
            MatchPatternAt(cd, kSetCountdownBody) ? "(+194h) OK" : "does NOT match");
    }
    else
        Log("[p2spawn] the game's pause push: %d pattern hits (need exactly 1), PushScreen %s", n, pushFn ? "found" : "NOT found");
    // (the pause push is the model for the call, checked and logged; player 2's screens need PushScreen /
    // PopScreen, which spawn_ui.cpp byte-checked, the managers and the screen list)
    (void)pauseOk;
    if (pushFn && popFn && g_mgrBaseAddr && g_screenList)
    {
        g_pushScreen = reinterpret_cast<PushScreen_t>(pushFn);
        g_popScreen = reinterpret_cast<PopScreen_t>(popFn);
        g_screensOk = true;
    }
    // 18u: player 2's spawn screens
    FlowFind();
    F34Find();            // 20f-r2: players 3 and 4's spawn screens
    // 21j-r15: players 1 / 2's pause objects for the 2-player split's menu layer (if F34Find did not find them); 21j-r16: and
    // CursorFrame (player 2's pause menu keeps the cursor)
    if (g_p2LikeWanted || g_cursorP1Wanted)
        PauseObjsFind();
    Log("[p2spawn] 21j-r15: [Splitscreen] PauseLikeP1P2=%d (%s; players 1 / 2's pause objects va 0x%08X %s), MenuBoxP1=%d (%s), "
        "EndStatsFullScreen=%d (%s)",
        g_p2LikeWanted ? 1 : 0,
        g_p2LikeWanted ? "the 2-player split: player 2's pause menu as player 1's - his OPTIONS gamepad only, his option screens and "
                         "popups in a 16:9 box of his half, his menu where player 1's stands in his half, player 1's menu sounds"
                       : "player 2's pause menu as the game has it",
        static_cast<unsigned>(g_pauseObjs), g_pauseObjs ? "OK" : "NOT found (his option screens stay in his whole half)",
        g_menuBoxP1Wanted ? 1 : 0,
        g_menuBoxP1Wanted ? "the 3-player layout: player 1's popups and option screens in his top left quarter" : "as before (the top half)",
        g_endFullWanted,
        g_endFullWanted >= 2 ? "the stats / awards screens over the whole screen with 2, 3 and 4 players"
        : g_endFullWanted == 1 ? "over the whole screen with 3 / 4 players, in player 1's half with 2" : "in player 1's half / quarter");
    Log("[p2spawn] 21j-r16: [Splitscreen] CursorP1Only=%d (%s; the game's cursor show %s)", g_cursorP1Wanted ? 1 : 0,
        g_cursorP1Wanted ? "in a split screen match the mouse cursor is hidden whenever player 1 has no menu up and player 2's pause "
                           "menu is not on - it no longer stands over player 2's half while he plays"
                         : "the mouse cursor as the game leaves it",
        g_cursorShow ? "found (PopScreen +3Bh)" : "NOT found - left as the game has it");
    SuicideWrapInstall(); // 19a: the pause menu's Respawn kills the player (split screen)
    Log("[p2spawn] PLAYER 2'S SPAWN SCREENS %s: in split screen, while player 2 is not in the world, ifs_readyselect2 (side, "
        "first) -> ifs_mapselect2 (spawn point; after a death) -> ifs_charselect2 (class, A = spawn) on manager 1 (PushScreen "
        "rva 0x%08X, PopScreen rva 0x%08X), pad 2: left / right / up / down, A, B%s",
        g_flowOk ? "ON" : "OFF", RvaOf(pushFn), RvaOf(popFn),
        g_flowWanted ? (g_flowOk ? "" : " - something above was not found; the auto-spawn spawns him") : " ([Splitscreen] P2SpawnScreen=0)");

    Ps34Install(); // 21h-r1: the pause scan (log only)
    RpInstall();   // 21h-r2: players 3 / 4's real pause menus
    LogManagers("start");
    const HANDLE t = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[p2spawn] watching both IF managers every %lu ms; the match scan runs once the spawn screen is set up; a screen "
            "coming up on either manager in a match is logged with its root %d frames later (game thread)",
            static_cast<unsigned long>(kPollMs), kDumpDelayFrames);
    }
    else
        Log("[p2spawn] thread failed to start (error %lu)", GetLastError());
}

} // namespace bf1
