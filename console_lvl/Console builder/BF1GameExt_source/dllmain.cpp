// BF1GameExt - dllmain.cpp   (file build p21j-r31)
// dinput8.dll proxy for Star Wars Battlefront (2004): GOG 1.2 and GOG 1.3.
// Drop the built dinput8.dll next to the game's exe.
// Which exe is running, and every address in it, comes from game_build.cpp.
//
// PHASE 21J - THE BUG LIST (21i-r17 included):
//   - r31: BF1GameExt_pause34.lua p21j-r31 (controller_scan.cpp built in, and in console.lvl's set): p21i-r10 with its build
//     as a string too. The copy next to the exe was p21h-r10 - older (no AUTO AIM rows for players 3 / 4, its stats-screen
//     RESPAWN code run twice) and it came before the DLL's own.
//   - r30: CONSOLE.LVL FROM THE MOD TOOLS (MungeConsole.bat in the shell / common builders: ScriptMunge + LevelPack, the
//     scripts compiled as the shell's own): read as r29's; compiled scripts carry their build as a string (gBFX_Build_*),
//     which the log shows. BF1GameExt_split.lua / BF1GameExt_ctrlpage.lua p21j-r30 (that line only).
//   - r29: CONSOLE.LVL (controller_scan.cpp; ConsoleMunge.exe): the three scripts (BF1GameExt_split, BF1GameExt_pause34,
//     BF1GameExt_ctrlpage) can come from GameData\Data\_LVL_PC\console.lvl, a .lvl in the game's own format (scr_ chunks:
//     NAME + BODY, as GOG 1.3's level loader reads them) made with ConsoleMunge - console.lvl first, then the .lua next to
//     the exe, then the copy built in; each script's log line says which, with its file build, and a NOTE when it is not
//     this DLL's own. Only BF1GameExt reads console.lvl: shell.lvl and common.lvl are not touched.
//   - r28: THE SIGN-IN'S SOUND AND MUSIC BACK (BF1GameExt_split.lua r28, [Splitscreen] SignInSoundFix=1): r27's audio scanner
//     showed every bus and volume normal but the music's stream gone after the sign-in (SetShellMusic found none and changed
//     nothing). On PC each profile switch in the shell restarts the sound engine and drops the shell's own sounds, music and
//     movie streams; the PC login / audio options restore them (ScriptCB_GetMixConfigChanged), the console sign-in (its
//     profile list: profile 2 current and back; its launch's profile loading) never did. Now it runs the game's own restore,
//     starts the music again and keeps its background movie. The scanner also logs the restarted byte and the music's stream.
//   - r27: THE SLIDER NUMBERS FIXED (controller_scan.cpp): BF1GameExt's own Lua trace (21c) read every call's arguments with
//     lua_isnumber, which in Lua 4 turns a string that reads as a number into that number in place - each options screen's
//     "40%" (UTF-16) reached the game's SetUString as the number 4 and was drawn as '4' plus whatever lay after it ("4@",
//     "6,", "|"). The arguments are read straight from the Lua stack now. THE SIGN-IN'S BUTTONS (BF1GameExt_split.lua r27):
//     the pad's B / A on every player's BACK / ACCEPT ([Splitscreen] SignInPadIcons=1 - the PC shell's icons there are the
//     mouse's), the BACK button at the bottom left works with the mouse and is shown with 3 / 4 players too (SignInMouseBack=1).
//     THE AUDIO SCANNER ([Splitscreen] AudioScan=1, [audio] lines, log only): for the music that stops at the split screen
//     sign-in - the scripts' sound / music / movie calls with the sound state after them, and every change of the sound on /
//     off byte, the shell music, the engine's and profiles' volumes and the buses' volume / gain.
//   - r26: THE FREE CAMERA ON PLAYER 1'S PAD (controller_scan.cpp FcUpdWrap, [Splitscreen] FreeCamPad=1): R3 up, L3 down
//     (not RT / LT), the right stick looks, X stops / starts the camera. The FreeCamera vtable's update slot points at the
//     wrapper; the game's update runs unchanged with player 1's pad inputs set around it.
//   - r25: START / ESC IN THE FREE CAMERA OPEN PLAYER 1'S PAUSE MENU (controller_scan.cpp FreeCamPauseFrame, p2spawn_scan.cpp
//     P2SpawnPauseP1): the game's own Start did not pause while the free camera was on, so CHASE CAMERA could not be picked
//     (FREE CAMERA leaves the world frozen - the console's photo camera, BACK switches it - and the game never pauses a frozen
//     world). BF1GameExt pauses as the game's Start does when the game has not by the next frame; [freecam] lines.
//   - r24: THE FAKE CONSOLE'S CRASH FIXED, FAKE CONSOLE UNDER FREE CAMERA (controller_scan.cpp kPauseExtrasLua,
//     FakeConsoleQuery): the game's ScriptCB_DoConsoleCmd switched the flag, then printed it to a debug console the 2020
//     exe never makes - the crash. BF1GameExt switches the flags itself now (the game's own "debugmenu" entries) and each
//     row shows ON / OFF.
//   - r23: FREE CAMERA AND FAKE CONSOLE IN PLAYER 1'S PAUSE MENU (controller_scan.cpp kPauseExtrasLua, [Splitscreen]
//     PauseFreeCam / PauseConsole, [freecam] lines): the game's own items, hidden by gFinalBuild in the 2020 exe, shown
//     again (single player; split screen with 2; never online or over the stats screen). The free camera scan (FreeCamScan,
//     log only) logs which keys / buttons drive its up / down / speed slots and its debug keys (Q, I, ').
//   - r22: THE TUTORIAL MOVIES IN THEIR BOX, GALACTIC CONQUEST'S AND THE CAMPAIGNS' TUTORIALS (controller_scan.cpp
//     kTutorialsLua, [Splitscreen] TutorialsMovieFit / TutorialsGC / TutorialsCampaign, [tutorials] lines): the console-box
//     movies go in movie_BG2's black box at 4:3 (the 2020 game reports 4:3 to its scripts - the movie stood in the box's top
//     left, under its frame); GALACTIC CONQUEST plays tutorial08 over its setup screen; the campaigns' first missions get
//     their command post tutorials (tutorial01cw / tutorial01gcw) after Yoda's briefing, as on the consoles.
//   - r21: THE MAIN MENU'S TUTORIALS BUTTON ON PC (controller_scan.cpp kTutorialsLua, [Splitscreen] TutorialsMenu=1,
//     [tutorials] lines): the PC shell has the consoles' tutorials menu and viewer (movies\tuteng.mvs), only ifs_main's
//     Enter hides the button on PC - shown again after it; the tutorials menu's BACK works with the mouse (the game's own
//     accept looked "_back" up as a tutorial: a Lua error, BACK did nothing).
//   - r20: THE MOUSE READS THE GAME'S OWN CURSOR (classwheel_scan.cpp ReadMouse): r19 took every click at the Windows cursor -
//     the game's cursor read was refused since 6c (the input manager's GetBufB is player2_input.cpp's redirect, outside the
//     exe's code) - so clicks landed away from the arrow on the screen. [wheel] "mouse: the game's cursor ..." lines.
//   - r19: THE SPAWN SCREENS' MOUSE AS THE CLASSIC COLLECTION'S ([ClassWheel] MouseClicks=1 - classwheel_scan.cpp). The class
//     wheel: a click left of the picked soldier turns it left, right of it right; SELECT CHARACTER spawns. The spawn point
//     screen: a click on one of your team's posts on the map picks it (the game's own map test, as its PC screen); SELECT
//     SPAWNPOINT goes on. The side screen: a click on a side joins it. A click anywhere else does nothing.
//   - r18: MODS IN THE MAIN MENU ONLY, RAGDOLLS OFF BY DEFAULT, THE SPAWN SCREEN ROW. (1) The MODS row only in the main menu's
//     OPTIONS (controller_scan.cpp's script does nothing in a match's Lua state): player 1 sets sprint / ragdolls before a match,
//     nobody switches them mid-game (r17 had MODS in player 1's OPTIONS in a match too). (2) Ragdolls off by default ([Ragdoll]
//     Enabled=0, and 0 when the key is missing - ragdoll.cpp); sprint on as before. (3) The main menu's GAME OPTIONS gets a SPAWN
//     SCREEN row ([Splitscreen] SpawnScreenMenu=1): CLASS WHEEL (SINGLEPLAYER ONLY) / PC MENU (SINGLEPLAYER ONLY) - [ClassWheel]
//     Enabled, now 1-player games only (classwheel_scan.cpp: split screen follows [ClassWheel] Split alone), CLASS WHEEL by default.
//   - r17: THE MODS ROW IN OPTIONS ([Splitscreen] ModsMenu=1; [mods] lines). OPTIONS (the main menu's, and player 1's in a match -
//     never player 2's) gets a MODS row above CREDITS (controller_scan.cpp's script, made with OPTIONS). MODS: SPRINT ON / OFF
//     ([Sprint] TestBoost - sprint.cpp: off = no boost, no stamina bar) and RAGDOLL EFFECTS ON / OFF ([Ragdoll] Enabled -
//     ragdoll.cpp: off ends the running ragdolls; with ModsMenu=1 the hooks go in even when it was 0 at game start), both at once
//     and saved; ADDON FOLDER: a box like the map list's with every folder of GameData\AddOn and the missions each adds, the
//     selected one's mission names, how many mods are installed (folders with an addme.script - the game loads only those) and
//     the game's addon missions used of its limit with a bar (512 the game's own, 4096 with [Limits] DLCMissions=1). The numbers
//     are the game's own addon mission list (limits_scan.cpp: AddDownloadableContent's entries, each with its folder's path).
//   - r16: THE PAUSE MENU NOISE FIXED; THE MOUSE CURSOR OFF PLAYER 2'S HALF (from the video and logs after r15). (1) In the 2-player
//     split every pause menu Enter (opening it, coming back to it from OPTIONS - player 1's and player 2's) played its sound over
//     and over in one frame (165 times in a Lua 4.0 test of the same scripts): r15's player 2 script named its helpers bfxp_*, the
//     prefix of the pause scan (21h-r1), and its bfxp_Enter replaced the scan's - the scan's wrappers call bfxp_Enter by name, so
//     the game's Enter ran again and again until Lua's stack overflowed (which also stopped player 2's menu from being moved).
//     controller_scan.cpp: the script's helpers are bfx2_* now, the Enters kept on each screen's own table. (2) The mouse cursor
//     stood in the corner of player 2's half while he played: the game hides it only when a menu stack runs empty, and player 2's
//     pause over his spawn screen left that screen on his stack. p2spawn_scan.cpp ([Splitscreen] CursorP1Only=1): in a split screen
//     match the cursor is hidden whenever player 1 has no menu up and player 2's pause menu is not on.
//   - r15: THE PAUSE MENUS AND THE END OF A MATCH (from the videos after r14). (1) The 2-player split, horizontal and vertical
//     ([Splitscreen] PauseLikeP1P2=1): player 2's pause menu as player 1's - his OPTIONS without VIDEO OPTIONS, CONTROL OPTIONS
//     straight to the gamepad page (controller_scan.cpp's script), his option screens and popups in a 16:9 box of his half
//     (spawn_ui_scan.cpp / frame_capture.cpp - they were stretched over his whole half), his menu where player 1's stands in his
//     half (p2spawn_scan.cpp's "rp p2dx / p2dy" - the halves' safe areas differ), player 1's menu sounds. (2) The 3-player
//     layout ([Splitscreen] MenuBoxP1=1): player 1's option screens and popups in his quarter, not across the top half. (3) The
//     2-player stats screen over the whole screen, centred ([Splitscreen] EndStatsFullScreen=2). (4) 4 players: player 1's
//     COMMAND POST / VEHICLES / OBJECTIVES bars whole again (classwheel_scan.cpp: a copied bar kept the game's bar's fly-in at
//     its start - a stub, no title); players 3 / 4's views on players 1 / 2's end shots at VICTORY / DEFEAT (playern_scan.cpp,
//     [Splitscreen] EndShotP34=1 - player 3 stayed on his soldier).
//   - r14: THE GLYPH LEAK FIXED ([Display] GlyphLeakFix=1; frame_capture.cpp, in the [glyphs] lines). r13's log: no scrambled
//     text (the pool ran low at 392 s, a block of BF1GameExt's was added), but the chunks in use never came back down - 611 at
//     the main menu at start, 1645 at the sign-in screen after the match. The game's text destructor (RedGlyphCacheElement's
//     cleanup) gives the heap the arrays that hold a text's chunk lists and drops the chunks in them: every text deleted with
//     letters leaks them (the game's screens at each match / shell change; with 3 / 4 players BF1GameExt's spawn screen windows
//     and texts, made and deleted each time a screen comes up). Now the cleanup runs the game's own ReleaseGlyphs first (what
//     SetFont does): the chunks go back to the pool. GlyphPoolGrow stays as the safety net.
//   - r13: THE SCRAMBLED TEXT FIXED ([Display] GlyphPoolGrow=1; frame_capture.cpp, [glyphs] lines). r12's scan cleared the vertex
//     buffers (no wraps, no stream mismatches, no freed buffers in a whole match); the game's code shows the cause: every text's
//     letters live in chunks from ONE glyph pool made at start (10240 letters = 1280 chunks of 8, a text takes whole chunks, at
//     least one per font page) - with four quarters of HUDs, windows and menus it runs dry, and a text laid out then keeps its
//     letter count with no letters, so its draw shows what an earlier text left in the vertex buffer (other texts' letters, the
//     same frame after frame; nothing at all on r12's stats screen). The pool's allocator goes through BF1GameExt: when the pool
//     runs low it gets 1024 more chunks of BF1GameExt's own (at most 16 times), taken and given back by the game as its own.
//     TextGarbleScan now 0 by default; the 3 / 4-player spawn point windows a little further apart (p2spawn_scan.cpp).
//   - r12: THE TEXT GARBLE SCAN ([Display] TextGarbleScan=1; frame_capture.cpp, [vbscan] lines; log only - nothing in the game
//     changes). r11's 4-player video: player 2's spawn windows (and at the end the stats screen) drew other texts' letters -
//     slices of the objectives line in his VEHICLES box and on a title bar, smeared letters - the same frame after frame: draws
//     reading vertices that are not theirs. The game's vertex buffer Lock goes through BF1GameExt (its single jump thunk): a
//     dynamic buffer running out of room mid-frame (WRAP), positions running past what the pool handed out (AHEAD), failed
//     locks, a lock on a freed buffer (DEAD), locks on other threads; each draw's stream 0 against the game's stream cache
//     (STREAM); every 10 s the buffers' reference counts (a count past 16383 wraps to 0); End = a MARK line. r11's frame-rate
//     scan stays on (no drop in r11's session: 60 fps; BF1GameExt's own parts 3.1 ms a frame with 4 players).
//   - r11: THE FRAME-RATE SCAN ([Display] PerfScan=1, PerfSpikeMs=25; frame_capture.cpp, [perf] lines; log only - nothing in
//     the game changes). r10's 4-player log fell to 55 - 57 fps in the busiest fights (60 Hz with vsync). The game's thread
//     keeps a stack of timed parts (the CPU's time stamp counter): the game's update, its render frame, its soldiers' poses,
//     its HUD / menu layers, Direct3D's draws / other calls / Present, and BF1GameExt's own (players 3 / 4's frame work,
//     player 2's input, the spawn screen layer, the chooser, players 3 / 4's menus, the ragdolls, the Direct3D wrappers, the
//     log). Every 5 s: fps, frame and work times, each part's average / longest, the frames over the 60 Hz budget broken
//     down; a SLOW FRAME line for each frame over PerfSpikeMs. The parts are entered in FrameWrap (playern_scan.cpp), the
//     input frame hook (player2_input.cpp), the menu / HUD layer and chooser hooks (spawn_ui_scan.cpp), RpIfUpdateWrap
//     (p2spawn_scan.cpp), the pose hook (ragdoll.cpp) and Log (log.cpp). No game code is patched by it.
//   - r10: r9'S CRASH FIXED (frame_capture.cpp): an automatic frame capture (log only, 15 / 45 / 75 s after player 1 is in the
//     world) read past the render thread's stack - TypeOnStack, its third stack walker, had no end check (18y gave the other
//     two theirs). It stops at the thread's stack top now. Not r9's windows (it was a 1-player match). Nothing else changed.
//   - r9: THE CONSOLE WINDOWS ON THE 3 / 4-PLAYER SPAWN SCREENS ([Splitscreen] SpawnPanels34=1; p2spawn_scan.cpp, [panels]
//     lines): the spawn point and class screens of players 1 - 4 in quarters (and player 3's half with 3 players) get the
//     game's windows in place of the texts that stood in for them - COMMAND POST, VEHICLES:, OBJECTIVES: and the class window
//     (orange title bar - a copy of the game's own - over the blue box, the text in it), built with the game's own element
//     constructors in its units and scaled to the view; the quarter's class ring stands left of the class window. 1-PLAYER:
//     the class ring further left, off the class window ([ClassWheel] RingShift=-31, was -13; classwheel_scan.cpp), SELECT
//     SOLDIER with it. 2-player screens unchanged. The INI. Nothing else changed.
//   - r8: GALACTIC CONQUEST'S PICK YOUR SIDE IN THE CONSOLE'S LOOK (bug #4; [Splitscreen] GCPickLook=1, GCCommander2P=1): a row of
//     side pictures for every player (four slots with 3 / 4 players), the PLAYERS text list replaced by a box per player in the
//     class screen's look - its orange title bar PLAYER n, then COMMANDER / PLAYER READY tinted green (Rebels / Republic) or red
//     (Empire / CIS) once he locks in; 2 players lock in and command too (co-op: only the commander moves on the galaxy map).
//     controller_scan.cpp (the built-in split screen script), BF1GameExt_split.lua and the INI. Nothing else changed.
//   - r7: (1) THE DOUBLE BLUR FIXED ([Splitscreen] ScopeBlurFix=1): r6's trace - the blur picture is drawn at the blur target's
//     top left for every view, and the game read it at the view's place on the screen: in split views part of it, twice as
//     big. Its last pass reads the whole picture over every split view now, as 1 player's. (2) PLAYERS 3 / 4'S RETICLE BOB
//     FIXED ([Splitscreen] ReticleFixP34=1): their reticle height came from last frame's camera; it is placed with this
//     frame's (players 1 / 2's always were). playern_scan.cpp and the INI. Nothing else changed.
//   - r6: (1) BIGGER SCOPES IN HALVES ([Splitscreen] ScopeHalfFill=96: the ring fills that % of the half's short side; r5's
//     was 1 player's proportion), the blur there drawn in passes that never overlap (no sharp sliver beside the scope). (2)
//     THE DOUBLE BLUR for players 2 - 4, log only: each view's blur last quad and a device trace of its blur draw ([scope]
//     lines; frame_capture.cpp) - where the blur pass draws its picture. (3) THE RETICLE BOB for players 3 / 4, log only
//     ([Splitscreen] ReticleScan=1, [reticle] lines): their reticle height projected again with the camera FrameWrap sets.
//     playern_scan.cpp, frame_capture.cpp / .hpp, the INI. Nothing else changed.
//   - r5: THE ZOOM SCOPE FITS EACH SPLIT VIEW ([Splitscreen] ScopeFit=1; playern_scan.cpp). r4's log: the scope is drawn
//     full-screen size in every view, its middle at the middle of a lopsided safe area. Now in 2 / 3 / 4-player views its
//     cross is on the crosshair and it is 1 player's scope at the view's size (quarters exactly; halves as big as the short
//     side, dark bands at the ends, the blur round the ring); 1 player untouched. INI: ScopeScan / ScopeFit added.
//   - r4: THE SCOPE SCAN (log only, [scope] lines; playern_scan.cpp, [Splitscreen] ScopeScan=1 - not in the INI yet). Bug #2:
//     the zoom scope in every split view is cut off and its cross is off the reticle (1 player is right). ScopeDisplay draws
//     the scope at the full screen's size in every view, its middle at the middle of the view's safe area (lopsided in the
//     quarters and side-by-side halves). Each ScopeDisplay's Update and the scope blur's draw go through their vtable slots
//     (the game's functions run as they are): its HUD root, safe area, anchor, quarters and what r5 would set (dry run), each
//     view's viewport; r5's sites checked. The grass (r1 - r3) is fixed. Nothing else changed.
//   - r3: r2's crash fixed (controller_scan.cpp): the GrassFadeFix stub sat inside the grass scan's fade stub (+96 in a stub
//     running from +64 for 34 bytes) - its first 2 bytes were overwritten and the first .prp grass draw ran into the middle of
//     it. Each stub has its own 64 bytes now (static_asserts). Nothing else changed.
//   - r2: THE .PRP GRASS FADES IN AGAIN ([Splitscreen] GrassFadeFix=1; controller_scan.cpp). r1's log: the 2020 port hands the
//     .prp layers' grass the near scene end (200 m on Naboo Plains) as its fade distance instead of the layer's own (60 / 160 m,
//     1.2's): drawn out to 196 m, growing in at 180 - 200 m where it cannot be seen. Now it gets the layer's own distance again
//     (held to the near scene end when that is nearer): the near grass grows in at 40 - 60 m as 1.2's. The scan's blinks tell a
//     patch the game moved from one that went missing.
//   - r1: THE GRASS SCAN (log only, [Splitscreen] GrassScan=1, [grass] lines; controller_scan.cpp). Bug #1: the grass never
//     fades in on GOG 1.3 / Steam (1.2 grows each patch out of the ground before it stands at full size) and flickers as you
//     walk closer. Two calls inside the grass draw log its callers (grasspatch objects, the .prp layers' grass), the
//     distances, the fade's maxDist against the layer's own distance (what 1.2 passes) and the prop range, the grow band,
//     blinks, and the near scene range. Nothing in the game changes.
//
// PHASE 21H - REAL PAUSE MENUS FOR PLAYERS 3 AND 4 (21g-r4 included):
//   - r9: (1) Players 3 / 4's VICTORY / DEFEAT as players 1 / 2's - the game's own image (ig_victory_defeat), made with the
//     game's constructors in their views' roots, the colour and texture half the game's Show picks for their team. (2) Start
//     on the stats screen opens player 1's pause menu over it (the game's SetPaused - the PC game only set a flag nobody
//     reads), over the whole screen with the stats: QUIT to the main menu, RESUME GAME back ([Splitscreen] StatsPauseP1=1).
//     (3) Player 1's pause gap 150 -> 400 ms (a match load was taken for a pause). (p2spawn_scan.cpp, classwheel_scan.cpp)
//   - r8: THE END OF A MATCH WITH 3 / 4 PLAYERS AS THE XBOX'S ([Splitscreen] EndMatch34=1, EndStatsFullScreen=1). When the
//     game switches its victory / defeat display on: player 2's and players 3 / 4's spawn screens and pause menus close and
//     stay closed (the text glitches were four sets of spawn screen texts with the stats), nobody spawns, players 3 / 4's
//     soldiers stand still; players 3 / 4 get VICTORY / DEFEAT for their team (the winners' mask from the game's Show -
//     its jump thunk through BF1GameExt) for the 5 s the game shows players 1 / 2's; the stats / awards screens over the
//     whole screen, player 1's pad (p2spawn_scan.cpp, spawn_ui_scan.cpp, frame_capture.cpp).
//   - r7: THE MATCH END SCAN (log only, [Splitscreen] EndScan34=1, [end34] lines; p2spawn_scan.cpp): the game's victory /
//     defeat display watched - when it is made (and the player count its constructor uses: it has room for 2), while it is
//     up (its flags, every player's screens and soldiers) and when it goes - for players 3 / 4's end of match as the Xbox's
//     (VICTORY / DEFEAT on every view, then one stats board over the whole screen). The gamepad page's orange tabs where
//     player 1's are (BF1GameExt_pause34.lua).
//   - r6: (1) Player 1's pause on his spawn screens: the A that picked RESUME GAME was still down when his side / spawn
//     point / class screen came back (his flow stops while the game is paused) and counted as a press - he joined the side,
//     took the post or was spawned. His flow latches the held buttons when it comes back after a gap ([ClassWheel]
//     PauseLatch=1, classwheel_scan.cpp). (2) Players 3 / 4 pause on their side / spawn point / class screens too: Start
//     opens their real pause menu there, the same screen comes back after it with the buttons latched, player 2's way
//     ([Splitscreen] SpawnPauseP34=1). (3) Players 3 / 4 change their buttons on their gamepad page as players 1 / 2 do:
//     the game's remap only writes the button's slot in the PlayerInput's map - the same slot of their own PlayerInput,
//     kept in [Splitscreen] ControlMapP3 / P4 ([Splitscreen] RemapP34=1, p2spawn_scan.cpp, BF1GameExt_pause34.lua).
//     (4) Their gamepad page: one SQUAD COMMANDS line, RESET TO DEFAULTS in full, a small "B BACK".
//   - r5: PLAYER 1'S PAGES FOR PLAYERS 3 / 4 (BF1GameExt_pause34.lua, [Splitscreen] OptionsPagesP34=1). GAME OPTIONS is a copy
//     of player 1's (VIEWPOINT, DIFFICULTY, RESET TO DEFAULTS); CONTROL OPTIONS goes straight to a copy of player 1's gamepad
//     page - INFANTRY / VEHICLE / STARFIGHTER tabs (LB / RB), the pad with each button's action from his own map, JOYSTICK
//     SENSITIVITY with its slider, VIBRATION (NO), INVERT LOOK / FLYING, RESET TO DEFAULTS (Y). No keyboard page. Built from
//     r4's [optscan] numbers at player 1's size on screen (PopupScaleP34). Saved per player as before.
//   - r4: GAMEPAD-ONLY OPTIONS FOR PLAYERS 2-4, STEP 1. (1) Player 2's OPTIONS (3 / 4 players, [Splitscreen] OptionsP2=1): no
//     VIDEO OPTIONS, CONTROL OPTIONS opens the gamepad page at once (no keyboard page). Player 1 keeps everything. (2) Players 3
//     / 4's YES / NO window at player 1's size (PopupScaleP34=51). (3) SCANS for the copies of player 1's GAME OPTIONS and
//     gamepad (INFANTRY / VEHICLE / STARFIGHTER) pages in players 3 / 4's quarters: [optscan] - the game's two pages as it
//     builds them (OptionsScan34=1); [ctlmap] - each player's button -> function map by mode, when players 3 / 4's pause opens.
//   - r3: PLAYER 1'S SCHEME, SOUND, NO CROUCH, PLAYER 2'S POPUPS. (1) Players 3 / 4's OPTIONS: an OPTIONS list with GAME
//     OPTIONS (DIFFICULTY, as player 1's page) and CONTROL OPTIONS (invert look / flying, sensitivity); B goes back a step.
//     RESPAWN / RESTART MISSION / QUIT ask in a popup window like player 1's (the game's question texts, YES / NO with the
//     A / B icons), drawn in their quarter (BF1GameExt_pause34.lua). (2) Their pause is silent and their menus click: the
//     sound's paused update (PauseState::Update +18h's function - the game's pause runs it each frame; nothing ran it for
//     players 3 / 4, so the fade-out never went on) runs while their pause holds the world ([Splitscreen] PauseSoundP34=1).
//     (3) No crouch as they leave the menu: their soldier gets a calm packet while the menu is up and until every button
//     is up after (playern_scan.cpp). (4) 3 / 4 players: player 2's "are you sure" popups and option screens in his quarter
//     (the menu layer in his box while his pause is on - spawn_ui_scan.cpp / frame_capture.cpp, [Splitscreen] MenuBoxP2=1).
//     (5) Player 2's pause menu with player 1's accept / back sounds in 3 / 4-player matches ([Splitscreen] PauseSoundsP2=1).
//   - r2: THE REAL PAUSE MENUS ([Splitscreen] RealPauseP34=1, [rp34] lines; p2spawn_scan.cpp, controller_scan.cpp,
//     BF1GameExt_pause34.lua). r1's scan: ifs_pausemenu / 2 are drawn in roots[viewport] (0 / 1), 512 x 312 each, the items
//     in a container at 0.6 / 0.5 of the root's safe area; BF1GameExt's own player 3 / 4 pause fell back to r8's places (it
//     found 8 menu bars in ifs_pausemenu2, not 6). Now players 3 / 4 get the game's own kind of screen: ifs_pausemenu3 / 4,
//     made by the game's script functions exactly as ifs_pausemenu2 (NewIFShellScreen2, AddVerticalButtons, the split screen
//     layout) right after it, on viewports 2 / 3 - the game draws them in their quarters itself. Two IF managers of
//     BF1GameExt's (the game's constructor; controller / Lua index 2 / 3) run them, driven right after the game's IF update;
//     the Lua index's 2-slot limits answer 2 / 3 with ours (current manager, active screen get / set, a manager's controller);
//     they are reset with the game's. RESUME GAME / OPTIONS (his CONTROLLER page in the same screen) / RESPAWN / RESTART
//     MISSION / QUIT (YES / NO first). World pause, sound and HUD as before. BF1GameExt's own pause screen is the fallback.
//   - r1 is a SCAN (log only, [Splitscreen] PauseScan34=1, [pause34] lines; p2spawn_scan.cpp + controller_scan.cpp): how the
//     game's pause reaches a player's IF manager (PauseState::Update - Start on his pad -> the pausing viewport -> SetPaused ->
//     GetIFManager(player) -> PushScreen("ifs_pausemenu" / "ifs_pausemenu2", player, player)), every 2-slot limit on that way
//     (GetIFManager, the Lua's current manager, the active screen per Lua index, a manager's controller, the IF update's two
//     loops, PauseState[2]), and where each pause screen is drawn (its record's viewport -> roots[viewport]; each element's class
//     and place, when it comes up and 12 frames later). Each pause / unpause logged with both managers, the active screens and
//     the pausing viewport; the 4 HUD roots (size, safe area, whose); the menu input of controllers 0 - 3; players 3 / 4's own
//     pause screens (BF1GameExt's) with their places. The Lua side: the two pause screens' tables as the game's script builds
//     them and at each Enter, the screen sizes the scripts see, and whether a third / fourth pause screen or its templates
//     exist. Three jump thunks go through BF1GameExt (SetPaused, a screen's activation and deactivation - the game's own
//     functions run unchanged). Nothing else changed.
//
// PHASE 21G - THE SPLIT SCREEN MENU, STEP 3: 4 PLAYERS (21f-r3 included):
//   - r4: GALACTIC CONQUEST - WHOSE TURN. (1) A GALAXY TURN SCANNER (log only, [Splitscreen] GCTurnScan=1, [gcturn]
//     lines; BF1GameExt_split.lua + controller_scan.cpp): each turn on the galaxy map and who moves that side (player and
//     pad, or the AI and its joystick), each choice and press and what the game made of it, the battle result and why the
//     turn stays or passes (the game: the attacker attacks again while he wins), the game's script errors, a turn that
//     stands still. (2) CO-OP's AI TURN FIXED: with players 1 and 2 on one side the game's AI moves the other on its third
//     joystick, whose team was never set - the game's script stopped at each AI move and the AI never took its turn; that
//     joystick is put on the AI's team (2 players too). (3) RAGDOLLS ON THE LATER MAPS OF A SESSION (ragdoll.cpp): the
//     16-slot skeleton table filled up after a few maps (Mos Eisley, the 7th level: 0 ragdolls, every death "skeleton:
//     not readable"); slots unused for 5 s are given to new skeletons, and a slot whose skeleton changed is read again.
//   - r3: GALACTIC CONQUEST WITH 3 / 4 PLAYERS: PICK YOUR SIDE - every player picks a side and locks in (A; B unlocks);
//     the first who locks in on a side commands it: only he moves on the galaxy map for it (BF1GameExt_split.lua routes
//     the presses on ifs_meta_main; players 3 / 4 through pads 3 / 4); VERSUS when both sides have players, else CO-OP.
//     NO SIDE SWITCHING FOR PLAYERS 3 / 4 TOO (p2spawn_scan.cpp): while sides are locked their spawn screens have no side
//     screen - the campaign: player 1's side; Galactic Conquest: their pick (controller_scan.cpp LockedSideP34).
//   - r2: (1) THE PLAYER COUNT FOLLOWS THE SIGN-IN: Players / Views are what is installed (the most); the 4-player
//     sign-in's LAUNCH sets each following match's count - 2 = the game's own 2-player split (no quarters, players 3 / 4
//     stand aside), 3 = the 3-player layout (player 3 in the bottom half), 4 = quarters - and the pads that signed in are
//     players 3 / 4's (player 4 alone plays as player 3). playern_scan.cpp PlayerNSignIn; until a sign-in, the INI's.
//     (2) TEMPORARY PROFILES ([Splitscreen] TempProfiles=1): PLAYER 2 / 3 / 4 in the sign-in list where no profile has that
//     name, players 2 / 3 / 4 start on their own, never saved (never in the profile screen). Player 2's is made at the
//     launch the way the game makes its own PLAYER 2 when there are not 2 profiles (controller_scan.cpp TempProfile2,
//     through the game's GetLoginList code, byte-checked); ifs_saveop does not save it. Player 1 signs in with a saved one.
//   - r1: THE 4-PLAYER SIGN-IN, as Battlefront II has it on the Xbox - with players 3 / 4 on (Players=3 / 4) and
//     [Splitscreen] SignIn4=1: the sign-in screen in four quarters, split by the game's own middle line (the shell's
//     gray_rect, tinted 192 / 192 / 255 at 0.8 - Battlefront II's lines are the same texture, tint and thickness) and a
//     vertical one made the same way; player 1 top left, player 2 top right, player 3 bottom left, player 4 bottom right
//     (their places in the match). Each quarter: PRESS START BUTTON TO JOIN -> the profile list with BACK / ACCEPT ->
//     the profile's name over READY, at Battlefront II's places; a profile READY for one player is greyed for the
//     others. Players 1 and 2 keep the game's own pieces (moved into their quarters while the screen is made); players
//     3 and 4 get copies of them (BF1GameExt_split.lua). Pads 3 and 4 are read in the shell as pad 2 is
//     (controller_scan.cpp: the first free XInput pads, as players 3 / 4's in a match). START launches once players 1
//     and 2 are READY and every other player who joined is READY too. Players 3 and 4's profiles are names only (the
//     game loads 2). The match's player count still comes from Players (r2: from the players who sign in). With 2
//     players the 2-player sign-in exactly as before. No new hooks.
//
// PHASE 21F - THE SPLIT SCREEN MENU, STEP 2: THE RIGHT TEAMS IN GALACTIC CONQUEST AND THE CAMPAIGN (21e included):
//   - r3: after a conquest's battle the conquest resumes again (the galaxy, its save question): r2's PICK YOUR SIDE took
//     over ifs_meta_top's whole Enter, also the game's "load the conquest in progress"; now only for a new conquest.
//   - r2: Galactic Conquest rebuilt after the Classic Collection video: PICK YOUR SIDE (below) instead of r1's pad 2 line
//     on the maps screen, the 4-conquest split screen map list. Nothing else changed from r1.
//   - r1: (1) GALACTIC CONQUEST as the Classic Collection has it in split screen: the map list (the 4 two-sided conquests)
//     -> LAUNCH -> PICK YOUR SIDE (ifs_meta_top, the console's screen still in the 2020 shell: player 1's row on top,
//     player 2's below, left / right picks a side, the picked one lit; BACK / LAUNCH) -> LAUNCH -> the galaxy. Same side =
//     CO-OP (the AI runs the other side), other sides = VERSUS (player 2 runs the other side on the galaxy map, the battles
//     are player 1 against player 2). The PC script launched from the map list with player 2 on player 1's opponent (the
//     AI on it) and read the full list's row while showing the split list; BF1GameExt_split.lua sets player 2's team
//     (metagame_state_local.mJoystickTeams[2]) and the AI at the launch, from the split list. (2) NO SIDE SWITCHING: the shell turns the game's "can switch sides" off for the
//     campaign (both players already get the mission's side) and Galactic Conquest; while it is off, players 1 and 2's
//     spawn screens have no side screen (a match starts at the spawn point screen on the game's side, B does not go back
//     to it) - classwheel_scan.cpp / p2spawn_scan.cpp through controller_scan.cpp's SidesLockedNow (the byte found through
//     ScriptCB_SetCanSwitchSides, byte-checked). [Splitscreen] SideLock=1. Instant Action keeps its side screens. (3) The
//     sign-in starts player 1 on his own profile by name when the list has no active slot 1 for it. Players 3 / 4: next.
//
// PHASE 21E - THE SPLIT SCREEN MENU, STEP 1: 2 PLAYERS (21d's controller page included; built on 20g-r14):
//   - r1: the console's SPLIT SCREEN entry of the main menu back on PC. The 2020 shell scripts still have the console's
//     split screen flow: ifs_main's "split" button (its Enter hides it on PC) sets split screen on and opens
//     ifs_split_profile, the console sign-in screen (each player picks a profile, player 2 joins with START, START with
//     both READY loads the profiles and opens the game modes with a SPLIT SCREEN title: Instant Action -> the map list
//     -> the match). controller_scan.cpp runs BF1GameExt_split.lua (built in, or the file next to the exe) with the
//     game's Lua while the game makes / opens ifs_main and ifs_split_profile: the button is shown; on the sign-in screen
//     player 1 is the game's own input (keyboard, mouse, the game's pad; A works as START), player 2 is pad 2 - the PC
//     shell gets only the game's pad, so BF1GameExt reads pad 2 (XInput) and hands its presses to the screen one at a
//     time (ScriptCB_GetPlatform("BFXQ: pad2") -> a number, pushed with the game's pushnumber); missing texts in English;
//     [splitmenu] lines, and the sign-in's ScriptCB calls in the [luacb] trace. [Splitscreen] MainMenuSplit=1 (0 = the main
//     menu as the game has it), SplitSignIn=1 (0 = SPLIT SCREEN goes straight to the game modes). 4 players next.
//
// PHASE 21D - THE CONTROLLER PAGE MADE CHANGEABLE (21c's scan included; built on 20g-r14):
//   - r2: RESET TO DEFAULTS (r1's test: remapping works; the PC controls page's RESET left the pad remap in place - the
//     game's ScriptCB_ResetControl only resets the keyboard / mouse of the shown tab). Right after the game's reset the
//     same tab's pad layout goes back to the game's default too. The controller page gets a RESET (Y) button of its own
//     (as the PC controls page's, bottom middle - the script now also runs while the game makes the screen,
//     ScriptCB_AddIFScreen, so the button is made with the rest); Y or a click on it resets the shown tab.
//   - r1: the 2020 script of the controller page (ifs_opt_controller, from the 2020 common scripts) still has the
//     console's remap code - the pick list, the action lists, SetEntry -> ScriptCB_SetFunctionIdForButtonId /
//     AnalogId - turned off (gNoControllerChanges = 1, the cursor functions emptied). The exe still has Lua 4.0.1's
//     compiler, so BF1GameExt runs BF1GameExt_ctrlpage.lua with the game's lua_dobuffer right before the page opens:
//     the pad labels are picked with the d-pad / stick (orange), A opens the tab's list of actions, A picks (the
//     game's own functions change the current player's pad table), B closes it; down from the bottom goes to the 2020
//     settings and back; LB / RB the tabs; Y resets the shown tab. Players 1 (menus / his pause) and 2 (his pause).
//     L3 stays sprint (shown as SPRINT). Changes last until the game closes. No common.lvl rebuild: the script is in
//     the DLL, or BF1GameExt_ctrlpage.lua next to the exe wins. [ctrlpage] lines, [Controls] ControllerPage=1.
//
// PHASE 21C - CONTROLLER PAGE SCAN (log only; built on 20g-r14 - 21a / 21b were taken back out):
//   - r1: controller_scan.cpp's [luacb] lines. The options screens are Lua; at DllMain, before the game registers its
//     Lua functions, a logging stub goes into the registration entries of the ScriptCB functions they use (the
//     console controller config: Get/SetFunctionIdForButtonId / AnalogId, Y flip, control scale, control mode,
//     reset, rumble, joystick, GetPlatform; screens and popups; IFObj_SetVis / Enable, IFText_SetString /
//     SetUString). None of them is read by any other part. Each call goes to the game's function unchanged; the first
//     call of each is logged, and every call while an "ifs_opt..." screen is up (exact repeats counted), with its
//     arguments, results and the current player. [Controls] LuaTrace=1.
//
// PHASE 20G - 4-PLAYER STEP 7, THE HUD FOR PLAYERS 3 AND 4 (everything from 20f still included):
//   - r14: 3-player: player 3's bottom half no longer stretched - the game's chase camera gives every camera HUD root 0's
//     (player 1's quarter's) shape each frame; his gets his own half's root's shape right after it, as the 2-player
//     bottom half has.
//   - r13: 3-player step 1 (Views=3, Players=3): players 1 and 2 in the top quarters as 4-player, player 3 in the
//     full-width bottom half (a half's camera shape, Classic's safe area) with his screens at player 2's horizontal places;
//     a log-only scan of player 2's panels / class window for player 3's copies next round.
//   - r12: 2-player horizontal split: both players' side, spawn point and class screens laid out as the Classic
//     Collection's horizontal split (its safe area in both halves' HUD roots, the game's own turned panels / class window,
//     soldiers, rings, texts and maps at its places) - [Splitscreen] MenusHorizontal / HorizontalSafeArea.
//   - r11: player 1's spawn point screen (his map) and class screen drawn as players 3 / 4's in 4-player; the droideka
//     on players 2 - 4's class rings stands at its slot.
//   - r10: players 2 - 4's death cameras as player 1's (the game's: they turn to the killer; DeathCamStill=0 by default)
//     until their spawn screens come up (their respawn timer, 4 s, as player 1's), then player 1's camera shot from that
//     frame; players 1 and 2's side screens drawn as players 3 / 4's (their group, root and places) in 4-player.
//   - r9: players 2 - 4 show player 1's camera shot while they have no soldier (CamShotP234); players 3 / 4's zoom scope
//     made at last (r7 allowed it without its wraps); objects near players 3 / 4 collide (CollisionP34: the collision
//     manager only collided objects near players 1 / 2's cameras); players 3 / 4's pause screen copies the game's menu
//     bars (PauseLookP34), mutes the game's sound (PauseMuteP34) and plays its sounds; the class rings in quarters laid
//     out like Battlefront II's 4-player wheel, droidekas on them placed around the game's soldier's place again.
//   - r8: OPTIONS on players 3 / 4's pause screens opens their CONTROLLER page: INVERT LOOK, INVERT FLYING, SENSITIVITY
//     (the game's own per-mode Y flip and look scale in their PlayerInputs; saved to BF1GameExt.ini [Splitscreen]
//     InvertLookP3 / P4, InvertFlyingP3 / P4, SensitivityP3 / P4). Flying is now inverted by default for them, as the
//     game's default profile has it for player 1.
//   - r7: one player backing into a wall no longer pulls players 3 / 4's cameras in (their own wall distance,
//     WallCamP34); their red damage tint (TintP34), command post light colours per view (CpLightsP34), their zoom
//     scopes and scope blur (ScopeP34); players 2-4's death cameras hold a still picture (DeathCamStill); players 3 / 4's
//     pause screens as the game's (GAME PAUSED - RESUME GAME / OPTIONS / RESPAWN / RESTART MISSION / QUIT, the game
//     paused while up: PauseWorldP34); in 4-player players 1 and 2's side / spawn point / class screens at players 3 /
//     4's places and sizes and player 1's menus in his own quarter (MenusLikeP34); [fx34] particle draws per view (log).
//   - r6: no more low-res soldiers in the other views while player 1 is on his spawn screens (the spawn screen's low
//     detail now only in his own view); players 2-4 spawn without waiting for player 1 (SpawnWaitP1=0); players 3 and
//     4 get the game's death camera (DeathCamP34), their map on their spawn point screen (SpawnMapP34) and pause
//     screens of their own (PauseP34: resume / respawn / change side) and sound listeners of their own (SoundP34);
//     ragdolls count players 3 and 4 as players and reach 150 m (ragdoll.cpp, [Ragdoll] Range).
//   - r5: the crash after the first round (players 3 and 4's spawn screen group deleted in the next match, after its
//     heap was gone - now deleted at the teardown); their minimap icons (command posts, soldiers, vehicles) and small
//     map; command post colours in views 3 / 4 from their own team; their stamina bar; [diag34] / [wall34] logs.
//   - r4: r3 crashed when player 3 was first in the world: his MapDisplay's objectives panel looked up
//     "ifs_mapselect3..." (no such screen) and read the null; it now does nothing for players 3 / 4's MapDisplays.
//   - r3: [Splitscreen] HudP34=1 - players 3 and 4 get their own HUD displays (the game's own classes, two more of
//     each but ScopeDisplay) in the HUD roots of views 3 / 4: made, set up, hidden while their spawn screens are up,
//     updated by the game and deleted at the teardown where the game does it for its own (playern_scan.cpp).
//   - r1 / r2 were a SCAN (log only): playern_scan.cpp finds and byte-checks what the HUD for players 3 and 4 needs -
//     the 8 displays' create, destroy, reset and per-camera Setup functions (and their jump thunks), the match
//     teardown's destroy order, the camera manager's RootIndex / IsChase / GetChase - and logs the live displays in
//     the 45 s / 3 min tables ([hud34] lines). r2: r1 crashed at start on Steam (the MapDisplay Setup scan read
//     outside the exe); its code readers are now bounded to the exe's image.
// PHASE 20F - 4-PLAYER STEP 6, SPAWN SCREENS FOR PLAYERS 3 AND 4 (everything from 20e still included):
//   - r4: the real cause of the r2 / r3 crash (r3's heap log): players 3 and 4's group was a RedGroupElement (0xA0
//     bytes) and the game's AddToRoot writes a root child's anchor and flag at +A0h..+A8h, past its end - over the next
//     heap block's header (the runtime heap's 75 MB free block became size 0, the game's next allocation failed). The
//     group is now a RedScreenGroupElement (0xB0, the class a HUD root's own groups are; classwheel_scan.cpp).
//   - r3: r2 crashed the first frame players 3 and 4's screens came up: the game's new / delete use the CURRENT heap,
//     and the heap current in that frame had about one group's worth left - our group took it and the game's next
//     allocation failed. Players 3 and 4's screens now make and delete everything with the game's runtime heap current
//     (p2spawn_scan.cpp; the heaps logged at start and each match start, the game's failed-allocation record logged
//     if an element still fails).
//   - r2: [Splitscreen] SpawnP34=1 (with Players=3 / 4 and PadsP34=1): r1's scan found no console screens for players
//     3 and 4 on PC (none in the screen list, none of their paths in the registry) and 2 IF managers only, so
//     BF1GameExt builds theirs: an empty group of the game's (classwheel_scan.cpp) in the HUD root of each one's
//     quarter (the game's AddToRoot; playern_scan.cpp turns that view's HUD flag on while it is shown) with player
//     2's screens' parts - texts, side soldiers, the class ring (no EQUIPMENT box), the blue grid - and player 2's
//     flow: side -> spawn point -> class, his respawn timer after a death, his own pad (d-pad / stick, A, B); A on
//     the class screen spawns him through the PC Spawn button's calls on his character (only while player 1 is in
//     the world). Without his pad the auto-spawn spawns him as before. [p34spawn] lines; [wheel] 20f line.
//   - r1 is a SCAN (log only): p2spawn_scan.cpp's match scan also looks for players 3 and 4's console screens
// (ifs_readyselect3 / 4, ifs_charselect3 / 4, ifs_mapselect3 / 4, ifs_pausemenu3 / 4) and their elements in the
// registry, and lists every screen record in the list. The game has 2 IF managers (the Xbox 4); what the scan
// finds decides how players 3 and 4 get theirs.
// PHASE 20E - 4-PLAYER STEP 5, PADS 3 AND 4 (everything from 20d still included):
//   - [Splitscreen] PadsP34=1 (with Players=3 / 4; playern_scan.cpp, pad2.cpp, player2_input.cpp): pads 3 and 4 are
//     built and driven like pad 2, players 3 and 4 get input objects like player 2's, their control packets go into
//     the control states of player ids 2 and 3, GetControlState answers those for ids 2 / 3 (offline the game ANDs
//     the id with 1) and the input manager's getters answer indexes 2 / 3 with their objects. [pad34] lines.
//     PadsP34=0: players 3 and 4 copy pads 1 and 2 as in 20d. Nothing changes for 2 players.
//   - r2: views 3 and 4 through the game's own chase camera for players 3 and 4 (a ChaseCamera of the game's class
//     each, given its target by the game's SetChaseTarget for camera 2 / 3); L3 sprint + stamina and third person for
//     players 3 and 4 (sprint.cpp). [chase34] lines.
// PHASE 20D - 4-PLAYER STEP 4, PLAYERS 3 AND 4 IN THE WORLD (everything from 20c still included):
//   - r6: the shooting crash found: the damage screen tint for player 3 / 4 wrote past the screen filter's 2
//     records into the next effect (its vtable became the tint's id 0xDE020766); the tint, its draw in views 3 / 4
//     and the motion blur pair guarded for players 3 / 4 (playern_scan.cpp).
//   - r5: still a crash when shooting: the input manager's rumble functions (one slot, indexed inline) do nothing
//     for players 3 / 4; [p34] lines: the first time each call site acts for player 3 / 4, logged at once.
//   - r4: the crash when shooting (player 3 / 4 fired with players 1 / 2's controls and the weapon's fire went to
//     a first-person model they don't have): two more first-person functions guarded (playern_scan.cpp).
//   - r3: the crash when player 3 / 4 was shot (his HUD display slot was empty): the display getters answer player
//     1's / 2's display for players 3 / 4 until they have their own HUDs (playern_scan.cpp).
//   - r2: the crash when players 3 and 4 spawned (the camera manager was handed camera slot 2 / 3 and wrote its own
//     camera count): its per-camera methods guarded for slots 2 and 3 (playern_scan.cpp).
//   - [Splitscreen] Players=3 / 4 (playern_scan.cpp): the 11 first-person / player-music functions (2-slot tables)
//     and the pause calls answer "nothing" for players 3 and 4 instead of reading past their tables; views 3 and 4
//     follow players 3 and 4's soldiers (a chase camera behind each one; the spectator until he is in the world);
//     AutoSpawnP34=1 presses Spawn for players 3 and 4 (SetReady, every 3 s while player 1 is in the world). They
//     have no pads yet (they stand). [players] lines.
// PHASE 20C - 4-PLAYER STEP 3, PLAYERS 3 AND 4 MADE TO EXIST (everything from 20b still included):
//   - [Splitscreen] Players=3 or 4 (with Views=4; playern_scan.cpp): while split screen is on the game is told 3 / 4
//     local players (the count's one reader goes through BF1GameExt), except the 10 call sites that walk a 2-slot
//     table (camera manager creation and SetNumCameras, the three pause loops, the message loop) - they keep getting
//     2; the network's local slots go to 4 of ours (operands through the relocation table, bounds 2 -> 4). The match
//     setup then makes characters for players 3 and 4. No input, camera or spawn for them yet. Players=2 (default):
//     nothing of 20c installed. [players] lines.
//   - 20b fixes: the spectators turned round the world's up (they looked up when the player looked down); the camera
//     count resets (about 270 a match) logged first 6 then every 100th
// PHASE 20B - 4-PLAYER STEP 2, THE DRAWING SIDE TO 4 VIEWS (everything from 20a still included):
//   - [Splitscreen] Views=4 (playern_scan.cpp): after the game's SetNumCameras(2) the two players' views become the
//     top quarters (player 1 left, player 2 right) and views 3 and 4 the bottom quarters, drawn by the game's own
//     renderer (it already takes 4 views) with cameras of BF1GameExt's own - spectators (player 1's / player 2's
//     camera turned round, no HUD) until players 3 and 4 exist. Views=2 (the default): nothing of 20b is installed,
//     the 2-player split exactly as before. [views4] lines.
//   - 20a's tables print the characters' names right (16-bit text)
// PHASE 20A - 4-PLAYER STEP 1, THE MAP (log only; everything from 19i still included, nothing in the game changes):
//   - playern_scan.cpp (new): every place the PC game stops at 2 local players, found in GOG 1.3 and listed at
//     the top of that file - the count and its 24 readers, the IF managers, the network's local slots, the
//     control states (offline the player index is ANDed with 1), the pause states, input (arrays of one), local
//     slots and profiles, the camera manager (2 cameras, only the 1- and 2-camera layouts) and the renderer
//     (already 4 views), the HUD (8 displays of 2 + 3 more), first person, the other static 2-slot arrays
//   - at start each site is found by pattern on the running exe and checked; a census of the calls into 16
//     per-player entry points (through their jump thunks) by caller and player; every per-player table live
//     45 s and 3 min into a split screen match, with what lies right after each 2-slot table; the census 45 s
//     in and at the match's end. [playerN] lines. [Splitscreen] PlayerNScan=1 (0 = none of it installed)
// PHASE 19I - NO GLITCH FRAME, PLAYER 2'S DEATH AND MAP AS PLAYER 1'S (everything from 19h-r2 still included):
//   - a screen switch of ours (player 2's screens, player 1's console screens) is made and settled in the
//     same frame (the IF manager's own tick runs its queued Exit / Enter / activation), and the new screen
//     is set up before it is first drawn: the one frame that drew the screen as the game left it is gone.
//     [Splitscreen] SpawnScreenSwitchNow=1 (p2spawn_scan.cpp, spawn_ui_scan.cpp, classwheel_scan.cpp)
//   - player 2's spawn point screen after a death waits for his character's respawn timer (+C0h) to run
//     out, exactly what opens player 1's (about 4 s, the killer shown meanwhile), not 1.5 s
//   - player 2's ring and side screen soldiers player 1's size and places (0.508 / 0.54)
//   - player 2's map in the spawn screen look player 1's has (MapDisplay::SetFlag(1): no dark quad, no HUD
//     icons; the layout picker's thunk goes through BF1GameExt so his map keeps its own layout 4), and his
//     chosen post lit on it (the spawn screen's post and team are his for each of his map's Update calls)
//   - UI sounds: left / right tick with a name the sound banks have (shell_select_change); plays logged
//   - log only: player 2's root's shown elements while his spawn point screen is up (the "OBJECTIVES:"
//     over his map), the map's +1F510h group
// PHASE 19H-R2 (19h's log and video): the flat layout never ran (it looked for the title bar in the
//   panel's skin; the bar is the title's own) and 19g's pin came 30 frames late (the game's layout slid
//   in): each panel is placed by its title bar, scale 0.6, the boxes stretched to the Xbox's; the pin
//   on the first frame. Player 2's map: the game runs his map's Update itself and hid it again after
//   ours - his map's vtable is copied with the Update slot wrapped while his spawn point screen is up.
// PHASE 19H - FLAT PANELS, THE XBOX'S RINGS, PLAYER 2'S MAP ON TOP (everything from 19g still included):
//   - the spawn point screen's panels and the class window flat (their groups were turned 42 degrees
//     about the vertical axis; the Xbox's are not), scaled and placed from their own measured sizes to
//     the Xbox's place in each half; 19g's places if a size does not check out (classwheel_scan.cpp,
//     p2spawn_scan.cpp). Player 2's class ring and grid stand straight with his window's group
//   - both class rings in the Xbox's arrangement: the picked soldier big, front right, the others
//     smaller behind, feet level
//   - player 2's map drawn last in his root (layer 1, as the spawn screen does player 1's) while his
//     spawn point screen is up, and a one-time dump of both maps side by side (log only)
//   - no "B BACK" on the spawn point screens, no score bar on the side screens (TeamDisplay +30h off
//     while it is up), player 2's VEHICLES box filled (the game's own PopupFill for his post), the
//     maps 157 x 168, the side soldiers' feet 3% lower
// PHASE 19G - THE XBOX'S VERTICAL SPAWN SCREENS (everything from 19f still included):
//   - player 2's three spawn screens get the blue grid player 1 has behind his (a copy of the spawn
//     screen's "shell_cube" model) [Splitscreen] P2SpawnBackdrop=1 (classwheel_scan.cpp, p2spawn_scan.cpp)
//   - the maps: split screen uses the map layouts 5 (player 1's) and 4 (player 2's), not 1: each takes
//     the Xbox's place above the panels on that player's spawn point screen, player 1's goes off the
//     screen on his side / class screens; player 2's map is updated every frame while his spawn point
//     screen is up (the game updates player 1's alone, so his never drew)
//   - player 2's class screen shows a ring of his side's classes that turns, like player 1's
//   - both halves laid out like the Xbox shots: side screen soldiers smaller with the names and title
//     where the Xbox has them, panels / window centred, the ring smaller over the window, "B BACK"
//     moved in and up (none on the side screen), the prompts on its row, no "SELECT SOLDIER" in split
//     screen; the panels' matrices logged (the PC tilts them). 1-player layout as before 18z.
// PHASE 19F - RESPAWN DIES AT ONCE (everything from 19e still included):
//   - after the kill the player is unpaused (the game's SetPaused(player, 0), as ScriptCB_Unpause does):
//     his pause menu goes and he falls at once, instead of when he leaves the menu (p2spawn_scan.cpp)
// PHASE 19E - RESPAWN THROUGH THE DEATH HOOK (everything from 19d still included):
//   - the Respawn's death is called through the vtable slot (ragdoll.cpp's hook): a ragdoll and a death
//     record like any death (p2spawn_scan.cpp)
// PHASE 19D - RESPAWN KILLS THE SOLDIER (everything from 19c still included):
//   - the character's unit is the controllable 250h into the EntitySoldier (19c's log): the soldier is
//     unit minus the RTTI locator's offset, and its +8 vtable slot 1 is the death function (p2spawn_scan.cpp)
// PHASE 19C - RESPAWN + LAYOUT DIAGNOSTIC (everything from 19b still included):
//   - log only: the Respawn's unit (type, +8 vtable, slot 1, ragdoll.cpp's original, which check failed)
//     and the roots / groups the screens are placed in (p2spawn_scan.cpp, classwheel_scan.cpp)
// PHASE 19B - RESPAWN BEHIND THE RAGDOLL HOOK + PINNED GROUPS (everything from 19a still included):
//   - the Respawn's death function is taken from ragdoll.cpp while its hook sits in the vtable slot
//     (19a found the hook there and gave up); the panels' / window's parent groups are pinned at 0, 0
//     before the children are placed (the script slid them to different places)
// PHASE 19A - LAYOUT TUNE + RESPAWN KILLS (everything from 18z still included):
//   - the panels keep the script's spacing (they overlapped), player 1's half gets its own x (it draws
//     a quarter further right than player 2's), his ring further right (classwheel_scan.cpp)
//   - the pause menu's Respawn in split screen kills the player's unit through its death function
//     (ScriptCB_PlayerSuicide's registration slot wrapped; the game's own removed it with no death)
//     [Splitscreen] P2RespawnDies=1 (p2spawn_scan.cpp)
// PHASE 18Z - THE XBOX LAYOUT (everything from 18y still included):
//   - both halves: the spawn point screen's COMMAND POST / VEHICLES / OBJECTIVES panels centred in the
//     lower half with the map above them (the map's drawn element at map +30h shown, its anchor moved
//     up, the big map layout smaller while a spawn point screen is up); the class window centred under
//     the soldiers; player 1's map really hidden on his side screen / soldier select (18x hid the
//     wrong object) (classwheel_scan.cpp, p2spawn_scan.cpp)
// PHASE 18Y - STACK WALK CRASH FIX (everything from 18x still included):
//   - frame_capture.cpp's stack walkers stop at the thread's stack top (TEB StackBase): a capture during
//     player 2's respawn read past the end of the stack (access violation in DeepCallers)
// PHASE 18X - SPLIT SCREEN LAYOUT (everything from 18w still included):
//   - player 1: his half is half as wide in the soldiers' units - the side soldiers and the ring use
//     half the width (the left side soldier was off the screen); his map element hidden on the side
//     screen and soldier select, shown on the spawn point screen (classwheel_scan.cpp)
//   - player 2: 3D soldiers on his side screen (both sides' first class, the picked one lit) and the
//     picked class on his class screen, made with the ring's code in his screens' groups; his map
//     hidden off the spawn point screen. [Splitscreen] P2SpawnSoldiers=1 (p2spawn_scan.cpp)
// PHASE 18W - PLAYER 1'S CONSOLE FLOW IN SPLIT SCREEN (everything from 18v still included):
//   - the console flow (side -> spawn point -> class wheel, classwheel_scan.cpp) runs in split screen
//     too, in player 1's half: the chooser shows ifs_readyselect1 there (spawn_ui_scan.cpp), the
//     mouse maps to the left half. [ClassWheel] Split=1. The map keeps the game's split layout for now
//   - player 2's "SELECT ..." prompts on the "B BACK" row (they were drawn tilted in the Info window)
// PHASE 18V - PLAYER 2 MENU INPUT (everything from 18u still included):
//   - 18u's log: in player 2's pause menu the interface never asked for his buttons at all. The IF
//     update gives a menu manager input only while its player is below the input manager's player
//     count, which the PC game sets to 1 at boot - so manager 1 (player 2's) never got input. Its one
//     read of that count now goes through BF1GameExt: 2 in split screen while player 2's input objects
//     are wired (player2_input.cpp's redirect), the game's own value otherwise; applied after 18u's
//     held-button table (player 2's slot is used from now on). [Splitscreen] P2MenuInputFix=1 covers
//     both (p2spawn_scan.cpp; player2_input.cpp: Player2RedirectActive)
// PHASE 18U - PLAYER 2 SPAWN SCREENS + MENU INPUT FIX (everything from 18t still included):
//   - player 2's menus take pad 2 again: the interface's input step keeps each player's held
//     buttons in a table of ONE 1Ah-byte slot on PC (the Xbox's has 4), so player 2's slot ran over
//     the Lua's current-manager index and the object after it - his pause menu never moved. The 25
//     references are moved to a 4-slot table in BF1GameExt (found by pattern, each one checked,
//     read back). [Splitscreen] P2MenuInputFix=1
//   - player 2's own console spawn screens in split screen, by themselves: ifs_readyselect2 ->
//     ifs_mapselect2 -> ifs_charselect2 on manager 1 (the game's PushScreen, drawn in his half),
//     driven by pad 2 (left / right / up / down, A, B); "CHOOSE YOUR SIDE" and the side names, the
//     post's name and the team's objective in the panels, the class name and EQUIPMENT; A on the
//     class spawns him through the game's own Spawn button calls (SetSpawnChoice, SetReady) - while
//     player 1 is in the world, as the auto-spawn did; auto-spawn stands aside while the screens run;
//     his map in its spawn screen mode on the spawn point screen. [Splitscreen] P2SpawnScreen=1,
//     P2SpawnMap=1 (p2spawn_scan.cpp; classwheel_scan.cpp lends its texts and sounds; autospawn.cpp)
//   - the F5 preview is gone (the screens come up by themselves)
// PHASE 18T - PLAYER 2 SCREENS PREVIEW (everything from 18s still included):
//   - 18s found player 2's console screens loaded in split screen (ifs_readyselect2 / mapselect2 /
//     charselect2, viewport 1), and the game draws a screen with a viewport in that player's own HUD
//     root - player 2's half. F5 in split screen (in a match) cycles them on manager 1 with the
//     game's own PushScreen / PopScreen, as the game pushes player 2's pause menu: none -> side ->
//     spawn point -> class -> none (diagnostic, for screenshots; the console scripts' own shells)
//   - the scan's misses fixed: the hooked chooser found, the match scan once the spawn screen is set
//     up, 2D roots walked at +18h; each screen coming up in a match logged with its record (viewport,
//     elements, the root each is in) and that root (p2spawn_scan.cpp; spawn_ui_scan.cpp hands over
//     its checked PushScreen / PopScreen)
// PHASE 18S - PLAYER 2 SPAWN SCAN (everything from 18r still included):
//   - log only (p2spawn_scan.cpp, new; [p2spawn] lines): what player 2's spawn screen in split
//     screen needs from the exe - the two IF managers (which one each screen goes to, which
//     player's input each reads), whether the console screens exist for player 2 in split screen
//     (ifs_readyselect2 / charselect2 / mapselect2), the one IF root every menu screen hangs off
//     and its tree, player 2's HUD root (the spawn screen's 3D soldiers and texts go into player
//     1's today), player 2's map and status displays, and the bytes of every player-1 site in the
//     spawn screen's code (the chooser's manager reads and "ifs_readyselect%i", 1; the setup's
//     root; GetSpawnDisplay(player) ignoring its player). The PushScreen / PopScreen lines now
//     name the manager (spawn_ui_scan.cpp); frame_capture.cpp: MenuViewportRead (read only)
// PHASE 18R - STEAM LIMITS FIX (everything from 18q still included):
//   - Steam: the heaps are raised any time before the game makes its main heap (main thread paused,
//     heap pointer still 0, not inside heap_main) - no longer only in the unlock gap, whose test read
//     the exe file's address as "fixed up" at low load addresses (0x00760000); the hi-res units at
//     the game's DirectInput call while the animator object does not exist; every value read back
// PHASE 18Q - MOUSE AND SOUNDS (everything from 18p still included):
//   - the game's own cursor (read where its hot-spot test reads it): click a soldier of the ring to
//     pick it, the front one / "SELECT CHARACTER" to spawn, BACK to go back; the side under the
//     cursor is picked; prompts turn yellow under the cursor; each sound's name resolved from a list
//     (the back sound plays); the ring wider
// PHASE 18P - CLASS WHEEL FINAL (everything from 18o still included):
//   - [ClassWheel] Enabled in the INI (console flow by itself, F6 cycles), mouse left / right = A / B,
//     real depth on the ring (weapons and droidekas no longer through the front soldier), the game's
//     interface sounds on select / back / tick, animation on pick, layout as the Classic Collection
// PHASE 18O - THE TEXTS (everything from 18n still included):
//   - texts show (the string is set after font / justify); "CHOOSE YOUR SIDE" and the side names,
//     the spawn point panels' texts (post name, vehicles, objective) with HINTS hidden, and the
//     "SELECT SPAWNPOINT" / "SELECT CHARACTER" / "SELECT SOLDIER" prompts
// PHASE 18N - DRAW ORDER AND DARK SIDE (everything from 18m still included):
//   - ring front drawn last (no more seeing through), the unpicked side dark, droidekas roll back
//     up when not selected, the map moved in whichever big layout it uses, side text diagnostic
// PHASE 18M - SIDE TEXT AND MAP (everything from 18l-r3 still included):
//   - side screen texts show, its soldiers no longer turn when picking, droidekas bigger on the
//     ring, the map moved left on the spawn point screen and off the screen elsewhere
// PHASE 18L - SIDE SCREEN, r3: no animation on the side screen at all
// PHASE 18L - SIDE SCREEN, r2: only the selected class animates on the class wheel
// PHASE 18L - SIDE SCREEN (everything from 18k still included):
//   - "CHOOSE YOUR SIDE": each side's first class side by side with the side's name, the picked
//     side lit, the other dark; left / right pick, A joins with the game's own SelectTeam
// PHASE 18K - CLASSIC LOOK 1 (everything from 18j still included):
//   - the ring laid out as the Classic Collection has it, no soldier on the spawn point screen,
//     log-only scan of the teams and the spawn screen's text elements for the side screen
// PHASE 18J - CONSOLE ORDER (everything from 18i-r2 still included):
//   - side -> spawn point -> class (A on the soldier select spawns; respawns start at the spawn
//     point screen; F6 cycles in that order), and the ring's soldiers play their idle animations
// PHASE 18I - THE WHEEL TURNS, r2: the CIS crash (droidekas now get their weapon like ShowClass
//   gives it) and the double model (the chooser's every-frame show-soldier call keeps the game's
//   soldier hidden while the ring shows it)
// PHASE 18I - THE WHEEL TURNS (everything from 18h-r3 still included):
//   - one ring element per class (selected one included; the game's own soldier hidden meanwhile),
//     the ring eases round to the selected class, droidekas are on it, , . move its centre
// PHASE 18H - WHEEL RING, r3: the real crash - the console texts' elements were kept from the
//   last match (the new spawn screen had the same address); now looked up every frame, and
//   nothing is touched until the spawn screen is set up; the r2 delete hook follows the jump thunk
// PHASE 18H - WHEEL RING, r2: quitting a match with the ring up no longer leaves its soldiers behind
//   (the spawn screen's deleting destructor deletes them first; the GCW crash), and the ring's
//   soldiers keep the game's position factor (+84h) and get smaller with +80h (they were too big)
// PHASE 18H - WHEEL RING (everything from 18g still included):
//   - the soldier select shows the team's other classes around the selected one, in a ring, each
//     made from the game's own soldier element with the spawn screen's own calls; deleted when the
//     soldier select is left; [ ] / - = tune the ring while on it (classwheel_scan.cpp)
// PHASE 18G - COMMAND POST CHOICE (everything from 18f still included):
//   - spawn point screen: left / up and right / down pick the previous / next command post your
//     team can spawn at, through the game's own NextSpawnPoint and SetSpawnPoint; the spawn point
//     panel's title shows the chosen post's name for now (classwheel_scan.cpp)
// PHASE 18F - CONSOLE FLOW (everything from 18e still included):
//   - 1 player, F6 enters it from the PC spawn screen: side -> soldier -> spawn point -> spawn,
//     on the game's own console screens. Pad (first XInput pad) or keyboard: left / right = class
//     (the game's own SelectClass), A / Enter = on, B / Backspace = back, A on the spawn point
//     screen = the PC Spawn button's own calls. The PC screen's input function is skipped while a
//     console screen is up (its call in the spawn screen's update goes through BF1GameExt)
//     (classwheel_scan.cpp)
// PHASE 18E - CHARSELECT TEXT (everything from 18d still included):
//   - the console driver's first piece (classwheel_scan.cpp, run each frame on the game thread
//     from spawn_ui_scan.cpp's chooser hook): while the F6 preview shows ifs_charselect1, its
//     equipment window shows the selected class's name and "EQUIPMENT:" + weapons (the game's
//     own texts, set with its own text setter); PageUp / PageDown pick the previous / next class
//     through the game's own SelectClass
// PHASE 18D - CONSOLE SCREEN PREVIEW (everything from 18c still included):
//   - F6 while choosing on the spawn screen (1 player) cycles the screen the game's chooser
//     shows: the PC screen -> ifs_readyselect1 -> ifs_charselect1 -> ifs_mapselect1 -> the PC
//     screen - the console screens as the PC game has them, for screenshots
//     (classwheel_scan.cpp, spawn_ui_scan.cpp). Split screen keeps SplitConsoleSpawn as it was
// PHASE 18C - CONSOLE SCREEN NAMES (everything from 18b still included):
//   - log only (classwheel_scan.cpp): 18b found the element registry (1620 elements) but no
//     screen name in it - screens have their own list. Now the screen list is read, the paths
//     the exe itself looks up are checked, the word search starts from each screen name, and
//     every "ifs_..." text in the game's memory is hashed (whole and up to each '.') to name the
//     screens and elements that exist
// PHASE 18B - CONSOLE SCREEN SCAN (everything from 18a still included):
//   - log only (classwheel_scan.cpp): reads the game's IF element registry (every interface
//     element by the hash of its full path): which console screens exist on PC (ifs_readyselect,
//     ifs_charselect, ifs_mapselect, 1 and 2) and the names of their elements, found by trying
//     words after each path; ifs_pc_SpawnSelect named the same way as the check. 18a's memory
//     search removed (answered), spawn screen timers no longer logged
// PHASE 18A - CLASS WHEEL SCAN (everything from 17e still included):
//   - log only (classwheel_scan.cpp, new; [wheel] lines): the PC spawn screen object
//     (SpawnDisplay) found by pattern and its fields watched - mode, team shown, selected class
//     index, character; each team's class list (classes, AddUnitClass numbers, extra classes);
//     one read-only memory search for the console spawn screens' scripts (ifs_charselect,
//     ifs_mapselect, ifs_sideselect, ifs_readyselect) - for the Classic Collection's three
//     screens (side, soldier wheel, spawn point). spawn_scan.cpp: SpawnScanManager getter
// PHASE 17E - RAGDOLL WALLS (everything from 17d still included):
//   - walls / railings / pillars / ceilings next to ragdolls from the game's
//     ray test, as solid slabs (ragdoll.cpp, ragdoll_body.cpp AddWall); scans
//     for the real collision meshes' triangles and the droid death code. INI
//     [Ragdoll] Walls
// PHASE 17D - STUTTER FIX (everything from 17c still included):
//   - ragdoll builds: no memory check per floor ray (17c: 50 of 51 ms), memory
//     checks remembered briefly, the death tracker's character lookup in one
//     go; droid scan part 2; mesh layout scan (ragdoll.cpp, spawn_scan.cpp)
// PHASE 17C - RAGDOLL SCANS (everything from 17b still included):
//   - deaths in the air get ragdolls; seated deaths named; the build stutter
//     timed (terrain calls skipped where there is none); droid scan (bodies
//     never drawn after death); wall scan (props' collision data, log only)
//     (ragdoll.cpp). INI [Ragdoll] WallScan
// PHASE 17B - FLOORS (everything from 17a still included):
//   - ragdolls on the real floor: the game's own ray test fills the ragdoll's
//     ground (platforms, walkways, interiors - Bespin, Cloud City) and decides
//     "is he on the floor?"; bodies fall off edges (ragdoll.cpp). INI [Ragdoll]
//     Floors, FloorStep; RayScan off by default
// PHASE 17A - RAY SCAN (everything from 16b still included):
//   - log only: the game's world ray test (bullets, camera, explosions, AI) is
//     passed through a logging hook, to learn how to ask it where the floor is
//     under a ragdoll - for Bespin Platforms / Cloud City (ragdoll.cpp). INI
//     [Ragdoll] RayScan
// PHASE 16B - LIMITS ON (everything from 16a still included):
//   - BF1MemExt's limits on GOG 1.3 and Steam (limits_scan.cpp), each set an
//     INI switch ([Limits]): heaps 64 -> 128 MB (+ debug, application), hi-res
//     units 10 -> 100 (+ render pools), matrix pool 3062 -> 50000, DLC missions
//     512 -> 4096. Heaps and hi-res units before the game starts (GOG: at load;
//     Steam: after the unlock, before the address fix-ups); matrix pool and
//     DLC list with the other scans, on the game's thread.
// PHASE 16A - LIMITS SCAN (everything from 15j still included):
//   - log only: finds and checks every place BF1MemExt's memory limits live
//     in the 2020 code (heaps, hi-res units, matrix pool, DLC missions) on GOG
//     1.3 and Steam, and times whether the main heap could still be raised
//     (limits_scan.cpp, new). Nothing is changed in the game.
//   - r2: the early heap timing waits for the heap code itself (Steam unlocks
//     the code in stages: r1 looked too soon) and times when the game makes
//     its main heap
// PHASE 15J - PLAYER RAGDOLLS (everything from 15i still included):
//   - a local player's own death gets a ragdoll (it was measured from the
//     other / a stale player soldier and always skipped); tracker names fixed
//     (ragdoll.cpp)
// PHASE 15I - DEATH TRACKER (everything from 15h still included):
//   - log only: every death and throw - who, what killed him, ragdoll or the
//     exact reason not, drawn in full detail or not ([deaths] lines,
//     ragdoll.cpp); INI [Ragdoll] DeathTracker
// PHASE 15H - RAGDOLL JOINTS + GUN DROP + GETTING UP (everything from 15g still included):
//   - physics ragdoll_body.cpp p15h-r3 (tried in the ragdoll lab first): the
//     waist, neck, hips and shoulders are limited to BF1's own animations (no
//     more over-twisted bodies), fitted torso / head / arm capsules, muscles
//     let go once the death animation stops, a nearly still body is calmed
//     (no leg twitching), the gun can fall as its own body
//   - WeaponDrop 0 / 1 / 2: in the hand / falls from the hand (default) / the
//     death animation's throw (ragdoll.cpp)
//   - thrown survivors blend into the game's getting-up animation as he
//     starts getting up, put onto where the ragdoll lies (ragdoll.cpp)
// PHASE 15G - LIVE TUNING + LAB FILE (everything from 15f still included):
//   - [Ragdoll] reloads by itself when BF1GameExt.ini or the live file
//     Documents\BF1GameExt\ragdoll_live.ini changes (checked every 0.5 s, read
//     once the change has settled); the live file's keys win. Enabled works
//     live too (0 ends running ragdolls). A reload logs the keys that changed.
//   - Documents\BF1GameExt\ragdoll_lab.txt: settings in force, ragdoll
//     skeletons and the start data of the first 60 ragdolls, for the ragdoll
//     lab page (which runs ragdoll_body.cpp compiled to WebAssembly)
//   - WeaponDrop (INI, default 1): a death ragdoll's gun follows the death
//     animation (BF1's clips throw it 2-3 m) instead of the ragdoll's hand;
//     the weapon draw site is checked at startup (log-only)
//   - the lab file also records the game's animated pose of the first 10
//     ragdolls (A lines), for replays with the real animation
//   - fix: ragdoll.cpp logged its build as p15c-r1 since 15c (ragdoll.cpp)
// PHASE 15F - ANIMATION BLEND + HITS (everything from 15e-r2 still included):
//   - the ragdoll's muscles follow the game's death / thrown animation, fading
//     to a little (AnimKeep); the body blends in from the animation (BlendIn)
//   - headshot punch (zone 2 hits: HeadshotPunch at the head, half on the
//     chest), body-hit push (HitPush), blast tumble on deaths, bullets jolt
//     ragdolls (CorpseHit); bodies over terrain cut-outs are left animated
//   - fix: hips / shoulders / waist / neck muscles held a slightly wrong pose
//     (Bullet body-space motor target) - now constraint space
//   - seat scan, log-only ([seat] lines): seated soldiers near a player
// PHASE 15E - THROWN RAGDOLLS + LIVE TUNING (everything from 15d still included):
//   - soldiers thrown by explosions near a local player (players too) become
//     ragdolls at once, under the game's thrown gravity, with a tumble; if
//     the game then sets them dead they stay ragdolls, survivors blend back
//     into their animation; explosions push bodies that are already ragdolls
//   - rolling friction (bodies no longer roll downhill like logs)
//   - every physics setting in INI [Ragdoll]; F7 reloads them, F8 = slow motion
//   - the damage hook's check accepts Steam's code (it was not installed)
//   - the previous session's log is kept as BF1GameExt_split.prev.log (the
//     15d crash's log had been overwritten by the next start)
//   - r2: crash fix - the damage hook read 3 floats at 0xFFFFFFFF (an
//     argument of -1) because ragdoll.cpp's Readable() let a range wrap past
//     4 GB; it now refuses wrapping ranges (ragdoll.cpp)
// PHASE 15D - SELF-COLLISION + HIT SCAN (everything from 15c still included):
//   - ragdoll parts collide with each other (pairs overlapping in the death
//     pose wait until they are apart), and the joints hold the death pose with
//     muscle tone that fades out over 0.4 s (ragdoll_body.cpp)
//   - log-only hit scan (INI [Ragdoll] HitScan=1, [hit] lines): every push
//     (knockback) and damage call on soldiers near a player, and pushed
//     soldiers' states / animation for 8 s - for thrown ragdolls, rocket push
//     and headshot punch next (ragdoll.cpp; two more checked vtable slots)
// PHASE 15C - FIRST RAGDOLL (everything from 15b still included):
//   - INI [Ragdoll] Enabled=1: a soldier who dies on terrain (not on an
//     object) within Range metres of another local player falls as a Bullet
//     ragdoll - 11 capsules, 10 joints, the terrain under him sampled from the
//     game - instead of playing the death animation. Built at his first draw
//     after death; his bone matrices are replaced after each animation update
//     until the body is gone (ragdoll.cpp + ragdoll_body.cpp, new). The 15b
//     scan's logging is off unless [Ragdoll] Scan=1
// PHASE 15B - RAGDOLL SCAN (everything from 15a still included):
//   - log-only (ragdoll.cpp, INI [Ragdoll] Scan / ScanRange, [ragdoll] lines):
//     the soldier render's two calls to the animation controller update go
//     through BF1GameExt (the game's update runs first, then its bone matrices
//     are read); the soldier's death function and its delete go through
//     checked EntitySoldier vtable slots. Logged: the skeletons, a model-space
//     / terrain check on the players' soldiers, every death, the pose of bodies
//     near the players after death, and when bodies are removed
// PHASE 15A - BULLET TEST (everything from 14f still included; sprint is final):
//   - Bullet Physics 3.25 (zlib license) is compiled into the DLL from its three
//     single-file builds (bullet3\src). At startup a private Bullet world drops
//     a 1 m box onto a ground plane and logs that it lands and rests at y = 0.5:
//     the first step towards ragdolls. Nothing in the game is touched
//     (ragdoll.cpp, INI [Ragdoll] BulletTest, [ragdoll] lines)
// PHASE 14F - RUN ANIMATION AT SPRINT SPEED:
//   - the soldier's move animation selector (GOG 0x00613680) gives the
//     standing pose to any soldier moving faster than 8 m/s; its cap now reads
//     INI [Sprint] AnimSpeedLimit (default 14 m/s), so the run animation
//     plays - faster with the speed - while sprinting (sprint.cpp)
//   - the 14e animation scans are off unless [Sprint] AnimScan=1 / AnimWatch=1
// PHASE 14E - RED REFILL + ANIMATION SCAN:
//   - the stamina bar is red while it refills after running out, green again at
//     StaminaMinSprint (stamina_hud.cpp)
//   - log-only: per player, the soldier fields that differ between normal
//     running and sprinting ([anim] lines, sprint.cpp) - the sprint shows a
//     standing pose instead of the run animation
//   - r2: log-only write watch (CPU data breakpoints on player 1's animation
//     channel fields +0x5E0 / +0x4BC for 25 s) to find the code that picks
//     the standing animation, plus channel state changes with the speed
//   - r3: those fields were contact lists, not animation; the watch now lists
//     every place that READS the soldier's velocity (+0x8CC / +0x8D4, 15 s)
//   - r4: r3 crashed when the watch armed (its handler read the watched value,
//     re-firing the read breakpoint inside itself); the handler no longer
//     touches it. INI [Sprint] AnimWatch=0 turns the watch off
//   - r5: deep animation scan (no debug registers): the run / sprint diff also
//     runs inside every block the soldier points at; AnimWatch defaults to 0
// PHASE 14D - STAMINA BAR:
//   - the green stamina bar under the grenade icon: the grenade panel's own
//     hidden bar (the jet fuel bar art) is shown after the game's HUD update
//     whenever the game is not using it, cut to the stamina left, flashing
//     below the minimum; put back exactly before the next update.
//     INI [Sprint] StaminaBar / StaminaBarX / Y / Width / Colour / LowColour
//     (stamina_hud.cpp, StatusDisplay::Update through a checked vtable slot)
// PHASE 14C - STAMINA + HUD SCAN:
//   - stamina with Battlefront II's own defaults (drain 20/s, regen 10/s moving,
//     15/s still, max 100), 20 needed to start, boost ramps up over 0.5 s;
//     INI [Sprint] Stamina* / SprintAccelTime (sprint.cpp)
//   - log-only HUD scan: each player's StatusDisplay elements (positions,
//     colours, bitmaps) 3 s after a spawn, for placing the stamina bar under the
//     grenade icon (stamina_hud.cpp, [hud] lines)
//   - r2: the HUD scan's memory check walks every region (on Steam r1 found
//     .data / .rdata "not readable" and switched itself off)
// PHASE 14B - L3 SPRINT ONLY + P2 THIRD PERSON:
//   - the test boost is forward only (14a: forward input is positive)
//   - INI [Sprint] L3SprintOnly=1: L3 no longer reaches the game's "view"
//     toggle (pad bit -> action table, action 6 -> an unused bit), both players
//   - INI [Splitscreen] P2ThirdPerson=1: whenever player 2's soldier is in first
//     person, the game's own SetFirstPerson(view, index, 0) switches it back
//     (sprint.cpp, [view] lines)
// PHASE 14A - SPRINT SCAN:
//   - the soldier's run speed (MaxSpeed, in the class, shared with the AI) is
//     only read by EntitySoldier::Move, which every soldier calls each frame
//     with its forward / strafe inputs. Move's entry goes through BF1GameExt:
//     player 1's and player 2's soldiers are logged (class speeds, state,
//     inputs, measured speed), the sprint button (L3; Left Shift for player 1)
//     is watched, and INI [Sprint] TestBoost=1 multiplies the forward input by
//     SpeedMult while it is held and the soldier is standing (sprint.cpp)
// PHASE 13G - SPAWN MAP IN THE BOX:
//   - while player 1's PC spawn screen is up, player 1's own HUD layer (the
//     command post map, the score bar) gets the same 16:9 box as the menu layer,
//     so it lines up with the panels. Player 2's HUD never moves. INI [Display]
//     SplitSpawnMapFit=1 (spawn_ui_scan.cpp + frame_capture.cpp)
// PHASE 13F - MENU IN PLAYER 1'S HALF:
//   - the frame-interface function draws each player's HUD in the view loop,
//     then the global interface root (every menu screen) once after it, with the
//     viewport still on player 2's half. That call now goes through BF1GameExt:
//     in split screen, while player 1's menu stack is not empty, the viewport is
//     player 1's box for it. INI [Display] SplitMenuFit=1 (16:9 box in player 1's
//     half; 2 = whole half; 0 = off) (spawn_ui_scan.cpp + frame_capture.cpp)
// PHASE 13E - MENU DRAW CHAINS:
//   - two IF managers exist (one per player, [0x838270] + i*244h); each lays its
//     screens out for its own viewport (+18Ch), but the menus are DRAWN at the
//     end of the frame while the D3D viewport is still player 2's. Each captured
//     frame now lists the draws after the last viewport change by deep call
//     chain (frame_capture.cpp), to find where menu drawing starts
// PHASE 13D - SPAWN SCREEN CAPTURE:
//   - a frame of the PC spawn screen is captured by itself 1.5 s after it comes
//     up in split screen (at most twice), to find the code that draws menus
//     (spawn_ui_scan.cpp)
// PHASE 13C - CONSOLE SPAWN SCREEN:
//   - 13b forced mode 2, which means "Spawn pressed": player 1 spawned at once.
//     Now: INI [Display] SplitConsoleSpawn=1 - while player 1 is choosing in
//     split screen, the chooser shows "ifs_readyselect1" (the console per-player
//     screen, made for split screen + pad) instead of "ifs_pc_SpawnSelect"
//     (spawn_ui_scan.cpp)
// PHASE 13A - SPAWN SCREEN SCAN:
//   - log-only: every PushScreen / PopScreen, the spawn-screen chooser (mode 1 =
//     PC ifs_pc_SpawnSelect, mode 2 = console ifs_readyselect1), the menu stack
//     in split screen, and a one-time read-only memory search for the console
//     spawn screen's scripts (spawn_ui_scan.cpp)
// PHASE 12I - FRAME CLEAR FIX:
//   - INI [Display] SplitFrameClearFix=1: the game's frame-start Clear only
//     covered the leftover viewport (player 2's half); after each Present the
//     viewport goes back to the whole target in split screen, so player 1's
//     half gets cleared too - no stale pixels / trails (frame_capture.cpp)
// PHASE 12H - STENCIL FIX:
//   - INI [Display] SplitStencilFix=1: in split screen the shadow-volume setup's
//     final COLORWRITE 7 becomes 0 - the volumes only mark the stencil, so
//     High stencil shadows work without the coloured wedges (split_shadows.cpp)
// PHASE 12G - SPLIT SCREEN BLUR + STENCIL DUMP:
//   - INI [Display] SplitScreenBlurOff=1: the screen blur's final blend-back
//     is skipped in split screen (half the picture stretched 2x); the pass
//     itself keeps refreshing the effects layer (r2: r1 skipped the whole pass
//     and froze that layer - trails on holograms) (split_shadows.cpp)
//   - captures dump every stencil-on draw group in both views once, with the
//     full stencil / blend state (frame_capture.cpp)
// PHASE 12F - SWAP-PROOF CAPTURE:
//   - Direct3D swaps its own state setters into the device's per-device method
//     table at runtime (12e health check: at 17 s), so the six state setters
//     are wrapped only around a captured frame, after reading the device's
//     current state; Direct3D's functions go back right after (frame_capture.cpp)
// PHASE 12E - SPLIT MOTION BLUR:
//   - INI [Display] SplitMotionBlurOff=1: the per-player motion blur draw is
//     skipped in split screen (it showed the other player's picture, stretched
//     2x); single player keeps motion blur (split_shadows.cpp)
// PHASE 12D - SPLIT SHADOWS:
//   - INI [Display] SplitShadowsLow=1: while two local players are active the
//     stencil-shadow gate answers "no", so High shadows act as Low in split
//     screen (no coloured wedges); single player keeps High (split_shadows.cpp)
// PHASE 12C - HOOK HEALTH:
//   - every 5 s: calls each device wrapper received, and whether the device's
//     method table still points at the wrappers (and if not, which DLL took the
//     slot); at install, which modules hold the device, its table and the
//     original methods - because 12b's constant dumps never changed
// PHASE 12B - SHADOW CONSTANT DUMP:
//   - shader constants, shaders and render-target / viewport / view are
//     tracked; captured frames dump them ([cdump] lines) at every shadow and
//     blur draw and at the first ordinary draw of each player's view, to see
//     whether player 1's shadow pass runs on player 2's camera numbers
// PHASE 12A - SHADOW + MOTION SCAN:
//   - SetRenderState wrapped on the device: states set on Direct3D directly
//     (bypassing the game's state copy) are logged, and so is every stencil
//     (shadow) draw where the game's copy and Direct3D disagree; captures show
//     stencil / colour-write / z-write per draw group and label shadow draws;
//     Num / keeps the game's copy in sync (candidate fix), Num - switches blur
//     and motion blur off / on (frame_capture.cpp)
// PHASE 11F - SCREEN-COPY SCAN:
//   - textures that receive screen copies or are render targets are tracked;
//     captured frames flag every draw group that samples one, and Num *
//     switches all such draws off / on (frame_capture.cpp)
// PHASE 11E - DRAW-GROUP SWITCHES:
//   - Num 1-9 switch off the draws of one game function each (Steam), Num 0
//     all back on; 11d's effect-type switches are retired (they could not reach
//     effects drawn later by the generic renderer), its labels stay
// PHASE 11D - EFFECT SCANNER:
//   - effect types (motion blur, blur, god rays, light beams, sun / lens flare,
//     command post / hologram, heat shimmer / refraction, screen filter) found
//     by their RTTI names; captured frames label every group of draws with its
//     type; Num 1-8 switch one whole type off / on each (with a beep), Num 0
//     turns all back on, LB+RB+X cycles (frame_capture.cpp, r2). Replaces the
//     11c address list.
// PHASE 11C - EFFECT FINDER:
//   - the 11b effects fix is now off by default (INI SplitEffectsFix=0)
// PHASE 11B - SPLIT EFFECTS FIX:
//   - INI [Display] SplitEffectsFix=1: two effect passes keep the player's
//     viewport when they switch to the full-screen effects target, so their
//     draws land in that player's half (effects_fix.cpp)
// PHASE 11A - FRAME CAPTURE:
//   - log-only wrappers on 6 Direct3D device methods; one whole frame's screen
//     copies, viewports and render targets are captured 3 times by themselves
//     in a match (r2), or on Insert / Home / LB+RB+Y (frame_capture.cpp)
// PHASE 10B - PRONE FIX:
//   - INI [Controls] DoubleTapMs=600: a double-tap (crouch -> prone) needs the
//     second press within the window; a stale tap no longer counts (prone_fix.cpp)
// PHASE 10A - PRONE SCAN:
//   - read-only watch of player 1's 12 button states every frame: presses,
//     releases, double-taps and game-made event changes (prone_scan.cpp)
// PHASE 9B - LOADING SCREEN FILL:
//   - INI [Display] LoadScreenFill=1: the native loading screen fills a wide
//     display instead of a 4:3 box (loadscreen.cpp)
//   - the 9a Lua scan is retired (the loading screen is not a Lua script); its
//     files stay in the project but are not installed
// PHASE 9A - LOADING SCREEN SCAN:
//   - log-only wrappers on the interface Lua calls (load_scan.cpp)
// PHASE 8A - VERTICAL SPLIT:
//   - INI [Splitscreen] Vertical=1: side-by-side views, with the cameras sized
//     for the halves (vertical.cpp)
// PHASE 7C - PLAYER 2 CONTROLS:
//   - copies player 2's control packet into control state 1 every frame, which
//     the game only does for player 1 in a local game (player2_input.cpp)
// PHASE 7B - AUTO-SPAWN PLAYER 2:
//   - presses player 2's Spawn button through the game's own SetReady while
//     player 1 is in the world and player 2 is not (autospawn.cpp)
// PHASE 7A - SPAWN SCAN:
//   - 2020 build: read-only watch of player 1's and player 2's characters on
//     the SpawnManager, logging every field that changes (spawn_scan.cpp)
// PHASE 6C - PLAYER 2 REDIRECT:
//   - the input manager's per-player getters (vtable +0x18/+0x1C/+0x20) give
//     player 2's objects for index 1; F11 toggles it (player2_input.cpp)
// PHASE 6B - PLAYER 2 INPUT OBJECT:
//   - 2020 build: builds player 2's input object (player2_input.cpp) and drives
//     pad 2 and player 2's input from the game's frame through one call-site
//     hook. The game does not read player 2's input yet.
// PHASE 6A - PAD 2:
//   - builds a second pad object with the game's own pad code (pad2.cpp)
// PHASE 5 - PLAYER 2 INPUT SCAN:
//   - 2020 build: logs the game's single pad, the per-player active pad table
//     and the player input object, plus every physical XInput pad (input_scan.cpp)
//   - detects the exe (game_build.cpp): GOG 1.2, GOG 1.3 or Steam
//   - Steam: waits until Steam has unlocked the game code in memory, then finds
//     every site there (runtime_resolve.cpp); never removes or bypasses the DRM
//   - GOG 1.2 only: loads BF1MemExt.dll at the game's entry point
//     (memext_loader.cpp), so Battlefront.exe never has to be patched on disk
//   - forwards DirectInput8Create to the real system dinput8
//   - runs the read-only scan (scan.cpp)
//   - logs crashes (crashlog.cpp)
//   - split screen test (split_test.cpp): F9 / LB+RB+Back in the menus turns
//     split ON (2 local players, sticky), F10 / LB+RB+Start turns it OFF, and
//     the game's split screen Lua functions are logged as [lua] lines
//   - controller scan (controller_scan.cpp): logs BF1's binding tables, the
//     profile's joystick flag and the local player slots, then every change
//
// Code hooks in this phase: 1 (the entry point, restored before the game starts).
// Game data changed: 4 Lua registration slots (logging wrappers), and the
// local player count when you press F9/F10.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
#include <cstdio>
#include <cstring>

#include "crashlog.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "limits_scan.hpp"
#include "memext_loader.hpp"
#include "scan.hpp"
#include "split_test.hpp"
#include "controller_scan.hpp"
#include "input_scan.hpp"
#include "pad2.hpp"
#include "player2_input.hpp"
#include "spawn_scan.hpp"
#include "autospawn.hpp"
#include "vertical.hpp"
#include "load_scan.hpp"
#include "loadscreen.hpp"
#include "effects_fix.hpp"
#include "frame_capture.hpp"
#include "prone_fix.hpp"
#include "split_shadows.hpp"
#include "spawn_ui_scan.hpp"
#include "classwheel_scan.hpp"
#include "p2spawn_scan.hpp"
#include "playern_scan.hpp"
#include "prone_scan.hpp"
#include "ragdoll.hpp"
#include "sprint.hpp"

#define BF1_PHASE "PHASE 21J R31 - BF1GAMEEXT_PAUSE34.LUA P21J-R31 (P21I-R10 WITH ITS BUILD STRING; THE OLD P21H-R10 COPY NEXT TO THE EXE REPLACED BY CONSOLE.LVL'S); R30 - CONSOLE.LVL FROM THE MOD TOOLS (MUNGECONSOLE.BAT: SCRIPTMUNGE + LEVELPACK, THE SCRIPTS COMPILED AS THE SHELL'S OWN) - READ AS R29'S, A COMPILED SCRIPT'S BUILD FROM ITS GBFX_BUILD STRING; R29 - CONSOLE.LVL: THE SCRIPTS FROM GAMEDATA\\DATA\\_LVL_PC\\CONSOLE.LVL (MADE WITH CONSOLEMUNGE: THE GAME'S OWN .LVL SCRIPT CHUNKS) FIRST, THEN THE .LUA FILES NEXT TO THE EXE, THEN THE COPIES BUILT IN - EACH SCRIPT'S LOG LINE SAYS WHICH; R28 - THE SIGN-IN'S SOUND AND MUSIC BACK (SIGNINSOUNDFIX: EACH PROFILE SWITCH IN THE SHELL RESTARTS THE SOUND ENGINE AND DROPS THE SHELL'S SOUNDS, MUSIC AND MOVIE STREAMS - THE SIGN-IN'S PROFILE LIST AND ITS LAUNCH NEVER PUT THEM BACK; NOW THE GAME'S OWN RESTORE RUNS, AS ITS LOGIN DOES); THE AUDIO SCANNER ALSO SHOWS THE MUSIC'S STREAM AND THE ENGINE RESTART; R27 - THE SLIDER NUMBERS FIXED (THE OPTIONS SCREENS' \"40%\" WAS TURNED INTO THE NUMBER 4 BY BF1GAMEEXT'S OWN LUA TRACE - DRAWN AS \"4@\", \"6,\", \"|\"); THE SIGN-IN'S BACK / ACCEPT WITH THE PAD'S B / A (SIGNINPADICONS), ITS BACK BUTTON WORKS WITH THE MOUSE (SIGNINMOUSEBACK); THE AUDIO SCANNER (LOG ONLY: [audio] LINES - THE MENUS' SOUND / MUSIC CALLS, THE SOUND ON / OFF, THE SHELL MUSIC, THE VOLUMES, THE BUSES - AUDIOSCAN); R26 - THE FREE CAMERA ON THE PAD (FREECAMPAD: R3 UP, L3 DOWN - NOT RT / LT; THE RIGHT STICK LOOKS; X STOPS / STARTS THE CAMERA); R25 - START / ESC IN THE FREE CAMERA OPEN THE PAUSE MENU AGAIN (THE GAME'S OWN START DID NOT PAUSE THERE - FREE CAMERA LEAVES THE WORLD FROZEN, AND THE GAME NEVER PAUSES A FROZEN WORLD - SO CHASE CAMERA COULD NOT BE PICKED; [freecam] LINES); R24 - THE FAKE CONSOLE'S COMMANDS NO LONGER CRASH THE GAME (THE GAME'S OWN SWITCHED THE COMMAND, THEN PRINTED IT TO A DEBUG CONSOLE THE 2020 EXE NEVER MAKES; BF1GAMEEXT SWITCHES THEM NOW, EACH ROW SAYS ON / OFF); FAKE CONSOLE RIGHT UNDER FREE CAMERA; R23 - FREE CAMERA AND FAKE CONSOLE BACK IN PLAYER 1'S PAUSE MENU (PAUSEFREECAM / PAUSECONSOLE: THE GAME'S OWN ITEMS, HIDDEN BECAUSE THE 2020 EXE CALLS ITSELF A FINAL BUILD; THE CONSOLE'S DEBUG COMMANDS - INVINCIBLE, UNLIMITED AMMO, FRAME RATE, COLLISION, HOLOGRAMS); THE FREE CAMERA SCAN (FREECAMSCAN, LOG ONLY: ITS UP / DOWN / SPEED CONTROLS AND DEBUG KEYS); R22 - THE TUTORIAL MOVIES FILL THEIR BLACK BOX AT 4:3 (TUTORIALSMOVIEFIT: THE 2020 GAME TOLD ITS SCRIPTS THE SCREEN IS 4:3, SO THE MOVIE SAT IN THE BOX'S TOP LEFT UNDER ITS FRAME); GALACTIC CONQUEST'S TUTORIAL MOVIE BEFORE ITS SETUP (TUTORIALSGC); THE CAMPAIGNS' COMMAND POST TUTORIALS BEFORE THEIR FIRST MISSION (TUTORIALSCAMPAIGN: CLONE WARS NABOO, GALACTIC CIVIL WAR TATOOINE); R21 - THE MAIN MENU'S TUTORIALS BUTTON BACK ON PC (TUTORIALSMENU: THE GAME'S OWN TUTORIALS MENU - 8 TUTORIALS FROM MOVIES\\TUTENG.MVS; ITS BACK WORKS WITH THE MOUSE - THE GAME'S OWN HIT A LUA ERROR THERE); R20 - THE SPAWN SCREENS' MOUSE READS THE GAME'S OWN CURSOR (R19 READ THE WINDOWS CURSOR, NOT THE GAME'S ARROW: SIDES AND POSTS WERE MISSED); R19 - THE SPAWN SCREENS' MOUSE AS THE CLASSIC COLLECTION'S (MOUSECLICKS: CLICK THE CLASS WHEEL LEFT / RIGHT OF THE PICKED SOLDIER TO TURN IT, SELECT CHARACTER TO SPAWN; CLICK A COMMAND POST ON THE MAP TO PICK IT, SELECT SPAWNPOINT TO GO ON; CLICK A SIDE TO JOIN IT); R18 - MODS IN THE MAIN MENU ONLY (NO MODS ROW IN ANY OPTIONS IN A MATCH), RAGDOLLS OFF BY DEFAULT (SPRINT ON); THE MAIN MENU'S GAME OPTIONS SPAWN SCREEN ROW: CLASS WHEEL / PC MENU (SINGLEPLAYER ONLY - SPAWNSCREENMENU, [CLASSWHEEL] ENABLED, CLASS WHEEL BY DEFAULT); R17 - THE MODS ROW IN OPTIONS (MODSMENU: SPRINT ON / OFF AND RAGDOLL EFFECTS ON / OFF - AT ONCE, SAVED IN THE INI; ADDON FOLDER: EVERY FOLDER IN GAMEDATA\\ADDON WITH THE MISSIONS IT ADDS, HOW MANY MODS ARE INSTALLED, THE ADDON MISSIONS USED OF THE LIMIT - 4096 RAISED, 512 THE GAME'S OWN); R16 - THE PAUSE MENU NOISE FIXED (R15'S PLAYER 2 SCRIPT REPLACED THE PAUSE SCAN'S BFXP_ENTER: EVERY PAUSE MENU ENTER RAN THE GAME'S ENTER OVER AND OVER IN ONE FRAME - ITS SOUND EACH TIME, ALL AT ONCE; NOW BFX2_*); THE MOUSE CURSOR OFF PLAYER 2'S HALF (CURSORP1ONLY: HIDDEN WHILE PLAYER 1 HAS NO MENU UP AND PLAYER 2 IS NOT PAUSED); R15 - PLAYER 2'S PAUSE MENU AS PLAYER 1'S IN THE 2-PLAYER SPLIT (PAUSELIKEP1P2: NO VIDEO OPTIONS, CONTROL OPTIONS = THE GAMEPAD PAGE, HIS OPTION SCREENS IN A 16:9 BOX OF HIS HALF, HIS MENU AT PLAYER 1'S PLACE, PLAYER 1'S SOUNDS); THE 3-PLAYER LAYOUT: PLAYER 1'S OPTION SCREENS IN HIS QUARTER (MENUBOXP1); THE 2-PLAYER STATS OVER THE WHOLE SCREEN (ENDSTATSFULLSCREEN=2); 4 PLAYERS: PLAYER 1'S WINDOW BARS WHOLE AGAIN, PLAYERS 3 / 4 ON PLAYERS 1 / 2'S END SHOTS (ENDSHOTP34); R14 - THE GLYPH LEAK FIXED (GLYPHLEAKFIX: A DELETED TEXT GIVES ITS LETTERS' CHUNKS BACK TO THE GLYPH POOL - THE GAME'S DESTRUCTOR DROPPED THEM, SO THE POOL RAN DRY SOONER OR LATER; [glyphs] LINES COUNT THEM); R13 - THE SCRAMBLED TEXT FIXED (GLYPHPOOLGROW: THE GAME'S TEXT GLYPH POOL - 1280 CHUNKS FOR EVERY TEXT AT ONCE - GETS MORE OF BF1GAMEEXT'S OWN WHEN IT RUNS LOW; [glyphs] LINES); THE 3 / 4-PLAYER SPAWN WINDOWS FURTHER APART; R12 - THE TEXT GARBLE SCAN (LOG ONLY: [vbscan] LINES - THE VERTEX BUFFERS TEXTS ARE DRAWN FROM: WRAPS MID-FRAME, STREAM MISMATCHES, FREED BUFFERS, REFERENCE COUNTS; END = MARK; TEXTGARBLESCAN); R11 - THE FRAME-RATE SCAN (LOG ONLY: [perf] LINES EVERY 5 S - FPS, FRAME TIMES, WHERE EACH FRAME'S TIME GOES ON THE GAME'S THREAD: THE GAME'S UPDATE / RENDER / POSES / HUD / MENUS, DIRECT3D'S DRAWS / PRESENT, BF1GAMEEXT'S OWN PARTS; SLOW FRAME LINES; PERFSCAN / PERFSPIKEMS); R10 - R9'S CRASH FIXED (AN AUTOMATIC FRAME CAPTURE READ PAST THE RENDER THREAD'S STACK: TYPEONSTACK STOPS AT THE STACK TOP NOW); R9 - THE CONSOLE WINDOWS ON THE 3 / 4-PLAYER SPAWN SCREENS (SPAWNPANELS34: COMMAND POST, VEHICLES, OBJECTIVES AND THE CLASS WINDOW IN THE GAME'S LOOK FOR PLAYERS 1 - 4, THE CLASS RING LEFT OF THE WINDOW); THE 1-PLAYER CLASS RING OFF THE CLASS WINDOW (RINGSHIFT -31); R8 - GALACTIC CONQUEST'S PICK YOUR SIDE IN THE CONSOLE'S LOOK (GCPICKLOOK: A ROW OF SIDE PICTURES PER PLAYER, A BOX PER PLAYER - COMMANDER / PLAYER READY IN HIS SIDE'S COLOUR; GCCOMMANDER2P: 2 PLAYERS LOCK IN TOO); R7 - THE DOUBLE BLUR FIXED (SCOPEBLURFIX: EVERY SPLIT VIEW READS THE WHOLE BLUR PICTURE, AS 1 PLAYER'S); PLAYERS 3 / 4'S RETICLE BOB FIXED (RETICLEFIXP34: PLACED WITH THIS FRAME'S CAMERA); R6 - BIGGER SCOPES IN HALVES (SCOPEHALFFILL), THE BLUR THERE IN PASSES; THE DOUBLE BLUR SCAN (LOG ONLY: [scope] LINES - EACH VIEW'S BLUR LAST QUAD, A DEVICE TRACE OF ITS DRAW); THE RETICLE SCAN (LOG ONLY: [reticle] LINES - PLAYERS 3 / 4'S RETICLE HEIGHT AGAINST THEIR CAMERA AS FRAMEWRAP SETS IT); R5 - THE ZOOM SCOPE FITS EACH SPLIT VIEW (SCOPEFIT: ITS CROSS ON THE CROSSHAIR, 1 PLAYER'S SCOPE AT THE VIEW'S SIZE, DARK BANDS AND THE BLUR IN A HALF); R4 - THE SCOPE SCAN (LOG ONLY: [scope] LINES - EACH PLAYER'S ZOOM SCOPE: HIS HUD ROOT, ITS SAFE AREA, THE SCOPE'S ANCHOR AND SIZE IN HIS VIEW, WHAT R5 WOULD SET; THE SCOPE BLUR'S VIEWPORTS); R3 - R2'S CRASH FIXED (THE GRASS STUBS NO LONGER OVERLAP); R2 - THE .PRP GRASS FADES IN AGAIN (GRASSFADEFIX: THE LAYER'S OWN DISTANCE AS ITS FADE DISTANCE, AS 1.2; THE SCAN'S BLINKS TELL MOVED PATCHES); R1 - THE GRASS SCAN (LOG ONLY: [grass] LINES - THE GRASS DRAW'S CALLERS, DISTANCES, MAXDIST AGAINST THE .PRP LAYER'S OWN DISTANCE, THE GROW BAND, BLINKS, THE NEAR SCENE RANGE); 21I R17 - THE FIELD OF VIEW SHOWS (THE CHASE CAMERA'S FOV SCALED AFTER THE UNIT ANSWERS IT, NO ZOOM, ONE PLAYER - AS THE CLASSIC COLLECTION); R16 - FIELD OF VIEW, SINGLE PLAYER ONLY (FOVSCALE 80 - 120 %, VIDEO OPTIONS' FIELD OF VIEW SLIDER - FOVMENU; SPLIT SCREEN KEEPS THE GAME'S 48 DEG); R15 - THE PAD CHANGES AN EXPERIMENT, OFF BY DEFAULT (PADEXPERIMENT=0: BF1'S OWN DEAD ZONE, LOOK CURVE, SENSITIVITY, GAMEPAD PAGE); R14 - THE GAMEPAD PAGE'S DEAD ZONE / LOOK CURVE ROWS (PADMENU, LIVE, SAVED), PADLOOKCURVE DEFAULT 2.0; R13 - THE PADS AS BATTLEFRONT II'S: PADDEADZONE (ROUND, RESCALED, DEFAULT 2 %), PADLOOKCURVE (DEFAULT 1.0 = A STRAIGHT LINE, BF1 3.32), PADSENSSTYLE (BF2'S SENSITIVITY); R12 - AUTO AIM ON WITHOUT THE AIM RESIST (AUTOAIMSLOWDOWN, DEFAULT 0: FULL LOOK SPEED OVER A TARGET), THE STRAFE FOLLOW TAKES AUTOAIMSTRENGTH; R11 - AUTO AIM OFF IS OFF (THE AIM ASSIST LEAVES AT ONCE: NO LOOK SLOW-DOWN), AUTOAIMSTRENGTH (THE PULL, DEFAULT 50% OF THE XBOX'S); R10 - AUTO AIM PER PLAYER (AUTOAIMPLAYERS: PLAYER 1 GAME OPTIONS, PLAYER 2 HIS PAUSE MENU'S GAME OPTIONS, PLAYERS 3 / 4 THEIRS); R9 - AUTO AIM FOR THE MOUSE / KEYBOARD PLAYER (AUTOAIMMOUSE, AS THE CLASSIC COLLECTION'S /MOUSEAIM); SPLIT SCREEN ON THE MAIN MENU'S GAME OPTIONS ONLY; R8 - GAME OPTIONS: SPLIT SCREEN HORIZONTAL / VERTICAL (SPLITMENU) AND AUTO AIM ON / OFF (AUTOAIMMENU); R7 - BACK FROM INSTANT ACTION'S GAME OPTIONS THE ERA BUTTONS STAY HIDDEN; NO HEROES ROW IN THE OPTIONS MENU (INSTANT ACTION PAGE ONLY); HEROES OFF BY DEFAULT; R6 - INSTANT ACTION'S GAME OPTIONS PAGE (X ON THE MAP LIST: AI UNITS PER TEAM, REINFORCEMENTS, HEROES, RESET - SAVED PER PROFILE; HEROES GREYED IN THE OPTIONS MENU); R5 - PLAYERS 2 - 4 SPAWN AGAIN AFTER A RESTART / DONE (THE END OF A MATCH CLEARED BY THE GAME'S GAME-OVER FLAG); R4 - THE BLEED SCAN AND THE INSTANT ACTION OPTIONS PAGE SCAN (LOG ONLY: [bots] / [iaopts] LINES); R3 - AIUnitsIA NEVER BELOW THE HUMAN PLAYERS (THE 0-UNIT CRASH); R2 - INSTANT ACTION'S AI UNITS PER TEAM / REINFORCEMENTS (AIUnitsIA / ReinforcementsIA); R1 - THE BOT / REINFORCEMENT SCAN FOR INSTANT ACTION (LOG ONLY, [bots] LINES); 21H R14 - NO BLACK SCREEN AFTER QUIT ON THE STATS SCREEN (THE STATS PAUSE ONLY ON A STATS SCREEN); R13 - RESTART MISSION ON THE STATS SCREEN PLAYS THE MAP AGAIN; R12 - QUIT ON THE STATS SCREEN GOES TO THE MAIN MENU, NO OPTIONS THERE; R11 - THE PAUSE MENU ON THE STATS SCREEN FIXED (START / ESC READ BY BF1GAMEEXT) AND IN EVERY MATCH (1 - 4 PLAYERS), NO RESPAWN THERE; R10; R9 - PLAYERS 3 / 4'S VICTORY / DEFEAT AS PLAYERS 1 / 2'S (THE GAME'S IMAGE), THE PAUSE MENU ON THE STATS SCREEN (QUIT TO THE MAIN MENU); R8 (THE END OF A MATCH AS THE XBOX'S), R7 (THE MATCH END SCAN, TABS), R6 (PAUSE ON SPAWN SCREENS, BUTTON REMAPPING), R5 (THEIR OPTIONS PAGES), R4, R3, R2 (REAL PAUSE MENUS FOR PLAYERS 3 AND 4), R1 AND 21G R4 INCLUDED; BUILT ON 20G R14"
#define BF1_BUILD "p21j-r31"

namespace {

typedef HRESULT(WINAPI* DI8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

INIT_ONCE g_loadOnce = INIT_ONCE_STATIC_INIT;
DI8Create_t g_realCreate = nullptr;
volatile LONG g_scanClaimed = 0;
volatile LONG g_di8Calls = 0;

BOOL CALLBACK LoadRealDinput8(PINIT_ONCE, PVOID, PVOID*)
{
    char sys[MAX_PATH];
    const UINT n = GetSystemDirectoryA(sys, MAX_PATH);
    if (!n || n >= MAX_PATH)
    {
        bf1::Log("ERROR: GetSystemDirectory failed (error %lu)", GetLastError());
        return TRUE;
    }

    char path[MAX_PATH + 16];
    snprintf(path, sizeof(path), "%s\\dinput8.dll", sys);

    const HMODULE h = LoadLibraryA(path);
    if (!h)
    {
        bf1::Log("ERROR: could not load the real dinput8.dll from %s (error %lu)", path,
                 GetLastError());
        return TRUE;
    }

    const FARPROC fp = GetProcAddress(h, "DirectInput8Create");
    if (!fp)
    {
        bf1::Log("ERROR: real dinput8.dll has no DirectInput8Create");
        return TRUE;
    }

    g_realCreate = reinterpret_cast<DI8Create_t>(reinterpret_cast<void*>(fp));
    bf1::Log("real dinput8 loaded: %s", path);
    return TRUE;
}

DI8Create_t RealCreate()
{
    InitOnceExecuteOnce(&g_loadOnce, LoadRealDinput8, nullptr, nullptr);
    return g_realCreate;
}

// Returns true once a scan has run (now or earlier). onGameThread: called from
// the game's own DirectInput8Create call (needed to install the frame hook).
bool TryScan(const char* trigger, bool onGameThread)
{
    if (InterlockedCompareExchange(&g_scanClaimed, 1, 0) != 0)
        return true;

    if (!bf1::CodeLooksUnlocked())
    {
        bf1::Log("scan requested by %s, but Steam has not unlocked the game code yet - will retry",
                 trigger);
        InterlockedExchange(&g_scanClaimed, 0);
        return false;
    }

    bf1::ScanRun(trigger, reinterpret_cast<void*>(RealCreate()));

    if (bf1::ScanSplitSitesOk())
        bf1::SplitTestStart();
    else
        bf1::Log("[split] DISABLED - exe identity or split screen byte-check failed");

    if (bf1::ScanIdentityOk())
    {
        bf1::ControllerScanStart();
        bf1::InputScanStart();
        bf1::Player2InputStart(onGameThread);
        bf1::SpawnScanStart();
        bf1::AutoSpawnStart();
        bf1::VerticalStart(onGameThread);
        bf1::LoadScreenFillStart(onGameThread);
        bf1::ProneFixStart(onGameThread);
        bf1::FrameCaptureStart();
        bf1::EffectsFixStart(onGameThread);
        bf1::SplitShadowsStart(onGameThread);
        bf1::SpawnUiScanStart(onGameThread);
        bf1::ClassWheelScanStart(onGameThread); // 18a: class wheel scan; 18f: console flow (after SpawnScanStart)
        bf1::P2SpawnScanStart(onGameThread);    // 18s: player 2 spawn scan; 18u: menu input fix + player 2 spawn screens (after SpawnUiScanStart, ClassWheelScanStart)
        bf1::SprintStart(onGameThread);
        bf1::RagdollStart(onGameThread); // 15a: Bullet test; 15b: ragdoll scan; 15c: ragdolls
        bf1::LimitsScanRun(onGameThread); // 16b: logs the early part; matrix pool + DLC list (game thread only)
        bf1::PlayerNScanStart(onGameThread); // 20a: 4-player map (log only); 20b: Views=4 - last: other parts' hooks go in first
    }
    else
    {
        bf1::Log("[controls] DISABLED - unknown exe");
    }
    return true;
}

// Backup in case the game never calls DirectInput8Create, or calls it while
// the code is still locked (Steam): retries once a second for another minute.
DWORD WINAPI FallbackThread(LPVOID)
{
    for (int i = 0; i < 240; ++i) // 60 s
    {
        Sleep(250);
        if (g_scanClaimed)
            return 0;
    }
    for (int i = 0; i < 60; ++i) // up to 60 more s
    {
        if (TryScan("fallback timer", false))
            return 0;
        Sleep(1000);
    }
    bf1::Log("FALLBACK: the game code still looked locked after 120 s - scan skipped");
    return 0;
}

} // namespace

extern "C" HRESULT WINAPI Proxy_DirectInput8Create(HINSTANCE hinst, DWORD version, REFIID riid,
                                                   LPVOID* out, LPUNKNOWN outer)
{
    const LONG call = InterlockedIncrement(&g_di8Calls);

    const DI8Create_t real = RealCreate();
    if (!real)
    {
        if (out)
            *out = nullptr;
        return E_FAIL;
    }

    const HRESULT hr = real(hinst, version, riid, out, outer);
    if (call <= 4)
        bf1::Log("game called DirectInput8Create #%ld (version 0x%04lX) -> 0x%08lX",
                 static_cast<long>(call), static_cast<unsigned long>(version),
                 static_cast<unsigned long>(hr));

    TryScan("game's DirectInput8Create call", true);
    return hr;
}

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:DirectInput8Create=_Proxy_DirectInput8Create@20")
#endif

BOOL WINAPI DllMain(HINSTANCE self, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(self);

        char exe[MAX_PATH] = {};
        if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
            exe[0] = 0;
        const char* slash = strrchr(exe, '\\');
        const int dirLen = slash ? static_cast<int>(slash - exe) : 0;

        char logPath[MAX_PATH + 32];
        if (dirLen)
            snprintf(logPath, sizeof(logPath), "%.*s\\BF1GameExt_split.log", dirLen, exe);
        else
            snprintf(logPath, sizeof(logPath), "BF1GameExt_split.log");

        // 15e: keep the previous session's log (a crash's log used to be
        // overwritten by the next start)
        char prevPath[MAX_PATH + 40];
        if (dirLen)
            snprintf(prevPath, sizeof(prevPath), "%.*s\\BF1GameExt_split.prev.log", dirLen, exe);
        else
            snprintf(prevPath, sizeof(prevPath), "BF1GameExt_split.prev.log");
        const BOOL kept = MoveFileExA(logPath, prevPath, MOVEFILE_REPLACE_EXISTING);

        bf1::LogOpen(logPath);
        bf1::Log("%s | BF1GameExt %s | built %s %s", BF1_PHASE, BF1_BUILD, __DATE__, __TIME__);
        bf1::Log("dinput8 proxy loaded. Phase 21g: detect the exe (GOG 1.2 / GOG 1.3 / Steam), "
                 "BF1MemExt (1.2 only), scan, split test, controller scan, input scan, pad 2, "
                 "player 2 input object, player 2 redirect, spawn scan, player 2 auto-spawn, "
                 "player 2 controls, vertical split, loading screen fill, prone scan, prone fix, "
                 "frame capture, split effects fix (off), effect scanner, draw-group switches, "
                 "screen-copy scan, shadow + motion scan, shadow constant dump, hook health, split shadows, "
                 "split motion blur, swap-proof capture, split screen blur, stencil dump, stencil fix, "
                 "frame clear fix, spawn screen scan, console spawn screen, spawn screen capture, menu draw chains, menu in player 1's half, spawn map in the box, "
                 "sprint scan (test boost), L3 sprint only, player 2 third person, stamina, HUD scan, stamina bar, animation scan, run animation cap raised, Bullet test, ragdoll scan, ragdolls, self-collision, hit scan, thrown ragdolls, live tuning, animation blend, hits, seat scan, settings reload by themselves, live file, lab file, weapon drop, 15h joints, gun physics, getting up, death tracker, player ragdolls, limits scan, limits on, ray scan, floors, ragdoll scans, stutter fix, ragdoll walls, class wheel scan, console screen scan, console screen names, console screen preview (F6), charselect text, console flow, command post choice, wheel ring that turns, mouse on the wheel, sound names resolved, Steam early limits before the heap / animator object exist, player 2 spawn scan (log-only), player 2 screens preview (F5, split screen), player 2 menu input fix, player 2 spawn screens, interface player count for player 2, player 1 console flow in split screen, split screen layout (soldiers for player 2), stack walk crash fix, the Xbox layout in both halves, respawn kills the player, the Xbox's vertical spawn screens (blue grid, maps, player 2's ring), flat panels and the Xbox's rings, screen switches settled in the frame, player 2's death / map / sizes as player 1's, his UI sounds, 4-player map (2-player limits scan, census, live tables - log only), 4 views (Views=4: quarters, spectators 3 and 4), players 3 and 4 made to exist (Players=4), controller page scan (21c, log only), the changeable controller page (21d), the split screen menu (21e: SPLIT SCREEN in the main menu, the console sign-in screen, pad 2 for player 2), split screen teams (21f: Galactic Conquest's PICK YOUR SIDE - co-op / versus, no side screens while sides are locked), the 4-player sign-in (21g: four quarters, pads 3 and 4; r2: the player count from the sign-in, temporary profiles; r3: Galactic Conquest with 3 / 4 players - commanders, locked sides; r4: the galaxy turn scanner, co-op's AI turn, ragdoll skeletons on later maps).");
        bf1::Log("previous session's log %s", kept ? "kept as BF1GameExt_split.prev.log" : "not found (first start?)");
        bf1::CrashLogInstall();
        bf1::DetectBuild();
        bf1::LimitsScanEarly(); // 16b: heaps + hi-res units before the game starts (GOG 1.3 now, Steam after the unlock)
        bf1::ScanOnAttach();
        bf1::SplitTestInstallLuaHooks();
        bf1::ControllerLuaTraceInstall(); // 21c: before the game registers its Lua functions (21d / 21e: the scripts too)

        if (bf1::ScanExePatchedByMemExt())
            bf1::Log("entry hook: SKIPPED - exe already loads BF1MemExt through its .bf1ext section");
        else
            bf1::MemExtInstallEntryHook();

        const HANDLE t = CreateThread(nullptr, 0, FallbackThread, nullptr, 0, nullptr);
        if (t)
            CloseHandle(t);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        bf1::LogNoLock("session end (%s)", reserved ? "process exit" : "DLL unloaded");
    }
    return TRUE;
}
