// BF1GameExt - controller_scan.cpp   (file build p21j-r31)
// 21j-r31: BF1GameExt_pause34.lua p21j-r31 built in - p21i-r10 with its build as a string too (gBFX_Build_pause34, for a
// compiled console.lvl). The copy next to the user's exe was p21h-r10 (no AUTO AIM rows for players 3 / 4, and r10's
// stats-screen RESPAWN code that 21h-r11 moved in here - it ran twice), and the file next to the exe comes before the copy
// built in: console.lvl now carries this one.
// 21j-r30: CONSOLE.LVL FROM THE MOD TOOLS: the mod tools' ScriptMunge + LevelPack (the shell / common builders, with
// MungeConsole.bat) make console.lvl with each script compiled (luac 4.0.1: Lua 4.0, 32-bit, float numbers - the header GOG
// 1.3's undump checks; NAME in lower case, an INFO of 1 byte 1, BODY ending in a 0 byte, as the GOG shell's own scripts) -
// read here as r29's (NAME matched without case, INFO skipped, compiled run by the same lua_dobuffer). A compiled script
// has no comments: the scripts carry their build as a string too (gBFX_Build_split / gBFX_Build_ctrlpage, their second
// line), which the log reads there; BF1GameExt_split.lua and BF1GameExt_ctrlpage.lua p21j-r30 (that line only).
// 21j-r29: CONSOLE.LVL - the three scripts (BF1GameExt_split, BF1GameExt_pause34, BF1GameExt_ctrlpage) can come from a .lvl
// in the game's own format, GameData\Data\_LVL_PC\console.lvl (made with ConsoleMunge.exe): read here at the start, the way
// the game's level loader reads a .lvl's scripts (GOG 1.3: the chunk switch's "scr_" 0x00566C1C -> the reader 0x00586180 -
// NAME and BODY, every other part skipped, every chunk at a 4-byte boundary; ScriptCB_DoFile's runner 0x00584DF0 passes
// BODY's size - 1 to lua_dobuffer 0x0065D750, the one the scripts are run with here; it takes Lua text, or compiled Lua 4.0
// when the first byte is ESC - 0x0065D43B). A script in console.lvl comes first, then the file next to the exe, then the
// copy built in; each script's log line says which, with its file build, and a note when that is not this DLL's own build.
// The game itself never loads console.lvl (no shell.lvl / common.lvl change).
// 21j-r28: THE SIGN-IN'S SOUND AND MUSIC BACK (r27's [audio] lines): on PC every profile switch in the shell restarts the sound
// engine (SetCurrentProfile -> the profile's sound applied with 1 -> in the shell: shut down, started, core.lvl's sounds read
// again, the "restarted" byte 1) and the shell's own sounds (shell.lvl: menu clicks, the music / voice streams) and movie
// stream go with it - the PC's login and audio options put them back when ScriptCB_GetMixConfigChanged says so. The console
// sign-in's profile list (ScriptCB_GetLoginList with split screen on: profile 2 current and back) and its launch's profile
// loading never did: a silent sign-in, and every later SetShellMusic found no stream for the music playing and changed
// nothing. BF1GameExt_split.lua r28 runs the game's own restore after both ([Splitscreen] SignInSoundFix=1, gBFX_SoundFix).
// The audio scanner also logs ScriptCB_GetMixConfigChanged (with its answer), the engine's restarted byte and the shell
// music's stream (as the game's music update finds it: its state, or NONE); the Lua trace's wide strings end at their 0.
// 21j-r27: (1) THE SLIDER NUMBERS FIXED: the Lua trace (21c) read every call's arguments with lua_isnumber, which in Lua 4
// converts a string that reads as a number into that number in place - each options screen's ScriptCB_IFText_SetUString(label,
// "40%") (UTF-16: "4" as a C string) reached the game as the number 4, drawn as '4' plus whatever lay after it ("4@", "6,",
// "|" on the audio / controller sliders). The arguments are now read straight from the Lua stack (LuaValueText, the game's
// own index helper) - nothing is converted. (2) THE AUDIO SCANNER ([Splitscreen] AudioScan=1, [audio] lines, log only): the
// scripts' sound / music / movie calls on every screen with the sound state after them, and the state every 250 ms (a line
// when it changes) - sound on / off, the shell music and its interval, the engine's and both profiles' volumes, the buses'
// volume and gain (ControllerAudioFrame, from p2spawn_scan.cpp's menu layer frame). (3) The split screen script's sign-in:
// the pad's B / A on every player's BACK / ACCEPT (SignInPadIcons=1) and the BACK button at the bottom left working with the
// mouse (SignInMouseBack=1) - BF1GameExt_split.lua r27, handed gBFX_PadIcons / gBFX_MouseBack.
// 21j-r26: THE FREE CAMERA ON PLAYER 1'S PAD ([Splitscreen] FreeCamPad=1; FcUpdWrap, FreeCamPadInstall / Frame): R3 up, L3
// down (RT / LT no longer move it), the right stick looks (the game's free camera turned with the mouse only), X stops / starts
// the camera (the game's FreeCameraStop flag). One pointer: the FreeCamera vtable's update slot goes to FcUpdWrap, which sets
// player 1's up / down inputs and turn rates from his pad right before the game's update, calls it unchanged and puts them
// back. The keyboard and mouse as before; 0 = the game's own pad controls. [freecam] lines.
// 21j-r25: START / ESC IN PLAYER 1'S FREE CAMERA OPEN HIS PAUSE MENU (FreeCamPauseFrame, from ControllerBotFrame): the game's
// own Start did not pause while the free camera was on, so its CHASE CAMERA could not be reached: FREE CAMERA picked in the
// pause menu leaves the world frozen (the console's photo camera - its BACK / Backspace switches the world frozen / running)
// and the game never pauses a frozen world. A new press of Start (his pad) or Esc while his free camera is on and the game is
// not paused: if the game has not paused by the next frame, BF1GameExt pauses it as the game's Start does (p2spawn_scan.cpp
// P2SpawnPauseP1, SetPaused). Each press is logged with the game's pause conditions at that moment ([freecam] lines).
// [Splitscreen] PauseFreeCam > 0. No new hooks.
// 21j-r24: THE FAKE CONSOLE FIXED, AND UNDER FREE CAMERA. A command picked crashed the game: the game's ScriptCB_DoConsoleCmd
// switches the debug flag and then prints "NAME = 1" to the exe's debug console, which the 2020 exe never makes (the print
// writes to a null console). Now BF1GameExt switches the flag itself ("BFXQ: con! NAME", FakeConsoleQuery: the same entries
// ScriptCB_GetConsoleCmds lists, found through the game's own "debugmenu" lookup; only a bool variable or a flag = !flag
// command is touched) and the game's DoConsoleCmd is never run from the console; each row shows ON / OFF ("con NAME"). FAKE
// CONSOLE stands right under FREE CAMERA (the layout's list and the buttons' up / down links in that order, once the menu shows
// both). The free camera scan's notes corrected (its up / down / speed inputs are interface inputs 18 / 19 / 22 - 25: Home /
// End, = / -, ] / [ and RT / LT, the d-pad). No new hooks.
// 21j-r23: FREE CAMERA AND FAKE CONSOLE IN PLAYER 1'S PAUSE MENU ([Splitscreen] PauseFreeCam / PauseConsole: 1 single player,
// 2 player 1's in split screen too; never online, never over the stats screen; kPauseExtrasLua, [freecam] lines). Both are the
// game's own pause menu items - its Enter hides them because the 2020 exe says it is a final build (gFinalBuild, set by the
// exe before game_interface runs) - and both are whole in the exe: ScriptCB_Freecamera (GOG 1.3 0x0016AB50) switches player
// 1's camera mode (0x001E6F30; never online); ScriptCB_GetConsoleCmds / DoConsoleCmd list and run the exe's "debugmenu"
// commands (invincible, unlimitedammo, framerate, rendercollision, ToggleHolos, skipplayer - the first five read by the game's
// damage / weapon / frame-rate / collision-draw / hologram code; skipplayer only reset). The script shows them again after the
// game's Enter (laid out as the stats pause script does), labels the console FAKE CONSOLE (the game's "Fake Console" is a text
// key with no text), logs every pick and every console command. THE FREE CAMERA SCAN (log only, FreeCamScan=1): player 1's
// free camera on / off and, while on, the interface slots its up / down / speed controls read with the keys and pads held,
// and its debug keys' flags (FreeCamScanFrame, from ControllerBotFrame). Nothing else changed.
// 21j-r22: THE TUTORIAL MOVIES IN THEIR BOX, GALACTIC CONQUEST'S AND THE CAMPAIGNS' TUTORIALS BACK (kTutorialsLua, [tutorials]
// lines). (1) TutorialsMovieFit=1: the 2020 game hands its scripts a 4:3 screen (ScriptCB_GetScreenInfo's 4th value is always
// 1.0, GOG 1.3 0x00574C90), so every console-box movie (90, 60, 460 / 450 x 350 - for 640 x 480) is placed in the 1024 x 624
// interface as is: ScriptCB_PlayMovie (0x00574890) keeps the numbers, the movie quad (0x006F95D0) is drawn from them - on a
// 3840 x 2160 screen at 337, 208 to 2062, 1419, and the stretched background (movie_BG2) covers all of it but its black box
// (550, 292 to 3267, 1825): the movie showed in the box's top left, its left / top under the frame. Now
// ifelem_shellscreen_fnStartMovie, called with that box, gets the black box (93, 66, 450 x 338 of 640 x 480, in interface
// units) - 4:3 on the screen from ScriptCB_GetCurrentAspect (height / width, 0x009F75CC) and centred (2 = the whole box,
// stretched; 0 = the game's). (2) TutorialsGC=1: GALACTIC CONQUEST (ifs_sp "meta") opens the setup screen (ifs_meta_opts) as
// before and the game's ifs_meta_tutorial (tutorial08) right over it, as the consoles' new game; its end, a click, Enter or Esc
// -> back to the setup screen (the game's quit pushed another setup screen, its BACK popped to the consoles' new / load
// screen the PC never opens), single player's music again; not with a conquest in progress. (3) TutorialsCampaign=1: the
// missionlist lines the 2020 game commented out ("no tutorials on the pc") put back - Clone Wars nab1c_h: tutorial01cw, Galactic
// Civil War tat1i_h: tutorial01gcw, after Yoda's briefing, before the mission (the PC's ifs_sp_yoda still plays them). The
// viewer's / galactic conquest's lines say whether a movie ended, was stopped by the player or did not play. Nothing else
// changed.
// 21j-r21: THE MAIN MENU'S TUTORIALS BUTTON ON PC ([Splitscreen] TutorialsMenu=1, [tutorials] lines). The PC shell has the
// consoles' whole tutorials menu (ifs_tutorials: 8 tutorials; ifs_viewtutorial: plays one from movies\tut<lang>.mvs, else
// movies\tuteng.mvs - the file and window of the consoles' galactic conquest tutorial, ifs_meta_tutorial), the button is in ifsmain's
// list - only ifs_main's Enter hides it on PC. A script of its own (kTutorialsLua, "BFXU: " lines) runs while the game makes
// ifs_main and ifs_tutorials (its ScriptCB_AddIFScreen, before the objects), MainMenuSplit or not: after the game's ifs_main Enter
// TUTORIALS is shown again (ShowHideVerticalButtons, as SPLIT SCREEN); ifs_tutorials' Input_Accept goes through the PC's
// gShellScreen_fnDefaultInputAccept first (the mouse on BACK: the game's own looks tutorial "_back" up - a Lua error, "table index
// is nil", so BACK did nothing) and plays only a real tutorial; English text where the game has none; the viewer logs its movie
// file, the interface size and how long the movie ran (a viewer closed at once = its movie did not play). Nothing else changed.
// 21j-r18: (1) THE MODS ROW IN THE MAIN MENU'S OPTIONS ONLY - kModsLua does nothing in a match's Lua state (no ifs_main there: the
// shell makes its main menu before OPTIONS, a match never), so no OPTIONS in a match has MODS (r17 gave player 1's one); it stays
// hidden there anyway (bfxm_Shown). Player 1 sets the mods in the main menu, nobody switches them mid-game. (2) THE MAIN MENU'S
// GAME OPTIONS GETS A SPAWN SCREEN ROW ([Splitscreen] SpawnScreenMenu=1; kIAOptsLua, gBFXO_WheelRow): CLASS WHEEL (SINGLEPLAYER
// ONLY) / PC MENU (SINGLEPLAYER ONLY), after BF1GameExt's other rows (a copy of their row, as AUTO AIM / SPLIT SCREEN); A / left /
// right switch it ("BFXQ: wheel get | set N" -> classwheel_scan.cpp, [ClassWheel] Enabled - single player only now), RESET TO
// DEFAULTS puts it back to CLASS WHEEL; hidden in a match's GAME OPTIONS and when the class wheel scan is off. The value in the
// rows' font, or the small one when the longer value is wider than the row (the game's own way for its CAMERA value; [iaopts]
// "SPAWN SCREEN's longer value N wide" says which). Nothing else changed.
// 21j-r17: THE OPTIONS MENU'S MODS ROW ([Splitscreen] ModsMenu=1, [mods] lines). A script of its own (kModsLua) runs while the
// game makes ifs_opt_contmain (OPTIONS - the main menu's and a match's), before its objects are made: a MODS button (a copy of
// CREDITS', above it; shown in the main menu and in player 1's OPTIONS in a match, hidden in player 2's - the pausing viewport),
// its Enter / Input_Accept wrapped (each kept on the screen's own table), and right after the game's ScriptCB_EndIFScreen for it
// two pages: MODS (ifs_bfx_mods, OPTIONS' look: SPRINT ON / OFF, RAGDOLL EFFECTS ON / OFF, ADDON FOLDER: n MODS) and ADDON
// FOLDER (ifs_bfx_addons: the map list's box and list - every folder of GameData\AddOn with the missions it adds, the selected
// one's mission names under it, how many mods are installed, the game's addon missions used of its cap with a bar). SPRINT /
// RAGDOLL EFFECTS switch at once and save ("BFXQ: mods sprint | ragdoll [set N]" -> sprint.cpp / ragdoll.cpp); "mods count" the
// mods the game loads (AddOn's folders with an addme.script); "mods addons" reads AddOn and the game's addon mission list
// (limits_scan.cpp: each entry's mission name and folder) and runs them into the asking Lua state as gBFXM_A / gBFXM_L. The
// mission list is the one [Limits] DLCMissions raises: 512 the game's own, 4096 raised. Nothing else changed.
// 21j-r16: THE PAUSE MENU NOISE FIXED. r15's player 2 pause script named its helpers bfxp_* - the prefix the pause scan (21h-r1,
// kPauseProbe, [Splitscreen] PauseScan34=1) already used, in the same Lua state: its bfxp_Enter replaced the scan's, and the
// scan's wrappers on both pause menus call bfxp_Enter by name after the game's Enter. So every pause menu Enter (player 1's and
// player 2's, opening it and coming back to it from OPTIONS) ran the game's Enter again and again in one frame until Lua's stack
// overflowed (165 times in a Lua 4.0 test of the same scripts), each time its sound: that many copies of the accept / exit sound
// at once, the "earrape" (r15's video: exactly that sound, clipped flat). The Lua error then ended the Enter, before player 2's
// menu was moved. Now: kP2PauseLua's helpers are bfx2_* / gBFX2_* (no other script uses them), the original Enter is kept on the
// screen's own table (bfx2OrigEnter) and only ifs_pausemenu2 is ever passed through it; the pause scan keeps its own Enter on
// each screen (bfxpScanEnter) instead of looking bfxp_Enter up by name. The [luacb] start line says this file's build (it said
// p21i-r11). Nothing else changed.
// 21j-r15: player 2's pause menu as player 1's in the 2-player split ([Splitscreen] PauseLikeP1P2=1, p2spawn_scan.cpp): a script of
// its own (kP2PauseLua, [rp34] lua lines) runs while the game makes ifs_pausemenu2 in a 2-player match - player 1's menu sounds,
// his menu moved where player 1's stands in his half ("rp p2dx / p2dy" each time it opens), his OPTIONS gamepad only (no VIDEO
// OPTIONS, CONTROL OPTIONS straight to the gamepad page - as BF1GameExt_pause34.lua does for him with 3 / 4 players). Nothing
// else changed.
// 21j-r8: GALACTIC CONQUEST'S PICK YOUR SIDE IN THE CONSOLE'S LOOK (bug list #4; the built-in split screen script, p21j-r8):
// a row of side pictures for every player (four slots with 3 / 4 players), the PLAYERS text list replaced by a box for each
// player in the class screen's look (the game's window, its orange title bar - PLAYER n, then COMMANDER / PLAYER READY in
// his side's colour: green Rebels / Republic, red Empire / CIS), 2 players locking in and commanding too. [Splitscreen]
// GCPickLook=1 and GCCommander2P=1 (default 1; 0 = as before) handed to the script as gBFX_GCPickLook / gBFX_GCCmd2.
// Nothing else changed.
// 21j-r3: THE r2 CRASH FIXED. r2 put the GrassFadeFix stub at +96 of the grass stubs' page, but the scan's fade stub runs from
// +64 for 34 bytes: its last 2 bytes ("0C C3") overwrote the fix stub's first 2 (push edi / call) - the first .prp grass draw
// called into the middle of it (PRIVILEGED INSTRUCTION at stub +62h, the call's own offset bytes). Each stub has its own 64
// bytes now (entry +0, fade +64, fix +128 of 256), checked by static_asserts. Nothing else changed.
// 21j-r2: THE .PRP GRASS FADES IN AGAIN ([Splitscreen] GrassFadeFix=1). r1's log (Steam, Naboo Plains): the .prp layers' grass got
// maxDist 200 (the near scene end, the 2020 option at 1.00) while the layers' own distances are 60 and 160 - so it was drawn out
// to 196 m and grew in at 180 - 200 m (invisible that far), the near layer (bigger blades) drawn three times as far as 1.2
// draws it. The grasspatch objects grew in as 1.2's (60 - 80 m, their MaxDistance 80). Now the .prp caller's maxDist getter
// call (GOG 1.3 0x001FF62E, "call <near scene end>", byte-checked: its target is the near scene end's getter) goes through
// GrassPropMaxDistC: the layer's own distance (its info +18h - what 1.2 passes), held to the near scene end when that is
// nearer (so the grow band is always in front of the prop's cull). The 60 m layer grows in at 40 - 60 m, the 160 m one at
// 140 - 160 m, as 1.2. 0 = the 2020 port's (the near scene end). The grass scan (GrassScan=1) logs it; its blinks now tell a
// patch the game moved somewhere else (a .prp object it re-uses: "moved") from one that went missing in place.
// 21j-r1: THE GRASS SCAN (log only, [Splitscreen] GrassScan=1, [grass] lines) - bug list #1: on GOG 1.3 / Steam the grass never
// fades in (1.2 grows each patch in out of the ground over the last 20 m before it is drawn at full size) and flickers as you
// walk closer. GOG 1.3 against GOG 1.2: the grass draw (1.3 0x00066EA0, 1.2 0x001B3960) is the same code - cut beyond maxDist,
// in the last 20 m before it fade = (maxDist - dist) x 0.05 and the up axis x (1 - (1 - fade)^2) - and a grasspatch object
// passes its class's MaxDistance in both. A .prp terrain layer's grass (PropObjectMesh::Render, its type 2) does not: 1.2
// passes the layer's own distance (its info +18h), 1.3 the near scene end (0x0028DE90: the sky's NearSceneRange picked by a 2020
// option - the prop's range, which already culled it). Two calls inside the grass draw (5 bytes each, byte-checked) log, per
// caller every 5 s: the calls a frame, cut / drawn / in the grow band, the distances in 10 m buckets, maxDist against the
// layer's own distance and the prop range, blinks (a patch missing 1 - 8 frames, then back); each grass class / layer the
// first time it is drawn; the near scene range. Nothing in the game changes.
// 21i-r17: THE FIELD OF VIEW SHOWS NOW. r16 scaled the default FOV getter (0x00094AA0) - the log showed it answering 57.6 deg
// at 120 %, but nothing changed on screen: a soldier with a weapon gets his FOV from the weapon's aimer (its +150h, filled
// from that getter when the aimer was made, or its zoom), not the getter. Now as the Classic Collection does it (its chase
// camera, Battlefront1.dll 0x155AE2: fov x ro.fovscale when zoom <= 1): the chase camera's FOV is scaled right after the
// unit answers it (GOG 1.3 0x0013AB62, after "call [eax+0Ch]" filled [ebp-24h]; the zoom is [ebp-14h], filled by
// "call [edx+8]" from 1.0) - x FovScale with one local player and no zoom, then the game's own lerp and SetFov
// (0x0013B160 - 0x0013B17E). The getter is left as the game has it. Zoomed views and split screen keep the game's FOV.
// 21i-r16: FIELD OF VIEW - SINGLE PLAYER ONLY ([Splitscreen] FovScale 80 - 120 %, FovMenu=1). Every camera's base FOV comes
// from one getter (GOG 1.3 0x00094AA0: "fld [48 deg] / ret", called through its thunk by 9 GetFov functions - the chase
// camera asks it each frame, 0x0013AB4E, before the controlled unit's own (zoom) answer). It now jumps to FovDefaultC: 48 deg x
// FovScale with one local player (the game's count, SetNumLocalPlayers' int: 1 = single player), 48 deg always in split
// screen (2 - 4 players) - their views keep the game's own FOV. The Classic Collection's ro.fovscale is the same idea (0.8 -
// 1.2). VIDEO OPTIONS (ifs_opt_pcvideo) gets a FIELD OF VIEW slider under LOD DISTANCE (FovMenu=1; a copy of the LOD row and
// slider, made before the game makes the screen): left / right 5 % steps, the mouse on the slider too, in effect at once,
// written to FovScale. Hidden in a split screen match. "BFXQ: fov get | set N | split | log TEXT"; [fov] lines.
// 21i-r15: THE PAD CHANGES ARE AN EXPERIMENT NOW, OFF BY DEFAULT ([Splitscreen] PadExperiment=0): nothing of r13 / r14 goes in -
// BF1's own square 24 % dead zone, its own look curve (3.32) and sensitivity, the gamepad page as the game has it (no DEAD
// ZONE / LOOK CURVE rows: "padf on" answers 0). PadExperiment=1 = r13 / r14 exactly as they were (PadDeadZone, PadLookCurve,
// PadSensStyle, PadMenu). Kept so other players get the game's controls until the experiment is settled.
// 21i-r14: THE GAMEPAD PAGE'S DEAD ZONE / LOOK CURVE ROWS ([Splitscreen] PadMenu=1). The controller page script (built in, or
// BF1GameExt_ctrlpage.lua next to the exe) adds DEAD ZONE (BF1 24 % SQUARE, 0 - 25 %) and LOOK CURVE (1.0 BF2 - 3.3 BF1)
// under VIBRATION; they ask / set through "BFXQ: padf on | get dz | get curve | set dz N | set curve N" (PadQuery), take
// effect at once and are written to BF1GameExt.ini (PadDeadZone / PadLookCurve). With PadMenu=1 both r13 hooks always go in,
// so any choice works live: PadShapeC does BF1's own square 24 % dead zone itself (both axes inside -> 0, no rescale) when
// PadDeadZone is -1. PadLookCurve's default is now 2.0 (r13's 1.0 with BF1's own turn rates felt too fast: the log showed
// half stick turning at 5 - 6 x BF1's speed).
// 21i-r13: THE PAD AS BATTLEFRONT II'S. BF1 PC's pads (the game's XInput pad class, GOG 1.3 0x002BA620 - every pad: the game's,
// pad 2, pads 3 / 4) have a 24 % square dead zone (+-7864 of 32767, both axes inside -> both 0, no rescale: past it a stick
// starts at 24 %), and the look (PlayerInputs, 0x00063A70) turns at |stick| ^ 3.32 (pow, 0x00063BE1 / 0x00063C61) times the
// sensitivity scale 2 ^ ((s - 5) / 5) - half stick = 10 % speed, the last third of the stick 30 -> 100 %. Battlefront II
// (2017 build: DirectInput, profile +572h dead zone 1 - 10, default 5 = 1.6 % with rescale; JOYSTICK SENSITIVITY 1 - 10,
// default 4 = 1:1, 0x0052B510: gain (s + 1) / 5 up to 85 % stick, blended to 1.0 at full - the top speed never changes).
// [Splitscreen] PadDeadZone (percent, round, rescaled; -1 = the game's square 24 %): the game's +-7864 test reads 0 (never
// zeroes) and a call in place of "movups xmm0, [edi] / movups [esp], xmm0" (0x002BA732) reshapes both sticks' floats
// (+0Ch - +18h). PadLookCurve (the exponent; 3.32 = the game's, 1.0 = Battlefront II's straight line) and PadSensStyle
// (1 = Battlefront II's sensitivity: small / middle pushes faster or slower, full push the same; 0 = the game's: everything
// times the scale): both pow calls go to a stub - pow(|stick|, PadLookCurve), shaped, divided by the scale the game
// multiplies by next. [padfeel] lines (the game's own numbers first - BF1GameExt_PadOriginal.txt has them too).
// 21i-r12: AUTO AIM ON WITHOUT THE AIM RESIST. Whenever the aim assist has a target (the controllable's +130h target - an
// enemy in the reticle's reach) it first halves both look inputs (GOG 1.3 0x001F2F74 / 0x001F2F94, mulss [0.5]) - the
// console's "sticky aim": every turn goes at half speed over an enemy. That is the resist with auto aim ON (r11 took it away
// with OFF). [Splitscreen] AutoAimSlowdown (0 - 100 % of the console's, default 0) sets it: 0 = full look speed over a target
// (only the pull helps), 100 = the console's half speed; both mulss read BF1GameExt's value (1 - 0.5 x pct / 100). The strafe
// follow (0x001F30BD: the turn added while you move sideways, its own 0.75) now takes AutoAimStrength too, as the pull does.
// 21i-r11: AUTO AIM FIXED UP. (1) OFF is off: r9 NOP'ed the aim assist's mouse / keyboard exit, so for player 1 it always ran
// - and before its auto aim test it halves both look inputs and adds a little turn toward the target (the console's look
// handling around auto aim): with auto aim OFF that was the "aim resist". The exit is a gate now: auto aim OFF -> it leaves
// at once (the PC game as it was for player 1), ON -> it runs. (2) The pull: the Xbox's aim assist (default.xbe 0x000DB6A5 -
// 0x000DBAD8) is the same code with the same numbers (x0.5 look, 0.75 pull, the pull cut to +-1 stick a frame) - it was made
// for the Xbox's turn speed, and the PC turns faster per stick unit (and far faster per mouse unit), so the pull overshoots
// and swings. [Splitscreen] AutoAimStrength (10 - 100 %, default 50) scales the pull's 0.75 (GOG 1.3 0x001F315A reads
// BF1GameExt's value). [autoaim] lines name each player's first aim assist run (mouse / keyboard or not, ON / OFF).
// 21i-r10: AUTO AIM PER PLAYER ([Splitscreen] AutoAimPlayers=1). The aim assist asks GetCurrentProfile (one profile for
// everybody) for the auto aim byte; its call (GOG 1.3 0x001F30EB, right before the +2Ah test) now goes through a stub that
// passes the aim assist's own object: its controller's net player id ([controller +D4h], 0 - 3 offline = players 1 - 4)
// picks the answer - player 1 the game's profile as before (the main menu's / his pause menu's GAME OPTIONS AUTO AIM),
// players 2 - 4 a byte of BF1GameExt's (AutoAimP2 / P3 / P4, saved in BF1GameExt.ini). Player 2 sets his in his own pause
// menu's GAME OPTIONS (the same row - BF1GameExt sees it is his: the pausing viewport 1), players 3 / 4 in their GAME
// OPTIONS page (BF1GameExt_pause34.lua's new AUTO AIM row). [autoaim] lines name each player id the first time it aims.
// 21i-r9: (1) AUTO AIM WORKS: the aim assist (GOG 1.3 0x001F2EE0) leaves at once for a player whose controller is the
// mouse / keyboard one ([controller +10h] bits 30-31 - player 1's on PC, pad or not): the PC version's auto aim never ran.
// The Classic Collection (Battlefront1.dll 0x180210A40) has the same function and the same test, and a "/mouseaim" command
// line switch that lets it run anyway. [Splitscreen] AutoAimMouse=1 does what /mouseaim does: that one early exit (its
// jne, byte-checked: the function's prologue / the profile's auto aim test inside it) becomes a 6-byte NOP once, before
// any match. GAME OPTIONS' AUTO AIM (the profile's option) turns it on / off as on the consoles. (2) SPLIT SCREEN is on the
// main menu's GAME OPTIONS only - no row in a match.
// 21i-r8: TWO ROWS IN GAME OPTIONS (the main menu's and a match's), after DIFFICULTY. (1) SPLIT SCREEN HORIZONTAL / VERTICAL
// ([Splitscreen] SplitMenu=1; vertical.cpp's edits now go in either way, with the game's own values while horizontal): from
// the main menu the next match takes it; from a match's pause menu (2-player split only) it shows "NEXT MATCH" and goes in
// when the main menu is made again (ifs_main) - the match keeps the layout its HUD and spawn screens were made for. Written
// to BF1GameExt.ini's Vertical too. (2) AUTO AIM ON / OFF ([Splitscreen] AutoAimMenu=1): the PC build still has the console's
// auto aim - its aim assist (GOG 1.3 0x001F30EB) reads the current profile's auto aim byte (+2Ah, ScriptCB_SetAutoAimOn /
// IsAutoAimOn) in an offline match, the server's setting online - and GAME OPTIONS' own code still handles an "autoaim"
// row; the PC page only left the row out of its list. The row is put back (hidden online). HEROES as r7. The page script is
// in two pieces now (MSVC: 16380 bytes a piece).
// 21i-r7: (1) back from Instant Action's GAME OPTIONS the map list's era buttons (BESPIN: PLATFORMS / GALACTIC CIVIL WAR)
// stayed up over the list - the shell's base Enter fades them in; they are kept hidden on the list now (as the game's own
// Enter does). (2) The options menu's GAME OPTIONS page has no HEROES row any more (hidden as the game hides it in a match,
// the rows laid out again) - HEROES is set on the Instant Action / split screen page only. (3) HEROES is OFF by default:
// a profile with nothing saved and RESET TO DEFAULTS both put it OFF (in the profile too, when the map list opens).
// 21i-r6: INSTANT ACTION'S GAME OPTIONS PAGE ([Splitscreen] IAGameOptions=1). X on the Instant Action map list
// (ifs_missionselect: single player and split screen, never multiplayer) opens GAME OPTIONS, made as the multiplayer SET
// GAME OPTIONS page: AI UNITS PER TEAM (MAP DEFAULT, 0 - 32), REINFORCEMENTS (MAP DEFAULT, 10 - 1000), HEROES ON / OFF,
// RESET TO DEFAULTS (MAP DEFAULT / MAP DEFAULT / ON; r7: OFF). Left / right change a value, left from the lowest -> MAP DEFAULT; the
// triggers 10 units / 5 steps. Each change is saved for player 1's profile in BF1GameExt_IAOptions.ini (next to the exe,
// [IAOptions] <profile> = units reinforcements heroes) and is what the map's SetUnitCount / SetReinforcementCount get in
// Instant Action (never the campaign / Galactic Conquest, as r2); AIUnitsIA / ReinforcementsIA are used only with
// IAGameOptions=0. The options menu's GAME OPTIONS page has its HEROES row greyed out (it does nothing there now). The
// script (kIAOptsLua) runs while the game makes ifs_missionselect / ifs_opt_general; the page is made right after the game
// finishes the map list (its ScriptCB_EndIFScreen). r4's options page scan is gone (its job done).
// 21i-r4: TWO SCANS (log only, [Splitscreen] BotScanIA=1). (1) The bleed: every map's AddBleedThreshold(team, reinforcements,
// rate) and, every 5 s while they change, each team's reinforcements now / start (ControllerBotFrame) - r3 showed the
// drain going on below 20 - 25 with ReinforcementsIA=50 (the map's thresholds are made for its own 200). (2) The Instant
// Action options page's ground: while the game makes ifs_missionselect (the Instant Action / split screen map list) a
// small script logs its buttons and inputs (X / Y free or not), the 2020 shell's button helpers, the multiplayer SET GAME
// OPTIONS layout, the texts the page would use, heroes on / off, and player 1's profile ("BFXQ: iaprofile"); each time the
// map list opens it logs whether it is for Instant Action or multiplayer ([iaopts] lines). Nothing in the game changes.
// 21i-r3: AIUnitsIA never below the match's human players (1 single player; 2 - 4 in split screen): a team's unit count
// includes its players - r2's 0 with 3 players crashed (the game read a unit that was never made). Nothing else changed.
// 21i-r2: INSTANT ACTION'S AI UNITS PER TEAM AND REINFORCEMENTS ([Splitscreen] AIUnitsIA / ReinforcementsIA; empty = the map's
// own). r1's scan showed both calls and the setters behind them (Bespin Platforms: 28 units, 200 reinforcements a team, rules
// 0 / 0 = Instant Action). After the map's own SetUnitCount / SetReinforcementCount the game's numbers are replaced with the
// INI's - only in Instant Action (1 - 4 players): with the campaign's or Galactic Conquest's rule byte on, or either byte not
// found, the map's numbers stay. [bots] lines say each change or why it was left.
// 21i-r1: THE BOT / REINFORCEMENT SCAN (log only, [Splitscreen] BotScanIA=1, [bots] lines) - for Instant Action's AI units per
// team and reinforcements (1 - 4 players; never the campaign or Galactic Conquest). The map functions SetUnitCount /
// SetReinforcementCount are wrapped before the game registers them (as the ScriptCB functions): every call is logged with its
// team and number, what the game holds after it, the rules in force (campaign / Galactic Conquest / Instant Action), the map
// and the local players. The setters behind them and the rule flags are found from the functions' own calls and byte-checked
// at the first call. Nothing is changed.
// 21h-r13: the stats pause script: RESTART MISSION -> YES there plays the same map again ("rp statsrestart", then
// ScriptCB_QuitFromStats and ScriptCB_Unpause, as r12's QUIT). Nothing else changed.
// 21h-r12: the stats pause script (kStatsPauseLua): OPTIONS hidden there too, and QUIT -> YES there goes to the main menu
// ("rp statsquit" empties what is left of the game's mission queue, then ScriptCB_QuitFromStats and ScriptCB_Unpause - the
// game's QuitToShell does nothing once a match is won). Nothing else changed.
// 21h-r11: (1) player 1's pause menu on the stats screen hides RESPAWN in every match now (1 / 2 / 3 / 4 players): a small
// script of its own (kStatsPauseLua) wraps the game's ifs_pausemenu.Enter while the game makes the screen - r10 had it in
// BF1GameExt_pause34.lua, which runs only in 3 / 4-player matches. (2) BF1GameExt_pause34.lua (p21h-r11, r7's again) built in.
// 21h-r10: BF1GameExt_pause34.lua (p21h-r10) built in (now six pieces) - no RESPAWN in player 1's pause menu while it is up
// over the stats screen at the match end. Nothing else changed.
// 21h-r7: BF1GameExt_pause34.lua (p21h-r7) built in - the gamepad page's orange tabs where player 1's are (on the blue window's
// top edge, narrower bars, STARFIGHTER placed, the unpicked two a darker orange). Nothing else changed.
// 21h-r6: BF1GameExt_pause34.lua (p21h-r6) built in - players 3 / 4 change their buttons on their gamepad page as players 1 /
// 2 do (the pad's labels, A = the list of actions, p2spawn_scan.cpp keeps their maps); the page's d-pad as one SQUAD COMMANDS
// line, RESET TO DEFAULTS in full, a "B BACK" at player 1's size; no RESPAWN while their pause is over their spawn screens.
// Nothing else changed.
// 21h-r5: BF1GameExt_pause34.lua (p21h-r5) built in - players 3 / 4's GAME OPTIONS and CONTROL OPTIONS are copies of player 1's
// pages (GAME OPTIONS; the gamepad page with the INFANTRY / VEHICLE / STARFIGHTER tabs, no keyboard page), at player 1's size
// in their quarters. [Splitscreen] OptionsPagesP34=1 -> gBFXR_Pages (0: r4's lists). Nothing else changed.
// 21h-r4: (1) THE OPTIONS PAGES SCAN (log only, [Splitscreen] OptionsScan34=1, [optscan] lines): the game's GAME OPTIONS and
// gamepad pages (ifs_opt_general / ifs_opt_controller) dumped as the game builds them - every element's place, size, font,
// texture, text - for the next round's copies in players 3 / 4's quarters. (2) BF1GameExt_pause34.lua (p21h-r4) built in:
// players 3 / 4's YES / NO window at player 1's size ([Splitscreen] PopupScaleP34=51 -> gBFXR_PopScale), player 2's OPTIONS
// gamepad only in 3 / 4-player matches ([Splitscreen] OptionsP2=1 -> gBFXR_P2Opts). Nothing else changed.
// 21h-r3: BF1GameExt_pause34.lua (file build p21h-r3) built in again - players 3 / 4's OPTIONS in player 1's scheme (OPTIONS ->
// GAME OPTIONS / CONTROL OPTIONS), their RESPAWN / RESTART MISSION / QUIT questions in a popup window like player 1's, and
// player 2's pause menu with player 1's accept / back sounds (gBFXR_P2Sounds from [Splitscreen] PauseSoundsP2=1; the script
// runs only in 3 / 4-player matches, so 2-player is unchanged). The copy built in is in two pieces now (kPause34Pieces, joined
// at load: MSVC holds a string literal piece to 16380 bytes). Nothing else changed.
// 21h-r2: REAL PAUSE MENUS FOR PLAYERS 3 AND 4 - BF1GameExt_pause34.lua (built in as kPause34Pieces, or the file next to the exe)
// runs with the game's Lua while the game makes ifs_pausemenu2 in a match with 3 / 4 players (gBFXR_Players first; only while
// p2spawn_scan.cpp's real pause menus are installed, [Splitscreen] RealPauseP34=1): it makes ifs_pausemenu3 / 4 right after
// ifs_pausemenu2 is finished. Their lines come in as "BFXR: ..." ([rp34] lua lines, their own budget); their requests as
// "BFXQ: rp ..." (p2spawn_scan.cpp P2SpawnRealPauseAsk). Nothing else changed; no new hooks here.
// 21h-r1: THE PAUSE SCAN's Lua side (log only, [Splitscreen] PauseScan34=1, [pause34] lua lines): when the game's script makes
// ifs_pausemenu2 (split screen: ScriptCB_AddIFScreen("ifs_pausemenu2"), right after ifs_pausemenu) a small script of BF1GameExt's
// (kPauseProbe) logs the two pause screens' Lua tables as they are built (Viewport, CurButton, the buttons' container and each
// button's place), the screen sizes the scripts see, and whether the console's third / fourth screens (ifs_pausemenu3 / 4) or
// the templates a third / fourth screen would need exist; and it wraps both screens' Enter (after the game's own) to log the
// same at each pause, with the pausing viewport. Its lines come in as ScriptCB_GetPlatform("BFXP: ...") (their own budget).
// Nothing else changed; no new hooks (p2spawn_scan.cpp has the C++ side).
// 21g-r4: THE GALAXY TURN SCANNER's channel - the split screen script's "BFXT: ..." lines are logged as [gcturn] lines
// (their own budget, kGCTurnMaxLines), with [Splitscreen] GCTurnScan (default 1) handed to the script as gBFX_GCTurnScan.
// The built-in split screen script is 21g-r4's (co-op: the AI's joystick on its team; the turn scanner). No new hooks.
// 21g-r3: GALACTIC CONQUEST WITH 3 / 4 PLAYERS - "BFXQ: gcset S1 S2 S3 S4 C1 C2" (each player's side, each side's
// commander, from PICK YOUR SIDE), "gcget I", "gcclear", "playersnow" (this match's count); kept across the battles and in
// [Splitscreen] GCLastPick (a conquest resumed in a later session - the script checks them against its players 1 / 2).
// LockedSideP34 (controller_scan.hpp): players 3 / 4's side while sides are locked (Galactic Conquest: their pick; the
// campaign: player 1's) for p2spawn_scan.cpp. The split screen script also runs for ifs_meta_main (the galaxy map).
// 21g-r2: (1) the sign-in's LAUNCH sets the player count ("BFXQ: signin N X3 X4" -> playern_scan.cpp PlayerNSignIn: 2 / 3 /
// 4 players, the pads that signed in as players 3 / 4). (2) TEMPORARY PROFILES ([Splitscreen] TempProfiles): the game's
// two profiles, the current one's index and the local player slots' profile pointers are found through
// ScriptCB_GetLoginList's own "make player 2 a profile" code (each piece found once and its target byte-checked:
// GetProfile, GetCurrentProfile, SetName, the two pointer stores); "BFXQ: tempprofile NAME" does what that code does -
// profile 1 copied into profile 2, named NAME, the pointers written; "tempok", "tempmark NAME", "tempclear", "temp2"
// (the script keeps ifs_saveop from saving a temporary profile 2). The split screen script also runs while the game
// makes ifs_saveop. Header line gBFX_TempProfiles.
// 21g-r1: THE 4-PLAYER SIGN-IN. Pads 3 and 4 read for the shell as pad 2 is (MenuPad34Pick: the first XInput pads that
// answer and are not the game's, pad 2's or the other one's - pad 3 first, as players 3 / 4's pads are picked in a
// match); the script's questions "pad3" / "pad4" (+ conn / index / reset) and "players"; the script's settings lines
// get gBFX_Players (2, or 3 / 4 with players 3 / 4 on - playern_scan.cpp's installed count) and gBFX_SignIn4
// ([Splitscreen] SignIn4). The built-in split screen script is 21g-r1's (BF1GameExt_split.lua: the four quarters).
// Nothing else changed; no new hooks.
// 21f-r3: only the built-in split screen script changed (a conquest in progress resumes; BF1GameExt_split.lua r3).
// 21f-r2: the split screen script also runs for ifs_meta_top (PICK YOUR SIDE); its full logging on there too.
// 21f-r1: GALACTIC CONQUEST TEAMS + LOCKED SIDES. The split screen script (BF1GameExt_split.lua) also goes into
// ifs_meta_opts (the conquest maps: LAUNCH opens PICK YOUR SIDE) and ifs_meta_top (PICK YOUR SIDE, the console's screen,
// as the Classic Collection has it: each player picks his side - co-op / versus). The game's "can switch sides" byte (set by
// ScriptCB_SetCanSwitchSides: the shell turns it off for the campaign and Galactic Conquest) and the local players' side
// table (ScriptCB_SetPlayerSide / SetMetagameTeams) are found through those functions (byte-checked) and read by
// classwheel_scan.cpp / p2spawn_scan.cpp (SidesLockedNow / PlayerSideNow): no side screens while sides are locked
// ([Splitscreen] SideLock=1). Every change of them is logged ([splitmenu] sides: ...).
// 21e-r1: the SPLIT SCREEN entry of the main menu (2 players) - BF1GameExt_split.lua (built in, or the file next to the
// exe) run with the game's Lua when the game makes / opens ifs_main and ifs_split_profile (the console sign-in screen);
// pad 2 read here (XInput) and handed to the script one event at a time ("BFXQ: pad2", a number back through the
// game's pushnumber); [splitmenu] lines; the split screen scripts' ScriptCB calls added to the [luacb] trace. See
// SplitLoad / SplitRun / SplitQuery. [Splitscreen] MainMenuSplit=1, SplitSignIn=1.
// 21d-r2: RESET TO DEFAULTS - the controller page gets a RESET (Y) button (made with the screen: the script runs at
// ScriptCB_AddIFScreen("ifs_opt_controller") too), and the game's own ScriptCB_ResetControl (the PC controls page's
// RESET) is followed by the pad layout of the same tab (bfx_ResetPadTab).
// Phase 21d: the controller page made changeable - BF1GameExt_ctrlpage.lua (built in, or the file next to the exe) run
// with the game's Lua when ifs_opt_controller opens ([ctrlpage] lines). See PageLoad / PageRun.
// Phase 21c: the controller page scan ([luacb] lines, log only) - the options screens' Lua calls: the console
// controller config functions (ScriptCB_Get/SetFunctionIdForButtonId ...), screens, popups, visibility and texts.
// See ControllerLuaTraceInstall below. The C0 watcher is unchanged.
// Phase C0: read-only watcher for BF1's control bindings.
//
// Layout (same in GOG 1.2 and GOG 1.3; addresses per build in game_build.cpp,
// byte-checked before the watcher starts):
//   current profile index   int  (-1 = none, 0..1 valid)
//   profile array           profile 0, profile 1 at +0x458
//   joystick enabled        byte at profile + 0x370
//   binding table 0/1/2     43 DWORDs at profile + 0x034 / 0x0E0 / 0x18C
//   local player slots      2 entries of 0x30 bytes:
//                           +0x00 = id passed to the controller lookup,
//                           +0x10 = the profile that player uses
//
// Nothing is written to game memory.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "classwheel_scan.hpp" // 21j-r18: GAME OPTIONS' SPAWN SCREEN row
#include "controller_scan.hpp"
#include "frame_capture.hpp" // 21j-r1: the Present count (the grass scan's frames)
#include "vertical.hpp" // 21i-r8: GAME OPTIONS' SPLIT SCREEN row
#include "game_build.hpp"
#include "limits_scan.hpp" // 21j-r17: the addon mission list (the MODS page's ADDON FOLDER)
#include "log.hpp"
#include "pad2.hpp"
#include "p2spawn_scan.hpp"
#include "playern_scan.hpp"
#include "ragdoll.hpp" // 21j-r17: the MODS page's RAGDOLL EFFECTS
#include "scan.hpp"
#include "sprint.hpp" // 21j-r17: the MODS page's SPRINT

namespace bf1 {
namespace {

constexpr uint32_t kProfileSize = 0x458;
constexpr uint32_t kJoyEnabledOffset = 0x370;
constexpr uint32_t kTableOffsets[3] = { 0x034, 0x0E0, 0x18C };
constexpr int kActions = 43;
constexpr uint32_t kLocalSlotStride = 0x30;
constexpr int kLocalSlotCount = 2;

constexpr int kMaxLines = 800;
constexpr DWORD kPollMs = 200;

// BF1's action order, decoded from the controls screen's naming switch
// (jump table at 0x004B0144). Where the screen shows a different name for
// vehicles or flyers, the infantry name is used.
const char* const kActionNames[kActions] = {
    "primaryfire",   "secondaryfire", "jump",          "crouch",
    "zoom",          "view",          "reload",        "use (enter/exit)",
    "squadleft",     "squadright",    "squadup",       "squaddown",
    "primarynext",   "primaryprev",   "secondarynext", "secondaryprev",
    "prone",         "playerlist",    "chat",          "teamchat",
    "commchat0",     "commchat1",     "commchat2",     "commchat3",
    "commchat4",     "commchat5",     "commchat6",     "commchat7",
    "map",           "takescreenshot","strafe axis",   "move axis",
    "turn axis",     "pitch axis",    "(unused 34)",   "strafe right",
    "strafe left",   "move forward",  "move back",     "turn right",
    "turn left",     "pitch up",      "pitch down",
};

struct Snapshot
{
    int32_t profileIndex;
    uint8_t joyEnabled;
    uint32_t tables[3][kActions];
    uint32_t slotId[kLocalSlotCount];
    uint32_t slotProfile[kLocalSlotCount];
};

int g_lines = 0;

bool Budget()
{
    if (g_lines < kMaxLines)
    {
        ++g_lines;
        return true;
    }
    if (g_lines == kMaxLines)
    {
        ++g_lines;
        Log("[controls] line limit reached, controller scan going quiet");
    }
    return false;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uint8_t ReadU8(uintptr_t va) // 21j-r27 (the audio scanner)
{
    return *reinterpret_cast<const volatile uint8_t*>(va);
}

uintptr_t ProfileArray()
{
    return Rva(CurrentBuild()->profileArrayRva);
}

void Take(Snapshot& s)
{
    const BuildInfo* build = CurrentBuild();
    const uintptr_t slots = Rva(build->localSlotsRva);
    memset(&s, 0, sizeof(s));
    s.profileIndex = static_cast<int32_t>(ReadU32(Rva(build->profileIndexRva)));
    for (int i = 0; i < kLocalSlotCount; ++i)
    {
        s.slotId[i] = ReadU32(slots + i * kLocalSlotStride);
        s.slotProfile[i] = ReadU32(slots + i * kLocalSlotStride + 0x10);
    }
    if (s.profileIndex < 0 || s.profileIndex > 1)
        return;

    const uintptr_t profile = ProfileArray() + static_cast<uint32_t>(s.profileIndex) * kProfileSize;
    s.joyEnabled = *reinterpret_cast<const volatile uint8_t*>(profile + kJoyEnabledOffset);
    for (int t = 0; t < 3; ++t)
        memcpy(s.tables[t], reinterpret_cast<const void*>(profile + kTableOffsets[t]),
               sizeof(s.tables[t]));
}

int ProfileNumberOf(uint32_t ptr)
{
    for (int i = 0; i < 2; ++i)
        if (ptr == ProfileArray() + static_cast<uint32_t>(i) * kProfileSize)
            return i;
    return -1;
}

void LogSlots(const char* why, const Snapshot& s)
{
    for (int i = 0; i < kLocalSlotCount; ++i)
    {
        if (!Budget())
            return;
        const int pn = ProfileNumberOf(s.slotProfile[i]);
        char prof[32];
        if (pn >= 0)
            snprintf(prof, sizeof(prof), "profile %d", pn);
        else
            snprintf(prof, sizeof(prof), "0x%08X (no profile)", static_cast<unsigned>(s.slotProfile[i]));
        Log("[controls] %lu ms: %s local player slot %d: id 0x%08X, uses %s", ScanMsSinceAttach(),
            why, i, static_cast<unsigned>(s.slotId[i]), prof);
    }
}

void LogFullDump(const Snapshot& s)
{
    if (!Budget())
        return;
    Log("[controls] %lu ms: profile %d is active, joystick enabled = %u. Binding tables "
        "(t0 / t1 / t2):",
        ScanMsSinceAttach(), static_cast<int>(s.profileIndex), static_cast<unsigned>(s.joyEnabled));
    for (int a = 0; a < kActions; ++a)
    {
        if (!Budget())
            return;
        Log("[controls]   %2d %-16s  %08X  %08X  %08X", a, kActionNames[a],
            static_cast<unsigned>(s.tables[0][a]), static_cast<unsigned>(s.tables[1][a]),
            static_cast<unsigned>(s.tables[2][a]));
    }
}

void LogChanges(const Snapshot& before, const Snapshot& now)
{
    if (now.joyEnabled != before.joyEnabled && Budget())
        Log("[controls] %lu ms: joystick enabled %u -> %u", ScanMsSinceAttach(),
            static_cast<unsigned>(before.joyEnabled), static_cast<unsigned>(now.joyEnabled));

    for (int t = 0; t < 3; ++t)
    {
        for (int a = 0; a < kActions; ++a)
        {
            if (now.tables[t][a] == before.tables[t][a])
                continue;
            if (!Budget())
                return;
            Log("[controls] %lu ms: profile %d table %d action %2d %-16s %08X -> %08X",
                ScanMsSinceAttach(), static_cast<int>(now.profileIndex), t, a, kActionNames[a],
                static_cast<unsigned>(before.tables[t][a]),
                static_cast<unsigned>(now.tables[t][a]));
        }
    }
}

bool SlotsDiffer(const Snapshot& a, const Snapshot& b)
{
    return memcmp(a.slotId, b.slotId, sizeof(a.slotId)) != 0 ||
           memcmp(a.slotProfile, b.slotProfile, sizeof(a.slotProfile)) != 0;
}

DWORD WINAPI ControllerScanThread(LPVOID)
{
    Snapshot last;
    Take(last);
    LogSlots("start:", last);
    if (last.profileIndex >= 0 && last.profileIndex <= 1)
        LogFullDump(last);
    else if (Budget())
        Log("[controls] %lu ms: no profile active yet (index %d) - tables will be logged when one "
            "loads",
            ScanMsSinceAttach(), static_cast<int>(last.profileIndex));

    for (;;)
    {
        Sleep(kPollMs);
        Snapshot now;
        Take(now);

        if (SlotsDiffer(now, last))
            LogSlots("changed:", now);

        if (now.profileIndex != last.profileIndex)
        {
            if (now.profileIndex >= 0 && now.profileIndex <= 1)
                LogFullDump(now);
            else if (Budget())
                Log("[controls] %lu ms: profile index %d -> %d (none active)", ScanMsSinceAttach(),
                    static_cast<int>(last.profileIndex), static_cast<int>(now.profileIndex));
        }
        else if (now.profileIndex >= 0 && now.profileIndex <= 1)
        {
            LogChanges(last, now);
        }
        last = now;
    }
}

// ---------------------------------------------------------------------------------------------------------
// Phase 21c: the controller page scan - the Lua calls of the options / controller screens. Log only.
// ---------------------------------------------------------------------------------------------------------
// The options screens (ifs_opt_contmain, ifs_opt_controller, ...) are Lua: everything they read or change goes
// through the game's ScriptCB_* functions. Their registration entries ({name, function}, in .data) are read by the
// game when it registers them with Lua, after DllMain - so a stub of ours put in an entry at DllMain is what Lua
// calls. Only the names below are wrapped: none of them is read by any other part of BF1GameExt.
// Each stub calls LuaTraceCall(index, L), which logs the call and calls the game's function unchanged.
//   - every function's first call is logged (with its arguments and results)
//   - while an options screen is up (from a ScriptCB_PushScreen / SetIFScreen of "ifs_opt..." until one of another
//     screen) every call is logged, except exact repeats (same arguments and results), which are counted
// Arguments and results are read with the game's own Lua helpers (GOG 1.3, found through ScriptCB_GetFunctionIdFor-
// ButtonId's and ScriptCB_PushScreen's calls, byte-checked): gettop 0x0056F910, isnumber 0x0056FA10, isstring
// 0x0056FA40, tonumber 0x0065B8F0 (only after isnumber), tostring 0x004C7190 (only after isstring). The controller
// functions work on the "current player" (0x0093FDC4, read through its getter 0x0053D6E0) - logged with them.
// [luacb] lines. [Controls] LuaTrace=1 (default).
const char* const kTraceNames[] = {
    // the controller page's own functions (console controller config)
    "ScriptCB_GetFunctionIdForButtonId", "ScriptCB_SetFunctionIdForButtonId", "ScriptCB_GetFunctionIdForAnalogId",
    "ScriptCB_SetFunctionIdForAnalogId", "ScriptCB_GetYAxisFlip", "ScriptCB_SetYAxisFlip", "ScriptCB_GetControlScale",
    "ScriptCB_SetControlScale", "ScriptCB_ResetControlsToDefault", "ScriptCB_ResetControl", "ScriptCB_SetControlMode",
    "ScriptCB_GetControlMode", "ScriptCB_IsRumbleOn", "ScriptCB_SetRumbleOn", "ScriptCB_GetJoySensitivity",
    "ScriptCB_GetMouseSensitivity", "ScriptCB_IsJoystickEnabled", "ScriptCB_IsLastInputFromJoystick",
    "ScriptCB_GetPlatform",
    // screens and popups
    "ScriptCB_PushScreen", "ScriptCB_PopScreen", "ScriptCB_SetIFScreen", "ScriptCB_OpenPopup", "ScriptCB_ClosePopup",
    "ScriptCB_IsPopupOpen", "ScriptCB_SetIgnoreInputs",
    // what the page shows: visibility, enabled, texts
    "ScriptCB_IFObj_SetVis", "ScriptCB_IFObj_Enable", "ScriptCB_IFText_SetString", "ScriptCB_IFText_SetUString",
    // 21d-r2: the game making a screen (the controller page's RESET button goes in while it is made)
    "ScriptCB_AddIFScreen",
    // 21e: the split screen sign-in (ifs_split_profile): the profile list, sides, profile loading, controllers
    "ScriptCB_GetLoginList", "ScriptCB_SetPlayerSide", "ScriptCB_SwapActiveProfileSlots", "ScriptCB_StartLoadProfile",
    "ScriptCB_ReadAllControllers", "ScriptCB_UnbindController",
    // 21f: the teams: switching sides allowed or not, Galactic Conquest's teams, the campaign / conquest rules
    "ScriptCB_SetCanSwitchSides", "ScriptCB_SetMetagameTeams", "ScriptCB_SetMetagameRulesOn",
    "ScriptCB_SetHistoricalRulesOn",
    // 21j-r27: the audio scanner ([audio] lines, [Splitscreen] AudioScan): the scripts' sound / music / movie calls - logged
    // whatever screen is up, with the sound state right after them (kTraceAudio)
    "ScriptCB_SndBusFade", "ScriptCB_SetShellMusic", "ScriptCB_SetShellMusicInterval", "ScriptCB_SoundEnable",
    "ScriptCB_SoundDisable", "ScriptCB_SetVolumes", "ScriptCB_ResetSoundToDefault", "ScriptCB_PlayMovie", "ScriptCB_StopMovie",
    "ScriptCB_CloseMovie", "ScriptCB_OpenMovie", "ScriptCB_SetMovieAudioBus", "ScriptCB_ShellPlayDelayedStream",
    "ScriptCB_GetMixConfigChanged", // 21j-r28: "the sound engine was restarted" (asked and cleared by the scripts)
};
// 21j-r27: the audio scanner's calls (the last kTraceAudioCount names above)
constexpr int kTraceAudioCount = 14;
// 21e: not logged while a split screen screen has the full logging on (the sign-in screen sets them on every input)
const char* const kTraceQuietInSplit[] = {
    "ScriptCB_IFObj_SetVis", "ScriptCB_IFObj_Enable", "ScriptCB_IFText_SetString", "ScriptCB_IFText_SetUString",
};
constexpr int kTraceCount = static_cast<int>(sizeof(kTraceNames) / sizeof(kTraceNames[0]));
constexpr int kTraceControllerGroup = 19; // the first 19 work on the current player
constexpr int kTraceStubBytes = 24;
constexpr int kTraceMaxLines = 4000;

typedef int(__cdecl* LuaCFn_t)(void* L);
typedef int(__cdecl* LuaGetTop_t)(void* L);
typedef bool(__cdecl* LuaIs_t)(void* L, int i);
typedef float(__cdecl* LuaToNumber_t)(void* L, int i);
typedef const char*(__cdecl* LuaToString_t)(void* L, int i);
typedef int(__cdecl* CurPlayer_t)();
typedef void(__cdecl* LuaPushInt_t)(void* L, int value); // 21e: lua_pushnumber of an int

struct TraceFn
{
    LuaCFn_t orig;
    uintptr_t slot;
    unsigned long calls;
    uint32_t lastSig;
    unsigned long repeats;
    bool quiet; // 21e: kTraceQuietInSplit
    bool audio; // 21j-r27: one of the audio scanner's calls (kTraceAudioCount)
    DWORD lastMs; // 21j-r27: its last logged call
};
TraceFn g_tf[kTraceCount] = {};
unsigned char* g_traceStubs = nullptr;
volatile LONG g_traceHelpersOk = 0;
LuaGetTop_t g_luaGetTop = nullptr;
LuaIs_t g_luaIsNumber = nullptr;
LuaIs_t g_luaIsString = nullptr;
LuaToNumber_t g_luaToNumber = nullptr;
LuaToString_t g_luaToString = nullptr;
CurPlayer_t g_curPlayer = nullptr;
LuaPushInt_t g_luaPushInt = nullptr; // 21e
bool g_traceArmed = false;
bool g_traceSplit = false; // 21e: armed by a split screen screen (kTraceQuietInSplit not logged)
// 21f: the game's "can switch sides" byte and the local players' side table (int per player, 30h apart) - found in
// LuaTraceHelpersInit; [Splitscreen] SideLock
volatile const uint8_t* g_canSwitchByte = nullptr;
volatile const uint8_t* g_metaRulesByte = nullptr; // ScriptCB_SetMetagameRulesOn's: the game's own check also says no
                                                   // switching while it is on (GOG 1.3 0x00477270)
uintptr_t g_sideTable = 0;
constexpr uint32_t kSideStride = 0x30;
bool g_sideLockIni = true;
// 21g-r2: the game's two profiles (0x458 bytes each, the name at +2: 16 wide chars), the current profile's index and the
// local player slots' profile pointers - found through ScriptCB_GetLoginList's own "make player 2 a profile" code
// (LuaTraceHelpersInit, byte-checked); the temporary profile for player 2 (TempProfile2); [Splitscreen] TempProfiles
constexpr uint32_t kProfileBytes = 0x458;
constexpr uint32_t kProfileNameAt = 2;
constexpr int kProfileNameChars = 16;
uintptr_t g_profBase = 0;
uintptr_t g_profCurIdx = 0;
uintptr_t g_profSlot0 = 0;
uintptr_t g_profSlot1 = 0;
typedef char(__cdecl* SetCurProfile_t)(int index); // the game's SetCurrentProfile: the index, then that profile's
                                                     // controls / settings applied to the local slot that has it
SetCurProfile_t g_profSetCur = nullptr;
bool g_tempProfilesIni = true;
bool g_tempOn = false;                        // profile 2 is a temporary one (made or marked by the last sign-in)
char g_tempName[kProfileNameChars + 1] = {};  // its name
// 21g-r3: Galactic Conquest with 3 / 4 players - each player's side as picked on PICK YOUR SIDE (1 / 2, 0 = none) and
// each side's commander (the player, 1 - 4, who moves on the galaxy map for it; 0 = none / the AI), handed over by the
// split screen script ("BFXQ: gcset ..."), asked back after a battle ("gcget"); players 3 / 4's sides lock their spawn
// screens in its battles (LockedSideP34)
volatile LONG g_gcSide[4] = {};
volatile LONG g_gcCmd[2] = {};
char g_gcIni[MAX_PATH + 32] = {}; // BF1GameExt.ini: [Splitscreen] GCLastPick keeps the last ones for a conquest resumed later
int g_traceLines = 0;

bool TraceBudget()
{
    if (g_traceLines < kTraceMaxLines)
    {
        ++g_traceLines;
        return true;
    }
    if (g_traceLines == kTraceMaxLines)
    {
        ++g_traceLines;
        Log("[luacb] line limit reached, going quiet");
    }
    return false;
}

uint32_t Fnv(uint32_t h, const char* s)
{
    for (; *s; ++s)
        h = (h ^ static_cast<unsigned char>(*s)) * 16777619u;
    return h;
}

// 21j-r27: THE SLIDER NUMBERS FIXED. A Lua value's type is read straight from the game's Lua stack - Lua 4's TObject (type at
// +0: 1 nil, 2 number, 3 string; the value at +4: a float, or the string's TString - its length at +8, its bytes at +14h),
// through the game's own index helper (lua_isnumber's, GOG 1.3 0x0065BCA0: Cbase + i - 1, or NULL past the top).
// lua_isnumber (and so this trace's argument logging since 21c) CONVERTS a string that reads as a number into that number in
// place (luaV_tonumber): every options screen's ScriptCB_IFText_SetUString(label, "40%") - its UTF-16 bytes '4', 0, '0', 0,
// ... read as the C string "4" - reached the game as the number 4, which it turned back into the 1-byte string "4" and read
// as UTF-16: '4' and then whatever lay after it ("4@", "6,", "|" on the audio / controller sliders).
typedef const void*(__cdecl* LuaIndex_t)(void* L, int i);
LuaIndex_t g_luaIndex = nullptr; // NULL: the helper was not found - no argument is read (nothing logged for them)
constexpr int kLuaTNil = 1;
constexpr int kLuaTNumber = 2;
constexpr int kLuaTString = 3;

// the type of stack value i (1-based), -1 if there is none (or no helper)
int LuaTypeAt(void* L, int i)
{
    if (!g_luaIndex || i < 1)
        return -1;
    const void* o = g_luaIndex(L, i);
    return o ? *static_cast<const int32_t*>(o) : -1;
}

// argument i is a string (not a number) - without converting anything; with no helper, the old check (lua_isnumber /
// lua_isstring: only a string that reads as a number would be converted - the screen names and BFX lines never do)
bool LuaArgStr(void* L, int i)
{
    if (g_luaIndex)
        return LuaTypeAt(L, i) == kLuaTString;
    return !g_luaIsNumber(L, i) && g_luaIsString(L, i);
}

// stack value i as text (number, "string", u"wide string" - a UTF-16 one shown by its low bytes, nil, or its type)
int LuaValueText(void* L, int i, char* out, size_t size)
{
    const void* o = (g_luaIndex && i >= 1) ? g_luaIndex(L, i) : nullptr;
    if (!o)
        return snprintf(out, size, "?");
    const int32_t t = *static_cast<const int32_t*>(o);
    const uintptr_t v = *reinterpret_cast<const uint32_t*>(static_cast<const unsigned char*>(o) + 4);
    if (t == kLuaTNumber)
    {
        float f = 0.0f;
        memcpy(&f, static_cast<const unsigned char*>(o) + 4, sizeof(f));
        return snprintf(out, size, "%g", static_cast<double>(f));
    }
    if (t == kLuaTNil)
        return snprintf(out, size, "nil");
    if (t != kLuaTString || !v)
        return snprintf(out, size, t == 4 ? "table" : t == 5 ? "function" : t == 0 ? "userdata" : "?");
    const uint32_t len = *reinterpret_cast<const uint32_t*>(v + 8);
    const unsigned char* s = reinterpret_cast<const unsigned char*>(v + 0x14);
    // UTF-16 (ScriptCB_tounicode's): even length, every odd byte 0, the first byte not
    bool wide = len >= 2 && (len & 1) == 0 && s[0] != 0;
    for (uint32_t j = 1; wide && j < len && j < 512; j += 2) // (the first 256 letters are enough to tell)
        wide = s[j] == 0;
    char text[52];
    size_t n = 0;
    const uint32_t step = wide ? 2 : 1;
    for (uint32_t j = 0; j < len && n + 1 < sizeof(text); j += step)
    {
        const unsigned char c = s[j];
        if (!c) // (a wide string's own terminator too)
            break;
        text[n++] = (c >= 32 && c < 127) ? static_cast<char>(c) : '.';
    }
    text[n] = 0;
    return snprintf(out, size, wide ? "u\"%s\"" : "\"%s\"", text);
}

// Lua values a..b (1-based stack indexes) as text: numbers, "strings", ? for anything else. At most 6. 21j-r27: read as they
// are (LuaValueText) - nothing on the stack is converted any more
void FormatValues(void* L, int a, int b, char* out, size_t size)
{
    size_t at = 0;
    out[0] = 0;
    for (int i = a, k = 0; i <= b && k < 6 && at + 1 < size; ++i, ++k)
    {
        char v[64];
        LuaValueText(L, i, v, sizeof(v));
        const int n = snprintf(out + at, size - at, "%s%s", k ? ", " : "", v);
        if (n < 0)
            break;
        at += static_cast<size_t>(n);
    }
    if (b - a + 1 > 6 && at + 1 < size)
        snprintf(out + at, size - at, ", ... (%d values)", b - a + 1);
}

// ---------------------------------------------------------------------------------------------------------
// Phase 21d: the controller page made changeable. When the game opens "ifs_opt_controller" (ScriptCB_PushScreen,
// wrapped above), BF1GameExt_ctrlpage.lua is run with the game's own Lua (lua_dobuffer, GOG 1.3 0x0065D750, found
// through the base library's dostring 0x0065F8E0 and byte-checked) right before the screen's Enter. The script puts the
// console's remap code (still in the 2020 script, turned off) back on the pad side of the 2020 page; the game's own
// ScriptCB_SetFunctionIdForButtonId / AnalogId then change the current player's pad table. The script next to the exe
// (BF1GameExt_ctrlpage.lua) is used if it is there, else the copy below. Its bfx_Log lines come in as
// ScriptCB_GetPlatform("BFX: ...") calls, caught here: [ctrlpage] lines. [Controls] ControllerPage=1.
// The page's changes last until the game closes (the PC version saves no pad layout; saving them is the next step).
const char kPageScript[] = R"BFXLUA(-- BF1GameExt_ctrlpage.lua   (file build p21j-r30)
gBFX_Build_ctrlpage = "BF1GameExt_ctrlpage.lua (file build p21j-r30)" -- 21j-r30: the build as a string too: a compiled script (the mod tools'
-- ScriptMunge, console.lvl) keeps no comments, and BF1GameExt's log reads its build from this string there
-- 21i-r14: DEAD ZONE and LOOK CURVE rows under JOYSTICK SENSITIVITY / VIBRATION (BF1GameExt's pad settings - [Splitscreen]
-- PadMenu=1; asked and set through "BFXQ: padf ...", saved in BF1GameExt.ini PadDeadZone / PadLookCurve, in effect at once).
-- The rows are copies of the 2020 page's own two (made before the game makes the screen); the three settings containers
-- move up a little (.85 -> .80 of the screen) so four rows clear RESET / BACK. Left / right (or A) change a row.
-- The 2020 controller page (ifs_opt_controller) made changeable, the way the console / Classic Collection page works.
-- BF1GameExt runs this (Lua 4.0, the game's own compiler) with two lines in front of it: gBFX_L3Sprint ([Sprint]
-- L3SprintOnly) and gBFX_Phase:
--   "build" - while the game makes the screen (ScriptCB_AddIFScreen("ifs_opt_controller"), when the common scripts
--             load): the RESET (Y) button is added to the screen table here, so the game makes it with the rest
--   "push"  - each time the game opens the screen (ScriptCB_PushScreen), right before its Enter (a fallback: the screen
--             is only changed once per Lua state, ifs_opt_controller.bfext)
-- r2: RESET TO DEFAULTS. bfx_ResetPadTab puts the shown tab's pad layout back to the game's default. It runs from the
-- page's own RESET (Y, or a click), and BF1GameExt also calls it right after the game's own ScriptCB_ResetControl -
-- the PC controls page's RESET (Y), which only resets the keyboard / mouse - so that one resets the pad too.
--
-- The 2020 script still has the console's remap code (the pick list, contents_functype_*, ifs_opt_controller_SetEntry
-- -> ScriptCB_SetFunctionIdForButtonId / ScriptCB_SetFunctionIdForAnalogId); Aspyr turned it off with
-- gNoControllerChanges = 1 and emptied the cursor functions. This puts the pad side back on top of the 2020 page:
--   - a pad label is picked with the d-pad / stick (it lights up orange); A opens the list of actions for it
--     (the list of that tab: infantry / vehicle / starfighter), A picks, B closes the list
--   - down from the bottom label goes to the 2020 settings (joystick sensitivity, rumble); up from them comes back
--   - LB / RB still change the tab; RESET (Y, or a click on it) puts the shown tab back to the game's default layout
--   - Back, Start and the d-pad (map, pause, squad commands) stay fixed, as on the Classic Collection
--   - L3 is BF1GameExt's sprint button while [Sprint] L3SprintOnly=1 (gBFX_L3Sprint): shown as SPRINT, not pickable
-- gNoControllerChanges stays 1: the 2020 handlers keep doing the settings; these wrappers do the pad.
-- bfx_Log lines appear in BF1GameExt_split.log as [ctrlpage] lines (BF1GameExt catches them).

-- Shared (defined on every run): the log channel and the pad reset.
function bfx_Log(s)
	ScriptCB_GetPlatform("BFX: " .. s)
end

-- The game's default layout (the same in all three modes): Lua button id -> function id; the sticks: left Move /
-- Strafe (0), right Free Look (2).
gBFX_DefaultFn = { [1] = 6, [2] = 4, [11] = 9, [12] = 2, [13] = 1, [14] = 7, [15] = 3, [16] = 5, [18] = 15, [19] = 8 }

-- The current player's pad layout of the current tab (control mode) back to the game's default.
function bfx_ResetPadTab(why)
	local mode = ScriptCB_GetControlMode()
	local id, fn
	for id, fn in gBFX_DefaultFn do
		ScriptCB_SetFunctionIdForButtonId(id, fn)
	end
	ScriptCB_SetFunctionIdForAnalogId(0, mode, 0)
	ScriptCB_SetFunctionIdForAnalogId(1, mode, 2)
	bfx_Log(format("pad layout of tab %d reset to the game's default (%s)", mode, why))
end

-- 21i-r14: BF1GameExt's pad settings (the DEAD ZONE / LOOK CURVE rows)
function bfx_PadAsk(s)
	return ScriptCB_GetPlatform("BFXQ: padf " .. s)
end

function bfx_CopyT(t)
	if (type(t) ~= "table") then
		return t
	end
	local c = {}
	local k, v
	for k, v in t do
		c[k] = bfx_CopyT(v)
	end
	return c
end

-- the choices: dead zone in percent (-1 = BF1's own square 24 %), look curve x 100 (100 = Battlefront II's straight line,
-- 332 = BF1's own)
gBFX_DzList = { -1, 0, 2, 4, 6, 8, 10, 12, 15, 20, 25 }
gBFX_CurveList = { 100, 150, 200, 250, 332 }

function bfx_PadNearest(list, v)
	local best = 1
	local i
	for i = 1, getn(list) do
		if (abs(list[i] - v) < abs(list[best] - v)) then
			best = i
		end
	end
	return best
end

function bfx_PadText(which, v)
	if (which == "dz") then
		if (v < 0) then
			return "BF1 (24% SQUARE)"
		end
		return format("%d%%", v)
	end
	if (v == 100) then
		return "1.0 (BF2)"
	end
	if (v == 332) then
		return "3.3 (BF1)"
	end
	return format("%d.%d", floor(v / 100), floor(mod(v, 100) / 10))
end

function bfx_PadShow(this)
	if (not this.bfxpadrows) then
		return
	end
	local s = this.extension_button_settings
	local v = bfx_PadAsk("get dz")
	if (v and s.bfxdz) then
		RoundIFButtonLabel_fnSetUString(s.bfxdz, ScriptCB_tounicode(bfx_PadText("dz", v)))
	end
	v = bfx_PadAsk("get curve")
	if (v and s.bfxcurve) then
		RoundIFButtonLabel_fnSetUString(s.bfxcurve, ScriptCB_tounicode(bfx_PadText("curve", v)))
	end
end

-- one step left (-1) / right (+1) on a row, round the list
function bfx_PadStep(this, which, d)
	local list = gBFX_DzList
	if (which == "curve") then
		list = gBFX_CurveList
	end
	local cur = bfx_PadAsk("get " .. which) or list[1]
	local i = bfx_PadNearest(list, cur) + d
	if (i < 1) then
		i = getn(list)
	elseif (i > getn(list)) then
		i = 1
	end
	bfx_PadAsk(format("set %s %d", which, list[i]))
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_PadShow(this)
	bfx_Log(format("pad %s -> %s", which, bfx_PadText(which, list[i])))
end

function bfx_PadGo(this, tag)
	SetCurButtonTable(this.extension_button_settings[tag])
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

-- While the game makes the screen: the two rows, copies of JOYSTICK SENSITIVITY's / VIBRATION's (not made yet)
if ((gBFX_Phase == "build") and ifs_opt_controller and ifs_opt_controller.extension_button_settings and
	ifs_opt_controller.exntension_labels and (not ifs_opt_controller.bfxpadrows) and (bfx_PadAsk("on") == 1)) then
	local t = ifs_opt_controller
	local s = t.extension_button_settings
	local l = t.exntension_labels
	if (s.joysens and s.rumble and l.joysens and l.rumble) then
		local dy = s.rumble.y - s.joysens.y
		local ldy = l.rumble.y - l.joysens.y
		s.bfxdz = bfx_CopyT(s.joysens)
		s.bfxdz.tag = "bfxdz"
		s.bfxdz.y = s.rumble.y + dy
		s.bfxcurve = bfx_CopyT(s.rumble)
		s.bfxcurve.tag = "bfxcurve"
		s.bfxcurve.y = s.rumble.y + dy + dy
		if (s.bfxdz.label) then
			s.bfxdz.label.string = ""
		end
		if (s.bfxcurve.label) then
			s.bfxcurve.label.string = ""
		end
		l.bfxdz = bfx_CopyT(l.joysens)
		l.bfxdz.y = l.rumble.y + ldy
		l.bfxdz.string = nil
		l.bfxdz.ustring = ScriptCB_tounicode("DEAD ZONE")
		l.bfxcurve = bfx_CopyT(l.rumble)
		l.bfxcurve.y = l.rumble.y + ldy + ldy
		l.bfxcurve.string = nil
		l.bfxcurve.ustring = ScriptCB_tounicode("LOOK CURVE")
		s.joysens.link_up = "bfxcurve"
		s.rumble.link_down = "bfxdz"
		s.bfxdz.link_up = "rumble"
		s.bfxdz.link_down = "bfxcurve"
		s.bfxcurve.link_up = "bfxdz"
		s.bfxcurve.link_down = "joysens"
		l.ScreenRelativeY = 0.8
		s.ScreenRelativeY = 0.8
		if (t.extension_sliders) then
			t.extension_sliders.ScreenRelativeY = 0.8
		end
		t.bfxpadrows = 1
		bfx_Log(format("DEAD ZONE / LOOK CURVE rows added under VIBRATION (rows %g apart, the settings moved up to .80)", dy))
	end
end

-- While the game makes the screen: the RESET (Y) button, as the PC controls page's (bottom middle).
if ((gBFX_Phase == "build") and ifs_opt_controller and (not ifs_opt_controller.bfxreset)) then
	ifs_opt_controller.bfxreset = CreateMiddleButton("common.reset", "Y", "reset")
	bfx_Log("RESET (Y) button added to ifs_opt_controller")
end

if (ifs_opt_controller and (not ifs_opt_controller.bfext)) then
ifs_opt_controller.bfext = 1

-- Platform_btn_map entries that can be changed: LB LT LS RS X A B Y RT RB R3 (L3 too when it is not sprint)
gBFX_Pick = { 1, 2, 7, 10, 11, 12, 13, 14, 15, 16, 19 }
if (not gBFX_L3Sprint) then
	gBFX_Pick[getn(gBFX_Pick) + 1] = 18
end

-- The 2020 handlers, called for everything that is not the pad
gBFX_Orig = {
	Enter = ifs_opt_controller.Enter,
	Input_Accept = ifs_opt_controller.Input_Accept,
	Input_Back = ifs_opt_controller.Input_Back,
	Input_GeneralUp = ifs_opt_controller.Input_GeneralUp,
	Input_GeneralDown = ifs_opt_controller.Input_GeneralDown,
	Input_GeneralLeft = ifs_opt_controller.Input_GeneralLeft,
	Input_GeneralRight = ifs_opt_controller.Input_GeneralRight,
	Input_LTrigger2 = ifs_opt_controller.Input_LTrigger2,
	Input_RTrigger2 = ifs_opt_controller.Input_RTrigger2,
	Update = ifs_opt_controller.Update,
}

-- The selected pad label lights up orange while the pad has the focus.
function bfx_Highlight(this)
	local lit = nil
	if (this.CurButton == "bfxpad") then
		lit = 1
	end
	local i
	for i = 1, getn(gBFX_Pick) do
		local k = gBFX_Pick[i]
		if (this.lines[k] and this.lines[k].label) then
			if (lit and (this.bfxPad == k)) then
				IFObj_fnSetColor(this.lines[k].label, 255, 170, 0)
			else
				IFObj_fnSetColor(this.lines[k].label, 255, 255, 255)
			end
		end
	end
	this.bfxLit = lit
end

-- Labels the 2020 code does not know about, then the highlight.
function bfx_Refresh(this)
	if (gBFX_L3Sprint and this.lines[18] and this.lines[18].label) then
		if (gControlMode == 0) then
			IFText_fnSetUString(this.lines[18].label, ScriptCB_tounicode("SPRINT"))
		else
			IFText_fnSetString(this.lines[18].label, "ifs.controls.General.none")
		end
	end
	bfx_Highlight(this)
end

function bfx_ToPad(this)
	SetCurButtonTable(nil)
	this.CurButton = "bfxpad"
	if (not this.bfxPad) then
		this.bfxPad = 2
	end
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

function bfx_ToSettings(this)
	SetCurButtonTable(this.extension_button_settings.joysens)
	ifelm_shellscreen_fnPlaySound(this.selectSound)
	bfx_Highlight(this)
end

-- Moves the pad selection to the nearest label in a direction (labels stand in two columns, left and right of the
-- pad, at Platform_btn_map's lx1 / ly1). Returns 1 if it moved.
function bfx_Move(this, dir)
	local cur = Platform_btn_map[this.bfxPad]
	local cx = cur.lx1
	local cy = cur.ly1
	local best = nil
	local bestScore = 0
	local i
	for i = 1, getn(gBFX_Pick) do
		local k = gBFX_Pick[i]
		if (k ~= this.bfxPad) then
			local v = Platform_btn_map[k]
			local dy = v.ly1 - cy
			local sameSide = ((v.lx1 < 0) == (cx < 0))
			local ok = nil
			local score = 0
			if (dir == "up") then
				if (sameSide and (dy < 0)) then ok = 1 end
				score = -dy
			elseif (dir == "down") then
				if (sameSide and (dy > 0)) then ok = 1 end
				score = dy
			elseif (dir == "left") then
				if ((v.lx1 < 0) and (cx > 0)) then ok = 1 end
				score = abs(dy)
			else
				if ((v.lx1 > 0) and (cx < 0)) then ok = 1 end
				score = abs(dy)
			end
			if (ok and ((not best) or (score < bestScore))) then
				best = k
				bestScore = score
			end
		end
	end
	if (best) then
		this.bfxPad = best
		ifelm_shellscreen_fnPlaySound(this.selectSound)
		bfx_Highlight(this)
		return 1
	end
	return nil
end

-- A on a pad label: the list of actions for it, on the shown tab.
function bfx_OpenList(this)
	local k = this.bfxPad
	local Button = Platform_btn_map[k]
	local CurFunc = Button.func
	if (Button.name == "lang") then
		CurFunc = 16 + ScriptCB_GetFunctionIdForAnalogId(0, gControlMode)
	elseif (Button.name == "rang") then
		CurFunc = 16 + ScriptCB_GetFunctionIdForAnalogId(1, gControlMode)
	end
	if (Button.type == 3) then
		if (gControlMode == 0) then
			ifs_controller_listbox_contents = contents_functype_infantry_stick
		elseif (gControlMode == 1) then
			ifs_controller_listbox_contents = contents_functype_vehicle_stick
		else
)BFXLUA"
R"BFXLUA(			ifs_controller_listbox_contents = contents_functype_flyer_stick
		end
	else
		if (gControlMode == 0) then
			ifs_controller_listbox_contents = contents_functype_infantry_button
		elseif (gControlMode == 1) then
			ifs_controller_listbox_contents = contents_functype_vehicle_button
		else
			ifs_controller_listbox_contents = contents_functype_flyer_button
		end
	end
	local aListIndex = 1
	local i
	for i = 1, getn(ifs_controller_listbox_contents) do
		if (ifs_controller_listbox_contents[i].showidx == CurFunc) then
			aListIndex = i
		end
	end
	ifs_controller_listbox_layout.SelectedIdx = aListIndex
	ifs_controller_listbox_layout.CursorIdx = aListIndex
	ifs_controller_listbox_layout.FirstShownIdx = max(aListIndex - floor(ifs_controller_listbox_layout.showcount * 0.5), 1)
	ListManager_fnFillContents(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)

	this.cont_menu_state = 1
	this.cont_element_idx = k
	ifs_opt_controller_SetLineVisibility(this, nil, this.lines[k])
	if (Button.lx1 < 0) then
		IFObj_fnSetPos(this.listbox, 120, 0)
	else
		IFObj_fnSetPos(this.listbox, -120, 0)
	end
	IFObj_fnSetVis(this.listbox, 1)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Log(format("list open: button %d (%s), mode %d, current %d", k, Button.name, gControlMode, CurFunc))
end

function bfx_CloseList(this)
	IFObj_fnSetVis(this.listbox, nil)
	this.cont_menu_state = 0
	ifs_opt_controller_SetLineVisibility(this, 1)
	bfx_Refresh(this)
end

-- A in the list: the picked action goes to the button (the 2020 script's own SetEntry, the console's code).
function bfx_Pick(this)
	local Selection = ifs_controller_listbox_contents[ifs_controller_listbox_layout.SelectedIdx]
	local k = this.cont_element_idx
	bfx_CloseList(this)
	if (Selection) then
		ifs_opt_controller_SetEntry(this, k, Selection.showidx)
		bfx_Log(format("picked: button %d (%s), mode %d -> %d", k, Platform_btn_map[k].name, gControlMode, Selection.showidx))
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Refresh(this)
end

-- RESET (Y, or a click on it): the shown tab back to the game's default layout.
function bfx_ResetTab(this)
	bfx_ResetPadTab("the controller page's RESET")
	ifs_opt_controller_GetAllEntries(this)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfx_Refresh(this)
end

ifs_opt_controller.Enter = function(this, bFwd)
	this.bfxMode0 = ScriptCB_GetControlMode()
	gBFX_Orig.Enter(this, bFwd)
	this.cont_menu_state = 0
	this.bfxPad = 2
	bfx_ToPad(this)
	bfx_Refresh(this)
	bfx_PadShow(this)
	bfx_Log(format("page up: control mode %d (put back when it closes)", this.bfxMode0))
end

ifs_opt_controller.Input_Accept = function(this)
	if (this.cont_menu_state == 1) then
		if (gMouseListBox) then
			gShellScreen_fnDefaultInputAccept(this)
		end
		bfx_Pick(this)
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_OpenList(this)
		return
	end
	if (this.CurButton == "reset") then
		bfx_ResetTab(this)
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", 1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", 1)
		return
	end
	gBFX_Orig.Input_Accept(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_Back = function(this)
	if (this.cont_menu_state == 1) then
		bfx_CloseList(this)
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
		return
	end
	if (this.bfxMode0) then
		ScriptCB_SetControlMode(this.bfxMode0)
	end
	gBFX_Orig.Input_Back(this)
end

ifs_opt_controller.Input_GeneralUp = function(this)
	if (this.cont_menu_state == 1) then
		ListManager_fnNavUp(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "up")
		return
	end
	if (this.CurButton == "joysens") then
		bfx_ToPad(this)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadGo(this, "bfxdz")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadGo(this, "rumble")
		return
	end
	gBFX_Orig.Input_GeneralUp(this)
end

ifs_opt_controller.Input_GeneralDown = function(this)
	if (this.cont_menu_state == 1) then
		ListManager_fnNavDown(this.listbox, ifs_controller_listbox_contents, ifs_controller_listbox_layout)
		return
	end
	if (this.CurButton == "bfxpad") then
		if (not bfx_Move(this, "down")) then
			bfx_ToSettings(this)
		end
		return
	end
	if (this.bfxpadrows and (this.CurButton == "rumble")) then
		bfx_PadGo(this, "bfxdz")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadGo(this, "bfxcurve")
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadGo(this, "joysens")
		return
	end
	gBFX_Orig.Input_GeneralDown(this)
end

ifs_opt_controller.Input_GeneralLeft = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "left")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", -1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", -1)
		return
	end
	gBFX_Orig.Input_GeneralLeft(this)
end

ifs_opt_controller.Input_GeneralRight = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	if (this.CurButton == "bfxpad") then
		bfx_Move(this, "right")
		return
	end
	if (this.CurButton == "bfxdz") then
		bfx_PadStep(this, "dz", 1)
		return
	end
	if (this.CurButton == "bfxcurve") then
		bfx_PadStep(this, "curve", 1)
		return
	end
	gBFX_Orig.Input_GeneralRight(this)
end

ifs_opt_controller.Input_LTrigger2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	gBFX_Orig.Input_LTrigger2(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_RTrigger2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	gBFX_Orig.Input_RTrigger2(this)
	bfx_Refresh(this)
end

ifs_opt_controller.Input_Misc2 = function(this)
	if (this.cont_menu_state == 1) then
		return
	end
	bfx_ResetTab(this)
end

ifs_opt_controller.Update = function(this, fDt)
	gBFX_Orig.Update(this, fDt)
	-- the mouse can move the focus to the 2020 settings: the pad label goes dark again
	local lit = nil
	if (this.CurButton == "bfxpad") then
		lit = 1
	end
	if (lit ~= this.bfxLit) then
		bfx_Highlight(this)
	end
end

bfx_Log(format("installed on ifs_opt_controller (%s)", gBFX_Phase or "?"))
end
)BFXLUA";

typedef int(__cdecl* LuaDoBuffer_t)(void* L, const char* buf, size_t size, const char* name);
LuaDoBuffer_t g_luaDoBuffer = nullptr;
bool g_pageIni = true;          // [Controls] ControllerPage
bool g_pageL3Sprint = true;     // [Sprint] L3SprintOnly: the page shows L3 as SPRINT
char* g_pageSrc = nullptr;      // the script (its two settings lines are run in front of it, RunLua)
size_t g_pageLen = 0;
char g_pageFrom[MAX_PATH + 160] = {};   // 21j-r29: where the script came from (console.lvl / the file / built in), for the log
unsigned long g_pageRuns = 0;
int g_pageLines = 0;
constexpr int kPageMaxLines = 600;

bool PageBudget()
{
    if (g_pageLines < kPageMaxLines)
    {
        ++g_pageLines;
        return true;
    }
    if (g_pageLines == kPageMaxLines)
    {
        ++g_pageLines;
        Log("[ctrlpage] line limit reached, going quiet");
    }
    return false;
}

// 21e: the split screen menu script's state (its log lines have a budget of their own)
bool g_splitIni = true;         // [Splitscreen] MainMenuSplit
bool g_splitSignIn = true;      // [Splitscreen] SplitSignIn
bool g_signIn4 = true;          // 21g: [Splitscreen] SignIn4 (the 4-player sign-in screen with players 3 / 4 on)
bool g_gcTurnScanIni = true;    // 21g-r4: [Splitscreen] GCTurnScan (the galaxy turn scanner's [gcturn] lines)
bool g_gcPickLookIni = true;    // 21j-r8: [Splitscreen] GCPickLook (PICK YOUR SIDE in the console's look)
bool g_gcCmd2Ini = true;        // 21j-r8: [Splitscreen] GCCommander2P (2 players lock in, commanders)
bool g_padIconsIni = true;      // 21j-r27: [Splitscreen] SignInPadIcons (the sign-in's BACK / ACCEPT with the pad's B / A)
bool g_mouseBackIni = true;     // 21j-r27: [Splitscreen] SignInMouseBack (the sign-in's BACK button works with the mouse)
bool g_soundFixIni = true;      // 21j-r28: [Splitscreen] SignInSoundFix (the shell's sound back after the sign-in's profile switches)
int g_gcTurnLines = 0;          // 21g-r4: [gcturn] lines so far
constexpr int kGCTurnMaxLines = 5000;
bool g_pauseScanIni = true;     // 21h-r1: [Splitscreen] PauseScan34 (the pause scan's Lua side, [pause34] lua lines)
bool g_realPauseIni = true;     // 21h-r2: [Splitscreen] RealPauseP34 (players 3 / 4's real pause menus)
bool g_p2SoundsIni = true;      // 21h-r3: [Splitscreen] PauseSoundsP2 (player 2's pause menu gets player 1's sounds; 3 / 4 players)
bool g_p2OptsIni = true;        // 21h-r4: [Splitscreen] OptionsP2 (player 2's OPTIONS gamepad only; 3 / 4 players)
int g_popScaleIni = 51;         // 21h-r4: [Splitscreen] PopupScaleP34 (players 3 / 4's YES / NO window size, percent)
bool g_optScanIni = true;       // 21h-r4: [Splitscreen] OptionsScan34 (the options pages scan, [optscan] lines)
bool g_pagesIni = true;         // 21h-r5: [Splitscreen] OptionsPagesP34 (players 3 / 4's copies of player 1's options pages)
bool g_botScanIni = true;       // 21i: [Splitscreen] BotScanIA (the bot / reinforcement scan, [bots] lines)
int g_iaLines = 0;              // 21i-r4: [iaopts] lua lines so far
constexpr int kIaMaxLines = 1000; // 21i-r6: 1000 (each GAME OPTIONS change is a line)
int g_botUnitsIA = -1;          // 21i-r2: [Splitscreen] AIUnitsIA (AI units per team in Instant Action; -1 / empty = the map's own)
int g_botReinfIA = -1;          // 21i-r2: [Splitscreen] ReinforcementsIA (each team's reinforcements in Instant Action; -1 / empty = the map's)
bool g_iaPageIni = true;        // 21i-r6: [Splitscreen] IAGameOptions (Instant Action's GAME OPTIONS page; its numbers replace AIUnitsIA / ReinforcementsIA)
char g_iaStore[MAX_PATH + 40] = {}; // 21i-r6: BF1GameExt_IAOptions.ini next to the exe ([IAOptions] <profile> = units reinforcements heroes)
int g_iaUnits = -1;             // 21i-r6: the page's numbers for player 1's profile (-1 = MAP DEFAULT)
int g_iaReinf = -1;
int g_iaHeroes = -1;            // 21i-r6: -1 not saved, 0 off, 1 on
constexpr int kIaMaxUnits = 32;
bool g_splitMenuIni = true;     // 21i-r8: [Splitscreen] SplitMenu (GAME OPTIONS' SPLIT SCREEN row; vertical.cpp reads it too)
bool g_aimMenuIni = true;       // 21i-r8: [Splitscreen] AutoAimMenu (GAME OPTIONS' AUTO AIM row)
bool g_aimMouseIni = true;      // 21i-r9: [Splitscreen] AutoAimMouse (the aim assist for the mouse / keyboard player too, as /mouseaim)
bool g_aimMouseTried = false;
bool g_aimPlayersIni = true;    // 21i-r10: [Splitscreen] AutoAimPlayers (auto aim on / off per player)
int g_aimStrengthIni = 50;      // 21i-r11: [Splitscreen] AutoAimStrength (the pull, percent of the Xbox's)
int g_aimSlowIni = 0;           // 21i-r12: [Splitscreen] AutoAimSlowdown (the look slow-down over a target, percent of the console's)
int g_padDzIni = 2;             // 21i-r13: [Splitscreen] PadDeadZone (percent, round; -1 = the game's square 24 %)
double g_padCurveIni = 2.0;     // 21i-r13 / r14: [Splitscreen] PadLookCurve (the look's exponent; 0 = the game's 3.32)
bool g_padMenuIni = true;       // 21i-r14: [Splitscreen] PadMenu (DEAD ZONE / LOOK CURVE rows on the gamepad page)
bool g_padExpIni = false;       // 21i-r15: [Splitscreen] PadExperiment (0 = none of r13 / r14: the game's own pad and look)
int g_fovPctIni = 100;          // 21i-r16: [Splitscreen] FovScale (single player's FOV, percent of the game's 48 deg, 80 - 120)
bool g_fovMenuIni = true;       // 21i-r16: [Splitscreen] FovMenu (VIDEO OPTIONS' FIELD OF VIEW slider)
bool g_modsMenuIni = true;      // 21j-r17: [Splitscreen] ModsMenu (the options menu's MODS row and pages)
bool g_wheelMenuIni = true;     // 21j-r18: [Splitscreen] SpawnScreenMenu (the main menu's GAME OPTIONS SPAWN SCREEN row)
bool g_tutorialsIni = true;     // 21j-r21: [Splitscreen] TutorialsMenu (the main menu's TUTORIALS button on PC)
int g_tutFitIni = 1;            // 21j-r22: [Splitscreen] TutorialsMovieFit (0 the game's place, 1 the box at 4:3, 2 the whole box)
bool g_tutGCIni = true;         // 21j-r22: [Splitscreen] TutorialsGC (galactic conquest's tutorial movie before its setup)
bool g_tutCampIni = true;       // 21j-r22: [Splitscreen] TutorialsCampaign (the campaigns' command post tutorials)
int g_pauseCamIni = 1;          // 21j-r23: [Splitscreen] PauseFreeCam (0 off, 1 single player, 2 player 1 in split screen too)
int g_pauseConIni = 1;          // 21j-r23: [Splitscreen] PauseConsole (the same values)
bool g_freeCamScanIni = true;   // 21j-r23: [Splitscreen] FreeCamScan (log only: [freecam] lines while the free camera is on)
bool g_freeCamPadIni = true;    // 21j-r26: [Splitscreen] FreeCamPad (R3 / L3 up / down, the right stick looks, X stops it)
int g_fcLines = 0;              // 21j-r23: [freecam] lines so far
constexpr int kFcMaxLines = 400;
unsigned long g_pxRuns = 0;     // 21j-r23: the FREE CAMERA / FAKE CONSOLE script's runs
int g_tutLines = 0;             // 21j-r21: [tutorials] lines so far
constexpr int kTutMaxLines = 200;
unsigned long g_tutRuns = 0;    // 21j-r21: the TUTORIALS script's runs
int g_modsLines = 0;            // 21j-r17: [mods] lines so far
constexpr int kModsMaxLines = 600;
unsigned long g_modsRuns = 0;   // 21j-r17: the MODS script's runs (one per Lua state that makes ifs_opt_contmain)
bool g_grassScanIni = true;     // 21j-r1: [Splitscreen] GrassScan (the grass scan, [grass] lines)
bool g_grassFixIni = true;      // 21j-r2: [Splitscreen] GrassFadeFix (the .prp grass gets its layer's own distance, as 1.2)
bool g_padSensBf2Ini = true;    // 21i-r13: [Splitscreen] PadSensStyle (1 = Battlefront II's sensitivity, 0 = the game's)
int g_aimP[4] = { 0, 0, 0, 0 }; // 21i-r10: players 2 - 4's auto aim ([1..3]; [0] unused - player 1 is the game's profile)
char g_mainIni[MAX_PATH + 32] = {}; // 21i-r10: BF1GameExt.ini (AutoAimP2 - P4 written back)
int g_optScanLines = 0;
constexpr int kOptScanMaxLines = 4000;
bool g_optScanned[2] = {};      // ifs_opt_general / ifs_opt_controller dumped this session
char* g_rpSrc = nullptr;        // 21h-r2: BF1GameExt_pause34.lua (21j-r29: console.lvl's, the file next to the exe, or the copy built in)
size_t g_rpLen = 0;
char g_rpFrom[MAX_PATH + 160] = {};
unsigned long g_rpRuns = 0;
int g_rpLuaLines = 0;
constexpr int kRpLuaMaxLines = 600;
int g_pauseLuaLines = 0;
constexpr int kPauseLuaMaxLines = 400;
unsigned long g_pauseProbes = 0;
int g_splitPlayersLogged = 0;   // 21g: the player count last put in the script's settings lines (logged when it changes)
char* g_splitSrc = nullptr;
size_t g_splitLen = 0;
char g_splitFrom[MAX_PATH + 160] = {};
unsigned long g_splitRuns = 0;
int g_splitLines = 0;
constexpr int kSplitMaxLines = 1500;

bool SplitBudget()
{
    if (g_splitLines < kSplitMaxLines)
    {
        ++g_splitLines;
        return true;
    }
    if (g_splitLines == kSplitMaxLines)
    {
        ++g_splitLines;
        Log("[splitmenu] line limit reached, going quiet");
    }
    return false;
}

// The folder of the game's exe (no trailing backslash).
bool ExeDir(char* out, size_t size)
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return false;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return false;
    *slash = 0;
    snprintf(out, size, "%s", exe);
    return true;
}

// 21j-r29: CONSOLE.LVL (header notes). Its scripts, read once - the bodies point into g_lvlBuf, kept for the game's run.
struct ConLvlScript
{
    char name[64];
    const char* body;   // as the game runs it: BODY's size - 1 bytes
    size_t len;
};
char* g_lvlBuf = nullptr;
ConLvlScript g_lvlScripts[16] = {};
int g_lvlCount = 0;
bool g_lvlTried = false;
char g_lvlPath[MAX_PATH + 40] = {};

// BFX-CONLVL-PARSE-BEGIN (21j-r29: also built and tested on its own)
uint32_t ConGet32(const uint8_t* p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

// The scr_ chunks of the .lvl in data[0, size), as the game's level loader reads them (GOG 1.3 0x00586180): each child at
// a 4-byte boundary, the children end where the next would not fit; in a scr_ its NAME (up to 0x100 characters, to its 0)
// and its BODY (size - 1 bytes, as ScriptCB_DoFile's runner passes it); any other part skipped. A scr_ without both is
// left out. Returns how many were put in out (at most max), -1 = not a .lvl (no "ucfb"), -2 = broken (a chunk runs
// past the one it is in).
int ConLvlParse(const uint8_t* data, size_t size, ConLvlScript* out, int max)
{
    if (size < 8 || memcmp(data, "ucfb", 4) != 0)
        return -1;
    size_t top = 8 + static_cast<size_t>(ConGet32(data + 4));
    if (top > size)
        top = size;
    int count = 0;
    size_t p = 8;
    for (;;)
    {
        p = (p + 3) & ~static_cast<size_t>(3);
        if (p >= top || p + 8 > top)
            break;
        const size_t n = ConGet32(data + p + 4);
        if (n > top - (p + 8))
            return -2;
        if (memcmp(data + p, "scr_", 4) == 0)
        {
            const size_t hi = p + 8 + n;
            ConLvlScript s = {};
            bool named = false;
            bool bodied = false;
            size_t q = p + 8;
            for (;;)
            {
                q = (q + 3) & ~static_cast<size_t>(3);
                if (q >= hi || q + 8 > hi)
                    break;
                const size_t m = ConGet32(data + q + 4);
                if (m > hi - (q + 8))
                    return -2;
                const uint8_t* c = data + q + 8;
                if (memcmp(data + q, "NAME", 4) == 0)
                {
                    const size_t lim = m < 0x100 ? m : 0x100;
                    size_t k = 0;
                    while (k < lim && k < sizeof(s.name) - 1 && c[k])
                    {
                        s.name[k] = static_cast<char>(c[k]);
                        ++k;
                    }
                    s.name[k] = 0;
                    named = true;
                }
                else if (memcmp(data + q, "BODY", 4) == 0)
                {
                    s.body = reinterpret_cast<const char*>(c);
                    s.len = m ? m - 1 : 0;
                    bodied = true;
                }
                q += 8 + m;
            }
            if (named && bodied && count < max)
                out[count++] = s;
        }
        p += 8 + n;
    }
    return count;
}

// Compiled Lua 4.0 (the first byte ESC, as the game's lua_dobuffer tells them apart - GOG 1.3 0x0065D43B): the mod tools'
// ScriptMunge makes these (luac), ConsoleMunge stores the text.
bool ScriptCompiled(const char* text, size_t len)
{
    return len && static_cast<unsigned char>(text[0]) == 0x1B;
}

// A script's file build into out ("" = none found): a text script's from its first line ("-- BF1GameExt_split.lua
// (file build p21j-r30)"); a compiled one keeps no comments - 21j-r30: from its gBFX_Build_* string ("BF1GameExt_split.lua
// (file build p21j-r30)", the script's second line), anywhere in it (string constants stay in compiled Lua).
void ScriptBuild(const char* text, size_t len, char* out, size_t outSize)
{
    out[0] = 0;
    static const char kKey[] = "(file build ";
    const size_t keyLen = sizeof(kKey) - 1;
    const size_t lim = ScriptCompiled(text, len) ? len : (len < 400 ? len : 400);
    for (size_t i = 0; i + keyLen < lim; ++i)
    {
        if (memcmp(text + i, kKey, keyLen) != 0)
            continue;
        size_t e = i + keyLen;
        while (e < lim && text[e] != ')' && text[e] != '\n' && e - (i + keyLen) < 40)
            ++e;
        if (e < lim && text[e] == ')')
            snprintf(out, outSize, "%.*s", static_cast<int>(e - (i + keyLen)), text + i + keyLen);
        return;
    }
}
// BFX-CONLVL-PARSE-END

// console.lvl, read at the first script load: its scripts listed in the log once.
void ConLvlLoad()
{
    if (g_lvlTried)
        return;
    g_lvlTried = true;
    char dir[MAX_PATH] = {};
    if (!ExeDir(dir, sizeof(dir)))
        return;
    snprintf(g_lvlPath, sizeof(g_lvlPath), "%s\\Data\\_LVL_PC\\console.lvl", dir);
    const HANDLE f = CreateFileA(g_lvlPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                 nullptr);
    if (f == INVALID_HANDLE_VALUE)
    {
        Log("[scripts] 21j-r29: no %s - the scripts come from the files next to the exe, else the DLL's own copies", g_lvlPath);
        return;
    }
    const DWORD size = GetFileSize(f, nullptr);
    bool read = false;
    if (size != INVALID_FILE_SIZE && size >= 8 && size < (16u << 20))
    {
        g_lvlBuf = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, size));
        DWORD got = 0;
        read = g_lvlBuf && ReadFile(f, g_lvlBuf, size, &got, nullptr) && got == size;
    }
    CloseHandle(f);
    const int n = read ? ConLvlParse(reinterpret_cast<const uint8_t*>(g_lvlBuf), size, g_lvlScripts, 16) : -3;
    if (n < 0)
    {
        Log("[scripts] 21j-r29: %s NOT USED - %s; the scripts come from the files next to the exe, else the DLL's own copies",
            g_lvlPath, n == -1 ? "not a .lvl (it does not start with \"ucfb\")"
                       : n == -2 ? "broken (a chunk runs past its end) - make it again with ConsoleMunge"
                                 : "cannot read it (or it is 16 MB or more)");
        if (g_lvlBuf)
            HeapFree(GetProcessHeap(), 0, g_lvlBuf);
        g_lvlBuf = nullptr;
        return;
    }
    g_lvlCount = n;
    char list[700] = {};
    size_t at = 0;
    for (int i = 0; i < n && at + 1 < sizeof(list); ++i)
    {
        char build[48];
        ScriptBuild(g_lvlScripts[i].body, g_lvlScripts[i].len, build, sizeof(build));
        const int w = snprintf(list + at, sizeof(list) - at, "%s%s (%lu bytes%s%s%s)", i ? ", " : "", g_lvlScripts[i].name,
                               static_cast<unsigned long>(g_lvlScripts[i].len),
                               ScriptCompiled(g_lvlScripts[i].body, g_lvlScripts[i].len) ? ", compiled" : ", text",
                               build[0] ? ", file build " : "", build[0] ? build : "");
        if (w < 0)
            break;
        at += static_cast<size_t>(w);
        if (at >= sizeof(list))
            at = sizeof(list) - 1;
    }
    Log("[scripts] 21j-r29: console.lvl: %s (%lu bytes) - %d script(s)%s%s", g_lvlPath, static_cast<unsigned long>(size), n,
        n ? ": " : " (nothing in it is used)", list);
}

// 21e (was PageLoad's own), 21j-r29: a script - console.lvl's script named lvlName if it has one, else the file at path
// (under 1 MB), else the copy built in - as a 0-terminated copy on the process heap; from = where it came from, for the
// caller's log line (with its file build). tag: the log lines' tag for the notes (the file next to the exe not used
// because console.lvl has the script; a file build that is not the DLL's own). nullptr: out of memory.
char* LoadScript(const char* tag, const char* lvlName, const char* path, const char* builtIn, size_t builtInLen, size_t* len,
                 char* from, size_t fromSize)
{
    const char* body = builtIn;
    size_t bodyLen = builtInLen;
    char* fileBuf = nullptr;
    int source = 0;   // 0 the copy built in, 1 the file, 2 console.lvl
    ConLvlLoad();
    for (int i = 0; i < g_lvlCount; ++i)
    {
        if (lstrcmpiA(g_lvlScripts[i].name, lvlName) == 0 && g_lvlScripts[i].len > 0 && g_lvlScripts[i].len < (1u << 20))
        {
            body = g_lvlScripts[i].body;
            bodyLen = g_lvlScripts[i].len;
            source = 2;
            break;
        }
    }
    if (source == 2)
    {
        if (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES)
            Log("%s 21j-r29: %s is there too - not used (console.lvl's %s comes first)", tag, path, lvlName);
    }
    else
    {
        const HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                     nullptr);
        if (f != INVALID_HANDLE_VALUE)
        {
            const DWORD size = GetFileSize(f, nullptr);
            if (size != INVALID_FILE_SIZE && size > 0 && size < (1u << 20))
            {
                fileBuf = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, size + 1));
                DWORD got = 0;
                if (fileBuf && ReadFile(f, fileBuf, size, &got, nullptr) && got == size)
                {
                    fileBuf[size] = 0;
                    body = fileBuf;
                    bodyLen = size;
                    source = 1;
                }
            }
            CloseHandle(f);
        }
    }
    char build[48];
    char ownBuild[48];
    ScriptBuild(body, bodyLen, build, sizeof(build));
    ScriptBuild(builtIn, builtInLen, ownBuild, sizeof(ownBuild));
    const bool compiled = ScriptCompiled(body, bodyLen);
    snprintf(from, fromSize, "%s%s (%s%s%s)", source == 2 ? "console.lvl's " : "",
             source == 2 ? lvlName : source == 1 ? path : "the copy built in", compiled ? "compiled Lua, " : "",
             build[0] ? "file build " : compiled ? "no file build in it - a script from before 21j-r30" : "no file build line",
             build);
    // a compiled script without the build string cannot be told apart: no note for it
    if (source != 0 && (build[0] || !compiled) && strcmp(build, ownBuild) != 0)
        Log("%s 21j-r29: NOTE - %s%s is file build %s, this DLL's own copy is file build %s: not the script this DLL was made "
            "with (%s)", tag, source == 2 ? "console.lvl's " : "", source == 2 ? lvlName : path, build[0] ? build : "(none)",
            ownBuild[0] ? ownBuild : "(none)",
            source == 2 ? "munge console.lvl again with the script that came with this DLL, unless you changed it on purpose"
                        : "replace the file with the one that came with this DLL, unless you changed it on purpose");
    char* copy = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, bodyLen + 1));
    if (copy)
    {
        memcpy(copy, body, bodyLen);
        copy[bodyLen] = 0;
        *len = bodyLen;
    }
    if (fileBuf)
        HeapFree(GetProcessHeap(), 0, fileBuf);
    return copy;
}

// DllMain: the INI switches and the script (the file next to the exe, or the copy built in).
void PageLoad()
{
    char dir[MAX_PATH] = {};
    if (!ExeDir(dir, sizeof(dir)))
        return;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", dir);
    g_pageIni = GetPrivateProfileIntA("Controls", "ControllerPage", 1, ini) != 0;
    g_pageL3Sprint = GetPrivateProfileIntA("Sprint", "L3SprintOnly", 1, ini) != 0;
    if (!g_pageIni)
    {
        Log("[ctrlpage] BF1GameExt.ini [Controls] ControllerPage=0: the controller page is left as the game has it");
        return;
    }

    char path[MAX_PATH + 40];
    snprintf(path, sizeof(path), "%s\\BF1GameExt_ctrlpage.lua", dir);
    g_pageSrc = LoadScript("[ctrlpage]", "BF1GameExt_ctrlpage", path, kPageScript, sizeof(kPageScript) - 1, &g_pageLen, g_pageFrom,
                           sizeof(g_pageFrom));
    if (!g_pageSrc)
    {
        Log("[ctrlpage] out of memory - the controller page is left as the game has it");
        return;
    }
    Log("[ctrlpage] controller page script: %s (%lu bytes), L3 shown as %s", g_pageFrom,
        static_cast<unsigned long>(g_pageLen), g_pageL3Sprint ? "SPRINT ([Sprint] L3SprintOnly=1)" : "a pad button");
}

// Runs Lua text with the game's Lua (lua_dobuffer). The Lua stack is left as it was (its top is put back if anything
// stayed on it). Returns lua_dobuffer's result (0 = ran). split: the split screen menu's script ([splitmenu] lines,
// its budget), else the controller page's ([ctrlpage]).
int RunLua(void* L, const char* text, size_t len, const char* chunk, bool split)
{
    const char* tag = split ? "[splitmenu]" : "[ctrlpage]";
    volatile uint32_t* top = static_cast<volatile uint32_t*>(L);
    const uint32_t top0 = *top;
    const int r = g_luaDoBuffer(L, text, len, chunk);
    const uint32_t top1 = *top;
    if (top1 != top0)
    {
        if (top1 > top0)
            *top = top0;
        if (split ? SplitBudget() : PageBudget())
            Log("%s %s left the Lua stack %s by %ld value(s)%s", tag, chunk, top1 > top0 ? "higher" : "LOWER",
                static_cast<long>((static_cast<int32_t>(top1) - static_cast<int32_t>(top0)) / 8),
                top1 > top0 ? " - put back" : " - left alone");
    }
    if (r != 0 && (split ? SplitBudget() : PageBudget()))
        Log("%s %lu ms: %s FAILED (lua_dobuffer %d) - the game's Lua error output has the message", tag,
            ScanMsSinceAttach(), chunk, r);
    return r;
}

// The page script, after its two settings lines: phase "build" (ScriptCB_AddIFScreen("ifs_opt_controller"), while the
// game makes the screen - the RESET button goes in) or "push" (ScriptCB_PushScreen, right before the page opens).
void PageRun(void* L, const char* phase)
{
    if (!g_luaDoBuffer || !g_pageSrc)
        return;
    ++g_pageRuns;
    char head[96];
    snprintf(head, sizeof(head), "gBFX_L3Sprint = %s\ngBFX_Phase = \"%s\"\n", g_pageL3Sprint ? "1" : "nil", phase);
    if (RunLua(L, head, strlen(head), "=BF1GameExt_ctrlpage settings", false) != 0)
        return;
    const int r = RunLua(L, g_pageSrc, g_pageLen, "=BF1GameExt_ctrlpage", false);
    if (r == 0 && g_pageRuns <= 4 && PageBudget())
        Log("[ctrlpage] %lu ms: script run (%s, run %lu)", ScanMsSinceAttach(), phase, g_pageRuns);
}

// 21d-r2: right after the game's ScriptCB_ResetControl (the PC controls page's RESET: keyboard / mouse of the shown tab)
// the same tab's pad layout goes back to the game's default too (the script's bfx_ResetPadTab, defined once the page
// script has run in this Lua state).
void PadResetAfterGameReset(void* L)
{
    if (!g_luaDoBuffer || !g_pageSrc)
        return;
    static const char kText[] =
        "if (bfx_ResetPadTab) then bfx_ResetPadTab(\"after the game's ScriptCB_ResetControl\") "
        "else ScriptCB_GetPlatform(\"BFX: the game's RESET ran before the page script - pad layout not reset\") end\n";
    RunLua(L, kText, sizeof(kText) - 1, "=BF1GameExt_ctrlpage reset", false);
}

// ---------------------------------------------------------------------------------------------------------
// Phase 21e: the SPLIT SCREEN entry of the main menu (2 players; 4 next). The 2020 shell scripts still have the
// console's split screen flow: ifs_main's "split" button (its Enter hides it on PC), whose Input_Accept sets split
// screen on (ScriptCB_SetSplitscreen(1), ifs_sp.bForSplitScreen) and opens ifs_split_profile, the console sign-in screen
// (each player picks a profile - ScriptCB_GetLoginList(.., 1) gives the active profile, a second one made by the game if
// there are fewer than 2, and the profile files; player 2 joins with START; START with both READY sets the sides,
// loads the profiles through ifs_saveop and opens ifs_sp: the game modes with a SPLIT SCREEN title, Instant Action ->
// ifs_split_map -> the map list -> the match). BF1GameExt_split.lua (built in below, or the file next to the exe) is
// run with the game's Lua while the game makes ifs_main / ifs_split_profile (ScriptCB_AddIFScreen) and when it opens
// them (ScriptCB_PushScreen): it shows the button and gives the sign-in screen player 2 on pad 2.
// The PC game gives the shell only its own pad, so pad 2 is read here (XInput: pad2.cpp's pick, or the first pad that
// answers and is not the game's) when the script asks - ScriptCB_GetPlatform("BFXQ: pad2") from the screen's Update
// gets one event back as a number (1 A, 2 B, 3 START, 4-7 up / down / left / right, the d-pad or the left stick, held
// directions repeat; 0 none), pushed with the game's own pushnumber (GOG 1.3 0x0056FC80, found through
// ScriptCB_GetFunctionIdForButtonId's call at +40h, byte-checked). Its log lines come in as ScriptCB_GetPlatform("BFXS:
// ...") - [splitmenu] lines. The split screen scripts' ScriptCB calls are in the [luacb] trace while ifs_split* /
// ifs_sp / ifs_saveop is up. [Splitscreen] MainMenuSplit=1 (0 = the main menu as the game has it), SplitSignIn=1 (0 = SPLIT
// SCREEN goes straight to the game modes).
const char* const kSplitPieces[] = {
R"BFXLUA(-- BF1GameExt_split.lua   (file build p21j-r30)
gBFX_Build_split = "BF1GameExt_split.lua (file build p21j-r30)" -- 21j-r30: the build as a string too: a compiled script (the mod tools'
-- ScriptMunge, console.lvl) keeps no comments, and BF1GameExt's log reads its build from this string there
-- 21j-r28: THE SIGN-IN'S SOUND AND MUSIC BACK ([Splitscreen] SignInSoundFix=1). On PC every profile switch in the shell
-- restarts the sound engine (the profile's sound settings are applied: the engine shut down, started again, core.lvl's
-- sounds read again) - the shell's own sounds (sound\shell.lvl: the menu clicks, the music and voice streams) and its movie
-- stream are gone after it. The PC's login and audio options screens know: ScriptCB_GetMixConfigChanged() says so and they
-- put the shell's sound back (ifs_opt_sound_closeShellSound / restoreShellSound). The console sign-in does not: its profile
-- list (ScriptCB_GetLoginList with split screen on makes player 2's profile current and back - two restarts) left the
-- sign-in silent, and the profile loading at its launch did it again - the music never came back (r27's [audio] lines:
-- every later SetShellMusic found no music stream). Now after both (and a cancelled load) the game's own restore runs when
-- the engine was restarted, the music starts again at once and the sign-in's background movie goes on.
-- 21j-r27: THE SIGN-IN'S BUTTONS. (1) BACK / ACCEPT show the pad's B / A for every player ([Splitscreen] SignInPadIcons=1):
-- the 2020 PC shell's btnb / btna are the mouse's right / left buttons - the quarters get the 2020 gamepad sheet's B / A
-- (gamepad_xbox360, gGamepadTexture's cells), set while the game makes the screen. (2) The BACK button at the bottom left
-- (the PC's own, CreateNegativeButton) works with the mouse ([Splitscreen] SignInMouseBack=1): the mouse over it, a click
-- leaves the sign-in as player 1's B on his list does (the screen's Input_Accept never looked at it); with 3 / 4 players it
-- is shown again (r1 hid it). A pad's A is never taken for the click (the game's last input says pad).
-- 21j-r8: PICK YOUR SIDE IN THE CONSOLE'S LOOK FOR 2 / 3 / 4 PLAYERS ([Splitscreen] GCPickLook=1, GCCommander2P=1). The
-- game's window gets a row of the two sides' pictures for every player - the console's two rows, players 3 / 4's made the
-- same way, four smaller slots with 3 / 4 players - the picked side lit, the other darker once he has locked in. The
-- PLAYERS list at the right (21g-r3's text) is now a box for each player in the class screen's look (the game's window,
-- its orange title bar): PLAYER n while he picks, then COMMANDER (the first who locked in on that side: he takes its turns
-- on the galaxy map) or PLAYER READY, the bar and his side's name green for the Rebels / Republic, red for the Empire /
-- CIS. With 2 players too: both lock in (A, B unlocks), player 1 launches; in co-op only the commander moves on the galaxy
-- map (GCCommander2P=0: the 2-player screen as 21f, the game's own galaxy). GCPickLook=0: 21g-r3's text list.
-- 21g-r4: GALACTIC CONQUEST - WHOSE TURN. (1) The AI's turn in co-op: with players 1 and 2 on one side the game's AI
-- moves the other side on the game's third joystick, whose team nothing ever set - the game's script stopped at every
-- AI move and the AI side never took its turn (2 players as well as 3 / 4 on one side). That joystick is put on the
-- AI's team now. (2) THE TURN SCANNER ([Splitscreen] GCTurnScan, log only, [gcturn] lines): on the galaxy map, who has
-- the turn and who moves that side (the player and his pad, or the AI), every change of it, every choice, every press
-- (whose, where it went, what the game made of it), the battle result and why the turn stays or passes (the game's rule:
-- the attacker attacks again while he wins), the game's script errors, and a turn that stands still.
-- 21g-r3: GALACTIC CONQUEST WITH 3 / 4 PLAYERS. PICK YOUR SIDE: every player picks a side (players 1 / 2 on their rows,
-- players 3 / 4 in the roster at the right, pads 3 / 4) and locks in with A (B unlocks); the first who locks in on a side
-- COMMANDS it - only he moves on the galaxy map and picks its bonuses (ifs_meta_main: the other players' presses are
-- dropped, players 3 / 4's commanders through their pads); both sides with players = VERSUS, one = CO-OP (the AI runs
-- the other). LAUNCH once everyone is locked in. BF1GameExt keeps the sides and commanders across the battles and locks
-- players 3 / 4 to their sides in them (the campaign: player 1's side). With 2 players: as 21f.
-- 21g-r2: (1) the 4-player sign-in's LAUNCH tells BF1GameExt how many signed in - the match has that many players (2 =
-- the 2-player split, 3 = the 3-player layout, 4 = quarters), and the pads of players 3 / 4 who signed in are theirs
-- (player 4 alone plays as player 3). (2) TEMPORARY PROFILES ([Splitscreen] TempProfiles): PLAYER 2 / 3 / 4 at the end of
-- the list where no profile has that name; players 2 / 3 / 4 start on their own; never saved (not in the profile screen):
-- player 2's is made by BF1GameExt at the launch ("BFXQ: tempprofile"), ifs_saveop does not save it; player 1 signs in
-- with a saved profile.
-- 21g-r1: THE 4-PLAYER SIGN-IN, as Battlefront II has it on the Xbox (with players 3 / 4 on - BF1GameExt.ini Players=3 / 4 -
-- and [Splitscreen] SignIn4=1): four quarters split by the game's own gray_rect line and a vertical one like it; player 1
-- top left, player 2 top right, player 3 bottom left, player 4 bottom right; each quarter PRESS START BUTTON TO JOIN ->
-- his profile list (BACK / ACCEPT) -> his profile's name over READY. Pads 3 and 4 join (BF1GameExt reads them). With 2
-- players the 2-player screen exactly as before. (The match's player count still comes from Players; 21g-r2: from
-- the players who sign in.)
-- 21f-r3: a conquest in progress (back from its battle, the game resumes it through ifs_meta_top) goes to the galaxy
-- again - the game's own Enter loads it - so the galaxy's save question comes up; PICK YOUR SIDE only for a new conquest
-- launched from the maps. (r2 showed PICK YOUR SIDE again after every battle.)
-- 21f-r2: Galactic Conquest rebuilt after the Classic Collection video: PICK YOUR SIDE (ifs_meta_top) instead of r1's
-- line on the maps screen; the split screen map list (4 conquests) as the Classic Collection has it.
-- The console's SPLIT SCREEN entry of the main menu, back on PC - 2 players (step 1; 4 players next).
-- 21f-r1: Galactic Conquest in split screen as the Classic Collection has it (below): the split screen map list ->
-- LAUNCH -> PICK YOUR SIDE (ifs_meta_top, the console's screen: each player picks his side - same side = CO-OP, other
-- sides = VERSUS) -> LAUNCH. Player 1 starts the sign-in on his own profile (by name when the list has no active slot 1).
-- (The campaign already puts both players on the mission's side; BF1GameExt skips the side screens while the game says
-- sides cannot be switched - [Splitscreen] SideLock.)
-- BF1GameExt runs this (Lua 4.0, the game's own compiler) with these lines in front of it:
--   gBFX_SplitSignIn  1 = SPLIT SCREEN opens the console sign-in screen (ifs_split_profile: each player picks a
--                     profile, player 2 joins with START on pad 2); nil = it goes straight to the split screen game
--                     modes ([Splitscreen] SplitSignIn)
--   gBFX_Screen       the screen the game is making or opening ("ifs_main" / "ifs_split_profile" / "ifs_meta_opts" /
--                     "ifs_meta_top")
--   gBFX_Phase        "build" (ScriptCB_AddIFScreen, while the shell scripts load) or "push" (ScriptCB_PushScreen)
--   gBFX_Players      21g: the players a split screen match has (2, or 3 / 4 with players 3 / 4 on)
--   gBFX_SignIn4      21g: 1 = the 4-player sign-in when gBFX_Players is 3 / 4 ([Splitscreen] SignIn4)
--   gBFX_TempProfiles 21g-r2: 1 = PLAYER 2 / 3 / 4 temporary profiles in the sign-in ([Splitscreen] TempProfiles, and
--                     BF1GameExt found the game's profiles)
--   gBFX_GCTurnScan   21g-r4: 1 = the galaxy turn scanner's [gcturn] lines ([Splitscreen] GCTurnScan)
--   gBFX_GCPickLook   21j-r8: 1 = PICK YOUR SIDE in the console's look: a row for each player, the player boxes
--                     ([Splitscreen] GCPickLook)
--   gBFX_GCCmd2       21j-r8: 1 = with 2 players both lock in and each side's commander moves it on the galaxy map
--                     ([Splitscreen] GCCommander2P)
--   gBFX_PadIcons     21j-r27: 1 = the sign-in's BACK / ACCEPT with the pad's B / A ([Splitscreen] SignInPadIcons)
--   gBFX_MouseBack    21j-r27: 1 = the sign-in's BACK button at the bottom left works with the mouse ([Splitscreen]
--                     SignInMouseBack)
--   gBFX_SoundFix     21j-r28: 1 = the shell's sound and music put back after the sign-in's profile switches
--                     ([Splitscreen] SignInSoundFix)
-- Every part below is put in once per Lua state (ifs_main.bfxsplit, ifs_split_profile.bfxpc, ifs_meta_opts.bfxgc,
-- ifs_meta_top.bfxtop), whichever run comes first.
--
-- What is in the shell already (the 2020 shell scripts): ifs_main has the "split" button (hidden on PC by its Enter);
-- its Input_Accept for it sets ifs_sp.bForSplitScreen, ScriptCB_SetSplitscreen(1) and opens ifs_split_profile, the
-- console sign-in screen (players' profiles; on success ScriptCB_PushScreen("ifs_sp") - the game modes with a SPLIT
-- SCREEN title, Instant Action -> ifs_split_map -> the map list -> the match).
-- What this adds:
--   ifs_main          the "split" button shown (its label in English if the game's text is missing)
--   ifs_split_profile player 1 = the game's own input (keyboard, mouse, the game's pad; A works as START where START
--                     is asked for: to launch); player 2 = pad 2, read by BF1GameExt (the PC game gives the shell only
--                     its own pad) and asked for each frame (ScriptCB_GetPlatform("BFXQ: pad2") -> an event number);
--                     player 1 starts on the profile he signed in with, player 2 on the second active profile;
--                     the texts in English where the game's are missing; everything logged
-- Its log lines come in as ScriptCB_GetPlatform("BFXS: ...") - [splitmenu] lines in BF1GameExt_split.log.

-- Shared (defined on every run)
function bfxs_Log(s)
	ScriptCB_GetPlatform("BFXS: " .. s)
end

-- 21g-r4: the galaxy turn scanner ([Splitscreen] GCTurnScan=1) - split screen only - and its lines ([gcturn] in
-- BF1GameExt_split.log)
function bfxt_On()
	return gBFX_GCTurnScan and ScriptCB_IsSplitscreen()
end

function bfxt_Log(s)
	if (bfxt_On()) then
		ScriptCB_GetPlatform("BFXT: " .. s)
	end
end

-- A question to BF1GameExt: a number back (-1 if nothing answered)
function bfxs_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

-- 1 if the game has the text, nil if not (ScriptCB_getlocalizestr's 2nd argument: nil when it is missing)
function bfxs_HasText(key)
	if (ScriptCB_getlocalizestr(key, 1)) then
		return 1
	end
	return nil
end

-- The game's text, or the English one when the game has none
function bfxs_SetText(obj, key, english)
	if (not obj) then
		return
	end
	if (bfxs_HasText(key)) then
		IFText_fnSetString(obj, key)
	else
		IFText_fnSetUString(obj, ScriptCB_tounicode(english))
		if (not gBFXS_Missing) then
			gBFXS_Missing = {}
		end
		if (not gBFXS_Missing[key]) then
			gBFXS_Missing[key] = 1
			bfxs_Log(format("text \"%s\" is not in the game - shown as \"%s\"", key, english))
		end
	end
end

-- ---------------------------------------------------------------------------------------------------------------------
-- ifs_main: the SPLIT SCREEN button
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_main and (not ifs_main.bfxsplit)) then
ifs_main.bfxsplit = 1

gBFXS_MainEnter = ifs_main.Enter
)BFXLUA",
R"BFXLUA(gBFXS_MainAccept = ifs_main.Input_Accept

ifs_main.Enter = function(this, bFwd)
	gBFXS_MainEnter(this, bFwd)
	if (this.buttons and this.buttons.split) then
		this.buttons.split.hidden = nil
		ShowHideVerticalButtons(this.buttons, ifsmain_vbutton_layout)
		if (this.buttons.split.label and (not bfxs_HasText("ifs.main.split"))) then
			IFText_fnSetUString(this.buttons.split.label, ScriptCB_tounicode("Split Screen"))
		end
		if (not gBFXS_MainShown) then
			gBFXS_MainShown = 1
			local where = "the console sign-in screen (ifs_split_profile)"
			if (not gBFX_SplitSignIn) then
				where = "the split screen game modes (ifs_sp) - SplitSignIn=0"
			elseif (not ifs_split_profile) then
				where = "the split screen game modes (ifs_sp) - ifs_split_profile is NOT in this shell"
			end
			local label = "the game's text"
			if (not bfxs_HasText("ifs.main.split")) then
				label = "\"Split Screen\" (the game's text ifs.main.split is missing)"
			end
			bfxs_Log(format("main menu: SPLIT SCREEN button shown, label %s; SPLIT SCREEN -> %s", label, where))
		end
	else
		bfxs_Log("main menu: NO \"split\" button in ifs_main - nothing shown")
	end
end

ifs_main.Input_Accept = function(this)
	if ((this.CurButton == "split") and ((not gBFX_SplitSignIn) or (not ifs_split_profile))) then
		-- straight to the game modes (as "sp", with split screen on)
		if (gShellScreen_fnDefaultInputAccept(this)) then
			return
		end
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxs_Log("main menu: SPLIT SCREEN picked - split screen ON, straight to the game modes (ifs_sp)")
		ifs_sp.bForSplitScreen = 1
		ScriptCB_SetSplitscreen(1)
		ScriptCB_SetInNetGame(nil)
		ifs_main_GotoLoginScreen(this, ifs_sp)
		return
	end
	if (this.CurButton == "split") then
		bfxs_Log("main menu: SPLIT SCREEN picked - split screen ON, the sign-in screen next (the game's own code)")
	end
	gBFXS_MainAccept(this)
end

bfxs_Log(format("ifs_main patched (%s, %s)", gBFX_Phase or "?", gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- ifs_split_profile: the console sign-in screen with player 2 on pad 2
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_split_profile and (not ifs_split_profile.bfxpc)) then
ifs_split_profile.bfxpc = 1

-- input events by number (BF1GameExt's pad 2 answers with these)
gBFXS_Name = { "Input_Accept", "Input_Back", "Input_Start", "Input_GeneralUp", "Input_GeneralDown", "Input_GeneralLeft",
	"Input_GeneralRight" }
gBFXS_Code = { Input_Accept = 1, Input_Back = 2, Input_Start = 3, Input_GeneralUp = 4, Input_GeneralDown = 5,
	Input_GeneralLeft = 6, Input_GeneralRight = 7 }
gBFXS_Short = { "A", "B", "START", "UP", "DOWN", "LEFT", "RIGHT" }
gBFXS_Clock = 0
gBFXS_LastPadCode = 0
gBFXS_LastPadTime = -10
gBFXS_GameP2 = nil   -- 1 once the game itself gives player 2's pad (Joystick 1): then BF1GameExt's is not used
gBFXS_Inside = nil   -- 1 while a handler runs: the screen's own calls (Input_Start -> Input_Accept) go straight on
gBFXS_Conn = -2
gBFXS_PadCheck = 0

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g: THE 4-PLAYER SIGN-IN (Battlefront II's on the Xbox) - with players 3 / 4 on (BF1GameExt.ini Players=3 / 4) and
-- [Splitscreen] SignIn4=1. The screen in four quarters, split by the game's own middle line (gray_rect tinted 192 / 192
-- / 255 at 0.8 - the texture, tint and thickness of Battlefront II's lines) and a vertical one made the same way:
-- player 1 top left, player 2 top right, player 3 bottom left, player 4 bottom right (their places in the match). Each
-- quarter: PRESS START BUTTON TO JOIN -> the profile list with BACK / ACCEPT -> his profile's name over READY, at
-- Battlefront II's places. Players 1 and 2 keep the game's own pieces (moved into their quarters while the screen is
-- made); players 3 and 4 get copies of them, run here (pads 3 and 4 read by BF1GameExt: "BFXQ: pad3" / "pad4"). A
-- profile READY for one player is greyed for the others and cannot be picked twice. START launches once players 1 and
-- 2 are READY and every other player who joined is READY too. Players 3 and 4's profiles are names only (the game
-- loads 2 profiles). With Players=3 the bottom right quarter stays empty.
-- ---------------------------------------------------------------------------------------------------------------------
gBFXS_Four = nil       -- 1: the 4-player sign-in was built into this screen
gBFXS_Players = 2      -- players in the match (2 / 3 / 4, from BF1GameExt)
gBFXS_Mode = {}        -- players 3 / 4: -1 hidden, 0 PRESS START, 1 picking his profile, 3 READY
gBFXS_Layout = {}      -- players 3 / 4's list layouts (as the game's layoutT)
gBFXS_GameP = {}       -- players 3 / 4: 1 once the game itself gives his pad (Joystick 2 / 3)
gBFXS_PadCode = {}     -- players 3 / 4: BF1GameExt's last event and when (not done twice when the game gives it too)
gBFXS_PadTime = {}
gBFXS_PadConn = {}
gBFXS_Pulse = 1
gBFXS_PulseDir = 1
-- each player's places (fractions of the screen): the centre of his pieces, his JOIN / READY line, his list, his
-- BACK / ACCEPT line (y -20 under it, as the game's)
gBFXS_QX = { 0.34, 0.66, 0.34, 0.66 }
gBFXS_QText = { 0.223, 0.223, 0.648, 0.648 }
gBFXS_QList = { 0.267, 0.267, 0.692, 0.692 }
gBFXS_QHelp = { 0.5, 0.5, 0.93, 0.93 }

function bfxs4_ModeName(m)
	if (m == 0) then
		return "PRESS START"
	elseif (m == 1) then
		return "picking his profile"
	elseif (m == 3) then
		return "READY"
	end
	return "-"
end

-- player p's mode, list layout and list (1 / 2: the game's, 3 / 4: ours)
function bfxs4_State(this, p)
	if (p == 1) then
		return this.ModeT, ifs_split_profile_listbox_layoutT, this.ProfileT.listbox
	elseif (p == 2) then
		return this.ModeB, ifs_split_profile_listbox_layoutB, this.ProfileB.listbox
	end
	local c = this["bfx" .. p .. "Profile"]
	if (c) then
		return gBFXS_Mode[p], gBFXS_Layout[p], c.listbox
	end
	return nil, nil, nil
end

-- the other player (not p) on that row: strict - READY on it; else - joined and on it too
function bfxs4_UsedBy(this, row, p, strict)
	if (not row) then
		return nil
	end
	local q
	for q = 1, gBFXS_Players do
		if (q ~= p) then
			local m, lay = bfxs4_State(this, q)
			if (m and lay and (lay.SelectedIdx == row)) then
				if ((m == 3) or ((not strict) and (m >= 1) and (m <= 3))) then
					return q
				end
			end
		end
	end
	return nil
end

function bfxs4_HeldBy(this, row, p)
	return bfxs4_UsedBy(this, row, p, 1)
end

-- a player 3 / 4 who joined and is still picking (nil: none)
function bfxs4_Picking()
	local p
	for p = 3, gBFXS_Players do
		if (gBFXS_Mode[p] == 1) then
			return p
		end
	end
	return nil
end

-- a row for a player who joins: one nobody else is on, else one nobody holds READY
function bfxs4_FreeRow(this, p)
	local n = getn(ifs_split_profile_listbox_contents)
	local row
	for row = 1, n do
		if (not bfxs4_UsedBy(this, row, p, nil)) then
			return row
		end
	end
	for row = 1, n do
		if (not bfxs4_HeldBy(this, row, p)) then
			return row
		end
	end
	return 1
end

-- a list call for player 3 / 4: the game's list globals kept as they were (its cursor pulse and mouse wheel follow
-- players 1 and 2's lists only)
function bfxs4_ListCall(this, p, fn)
	local keep1 = gCurHiliteListbox
	local keep2 = gCurHiliteListbox2
	local keepL = gCurListboxLayout
	fn(this["bfx" .. p .. "Profile"].listbox, ifs_split_profile_listbox_contents, gBFXS_Layout[p])
	gCurHiliteListbox = keep1
	gCurHiliteListbox2 = keep2
	gCurListboxLayout = keepL
end

-- a name over READY: the profile on the player's row
function bfxs4_Name(obj, lay, on)
	if (not obj) then
		return
	end
	if (on and lay and lay.SelectedIdx) then
		local sel = ifs_split_profile_listbox_contents[lay.SelectedIdx]
		if (sel and sel.showstr) then
			IFText_fnSetUString(obj, sel.showstr)
			IFObj_fnSetVis(obj, 1)
			return
		end
	end
	IFObj_fnSetVis(obj, nil)
end

-- every list's rows: grey where another player holds that profile READY (the game does it for players 1 and 2 only)
function bfxs4_Grey(this)
	local n = getn(ifs_split_profile_listbox_contents)
	local p
	for p = 1, gBFXS_Players do
		local m, lay, list = bfxs4_State(this, p)
		if (lay and list and lay.showcount) then
			local first = max(lay.FirstShownIdx or 1, 1)
			local last = min(n, first - 1 + lay.showcount)
			local cnt
			for cnt = first, last do
				if (list[cnt - first + 1]) then
					if (bfxs4_HeldBy(this, cnt, p)) then
						IFObj_fnSetColor(list[cnt - first + 1], 128, 128, 128)
					else
						IFObj_fnSetColor(list[cnt - first + 1], 255, 255, 255)
					end
				end
			end
		end
	end
end

-- after the game's own ifs_split_profile_fnSetPieceVis: the names, players 3 / 4's pieces, the launch prompt
function bfxs4_Vis(this)
	local busy = (not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)
	bfxs4_Name(this.bfxNameT, ifs_split_profile_listbox_layoutT, (not busy) and (this.ModeT == 3))
	bfxs4_Name(this.bfxNameB, ifs_split_profile_listbox_layoutB, (not busy) and (this.ModeB == 3))
	local p
	for p = 3, 4 do
		local m = gBFXS_Mode[p] or -1
		local on = (not busy) and (p <= gBFXS_Players) and (m >= 0)
		IFObj_fnSetVis(this["bfx" .. p .. "Press"], on and (m == 0))
		IFObj_fnSetVis(this["bfx" .. p .. "Profile"], on and (m == 1))
		IFObj_fnSetVis(this["bfx" .. p .. "Ready"], on and (m == 3))
		bfxs4_Name(this["bfx" .. p .. "Name"], gBFXS_Layout[p], on and (m == 3))
		IFObj_fnSetVis(this["bfx" .. p .. "Back"], on and (m >= 1))
		IFObj_fnSetVis(this["bfx" .. p .. "Accept"], on and (m == 1))
		if (on and (m == 1)) then
			bfxs4_ListCall(this, p, ListManager_fnFillContents)
		end
	end
	-- the launch prompt: players 1 and 2 READY and nobody still picking
	IFObj_fnSetVis(this.pressstart, (not busy) and (this.ModeT == 3) and (this.ModeB == 3) and (not bfxs4_Picking()))
	-- the PC's own BACK button at the bottom left: 21j-r27 - on again (gBFX_MouseBack: the mouse's way out of the sign-in,
	-- under player 3's quarter's own BACK / ACCEPT line); without it hidden as r1 had it (player 1 has his BACK in his quarter)
	if (this.Helptext_Back) then
		IFObj_fnSetVis(this.Helptext_Back, gBFX_MouseBack and (not busy))
	end
	if (not busy) then
		bfxs4_Grey(this)
	end
end

-- players 1 and 2's presses the 4-player screen refuses (1 = refused): READY on a profile player 3 / 4 holds; the launch
-- while player 3 / 4 is still picking
function bfxs4_Gate(this, name, joy)
	local p = 1
	local m = this.ModeT
	local lay = ifs_split_profile_listbox_layoutT
	if (joy > 0.5) then
		p = 2
		m = this.ModeB
		lay = ifs_split_profile_listbox_layoutB
	end
	if (((name == "Input_Accept") or (name == "Input_Start")) and ((m == 1) or (m == 2))) then
		local q = bfxs4_HeldBy(this, lay.SelectedIdx, p)
		if (q and (q > 2)) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player %d: row %s (%s) is player %d's READY profile - not READY", p, tostring(lay.SelectedIdx),
				bfxs_ProfileName(ifs_split_profile_listbox_contents[lay.SelectedIdx or 0]), q))
			return 1
		end
	end
	if ((this.ModeT == 3) and (this.ModeB == 3) and ((name == "Input_Start") or ((name == "Input_Accept") and (p == 1)))) then
		local q = bfxs4_Picking()
		if (q) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player %d: START - player %d is still picking his profile: no launch yet", p, q))
			return 1
		end
	end
	return nil
end

function bfxs4_Join(this, p)
	local lay = gBFXS_Layout[p]
	local row = bfxs_OwnRow(this, p) or bfxs4_FreeRow(this, p) -- 21g-r2: PLAYER p first
	gBFXS_Mode[p] = 1
	lay.SelectedIdx = row
	lay.CursorIdx = nil -- (the PC's mouse-over box stays off: the mouse is player 1's)
	lay.FirstShownIdx = 1
	if (row > lay.showcount) then
		lay.FirstShownIdx = row - lay.showcount + 1
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
)BFXLUA",
R"BFXLUA(	bfxs_Log(format("player %d joined - on row %d (%s)", p, row, bfxs_ProfileName(ifs_split_profile_listbox_contents[row])))
end

function bfxs4_Ready(this, p)
	local lay = gBFXS_Layout[p]
	local row = lay.SelectedIdx
	local sel = ifs_split_profile_listbox_contents[row or 0]
	if ((not sel) or (not sel.showstr)) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxs_Log(format("player %d: no profile on row %s - not READY", p, tostring(row)))
		return
	end
	local q = bfxs4_HeldBy(this, row, p)
	if (q) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxs_Log(format("player %d: row %d (%s) is player %d's READY profile - not READY", p, row, bfxs_ProfileName(sel), q))
		return
	end
	gBFXS_Mode[p] = 3
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
end

-- one input for player 3 / 4
function bfxs4_Do(this, name, p)
	local m = gBFXS_Mode[p]
	if ((not m) or (m < 0) or (p > gBFXS_Players)) then
		return
	end
	local lay = gBFXS_Layout[p]
	if ((name == "Input_Start") or (name == "Input_Accept")) then
		if (m == 0) then
			bfxs4_Join(this, p)
		elseif (m == 1) then
			bfxs4_Ready(this, p)
		elseif ((m == 3) and (name == "Input_Start") and (this.ModeT == 3) and (this.ModeB == 3)) then
			-- his START launches too (as player 2's)
			bfxs_Log(format("player %d: START - the launch (player 1's START)", p))
			bfxs_Do(this, "Input_Start", 0)
			return
		end
	elseif (name == "Input_Back") then
		if (m == 3) then
			gBFXS_Mode[p] = 1
			ifelm_shellscreen_fnPlaySound(this.exitSound)
		elseif (m == 1) then
			gBFXS_Mode[p] = 0
			lay.SelectedIdx = nil
			lay.CursorIdx = nil
			ifelm_shellscreen_fnPlaySound(this.exitSound)
		end
	elseif ((name == "Input_GeneralUp") and (m == 1)) then
		bfxs4_ListCall(this, p, ListManager_fnNavUp)
	elseif ((name == "Input_GeneralDown") and (m == 1)) then
		bfxs4_ListCall(this, p, ListManager_fnNavDown)
	end
	ifs_split_profile_fnSetPieceVis(this)
	if (gBFXS_Mode[p] ~= m) then
		local sel = ""
		if (gBFXS_Mode[p] >= 1) then
			sel = format(" (row %s, %s)", tostring(lay.SelectedIdx),
				bfxs_ProfileName(ifs_split_profile_listbox_contents[lay.SelectedIdx or 0]))
		end
		bfxs_Log(format("player %d %s: %s -> %s%s", p, gBFXS_Short[gBFXS_Code[name]], bfxs4_ModeName(m),
			bfxs4_ModeName(gBFXS_Mode[p]), sel))
	end
end

-- the game itself giving player 3's / 4's pad (Joystick 2 / 3): BF1GameExt's pad for him is not read from then on
function bfxs4_GameInput(this, name, p, jtxt)
	if (not gBFXS_GameP[p]) then
		gBFXS_GameP[p] = 1
		bfxs_Log(format("game: %s(Joystick %s) - the game itself gives player %d's pad: BF1GameExt's pad %d is not read "
			.. "here from now on", name, jtxt, p, p))
		if ((gBFXS_PadCode[p] == gBFXS_Code[name]) and (gBFXS_Clock - (gBFXS_PadTime[p] or -10) < 0.5)) then
			bfxs_Log("  (the same press came from BF1GameExt's pad just before - not done twice)")
			return
		end
	else
		bfxs_Log(format("game: %s(Joystick %s) -> player %d", name, jtxt, p))
	end
	bfxs4_Do(this, name, p)
end

-- players 3 / 4 back to PRESS START (the profile list was just loaded)
function bfxs4_Reset(this)
	local p
	for p = 3, 4 do
		gBFXS_Mode[p] = 0
		gBFXS_GameP[p] = nil
		gBFXS_PadConn[p] = -2
		gBFXS_Layout[p].SelectedIdx = nil
		gBFXS_Layout[p].CursorIdx = nil
		gBFXS_Layout[p].FirstShownIdx = 1
		bfxs_Ask(format("pad%dreset", p))
	end
end

-- pads 3 / 4: connected or not, logged when it changes
function bfxs4_PadLog()
	local p
	for p = 3, gBFXS_Players do
		local conn = bfxs_Ask(format("pad%dconn", p))
		if (conn ~= gBFXS_PadConn[p]) then
			gBFXS_PadConn[p] = conn
			if (conn > 0) then
				bfxs_Log(format("pad %d is connected (XInput %d) - player %d can join", p, bfxs_Ask(format("pad%dindex", p)), p))
			elseif (conn == 0) then
				bfxs_Log(format("pad %d is NOT connected - player %d cannot join until it is", p, p))
			else
				bfxs_Log(format("pad %d: BF1GameExt did not answer", p))
			end
		end
	end
end

-- each frame: pads 3 / 4's presses, players 3 / 4's list cursors pulsing as the game's
function bfxs4_Update(this, fDt)
	if ((not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)) then
		return
	end
	local p
	for p = 3, gBFXS_Players do
		if (not gBFXS_GameP[p]) then
			local n = 0
			local e = bfxs_Ask(format("pad%d", p))
			while ((e > 0) and (n < 8)) do
				local name = gBFXS_Name[e]
				if (name) then
					gBFXS_PadCode[p] = e
					gBFXS_PadTime[p] = gBFXS_Clock
					bfxs_Log(format("pad %d: %s -> player %d (%s)", p, gBFXS_Short[e], p, bfxs4_ModeName(gBFXS_Mode[p])))
					bfxs4_Do(this, name, p)
				end
				n = n + 1
				e = 0
				if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4)) then
					e = bfxs_Ask(format("pad%d", p))
				end
			end
		end
	end
	gBFXS_Pulse = gBFXS_Pulse + fDt * gBFXS_PulseDir
	if (gBFXS_Pulse > 1) then
		gBFXS_Pulse = 1
		gBFXS_PulseDir = -1
	elseif (gBFXS_Pulse < 0.5) then
		gBFXS_Pulse = 0.5
		gBFXS_PulseDir = 1
	end
	for p = 3, gBFXS_Players do
		if (gBFXS_Mode[p] == 1) then
			local c = this["bfx" .. p .. "Profile"]
			if (c and c.listbox and c.listbox.cursor) then
				IFObj_fnSetAlpha(c.listbox.cursor, gBFXS_Pulse)
			end
		end
	end
end

-- texts in English where the game has none
function bfxs4_FixTexts(this)
	local p
	for p = 3, 4 do
		bfxs_SetText(this["bfx" .. p .. "Press"], "ifs.split.starttojoin", "Press START to join")
		bfxs_SetText(this["bfx" .. p .. "Ready"], "ifs.split.ready", "Ready")
		bfxs_SetText(this["bfx" .. p .. "Accept"].helpstr, "common.accept", "Accept")
		gIFShellScreenTemplate_fnMoveIcon(this["bfx" .. p .. "Accept"])
	end
end

-- 21j-r27: an icon of a BACK / ACCEPT pair made the pad's B / A (gBFX_PadIcons): the 2020 gamepad sheet (gamepad_xbox360) at
-- gGamepadTexture's cell for it, a square 22 units tall where the game's icon was (ACCEPT's icon is placed again by the
-- game's fnMoveIcon, 20 x 20). Only before the game makes the icon (its table is read then). 1 = done.
function bfxs_PadIcon(icon, which)
	if ((not gBFX_PadIcons) or (not icon) or (not gGamepadTexture) or (not gGamepadTexture[which])) then
		return nil
	end
	local g = gGamepadTexture[which]
	local cx = ((icon.localpos_l or 0) + (icon.localpos_r or 0)) * 0.5
	local cy = ((icon.localpos_t or 0) + (icon.localpos_b or 0)) * 0.5
	icon.texture = "gamepad_xbox360"
	icon.uvs_l = g.ul
	icon.uvs_t = g.ut
	icon.uvs_r = g.ur
	icon.uvs_b = g.ub
	icon.localpos_l = cx - 11
	icon.localpos_r = cx + 11
	icon.localpos_t = cy - 11
	icon.localpos_b = cy + 11
	return 1
end

-- 21j-r27: players 1 and 2's BACK / ACCEPT (the game's BackGroupT / B, AcceptGroupT / B) with the pad's B / A - while the
-- game makes the screen
function bfxs_PadIcons(this)
	if (not gBFX_PadIcons) then
		return
	end
	local n = 0
	local k, v
	for k, v in { BackGroupT = "B", BackGroupB = "B", AcceptGroupT = "A", AcceptGroupB = "A" } do
		if (this[k] and bfxs_PadIcon(this[k].icon, v)) then
			n = n + 1
		end
	end
	bfxs_Log(format("21j-r27: players 1 and 2's BACK / ACCEPT: %d of 4 icons the pad's B / A (gamepad_xbox360)%s", n,
		gGamepadTexture and "" or " - the game's gGamepadTexture is missing"))
end

-- 21j-r27: the last input was a pad's (ScriptCB_IsLastInputFromJoystick: 1 / non-zero) - nil for the mouse / keyboard
function bfxs_LastFromPad()
	local r = ScriptCB_IsLastInputFromJoystick()
	if (r and (r ~= 0)) then
		return 1
	end
	return nil
end

-- 21j-r27: a click on the BACK button at the bottom left - player 1's Input_Accept with the mouse over it (the game made it the
-- screen's CurButton "_back"), never a pad's A
function bfxs_MouseBackClick(this, name, Joystick)
	if ((not gBFX_MouseBack) or (name ~= "Input_Accept") or (this.CurButton ~= "_back")) then
		return nil
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		return nil
	end
	if (bfxs_LastFromPad()) then
		return nil
	end
	return 1
end

-- 21j-r27: the sign-in left with the mouse's BACK - as player 1's B on his profile list (the game's own lines), whatever
-- the players are on
function bfxs_MouseBack(this)
	bfxs_Log(format("mouse: BACK clicked (%s) - the sign-in screen closed, as player 1's B on his list", bfxs_Modes(this)))
	if (gBFXS_Four) then
		gBFXS_Mode[3] = -1
		gBFXS_Mode[4] = -1
	end
	this.CurButton = nil
	ScriptCB_UnbindController(2) -- (the game's: don't care about the second controller anymore)
	ScriptCB_ReadAllControllers(nil)
	ScriptCB_PopScreen()
end

-- 21j-r28: the shell's sound back after a profile switch (gBFX_SoundFix) - as the game's login does it: when
-- ScriptCB_GetMixConfigChanged() says the sound engine was restarted (it clears that), the shell's sound data, its voice /
-- music streams and its movie stream again (ifs_opt_sound_closeShellSound / restoreShellSound), the music started again
-- at once (its stream is new; the game's own music update would start it only after its 10 s interval) and the background
-- movie bgMovie again (looped, as the screen's own)
function bfxs_SoundRestore(where, bgMovie)
	if ((not gBFX_SoundFix) or (not ScriptCB_GetMixConfigChanged)) then
		return
	end
	local r = ScriptCB_GetMixConfigChanged()
	if ((not r) or (r == 0)) then
		bfxs_Log(format("sound: %s - the sound engine was not restarted, nothing to put back", where))
		return
	end
	if ((not ifs_opt_sound_closeShellSound) or (not ifs_opt_sound_restoreShellSound)) then
		bfxs_Log(format("sound: %s - the sound engine WAS restarted, but the game's restore (ifs_opt_sound) is not in this shell",
			where))
		return
	end
	ifs_opt_sound_closeShellSound(nil)
	ifs_opt_sound_restoreShellSound(nil)
	local music = (ifs_sp and ifs_sp.music) or "shell_soundtrack"
	ScriptCB_SetShellMusic()
	ScriptCB_SetShellMusic(music)
	local movie = "none"
	if (bgMovie) then
		local keep = gMovieDisabled
		gMovieDisabled = nil
		ifelem_shellscreen_fnStartMovie(bgMovie, 1, nil, 1)
		gMovieDisabled = keep
		movie = bgMovie
	end
	bfxs_Log(format("sound: %s - the sound engine was restarted (a profile switch): the shell's sounds, music and movie streams "
		.. "put back (the game's own restore, as its login does), music %s again, background movie %s", where, music, movie))
end

-- a BACK / ACCEPT pair as the game's (BackGroupT / AcceptGroupT); 21j-r27: the pad's B / A (gBFX_PadIcons)
function bfxs4_BackGroup(sx, sy, dx)
	local c = NewIFContainer {
		ScreenRelativeX = sx,
		ScreenRelativeY = sy,
		x = dx,
		y = -20,
		icon = NewIFImage { ZPos = 200, texture = "btnb", localpos_l = 0, localpos_t = -11, localpos_r = 20, localpos_b = 11,
			inert = 1 },
		helpstr = NewIFText { string = "common.back", font = "gamefont_medium", textw = 460, x = 25, y = -15, halign = "left",
			inert = 1, nocreatebackground = 1 },
	}
	bfxs_PadIcon(c.icon, "B")
	return c
end

function bfxs4_AcceptGroup(sx, sy, dx)
	local c = NewIFContainer {
		ScreenRelativeX = sx,
		ScreenRelativeY = sy,
		x = dx,
		y = -20,
		icon = NewIFImage { ZPos = 200, texture = "btna", localpos_l = -10, localpos_t = -11, localpos_r = 10, localpos_b = 11 },
		helpstr = NewIFText { string = "common.accept", font = "gamefont_medium", textw = 460, x = -460, y = -15,
			halign = "right", nocreatebackground = 1 },
	}
	bfxs_PadIcon(c.icon, "A")
	return c
end

-- While the game makes the screen (ScriptCB_AddIFScreen, before its pieces exist in the game): players 1 and 2's pieces
-- moved into the top quarters, players 3 and 4's made, the vertical line.
function bfxs4_Build(this)
	local r, b = ScriptCB_GetSafeScreenInfo()
	local lw = this.ProfileT.listbox.width
	local lh = this.ProfileT.listbox.height
	local QX = gBFXS_QX
	local QT = gBFXS_QText
	local QL = gBFXS_QList
	local QH = gBFXS_QHelp
	-- the vertical line: the game's middle line (ifssplit_profile_reposition_items) stood up
	this.bfxvline = NewIFImage {
		texture = "gray_rect",
		alpha = 0.8,
		localpos_l = r * 0.5 - 2, localpos_t = -100,
		localpos_r = r * 0.5 + 2, localpos_b = b + 100,
		inertUVs = 1,
		ScreenRelativeX = 0,
		ScreenRelativeY = 0,
		ColorR = 192, ColorG = 192, ColorB = 255,
	}
	-- player 1: top left; player 2: top right
	this.pressstartT.ScreenRelativeX = QX[1]
	this.pressstartT.ScreenRelativeY = QT[1]
)BFXLUA",
R"BFXLUA(	this.pressstartB.ScreenRelativeX = QX[2]
	this.pressstartB.ScreenRelativeY = QT[2]
	this.ReadyT.ScreenRelativeX = QX[1]
	this.ReadyT.ScreenRelativeY = QT[1]
	this.ReadyB.ScreenRelativeX = QX[2]
	this.ReadyB.ScreenRelativeY = QT[2]
	this.ProfileT.ScreenRelativeX = QX[1]
	this.ProfileT.listbox.y = b * QL[1]
	this.ProfileB.ScreenRelativeX = QX[2]
	this.ProfileB.listbox.y = b * QL[2]
	this.BackGroupT.ScreenRelativeX = QX[1]
	this.BackGroupT.ScreenRelativeY = QH[1]
	this.BackGroupT.x = lw * -0.5
	this.AcceptGroupT.ScreenRelativeX = QX[1]
	this.AcceptGroupT.ScreenRelativeY = QH[1]
	this.AcceptGroupT.x = lw * 0.5
	this.BackGroupB.ScreenRelativeX = QX[2]
	this.BackGroupB.ScreenRelativeY = QH[2]
	this.BackGroupB.x = lw * -0.5
	this.AcceptGroupB.ScreenRelativeX = QX[2]
	this.AcceptGroupB.ScreenRelativeY = QH[2]
	this.AcceptGroupB.x = lw * 0.5
	-- PRESS START BUTTON TO LAUNCH: under player 1's READY (his START launches), a little to the left (the long line
	-- clears the vertical one)
	this.pressstart.ScreenRelativeX = QX[1] - 0.01
	this.pressstart.ScreenRelativeY = QT[1]
	this.pressstart.y = 28
	-- the profile names over READY (Battlefront II: "Player 1" / "Ready")
	this.bfxNameT = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38, ScreenRelativeX = QX[1],
		ScreenRelativeY = QT[1], nocreatebackground = 1 }
	this.bfxNameB = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38, ScreenRelativeX = QX[2],
		ScreenRelativeY = QT[2], nocreatebackground = 1 }
	-- players 3 and 4: copies of the game's pieces in the bottom quarters
	local p
	for p = 3, 4 do
		this["bfx" .. p .. "Press"] = NewIFText { string = "ifs.split.starttojoin", font = "gamefont_medium", valign = "vcenter",
			textw = 460, texth = 120, ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		this["bfx" .. p .. "Ready"] = NewIFText { string = "ifs.split.ready", font = "gamefont_large", textw = 460, y = -8,
			ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		this["bfx" .. p .. "Name"] = NewIFText { string = "", font = "gamefont_large", textw = 460, y = -38,
			ScreenRelativeX = QX[p], ScreenRelativeY = QT[p], nocreatebackground = 1 }
		local t = ifs_split_profile_listbox_layoutT
		local lay = { showcount = t.showcount, yHeight = t.yHeight, ySpacing = t.ySpacing, width = t.width, slider = t.slider,
			CreateFn = t.CreateFn, PopulateFn = t.PopulateFn }
		gBFXS_Layout[p] = lay
		local c = NewIFContainer {
			ScreenRelativeX = QX[p],
			ScreenRelativeY = 0.0,
			listbox = NewButtonWindow { ZPos = 200, x = 0, y = b * QL[p], width = lw, height = lh },
		}
		ListManager_fnInitList(c.listbox, lay)
		-- no mouse spots on them (the mouse is player 1's)
		local i
		for i = 1, lay.showcount do
			if (c.listbox[i]) then
				c.listbox[i].bHotspot = nil
			end
		end
		if (c.listbox.sliderbg) then
			c.listbox.sliderbg.bHotspot = nil
		end
		this["bfx" .. p .. "Profile"] = c
		this["bfx" .. p .. "Back"] = bfxs4_BackGroup(QX[p], QH[p], lw * -0.5)
		this["bfx" .. p .. "Accept"] = bfxs4_AcceptGroup(QX[p], QH[p], lw * 0.5)
		gBFXS_Mode[p] = -1
	end
	bfxs_Log(format("4-player sign-in built (%d players): four quarters - the game's middle line and a vertical one (gray_rect, "
		.. "192/192/255 at 0.8, 4 units); screen %s x %s, lists %s x %s at x %.2f / %.2f, y %.3f / %.3f of the screen",
		gBFXS_Players, tostring(r), tostring(b), tostring(lw), tostring(lh), QX[1], QX[2], QL[1], QL[3]))
end


gBFXS_Orig = {
	Enter = ifs_split_profile.Enter,
	Exit = ifs_split_profile.Exit,
	Update = ifs_split_profile.Update,
}

function bfxs_Modes(this)
	return format("ModeT %s ModeB %s", tostring(this.ModeT), tostring(this.ModeB))
end

function bfxs_ProfileName(entry)
	if (entry and entry.showstr) then
		return ScriptCB_ununicode(entry.showstr)
	end
	return "?"
end

-- the sign-in list, logged
function bfxs_ListLog()
	local list = ifs_split_profile_listbox_contents
	local s = ""
	local i = 1
	while (list[i]) do
		local slot = "file"
		if (list[i].memslot) then
			slot = format("active slot %s", tostring(list[i].memslot))
		end
		s = s .. format("  [%d] %s (%s)", i, bfxs_ProfileName(list[i]), slot)
		i = i + 1
	end
	bfxs_Log(format("profile list (ScriptCB_GetLoginList, split): %d -%s", i - 1, s))
end

-- 21g-r2: the row of the list with that name (any case), nil if none
function bfxs_RowByName(name)
	local list = ifs_split_profile_listbox_contents
	local want = strupper(name)
	local i = 1
	while (list[i]) do
		if (strupper(bfxs_ProfileName(list[i])) == want) then
			return i
		end
		i = i + 1
	end
	return nil
end

-- 21g-r2: PLAYER 2 / 3 / 4 as temporary profiles ([Splitscreen] TempProfiles): added at the end of the list where no
-- profile has that name. Never saved, so never in the profile screen: player 2's is made by BF1GameExt right before the
-- game loads the profiles (a copy of player 1's, as the game's own PLAYER 2 when there are not 2 profiles) and kept from
-- ifs_saveop; players 3 / 4's are names only.
function bfxs_AddTemps(this)
	if (not gBFX_TempProfiles) then
		return
	end
	local list = ifs_split_profile_listbox_contents
	-- the game's own temporary profile (its list makes one in "active slot 2" when there are not 2 profiles) tagged now:
	-- the launch's slot swap changes the rows' memslot, the tag stays
	local i = 1
	while (list[i]) do
		if (list[i].memslot == 2) then
			list[i].bfxgametemp = 1
			bfxs_Log(format("row %d (%s) is the game's own temporary profile - not saved either", i, bfxs_ProfileName(list[i])))
		end
		i = i + 1
	end
	local last = 2
	if (gBFXS_Four) then
		last = gBFXS_Players
	end
	local added = ""
	local k
	for k = 2, last do
		local name = format("PLAYER %d", k)
		if (not bfxs_RowByName(name)) then
			list[getn(list) + 1] = { showstr = ScriptCB_tounicode(name), bfxtemp = k }
			added = added .. format("  [%d] %s", getn(list), name)
		end
	end
	if (added ~= "") then
		ListManager_fnFillContents(this.ProfileT.listbox, list, ifs_split_profile_listbox_layoutT)
		ListManager_fnFillContents(this.ProfileB.listbox, list, ifs_split_profile_listbox_layoutB)
		bfxs_Log("temporary profiles added (never saved):" .. added)
	else
		bfxs_Log(format("temporary profiles: none added (PLAYER 2 - %d are in the list already)", last))
	end
end

-- 21g-r2: player p's own row - PLAYER p (temporary or saved) - when nobody else is on it, else nil
function bfxs_OwnRow(this, p)
	if (not gBFX_TempProfiles) then
		return nil
	end
	local row = bfxs_RowByName(format("PLAYER %d", p))
	if (not row) then
		return nil
	end
	if (gBFXS_Four) then
		if (bfxs4_UsedBy(this, row, p, nil)) then
			return nil
		end
	elseif (row == ifs_split_profile_listbox_layoutT.SelectedIdx) then
		return nil
	end
	return row
end

-- 21g-r2: presses refused for the temporary profiles (1 = refused): player 1 READY on one (he signs in with a saved
-- profile); the launch while player 2's is one that cannot be made now
function bfxs_TempGate(this, name, joy)
	if (not gBFX_TempProfiles) then
		return nil
	end
	local list = ifs_split_profile_listbox_contents
	local lt = ifs_split_profile_listbox_layoutT
	local lb = ifs_split_profile_listbox_layoutB
	if ((joy < 0.5) and ((name == "Input_Accept") or (name == "Input_Start")) and ((this.ModeT == 1) or (this.ModeT == 2))) then
		local sel = list[lt.SelectedIdx or 0]
		if (sel and (sel.bfxtemp or sel.bfxgametemp)) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			bfxs_Log(format("player 1: %s is a temporary profile - player 1 signs in with a saved one", bfxs_ProfileName(sel)))
			return 1
		end
	end
	if ((this.ModeT == 3) and (this.ModeB == 3) and ((name == "Input_Start") or ((name == "Input_Accept") and (joy < 0.5)))) then
		local sb = list[lb.SelectedIdx or 0]
		local st = list[lt.SelectedIdx or 0]
		if (sb and sb.bfxtemp) then
			local why = nil
			if (st and (st.memslot == 2)) then
				why = "player 1's profile is in the second profile slot"
			elseif (bfxs_Ask("tempok") ~= 1) then
				why = "BF1GameExt cannot make it now"
			end
			if (why) then
				ifelm_shellscreen_fnPlaySound(this.errorSound)
				bfxs_Log(format("START: player 2's temporary profile %s cannot be used (%s) - pick a saved profile for player 2",
					bfxs_ProfileName(sb), why))
				return 1
			end
		end
	end
	return nil
end

-- the row of the list with that active slot (nil if none), not the row skip
function bfxs_SlotRow(slot, skip)
	local list = ifs_split_profile_listbox_contents
	local i = 1
	while (list[i]) do
		if ((list[i].memslot == slot) and (i ~= skip)) then
			return i
		end
		i = i + 1
	end
	return nil
end

-- 21f: the row of the signed-in profile by name (the game lists it as a file when it has no active slot 1 for it)
function bfxs_NameRow()
	if (not ScriptCB_GetCurrentProfileName) then
		return nil
	end
	local me = ScriptCB_GetCurrentProfileName()
	if (not me) then
		return nil
	end
	me = ScriptCB_ununicode(me)
	local list = ifs_split_profile_listbox_contents
	local i = 1
	while (list[i]) do
		if ((not list[i].memslot) and (bfxs_ProfileName(list[i]) == me)) then
			return i
		end
		i = i + 1
	end
	return nil
end

function bfxs_SetRow(listbox, layout, row)
	layout.SelectedIdx = row
	layout.CursorIdx = row
	layout.FirstShownIdx = 1
	if (row > layout.showcount) then
		layout.FirstShownIdx = row - layout.showcount + 1
	end
	ListManager_fnFillContents(listbox, ifs_split_profile_listbox_contents, layout)
end

-- the bottom "press START" text: the PC game counts only its own pad (its check would say "needs 2 controllers" all
-- the time - it is kept from running, Update), so it follows pad 2 as BF1GameExt sees it. Written when it changes
-- (or force: on Enter).
function bfxs_Pad2Text(this, force)
	local conn = bfxs_Ask("pad2conn")
	if ((conn == gBFXS_Conn) and (not force)) then
		return
	end
	if (conn > 0) then
		bfxs_SetText(this.pressstartB, "ifs.split.starttojoin", "Press START to join")
	else
		bfxs_SetText(this.pressstartB, "ifs.split.needs2controllers", "Player 2: connect a second controller")
	end
	if (conn ~= gBFXS_Conn) then
		gBFXS_Conn = conn
		if (conn > 0) then
			bfxs_Log(format("pad 2 is connected (XInput %d) - \"press START to join\"", bfxs_Ask("pad2index")))
		elseif (conn == 0) then
			bfxs_Log("pad 2 is NOT connected - \"connect a second controller\"")
		else
			bfxs_Log("pad 2: BF1GameExt did not answer")
		end
	end
end

function bfxs_FixTexts(this)
	bfxs_SetText(this.pressstartT, "ifs.split.starttojoin", "Press START to join")
	bfxs_SetText(this.pressstart, "ifs.split.starttolaunch", "Press START to launch")
	bfxs_SetText(this.ReadyT, "ifs.split.ready", "Ready")
	bfxs_SetText(this.ReadyB, "ifs.split.ready", "Ready")
	if (this.AcceptGroupT) then
		bfxs_SetText(this.AcceptGroupT.helpstr, "common.accept", "Accept")
	end
	if (this.AcceptGroupB) then
		bfxs_SetText(this.AcceptGroupB.helpstr, "common.accept", "Accept")
	end
	bfxs_Pad2Text(this, 1)
	if (gBFXS_Four) then
		bfxs4_FixTexts(this)
	end
end

-- One input for one player (0 top, 1 bottom), with the PC's extras
function bfxs_Do(this, name, joy)
	if (bfxs_TempGate(this, name, joy)) then
		return
	end
	if (gBFXS_Four and bfxs4_Gate(this, name, joy)) then
		return
	end
	local before = bfxs_Modes(this)
	local beforeB = this.ModeB
	gBFXS_Inside = 1
	if (joy < 0.5) then
		if ((name == "Input_Accept") and ((this.ModeT == 0) or ((this.ModeT == 3) and (this.ModeB == 3)))) then
			-- A where START is asked for: join / launch
			ifs_split_profile.Input_Start(this, 0)
		else
			ifs_split_profile[name](this, 0)
		end
	else
		if ((name == "Input_Start") and (this.ModeT == 3) and (this.ModeB == 3)) then
			-- player 2's START launches too (on the console only player 1's did)
			ifs_split_profile.Input_Start(this, 0)
		elseif ((name == "Input_Accept") and (this.ModeB == 0)) then
			-- A joins too
			ifs_split_profile.Input_Start(this, 1)
		else
			ifs_split_profile[name](this, 1)
		end
		-- just joined: player 2 starts on PLAYER 2 (21g-r2, TempProfiles), else the second active profile, if the list has one
)BFXLUA",
R"BFXLUA(		if ((beforeB == 0) and (this.ModeB == 1)) then
			local row = bfxs_OwnRow(this, 2)
			if (not row) then
				row = bfxs_SlotRow(2, ifs_split_profile_listbox_layoutT.SelectedIdx)
			end
			if (row and (row ~= ifs_split_profile_listbox_layoutB.SelectedIdx)) then
				bfxs_SetRow(this.ProfileB.listbox, ifs_split_profile_listbox_layoutB, row)
				ifs_split_profile_fnSetPieceVis(this)
			end
			local sel = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutB.SelectedIdx]
			bfxs_Log(format("player 2 joined - on row %s (%s)", tostring(ifs_split_profile_listbox_layoutB.SelectedIdx),
				bfxs_ProfileName(sel)))
		end
	end
	gBFXS_Inside = nil
	local after = bfxs_Modes(this)
	if (after ~= before) then
		bfxs_Log(format("player %d %s: %s -> %s", joy + 1, gBFXS_Short[gBFXS_Code[name]], before, after))
	end
end

-- Every input handler of the screen goes through here
function bfxs_Input(this, name, Joystick)
	if (gBFXS_Inside) then
		-- the screen calling itself (Input_Start -> Input_Accept): straight on
		local j = Joystick
		if (type(j) ~= "number") then
			j = 0
		end
		gBFXS_Orig[name](this, j)
		return
	end
	local jtxt = tostring(Joystick)
	if ((not this.ModeT) or (this.ModeT < 0) or (this.ModeT > 3)) then
		bfxs_Log(format("game: %s(Joystick %s) while the screen is busy (%s) - ignored", name, jtxt, bfxs_Modes(this)))
		return
	end
	-- 21j-r27: a click on the BACK button at the bottom left leaves the sign-in
	if (bfxs_MouseBackClick(this, name, Joystick)) then
		bfxs_MouseBack(this)
		return
	end
	if (gBFXS_Four and (type(Joystick) == "number") and (Joystick > 1.5)) then
		-- 21g: the game itself gave player 3's / 4's pad
		local p = 3
		if (Joystick > 2.5) then
			p = 4
		end
		bfxs4_GameInput(this, name, p, jtxt)
		return
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		-- the game itself gave player 2's pad
		if (not gBFXS_GameP2) then
			gBFXS_GameP2 = 1
			bfxs_Log(format("game: %s(Joystick %s) - the game itself gives player 2's pad: BF1GameExt's pad 2 is not read "
				.. "here from now on", name, jtxt))
			if ((gBFXS_LastPadCode == gBFXS_Code[name]) and (gBFXS_Clock - gBFXS_LastPadTime < 0.5)) then
				bfxs_Log("  (the same press came from BF1GameExt's pad 2 just before - not done twice)")
				return
			end
		else
			bfxs_Log(format("game: %s(Joystick %s) -> player 2 (%s)", name, jtxt, bfxs_Modes(this)))
		end
		bfxs_Do(this, name, 1)
		return
	end
	bfxs_Log(format("game: %s(Joystick %s) -> player 1 (%s)", name, jtxt, bfxs_Modes(this)))
	bfxs_Do(this, name, 0)
end

-- the wrappers (Lua 4: the handler's name is an upvalue)
function bfxs_Wrap(name)
	gBFXS_Orig[name] = ifs_split_profile[name]
	ifs_split_profile[name] = function(this, Joystick)
		bfxs_Input(this, %name, Joystick)
	end
end
bfxs_Wrap("Input_Accept")
bfxs_Wrap("Input_Back")
bfxs_Wrap("Input_Start")
bfxs_Wrap("Input_GeneralUp")
bfxs_Wrap("Input_GeneralDown")
bfxs_Wrap("Input_GeneralLeft")
bfxs_Wrap("Input_GeneralRight")

ifs_split_profile.Enter = function(this, bFwd)
	local fromList = this.bFromLoadFileList
	gBFXS_Inside = nil
	bfxs_Log(format("Enter(bFwd %s, from the profile list %s) - %s", tostring(bFwd), tostring(fromList), bfxs_Modes(this)))
	if (gBFXS_Four) then
		if (fromList) then
			bfxs4_Reset(this) -- players 3 / 4 to PRESS START
		elseif (bFwd) then
			-- the profile list is loaded first (ifs_saveop): players 3 / 4 hidden until it is back
			gBFXS_Mode[3] = -1
			gBFXS_Mode[4] = -1
		end
	end
	gBFXS_Orig.Enter(this, bFwd)
	if (fromList) then
		bfxs_ListLog()
		-- player 1 starts on the profile he signed in with (the list's active slot 1, else his profile's row by name),
		-- not the login list's row
		local row = bfxs_SlotRow(1, nil)
		if (not row) then
			row = bfxs_NameRow()
		end
		if (row and (row ~= ifs_split_profile_listbox_layoutT.SelectedIdx)) then
			bfxs_SetRow(this.ProfileT.listbox, ifs_split_profile_listbox_layoutT, row)
			ifs_split_profile_fnSetPieceVis(this)
		end
		bfxs_Log(format("player 1 on row %s (%s)", tostring(ifs_split_profile_listbox_layoutT.SelectedIdx),
			bfxs_ProfileName(ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutT.SelectedIdx])))
		bfxs_AddTemps(this) -- 21g-r2
		bfxs_Ask("pad2reset")
		-- 21j-r28: the profile list made player 2's profile current and back - the sound engine restarted
		bfxs_SoundRestore("the sign-in's profile list", this.movieBackground)
	end
	if (this.ModeT) then
		bfxs_FixTexts(this)
	end
	if (gBFXS_Four) then
		bfxs4_Vis(this)
		bfxs_Log(format("Enter done - %s, player 3 %s, player 4 %s", bfxs_Modes(this), bfxs4_ModeName(gBFXS_Mode[3]),
			bfxs4_ModeName(gBFXS_Mode[4])))
		return
	end
	bfxs_Log(format("Enter done - %s", bfxs_Modes(this)))
end

ifs_split_profile.Exit = function(this, bFwd)
	bfxs_Log(format("Exit(bFwd %s) - %s", tostring(bFwd), bfxs_Modes(this)))
	gBFXS_Orig.Exit(this, bFwd)
end

ifs_split_profile.Update = function(this, fDt)
	gBFXS_Inside = nil
	-- the game's controller check (its "needs 2 controllers" text every 0.3 s) kept from running: ours below
	this.fControllerCheckTimer = 1000
	gBFXS_Orig.Update(this, fDt)
	gBFXS_Clock = gBFXS_Clock + fDt
	gBFXS_PadCheck = gBFXS_PadCheck - fDt
	if (this.ModeT and (gBFXS_PadCheck < 0)) then
		gBFXS_PadCheck = 0.3
		bfxs_Pad2Text(this, nil)
		if (gBFXS_Four) then
			bfxs4_PadLog()
		end
	end
	if (gBFXS_Four) then
		bfxs4_Update(this, fDt)
	end
	-- pad 2's presses (BF1GameExt reads pad 2; one event per question)
	if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4) and (not gBFXS_GameP2)) then
		local n = 0
		local e = bfxs_Ask("pad2")
		while ((e > 0) and (n < 8)) do
			local name = gBFXS_Name[e]
			if (name) then
				gBFXS_LastPadCode = e
				gBFXS_LastPadTime = gBFXS_Clock
				bfxs_Log(format("pad 2: %s -> player 2 (%s)", gBFXS_Short[e], bfxs_Modes(this)))
				bfxs_Do(this, name, 1)
			end
			n = n + 1
			e = 0
			if (this.ModeT and (this.ModeT >= 0) and (this.ModeT < 4)) then
				e = bfxs_Ask("pad2")
			end
		end
	end
end

-- the launch, logged
gBFXS_StartLoad = ifs_split_profile_StartLoadProfile
ifs_split_profile_StartLoadProfile = function(profile1, profile2)
	local this = ifs_split_profile
	local t = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutT.SelectedIdx]
	local b = ifs_split_profile_listbox_contents[ifs_split_profile_listbox_layoutB.SelectedIdx]
	local f1 = "already active"
	local f2 = "already active"
	local temp2 = nil
	-- 21g-r2: player 2 on a temporary profile: BF1GameExt makes it now (nothing to load); on the game's own temporary
	-- one (its "active slot 2"): marked so it is not saved either; on a saved one: saved as the game does it
	if (gBFX_TempProfiles) then
		if (b and b.bfxtemp) then
			if (bfxs_Ask("tempprofile " .. bfxs_ProfileName(b)) == 1) then
				if (ScriptCB_SetProfileNotDirty) then
					ScriptCB_SetProfileNotDirty(2)
				end
				profile2 = nil
				temp2 = 1
			else
				bfxs_Log("LAUNCH: player 2's temporary profile could NOT be made - the game looks for it as a file (it fails)")
			end
		elseif (b and b.bfxgametemp) then
			bfxs_Ask("tempmark " .. bfxs_ProfileName(b))
		else
			bfxs_Ask("tempclear")
		end
	end
	if (profile1) then
		f1 = "loaded from its file"
	end
	if (profile2) then
		f2 = "loaded from its file"
	elseif (temp2) then
		f2 = "temporary - made by BF1GameExt, never saved"
	end
	bfxs_Log(format("LAUNCH: player 1 = %s (%s, side %s), player 2 = %s (%s, side %s); split screen %s",
		bfxs_ProfileName(t), f1, tostring(this.SideT), bfxs_ProfileName(b), f2, tostring(this.SideB),
		tostring(ScriptCB_IsSplitscreen())))
	if (gBFXS_Four) then
		-- 21g-r2: the matches get as many players as signed in; the pads of the ones who did are players 3 / 4's (in that
		-- order - player 4 alone plays as player 3)
		local joined = 2
		local pads = {}
		local p
		for p = 3, gBFXS_Players do
			if (gBFXS_Mode[p] == 3) then
				joined = joined + 1
				local x = -1
				if (not gBFXS_GameP[p]) then
					x = bfxs_Ask(format("pad%dindex", p))
				end
				pads[getn(pads) + 1] = x
				bfxs_Log(format("LAUNCH: player %d = %s (READY on pad XInput %d - a name only: the game loads 2 profiles)%s", p,
					bfxs_ProfileName(ifs_split_profile_listbox_contents[gBFXS_Layout[p].SelectedIdx or 0]), x,
					(joined < p) and format(" - he plays as player %d", joined) or ""))
			else
				bfxs_Log(format("LAUNCH: player %d did not join", p))
			end
		end
		local set = bfxs_Ask(format("signin %d %d %d", joined, pads[1] or -1, pads[2] or -1))
		bfxs_Log(format("LAUNCH: %d of %d players signed in - the match has %d players", joined, gBFXS_Players, set))
	end
	gBFXS_StartLoad(profile1, profile2)
end
gBFXS_LoadOk = ifs_split_profile_LoadProfileSuccess
ifs_split_profile_LoadProfileSuccess = function()
	bfxs_Log("profiles loaded - the split screen game modes next (ifs_sp)")
	-- 21j-r28: the profiles loaded (and player 2's made current) - the sound engine restarted; the sign-in's background
	-- movie goes on behind the game modes (the game keeps it there: gMovieDisabled)
	bfxs_SoundRestore("the launch's profile loading", ifs_split_profile.movieBackground)
	gBFXS_LoadOk()
end
gBFXS_LoadCancel = ifs_split_profile_LoadProfileCancel
ifs_split_profile_LoadProfileCancel = function()
	bfxs_Log("profile load FAILED or cancelled - back to the sign-in screen")
	gBFXS_LoadCancel()
	bfxs_SoundRestore("the cancelled profile loading", ifs_split_profile.movieBackground) -- 21j-r28
end

-- 21j-r27: players 1 and 2's BACK / ACCEPT with the pad's B / A - while the game makes the screen
if (gBFX_Phase == "build") then
	bfxs_PadIcons(ifs_split_profile)
elseif (gBFX_PadIcons) then
	bfxs_Log(format("21j-r27: the sign-in was made before this script ran (%s) - players 1 and 2's BACK / ACCEPT keep the game's icons",
		gBFX_Phase or "?"))
end

-- 21g: the 4-player sign-in - made while the game makes the screen (its pieces are made right after this script)
gBFXS_Players = gBFX_Players or 2
if (gBFX_SignIn4 and (gBFXS_Players >= 3)) then
	if (gBFX_Phase == "build") then
		bfxs4_Build(ifs_split_profile)
		gBFXS_Four = 1
		gBFXS_SetPieceVis = ifs_split_profile_fnSetPieceVis
		ifs_split_profile_fnSetPieceVis = function(this, a)
			gBFXS_SetPieceVis(this, a)
			bfxs4_Vis(this)
		end
	else
		bfxs_Log(format("4-player sign-in NOT built: the screen was made before this script ran (%s) - the 2-player one",
			gBFX_Phase or "?"))
	end
end

if (gBFXS_Four) then
	bfxs_Log(format("ifs_split_profile patched (%s, %s): the 4-PLAYER sign-in (%d players) - player 1 = the game's input, "
		.. "players 2 / 3 / 4 = pads 2 / 3 / 4", gBFX_Phase or "?", gBFX_Screen or "?", gBFXS_Players))
else
bfxs_Log(format("ifs_split_profile patched (%s, %s): player 1 = the game's input, player 2 = pad 2", gBFX_Phase or "?",
	gBFX_Screen or "?"))
end
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21f: Galactic Conquest in split screen, as the Classic Collection has it: the map list (ifs_meta_opts: the split
-- screen list, the 4 conquests with both sides) -> LAUNCH -> PICK YOUR SIDE (ifs_meta_top, the console's own screen,
-- still in the 2020 shell: player 1's row on top, player 2's row below, left / right picks a side, the picked one lit)
-- -> LAUNCH -> the galaxy. Same side = CO-OP (the AI runs the other side), other sides = VERSUS (player 2 runs the other
-- side on the galaxy map - the game gives that screen both pads - and the battles are player 1 against player 2).
-- The PC script launched straight from the map list with player 2 on player 1's opponent (the AI on it), and read the
-- full list's row while it showed the split list: the split list is used for all of it now. At the launch,
-- metagame_state_local.mJoystickTeams[2] (player 2's team) and the AI control are set from the picks; the game's
-- ScriptCB_SetMetagameTeams puts both local players on their teams for each battle. mIsSplitscreen = 1: a saved split
-- screen conquest loads in split screen.
)BFXLUA",
R"BFXLUA(-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_opts and (not ifs_meta_opts.bfxgc)) then
ifs_meta_opts.bfxgc = 1

gBFXS_GCP2Side = nil -- player 2's side for the launch (nil: player 1's)

gBFXS_GCOrig = {
	Enter = ifs_meta_opts.Enter,
	Input_Accept = ifs_meta_opts.Input_Accept,
	Input_GeneralLeft = ifs_meta_opts.Input_GeneralLeft,
	Input_GeneralRight = ifs_meta_opts.Input_GeneralRight,
}
gBFXS_GCSetButtonPos = ifs_meta_opts_fnSetButtonPos

-- the list the screen shows
function bfxs_GCList()
	if (ScriptCB_IsSplitscreen()) then
		return ifs_meta_opts_listbox_contents_split
	end
	return ifs_meta_opts_listbox_contents
end

function bfxs_GCSelection()
	return bfxs_GCList()[ifs_meta_opts_name_listboxL_layout.SelectedIdx]
end

-- one of the screen's own functions, with the list it shows (in split screen the 2020 script read the full list's row)
function bfxs_GCWithList(fn, this)
	local keep = ifs_meta_opts_listbox_contents
	ifs_meta_opts_listbox_contents = bfxs_GCList()
	fn(this)
	ifs_meta_opts_listbox_contents = keep
end

-- player 1's side as the screen has it (its Side, else the configuration's default): 1 / 2
function bfxs_GCP1Side(this)
	local s = this.Side
	if (not s) then
		local sel = bfxs_GCSelection()
		if (sel) then
			s = sel.default
		end
	end
	if (s ~= 2) then
		s = 1
	end
	return s
end

-- a side's name: 1 = Rebels / Republic, 2 = Empire / CIS (the configuration's era)
function bfxs_GCSideName(side)
	local sel = bfxs_GCSelection()
	local key = "common.sides.all.name"
	if (sel and (sel.era == "new")) then
		key = "common.sides.rep.name"
		if (side == 2) then
			key = "common.sides.cis.name"
		end
	elseif (side == 2) then
		key = "common.sides.imp.name"
	end
	local u = ScriptCB_getlocalizestr(key, 1)
	if (u) then
		return ScriptCB_ununicode(u)
	end
	return format("side %d", side)
end

-- the launch: player 2's team and the AI from the picks
function bfxs_GCTeams(this)
	local j1 = metagame_state_local.mJoystickTeams[1]
	if ((j1 ~= 1) and (j1 ~= 2)) then
		bfxs_Log(format("Galactic Conquest LAUNCH: player 1's team is %s - the teams are left as the game set them",
			tostring(j1)))
		return
	end
	if (gBFXS_GC4) then
		bfxs_GCTeams4()
		return
	end
	local p2 = gBFXS_GCP2Side or j1
	metagame_state_local.mIsSplitscreen = 1
	if (p2 == j1) then
		metagame_state_local.mJoystickTeams[2] = j1
		metagame_state.numhumans = 1
		bfxs_Log(format("Galactic Conquest LAUNCH: CO-OP - players 1 and 2 on team %d (%s), the AI on team %d; AI control: "
			.. "team 1 %s, team 2 %s", j1, bfxs_GCSideName(j1), 3 - j1, tostring(metagame_state.team1.aicontrol),
			tostring(metagame_state.team2.aicontrol)))
		bfxt_Log(format("CONQUEST LAUNCH (2 players): CO-OP - players 1 and 2 on side %d (%s), the AI on side %d; AI control: "
			.. "team 1 %s, team 2 %s", j1, bfxs_GCSideName(j1), 3 - j1, tostring(metagame_state.team1.aicontrol),
			tostring(metagame_state.team2.aicontrol)))
	else
		metagame_state_local.mJoystickTeams[2] = 3 - j1
		metagame_state.team1.aicontrol = nil
		metagame_state.team2.aicontrol = nil
		metagame_state.numhumans = 2
		bfxs_Log(format("Galactic Conquest LAUNCH: VERSUS - player 1 on team %d (%s), player 2 on team %d (%s), no AI",
			j1, bfxs_GCSideName(j1), 3 - j1, bfxs_GCSideName(3 - j1)))
		bfxt_Log(format("CONQUEST LAUNCH (2 players): VERSUS - player 1 on side %d (%s), player 2 on side %d (%s), no AI",
			j1, bfxs_GCSideName(j1), 3 - j1, bfxs_GCSideName(3 - j1)))
	end
end

-- 21g-r3: 3 / 4 players (PICK YOUR SIDE's picks in gBFXS_GC4): a side with a player on it is human, a side without one is
-- the AI's; VERSUS when both sides have players. Players 1 / 2's battle sides go through the game's own joystick teams;
-- every player's side and each side's commander (the first who locked in on it - only he moves on the galaxy map) to
-- BF1GameExt: it keeps them across the battles (the shell starts again after each) and locks players 3 / 4's spawn
-- screens to their sides.
function bfxs_GCTeams4()
	local g = gBFXS_GC4
	local s = g.side
	local human1 = nil
	local human2 = nil
	local p
	for p = 1, g.n do
		if (s[p] == 1) then
			human1 = 1
		elseif (s[p] == 2) then
			human2 = 1
		end
	end
	metagame_state_local.mIsSplitscreen = 1
	metagame_state_local.mJoystickTeams[2] = s[2]
	if (human1) then
		metagame_state.team1.aicontrol = nil
	else
		metagame_state.team1.aicontrol = 1
	end
	if (human2) then
		metagame_state.team2.aicontrol = nil
	else
		metagame_state.team2.aicontrol = 1
	end
	local mode = "CO-OP"
	if (human1 and human2) then
		metagame_state.numhumans = 2
		mode = "VERSUS"
	else
		metagame_state.numhumans = 1
	end
	bfxs_Ask(format("gcset %d %d %d %d %d %d", s[1] or 0, s[2] or 0, s[3] or 0, s[4] or 0, g.cmd[1] or 0, g.cmd[2] or 0))
	local who = ""
	for p = 1, g.n do
		who = who .. format(" player %d %s,", p, bfxs_GCSideName(s[p]))
	end
	bfxs_Log(format("Galactic Conquest LAUNCH (%d players): %s -%s side 1 (%s) moved by %s, side 2 (%s) by %s; AI control: "
		.. "team 1 %s, team 2 %s", g.n, mode, who, bfxs_GCSideName(1),
		(g.cmd[1] and format("player %d", g.cmd[1])) or "the AI", bfxs_GCSideName(2),
		(g.cmd[2] and format("player %d", g.cmd[2])) or "the AI", tostring(metagame_state.team1.aicontrol),
		tostring(metagame_state.team2.aicontrol)))
	bfxt_Log(format("CONQUEST LAUNCH (%d players): %s -%s side 1 (%s) moved by %s, side 2 (%s) by %s (the first to lock in "
		.. "on each); players 1 / 2's joysticks on teams %s / %s", g.n, mode, who, bfxs_GCSideName(1),
		(g.cmd[1] and format("player %d", g.cmd[1])) or "the AI", bfxs_GCSideName(2),
		(g.cmd[2] and format("player %d", g.cmd[2])) or "the AI", tostring(metagame_state_local.mJoystickTeams[1]),
		tostring(metagame_state_local.mJoystickTeams[2])))
end

-- while the screen's own launch runs: its ScriptCB_SetIFScreen("ifs_meta_main") sets the teams first
function bfxs_GCSetIFScreen(name)
	if (name == "ifs_meta_main") then
		bfxs_GCTeams(ifs_meta_opts)
	end
	gBFXS_GCSetIF(name)
end

-- the 2020 script's own launch (ifs_meta_opts' LAUNCH), with player 1's side and player 2's
function bfxs_GCLaunch(p1side, p2side)
	local this = ifs_meta_opts
	this.Side = p1side
	gBFXS_GCP2Side = p2side
	this.CurButton = "launch"
	gBFXS_GCSetIF = ScriptCB_SetIFScreen
	ScriptCB_SetIFScreen = bfxs_GCSetIFScreen
	bfxs_GCWithList(gBFXS_GCOrig.Input_Accept, this)
	ScriptCB_SetIFScreen = gBFXS_GCSetIF
end

-- the side icons' places: the two-sided ones for the split list (the script placed by row number: rows 1-4 one side)
ifs_meta_opts_fnSetButtonPos = function(this)
	if (ScriptCB_IsSplitscreen()) then
		local keep = ifs_meta_opts_name_listboxL_layout.SelectedIdx
		ifs_meta_opts_name_listboxL_layout.SelectedIdx = 5
		gBFXS_GCSetButtonPos(this)
		ifs_meta_opts_name_listboxL_layout.SelectedIdx = keep
	else
		gBFXS_GCSetButtonPos(this)
	end
end

ifs_meta_opts.Enter = function(this, bFwd)
	local split = ScriptCB_IsSplitscreen()
	if (split) then
		ifs_meta_opts_name_listboxL_layout.enablecount = getn(ifs_meta_opts_listbox_contents_split)
	end
	gBFXS_GCOrig.Enter(this, bFwd)
	if (split) then
		bfxs_Log(format("Galactic Conquest maps (split screen, bFwd %s): %d conquests; LAUNCH -> PICK YOUR SIDE",
			tostring(bFwd), getn(ifs_meta_opts_listbox_contents_split)))
	end
end

ifs_meta_opts.Input_GeneralLeft = function(this)
	bfxs_GCWithList(gBFXS_GCOrig.Input_GeneralLeft, this)
end

ifs_meta_opts.Input_GeneralRight = function(this)
	bfxs_GCWithList(gBFXS_GCOrig.Input_GeneralRight, this)
end

ifs_meta_opts.Input_Accept = function(this)
	if (not ScriptCB_IsSplitscreen()) then
		gBFXS_GCOrig.Input_Accept(this)
		return
	end
	if (ScriptCB_IsLastInputFromJoystick()) then
		this.CurButton = "launch"
	end
	if (this.CurButton == "launch") then
		local sel = bfxs_GCSelection()
		if (ifs_meta_top and ifs_meta_top.bfxtop) then
			ifelm_shellscreen_fnPlaySound(this.acceptSound)
			gBFXS_GCFromMaps = 1 -- (r3) a new conquest: PICK YOUR SIDE, not the saved one
			bfxs_Log(format("Galactic Conquest (split screen): LAUNCH on \"%s\" - PICK YOUR SIDE next",
				(sel and sel.showstr) or "?"))
			ifs_movietrans_PushScreen(ifs_meta_top)
		else
			bfxs_Log("Galactic Conquest (split screen): NO PICK YOUR SIDE screen (ifs_meta_top) - launched co-op")
			bfxs_GCLaunch(bfxs_GCP1Side(this), nil)
		end
		return
	end
	bfxs_GCWithList(gBFXS_GCOrig.Input_Accept, this)
end

bfxs_Log(format("ifs_meta_opts patched (%s, %s): Galactic Conquest's split screen list, LAUNCH -> PICK YOUR SIDE",
	gBFX_Phase or "?", gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21f: ifs_meta_top - PICK YOUR SIDE. The console's split screen layout (buttons_s1: player 1's row, buttons_s2: player
-- 2's) in a window titled PICK YOUR SIDE, BACK (B) / LAUNCH (A) at the bottom; left / right picks (ReadAllControllers:
-- the game gives pad 2 as joystick 1; BF1GameExt's pad 2 until it does). Player 1's A / LAUNCH launches, his B / BACK
-- goes back to the maps. Its 2020 Enter (the console's conquest set-up) is not used in split screen.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_top and (not ifs_meta_top.bfxtop)) then
ifs_meta_top.bfxtop = 1

-- 21j-r8: the rows of side pictures (top / bottom of each, the window's units): the console's two, or four slots with
-- 3 / 4 players (two above the side names, two below); the half widths; the player boxes (width, height, the step)
gBFXS_TopRows2 = { { -128, 2 }, { 40, 170 } }
gBFXS_TopRows4 = { { -128, -65 }, { -61, 2 }, { 40, 103 }, { 107, 170 } }
gBFXS_TopHalfW2 = 64
gBFXS_TopHalfW4 = 40
gBFXS_BoxW = 250
gBFXS_BoxH = 40
gBFXS_BoxStep = 86
gBFXS_TopLockIn = nil -- (the players lock in: 3 / 4 players, or 2 with GCCommander2P)

if (gBFX_Phase == "build") then
	-- the subtitle stays in Lua (inert: it would be dropped once made) so it can be hidden
	if (ifs_meta_top.subtitle) then
		ifs_meta_top.subtitle.inert = nil
	end
	-- the rows' soldiers under their side names (the PC spacing put them half over each other)
	local k, v
	for k, v in { side_a = -100, side_r = -100, side_i = 100, side_c = 100 } do
		if (ifs_meta_top.buttons_s1[k]) then
			ifs_meta_top.buttons_s1[k].x = v
		end
		if (ifs_meta_top.buttons_s2[k]) then
			ifs_meta_top.buttons_s2[k].x = v
		end
	end
	-- BACK (B) / LAUNCH (A), the 2020 screens' own buttons
	if (CreatePositiveButton and CreateNegativeButton) then
		ifs_meta_top.bfxback = CreateNegativeButton("common.back", "B", "_back")
		ifs_meta_top.bfxlaunch = CreatePositiveButton("ifs.onlinelobby.launch", "A", "_ok")
	end
	-- 21j-r8: the console's look - players 3 / 4's rows of side pictures (as the game's two, in containers of the same
	-- place and turn) and a box for each player at the right (the game's window with its orange title bar, as the class
	-- screen's): its title PLAYER n, then COMMANDER / PLAYER READY; inside, the player and his side
	if (gBFX_GCPickLook and NewButtonWindow) then
		local xs = { side_a = -100, side_r = -100, side_i = 100, side_c = 100 }
		local k
		if (gBFX_Players and (gBFX_Players >= 3)) then
			for k = 3, 4 do
				local c = NewIFContainer { ScreenRelativeX = 0.4, ScreenRelativeY = 0.5 }
				local nm, x
				for nm, x in xs do
					c[nm] = NewIFImage { x = x, y = 0, localpos_l = -40, localpos_r = 40, localpos_t = gBFXS_TopRows4[k][1],
						localpos_b = gBFXS_TopRows4[k][2], texture = nm, ZPos = 130 }
				end
				c.rotX = ifs_meta_top.button_rot_x
				c.rotY = ifs_meta_top.button_rot_y
				ifs_meta_top["bfxrow" .. k] = c
			end
		end
		local box = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.5 }
		local p
		for p = 1, 4 do
			local w = NewButtonWindow { ZPos = 200, x = 0, y = (p - 2.5) * gBFXS_BoxStep, width = gBFXS_BoxW, height = gBFXS_BoxH,
				titleText = "", font = "gamefont_small" }
)BFXLUA",
R"BFXLUA(			w.bfxname = NewIFText { string = "", font = "gamefont_medium", halign = "left", valign = "vcenter",
				textw = gBFXS_BoxW * 0.45, texth = gBFXS_BoxH, x = gBFXS_BoxW * -0.5 + 12, y = gBFXS_BoxH * -0.5,
				nocreatebackground = 1 }
			w.bfxside = NewIFText { string = "", font = "gamefont_medium", halign = "left", valign = "vcenter",
				textw = gBFXS_BoxW * 0.5, texth = gBFXS_BoxH, x = gBFXS_BoxW * -0.02, y = gBFXS_BoxH * -0.5,
				nocreatebackground = 1 }
			box["p" .. p] = w
		end
		ifs_meta_top.bfxbox = box
	end
	-- 21g-r3: 3 / 4 players - the roster at the right: each player, his side, READY / COMMANDER (21j-r8: GCPickLook=0)
	if (gBFX_Players and (gBFX_Players >= 3) and (not ifs_meta_top.bfxbox)) then
		local r = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.5 }
		r.title = NewIFText { string = "", font = "gamefont_medium", textw = 300, x = -150, y = -100, nocreatebackground = 1 }
		local p
		for p = 1, 4 do
			local y = -62 + (p - 1) * 34
			r["name" .. p] = NewIFText { string = "", font = "gamefont_medium", halign = "left", textw = 110, x = -150, y = y,
				nocreatebackground = 1 }
			r["side" .. p] = NewIFText { string = "", font = "gamefont_medium", halign = "left", textw = 120, x = -38, y = y,
				nocreatebackground = 1 }
			r["state" .. p] = NewIFText { string = "", font = "gamefont_small", halign = "left", textw = 100, x = 84, y = y + 3,
				nocreatebackground = 1 }
		end
		ifs_meta_top.bfxroster = r
	end
end

gBFXS_TopOrig = {
	Enter = ifs_meta_top.Enter,
	Update = ifs_meta_top.Update,
	Input_Accept = ifs_meta_top.Input_Accept,
	Input_Back = ifs_meta_top.Input_Back,
	Input_GeneralLeft = ifs_meta_top.Input_GeneralLeft,
	Input_GeneralRight = ifs_meta_top.Input_GeneralRight,
}
gBFXS_TopGameP2 = nil
gBFXS_TopClock = 0
gBFXS_TopPadTime = -10

function bfxs_TopSide(tag)
	if ((tag == "i") or (tag == "c")) then
		return 2
	end
	return 1
end

function bfxs_TopPicks(this)
	local p1 = bfxs_TopSide(this.CurButton_s1)
	local p2 = bfxs_TopSide(this.CurButton_s2)
	local mode = "CO-OP"
	if (p1 ~= p2) then
		mode = "VERSUS"
	end
	return format("player 1 %s, player 2 %s - %s", bfxs_GCSideName(p1), bfxs_GCSideName(p2), mode)
end

function bfxs_TopVis(this, split)
	if (this.bfxback) then
		IFObj_fnSetVis(this.bfxback, split)
	end
	if (this.bfxlaunch) then
		IFObj_fnSetVis(this.bfxlaunch, split)
	end
	if (this.bfxroster) then
		IFObj_fnSetVis(this.bfxroster, split and gBFXS_TopN and (gBFXS_TopN >= 3))
	end
	-- 21j-r8: the player boxes; players 3 / 4's rows (hidden here, shown for their players by bfxs_TopLayout)
	if (this.bfxbox) then
		IFObj_fnSetVis(this.bfxbox, split)
	end
	local k
	for k = 3, 4 do
		if (this["bfxrow" .. k] and (not split)) then
			IFObj_fnSetVis(this["bfxrow" .. k], nil)
		end
	end
end

-- 21j-r8: the era's two side pictures (side 1, side 2) - Rebels / Empire or Republic / CIS
function bfxs_TopEraNames()
	local sel = bfxs_GCSelection()
	if (sel and (sel.era == "new")) then
		return "side_r", "side_c"
	end
	return "side_a", "side_i"
end

-- 21j-r8: player p's row of side pictures (1 / 2: the game's, 3 / 4: BF1GameExt's)
function bfxs_TopRow(this, p)
	if (p == 1) then
		return this.buttons_s1
	elseif (p == 2) then
		return this.buttons_s2
	end
	return this["bfxrow" .. p]
end

-- 21j-r8: the rows' places and sizes for this many players (four slots with 3 / 4, the console's two with 2), players
-- 3 / 4's rows shown for their players, the boxes stacked round the middle - each time the screen opens
function bfxs_TopLayout(this)
	if (not this.bfxbox) then
		return
	end
	local n = gBFXS_TopN
	local four = (n >= 3)
	local hw = gBFXS_TopHalfW2
	if (four) then
		hw = gBFXS_TopHalfW4
	end
	local n1, n2 = bfxs_TopEraNames()
	local k
	for k = 1, 4 do
		local row = bfxs_TopRow(this, k)
		local r = gBFXS_TopRows2[k]
		if (four) then
			r = gBFXS_TopRows4[k]
		end
		if (row) then
			local nm
			for nm, _ in { side_a = 1, side_i = 1, side_r = 1, side_c = 1 } do
				if (row[nm] and r) then
					IFImage_fnSetTexturePos(row[nm], -hw, r[1], hw, r[2])
				end
			end
			if (k >= 3) then
				local on = (k <= n)
				IFObj_fnSetVis(row, on)
				for nm, _ in { side_a = 1, side_i = 1, side_r = 1, side_c = 1 } do
					if (row[nm]) then
						IFObj_fnSetVis(row[nm], on and ((nm == n1) or (nm == n2)))
					end
				end
			end
		end
	end
	local p
	for p = 1, 4 do
		local w = this.bfxbox["p" .. p]
		IFObj_fnSetVis(w, p <= n)
		IFObj_fnSetPos(w, 0, (p - (n + 1) * 0.5) * gBFXS_BoxStep)
	end
	local sw, sh = ScriptCB_GetSafeScreenInfo()
	bfxs_Log(format("PICK YOUR SIDE (21j-r8 look): %d players - %s rows of the side pictures (%s: %d wide, rows %s), %d player "
		.. "boxes (%d x %d, %d apart, at 0.80 of the screen across); lock in: %s; the safe screen %s x %s", n,
		(four and "four") or "two", n1 .. " / " .. n2, hw * 2, (four and "-128..-65 -61..2 40..103 107..170") or "-128..2 40..170",
		n, gBFXS_BoxW, gBFXS_BoxH, gBFXS_BoxStep, (gBFXS_TopLockIn and "yes (A locks in, B unlocks; the first on a side commands "
		.. "it)") or "no (2 players as 21f)", tostring(sw), tostring(sh)))
end

ifs_meta_top.Enter = function(this, bFwd)
	if (not ScriptCB_IsSplitscreen()) then
		gBFXS_TopOrig.Enter(this, bFwd)
		bfxs_TopVis(this, nil)
		return
	end
	local fromMaps = gBFXS_GCFromMaps
	gBFXS_GCFromMaps = nil
	-- r3: a conquest in progress (after its battle the game goes ifs_sp -> ifs_meta_new_load -> here): the game's own
	-- Enter loads it and opens the galaxy, which asks whether to save
	if (bFwd and (not fromMaps) and ScriptCB_IsMetagameStateSaved()) then
		gBFXS_TopResumed = 1
		bfxs_TopVis(this, nil)
		bfxs_Log("PICK YOUR SIDE: a conquest is in progress (saved state) - the game's own: it is loaded, the galaxy next")
		gBFXS_TopOrig.Enter(this, bFwd)
		return
	end
	-- back here from that galaxy: on to the menu before (no new side pick for a conquest in progress)
	if ((not bFwd) and gBFXS_TopResumed) then
		gBFXS_TopResumed = nil
		bfxs_TopVis(this, nil)
		bfxs_Log("PICK YOUR SIDE: back from the conquest in progress - on back to the menu before it")
		ScriptCB_PopScreen()
		return
	end
	gBFXS_TopResumed = nil
	gIFShellScreenTemplate_fnEnter(this, bFwd)
	local sel = bfxs_GCSelection()
	local eraNew = nil
	if (sel and (sel.era == "new")) then
		eraNew = 1
	end
	-- the 1-player buttons off; each player's row for the era (the console's split screen set-up)
	local k, v
	for k, v in { side_a = 1, side_i = 1, side_c = 1, side_r = 1, a = 1, i = 1, c = 1, r = 1 } do
		if (this.buttons[k]) then
			IFObj_fnSetVis(this.buttons[k], nil)
		end
	end
	IFObj_fnSetVis(this.buttons_s1.side_a, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_i, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_c, eraNew)
	IFObj_fnSetVis(this.buttons_s1.side_r, eraNew)
	IFObj_fnSetVis(this.buttons_s1.a, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.i, not eraNew)
	IFObj_fnSetVis(this.buttons_s1.c, eraNew)
	IFObj_fnSetVis(this.buttons_s1.r, eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_a, not eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_i, not eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_c, eraNew)
	IFObj_fnSetVis(this.buttons_s2.side_r, eraNew)
	IFObj_fnSetVis(this.buttonWindow, 1)
	if (this.buttonWindow.titleBarElement) then
		IFText_fnSetUString(this.buttonWindow.titleBarElement, ScriptCB_tounicode("Pick your side"))
	end
	if (this.subtitle) then
		IFObj_fnSetVis(this.subtitle, nil)
	end
	bfxs_TopVis(this, 1)
	-- player 1 starts on the side the maps screen had, player 2 with him (co-op)
	local t1 = "a"
	local t2 = "i"
	if (eraNew) then
		t1 = "r"
		t2 = "c"
	end
	if (bFwd or (not this.CurButton_s1)) then
		if (bfxs_GCP1Side(ifs_meta_opts) == 2) then
			this.CurButton_s1 = t2
		else
			this.CurButton_s1 = t1
		end
		this.CurButton_s2 = this.CurButton_s1
	end
	this.CurButton = this.CurButton_s1
	ifs_meta_top_fnHilightSide(this)
	ScriptCB_ReadAllControllers(1)
	gBFXS_TopGameP2 = nil
	bfxs_Ask("pad2reset")
	bfxs_Top4Begin(this, bFwd) -- 21g-r3
	bfxs_TopVis(this, 1)
	if (gBFXS_TopLockIn) then
		bfxs_Log(format("PICK YOUR SIDE (bFwd %s, %d players): %s", tostring(bFwd), gBFXS_TopN, bfxs_Top4Picks(this)))
		return
	end
	bfxs_Log(format("PICK YOUR SIDE (bFwd %s): %s", tostring(bFwd), bfxs_TopPicks(this)))
end

-- ---- 21g-r3: 3 / 4 players --------------------------------------------------------------------------------------------
-- Each player picks a side (left / right; players 1 / 2 on their rows of the console screen, players 3 / 4 in the
-- roster) and locks in with A (B: unlocks). The first player who locked in on a side commands it: only he moves on the
-- galaxy map for it (and picks its bonuses); the others on it fight in its battles. LAUNCH (player 1's A once he is
-- locked in) when everyone is locked in. Pads 3 / 4 are BF1GameExt's ("BFXQ: pad3" / "pad4").
gBFXS_TopN = 2
gBFXS_TopSides = {}
gBFXS_TopLock = {}
gBFXS_TopSeq = 0

function bfxs_Top4Begin(this, bFwd)
	gBFXS_TopN = 2
	if (this.bfxroster or this.bfxbox) then
		local n = bfxs_Ask("playersnow")
		if ((n >= 3) and (this.bfxroster or this.bfxrow3)) then
			gBFXS_TopN = n
		end
	end
	-- 21j-r8: 3 / 4 players lock in; 2 players too with GCCommander2P (and the boxes)
	gBFXS_TopLockIn = (gBFXS_TopN >= 3) or (gBFX_GCCmd2 and this.bfxbox and 1)
	bfxs_TopLayout(this)
	if (not gBFXS_TopLockIn) then
		bfxs_Top4Show(this)
		return
	end
	if (bFwd or (not gBFXS_TopSides[3])) then
		local s1 = bfxs_TopSide(this.CurButton_s1)
		gBFXS_TopSides = { s1, s1, s1, s1 } -- (players 1 / 2's own come from their rows)
		gBFXS_TopLock = {}
		gBFXS_TopSeq = 0
	end
	bfxs_Ask("pad3reset")
	bfxs_Ask("pad4reset")
	bfxs_Top4Show(this)
end

-- player p's side (1 / 2)
function bfxs_Top4Side(this, p)
	if (p == 1) then
		return bfxs_TopSide(this.CurButton_s1)
	elseif (p == 2) then
		return bfxs_TopSide(this.CurButton_s2)
	end
	return gBFXS_TopSides[p] or 1
end

-- the commander of a side: the player who locked in on it first (nil: nobody locked in on it)
function bfxs_Top4Cmd(this, side)
	local best = nil
	local bestSeq = nil
	local p
	for p = 1, gBFXS_TopN do
		local seq = gBFXS_TopLock[p]
		if (seq and (bfxs_Top4Side(this, p) == side) and ((not bestSeq) or (seq < bestSeq))) then
			best = p
			bestSeq = seq
		end
	end
	return best
end

function bfxs_Top4Picks(this)
	local s = ""
	local human1 = nil
	local human2 = nil
	local c1 = bfxs_Top4Cmd(this, 1)
	local c2 = bfxs_Top4Cmd(this, 2)
	local p
	for p = 1, gBFXS_TopN do
		local side = bfxs_Top4Side(this, p)
		if (side == 1) then
			human1 = 1
		else
			human2 = 1
		end
		local st = "picking"
		if ((p == c1) or (p == c2)) then
			st = "COMMANDER"
		elseif (gBFXS_TopLock[p]) then
			st = "READY"
		end
		s = s .. format("player %d %s (%s), ", p, bfxs_GCSideName(side), st)
	end
	if (human1 and human2) then
		return s .. "VERSUS"
	end
	return s .. "CO-OP"
end

-- 21j-r8: a side's colour (the bar's tint and its name's): 1 = Rebels / Republic green, 2 = Empire / CIS red
gBFXS_SideBar = { { 40, 255, 40 }, { 255, 40, 40 } }
gBFXS_SideText = { { 110, 255, 110 }, { 255, 95, 80 } }

-- 21j-r8: the console's look - every player's row (his side lit, the other darker once he has locked in) and his box (its
-- title PLAYER n while he picks, then COMMANDER / PLAYER READY in his side's colour; inside, the player and his side)
function bfxs_TopShowLook(this)
	local box = this.bfxbox
	local n1, n2 = bfxs_TopEraNames()
	local c1 = nil
	local c2 = nil
	if (gBFXS_TopLockIn) then
		c1 = bfxs_Top4Cmd(this, 1)
		c2 = bfxs_Top4Cmd(this, 2)
	end
	local p
	for p = 1, gBFXS_TopN do
		local side = bfxs_Top4Side(this, p)
		local locked = gBFXS_TopLockIn and gBFXS_TopLock[p]
		local row = bfxs_TopRow(this, p)
		if (row and row[n1] and row[n2]) then
			local dim = 0.5
			if (locked) then
				dim = 0.15
			end
			if (side == 1) then
				IFObj_fnSetAlpha(row[n1], 1.0)
				IFObj_fnSetAlpha(row[n2], dim)
			else
				IFObj_fnSetAlpha(row[n1], dim)
				IFObj_fnSetAlpha(row[n2], 1.0)
			end
		end
		local w = box["p" .. p]
		if (w) then
			local title = format("PLAYER %d", p)
)BFXLUA",
R"BFXLUA(			local sname = bfxs_GCSideName(side)
			local bar = { 255, 255, 255 }
			local txt = { 255, 255, 255 }
			if (locked) then
				if ((p == c1) or (p == c2)) then
					title = "COMMANDER"
				else
					title = "PLAYER READY"
				end
				bar = gBFXS_SideBar[side] or bar
				txt = gBFXS_SideText[side] or txt
			else
				sname = "< " .. sname .. " >"
			end
			if (w.titleBarElement) then
				IFText_fnSetUString(w.titleBarElement, ScriptCB_tounicode(title))
				IFObj_fnSetColor(w.titleBarElement, bar[1], bar[2], bar[3])
			end
			IFText_fnSetUString(w.bfxname, ScriptCB_tounicode(format("PLAYER %d", p)))
			IFText_fnSetUString(w.bfxside, ScriptCB_tounicode(sname))
			IFObj_fnSetColor(w.bfxside, txt[1], txt[2], txt[3])
		end
	end
end

function bfxs_Top4Show(this)
	if (this.bfxbox) then
		bfxs_TopShowLook(this) -- 21j-r8
		return
	end
	local r = this.bfxroster
	if (not r) then
		return
	end
	IFText_fnSetUString(r.title, ScriptCB_tounicode("PLAYERS"))
	local c1 = bfxs_Top4Cmd(this, 1)
	local c2 = bfxs_Top4Cmd(this, 2)
	local p
	for p = 1, 4 do
		local on = (p <= gBFXS_TopN)
		IFObj_fnSetVis(r["name" .. p], on)
		IFObj_fnSetVis(r["side" .. p], on)
		IFObj_fnSetVis(r["state" .. p], on)
		if (on) then
			local side = bfxs_Top4Side(this, p)
			local sname = bfxs_GCSideName(side)
			if (not gBFXS_TopLock[p]) then
				sname = "< " .. sname .. " >"
			end
			local st = ""
			if ((p == c1) or (p == c2)) then
				st = "COMMANDER"
			elseif (gBFXS_TopLock[p]) then
				st = "READY"
			end
			IFText_fnSetUString(r["name" .. p], ScriptCB_tounicode(format("PLAYER %d", p)))
			IFText_fnSetUString(r["side" .. p], ScriptCB_tounicode(sname))
			IFText_fnSetUString(r["state" .. p], ScriptCB_tounicode(st))
		end
	end
end

-- one input for player p (1 - 4)
function bfxs_Top4Do(this, name, p)
	if (p > gBFXS_TopN) then
		return
	end
	local locked = gBFXS_TopLock[p]
	if ((name == "Input_GeneralLeft") or (name == "Input_GeneralRight")) then
		if (locked) then
			return -- (B unlocks first)
		end
		if (p <= 2) then
			gBFXS_TopOrig.Input_GeneralLeft(this, p - 1) -- the console's own toggle of his row
		else
			gBFXS_TopSides[p] = 3 - (gBFXS_TopSides[p] or 1)
			ifelm_shellscreen_fnPlaySound("shell_select_change")
		end
		bfxs_Top4Show(this)
		bfxs_Log(format("PICK YOUR SIDE: player %d changed side - %s", p, bfxs_Top4Picks(this)))
	elseif (name == "Input_Accept") then
		if ((p == 1) and (this.CurButton == "_back")) then
			bfxs_TopBack(this)
			return
		end
		if (not locked) then
			gBFXS_TopSeq = gBFXS_TopSeq + 1
			gBFXS_TopLock[p] = gBFXS_TopSeq
			ifelm_shellscreen_fnPlaySound("shell_menu_accept")
			bfxs_Top4Show(this)
			bfxs_Log(format("PICK YOUR SIDE: player %d locked in - %s", p, bfxs_Top4Picks(this)))
		elseif (p == 1) then
			bfxs_Top4Launch(this)
		end
	elseif (name == "Input_Back") then
		if (locked) then
			gBFXS_TopLock[p] = nil
			ifelm_shellscreen_fnPlaySound("shell_menu_exit")
			bfxs_Top4Show(this)
			bfxs_Log(format("PICK YOUR SIDE: player %d unlocked - %s", p, bfxs_Top4Picks(this)))
		elseif (p == 1) then
			bfxs_TopBack(this)
		end
	end
end

-- LAUNCH: everyone locked in - the game's own launch with players 1 / 2's sides, the rest set by bfxs_GCTeams4
function bfxs_Top4Launch(this)
	local p
	for p = 1, gBFXS_TopN do
		if (not gBFXS_TopLock[p]) then
			ifelm_shellscreen_fnPlaySound("shell_menu_error")
			bfxs_Log(format("PICK YOUR SIDE: LAUNCH - player %d has not locked in yet", p))
			return
		end
	end
	local g = { n = gBFXS_TopN, side = {}, cmd = {} }
	for p = 1, gBFXS_TopN do
		g.side[p] = bfxs_Top4Side(this, p)
	end
	g.cmd[1] = bfxs_Top4Cmd(this, 1)
	g.cmd[2] = bfxs_Top4Cmd(this, 2)
	ifelm_shellscreen_fnPlaySound("shell_menu_accept")
	bfxs_Log("PICK YOUR SIDE: LAUNCH - " .. bfxs_Top4Picks(this))
	gBFXS_GC4 = g
	bfxs_GCLaunch(g.side[1], g.side[2])
	gBFXS_GC4 = nil
end

-- one input for one player (0 / 1)
function bfxs_TopDo(this, name, joy)
	if (gBFXS_TopLockIn) then
		bfxs_Top4Do(this, name, joy + 1) -- 21g-r3; 21j-r8: 2 players too (GCCommander2P)
		return
	end
	if ((name == "Input_GeneralLeft") or (name == "Input_GeneralRight")) then
		-- two sides: left and right both change it (the console's own toggle)
		gBFXS_TopOrig.Input_GeneralLeft(this, joy)
		bfxs_Top4Show(this) -- 21j-r8: the boxes
		bfxs_Log(format("PICK YOUR SIDE: player %d changed side - %s", joy + 1, bfxs_TopPicks(this)))
	elseif (name == "Input_Accept") then
		if (joy > 0.5) then
			return -- player 1 launches
		end
		if (this.CurButton == "_back") then
			bfxs_TopBack(this)
		elseif (ScriptCB_IsLastInputFromJoystick() or (this.CurButton == "_ok")) then
			ifelm_shellscreen_fnPlaySound("shell_menu_accept")
			local p1 = bfxs_TopSide(this.CurButton_s1)
			local p2 = bfxs_TopSide(this.CurButton_s2)
			bfxs_Log(format("PICK YOUR SIDE: LAUNCH - %s", bfxs_TopPicks(this)))
			bfxs_GCLaunch(p1, p2)
		end
	elseif (name == "Input_Back") then
		if (joy < 0.5) then
			bfxs_TopBack(this)
		end
	end
end

function bfxs_TopBack(this)
	ifelm_shellscreen_fnPlaySound("shell_menu_exit")
	bfxs_Log("PICK YOUR SIDE: BACK - the maps")
	ScriptCB_PopScreen()
end

-- the game's input: player 1 = its own devices (joystick 0 / none), player 2 = joystick 1
function bfxs_TopInput(this, name, Joystick)
	if (not ScriptCB_IsSplitscreen()) then
		if (gBFXS_TopOrig[name]) then
			gBFXS_TopOrig[name](this, Joystick)
		end
		return
	end
	local joy = 0
	if ((gBFXS_TopN >= 3) and (type(Joystick) == "number") and (Joystick > 1.5)) then
		return -- 21g-r3: (pads 3 / 4 come from BF1GameExt, not from the game)
	end
	if ((type(Joystick) == "number") and (Joystick > 0.5)) then
		joy = 1
		if (not gBFXS_TopGameP2) then
			gBFXS_TopGameP2 = 1
			bfxs_Log(format("PICK YOUR SIDE: the game itself gives player 2's pad (%s, joystick %s) - BF1GameExt's pad 2 "
				.. "is not read here from now on", name, tostring(Joystick)))
			if (gBFXS_TopClock - gBFXS_TopPadTime < 0.5) then
				bfxs_Log("  (BF1GameExt's pad 2 gave a press just before - not done twice)")
				return
			end
		end
	end
	bfxs_TopDo(this, name, joy)
end

ifs_meta_top.Input_GeneralLeft = function(this, Joystick)
	bfxs_TopInput(this, "Input_GeneralLeft", Joystick)
end
ifs_meta_top.Input_GeneralRight = function(this, Joystick)
	bfxs_TopInput(this, "Input_GeneralRight", Joystick)
end
ifs_meta_top.Input_Accept = function(this, Joystick)
	bfxs_TopInput(this, "Input_Accept", Joystick)
end
ifs_meta_top.Input_Back = function(this, Joystick)
	bfxs_TopInput(this, "Input_Back", Joystick)
end

ifs_meta_top.Update = function(this, fDt)
	if (gBFXS_TopOrig.Update) then
		gBFXS_TopOrig.Update(this, fDt)
	end
	if (not ScriptCB_IsSplitscreen()) then
		return
	end
	gBFXS_TopClock = gBFXS_TopClock + fDt
	-- 21g-r3: pads 3 / 4 with 3 / 4 players: left / right = his side, A = lock in, B = unlock
	if (gBFXS_TopN >= 3) then
		local p
		for p = 3, gBFXS_TopN do
			local m = 0
			local ev = bfxs_Ask(format("pad%d", p))
			while ((ev > 0) and (m < 8)) do
				if ((ev == 6) or (ev == 7)) then
					bfxs_Top4Do(this, "Input_GeneralLeft", p)
				elseif (ev == 1) then
					bfxs_Top4Do(this, "Input_Accept", p)
				elseif (ev == 2) then
					bfxs_Top4Do(this, "Input_Back", p)
				end
				m = m + 1
				ev = bfxs_Ask(format("pad%d", p))
			end
		end
	end
	if (gBFXS_TopGameP2) then
		return
	end
	-- pad 2 (BF1GameExt's) until the game gives it: left / right = his side (21g-r3: with 3 / 4 players A / B too; 21j-r8:
	-- with 2 players locking in too)
	local n = 0
	local e = bfxs_Ask("pad2")
	while ((e > 0) and (n < 8)) do
		if ((e == 6) or (e == 7)) then
			gBFXS_TopPadTime = gBFXS_TopClock
			bfxs_TopDo(this, "Input_GeneralLeft", 1)
		elseif (gBFXS_TopLockIn and ((e == 1) or (e == 2))) then
			gBFXS_TopPadTime = gBFXS_TopClock
			if (e == 1) then
				bfxs_TopDo(this, "Input_Accept", 1)
			else
				bfxs_TopDo(this, "Input_Back", 1)
			end
		end
		n = n + 1
		e = bfxs_Ask("pad2")
	end
end

bfxs_Log(format("ifs_meta_top patched (%s, %s): PICK YOUR SIDE for Galactic Conquest in split screen", gBFX_Phase or "?",
	gBFX_Screen or "?"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g-r3: the galaxy map (ifs_meta_main) with 3 / 4 players: each side is moved only by its commander (the player who
-- locked in on it first on PICK YOUR SIDE). The game reads a side from the joystick a press came on (joystick 1 = player
-- 1's team, joystick 2 = player 2's: metagame_state_local.mJoystickTeams); a commander's press is handed to it as the
-- joystick of his side - when neither player 1 nor player 2 is on his side, joystick 2 is given his side for that one
-- call. Presses of players who do not command are dropped; players 3 / 4's come from BF1GameExt's pads 3 / 4. The pause
-- (B / BACK) and the pause menu stay the game's own (players 1 / 2). The sides and commanders come from BF1GameExt
-- ("gcget": it keeps them across the battles), checked against this conquest's players 1 / 2. With 2 players: the
-- game's own.
-- 21g-r4: (1) THE AI'S TURN IN CO-OP. With players 1 and 2 on one side the game's AI moves the other side on the third
-- joystick (metagame_ai.lua: "AI will take over the third joyStick input"), and its team (mJoystickTeams[3]) was never
-- set: the game's script stopped at every AI move (format "team%d" of nil), so the AI side never picked its planet or
-- its bonus and the conquest stood still - 2-player co-op as well as 3 / 4 players on one side. The third joystick is
-- put on the AI's team when the galaxy opens and in each frame (bfxs_GCAIJoy). (2) THE TURN SCANNER ([Splitscreen]
-- GCTurnScan=1, log only, [gcturn] lines, 2 / 3 / 4 players): each turn (the page, the side choosing, who moves it - the
-- player and his pad, or the AI and its joystick - and the raw state), each choice (planet, bonuses), the battle and its
-- result (the game's rule: the attacker attacks again after a win, the turn passes only when he loses), every press
-- (whose, from which pad, the joystick it went in on, what the game's own checks say, what changed), the game's script
-- errors on this screen (caught to be logged: its Update and the pads' presses), and a turn that stands still 30 s.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_meta_main and ifs_meta_main.Input_Accept and (not ifs_meta_main.bfxmain)) then
ifs_meta_main.bfxmain = 1

gBFXS_MainOrig = {
	Enter = ifs_meta_main.Enter,
	Update = ifs_meta_main.Update,
}
gBFXS_MainCfg = nil
gBFXS_MainBlocked = nil
gBFXS_MainShort = { Input_GeneralLeft = "LEFT", Input_GeneralRight = "RIGHT", Input_GeneralUp = "UP",
	Input_GeneralDown = "DOWN", Input_Accept = "A", Input_Misc = "Y" }

-- this conquest's sides and commanders, or nil (the game's own: players 1 / 2 each move the side of their joystick).
-- BF1GameExt's picks are used when they are this conquest's: players 1 / 2 on its joystick teams, and every human side
-- (no aicontrol) commanded by a player of this match who is on it. Otherwise players 1 / 2 move the sides of their
-- joysticks, and a human side neither has (a conquest made with 3 / 4 players, resumed with fewer, or other picks) is
-- moved by player 1 too - never a human side nobody can move.
function bfxs_MainCfg()
	if (not ScriptCB_IsSplitscreen()) then
		return nil
	end
	local n = bfxs_Ask("playersnow")
	local jt = metagame_state_local.mJoystickTeams
	local human = { (not metagame_state.team1.aicontrol), (not metagame_state.team2.aicontrol) }
	local side = {}
	local cmd = { 0, 0 }
	local i
	local ok = (n >= 3) or ((n == 2) and gBFX_GCCmd2) -- 21j-r8: 2 players' commanders too (GCCommander2P)
	if (ok) then
		for i = 1, 4 do
			side[i] = bfxs_Ask(format("gcget %d", i))
		end
		cmd = { bfxs_Ask("gcget 5"), bfxs_Ask("gcget 6") }
		ok = (side[1] == jt[1]) and (side[2] == jt[2])
		for i = 1, 2 do
			if (ok and human[i] and ((cmd[i] < 1) or (cmd[i] > n) or (side[cmd[i]] ~= i))) then
				ok = nil
			end
		end
		if (not ok) then
)BFXLUA",
R"BFXLUA(			bfxs_Log(format("galaxy (%d players): BF1GameExt's picks (players 1 / 2 on %d / %d, commanders %d / %d) are not this "
				.. "conquest's (players 1 / 2 on %s / %s, human sides: 1 %s, 2 %s)", n, side[1], side[2], cmd[1], cmd[2],
				tostring(jt[1]), tostring(jt[2]), tostring(human[1]), tostring(human[2])))
			bfxt_Log(format("the picks BF1GameExt kept (players 1 / 2 on %d / %d, commanders %d / %d) are not this conquest's "
				.. "(players 1 / 2 on %s / %s) - players 1 / 2 move their joysticks' sides", side[1], side[2], cmd[1], cmd[2],
				tostring(jt[1]), tostring(jt[2])))
			bfxs_Ask("gcclear")
		end
	end
	if (ok) then
		return { n = n, side = side, cmd = cmd }
	end
	-- the game's own joysticks; a human side without one goes to player 1
	side = { jt[1], jt[2], 0, 0 }
	cmd = { 0, 0 }
	local orphan = nil
	for i = 1, 2 do
		if (human[i]) then
			if (jt[1] == i) then
				cmd[i] = 1
			elseif (jt[2] == i) then
				cmd[i] = 2
			else
				cmd[i] = 1
				orphan = i
			end
		end
	end
	if (not orphan) then
		return nil
	end
	bfxs_Log(format("galaxy: side %d is human but neither player 1 nor player 2 is on it - player 1 moves it too", orphan))
	bfxt_Log(format("side %d is a player's but neither player 1 nor player 2 is on it - player 1 moves it too", orphan))
	if (n < 2) then
		n = 2
	end
	return { n = n, side = side, cmd = cmd }
end

-- the side player p moves now (nil: none): the one he commands; when he commands both (a side without its own player),
-- the one choosing on this page (pages 1 / 3: the attacker, the picking team; 2 / 4: the defender)
function bfxs_MainTarget(cfg, p)
	local a = (cfg.cmd[1] == p)
	local b = (cfg.cmd[2] == p)
	if (a and b) then
		local pick = metagame_state.pickteam or 1
		if ((gMetagameCurrentPage == 2) or (gMetagameCurrentPage == 4)) then
			return 3 - pick
		end
		return pick
	elseif (a) then
		return 1
	elseif (b) then
		return 2
	end
	return nil
end

-- 21g-r4: co-op - the game's AI moves its side on the third joystick: that joystick on the AI's team
function bfxs_GCAIJoy(why)
	local jt = metagame_state_local.mJoystickTeams
	local j1 = jt[1]
	if (((j1 == 1) or (j1 == 2)) and (jt[2] == j1)) then
		local ai = 3 - j1
		local t = metagame_state[format("team%d", ai)]
		if (t and t.aicontrol and (jt[3] ~= ai)) then
			local was = jt[3]
			jt[3] = ai
			bfxs_Log(format("galaxy: co-op - the AI's joystick (the game's third) put on team %d (it was %s) - %s", ai,
				tostring(was), why))
			bfxt_Log(format("FIX: co-op - the AI moves %s on the game's third joystick, whose team was %s: set to %d (%s)",
				bfxt_Side(ai), tostring(was), ai, why))
		end
	end
end

-- ---- 21g-r4: the turn scanner ----
gBFXT = { turn = 0, sig = nil, since = 0, still = 30, lines = 0, more = 0, taken = 0, ignored = 0, dropped = 0,
	errn = 0, presses = 0, errs = {}, choice = nil, paused = nil, blocked = {} }
gBFXT_Err = nil

function bfxt_OnErr(m)
	gBFXT_Err = m
end

-- a side's name, from the conquest's own teams
function bfxt_Side(s)
	if ((s ~= 1) and (s ~= 2)) then
		return format("side %s", tostring(s))
	end
	local t = metagame_state[format("team%d", s)]
	if (t and t.teamname) then
		local u = ScriptCB_getlocalizestr(t.teamname, 1)
		if (u) then
			return format("side %d (%s)", s, ScriptCB_ununicode(u))
		end
	end
	return format("side %d", s)
end

-- player p's pad as the galaxy gets it
function bfxt_Pad(p)
	if (p == 1) then
		return "the game's joystick 0 / keyboard / mouse"
	elseif (p == 2) then
		return "the game's joystick 1"
	end
	return format("BF1GameExt's pad %d, XInput %d", p, bfxs_Ask(format("pad%dindex", p)))
end

-- who moves side s now: a player (his pad), the AI (its joystick), or nobody
function bfxt_Mover(s)
	if ((s ~= 1) and (s ~= 2)) then
		return "nobody (no side)"
	end
	local jt = metagame_state_local.mJoystickTeams
	local t = metagame_state[format("team%d", s)]
	if (t and t.aicontrol) then
		-- as metagame_ai_fnUpdate picks the AI's joystick
		local j = nil
		if (jt[1] == s) then
			j = 0
		end
		if (jt[2] == s) then
			j = 1
		end
		if (jt[1] == jt[2]) then
			j = 2
		end
		if (not j) then
			return "the AI - but no joystick is on its team: the game's AI does NOT move it"
		end
		if (jt[j + 1] ~= s) then
			return format("the AI on joystick %d - whose team is %s, not %d: the game's script STOPS at each AI move", j,
				tostring(jt[j + 1]), s)
		end
		return format("the AI (the game's joystick %d)", j)
	end
	local cfg = gBFXS_MainCfg
	if (cfg) then
		local c = cfg.cmd[s] or 0
		if (c < 1) then
			return "NOBODY (no commander)"
		end
		return format("player %d (%s), its commander", c, bfxt_Pad(c))
	end
	local who = nil
	if (jt[1] == s) then
		who = "player 1 (" .. bfxt_Pad(1) .. ")"
	end
	if (jt[2] == s) then
		if (who) then
			who = who .. " and player 2 (" .. bfxt_Pad(2) .. ")"
		else
			who = "player 2 (" .. bfxt_Pad(2) .. ")"
		end
	end
	return who or "NOBODY (no joystick on its team and not the AI's)"
end

-- the turn now: the page, the side choosing, what it chooses, who moves it
function bfxt_Turn()
	local page = gMetagameCurrentPage
	local pick = metagame_state.pickteam
	if ((pick ~= 1) and (pick ~= 2)) then
		return format("page %s - no side picks (pickteam %s)", tostring(page), tostring(pick))
	end
	local s = pick
	local what = format("(page %s?)", tostring(page))
	if (page == 1) then
		if (metagame_state.applyingbonus) then
			what = "picks the planet for its secret base bonus"
		else
			what = "picks the planet to attack"
		end
	elseif (page == 2) then
		what = "attacks - the attack is shown (nobody chooses)"
	elseif (page == 3) then
		what = "picks the attacker's bonus"
	elseif (page == 4) then
		s = 3 - pick
		what = "picks the defender's bonus"
	end
	return format("page %s: %s %s - moved by %s", tostring(page), bfxt_Side(s), what, bfxt_Mover(s))
end

-- the raw state behind it
function bfxt_State()
	local jt = metagame_state_local.mJoystickTeams
	return format("pickteam %s, page %s, applying bonus %s; joysticks' teams %s / %s / %s; AI control: team 1 %s, "
		.. "team 2 %s; numhumans %s; split %s", tostring(metagame_state.pickteam), tostring(gMetagameCurrentPage),
		tostring(metagame_state.applyingbonus), tostring(jt[1]), tostring(jt[2]), tostring(jt[3]),
		tostring(metagame_state.team1.aicontrol), tostring(metagame_state.team2.aicontrol),
		tostring(metagame_state.numhumans), tostring(metagame_state_local.mIsSplitscreen))
end

function bfxt_TurnSig()
	local jt = metagame_state_local.mJoystickTeams
	return format("%s|%s|%s|%s|%s|%s|%s|%s", tostring(gMetagameCurrentPage), tostring(metagame_state.pickteam),
		tostring(metagame_state.applyingbonus), tostring(jt[1]), tostring(jt[2]), tostring(jt[3]),
		tostring(metagame_state.team1.aicontrol), tostring(metagame_state.team2.aicontrol))
end

-- the choices made so far on this turn (for "choice:" lines)
gBFXT_ChoiceNames = { "ATTACK planet chosen", "attack planet (the game's)", "BASE BONUS planet chosen",
	"base bonus planet (the game's)", "side 1's BONUS chosen", "side 2's BONUS chosen", "side 1's bonus (the game's)",
	"side 2's bonus (the game's)", "the battle's map" }
function bfxt_Choices()
	local v = metagame_vote_state or {}
	local h = metagame_host_state or {}
	return { v.gAttackPlanetVote, h.gAttackPlanet, v.gBonusPlanetVote, h.gBonusPlanet, v.gBonus1Vote, v.gBonus2Vote,
		h.gBonus1, h.gBonus2, metagame_state.NextMapName }
end

-- what a press can change
gBFXT_SnapNames = { "planet picked", "base bonus planet picked", "side 1's bonus picked", "side 2's bonus picked",
	"ATTACK chosen", "BASE BONUS chosen", "side 1's BONUS chosen", "side 2's BONUS chosen", "page", "paused" }
function bfxt_Snap()
	local v = metagame_vote_state or {}
	return { gAttackPlanetSelect, gBonusPlanetSelect, gTopBonusIdx, gBotBonusIdx, v.gAttackPlanetVote,
		v.gBonusPlanetVote, v.gBonus1Vote, v.gBonus2Vote, gMetagameCurrentPage, ifs_meta_main.paused }
end

function bfxt_Changes(a, b, names, n)
	local s = nil
	local i
	for i = 1, n do
		if (a[i] ~= b[i]) then
			local one = format("%s %s -> %s", names[i], tostring(a[i]), tostring(b[i]))
			if (s) then
				s = s .. ", " .. one
			else
				s = one
			end
		end
	end
	return s
end

-- what the game's own checks make of a press on joystick j (0-based) now
function bfxt_Verdict(name, j, bFromAI)
	if (ifs_meta_main.paused) then
		return "the pause menu's"
	end
	if (name == "Input_Misc") then
		return "nothing (Y does nothing on the galaxy map)"
	end
	if (type(j) ~= "number") then
		return format("joystick %s (the mouse / keyboard?)", tostring(j))
	end
	local jt = metagame_state_local.mJoystickTeams
	local team = jt[j + 1]
	if ((team ~= 1) and (team ~= 2)) then
		return format("joystick %d has no team (%s): the game's script STOPS here", j, tostring(team))
	end
	local t = metagame_state[format("team%d", team)]
	if (t.aicontrol ~= bFromAI) then
		if (t.aicontrol) then
			return format("IGNORED - %s is the AI's", bfxt_Side(team))
		end
		return format("IGNORED - %s is a player's, not the AI's", bfxt_Side(team))
	end
	local page = gMetagameCurrentPage
	local pick = metagame_state.pickteam
	if (page == 1) then
		if (team == pick) then
			return format("taken - %s's turn", bfxt_Side(team))
		end
		return format("IGNORED - not %s's turn (%s picks)", bfxt_Side(team), bfxt_Side(pick))
	elseif ((page == 3) or (page == 4)) then
		if (name ~= "Input_Accept") then
			return "moves the bonus pick (on the bonus pages any side's pad moves it - the game's own)"
		end
		local turn = pick
		if (page == 4) then
			turn = 3 - pick
		end
		if (team == turn) then
			return format("taken - %s's bonus", bfxt_Side(team))
		end
		return format("IGNORED - %s picks its bonus now, not %s", bfxt_Side(turn), bfxt_Side(team))
	end
	return format("IGNORED - page %s (nothing to choose)", tostring(page))
end

-- one press, logged (each turn: 30 lines, 60 with A presses, 80 with errors; the rest counted)
function bfxt_Press(name, who, how, j, team, bFromAI, verdict, before, err)
	local g = gBFXT
	g.presses = g.presses + 1
	local ch = bfxt_Changes(before, bfxt_Snap(), gBFXT_SnapNames, 10)
	if (err) then
		-- (counted by bfxt_Error)
	elseif (strfind(verdict, "IGNORED", 1, 1)) then
		g.ignored = g.ignored + 1
	else
		g.taken = g.taken + 1
	end
	if ((g.lines < 30) or ((name == "Input_Accept") and (g.lines < 60)) or (err and (g.lines < 80))) then
		g.lines = g.lines + 1
		local e = ""
		if (err) then
			e = " | THE GAME'S SCRIPT STOPPED: " .. err
		end
		bfxt_Log(format("press: %s %s (%s) -> joystick %s (team %s)%s; the game: %s; %s%s", who,
			gBFXS_MainShort[name] or name, how, tostring(j), tostring(team), (bFromAI and ", from the AI") or "", verdict,
			ch or "nothing changed", e))
	else
		g.more = g.more + 1
	end
end

-- a press that went nowhere (player p commands no side, or a joystick BF1GameExt does not use) - once a turn each
function bfxt_Dropped(key, text)
	local g = gBFXT
	g.dropped = g.dropped + 1
	if (not g.blocked[key]) then
		g.blocked[key] = 1
		bfxt_Log("dropped: " .. text)
	end
end

-- the game's script stopped (counted by message: the 1st, 10th, 100th and 1000th time logged)
function bfxt_Error(where, msg)
	local g = gBFXT
	local n = (g.errs[msg] or 0) + 1
	g.errs[msg] = n
	g.errn = g.errn + 1
	if ((n == 1) or (n == 10) or (n == 100) or (n == 1000)) then
		bfxt_Log(format("ERROR - the game's galaxy script stopped (%s, %d time(s) so far): %s | %s", where, n, msg,
			bfxt_Turn()))
	end
end

-- once a frame: a new turn, a choice made, the pause, a turn standing still
function bfxt_Frame(this, fDt)
	local g = gBFXT
	local sig = bfxt_TurnSig()
	if (sig ~= g.sig) then
		if (g.sig and (g.more > 0)) then
			bfxt_Log(format("(turn %d: %d more press(es) not logged)", g.turn, g.more))
		end
		g.sig = sig
		g.turn = g.turn + 1
		g.since = 0
		g.still = 30
		g.lines = 0
		g.more = 0
		g.taken = 0
		g.ignored = 0
		g.dropped = 0
		g.presses = 0
		g.blocked = {}
		bfxt_Log(format("TURN %d - %s | %s", g.turn, bfxt_Turn(), bfxt_State()))
	else
		g.since = g.since + (fDt or 0)
		if (g.since >= g.still) then
)BFXLUA",
R"BFXLUA(			bfxt_Log(format("turn %d still on after %d s - %s | presses on it: %d (taken %d, ignored by the game %d, dropped %d), "
				.. "script errors so far %d | %s", g.turn, g.still, bfxt_Turn(), g.presses, g.taken, g.ignored, g.dropped,
				g.errn, bfxt_State()))
			g.still = g.still * 2
		end
	end
	local c = bfxt_Choices()
	if (g.choice) then
		local ch = bfxt_Changes(g.choice, c, gBFXT_ChoiceNames, 9)
		if (ch) then
			bfxt_Log("choice: " .. ch)
		end
	end
	g.choice = c
	if (ifs_meta_main.paused ~= g.paused) then
		g.paused = ifs_meta_main.paused
		if (g.paused) then
			bfxt_Log("paused (the galaxy's pause menu)")
		else
			bfxt_Log("pause menu closed")
		end
	end
end

-- the battle starting (the game's own, logged first)
function bfxt_BattleWrap()
	if (gBFXT_BattleOrig or (not ifs_meta_battle_fnOnEnter)) then
		return
	end
	gBFXT_BattleOrig = ifs_meta_battle_fnOnEnter
	ifs_meta_battle_fnOnEnter = function(this)
		local jt = metagame_state_local.mJoystickTeams
		local pick = metagame_state.pickteam
		local v = metagame_vote_state or {}
		bfxt_Log(format("BATTLE: %s attacks %s (map %s), %s defends; bonuses - side 1 %s, side 2 %s; players 1 / 2 on "
			.. "teams %s / %s | %s", bfxt_Side(pick), tostring(metagame_state.pickplanet),
			tostring(metagame_state.NextMapName), bfxt_Side(3 - (pick or 1)), tostring(v.gBonus1Vote),
			tostring(v.gBonus2Vote), tostring(jt[1]), tostring(jt[2]), bfxt_State()))
		gBFXT_BattleOrig(this)
	end
end

-- one press handed to the game's own input on joystick j (0-based) - logged by the scanner; the presses from the pads
-- (not the AI's) caught when the game's script stops, for the log
function bfxs_MainGame(this, name, j, bFromAI, who, how, catch)
	if (not bfxt_On()) then
		gBFXS_MainOrig[name](this, j, bFromAI)
		return
	end
	local before = bfxt_Snap()
	local verdict = bfxt_Verdict(name, j, bFromAI)
	local team = nil
	if (type(j) == "number") then
		team = metagame_state_local.mJoystickTeams[j + 1]
	end
	local err = nil
	if (catch and (type(call) == "function")) then
		gBFXT_Err = nil
		call(gBFXS_MainOrig[name], { this, j, bFromAI }, "x", bfxt_OnErr)
		err = gBFXT_Err
	else
		gBFXS_MainOrig[name](this, j, bFromAI)
	end
	bfxt_Press(name, who, how, j, team, bFromAI, verdict, before, err)
	if (err) then
		bfxt_Error(format("%s from %s", gBFXS_MainShort[name] or name, who), err)
	end
end

-- one press of player p (1 - 4): only a commander's, as the joystick of his side
function bfxs_MainAs(this, name, p, fromPad)
	local cfg = gBFXS_MainCfg
	bfxs_MainUnmap()
	local side = bfxs_MainTarget(cfg, p)
	local how = bfxt_Pad(p)
	if (not side) then
		if (gBFXS_MainBlocked ~= p) then
			gBFXS_MainBlocked = p
			bfxs_Log(format("galaxy: player %d's %s dropped - he does not command a side (side 1: player %d, side 2: player %d)",
				p, gBFXS_MainShort[name] or name, cfg.cmd[1], cfg.cmd[2]))
		end
		bfxt_Dropped(format("p%d", p), format("player %d's %s (%s) - he commands no side (side 1: player %d, side 2: player "
			.. "%d) | %s", p, gBFXS_MainShort[name] or name, how, cfg.cmd[1], cfg.cmd[2], bfxt_Turn()))
		return
	end
	gBFXS_MainBlocked = nil
	if (fromPad and (name == "Input_Accept")) then
		this.CurButton = "_ok" -- (the game sets it only for its own pads)
	end
	local who = format("player %d, %s's commander,", p, bfxt_Side(side))
	local jt = metagame_state_local.mJoystickTeams
	if (jt[1] == side) then
		bfxs_MainGame(this, name, 0, nil, who, how, 1)
	elseif (jt[2] == side) then
		bfxs_MainGame(this, name, 1, nil, who, how, 1)
	else
		gBFXS_MainKeep = { jt[2] } -- (put back after the call; and by bfxs_MainUnmap if the call broke off)
		jt[2] = side
		bfxs_MainGame(this, name, 1, nil, who, how .. "; joystick 1 lent his side for this press", 1)
		bfxs_MainUnmap()
	end
end

-- joystick 2's own team back, if a call left it changed
gBFXS_MainKeep = nil
function bfxs_MainUnmap()
	if (gBFXS_MainKeep) then
		metagame_state_local.mJoystickTeams[2] = gBFXS_MainKeep[1]
		gBFXS_MainKeep = nil
	end
end

-- the game's own press (its joysticks 0 / 1 = players 1 / 2, the AI's with bFromAI)
function bfxs_MainInput(this, name, iJoystick, bFromAI)
	bfxs_MainUnmap()
	-- (BACK clicked with the mouse is the game's pause; a pad's A the game turns into "_ok" itself)
	local back = (name == "Input_Accept") and (this.CurButton == "_back") and (not ScriptCB_IsLastInputFromJoystick())
	if ((not gBFXS_MainCfg) or bFromAI or ifs_meta_main.paused or back) then
		local who = "the AI"
		local how = "metagame_ai"
		if (not bFromAI) then
			who = "the game's joystick " .. tostring(iJoystick)
			how = "the game's own"
			if (iJoystick == 0) then
				who = "player 1"
				how = bfxt_Pad(1)
			elseif (iJoystick == 1) then
				who = "player 2"
				how = bfxt_Pad(2)
			end
		end
		bfxs_MainGame(this, name, iJoystick, bFromAI, who, how, not bFromAI)
		return
	end
	if ((type(iJoystick) == "number") and (iJoystick > 1.5)) then
		bfxt_Dropped(format("j%d", iJoystick), format("the game's joystick %d's %s - players 3 / 4 come from BF1GameExt's pads",
			iJoystick, gBFXS_MainShort[name] or name))
		return -- (pads 3 / 4 come from BF1GameExt, not from the game)
	end
	local p = 1
	if ((type(iJoystick) == "number") and (iJoystick > 0.5)) then
		p = 2
	end
	bfxs_MainAs(this, name, p, nil)
end

function bfxs_MainWrap(name)
	gBFXS_MainOrig[name] = ifs_meta_main[name]
	ifs_meta_main[name] = function(this, iJoystick, bFromAI)
		bfxs_MainInput(this, %name, iJoystick, bFromAI)
	end
end
bfxs_MainWrap("Input_GeneralLeft")
bfxs_MainWrap("Input_GeneralRight")
bfxs_MainWrap("Input_GeneralUp")
bfxs_MainWrap("Input_GeneralDown")
bfxs_MainWrap("Input_Accept")
bfxs_MainWrap("Input_Misc")

ifs_meta_main.Enter = function(this, bFwd)
	-- (21g-r4: back from a battle? - the game's Enter reloads the conquest and plays the result)
	local back = nil
	local pickWas = metagame_state.pickteam
	if (bfxt_On() and bFwd and ScriptCB_IsMetagameStateSaved and ScriptCB_IsMetagameStateSaved()) then
		back = 1
	end
	gBFXS_MainOrig.Enter(this, bFwd)
	bfxs_GCAIJoy("the galaxy opened")
	gBFXS_MainCfg = bfxs_MainCfg()
	gBFXS_MainBlocked = nil
	if (gBFXS_MainCfg) then
		local c = gBFXS_MainCfg
		bfxs_Log(format("galaxy (bFwd %s, %d players): sides - player 1 %s, player 2 %s, player 3 %s, player 4 %s; side 1 moved "
			.. "by player %d, side 2 by player %d (0: the AI)", tostring(bFwd), c.n, tostring(c.side[1]), tostring(c.side[2]),
			tostring(c.side[3]), tostring(c.side[4]), c.cmd[1], c.cmd[2]))
		bfxs_Ask("pad3reset")
		bfxs_Ask("pad4reset")
	end
	if (bfxt_On()) then
		bfxt_BattleWrap()
		local c = gBFXS_MainCfg
		local who = "the game's own: players 1 / 2 move their joysticks' sides"
		if (c) then
			who = format("%d players - sides: player 1 %s, player 2 %s, player 3 %s, player 4 %s; commanders: side 1 player %d, "
				.. "side 2 player %d (0: the AI)", c.n, tostring(c.side[1]), tostring(c.side[2]), tostring(c.side[3]),
				tostring(c.side[4]), c.cmd[1], c.cmd[2])
		end
		bfxt_Log(format("GALAXY MAP (bFwd %s, %s; caught script errors: %s) - %s | %s | %s", tostring(bFwd),
			(ScriptCB_IsSplitscreen() and "split screen") or "one player", (type(call) == "function" and "yes") or "no", who,
			bfxt_Turn(), bfxt_State()))
		if (back) then
			local r = nil
			if (ScriptCB_GetLastBattleVictory) then
				r = ScriptCB_GetLastBattleVictory()
			end
			local rule = "the attacker LOST - the turn passes to the other side"
			if (metagame_state.pickteam == pickWas) then
				rule = "the attacker WON - he attacks again (the game's rule: the turn passes only when the attacker loses)"
			end
			if ((type(r) ~= "number") or (r < 1)) then
				rule = "no result (the battle was left?) - " .. rule
			end
			bfxt_Log(format("BACK FROM THE BATTLE: result %s (1 = the attacker won, 2 = the defender won; sides backwards %s); "
				.. "the attacker was %s, %s picks now: %s", tostring(r), tostring(metagame_state.teamsAreBackwards),
				bfxt_Side(pickWas), bfxt_Side(metagame_state.pickteam), rule))
		end
		gBFXT.sig = nil -- (the turn line next frame)
		gBFXT.choice = nil
	end
end

-- pads 3 / 4: a commander's presses (d-pad / left stick, A)
gBFXS_MainPadEv = { [1] = "Input_Accept", [4] = "Input_GeneralUp", [5] = "Input_GeneralDown", [6] = "Input_GeneralLeft",
	[7] = "Input_GeneralRight" }
ifs_meta_main.Update = function(this, fDt)
	bfxs_MainUnmap()
	bfxs_GCAIJoy("a frame")
	if (gBFXS_MainOrig.Update) then
		if (bfxt_On() and (type(call) == "function")) then
			gBFXT_Err = nil
			call(gBFXS_MainOrig.Update, { this, fDt }, "x", bfxt_OnErr)
			if (gBFXT_Err) then
				bfxt_Error("its Update", gBFXT_Err)
			end
		else
			gBFXS_MainOrig.Update(this, fDt)
		end
	end
	bfxs_MainUnmap()
	if (bfxt_On()) then
		bfxt_Frame(this, fDt)
	end
	local cfg = gBFXS_MainCfg
	if (not cfg) then
		return
	end
	local p
	for p = 3, cfg.n do
		local m = 0
		local ev = bfxs_Ask(format("pad%d", p))
		while ((ev > 0) and (m < 8)) do
			local name = gBFXS_MainPadEv[ev]
			if (name and (not ifs_meta_main.paused)) then
				bfxs_MainAs(this, name, p, 1)
			end
			m = m + 1
			ev = bfxs_Ask(format("pad%d", p))
		end
	end
end

bfxs_Log(format("ifs_meta_main patched (%s, %s): the galaxy map - with 3 / 4 players each side moved by its commander only "
	.. "(21j-r8: 2 players too: %s); co-op: the AI's joystick on its team (21g-r4); turn scanner %s", gBFX_Phase or "?",
	gBFX_Screen or "?", (gBFX_GCCmd2 and "GCCommander2P=1") or "no - the game's own", (gBFX_GCTurnScan and "ON ([gcturn] lines)")
	or "off"))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- 21g-r2: ifs_saveop - player 2's temporary profile is never saved (the campaign's briefing and Galactic Conquest save
-- profile 2 when it changed: that would put PLAYER 2 in the profile screen). While BF1GameExt says profile 2 is the
-- temporary one ("BFXQ: temp2"), a SaveProfile for it finds it not changed and goes on as the game does then.
-- ---------------------------------------------------------------------------------------------------------------------
if (ifs_saveop and ifs_saveop.Enter and (not ifs_saveop.bfxsave)) then
ifs_saveop.bfxsave = 1
ifs_saveop.bfxOrigEnter = ifs_saveop.Enter -- (one per table: the game may make two ifs_saveop)

ifs_saveop.Enter = function(this, bFwd)
	if (bFwd and gBFX_TempProfiles and (this.doOp == "SaveProfile") and (this.saveProfileNum == 2) and
		(bfxs_Ask("temp2") == 1)) then
		if (ScriptCB_SetProfileNotDirty) then
			ScriptCB_SetProfileNotDirty(2)
		end
		bfxs_Log("ifs_saveop: SaveProfile 2 - profile 2 is player 2's temporary profile: not saved")
	end
	this.bfxOrigEnter(this, bFwd)
end

bfxs_Log(format("ifs_saveop patched (%s, %s): a temporary profile 2 is never saved", gBFX_Phase or "?", gBFX_Screen or "?"))
end
)BFXLUA",
};

// 21h-r2: BF1GameExt_pause34.lua, built in (the file next to the exe wins) - players 3 / 4's real pause menus
// 21h-r3: in pieces (MSVC: a string literal piece holds at most 16380 bytes) - joined in SplitLoad
const char* const kPause34Pieces[] = {
R"BFXLUA(-- BF1GameExt_pause34.lua   (file build p21j-r31)
gBFX_Build_pause34 = "BF1GameExt_pause34.lua (file build p21j-r31)" -- 21j-r31: the build as a string too: a compiled script (the mod
-- tools' ScriptMunge, console.lvl) keeps no comments, and BF1GameExt's log reads its build from this string there. Otherwise
-- p21i-r10 unchanged (the newest: a p21h-r10 copy next to the exe had no AUTO AIM rows and its own stats-screen RESPAWN code)
-- 21i-r10: players 3 / 4's GAME OPTIONS get an AUTO AIM row (ON / OFF, each his own: "aimp get / set K" - BF1GameExt keeps it
-- in AutoAimP3 / P4 and hands it to the aim assist for that player only). "-" when BF1GameExt does not keep it.
-- 21h-r11: r10's RESPAWN hiding for player 1's pause menu over the stats screen moved out of this file - it is needed in
-- every match now (1 / 2-player too) and this script runs only in 3 / 4-player ones; BF1GameExt runs it for ifs_pausemenu
-- itself (controller_scan.cpp). This file is r7's again otherwise.
-- 21h-r7: the gamepad page's orange tabs as player 1's (from his and player 4's page side by side): up 12 (they sat half over the
-- blue window's top edge - now their bottom is on it), the bars narrower (their end pieces are not scaled, so 207 -> 186), the
-- STARFIGHTER tab 19 to the right, the two tabs not shown at 0.6 (a darker orange, as his) instead of 0.4 (nearly see-through).
-- 21h-r6: (1) THEIR BUTTONS CHANGED AS PLAYER 1'S: the gamepad page starts on the pad (LB's label lit orange); the d-pad / stick
-- moves between the labels that can change (LB LT X A B Y RB RT R3, L3 when it is not BF1GameExt's sprint), down from the
-- bottom one to the settings; A opens the list of actions beside it (player 1's list, the shown tab's), A picks ("rp
-- ctlfnset": his own map, kept in BF1GameExt.ini), B closes it. RESET TO DEFAULTS / Y: the shown tab's buttons back to the
-- game's too. ([Splitscreen] RemapP34=0: as r5.) (2) The page as player 1's: one SQUAD COMMANDS line (the d-pad's other
-- three hidden while they are squad commands - the SPRINT label no longer under one), RESET TO DEFAULTS in full, a "B BACK" at
-- player 1's size (the screen's own, twice as big, hidden while a page is up). (3) No RESPAWN while his pause menu is over
-- his spawn screens ("rp down").
-- 21h-r5: PLAYER 1'S PAGES FOR PLAYERS 3 / 4. GAME OPTIONS and CONTROL OPTIONS are copies of player 1's pages (from r4's scan of
-- ifs_opt_general / ifs_opt_controller), built in their own screens at player 1's size on screen (gBFXR_PopScale):
-- GAME OPTIONS - the title, VIEWPOINT (3RD PERSON - their cameras are third person only), DIFFICULTY (the game's, as player 1's
-- page), RESET TO DEFAULTS (difficulty back to MEDIUM). CONTROL OPTIONS goes straight to the gamepad page - the blue window,
-- the INFANTRY / VEHICLE / STARFIGHTER tabs (LB / RB), the pad picture with every button's action from his own button map
-- ("rp ctlfn"), JOYSTICK SENSITIVITY with its slider (his own sensitivity), VIBRATION (NO - players 3 / 4's pads have no
-- rumble), INVERT LOOK / INVERT FLYING (by tab, his own), RESET TO DEFAULTS. Up / down picks, left / right (and A) change, B
-- back. gBFXR_Pages = 0 ([Splitscreen] OptionsPagesP34=0): r4's lists instead.
-- 21h-r4: (1) Players 3 / 4's YES / NO popup window at player 1's size: player 1's popup is drawn in the interface root (1024
-- x 624, 90% safe area) squeezed into his quarter, theirs in their quarter's root (512 x 312, 88%) - so theirs came out about
-- twice as big. Now it is built at gBFXR_PopScale (PopupScaleP34 / 100, 0.51 = the two roots' ratio): its window, frame
-- corners, question text (text scale), YES / NO and their A / B icons. (2) Player 2's OPTIONS in 3 / 4-player matches
-- (gBFXR_P2Opts, [Splitscreen] OptionsP2): no VIDEO OPTIONS, and CONTROL OPTIONS opens the gamepad page (ifs_opt_controller)
-- at once - no keyboard page. Player 1's OPTIONS unchanged. Players 3 / 4's GAME OPTIONS / CONTROL OPTIONS pages as r3 (the
-- copies of player 1's pages come next, from this round's scan).
-- 21h-r3: PLAYER 1'S SCHEME. OPTIONS opens an OPTIONS list with GAME OPTIONS (DIFFICULTY, the game's own setting, as player
-- 1's GAME OPTIONS page has it) and CONTROL OPTIONS (his invert look / invert flying / sensitivity); B goes back one step, as
-- player 1's pages do. RESPAWN / RESTART MISSION / QUIT ask in a popup window like player 1's (the game's Popup_YesNo look:
-- its frame, its question text, YES / NO with the A / B icons; A = yes, B = no), built inside his own screen so it is drawn in
-- his quarter. Player 2's pause menu gets player 1's accept / back / cancel sounds (gBFXR_P2Sounds, [Splitscreen]
-- PauseSoundsP2; only in 3 / 4-player matches - this script does not run in 2-player ones).
-- 21h-r2: REAL PAUSE MENUS FOR PLAYERS 3 AND 4 - the game's own kind of screen, made by the game's own script functions:
-- ifs_pausemenu3 (viewport 2: player 3's quarter / half) and ifs_pausemenu4 (viewport 3: player 4's quarter), built the way
-- ifs_pausemenu2 is (NewIFShellScreen2, AddVerticalButtons with the split screen layout, the same container place: 0.6 / 0.5,
-- y 20) right after the game has made ifs_pausemenu2. BF1GameExt runs them on two IF managers of its own (players 3 / 4's
-- pads), opens them on Start and does what they ask ("BFXQ: rp ..."): RESUME GAME / OPTIONS / RESPAWN / RESTART MISSION / QUIT.
-- Each screen keeps its own highlighted item (never the game's gCurHiliteButton / gCurHiliteButton2).
-- Run by BF1GameExt with the game's Lua while the game makes ifs_pausemenu2 (the copy built into the DLL, or this file next
-- to the exe), with gBFXR_Players (this match's player count) and gBFXR_P2Sounds set first. Lua 4.0: no upvalues, every
-- helper a global bfxr_*. Its log lines come in as ScriptCB_GetPlatform("BFXR: ...") - [rp34] lua lines in
-- BF1GameExt_split.log.

function bfxr_Log(s)
	ScriptCB_GetPlatform("BFXR: " .. s)
end

-- a question to BF1GameExt: a number back (-1 if nothing answered)
function bfxr_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

-- the items (one layout for both screens: AddVerticalButtons fills it the same way each time). yes / no: only if the popup
-- window could not be made (the old YES / NO list)
bfxr_layout = {
	ySpacing = 5,
	width = 220,
	font = "gamefont_small",
	buttonlist = {
		{ tag = "resume", string = "common.resume", },
		{ tag = "opts", string = "ifs.main.options", },
		{ tag = "suicide", string = "game.pause.suicide", },
		{ tag = "restart", string = "common.restart", },
		{ tag = "quit", string = "common.quit", },
		{ tag = "gameopts", string = "ifs.GameOpt.title", },
		{ tag = "ctlopts", string = "ifs.GameOpt.pc_optionstitle", },
		{ tag = "diff", string = "common.accept", },
		{ tag = "invert", string = "common.accept", },
		{ tag = "fly", string = "common.accept", },
		{ tag = "sens", string = "common.accept", },
		{ tag = "yes", string = "common.yes", },
		{ tag = "no", string = "common.no", },
	},
	title = "game.pause.title",
}

function bfxr_Text(obj, s)
	if (obj) then
		IFText_fnSetUString(obj, ScriptCB_tounicode(s))
	end
end

-- the highlighted item's pulse, kept per screen (the game's globals put back after)
function bfxr_Hilite(this, dt)
	if (not this.bfxrHi) then
		return
	end
	local a = gButton_CurSizeAdd
	local d = gButton_CurDir
	gButton_CurSizeAdd = this.bfxrSz or 0.5
	gButton_CurDir = this.bfxrDir or 2
	IFButton_fnHilight(this.bfxrHi, 1, dt)
	this.bfxrSz = gButton_CurSizeAdd
	this.bfxrDir = gButton_CurDir
	gButton_CurSizeAdd = a
	gButton_CurDir = d
end

function bfxr_Select(this, tag)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = nil
	this.CurButton = tag
	if (tag) then
		this.bfxrHi = this.buttons[tag]
	end
	if (this.bfxrHi) then
		IFButton_fnSelect(this.bfxrHi, 1)
		bfxr_Hilite(this, 0)
	end
end

function bfxr_OnOff(v)
	if (v == 1) then
		return "ON"
	elseif (v == 0) then
		return "OFF"
	end
	return "?"
end

-- CONTROL OPTIONS: the three items, with their values
function bfxr_OptsLabels(this)
	local k = this.bfxrPlayer
	local inv = bfxr_Ask(format("rp ctlget %d 0", k))
	local fly = bfxr_Ask(format("rp ctlget %d 1", k))
	local sens = bfxr_Ask(format("rp ctlget %d 2", k))
	bfxr_Text(this.buttons.invert.label, "INVERT LOOK   " .. bfxr_OnOff(inv))
	bfxr_Text(this.buttons.fly.label, "INVERT FLYING   " .. bfxr_OnOff(fly))
	bfxr_Text(this.buttons.sens.label, format("SENSITIVITY   < %d >", sens))
end

-- GAME OPTIONS: the game's difficulty (ScriptCB_Get / SetDifficulty, as player 1's GAME OPTIONS page) - only while the game's
-- pausing viewport is player 1's / 2's (the profile it reads)
function bfxr_DiffOk()
	local v = ScriptCB_GetPausingViewport()
	return (type(v) ~= "number") or (v <= 1)
end

function bfxr_DiffLabel(this)
	local s = "-"
	if (bfxr_DiffOk()) then
		local d = ScriptCB_GetDifficulty()
		if (d == 1) then
			s = "EASY"
		elseif (d == 2) then
			s = "MEDIUM"
		else
			s = "HARD"
		end
	end
	bfxr_Text(this.buttons.diff.label, "DIFFICULTY   < " .. s .. " >")
end

function bfxr_DiffChange(this, step)
	if (not bfxr_DiffOk()) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		return
	end
	local d = ScriptCB_GetDifficulty()
	if (step > 0) then
		d = mod(d, 3) + 1
	else
		d = mod(d + 1, 3) + 1
	end
	ScriptCB_SetDifficulty(d)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfxr_DiffLabel(this)
	bfxr_Hilite(this, 0)
	bfxr_Log(format("player %d: GAME OPTIONS - difficulty now %d", this.bfxrPlayer, ScriptCB_GetDifficulty()))
end

-- the popup window (player 1's Popup_YesNo look) on / off; the pause items hidden while it is up, as player 2's are
function bfxr_PopVis(this, on)
	if (this.bfxrPop) then
		IFObj_fnSetVis(this.bfxrPop, on)
	end
	IFObj_fnSetVis(this.buttons, not on)
end

function bfxr_PopAsk(this, what)
	local p = this.bfxrPop
	local key = "game.pause.suicide_prompt"
	if (what == "restart") then
		key = "ifs.pause.warn_restart"
	elseif (what == "quit") then
		if (ScriptCB_AreMetagameRulesOn()) then
			key = "ifs.pause.warn_quit_meta"
		else
			key = "ifs.pause.warn_quit"
		end
	end
	IFText_fnSetString(p.title, key)
	IFButton_fnSelect(p.buttons.yes, nil)
	IFButton_fnSelect(p.buttons.no, 1)
	p.CurButton = "no"
	bfxr_PopVis(this, 1)
end

-- which items are shown: "main" (the pause items), "optsmenu" (OPTIONS: GAME OPTIONS / CONTROL OPTIONS), "game" (GAME
-- OPTIONS), "opts" (CONTROL OPTIONS), "confirm" (the popup - or YES / NO in the list if there is no popup)
function bfxr_Show(this, mode, cur)
	local b = this.buttons
	local i = 1
	while (bfxr_layout.buttonlist[i]) do
		b[bfxr_layout.buttonlist[i].tag].hidden = 1
		i = i + 1
	end
	this.bfxrMode = mode
	if (this.bfxrPBg) then
		bfxr_PagesVis(this, nil, nil)
	end
	if ((mode == "confirm") and this.bfxrPop) then
		if (this.bfxrHi) then
			IFButton_fnHilight(this.bfxrHi, nil, 0)
			IFButton_fnSelect(this.bfxrHi, nil)
		end
		this.bfxrHi = nil
		bfxr_PopAsk(this, this.bfxrWhat)
		return
	end
	bfxr_PopVis(this, nil)
	if (mode == "main") then
		b.resume.hidden = nil
		b.opts.hidden = nil
		-- 21h-r6: no RESPAWN while his pause menu is over his spawn screens (he has no soldier)
		if (bfxr_Ask(format("rp down %d", this.bfxrPlayer)) ~= 1) then
			b.suicide.hidden = nil
		end
		b.quit.hidden = nil
		if (not ScriptCB_AreMetagameRulesOn()) then
			b.restart.hidden = nil
		end
		IFText_fnSetString(b._titlebar_, "game.pause.title")
	elseif (mode == "optsmenu") then
		b.gameopts.hidden = nil
		b.ctlopts.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.Main.options")
	elseif (mode == "game") then
		b.diff.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.GameOpt.title")
		bfxr_DiffLabel(this)
	elseif (mode == "opts") then
		b.invert.hidden = nil
		b.fly.hidden = nil
		b.sens.hidden = nil
		IFText_fnSetString(b._titlebar_, "ifs.GameOpt.pc_optionstitle")
		bfxr_OptsLabels(this)
	else
		b.yes.hidden = nil
		b.no.hidden = nil
)BFXLUA",
R"BFXLUA(		if (this.bfxrWhat == "suicide") then
			bfxr_Text(b._titlebar_, "RESPAWN?")
		elseif (this.bfxrWhat == "restart") then
			bfxr_Text(b._titlebar_, "RESTART MISSION?")
		else
			bfxr_Text(b._titlebar_, "QUIT?")
		end
	end
	local first = ShowHideVerticalButtons(b, bfxr_layout)
	if ((not cur) or b[cur].hidden) then
		cur = first
	end
	bfxr_Select(this, cur)
end

-- asks BF1GameExt to do it (it closes the screen); nothing more from this screen until it is up again
function bfxr_Do(this, what)
	this.bfxrBusy = 1
	local r = bfxr_Ask(format("rp %s %d", what, this.bfxrPlayer))
	bfxr_Log(format("player %d: %s - asked BF1GameExt (%d)", this.bfxrPlayer, what, r))
end

function bfxr_Enter(this, bFwd)
	this.bfxrBusy = nil
	this.bfxrSz = 0.5
	this.bfxrDir = 2
	this.bfxrFlash = random() * 15 + 10
	this.bfxrWhat = nil
	bfxr_Show(this, "main", "resume")
	IFFlashyText_StartFlashiness(this)
	if (bFwd) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
	end
	bfxr_Log(format("player %d: %s up (Enter %s): %s, popup %s", this.bfxrPlayer, tostring(this.ScreenName), tostring(bFwd),
		tostring(this.CurButton), this.bfxrPop and "made" or "none"))
end

function bfxr_Leave(this, bFwd)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = nil
	if ((this.bfxrMode == "opts") or (this.bfxrMode == "ppage")) then
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
	end
	if (this.bfxrPop) then
		IFObj_fnSetVis(this.bfxrPop, nil)
	end
	if (this.bfxrPBg) then
		bfxr_PagesVis(this, nil, nil)
	end
	this.bfxrMode = nil
end

function bfxr_Update(this, fDt)
	bfxr_Hilite(this, fDt)
	this.bfxrFlash = (this.bfxrFlash or 10) - fDt
	if (this.bfxrFlash <= 0) then
		IFFlashyText_DoRandomFlashiness(this)
		this.bfxrFlash = random() * 15 + 10
	end
end

function bfxr_Move(this, dir)
	if (this.bfxrBusy or (this.bfxrMode == "confirm" and this.bfxrPop)) then
		return
	end
	if ((this.bfxrMode == "gpage") or (this.bfxrMode == "ppage")) then
		if (dir == "link_up") then
			bfxr_PageMove(this, -1)
		else
			bfxr_PageMove(this, 1)
		end
		return
	end
	this.CurButton = FollowButtonLink(this.buttons, this.CurButton, dir)
	if (this.CurButton) then
		if (this.bfxrHi ~= this.buttons[this.CurButton]) then
			if (this.bfxrHi) then
				IFButton_fnHilight(this.bfxrHi, nil, 0)
			end
			this.bfxrHi = this.buttons[this.CurButton]
			bfxr_Hilite(this, 0)
		end
	end
end

function bfxr_Up(this)
	bfxr_Move(this, "link_up")
end

function bfxr_Down(this)
	bfxr_Move(this, "link_down")
end

-- CONTROL OPTIONS: left / right (and A) change the item picked
function bfxr_OptChange(this, step)
	local k = this.bfxrPlayer
	local c = this.CurButton
	local i = 0
	if (c == "fly") then
		i = 1
	elseif (c == "sens") then
		i = 2
	end
	local v = bfxr_Ask(format("rp ctlget %d %d", k, i))
	if (i == 2) then
		v = v + step
		if (v < 1) then
			v = 1
		elseif (v > 10) then
			v = 10
		end
	else
		v = 1 - v
	end
	bfxr_Ask(format("rp ctlset %d %d %d", k, i, v))
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_OptsLabels(this)
	bfxr_Hilite(this, 0)
end

-- the popup's YES / NO highlight (left / right), as the game's buttons are selected
function bfxr_PopToggle(this)
	local p = this.bfxrPop
	if (p.CurButton == "yes") then
		p.CurButton = "no"
	else
		p.CurButton = "yes"
	end
	IFButton_fnSelect(p.buttons.yes, p.CurButton == "yes")
	IFButton_fnSelect(p.buttons.no, p.CurButton == "no")
	ScriptCB_SndPlaySound("shell_select_change")
end

function bfxr_Side(this, step)
	if (this.bfxrBusy) then
		return
	end
	if ((this.bfxrMode == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		return
	elseif ((this.bfxrMode == "ppage") and this.bfxrPadKey) then
		if (step < 0) then
			bfxr_PadMove(this, "left")
		else
			bfxr_PadMove(this, "right")
		end
	elseif ((this.bfxrMode == "gpage") or (this.bfxrMode == "ppage")) then
		bfxr_PageChange(this, step)
	elseif (this.bfxrMode == "opts") then
		bfxr_OptChange(this, step)
	elseif (this.bfxrMode == "game") then
		bfxr_DiffChange(this, step)
	elseif ((this.bfxrMode == "confirm") and this.bfxrPop) then
		bfxr_PopToggle(this)
	end
end

function bfxr_Left(this)
	bfxr_Side(this, -1)
end

function bfxr_Right(this)
	bfxr_Side(this, 1)
end

-- the popup's answer (A = yes, B = no - player 1's popup on a pad)
function bfxr_Answer(this, yes)
	ifelm_shellscreen_fnPlaySound("shell_menu_accept")
	if (yes) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Do(this, this.bfxrWhat)
	else
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
		bfxr_Show(this, "main", this.bfxrWhat)
	end
end

function bfxr_Accept(this)
	if (this.bfxrBusy) then
		return
	end
	local c = this.CurButton
	local m = this.bfxrMode
	if ((m == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListPick(this)
		return
	elseif ((m == "ppage") and this.bfxrPadKey) then
		bfxr_ListOpen(this)
		return
	elseif ((m == "gpage") or (m == "ppage")) then
		bfxr_PageChange(this, 1)
		return
	elseif (m == "opts") then
		bfxr_OptChange(this, 1)
		return
	elseif (m == "game") then
		bfxr_DiffChange(this, 1)
		return
	elseif (m == "confirm") then
		if (this.bfxrPop) then
			bfxr_Answer(this, 1)
		else
			bfxr_Answer(this, c == "yes")
		end
		return
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	if (m == "optsmenu") then
		if (c == "gameopts") then
			if (this.bfxrGItems) then
				bfxr_ShowPage(this, "gpage", 1)
			else
				bfxr_Show(this, "game", "diff")
			end
		elseif (c == "ctlopts") then
			if (bfxr_Ask(format("rp ctllive %d", this.bfxrPlayer)) == 1) then
				if (this.bfxrPBg) then
					this.bfxrTab = 0
					bfxr_ShowPage(this, "ppage", 1)
				else
					bfxr_Show(this, "opts", "invert")
				end
			else
				ifelm_shellscreen_fnPlaySound(this.errorSound)
				bfxr_Log(format("player %d: CONTROL OPTIONS - his controls cannot be changed now (pads 3 / 4 off?)", this.bfxrPlayer))
			end
		end
		return
	end
	if (c == "resume") then
		bfxr_Do(this, "resume")
	elseif (c == "opts") then
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif ((c == "suicide") or (c == "restart") or (c == "quit")) then
		this.bfxrWhat = c
		bfxr_Show(this, "confirm", "no")
	end
end

function bfxr_Back(this)
	if (this.bfxrBusy) then
		return
	end
	local m = this.bfxrMode
	if (m == "confirm") then
		bfxr_Answer(this, nil)
	elseif ((m == "ppage") and gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListClose(this)
		ifelm_shellscreen_fnPlaySound(this.cancelSound)
	elseif (m == "ppage") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
		bfxr_Show(this, "optsmenu", "ctlopts")
	elseif (m == "gpage") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif (m == "opts") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
		bfxr_Show(this, "optsmenu", "ctlopts")
	elseif (m == "game") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "optsmenu", "gameopts")
	elseif (m == "optsmenu") then
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Show(this, "main", "opts")
	else
		ifelm_shellscreen_fnPlaySound(this.exitSound)
		bfxr_Do(this, "resume")
	end
end

function bfxr_Start(this)
	if (this.bfxrBusy) then
		return
	end
	if ((this.bfxrMode == "opts") or (this.bfxrMode == "ppage")) then
		bfxr_Ask(format("rp ctlsave %d", this.bfxrPlayer))
	end
	ifelm_shellscreen_fnPlaySound(this.exitSound)
	bfxr_Do(this, "resume")
end

-- player 1's Popup_YesNo, rebuilt as a child of his screen (so it is drawn in his quarter): the game's button window (its
-- frame), the question text, YES / NO with the A / B icons. 21h-r4: built at scale f (player 1's popup size on screen: his is
-- drawn in the interface root, 1024 units wide in a quarter; ours in the quarter's own root, 512 units). nil if the game's
-- helpers are not there.
function bfxr_SkinSize(skin, w, h, c)
	local x1 = w * -0.5
	local x2 = x1 + c
	local x4 = w * 0.5
	local x3 = x4 - c
	local y1 = h * -0.5
	local y2 = y1 + c
	local y4 = h * 0.5
	local y3 = y4 - c
	if (y3 < y2) then
		y2 = 0
		y3 = 0
	end
	IFImage_fnSetTexturePos(skin.ChunkTL, x1, y1, x2, y2)
	IFImage_fnSetTexturePos(skin.ChunkTC, x2, y1, x3, y2)
	IFImage_fnSetTexturePos(skin.ChunkTR, x3, y1, x4, y2)
	IFImage_fnSetTexturePos(skin.ChunkCL, x1, y2, x2, y3)
	IFImage_fnSetTexturePos(skin.ChunkC, x2, y2, x3, y3)
	IFImage_fnSetTexturePos(skin.ChunkCR, x3, y2, x4, y3)
	IFImage_fnSetTexturePos(skin.ChunkBL, x1, y3, x2, y4)
	IFImage_fnSetTexturePos(skin.ChunkBC, x2, y3, x3, y4)
	IFImage_fnSetTexturePos(skin.ChunkBR, x3, y3, x4, y4)
end

function bfxr_NewPop()
	if ((not NewButtonWindow) or (not AddHorizontalButtons) or (not Horiz_YesNoButtons_layout)) then
		return nil
	end
	local f = (gBFXR_PopScale or 51) / 100
	if ((f < 0.2) or (f > 1.5)) then
		f = 0.51
	end
	local w = 300 * f
	local h = 180 * f
	local p = NewButtonWindow {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0.5,
		height = h,
		width = w,
		ZPos = 50,
		title = NewIFText {
			font = "gamefont_medium",
			textw = 260 * f,
			texth = 120 * f,
			y = -80 * f,
			flashy = 0,
			HScale = f,
			VScale = f,
		},
		buttons = NewIFContainer {
			y = 60 * f,
		},
	}
	if (p.skin and p.skin.ChunkTL) then
		bfxr_SkinSize(p.skin, w, h, 16 * f)
	end
	-- the game's YES / NO layout at scale f (its own table is left as it is)
	local L = Horiz_YesNoButtons_layout
	local lay = {
		xLeft = (L.xLeft or -40) * f,
		yTop = (L.yTop or 0) * f,
		xWidth = (L.xWidth or 80) * f,
		xSpacing = (L.xSpacing or 5) * f,
		height = (L.height or 45) * f,
		font = L.font or "gamefont_medium",
		flashy = L.flashy,
		nocreatebackground = L.nocreatebackground,
		buttonlist = {},
	}
	local i = 1
	while (L.buttonlist[i]) do
		lay.buttonlist[i] = { tag = L.buttonlist[i].tag, string = L.buttonlist[i].string, gamepad = L.buttonlist[i].gamepad, }
		i = i + 1
	end
	AddHorizontalButtons(p.buttons, lay)
	i = 1
	while (lay.buttonlist[i]) do
		local b = p.buttons[lay.buttonlist[i].tag]
		if (b and b.label) then
			b.label.HScale = f
			b.label.VScale = f
		end
		i = i + 1
	end
	if (AddGamepadIcons and NewIFGamepadImage and gGamepadTexture) then
		AddGamepadIcons(p.buttons, lay, 1)
		i = 1
		while (lay.buttonlist[i]) do
			local ic = p.buttons[lay.buttonlist[i].tag .. "Icon"]
			if (ic and ic.localpos_l) then
				ic.localpos_l = ic.localpos_l * f
				ic.localpos_t = ic.localpos_t * f
				ic.localpos_r = ic.localpos_r * f
				ic.localpos_b = ic.localpos_b * f
			end
			i = i + 1
		end
	end
	p.bfxrScale = f
	return p
end

-- 21h-r5: PLAYER 1'S GAME OPTIONS AND GAMEPAD PAGES, copied for his screen. Built with the game's own element functions and
-- the numbers of player 1's pages (as the game builds them for the interface root, 1024 x 624, safe area 921.6 x 561.6),
-- then scaled to player 1's size on screen (f): offsets and sizes times f, text scaled by f; what the game places from the
-- safe area's corner placed at the same fraction of his quarter. The page elements are children of his screen, hidden
-- unless their page is up (their names: gBFXR_GNames / gBFXR_PNames).
gBFXR_GNames = { "bfxrGTitle", "bfxrGLabels", "bfxrGItems", }
gBFXR_PNames = { "bfxrPBg", "bfxrPTitle", "bfxrPTab0", "bfxrPTab1", "bfxrPTab2", "bfxrPLb", "bfxrPRb", "bfxrPPad", "bfxrPLines",
	"bfxrPLabels", "bfxrPVals", "bfxrPSlider", "bfxrPReset", }
gBFXR_GItems = { "persp", "diff", "autoaim", "reset", } -- 21i-r10: AUTO AIM
gBFXR_PItems = { "joysens", "invert", "reset", }
gBFXR_TabTitle = { [0] = "ifs.Controls.Soldier.title", [1] = "ifs.Controls.Vehicle.title", [2] = "ifs.Controls.Flyer.title", }
-- 21h-r6: the action list per player (globals: a screen table's tables become elements)
gBFXR_List = {}
gBFXR_ListOpen = {}
gBFXR_ListLogged = {}

function bfxr_Scale(t, f, seen)
	if (seen[t]) then
		return
	end
	seen[t] = 1
	if (t.x) then
		t.x = t.x * f
	end
	if (t.y) then
		t.y = t.y * f
	end
	if (t.textw) then
		t.textw = t.textw * f
	end
	if (t.texth) then
		t.texth = t.texth * f
	end
)BFXLUA",
R"BFXLUA(	if (t.localpos_l) then
		t.localpos_l = t.localpos_l * f
		t.localpos_t = t.localpos_t * f
		t.localpos_r = t.localpos_r * f
		t.localpos_b = t.localpos_b * f
	end
	if (t.type == "text") then
		t.HScale = (t.HScale or 1) * f
		t.VScale = (t.VScale or 1) * f
		if (t.bg_width) then
			t.bg_width = t.bg_width * f
		end
		if (t.bgoffsetx) then
			t.bgoffsetx = t.bgoffsetx * f
		end
		if (t.bgoffsety) then
			t.bgoffsety = t.bgoffsety * f
		end
		if (t.leading) then
			t.leading = t.leading * f
		end
	end
	local kids = {}
	local k, v
	for k, v in t do
		if (type(v) == "table") then
			tinsert(kids, v)
		end
	end
	local i = 1
	while (kids[i]) do
		bfxr_Scale(kids[i], f, seen)
		i = i + 1
	end
end

-- a value item as the game's option pages make them (a round button, text only)
function bfxr_Item(x, y, w, h, font, halign)
	local b = NewRoundIFButton {
		x = x,
		y = y,
		btnw = w,
		btnh = h,
		font = font,
		nocreatebackground = 1,
		flashy = 0,
	}
	if (b.label) then
		b.label.halign = halign
		b.label.flashy = 0
	end
	return b
end

function bfxr_Label(x, y, w, h, font, halign, str)
	return NewIFText {
		x = x,
		y = y,
		textw = w,
		texth = h,
		font = font,
		halign = halign,
		valign = "vcenter",
		string = str,
		style = "glow",
		flashy = 0,
		nocreatebackground = 1,
	}
end

-- every game helper the two pages are built with (nil: r4's lists instead)
function bfxr_PagesOk()
	return NewRoundIFButton and NewIFText and NewIFContainer and NewButtonWindow and NewIFImage and NewSegmentLine and
		Platform_btn_map and IFObj_fnSetAlpha and IFImage_fnSetTexturePos
end

-- the names of the missing ones, for the log
function bfxr_PagesMissing()
	local s = ""
	if (not NewRoundIFButton) then s = s .. " NewRoundIFButton" end
	if (not NewIFText) then s = s .. " NewIFText" end
	if (not NewIFContainer) then s = s .. " NewIFContainer" end
	if (not NewButtonWindow) then s = s .. " NewButtonWindow" end
	if (not NewIFImage) then s = s .. " NewIFImage" end
	if (not NewSegmentLine) then s = s .. " NewSegmentLine" end
	if (not Platform_btn_map) then s = s .. " Platform_btn_map" end
	if (not IFObj_fnSetAlpha) then s = s .. " IFObj_fnSetAlpha" end
	if (not IFImage_fnSetTexturePos) then s = s .. " IFImage_fnSetTexturePos" end
	return s
end

-- GAME OPTIONS (ifs_opt_general: rows at y 150 / 190 / 230 from the top, labels right-aligned left of the middle, values left-
-- aligned right of it)
function bfxr_NewGamePage(t, f)
	t.bfxrGTitle = NewIFText {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0,
		x = -230,
		y = 10,
		textw = 460,
		texth = 30,
		font = "gamefont_large",
		halign = "hcenter",
		valign = "top",
		string = "ifs.GameOpt.title",
		style = "glow",
		flashy = 1,
		nocreatebackground = 1,
		bg_width = 0,
		bg_tail = 0,
		startdelay = 0,
	}
	t.bfxrGLabels = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0, x = -365, y = 0, }
	t.bfxrGItems = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0, x = 15, y = 0, }
	local titles = { "ifs.gameopt.camera", "ifs.gameopt.diff", "ifs.gameopt.autoaim", "", }
	local i = 1
	while (gBFXR_GItems[i]) do
		local row = 150 + (i - 1) * 40
		local tag = gBFXR_GItems[i]
		t.bfxrGLabels[tag] = bfxr_Label(0, row - 17.5, 350, 35, "gamefont_medium", "right", titles[i])
		local b = bfxr_Item(0, row, 350, 35, "gamefont_medium", "left")
		if (b.label) then
			b.label.x = 0
		end
		t.bfxrGItems[tag] = b
		i = i + 1
	end
	bfxr_Scale(t.bfxrGTitle, f, {})
	bfxr_Scale(t.bfxrGLabels, f, {})
	bfxr_Scale(t.bfxrGItems, f, {})
end

-- the gamepad page (ifs_opt_controller as BF1GameExt's controller page script has it): its window, title, tabs, the pad and its
-- lines, the settings. nil if the game's helpers or data are not there.
function bfxr_NewPadPage(t, f)
	if ((not Platform_btn_map) or (not NewSegmentLine) or (not NewButtonWindow) or (not NewIFImage)) then
		return nil
	end
	local sw = 921.6
	local sh = 561.6
	t.bfxrPBg = NewButtonWindow {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 338.88 / sh,
		ZPos = 255,
		width = 967.68,
		height = 492.224,
	}
	t.bfxrPTitle = NewIFText {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0,
		x = -230,
		y = 0,
		textw = 460,
		texth = 30,
		font = "gamefont_large",
		halign = "hcenter",
		valign = "top",
		string = gBFXR_TabTitle[0],
		style = "glow",
		flashy = 1,
		nocreatebackground = 1,
		bg_width = 0,
		bg_tail = 0,
		startdelay = 0,
	}
	local tabs = { [0] = "ifs.Controls.SoldierTab", [1] = "ifs.Controls.VehicleTab", [2] = "ifs.Controls.FlyerTab", }
	local tx = { [0] = 0, [1] = 1 - 645.12 / sw, [2] = 1 - 303 / sw, } -- 21h-r7: STARFIGHTER 19 right
	local i = 0
	while (i <= 2) do
		t["bfxrPTab" .. i] = NewIFText {
			ScreenRelativeX = tx[i],
			ScreenRelativeY = (70.768 - 12) / sh, -- 21h-r7: up 12 (on the window's top edge, as player 1's)
			x = 0,
			y = 0,
			ZPos = 255,
			textw = 230.4,
			texth = 40,
			font = "gamefont_small",
			halign = "hcenter",
			valign = "top",
			string = tabs[i],
			style = "glow",
			flashy = 1,
			bgleft = "headerbuttonleft",
			bgmid = "headerbuttonmid",
			bgright = "headerbuttonright",
			bg_width = 186, -- 21h-r7: (207.36: its end pieces are not scaled - the bar came out wider than his)
			bg_tail = 0,
			startdelay = 0,
		}
		i = i + 1
	end
	if (NewIFGamepadImage and gGamepadTexture) then
		t.bfxrPLb = NewIFGamepadImage("LB", 0, 0)
		t.bfxrPRb = NewIFGamepadImage("RB", 0, 0)
		t.bfxrPLb.inert = nil
		t.bfxrPRb.inert = nil
		t.bfxrPLb.ScreenRelativeX = 0
		t.bfxrPLb.ScreenRelativeY = (80.768 - 12) / sh
		t.bfxrPRb.ScreenRelativeX = 1
		t.bfxrPRb.ScreenRelativeY = (80.768 - 12) / sh
	end
	t.bfxrPPad = NewIFImage {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0.55,
		ZPos = 240,
		texture = "gamepad_xbox360",
		localpos_l = -128,
		localpos_t = -80.64,
		localpos_r = 128,
		localpos_b = 80.64,
		uvs_l = 0,
		uvs_t = 0,
		uvs_r = 1,
		uvs_b = 0.639,
	}
	t.bfxrPLines = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0.55, }
	local k, v
	for k, v in Platform_btn_map do
		if ((type(v) == "table") and (v.type ~= 5) and (v.type ~= 99)) then
			t.bfxrPLines[k] = NewSegmentLine(v.lx1, v.ly1, v.lx2, v.ly2, "")
		end
	end
	t.bfxrPLabels = NewIFContainer { ScreenRelativeX = 0.2, ScreenRelativeY = 0.85, }
	t.bfxrPLabels.joysens = bfxr_Label(0, -10, 260, 20, "gamefont_tiny", "left", "ifs.GameOpt.pc_joysens")
	t.bfxrPLabels.rumble = bfxr_Label(0, 15, 260, 20, "gamefont_tiny", "left", "ifs.gameopt.rumble")
	t.bfxrPLabels.invert = bfxr_Label(0, 40, 260, 20, "gamefont_tiny", "left", "")
	t.bfxrPVals = NewIFContainer { ScreenRelativeX = 0.8, ScreenRelativeY = 0.85, }
	t.bfxrPVals.joysens = bfxr_Item(-130, 0, 260, 20, "gamefont_tiny", "right")
	t.bfxrPVals.rumble = bfxr_Item(-130, 25, 260, 20, "gamefont_tiny", "right")
	t.bfxrPVals.invert = bfxr_Item(-130, 50, 260, 20, "gamefont_tiny", "right")
	t.bfxrPSlider = NewIFContainer { ScreenRelativeX = 0.2, ScreenRelativeY = 0.85, x = 368.64, y = 3, }
	t.bfxrPSlider.bg = NewIFImage { ZPos = 160, texture = "slider_bg", localpos_l = -115.2, localpos_t = -12, localpos_r = 138.24,
		localpos_b = 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	t.bfxrPSlider.fg = NewIFImage { ZPos = 159, texture = "slider_fg", localpos_l = -115.2, localpos_t = -12, localpos_r = -92.16,
		localpos_b = 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	t.bfxrPReset = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 1, x = 0, y = -15, }
	t.bfxrPReset.btn = bfxr_Item(0, 0, 200, 25, "gamefont_medium", "left")
	if (t.bfxrPReset.btn.label) then
		t.bfxrPReset.btn.label.string = "common.reset"
		t.bfxrPReset.btn.label.textw = 400 -- 21h-r6: RESET TO DEFAULTS in full
	end
	if (NewIFGamepadImage and gGamepadTexture) then
		t.bfxrPReset.icon = NewIFGamepadImage("Y", -117.5, 0)
		t.bfxrPReset.icon.inert = nil
	end
	-- 21h-r6: the action list (player 1's listbox: 215 x 230 at the pad's centre + 40, five rows of 40, beside the label)
	t.bfxrPList = NewIFContainer { ScreenRelativeX = 0.5, ScreenRelativeY = 0.55, x = 120, y = 40, ZPos = 100, }
	t.bfxrPList.win = NewButtonWindow { ZPos = 200, x = 0, y = 0, width = 215, height = 230, }
	i = 1
	while (i <= 5) do
		t.bfxrPList["row" .. i] = bfxr_Label(-90, -99 + (i - 1) * 40, 148, 40, "gamefont_tiny", "left", "")
		i = i + 1
	end
	-- 21h-r6: the page's "B BACK" at player 1's size (the screen's own is his quarter root's size - twice as big)
	t.bfxrBack = NewIFContainer { ScreenRelativeX = 0, ScreenRelativeY = 1, x = 0, y = -15, }
	t.bfxrBack.icon = NewIFImage { ZPos = 100, texture = "gamepad_xbox360", localpos_l = -15, localpos_t = -12, localpos_r = 15,
		localpos_b = 18, uvs_l = 0.125, uvs_t = 0.639, uvs_r = 0.25, uvs_b = 0.764, }
	t.bfxrBack.label = bfxr_Label(24, -12.5, 200, 25, "gamefont_medium", "left", "common.back")
	i = 1
	while (gBFXR_PNames[i]) do
		if (t[gBFXR_PNames[i]]) then
			bfxr_Scale(t[gBFXR_PNames[i]], f, {})
		end
		i = i + 1
	end
	bfxr_Scale(t.bfxrPList, f, {})
	bfxr_Scale(t.bfxrBack, f, {})
	return 1
end

-- his pages' elements shown / hidden (g: GAME OPTIONS, p: the gamepad page). 21h-r6: the screen's own big "B BACK" (drawn at
-- his quarter's root size) hidden while a page is up and the page's small one shown (player 1's size); the action list closed
function bfxr_PagesVis(this, g, p)
	local i = 1
	while (gBFXR_GNames[i]) do
		if (this[gBFXR_GNames[i]]) then
			IFObj_fnSetVis(this[gBFXR_GNames[i]], g)
		end
		i = i + 1
	end
	i = 1
	while (gBFXR_PNames[i]) do
		if (this[gBFXR_PNames[i]]) then
			IFObj_fnSetVis(this[gBFXR_PNames[i]], p)
		end
		i = i + 1
	end
	local any = g or p
	if (this.bfxrBack) then
		IFObj_fnSetVis(this.bfxrBack, any)
	end
	if (this.Helptext_Back) then
		IFObj_fnSetVis(this.Helptext_Back, not any)
	end
	if (this.bfxrPList) then
		IFObj_fnSetVis(this.bfxrPList, nil)
	end
	gBFXR_ListOpen[this.bfxrPlayer] = nil
end

function bfxr_SelectItem(this, item)
	if (this.bfxrHi) then
		IFButton_fnHilight(this.bfxrHi, nil, 0)
		IFButton_fnSelect(this.bfxrHi, nil)
	end
	this.bfxrHi = item
	if (item) then
		IFButton_fnSelect(item, 1)
		bfxr_Hilite(this, 0)
	end
end

function bfxr_PageItem(this)
	local i = this.bfxrPageIdx or 1
	if (this.bfxrMode == "gpage") then
		return this.bfxrGItems[gBFXR_GItems[i]]
	end
	if (gBFXR_PItems[i] == "reset") then
		return this.bfxrPReset.btn
	end
	return this.bfxrPVals[gBFXR_PItems[i]]
end

function bfxr_GameValues(this)
	local it = this.bfxrGItems
	IFText_fnSetString(it.persp.label, "ifs.GameOpt.cam_forward")
	if (bfxr_DiffOk()) then
		local d = ScriptCB_GetDifficulty()
		if (d == 1) then
			IFText_fnSetString(it.diff.label, "ifs.difficulty.easy")
		elseif (d == 2) then
			IFText_fnSetString(it.diff.label, "ifs.difficulty.medium")
		else
			IFText_fnSetString(it.diff.label, "ifs.difficulty.hard")
		end
	else
		bfxr_Text(it.diff.label, "-")
	end
	IFText_fnSetString(it.reset.label, "common.reset")
	-- 21i-r10: his AUTO AIM (the title in English if the game has no text for it)
	if (it.autoaim) then
		local v = bfxr_Ask(format("aimp get %d", this.bfxrPlayer or 3))
		if (v < 0) then
			bfxr_Text(it.autoaim.label, "-")
		elseif (v == 1) then
			IFText_fnSetString(it.autoaim.label, "common.on")
		else
			IFText_fnSetString(it.autoaim.label, "common.off")
		end
		if (this.bfxrGLabels and this.bfxrGLabels.autoaim and not (ScriptCB_getlocalizestr and ScriptCB_getlocalizestr("ifs.gameopt.autoaim", 1))) then
			bfxr_Text(this.bfxrGLabels.autoaim, "AUTO AIM")
		end
	end
end

-- the button k's function on tab m from his own map (the page's number, -1 unknown), and the label's string index (as player
-- 1's page: the d-pad's four squad directions are one SQUAD COMMANDS, the sticks Move / Strafe)
function bfxr_PadFn(this, k, v, m)
	if ((v.name == "lang") or (v.name == "rang")) then
		return 16, 16 + 19 * m
	end
	local fn = bfxr_Ask(format("rp ctlfn %d %d %d", this.bfxrPlayer, m, k))
	local idx = fn
	if ((idx > 10) and (idx < 15)) then
		idx = 10
	end
	if (idx > 0) then
		idx = idx + 19 * m
	end
	return fn, idx
end

function bfxr_FnString(idx)
)BFXLUA",
R"BFXLUA(	local str = (idx > 0) and gButtonFunctionStringsGamePad and gButtonFunctionStringsGamePad[idx]
	if (type(str) == "string") then
		return str
	end
	return "ifs.controls.General.none"
end

-- the buttons he can change (player 1's page's: LB LT X A B Y RB RT R3, L3 when it is not BF1GameExt's sprint) - the sticks,
-- BACK / START and the d-pad stay as they are
function bfxr_PickList()
	local t = { 1, 2, 11, 12, 13, 14, 15, 16, 19, }
	if (not gBFX_L3Sprint) then
		tinsert(t, 18)
	end
	return t
end

function bfxr_PadValues(this)
	local m = this.bfxrTab or 0
	local k = this.bfxrPlayer
	IFText_fnSetString(this.bfxrPTitle, gBFXR_TabTitle[m])
	local i = 0
	while (i <= 2) do
		local a = 0.6 -- 21h-r7: (0.4 was nearly see-through; his are a darker orange)
		if (i == m) then
			a = 1
		end
		IFObj_fnSetAlpha(this["bfxrPTab" .. i], a)
		i = i + 1
	end
	local key, v
	for key, v in Platform_btn_map do
		local ln = this.bfxrPLines[key]
		if (ln and ln.label) then
			local show = 1
			-- as player 1's page: BACK = MAP, START = PAUSE MENU (fixed), L3 = SPRINT on the infantry tab (BF1GameExt's L3
			-- sprint) and nothing on the others; the d-pad's squad commands one line (the left one's), as his page has it
			if (key == 8) then
				bfxr_Text(ln.label, "MAP")
			elseif (key == 9) then
				bfxr_Text(ln.label, "PAUSE MENU")
			elseif ((key == 18) and gBFX_L3Sprint and (m == 0)) then
				bfxr_Text(ln.label, "SPRINT")
			elseif ((key == 18) and gBFX_L3Sprint) then
				IFText_fnSetString(ln.label, "ifs.controls.General.none")
			else
				local fn, idx = bfxr_PadFn(this, key, v, m)
				IFText_fnSetString(ln.label, bfxr_FnString(idx))
				if (((key == 3) or (key == 5) or (key == 6)) and (idx == 10 + 19 * m)) then
					show = nil
				end
			end
			if (gBFXR_ListOpen[k] and (key ~= this.bfxrPadKey)) then
				show = nil
			end
			IFObj_fnSetVis(ln, show)
			if (IFObj_fnSetColor) then
				if (this.bfxrPadKey and (key == this.bfxrPadKey)) then
					IFObj_fnSetColor(ln.label, 255, 170, 0)
				else
					IFObj_fnSetColor(ln.label, 255, 255, 255)
				end
			end
		end
	end
	local sens = bfxr_Ask(format("rp ctlget %d 2", k))
	if (sens < 1) then
		sens = 5
	end
	-- shown 10 - 100 as player 1's page shows his (a percent); the thumb from the left end (1) to the right end (10)
	bfxr_Text(this.bfxrPVals.joysens.label, format("%d", sens * 10))
	local f = this.bfxrPageScale or 0.51
	local l = -115.2 + (sens - 1) / 9 * 207.36
	IFImage_fnSetTexturePos(this.bfxrPSlider.fg, l * f, -12 * f, (l + 23.04) * f, 12 * f)
	IFText_fnSetString(this.bfxrPVals.rumble.label, "common.no")
	IFObj_fnSetAlpha(this.bfxrPVals.rumble, 0.4)
	IFObj_fnSetAlpha(this.bfxrPLabels.rumble, 0.4)
	local inv = 0
	if (m == 2) then
		bfxr_Text(this.bfxrPLabels.invert, "INVERT FLYING")
		inv = bfxr_Ask(format("rp ctlget %d 1", k))
	else
		bfxr_Text(this.bfxrPLabels.invert, "INVERT LOOK")
		inv = bfxr_Ask(format("rp ctlget %d 0", k))
	end
	if (inv == 1) then
		IFText_fnSetString(this.bfxrPVals.invert.label, "common.yes")
	else
		IFText_fnSetString(this.bfxrPVals.invert.label, "common.no")
	end
	IFText_fnSetString(this.bfxrPReset.btn.label, "common.reset")
end

-- his page up (mode "gpage" / "ppage"): the pause list hidden, the page shown, its first item picked (21h-r6: the gamepad
-- page starts on the pad - LB's label lit, as player 1's - when his buttons can be changed)
function bfxr_ShowPage(this, mode, idx)
	this.bfxrMode = mode
	bfxr_PopVis(this, nil)
	IFObj_fnSetVis(this.buttons, nil)
	this.CurButton = nil
	this.bfxrPageIdx = idx or 1
	this.bfxrPadKey = nil
	if (mode == "gpage") then
		bfxr_PagesVis(this, 1, nil)
		bfxr_GameValues(this)
		bfxr_SelectItem(this, bfxr_PageItem(this))
		return
	end
	bfxr_PagesVis(this, nil, 1)
	this.bfxrRemap = nil
	if (bfxr_Ask(format("rp remap %d", this.bfxrPlayer)) == 1) then
		this.bfxrRemap = 1
		this.bfxrPadKey = 2
		bfxr_SelectItem(this, nil)
	else
		bfxr_SelectItem(this, bfxr_PageItem(this))
	end
	bfxr_PadValues(this)
end

-- the nearest changeable label in a direction (labels in two columns left / right of the pad, at Platform_btn_map's lx1 /
-- ly1 - player 1's page's way); 1 if it moved
function bfxr_PadMove(this, dir)
	local cur = Platform_btn_map[this.bfxrPadKey]
	local cx = cur.lx1
	local cy = cur.ly1
	local best = nil
	local bestScore = 0
	local pick = bfxr_PickList()
	local i
	for i = 1, getn(pick) do
		local k = pick[i]
		local v = Platform_btn_map[k]
		if ((k ~= this.bfxrPadKey) and v) then
			local dy = v.ly1 - cy
			local sameSide = ((v.lx1 < 0) == (cx < 0))
			local ok = nil
			local score = 0
			if (dir == "up") then
				if (sameSide and (dy < 0)) then ok = 1 end
				score = -dy
			elseif (dir == "down") then
				if (sameSide and (dy > 0)) then ok = 1 end
				score = dy
			elseif (dir == "left") then
				if ((v.lx1 < 0) and (cx > 0)) then ok = 1 end
				score = abs(dy)
			else
				if ((v.lx1 > 0) and (cx < 0)) then ok = 1 end
				score = abs(dy)
			end
			if (ok and ((not best) or (score < bestScore))) then
				best = k
				bestScore = score
			end
		end
	end
	if (best) then
		this.bfxrPadKey = best
		ScriptCB_SndPlaySound("shell_select_change")
		bfxr_PadValues(this)
		return 1
	end
	return nil
end

-- the actions his button can get on tab m (player 1's list: the game's contents_functype_*_button when it is there, else the
-- tab's button actions), as the page's function numbers
function bfxr_ListFns(m)
	local c = contents_functype_infantry_button
	if (m == 1) then
		c = contents_functype_vehicle_button
	elseif (m == 2) then
		c = contents_functype_flyer_button
	end
	local out = {}
	local seen = {}
	if (type(c) == "table") then
		local i
		for i = 1, getn(c) do
			local e = c[i]
			if ((type(e) == "table") and (type(e.showidx) == "number")) then
				local fn = e.showidx
				if (fn > 19) then
					fn = fn - 19 * m
				end
				if ((fn >= 1) and (fn <= 15) and ((fn < 11) or (fn > 14)) and (not seen[fn])) then
					seen[fn] = 1
					tinsert(out, fn)
				end
			end
		end
	end
	if (getn(out) == 0) then
		local all = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 15, }
		local i
		for i = 1, getn(all) do
			local str = bfxr_FnString(all[i] + 19 * m)
			if (not strfind(strlower(str), "none")) then
				tinsert(out, all[i])
			end
		end
	end
	if (not gBFXR_ListLogged[m]) then
		gBFXR_ListLogged[m] = 1
		local s = ""
		local i
		for i = 1, getn(out) do
			s = s .. " " .. out[i]
		end
		bfxr_Log(format("tab %d's action list (%s):%s", m, (type(c) == "table") and "the game's contents_functype list" or
			"the tab's button actions", s))
	end
	return out
end

-- the list's rows (5 shown, as player 1's): the action names, the picked one lit
function bfxr_ListShow(this)
	local k = this.bfxrPlayer
	local L = gBFXR_List[k]
	local m = this.bfxrTab or 0
	local n = getn(L.fns)
	if (L.sel < L.first) then
		L.first = L.sel
	elseif (L.sel > L.first + 4) then
		L.first = L.sel - 4
	end
	local i = 1
	while (i <= 5) do
		local row = this.bfxrPList["row" .. i]
		local j = L.first + i - 1
		if (j <= n) then
			IFText_fnSetString(row, bfxr_FnString(L.fns[j] + 19 * m))
			IFObj_fnSetVis(row, 1)
			if (IFObj_fnSetColor) then
				if (j == L.sel) then
					IFObj_fnSetColor(row, 255, 170, 0)
				else
					IFObj_fnSetColor(row, 255, 255, 255)
				end
			end
		else
			IFObj_fnSetVis(row, nil)
		end
		i = i + 1
	end
end

-- A on a pad label: the list of actions for it, beside it (player 1's: the other side of the pad), the other lines hidden
function bfxr_ListOpen(this)
	local k = this.bfxrPlayer
	local key = this.bfxrPadKey
	local m = this.bfxrTab or 0
	local fns = bfxr_ListFns(m)
	local cur = bfxr_PadFn(this, key, Platform_btn_map[key], m)
	local sel = 1
	local i
	for i = 1, getn(fns) do
		if (fns[i] == cur) then
			sel = i
		end
	end
	gBFXR_List[k] = { fns = fns, sel = sel, first = max(sel - 2, 1), key = key, }
	gBFXR_ListOpen[k] = 1
	local f = this.bfxrPageScale or 0.51
	if (IFObj_fnSetPos) then
		if (Platform_btn_map[key].lx1 < 0) then
			IFObj_fnSetPos(this.bfxrPList, 120 * f, 40 * f)
		else
			IFObj_fnSetPos(this.bfxrPList, -120 * f, 40 * f)
		end
	end
	IFObj_fnSetVis(this.bfxrPList, 1)
	bfxr_ListShow(this)
	bfxr_PadValues(this)
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
end

function bfxr_ListClose(this)
	gBFXR_ListOpen[this.bfxrPlayer] = nil
	IFObj_fnSetVis(this.bfxrPList, nil)
	bfxr_PadValues(this)
end

function bfxr_ListMove(this, step)
	local L = gBFXR_List[this.bfxrPlayer]
	local n = getn(L.fns)
	local s = L.sel + step
	if ((s < 1) or (s > n)) then
		return
	end
	L.sel = s
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_ListShow(this)
end

-- A in the list: the action goes to his button (his own map, kept in BF1GameExt.ini)
function bfxr_ListPick(this)
	local k = this.bfxrPlayer
	local L = gBFXR_List[k]
	local m = this.bfxrTab or 0
	local fn = L.fns[L.sel]
	local r = bfxr_Ask(format("rp ctlfnset %d %d %d %d", k, m, L.key, fn or -1))
	bfxr_ListClose(this)
	if (r == 1) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Log(format("player %d: tab %d, %s -> %s (function %d)", k, m, Platform_btn_map[L.key].name, bfxr_FnString(fn + 19 * m), fn))
	else
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxr_Log(format("player %d: tab %d, %s - NOT changed (%d)", k, m, Platform_btn_map[L.key].name, r))
	end
end

function bfxr_PageMove(this, step)
	if (gBFXR_ListOpen[this.bfxrPlayer]) then
		bfxr_ListMove(this, step)
		return
	end
	local list = gBFXR_GItems
	if (this.bfxrMode == "ppage") then
		list = gBFXR_PItems
		-- 21h-r6: the pad's labels first, then the settings (player 1's page: down from the bottom label to them, up from the
		-- first back to the pad)
		if (this.bfxrPadKey) then
			local dir = "down"
			if (step < 0) then
				dir = "up"
			end
			if ((not bfxr_PadMove(this, dir)) and (step > 0)) then
				this.bfxrLastPad = this.bfxrPadKey
				this.bfxrPadKey = nil
				this.bfxrPageIdx = 1
				ScriptCB_SndPlaySound("shell_select_change")
				bfxr_PadValues(this)
				bfxr_SelectItem(this, bfxr_PageItem(this))
			end
			return
		end
		if (this.bfxrRemap and (((step < 0) and ((this.bfxrPageIdx or 1) == 1)) or ((step > 0) and ((this.bfxrPageIdx or 1) == getn(list))))) then
			this.bfxrPadKey = this.bfxrLastPad or 2
			ScriptCB_SndPlaySound("shell_select_change")
			bfxr_SelectItem(this, nil)
			bfxr_PadValues(this)
			return
		end
	end
	local n = getn(list)
	local i = (this.bfxrPageIdx or 1) + step
	if (i < 1) then
		i = n
	elseif (i > n) then
		i = 1
	end
	this.bfxrPageIdx = i
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_SelectItem(this, bfxr_PageItem(this))
end

-- RESET TO DEFAULTS on the gamepad page: the shown tab's buttons back to the game's (player 1's page's RESET), and his
-- sensitivity / invert back to the defaults
function bfxr_PadReset(this)
	local k = this.bfxrPlayer
	local m = this.bfxrTab or 0
	if (this.bfxrRemap) then
		bfxr_Ask(format("rp ctlfnreset %d %d", k, m))
	end
	bfxr_Ask(format("rp ctlset %d 0 0", k))
	bfxr_Ask(format("rp ctlset %d 1 1", k))
	bfxr_Ask(format("rp ctlset %d 2 5", k))
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	bfxr_Log(format("player %d: CONTROL OPTIONS - RESET TO DEFAULTS (tab %d's buttons, invert look OFF, invert flying ON, sensitivity 5)", k, m))
end

-- left / right (and A, step 1) on a page item
function bfxr_PageChange(this, step)
	local k = this.bfxrPlayer
	local i = this.bfxrPageIdx or 1
	if (this.bfxrMode == "gpage") then
		local tag = gBFXR_GItems[i]
		if (tag == "persp") then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
		elseif (tag == "diff") then
			bfxr_DiffChange(this, step)
			bfxr_GameValues(this)
		elseif (tag == "autoaim") then
			-- 21i-r10: his own auto aim
			local v = bfxr_Ask(format("aimp get %d", k))
			if (v < 0) then
				ifelm_shellscreen_fnPlaySound(this.errorSound)
			else
				bfxr_Ask(format("aimp set %d %d", k, 1 - v))
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				bfxr_Log(format("player %d: GAME OPTIONS - AUTO AIM %s", k, (v == 0) and "ON" or "OFF"))
			end
			bfxr_GameValues(this)
		elseif (step == 1) then
)BFXLUA",
R"BFXLUA(			if (bfxr_DiffOk()) then
				ScriptCB_SetDifficulty(2)
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				bfxr_Log(format("player %d: GAME OPTIONS - RESET TO DEFAULTS (difficulty MEDIUM)", k))
			else
				ifelm_shellscreen_fnPlaySound(this.errorSound)
			end
			bfxr_GameValues(this)
		end
		bfxr_Hilite(this, 0)
		return
	end
	local tag = gBFXR_PItems[i]
	if (tag == "joysens") then
		local v = bfxr_Ask(format("rp ctlget %d 2", k))
		if (v < 1) then
			v = 5
		end
		v = v + step
		if (v < 1) then
			v = 1
		elseif (v > 10) then
			v = 10
		end
		bfxr_Ask(format("rp ctlset %d 2 %d", k, v))
		ScriptCB_SndPlaySound("shell_select_change")
	elseif (tag == "invert") then
		local which = 0
		if ((this.bfxrTab or 0) == 2) then
			which = 1
		end
		local v = 1
		if (bfxr_Ask(format("rp ctlget %d %d", k, which)) == 1) then
			v = 0
		end
		bfxr_Ask(format("rp ctlset %d %d %d", k, which, v))
		ScriptCB_SndPlaySound("shell_select_change")
	elseif (step == 1) then
		bfxr_PadReset(this)
	end
	bfxr_PadValues(this)
	bfxr_Hilite(this, 0)
end

-- LB / RB on the gamepad page: the tab
function bfxr_Tab(this, step)
	if (this.bfxrBusy or (this.bfxrMode ~= "ppage") or gBFXR_ListOpen[this.bfxrPlayer]) then
		return
	end
	local m = (this.bfxrTab or 0) + step
	if (m < 0) then
		m = 2
	elseif (m > 2) then
		m = 0
	end
	this.bfxrTab = m
	ScriptCB_SndPlaySound("shell_select_change")
	bfxr_PadValues(this)
	bfxr_Hilite(this, 0)
	if (not gBFXR_TabLogged) then
		gBFXR_TabLogged = 1
		bfxr_Log(format("player %d: the gamepad page's tab %d (LB / RB)", this.bfxrPlayer, m))
	end
end

function bfxr_TabLeft(this)
	bfxr_Tab(this, -1)
end

function bfxr_TabRight(this)
	bfxr_Tab(this, 1)
end

-- Y on the gamepad page: RESET TO DEFAULTS (as player 1's page: the shown tab)
function bfxr_Misc2(this)
	if ((not this.bfxrBusy) and (this.bfxrMode == "ppage") and (not gBFXR_ListOpen[this.bfxrPlayer])) then
		bfxr_PadReset(this)
		bfxr_PadValues(this)
		bfxr_Hilite(this, 0)
	end
end

-- one screen: the same build as ifs_pausemenu2, on its own viewport (k = 3 / 4 -> viewport 2 / 3)
function bfxr_Make(k)
	local name = format("ifs_pausemenu%d", k)
	local t = NewIFShellScreen2 {
		nologo = 1,
		bfxrPlayer = k,
		acceptSound = "shell_menu_accept",
		exitSound = "shell_menu_exit",
		cancelSound = "shell_menu_cancel",
		errorSound = "shell_menu_error",
		buttons = NewIFContainer {
			ScreenRelativeX = 0.6,
			ScreenRelativeY = 0.5,
			y = 20,
			rotY = 25,
		},
		Enter = bfxr_Enter,
		Leave = bfxr_Leave,
		Update = bfxr_Update,
		Input_Accept = bfxr_Accept,
		Input_Back = bfxr_Back,
		Input_Start = bfxr_Start,
		Input_GeneralUp = bfxr_Up,
		Input_GeneralDown = bfxr_Down,
		Input_GeneralLeft = bfxr_Left,
		Input_GeneralRight = bfxr_Right,
		Input_LTrigger2 = bfxr_TabLeft,
		Input_RTrigger2 = bfxr_TabRight,
		Input_Misc2 = bfxr_Misc2,
	}
	t.bfxrPop = bfxr_NewPop()
	-- 21h-r5: player 1's GAME OPTIONS and gamepad pages, copied (gBFXR_Pages, [Splitscreen] OptionsPagesP34)
	local f = (gBFXR_PopScale or 51) / 100
	if ((f < 0.2) or (f > 1.5)) then
		f = 0.51
	end
	local pages = nil
	if (((gBFXR_Pages or 1) == 1) and bfxr_PagesOk()) then
		bfxr_NewGamePage(t, f)
		pages = bfxr_NewPadPage(t, f)
		if (not pages) then
			t.bfxrGTitle = nil
			t.bfxrGLabels = nil
			t.bfxrGItems = nil
		end
	end
	t.bfxrPageScale = f
	t.CurButton = AddVerticalButtons(t.buttons, bfxr_layout)
	t.Viewport = k - 1
	setglobal(name, t)
	AddIFScreen(t, name)
	if (t.bfxrPop) then
		IFObj_fnSetVis(t.bfxrPop, nil)
	end
	if (pages) then
		bfxr_PagesVis(t, nil, nil)
	end
	bfxr_Log(format("%s made: viewport %d, %s, popup window %s (scale %.2f), player 1's GAME OPTIONS / gamepad pages %s", name,
		t.Viewport, tostring(t.ScreenName), t.bfxrPop and "made (player 1's look)" or "NOT made (YES / NO in the list)",
		(t.bfxrPop and t.bfxrPop.bfxrScale) or 0, pages and "made" or ((gBFXR_Pages == 0) and "off (OptionsPagesP34=0)" or
		("NOT made (r4's lists; missing:" .. bfxr_PagesMissing() .. ")"))))
end

function bfxr_MakeAll()
	local n = gBFXR_Players or 2
	if (n >= 3) then
		bfxr_Make(3)
	end
	if (n >= 4) then
		bfxr_Make(4)
	end
end

-- 21h-r3: player 2's pause menu gets player 1's sounds (the game's ifs_pausemenu2 has none: its template has no sound fields)
function bfxr_P2Sounds()
	if ((gBFXR_P2Sounds ~= 1) or (not ifs_pausemenu2)) then
		return
	end
	local s = ifs_pausemenu2
	s.acceptSound = s.acceptSound or "shell_menu_accept"
	s.exitSound = s.exitSound or "shell_menu_exit"
	s.cancelSound = s.cancelSound or "shell_menu_cancel"
	s.errorSound = s.errorSound or "shell_menu_error"
	if (not gBFXR_P2SoundsLogged) then
		gBFXR_P2SoundsLogged = 1
		bfxr_Log("ifs_pausemenu2: player 1's accept / exit / cancel / error sounds set")
	end
end

bfxr_P2Sounds()

-- 21h-r4: player 2's OPTIONS (the game's ifs_opt_contmain while player 2's pause is the one on - the pausing viewport 1, in a
-- match): no VIDEO OPTIONS, and CONTROL OPTIONS goes straight to the gamepad page (ifs_opt_controller), never the keyboard
-- page. Player 1's (pausing viewport 0) as the game has it. Wrapped once per Lua state.
function bfxr_IsP2Opts()
	if (ScriptCB_GetShellActive and ScriptCB_GetShellActive()) then
		return nil
	end
	return ScriptCB_GetPausingViewport() == 1
end

function bfxr_P2OptsEnter(this, bFwd)
	gBFXR_OrigOptsEnter(this, bFwd)
	if (bfxr_IsP2Opts() and this.buttons and this.buttons.pcvideo and ifs_opt_top_vbutton_layout) then
		this.buttons.pcvideo.hidden = 1
		local cur = this.CurButton
		local first = ShowHideVerticalButtons(this.buttons, ifs_opt_top_vbutton_layout)
		if ((not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
			cur = first
		end
		this.CurButton = cur
		SetCurButton(this.CurButton)
		if (not gBFXR_P2OptsLogged) then
			gBFXR_P2OptsLogged = 1
			bfxr_Log(format("player 2's OPTIONS: VIDEO OPTIONS hidden (first item %s)", tostring(this.CurButton)))
		end
	end
end

function bfxr_P2OptsAccept(this)
	if (bfxr_IsP2Opts() and (this.CurButton == "pccontrols")) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxr_Log("player 2's CONTROL OPTIONS: straight to the gamepad page (ifs_opt_controller)")
		ScriptCB_PushScreen("ifs_opt_controller")
		return
	end
	gBFXR_OrigOptsAccept(this)
end

function bfxr_P2Opts()
	if (gBFXR_P2Opts ~= 1) then
		return
	end
	if (gBFXR_P2OptsWrapped) then
		return
	end
	if ((not ifs_opt_contmain) or (not ifs_opt_contmain.Enter) or (not ifs_opt_contmain.Input_Accept)) then
		bfxr_Log("player 2's OPTIONS: ifs_opt_contmain is not made yet - not changed")
		return
	end
	gBFXR_P2OptsWrapped = 1
	gBFXR_OrigOptsEnter = ifs_opt_contmain.Enter
	gBFXR_OrigOptsAccept = ifs_opt_contmain.Input_Accept
	ifs_opt_contmain.Enter = bfxr_P2OptsEnter
	ifs_opt_contmain.Input_Accept = bfxr_P2OptsAccept
	bfxr_Log("player 2's OPTIONS: gamepad only (no VIDEO OPTIONS; CONTROL OPTIONS -> the gamepad page)")
end

bfxr_P2Opts()

-- made right after the game has finished ifs_pausemenu2 (its ScriptCB_EndIFScreen), once per Lua state
if ((not gBFXR_EndWrapped) and ((gBFXR_Players or 2) >= 3)) then
	gBFXR_EndWrapped = 1
	gBFXR_OrigEnd = ScriptCB_EndIFScreen
	ScriptCB_EndIFScreen = function(name)
		gBFXR_OrigEnd(name)
		if ((name == "ifs_pausemenu2") and (not gBFXR_Made)) then
			gBFXR_Made = 1
			ScriptCB_EndIFScreen = gBFXR_OrigEnd
			bfxr_P2Sounds()
			bfxr_P2Opts()
			bfxr_MakeAll()
		end
	end
	bfxr_Log(format("waiting for the game's ifs_pausemenu2 to be finished (%d players)", gBFXR_Players or 2))
end
)BFXLUA",
};

// DllMain: the INI switches and the script.
void SplitLoad()
{
    char dir[MAX_PATH] = {};
    if (!ExeDir(dir, sizeof(dir)))
        return;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", dir);
    g_sideLockIni = GetPrivateProfileIntA("Splitscreen", "SideLock", 1, ini) != 0; // 21f
    g_splitIni = GetPrivateProfileIntA("Splitscreen", "MainMenuSplit", 1, ini) != 0;
    g_splitSignIn = GetPrivateProfileIntA("Splitscreen", "SplitSignIn", 1, ini) != 0;
    g_signIn4 = GetPrivateProfileIntA("Splitscreen", "SignIn4", 1, ini) != 0; // 21g
    g_tempProfilesIni = GetPrivateProfileIntA("Splitscreen", "TempProfiles", 1, ini) != 0; // 21g-r2
    g_gcTurnScanIni = GetPrivateProfileIntA("Splitscreen", "GCTurnScan", 1, ini) != 0; // 21g-r4
    g_gcPickLookIni = GetPrivateProfileIntA("Splitscreen", "GCPickLook", 1, ini) != 0;  // 21j-r8
    g_gcCmd2Ini = GetPrivateProfileIntA("Splitscreen", "GCCommander2P", 1, ini) != 0;   // 21j-r8
    g_padIconsIni = GetPrivateProfileIntA("Splitscreen", "SignInPadIcons", 1, ini) != 0;  // 21j-r27
    g_mouseBackIni = GetPrivateProfileIntA("Splitscreen", "SignInMouseBack", 1, ini) != 0; // 21j-r27
    g_soundFixIni = GetPrivateProfileIntA("Splitscreen", "SignInSoundFix", 1, ini) != 0;   // 21j-r28
    g_pauseScanIni = GetPrivateProfileIntA("Splitscreen", "PauseScan34", 1, ini) != 0; // 21h-r1
    g_realPauseIni = GetPrivateProfileIntA("Splitscreen", "RealPauseP34", 1, ini) != 0; // 21h-r2
    g_p2SoundsIni = GetPrivateProfileIntA("Splitscreen", "PauseSoundsP2", 1, ini) != 0; // 21h-r3
    g_p2OptsIni = GetPrivateProfileIntA("Splitscreen", "OptionsP2", 1, ini) != 0;        // 21h-r4
    g_popScaleIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "PopupScaleP34", 51, ini)); // 21h-r4
    if (g_popScaleIni < 20 || g_popScaleIni > 150)
        g_popScaleIni = 51;
    g_optScanIni = GetPrivateProfileIntA("Splitscreen", "OptionsScan34", 1, ini) != 0;   // 21h-r4
    g_pagesIni = GetPrivateProfileIntA("Splitscreen", "OptionsPagesP34", 1, ini) != 0;   // 21h-r5
    g_botScanIni = GetPrivateProfileIntA("Splitscreen", "BotScanIA", 1, ini) != 0;        // 21i
    g_iaPageIni = GetPrivateProfileIntA("Splitscreen", "IAGameOptions", 1, ini) != 0;     // 21i-r6
    g_splitMenuIni = GetPrivateProfileIntA("Splitscreen", "SplitMenu", 1, ini) != 0;      // 21i-r8
    g_aimMenuIni = GetPrivateProfileIntA("Splitscreen", "AutoAimMenu", 1, ini) != 0;      // 21i-r8
    g_aimMouseIni = GetPrivateProfileIntA("Splitscreen", "AutoAimMouse", 1, ini) != 0;    // 21i-r9
    g_aimPlayersIni = GetPrivateProfileIntA("Splitscreen", "AutoAimPlayers", 1, ini) != 0; // 21i-r10
    g_aimStrengthIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "AutoAimStrength", 50, ini)); // 21i-r11
    g_aimSlowIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "AutoAimSlowdown", 0, ini));      // 21i-r12
    {   // 21i-r13: the pad's dead zone / look curve / sensitivity style (read as text: -1 and decimals)
        char v[32] = {};
        GetPrivateProfileStringA("Splitscreen", "PadDeadZone", "2", v, sizeof(v), ini);
        char* end = nullptr;
        const long dz = strtol(v, &end, 10);
        g_padDzIni = end == v ? 2 : (dz < 0 ? -1 : static_cast<int>(dz > 50 ? 50 : dz));
        v[0] = 0;
        GetPrivateProfileStringA("Splitscreen", "PadLookCurve", "2.0", v, sizeof(v), ini);
        end = nullptr;
        const double c = strtod(v, &end);
        g_padCurveIni = (end == v || c <= 0.0) ? 0.0 : (c < 0.5 ? 0.5 : (c > 5.0 ? 5.0 : c));
        g_padSensBf2Ini = GetPrivateProfileIntA("Splitscreen", "PadSensStyle", 1, ini) != 0;
        g_padMenuIni = GetPrivateProfileIntA("Splitscreen", "PadMenu", 1, ini) != 0; // 21i-r14
        g_padExpIni = GetPrivateProfileIntA("Splitscreen", "PadExperiment", 0, ini) != 0; // 21i-r15
        const int fov = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "FovScale", 100, ini)); // 21i-r16
        g_fovPctIni = fov < 80 ? 80 : (fov > 120 ? 120 : fov);
        g_fovMenuIni = GetPrivateProfileIntA("Splitscreen", "FovMenu", 1, ini) != 0;
        g_modsMenuIni = GetPrivateProfileIntA("Splitscreen", "ModsMenu", 1, ini) != 0; // 21j-r17
        g_wheelMenuIni = GetPrivateProfileIntA("Splitscreen", "SpawnScreenMenu", 1, ini) != 0; // 21j-r18
        g_tutorialsIni = GetPrivateProfileIntA("Splitscreen", "TutorialsMenu", 1, ini) != 0;   // 21j-r21
        g_tutFitIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "TutorialsMovieFit", 1, ini)); // 21j-r22
        if (g_tutFitIni < 0 || g_tutFitIni > 2)
            g_tutFitIni = 1;
        g_tutGCIni = GetPrivateProfileIntA("Splitscreen", "TutorialsGC", 1, ini) != 0;         // 21j-r22
        g_tutCampIni = GetPrivateProfileIntA("Splitscreen", "TutorialsCampaign", 1, ini) != 0; // 21j-r22
        g_pauseCamIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "PauseFreeCam", 1, ini)); // 21j-r23
        if (g_pauseCamIni < 0 || g_pauseCamIni > 2)
            g_pauseCamIni = 1;
        g_pauseConIni = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "PauseConsole", 1, ini)); // 21j-r23
        if (g_pauseConIni < 0 || g_pauseConIni > 2)
            g_pauseConIni = 1;
        g_freeCamScanIni = GetPrivateProfileIntA("Splitscreen", "FreeCamScan", 1, ini) != 0; // 21j-r23
        g_freeCamPadIni = GetPrivateProfileIntA("Splitscreen", "FreeCamPad", 1, ini) != 0;   // 21j-r26
        g_grassScanIni = GetPrivateProfileIntA("Splitscreen", "GrassScan", 1, ini) != 0; // 21j-r1
        g_grassFixIni = GetPrivateProfileIntA("Splitscreen", "GrassFadeFix", 1, ini) != 0; // 21j-r2
    }
    g_aimP[1] = GetPrivateProfileIntA("Splitscreen", "AutoAimP2", 0, ini) != 0 ? 1 : 0;
    g_aimP[2] = GetPrivateProfileIntA("Splitscreen", "AutoAimP3", 0, ini) != 0 ? 1 : 0;
    g_aimP[3] = GetPrivateProfileIntA("Splitscreen", "AutoAimP4", 0, ini) != 0 ? 1 : 0;
    snprintf(g_mainIni, sizeof(g_mainIni), "%s", ini);
    snprintf(g_iaStore, sizeof(g_iaStore), "%s\\BF1GameExt_IAOptions.ini", dir);
    // 21i-r2: empty (or -1) = the map's own number; read as text so an empty value is not 0
    {
        char v[32] = {};
        GetPrivateProfileStringA("Splitscreen", "AIUnitsIA", "", v, sizeof(v), ini);
        char* end = nullptr;
        const long n = strtol(v, &end, 10);
        g_botUnitsIA = (end != v && n >= 0) ? static_cast<int>(n > 64 ? 64 : n) : -1;
        v[0] = 0;
        GetPrivateProfileStringA("Splitscreen", "ReinforcementsIA", "", v, sizeof(v), ini);
        end = nullptr;
        const long r = strtol(v, &end, 10);
        g_botReinfIA = (end != v && r >= 0) ? static_cast<int>(r > 9999 ? 9999 : (r < 1 ? 1 : r)) : -1;
    }
    Log("[mods] 21j-r17 / r18: [Splitscreen] ModsMenu=%d (%s)", g_modsMenuIni ? 1 : 0,
        g_modsMenuIni ? "the main menu's OPTIONS gets a MODS row above CREDITS: SPRINT, RAGDOLL EFFECTS, ADDON FOLDER - made "
                        "while the game makes the shell's ifs_opt_contmain (never in a match: nobody switches them mid-game)"
                      : "no MODS row");
    {
        static const char* const kWhere[3] = { "off", "player 1's pause menu in single player",
                                                "player 1's pause menu in single player and split screen" };
        Log("[freecam] 21j-r23: [Splitscreen] PauseFreeCam=%d (FREE CAMERA: %s), PauseConsole=%d (FAKE CONSOLE: %s), FreeCamScan=%d (%s)",
            g_pauseCamIni, kWhere[g_pauseCamIni], g_pauseConIni, kWhere[g_pauseConIni], g_freeCamScanIni ? 1 : 0,
            g_freeCamScanIni ? "[freecam] lines while player 1's free camera is on: its up / down / speed controls, the keys and "
                               "pad buttons held, its debug keys' flags"
                             : "off");
        Log("[freecam] 21j-r26: [Splitscreen] FreeCamPad=%d (%s)", g_freeCamPadIni ? 1 : 0,
            g_freeCamPadIni ? "player 1's free camera on his pad: R3 up, L3 down (not RT / LT), the right stick looks, X stops / "
                              "starts it"
                            : "the game's own pad controls (RT / LT up / down, no stick look)");
    }
    Log("[tutorials] 21j-r22: [Splitscreen] TutorialsMovieFit=%d (%s), TutorialsGC=%d (%s), TutorialsCampaign=%d (%s)",
        g_tutFitIni,
        g_tutFitIni == 1   ? "the tutorial movies in the background's black box at 4:3"
        : g_tutFitIni == 2 ? "the tutorial movies over the whole black box (stretched)"
                           : "the tutorial movies where the game puts them (the box's top left, partly under its frame)",
        g_tutGCIni ? 1 : 0,
        g_tutGCIni ? "GALACTIC CONQUEST plays its tutorial movie (tutorial08) before its setup, as the consoles' new game"
                   : "no galactic conquest tutorial movie",
        g_tutCampIni ? 1 : 0,
        g_tutCampIni ? "the campaigns' command post tutorials after Yoda's briefing of their first mission (tutorial01cw / "
                       "tutorial01gcw)"
                     : "no campaign tutorials");
    Log("[tutorials] 21j-r21: [Splitscreen] TutorialsMenu=%d (%s)", g_tutorialsIni ? 1 : 0,
        g_tutorialsIni ? "the main menu shows TUTORIALS on PC: the game's own tutorials menu and its 8 movies "
                         "(movies\\tut<lang>.mvs, else movies\\tuteng.mvs) - made while the game makes ifs_main / ifs_tutorials"
                       : "no TUTORIALS button (the game's PC main menu)");
    Log("[iaopts] 21j-r18: [Splitscreen] SpawnScreenMenu=%d (%s)", g_wheelMenuIni ? 1 : 0,
        g_wheelMenuIni ? "the main menu's GAME OPTIONS gets a SPAWN SCREEN row: CLASS WHEEL / PC MENU (SINGLEPLAYER ONLY) - "
                         "[ClassWheel] Enabled"
                       : "no SPAWN SCREEN row");
    Log("[optscan] 21h-r5: [Splitscreen] OptionsPagesP34=%d (%s)", g_pagesIni ? 1 : 0,
        g_pagesIni ? "players 3 / 4's GAME OPTIONS / CONTROL OPTIONS: copies of player 1's pages (the gamepad page with its 3 tabs)"
                   : "players 3 / 4's GAME OPTIONS / CONTROL OPTIONS as r4's lists");
    Log("[optscan] 21h-r4: [Splitscreen] OptionsP2=%d (%s), PopupScaleP34=%d (players 3 / 4's YES / NO window at %d%%), "
        "OptionsScan34=%d (%s)",
        g_p2OptsIni ? 1 : 0, g_p2OptsIni ? "3 / 4 players: player 2's OPTIONS without VIDEO, CONTROL OPTIONS -> the gamepad page"
                                         : "player 2's OPTIONS as the game has them",
        g_popScaleIni, g_popScaleIni, g_optScanIni ? 1 : 0,
        g_optScanIni ? "the game's GAME OPTIONS and gamepad pages dumped as the game builds them, [optscan] lines" : "off");
    if (g_realPauseIni)
    {
        char rpPath[MAX_PATH + 40];
        snprintf(rpPath, sizeof(rpPath), "%s\\BF1GameExt_pause34.lua", dir);
        // 21h-r3: the copy built in is in pieces - joined here
        size_t builtLen = 0;
        for (const char* piece : kPause34Pieces)
            builtLen += strlen(piece);
        char* built = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, builtLen + 1));
        if (built)
        {
            size_t at = 0;
            for (const char* piece : kPause34Pieces)
            {
                const size_t n = strlen(piece);
                memcpy(built + at, piece, n);
                at += n;
            }
            built[at] = 0;
            g_rpSrc = LoadScript("[rp34]", "BF1GameExt_pause34", rpPath, built, builtLen, &g_rpLen, g_rpFrom, sizeof(g_rpFrom));
            HeapFree(GetProcessHeap(), 0, built);
        }
        Log("[rp34] players 3 / 4's real pause menus script: %s (%lu bytes); 21h-r3: [Splitscreen] PauseSoundsP2=%d (%s)",
            g_rpSrc ? g_rpFrom : "OUT OF MEMORY - not loaded",
            static_cast<unsigned long>(g_rpLen), g_p2SoundsIni ? 1 : 0,
            g_p2SoundsIni ? "player 2's pause menu gets player 1's accept / back sounds in 3 / 4-player matches"
                          : "player 2's pause menu as the game has it");
    }
    else
        Log("[rp34] BF1GameExt.ini [Splitscreen] RealPauseP34=0: no ifs_pausemenu3 / 4 (BF1GameExt's own pause screens)");
    // 21g-r3: the last Galactic Conquest picks (3 / 4 players) - for a conquest resumed in a later session (the script checks
    // them against the conquest's players 1 / 2 before it uses them)
    snprintf(g_gcIni, sizeof(g_gcIni), "%s", ini);
    {
        char last[64] = {};
        GetPrivateProfileStringA("Splitscreen", "GCLastPick", "", last, sizeof(last), ini);
        long v[6] = {};
        const char* q = last;
        int got = 0;
        for (int i = 0; i < 6; ++i)
        {
            char* end = nullptr;
            const long x = strtol(q, &end, 10);
            if (end == q)
                break;
            v[i] = x;
            q = end;
            ++got;
        }
        if (got == 6)
        {
            for (int i = 0; i < 4; ++i)
                g_gcSide[i] = (v[i] == 1 || v[i] == 2) ? v[i] : 0;
            for (int i = 0; i < 2; ++i)
                g_gcCmd[i] = (v[4 + i] >= 1 && v[4 + i] <= 4) ? v[4 + i] : 0;
            Log("[splitmenu] 21g-r3: [Splitscreen] GCLastPick=%s - the last Galactic Conquest's sides / commanders (for a "
                "conquest resumed with 3 / 4 players)", last);
        }
    }
    if (!g_splitIni)
    {
        Log("[splitmenu] BF1GameExt.ini [Splitscreen] MainMenuSplit=0: the main menu is left as the game has it (no SPLIT "
            "SCREEN)");
        return;
    }
    char path[MAX_PATH + 40];
    snprintf(path, sizeof(path), "%s\\BF1GameExt_split.lua", dir);
    // 21g-r2: the copy built in is in pieces (MSVC: 65535 bytes at most in one literal) - joined here
    size_t builtLen = 0;
    for (const char* piece : kSplitPieces)
        builtLen += strlen(piece);
    char* built = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, builtLen + 1));
    if (built)
    {
        size_t at = 0;
        for (const char* piece : kSplitPieces)
        {
            const size_t n = strlen(piece);
            memcpy(built + at, piece, n);
            at += n;
        }
        built[at] = 0;
        g_splitSrc = LoadScript("[splitmenu]", "BF1GameExt_split", path, built, builtLen, &g_splitLen, g_splitFrom,
                                sizeof(g_splitFrom));
        HeapFree(GetProcessHeap(), 0, built);
    }
    if (!g_splitSrc)
    {
        Log("[splitmenu] out of memory - the main menu is left as the game has it");
        return;
    }
    Log("[splitmenu] split screen menu script: %s (%lu bytes); SPLIT SCREEN -> %s", g_splitFrom,
        static_cast<unsigned long>(g_splitLen),
        g_splitSignIn ? "the console sign-in screen (SplitSignIn=1)" : "straight to the game modes (SplitSignIn=0)");
    Log("[splitmenu] 21g: [Splitscreen] SignIn4=%d - %s", g_signIn4 ? 1 : 0,
        g_signIn4 ? "with players 3 / 4 on (Players=3 / 4) the sign-in screen is Battlefront II's 4-player one (four quarters, "
                    "players 3 and 4 join with pads 3 and 4); with 2 players the 2-player one as 21e"
                  : "the 2-player sign-in screen always (as 21e)");
    Log("[splitmenu] 21g-r2: [Splitscreen] TempProfiles=%d - %s", g_tempProfilesIni ? 1 : 0,
        g_tempProfilesIni ? "the sign-in lists PLAYER 2 / 3 / 4 as temporary profiles (never saved) where no profile has that "
                            "name; players 2 / 3 / 4 start on them"
                          : "the sign-in lists the saved profiles only (as 21g-r1)");
    Log("[splitmenu] 21g-r4: [Splitscreen] GCTurnScan=%d - %s", g_gcTurnScanIni ? 1 : 0,
        g_gcTurnScanIni ? "the galaxy turn scanner: on Galactic Conquest's galaxy map in split screen, [gcturn] lines - each "
                          "turn and who moves it, each choice and press, the battle result, the game's script errors (log only)"
                        : "no galaxy turn scanner");
    Log("[splitmenu] 21j-r8: [Splitscreen] GCPickLook=%d GCCommander2P=%d - Galactic Conquest's PICK YOUR SIDE %s; with 2 players %s",
        g_gcPickLookIni ? 1 : 0, g_gcCmd2Ini ? 1 : 0,
        g_gcPickLookIni ? "in the console's look (a row of side pictures for every player, a box for each in the class screen's "
                          "look: PLAYER n, then COMMANDER / PLAYER READY in his side's colour)"
                        : "with 21g-r3's text list",
        g_gcCmd2Ini ? "both lock in, the first on a side commands it on the galaxy map (co-op: only he moves)"
                    : "as 21f (player 1 launches, the game's own galaxy)");
    Log("[splitmenu] 21j-r27: [Splitscreen] SignInPadIcons=%d SignInMouseBack=%d - the sign-in's BACK / ACCEPT %s; its BACK button "
        "at the bottom left %s",
        g_padIconsIni ? 1 : 0, g_mouseBackIni ? 1 : 0,
        g_padIconsIni ? "with the pad's B / A (the 2020 gamepad sheet) for every player" : "with the game's icons (the mouse's)",
        g_mouseBackIni ? "on with 2 / 3 / 4 players, a click on it leaves the sign-in (as player 1's B on his list)"
                       : "as before (with 3 / 4 players hidden; a click does nothing)");
    Log("[splitmenu] 21j-r28: [Splitscreen] SignInSoundFix=%d - %s", g_soundFixIni ? 1 : 0,
        g_soundFixIni ? "after the sign-in's profile list and its launch's profile loading (each restarts the sound engine on PC) the "
                        "shell's sounds, music and movie streams are put back with the game's own restore, the music started again"
                      : "the sign-in's profile switches leave the shell's sound off (as before: a silent sign-in, no music after it)");
}

// 21h-r1: THE PAUSE SCAN's Lua side (log only): run while the game makes ifs_pausemenu2. Logs the two pause screens' tables as built,
// the screen sizes the scripts see, what a third / fourth pause screen would need, and wraps both screens' Enter (after the game's
// own) to log them at each pause. Lua 4.0 (the game's): no upvalues used, every helper a global named bfxp_*. 21j-r16: the wrapper
// calls the scan's own Enter kept on the screen (bfxpScanEnter), not the global bfxp_Enter by name - r15's player 2 script had
// redefined that name, and the wrappers then ran the game's Enter over and over (the pause menu noise).
const char kPauseProbe[] = R"BFXLUA(-- BF1GameExt pause scan (21h-r1, log only)
function bfxp_Log(s)
	ScriptCB_GetPlatform("BFXP: " .. s)
end
function bfxp_Buttons(t)
	if (type(t) ~= "table") then
		return "no table"
	end
	local b = t.buttons
	if (type(b) ~= "table") then
		return "no buttons"
	end
	local s = format("buttons x %s y %s, ScreenRelative %s %s, rotY %s", tostring(b.x), tostring(b.y),
		tostring(b.ScreenRelativeX), tostring(b.ScreenRelativeY), tostring(b.rotY))
	local lay = ifspausemenu_split_vbutton_layout
	if ((type(lay) == "table") and (type(lay.buttonlist) == "table")) then
		local i = 1
		while (lay.buttonlist[i]) do
			local tag = lay.buttonlist[i].tag
			local o = b[tag]
			if (type(o) == "table") then
				local h = "shown"
				if (o.hidden) then
					h = "hidden"
				end
				s = s .. format("; %s y %s %s", tostring(tag), tostring(o.y), h)
			end
			i = i + 1
		end
	end
	return s
end
function bfxp_Screen(name, t, why)
	if (type(t) ~= "table") then
		bfxp_Log(format("%s (%s): no Lua table", name, why))
		return
	end
	bfxp_Log(format("%s (%s): Viewport %s, CurButton %s, ScreenName %s, %s", name, why, tostring(t.Viewport),
		tostring(t.CurButton), tostring(t.ScreenName), bfxp_Buttons(t)))
end
function bfxp_Enter(this, bFwd)
	local vp = "?"
	if (ScriptCB_GetPausingViewport) then
		vp = tostring(ScriptCB_GetPausingViewport())
	end
	bfxp_Screen(tostring(this.bfxpName), this, format("Enter %s, pausing viewport %s, hilite 1 %s, hilite 2 %s", tostring(bFwd), vp,
		tostring(gCurHiliteButton ~= nil), tostring(gCurHiliteButton2 ~= nil)))
end
function bfxp_Wrap(t, name)
	if ((type(t) ~= "table") or t.bfxpWrapped or (type(t.Enter) ~= "function")) then
		return
	end
	t.bfxpWrapped = 1
	t.bfxpName = name
	t.bfxpOrigEnter = t.Enter
	t.bfxpScanEnter = bfxp_Enter -- 21j-r16: its own, kept on the screen (never another script's function of that name)
	t.Enter = function(this, bFwd)
		this.bfxpOrigEnter(this, bFwd)
		this.bfxpScanEnter(this, bFwd)
	end
end
local sw, sh = ScriptCB_GetScreenInfo()
local fw, fh = ScriptCB_GetSafeScreenInfo()
bfxp_Log(format("the scripts see: platform %s, split %s, screen %s x %s, safe %s x %s, gScrn %s x %s, gSafe %s x %s",
	tostring(gPlatformStr), tostring(ScriptCB_IsSplitscreen()), tostring(sw), tostring(sh), tostring(fw), tostring(fh),
	tostring(gScrnW), tostring(gScrnH), tostring(gSafeW), tostring(gSafeH)))
bfxp_Log(format("a third / fourth screen: ifs_pausemenu3 %s, ifs_pausemenu4 %s, NewIFShellScreen2 %s, gIFShellScreenTemplate2 %s, " ..
	"gIFScreenTemplate2 %s, gCurHiliteButton3 %s, ScriptCB_GetNumCameras %s, ScriptCB_SetIFScreenViewport %s",
	type(ifs_pausemenu3), type(ifs_pausemenu4), type(NewIFShellScreen2), type(gIFShellScreenTemplate2), type(gIFScreenTemplate2),
	type(gCurHiliteButton3), type(ScriptCB_GetNumCameras), type(ScriptCB_SetIFScreenViewport)))
if (type(ifspausemenu_split_vbutton_layout) == "table") then
	bfxp_Log(format("the split layout: width %s, ySpacing %s, font %s", tostring(ifspausemenu_split_vbutton_layout.width),
		tostring(ifspausemenu_split_vbutton_layout.ySpacing), tostring(ifspausemenu_split_vbutton_layout.font)))
end
bfxp_Screen("ifs_pausemenu", ifs_pausemenu, "as built")
bfxp_Screen("ifs_pausemenu2", ifs_pausemenu2, "as built")
bfxp_Wrap(ifs_pausemenu, "ifs_pausemenu")
bfxp_Wrap(ifs_pausemenu2, "ifs_pausemenu2")
)BFXLUA";

void PauseProbeRun(void* L)
{
    ++g_pauseProbes;
    const int r = RunLua(L, kPauseProbe, sizeof(kPauseProbe) - 1, "=BF1GameExt_pause34", true);
    if (g_pauseLuaLines < kPauseLuaMaxLines)
    {
        ++g_pauseLuaLines;
        Log("[pause34] lua %lu ms: the pause scan's script %s (the game making ifs_pausemenu2, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", g_pauseProbes);
    }
}

// 21h-r4: THE OPTIONS PAGES SCAN (log only, [Splitscreen] OptionsScan34=1, [optscan] lines): the game's GAME OPTIONS
// (ifs_opt_general) and gamepad (ifs_opt_controller) pages' Lua tables as the game builds them (before ScriptCB_AddIFScreen
// makes their elements: every element's place, size, font, texture, text, scale) and their data tables - for the next round's
// copies of player 1's pages in players 3 / 4's quarters. Once each per session, only with players 3 / 4 installed.
const char kOptScan[] = R"BFXLUA(-- BF1GameExt options pages scan (21h-r4, log only)
function bfxd_Log(s)
	ScriptCB_GetPlatform("BFXD: " .. s)
end
function bfxd_Val(v)
	local t = type(v)
	if (t == "number") then
		if (v == floor(v)) then
			return format("%d", v)
		end
		return format("%.3f", v)
	elseif (t == "string") then
		return "\"" .. v .. "\""
	end
	return nil
end
function bfxd_Less(a, b)
	return tostring(a) < tostring(b)
end
function bfxd_Walk(path, t, depth, seen)
	if (seen[t]) then
		bfxd_Log(path .. ": (the same table as " .. seen[t] .. ")")
		return
	end
	seen[t] = path
	local keys = {}
	local k, v
	for k, v in t do
		tinsert(keys, k)
	end
	sort(keys, bfxd_Less)
	local line = ""
	local kids = {}
	local fns = ""
	local i = 1
	while (keys[i]) do
		local key = keys[i]
		local val = t[key]
		if (type(val) == "table") then
			tinsert(kids, key)
		elseif (type(val) == "function") then
			fns = fns .. " " .. tostring(key)
		else
			local sv = bfxd_Val(val)
			if (sv) then
				line = line .. " " .. tostring(key) .. "=" .. sv
				if (strlen(line) > 640) then
					bfxd_Log(path .. ":" .. line .. " (more)")
					line = ""
				end
			end
		end
		i = i + 1
	end
	if (strlen(fns) > 0) then
		if (strlen(fns) > 120) then
			fns = strsub(fns, 1, 120) .. " ..."
		end
		line = line .. " | functions:" .. fns
	end
	bfxd_Log(path .. ":" .. line)
	if (depth >= 9) then
		return
	end
	i = 1
	while (kids[i]) do
		bfxd_Walk(path .. "." .. tostring(kids[i]), t[kids[i]], depth + 1, seen)
		i = i + 1
	end
end
function bfxd_Dump(name)
	local t = getglobal(name)
	if (type(t) ~= "table") then
		bfxd_Log(name .. ": not a table")
		return
	end
	bfxd_Walk(name, t, 0, {})
end
local a, b, c, d = ScriptCB_GetScreenInfo()
bfxd_Log(format("%s (as the game builds it): ScriptCB_GetScreenInfo %s %s %s %s", gBFXD_Screen, tostring(a), tostring(b),
	tostring(c), tostring(d)))
if (ScriptCB_GetSafeScreenInfo) then
	local e, f, g, h = ScriptCB_GetSafeScreenInfo()
	bfxd_Log(format("ScriptCB_GetSafeScreenInfo %s %s %s %s", tostring(e), tostring(f), tostring(g), tostring(h)))
end
bfxd_Dump(gBFXD_Screen)
if (gBFXD_Screen == "ifs_opt_controller") then
	bfxd_Dump("Platform_btn_map")
	bfxd_Dump("gButtonFunctionStringsGamePad")
	bfxd_Dump("ifs_controller_listbox_layout")
	bfxd_Log(format("gControlMode %s, gPlatformStr %s", tostring(gControlMode), tostring(gPlatformStr)))
else
	bfxd_Dump("ifs_opt_general_vbutton_layout")
end
bfxd_Log(gBFXD_Screen .. ": done")
)BFXLUA";

void OptScanRun(void* L, const char* screen)
{
    char head[96];
    snprintf(head, sizeof(head), "gBFXD_Screen = \"%s\"\n", screen);
    if (RunLua(L, head, strlen(head), "=BF1GameExt optscan settings", true) != 0)
        return;
    const int r = RunLua(L, kOptScan, sizeof(kOptScan) - 1, "=BF1GameExt optscan", true);
    Log("[optscan] %lu ms: %s scanned as the game builds it - %s (%d line(s) so far)", ScanMsSinceAttach(), screen,
        r == 0 ? "done" : "FAILED - the game's Lua error output has the message", g_optScanLines);
}

// 21h-r2: BF1GameExt_pause34.lua with gBFXR_Players (this match's count) first - only with 3 / 4 players and p2spawn_scan.cpp's
// managers installed
void RealPauseRun(void* L)
{
    const int players = PlayerNLocalPlayers();
    const bool ready = P2SpawnRealPauseReady();
    if (players < 3 || !ready)
    {
        if (g_rpLuaLines < kRpLuaMaxLines)
        {
            ++g_rpLuaLines;
            Log("[rp34] lua %lu ms: no ifs_pausemenu3 / 4 this match (%d players%s)", ScanMsSinceAttach(), players < 3 ? 2 : players,
                ready ? "" : ", the real pause menus are not installed");
        }
        return;
    }
    ++g_rpRuns;
    char head[200];
    snprintf(head, sizeof(head),
             "gBFXR_Players = %d\ngBFXR_P2Sounds = %d\ngBFXR_P2Opts = %d\ngBFXR_PopScale = %d\ngBFXR_Pages = %d\n", players,
             g_p2SoundsIni ? 1 : 0, g_p2OptsIni ? 1 : 0, g_popScaleIni, g_pagesIni ? 1 : 0);
    if (RunLua(L, head, strlen(head), "=BF1GameExt_pause34 settings", true) != 0)
        return;
    const int r = RunLua(L, g_rpSrc, g_rpLen, "=BF1GameExt_pause34", true);
    if (g_rpLuaLines < kRpLuaMaxLines)
    {
        ++g_rpLuaLines;
        Log("[rp34] lua %lu ms: BF1GameExt_pause34.lua %s (the game making ifs_pausemenu2, %d players, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", players, g_rpRuns);
    }
}

// 21h-r11: player 1's pause menu on the stats screen - no RESPAWN there, in every match (1 / 2 / 3 / 4 players). The small
// script below wraps the game's ifs_pausemenu.Enter while the game makes the screen (ScriptCB_AddIFScreen("ifs_pausemenu"));
// it asks "rp statspause" (p2spawn_scan.cpp: 1 while that pause is up) each time the menu opens. Its lines are [rp34] lua lines.
const char kStatsPauseLua[] = R"BFXLUA(-- BF1GameExt stats pause (21h-r11 / r12 / r13): no RESPAWN (r12: and no OPTIONS) in player 1's pause menu while it is up over the
-- stats screen at the match end ("rp statspause" = 1 then). r12: QUIT -> YES there goes to the main menu: the game's own
-- QuitToShell does nothing once a match is won, so BF1GameExt empties what is left of the mission queue ("rp statsquit") and
-- the script leaves the stats as DONE does (ScriptCB_QuitFromStats, then ScriptCB_Unpause) - with no next mission the game goes
-- to the main menu. r13: RESTART MISSION -> YES there plays the same map again: the game's RestartMission does nothing
-- after a win either, so BF1GameExt points the queue back at the map just played ("rp statsrestart") and the stats are left
-- the same way - the game loads that map as its next mission. Run by BF1GameExt with the game's Lua while the game makes ifs_pausemenu (every match: 1,
-- 2, 3 or 4 players). The game's Enter runs first, then RESPAWN is hidden and the list laid out again with the layout the
-- game's Enter used; any other time RESPAWN is shown again. Lua 4.0: no upvalues, the original Enter kept on the screen.
function bfxs_Log(s)
	ScriptCB_GetPlatform("BFXR: " .. s)
end

function bfxs_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

function bfxs_Enter(this, bFwd)
	this.bfxsOrigEnter(this, bFwd)
	if ((not this.buttons) or (not this.buttons.suicide)) then
		return
	end
	local hide = (bfxs_Ask("rp statspause") == 1)
	if (hide) then
		this.buttons.suicide.hidden = 1
		if (this.buttons.opts) then
			this.buttons.opts.hidden = 1
		end
		this.bfxsNoSuicide = 1
	elseif (this.bfxsNoSuicide) then
		this.bfxsNoSuicide = nil
		this.buttons.suicide.hidden = nil
		if (this.buttons.opts) then
			this.buttons.opts.hidden = nil
		end
	else
		return
	end
	local layout = ifspausemenu_split_vbutton_layout
	if ((not ScriptCB_IsSplitscreen()) or (not layout)) then
		layout = ifspausemenu_vbutton_layout
	end
	if (not layout) then
		IFObj_fnSetVis(this.buttons.suicide, not hide)
		if (this.buttons.opts) then
			IFObj_fnSetVis(this.buttons.opts, not hide)
		end
		return
	end
	local cur = this.CurButton
	if (cur and this.buttons[cur]) then
		IFButton_fnSelect(this.buttons[cur], nil)
	end
	local first = ShowHideVerticalButtons(this.buttons, layout)
	if ((not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(this.CurButton)
	gBFXS_Logs = (gBFXS_Logs or 0) + 1
	if (gBFXS_Logs <= 6) then
		if (hide) then
			bfxs_Log(format("player 1's pause menu over the stats: RESPAWN and OPTIONS hidden (first item %s)", tostring(this.CurButton)))
		else
			bfxs_Log("player 1's pause menu: RESPAWN and OPTIONS shown again")
		end
	end
end

-- r12: QUIT -> YES while the pause menu is over the stats: the main menu (any other time the game's own)
function bfxs_QuitDone(bResult)
	if (bResult and (bfxs_Ask("rp statspause") == 1)) then
		local r = bfxs_Ask("rp statsquit")
		if (r == 1) then
			Popup_YesNo.fnDone = nil
			if (ifs_pausemenu) then
				ifelm_shellscreen_fnPlaySound(ifs_pausemenu.acceptSound)
			end
			bfxs_Log("QUIT on the stats screen: no next mission - leaving the stats for the main menu (QuitFromStats, Unpause)")
			ScriptCB_QuitFromStats()
			ScriptCB_Unpause()
			return
		end
		bfxs_Log(format("QUIT on the stats screen: the mission queue could not be changed (%s) - the game's own QUIT", tostring(r)))
	end
	gBFXS_OrigQuitDone(bResult)
end

-- r13: RESTART MISSION -> YES while the pause menu is over the stats: the same map again (any other time the game's own)
function bfxs_RestartDone(bResult)
	if (bResult and (bfxs_Ask("rp statspause") == 1)) then
		local r = bfxs_Ask("rp statsrestart")
		if (r == 1) then
			Popup_YesNo.fnDone = nil
			if (ifs_pausemenu) then
				ifelm_shellscreen_fnPlaySound(ifs_pausemenu.acceptSound)
			end
			bfxs_Log("RESTART MISSION on the stats screen: the same map next - leaving the stats (QuitFromStats, Unpause)")
			ScriptCB_QuitFromStats()
			ScriptCB_Unpause()
			return
		end
		bfxs_Log(format("RESTART MISSION on the stats screen: the mission queue could not be changed (%s) - the game's own RESTART", tostring(r)))
	end
	gBFXS_OrigRestartDone(bResult)
end

if (ifs_pausemenu_fnRestartPopupDone and (ifs_pausemenu_fnRestartPopupDone ~= gBFXS_RestartWrapper)) then
	gBFXS_OrigRestartDone = ifs_pausemenu_fnRestartPopupDone
	gBFXS_RestartWrapper = bfxs_RestartDone
	ifs_pausemenu_fnRestartPopupDone = bfxs_RestartDone
	bfxs_Log("player 1's pause menu: RESTART MISSION over the stats screen plays the same map again")
end

if (ifs_pausemenu_fnQuitPopupDone and (ifs_pausemenu_fnQuitPopupDone ~= gBFXS_QuitWrapper)) then
	gBFXS_OrigQuitDone = ifs_pausemenu_fnQuitPopupDone
	gBFXS_QuitWrapper = bfxs_QuitDone
	ifs_pausemenu_fnQuitPopupDone = bfxs_QuitDone
	bfxs_Log("player 1's pause menu: QUIT over the stats screen goes to the main menu")
end

if ((not ifs_pausemenu) or (not ifs_pausemenu.Enter)) then
	bfxs_Log("player 1's pause menu: ifs_pausemenu is not there - RESPAWN not hidden over the stats")
elseif (ifs_pausemenu.bfxsOrigEnter) then
	bfxs_Log("player 1's pause menu: already changed")
else
	ifs_pausemenu.bfxsOrigEnter = ifs_pausemenu.Enter
	ifs_pausemenu.Enter = bfxs_Enter
	gBFXS_Logs = 0
	if (ScriptCB_IsSplitscreen()) then
		bfxs_Log("player 1's pause menu (split screen): no RESPAWN / OPTIONS while it is over the stats screen")
	else
		bfxs_Log("player 1's pause menu (1 player): no RESPAWN / OPTIONS while it is over the stats screen")
	end
end
)BFXLUA";

unsigned long g_statsLuaRuns = 0;

void StatsPauseLuaRun(void* L)
{
    ++g_statsLuaRuns;
    const int r = RunLua(L, kStatsPauseLua, sizeof(kStatsPauseLua) - 1, "=BF1GameExt stats pause", true);
    if (g_rpLuaLines < kRpLuaMaxLines)
    {
        ++g_rpLuaLines;
        Log("[rp34] lua %lu ms: the stats pause script %s (the game making ifs_pausemenu, %d local player(s) in the game's count, run %lu)",
            ScanMsSinceAttach(), r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", PlayerNLocalPlayers(),
            g_statsLuaRuns);
    }
}

// 21j-r23: FREE CAMERA / FAKE CONSOLE in player 1's pause menu ([Splitscreen] PauseFreeCam / PauseConsole) - the game's own
// items, hidden by its pause menu because the 2020 exe says it is a final build (gFinalBuild). Run while the game makes
// ifs_pausemenu and ifs_fakeconsole, after a settings chunk (gBFXK_Cam / gBFXK_Con). [freecam] lua lines ("BFXK: ").
// 21j-r24: FAKE CONSOLE right under FREE CAMERA; a command picked is switched by BF1GameExt ("BFXQ: con! NAME",
// FakeConsoleQuery) - never by the game's ScriptCB_DoConsoleCmd, whose print crashed the 2020 game; the rows show ON / OFF.
const char kPauseExtrasLua[] = R"BFXLUA(-- BF1GameExt FREE CAMERA / FAKE CONSOLE (21j-r23 / r24): player 1's pause menu gets the game's own FREE CAMERA (CHASE CAMERA
-- while it is on) and FAKE CONSOLE items back. The 2020 exe tells its scripts it is a final build (gFinalBuild) and the pause
-- menu's Enter hides both then; their code is all in the exe (ScriptCB_Freecamera: player 1's camera; ScriptCB_GetConsoleCmds:
-- the exe's "debugmenu" list). Set by BF1GameExt before this ([Splitscreen] PauseFreeCam / PauseConsole): gBFXK_Cam /
-- gBFXK_Con - 0 off, 1 single player, 2 player 1's pause menu in split screen too. Never in an online game, never over the
-- stats screen at a match end. r24: FAKE CONSOLE right under FREE CAMERA; a command picked is switched by BF1GameExt ("BFXQ:
-- con! NAME") - the game's ScriptCB_DoConsoleCmd switched it and then printed "NAME = 1" to the exe's debug console, which the
-- 2020 exe never makes: that print crashed the game. The list shows each one ON / OFF ("BFXQ: con NAME"). Run while the game
-- makes ifs_pausemenu and ifs_fakeconsole (each part once per screen table; the originals kept on the screen - Lua 4.0, no
-- upvalues). [freecam] lua lines ("BFXK: ").
function bfxk_Log(s)
	ScriptCB_GetPlatform("BFXK: " .. s)
end

function bfxk_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

-- 0 off, 1 single player, 2 single player and player 1 in split screen; never online
function bfxk_Want(v)
	if ((not v) or (v < 1)) then
		return nil
	end
	if (ScriptCB_InNetGame()) then
		return nil
	end
	if (ScriptCB_IsSplitscreen() and (v < 2)) then
		return nil
	end
	return 1
end

-- r24: FAKE CONSOLE right under FREE CAMERA (the game's list has OPTIONS between them). The layout's list is put in that order
-- (it sets the positions) and the buttons' up / down links are set from it, as the game sets them when it makes the buttons.
-- Only while the menu shows both: single player's layout (ifspausemenu_vbutton_layout) is player 1's alone; the split one
-- (PauseFreeCam / PauseConsole 2) is player 2's too, but his pause menu never shows FAKE CONSOLE - nothing of his moves.
function bfxk_Order(layout, b)
	local l = layout.buttonlist
	if (not l) then
		return nil
	end
	local n = getn(l)
	local i
	local fc = nil
	local co = nil
	for i = 1, n do
		if (l[i].tag == "freecam") then
			fc = i
		elseif (l[i].tag == "console") then
			co = i
		end
	end
	if ((not fc) or (not co)) then
		return nil
	end
	local moved = nil
	if (co ~= fc + 1) then
		local e = tremove(l, co)
		if (co < fc) then
			fc = fc - 1
		end
		tinsert(l, fc + 1, e)
		moved = 1
	end
	-- the links, over the buttons there are
	local tags = {}
	for i = 1, n do
		if (b[l[i].tag]) then
			tinsert(tags, l[i].tag)
		end
	end
	local m = getn(tags)
	for i = 1, m do
		local up = i - 1
		local dn = i + 1
		if (up < 1) then
			up = m
		end
		if (dn > m) then
			dn = 1
		end
		b[tags[i]].link_up = tags[up]
		b[tags[i]].link_down = tags[dn]
	end
	if (moved) then
		bfxk_Log(format("pause menu: FAKE CONSOLE moved right under FREE CAMERA (%d items linked)", m))
	end
	return 1
end

-- after the game's Enter (it hides both items: gFinalBuild)
function bfxk_Enter(this, bFwd)
	this.bfxkOrigEnter(this, bFwd)
	local b = this.buttons
	if (not b) then
		return
	end
	local stats = (bfxk_Ask("rp statspause") == 1)
	local cam = b.freecam and (not stats) and bfxk_Want(gBFXK_Cam)
	local con = b.console and ifs_fakeconsole and (not stats) and bfxk_Want(gBFXK_Con)
	local changed = nil
	if (cam and b.freecam.hidden) then
		b.freecam.hidden = nil
		changed = 1
	end
	if (con and b.console.hidden) then
		b.console.hidden = nil
		-- the game's label is the text key "Fake Console" (no such text in the game)
		RoundIFButtonLabel_fnSetUString(b.console, ScriptCB_tounicode("FAKE CONSOLE"))
		changed = 1
	end
	if (not changed) then
		return
	end
	local layout = ifspausemenu_vbutton_layout
	if (ScriptCB_IsSplitscreen() and ifspausemenu_split_vbutton_layout) then
		layout = ifspausemenu_split_vbutton_layout
	end
	if (cam and con) then
		bfxk_Order(layout, b)
	end
	local cur = this.CurButton
	if (cur and b[cur]) then
		IFButton_fnSelect(b[cur], nil)
	end
	local first = ShowHideVerticalButtons(b, layout)
	if ((not cur) or (not b[cur]) or b[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(this.CurButton)
	gBFXK_Logs = (gBFXK_Logs or 0) + 1
	if (gBFXK_Logs <= 6) then
		local what = ""
		if (cam) then
			what = "FREE CAMERA"
		end
		if (con) then
			if (what ~= "") then
				what = what .. " and "
			end
			what = what .. "FAKE CONSOLE"
		end
		local where = "single player"
		if (ScriptCB_IsSplitscreen()) then
			where = "split screen, player 1's"
		end
		bfxk_Log(format("pause menu (%s): %s shown (first item %s)", where, what, tostring(this.CurButton)))
	end
end

-- the items picked: what the game did
function bfxk_Accept(this)
	local cur = this.CurButton
	this.bfxkOrigAccept(this)
	if (cur == "freecam") then
		if (this.bChaseCam) then
			bfxk_Log("FREE CAMERA picked - player 1's free camera ON (RESUME GAME to fly it; the item says CHASE CAMERA now)")
		else
			bfxk_Log("CHASE CAMERA picked - player 1's own camera again")
		end
	elseif (cur == "console") then
		local n = 0
		if (gConsoleCmdList) then
			n = getn(gConsoleCmdList)
		end
		bfxk_Log(format("FAKE CONSOLE picked - %d command(s)", n))
	end
end

-- r24: a command picked: BF1GameExt switches the debug flag behind it (1 / 0 back = its new value; -1 = not one it knows:
-- nothing done). In place of the game's ScriptCB_DoConsoleCmd while the console's Input_Accept runs - never the game's
-- (it would print to the debug console the 2020 exe never makes, and crash).
function bfxk_DoCmd(name)
	local r = bfxk_Ask("con! " .. tostring(name))
	gBFXK_Did = name
	if ((r == 0) or (r == 1)) then
		bfxk_Log(format("FAKE CONSOLE: %s now %s", tostring(name), (r == 1 and "ON") or "OFF"))
	else
		bfxk_Log(format("FAKE CONSOLE: %s NOT run - BF1GameExt does not know it (%s)", tostring(name), tostring(r)))
	end
end

-- r24: each list row with its state: NAME   ON / OFF (the rows BF1GameExt knows)
function bfxk_Populate(Dest, Data)
	gBFXK_OrigPopulate(Dest, Data)
	if (Data and Data.ShowStr and (not gBlankListbox) and Dest and Dest.showstr) then
		local v = bfxk_Ask("con " .. tostring(Data.ShowStr))
		if ((v == 0) or (v == 1)) then
			IFText_fnSetUString(Dest.showstr, ScriptCB_tounicode(format("%s   %s", Data.ShowStr, (v == 1 and "ON") or "OFF")))
		end
	end
end

if (ifs_pausemenu and ifs_pausemenu.Enter and ifs_pausemenu.Input_Accept and (not ifs_pausemenu.bfxk)) then
	ifs_pausemenu.bfxk = 1
	ifs_pausemenu.bfxkOrigEnter = ifs_pausemenu.Enter
	ifs_pausemenu.bfxkOrigAccept = ifs_pausemenu.Input_Accept
	ifs_pausemenu.Enter = bfxk_Enter
	ifs_pausemenu.Input_Accept = bfxk_Accept
	bfxk_Log(format("ifs_pausemenu: Enter / Input_Accept wrapped (FREE CAMERA %d, FAKE CONSOLE %d - 0 off, 1 single player, "
		.. "2 split screen too)", gBFXK_Cam or 0, gBFXK_Con or 0))
end

-- the fake console: its commands, which one is picked, each one's state
if (ifs_fakeconsole and ifs_fakeconsole.Enter and ifs_fakeconsole.Input_Accept and (not ifs_fakeconsole.bfxk)) then
	ifs_fakeconsole.bfxk = 1
	ifs_fakeconsole.bfxkOrigEnter = ifs_fakeconsole.Enter
	ifs_fakeconsole.bfxkOrigAccept = ifs_fakeconsole.Input_Accept
	if (fakeconsole_listbox_layout and fakeconsole_listbox_layout.PopulateFn and (not fakeconsole_listbox_layout.bfxk)) then
		fakeconsole_listbox_layout.bfxk = 1
		gBFXK_OrigPopulate = fakeconsole_listbox_layout.PopulateFn
		fakeconsole_listbox_layout.PopulateFn = bfxk_Populate
	end
	ifs_fakeconsole.Enter = function(this, bFwd)
		this.bfxkOrigEnter(this, bFwd)
		if (not gBFXK_ListLogged) then
			gBFXK_ListLogged = 1
			local s = ""
			local n = 0
			local i
			if (gConsoleCmdList) then
				n = getn(gConsoleCmdList)
				for i = 1, n do
					local e = gConsoleCmdList[i]
					if (e and e.ShowStr) then
						if (s ~= "") then
							s = s .. ", "
						end
						s = s .. tostring(e.ShowStr) .. " " .. tostring(bfxk_Ask("con " .. tostring(e.ShowStr)))
					end
				end
			end
			bfxk_Log(format("FAKE CONSOLE: %d command(s) (1 on, 0 off, -1 not known): %s", n, s))
		end
	end
	ifs_fakeconsole.Input_Accept = function(this)
		local orig = ScriptCB_DoConsoleCmd
		gBFXK_Did = nil
		ScriptCB_DoConsoleCmd = bfxk_DoCmd
		this.bfxkOrigAccept(this)
		ScriptCB_DoConsoleCmd = orig
	end
	bfxk_Log("ifs_fakeconsole: Enter / Input_Accept wrapped (a command picked is switched by BF1GameExt, the rows show ON / OFF)")
end
)BFXLUA";

bool FcBudget()
{
    if (g_fcLines >= kFcMaxLines)
        return false;
    ++g_fcLines;
    if (g_fcLines == kFcMaxLines)
        Log("[freecam] line limit reached (%d), going quiet", kFcMaxLines);
    return true;
}

void PauseExtrasRun(void* L, const char* screen)
{
    ++g_pxRuns;
    char head[64];
    snprintf(head, sizeof(head), "gBFXK_Cam = %d\ngBFXK_Con = %d\n", g_pauseCamIni, g_pauseConIni);
    if (RunLua(L, head, strlen(head), "=BF1GameExt freecam settings", true) != 0)
    {
        if (FcBudget())
            Log("[freecam] %lu ms: the settings chunk FAILED - script not run (the game making %s)", ScanMsSinceAttach(), screen);
        return;
    }
    const int r = RunLua(L, kPauseExtrasLua, sizeof(kPauseExtrasLua) - 1, "=BF1GameExt freecam", true);
    if (FcBudget())
        Log("[freecam] %lu ms: the FREE CAMERA / FAKE CONSOLE script %s (the game making %s, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", screen, g_pxRuns);
}

// 21j-r15: PLAYER 2'S PAUSE MENU AS PLAYER 1'S IN THE 2-PLAYER SPLIT ([Splitscreen] PauseLikeP1P2=1, p2spawn_scan.cpp's
// P2SpawnP2PauseLike) - run while the game makes ifs_pausemenu2 in a 2-player match (with 3 / 4 players BF1GameExt_pause34.lua
// does the same for player 2: OptionsP2, PauseSoundsP2). [rp34] lua lines.
const char kP2PauseLua[] = R"BFXLUA(-- BF1GameExt player 2's pause menu as player 1's (21j-r15: the 2-player split, horizontal and vertical - [Splitscreen]
-- PauseLikeP1P2=1). Run by BF1GameExt with the game's Lua while the game makes ifs_pausemenu2 in a 2-player match. (1) Player 1's
-- accept / exit / cancel / error sounds (the game's ifs_pausemenu2 has none). (2) Each time it opens, its buttons container moves
-- to stand where player 1's stands in his half ("rp p2dx / p2dy": BF1GameExt reads both halves' HUD roots - their safe areas
-- differ: player 1's half 10% at its outer edge, player 2's at his). (3) His OPTIONS (ifs_opt_contmain while his pause is the
-- one on - the pausing viewport 1): no VIDEO OPTIONS (player 1's OPTIONS keep them: the screen is shared), and CONTROL OPTIONS
-- goes straight to the gamepad page (ifs_opt_controller), never the keyboard page - as BF1GameExt_pause34.lua does for him with
-- 3 / 4 players (the same globals: one wrap per Lua state; the game's own Enter shows VIDEO OPTIONS again for player 1 each
-- time). His option screens and popups are drawn in a 16:9 box of his half by BF1GameExt's menu layer. Lua 4.0: no upvalues,
-- every helper a global bfx2_* (21j-r16: r15 named them bfxp_*, the pause scan's prefix - its bfxp_Enter replaced the scan's and
-- every pause menu Enter ran the game's Enter until the stack overflowed: the noise); the game's Enter kept on the screen itself.
function bfx2_Log(s)
	ScriptCB_GetPlatform("BFXR: " .. s)
end

function bfx2_Ask(what)
	local r = ScriptCB_GetPlatform("BFXQ: " .. what)
	if (type(r) == "number") then
		return r
	end
	return -1
end

if (ifs_pausemenu2) then
	local s = ifs_pausemenu2
	s.acceptSound = s.acceptSound or "shell_menu_accept"
	s.exitSound = s.exitSound or "shell_menu_exit"
	s.cancelSound = s.cancelSound or "shell_menu_cancel"
	s.errorSound = s.errorSound or "shell_menu_error"
	bfx2_Log("ifs_pausemenu2 (2-player split): player 1's accept / exit / cancel / error sounds set")
end

function bfx2_Place(this)
	if ((not this.buttons) or (not gBFX2_X0)) then
		return
	end
	local dx = bfx2_Ask(format("rp p2dx %d", gBFX2_AX))
	local dy = bfx2_Ask(format("rp p2dy %d", gBFX2_AY))
	if ((dx < 1) or (dy < 1)) then
		return
	end
	IFObj_fnSetPos(this.buttons, gBFX2_X0 + dx - 1000, gBFX2_Y0 + dy - 1000)
	if (not gBFX2_PlaceLogged) then
		gBFX2_PlaceLogged = 1
		bfx2_Log(format("player 2's pause menu at %d %d in his root (the game's %d %d): where player 1's stands in his half",
			gBFX2_X0 + dx - 1000, gBFX2_Y0 + dy - 1000, gBFX2_X0, gBFX2_Y0))
	end
end

-- player 2's pause menu only: the Enter it had (the game's, or the pause scan's wrapper of it) kept on it, then his menu placed
function bfx2_Enter(this, bFwd)
	if ((this ~= ifs_pausemenu2) or (type(this.bfx2OrigEnter) ~= "function")) then
		return
	end
	this.bfx2OrigEnter(this, bFwd)
	bfx2_Place(this)
end

if (ifs_pausemenu2 and ifs_pausemenu2.Enter and ifs_pausemenu2.buttons and (not ifs_pausemenu2.bfx2OrigEnter) and
		(ifs_pausemenu2.Enter ~= bfx2_Enter)) then
	ifs_pausemenu2.bfx2OrigEnter = ifs_pausemenu2.Enter
	gBFX2_X0 = ifs_pausemenu2.buttons.x or 0
	gBFX2_Y0 = ifs_pausemenu2.buttons.y or 0
	gBFX2_AX = floor((ifs_pausemenu2.buttons.ScreenRelativeX or 0.5) * 1000 + 0.5)
	gBFX2_AY = floor((ifs_pausemenu2.buttons.ScreenRelativeY or 0.5) * 1000 + 0.5)
	ifs_pausemenu2.Enter = bfx2_Enter
	bfx2_Log(format("ifs_pausemenu2 (2-player split): its menu (at %d %d, anchored %d %d / 1000) goes where player 1's stands in his half",
		gBFX2_X0, gBFX2_Y0, gBFX2_AX, gBFX2_AY))
end

-- his OPTIONS while his pause is the one on (not in the shell)
function bfx2_IsP2()
	if (ScriptCB_GetShellActive and ScriptCB_GetShellActive()) then
		return nil
	end
	return ScriptCB_GetPausingViewport() == 1
end

function bfx2_OptsEnter(this, bFwd)
	gBFXR_OrigOptsEnter(this, bFwd)
	if (bfx2_IsP2() and this.buttons and this.buttons.pcvideo and ifs_opt_top_vbutton_layout) then
		this.buttons.pcvideo.hidden = 1
		local cur = this.CurButton
		local first = ShowHideVerticalButtons(this.buttons, ifs_opt_top_vbutton_layout)
		if ((not cur) or (not this.buttons[cur]) or this.buttons[cur].hidden) then
			cur = first
		end
		this.CurButton = cur
		SetCurButton(this.CurButton)
		if (not gBFX2_OptsLogged) then
			gBFX2_OptsLogged = 1
			bfx2_Log(format("player 2's OPTIONS (2-player split): VIDEO OPTIONS hidden (first item %s)", tostring(this.CurButton)))
		end
	end
end

function bfx2_OptsAccept(this)
	if (bfx2_IsP2() and (this.CurButton == "pccontrols")) then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfx2_Log("player 2's CONTROL OPTIONS (2-player split): straight to the gamepad page (ifs_opt_controller)")
		ScriptCB_PushScreen("ifs_opt_controller")
		return
	end
	gBFXR_OrigOptsAccept(this)
end

if (gBFXR_P2OptsWrapped) then
	bfx2_Log("player 2's OPTIONS: already wrapped in this Lua state - left as they are")
elseif ((not ifs_opt_contmain) or (not ifs_opt_contmain.Enter) or (not ifs_opt_contmain.Input_Accept)) then
	bfx2_Log("player 2's OPTIONS: ifs_opt_contmain is not made yet - not changed")
else
	gBFXR_P2OptsWrapped = 1
	gBFXR_OrigOptsEnter = ifs_opt_contmain.Enter
	gBFXR_OrigOptsAccept = ifs_opt_contmain.Input_Accept
	ifs_opt_contmain.Enter = bfx2_OptsEnter
	ifs_opt_contmain.Input_Accept = bfx2_OptsAccept
	bfx2_Log("player 2's OPTIONS (2-player split): gamepad only (no VIDEO OPTIONS; CONTROL OPTIONS -> the gamepad page)")
end
)BFXLUA";

unsigned long g_p2PauseRuns = 0;

void P2PauseLuaRun(void* L)
{
    ++g_p2PauseRuns;
    const int r = RunLua(L, kP2PauseLua, sizeof(kP2PauseLua) - 1, "=BF1GameExt p2pause", true);
    if (g_rpLuaLines < kRpLuaMaxLines)
    {
        ++g_rpLuaLines;
        Log("[rp34] lua %lu ms: 21j-r15 / r16: player 2's pause menu script (bfx2_*) %s (the game making ifs_pausemenu2 in a "
            "2-player match, run %lu)",
            ScanMsSinceAttach(), r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", g_p2PauseRuns);
    }
}

// 21i-r6: Instant Action's GAME OPTIONS page ([Splitscreen] IAGameOptions=1) - run while the game makes ifs_missionselect
// (the X button, the page right after the map list is finished) and ifs_opt_general (its HEROES row greyed out); gBFXO_Now
// is the screen. [iaopts] lua lines; its questions are "BFXQ: iaopt ..." (SplitQuery -> IaQuery).
const char kIAOptsLua[] = R"BFXLUA(-- BF1GameExt Instant Action GAME OPTIONS (21i-r6 / r7): X on the Instant Action map list (ifs_missionselect - single player and
-- split screen) opens GAME OPTIONS, made as the multiplayer SET GAME OPTIONS page: AI UNITS PER TEAM, REINFORCEMENTS (both
-- MAP DEFAULT by default), HEROES, RESET TO DEFAULTS. Left / right change a value (left from the lowest -> MAP DEFAULT),
-- the triggers by 10 units / 5 steps. Saved for player 1's profile by BF1GameExt (BF1GameExt_IAOptions.ini, "BFXQ: iaopt"),
-- which puts the numbers in at the map's SetUnitCount / SetReinforcementCount (Instant Action only). 21i-r7: the options
-- menu's GAME OPTIONS page has no HEROES row any more (set here only); HEROES is OFF until set (and after RESET); back
-- from the page the map list's era buttons stay hidden. 21i-r8: the GAME OPTIONS page's SPLIT SCREEN / AUTO AIM rows (below). Run by BF1GameExt while the game makes ifs_missionselect and
-- ifs_opt_general (gBFXO_Now is the screen); [iaopts] lua lines.
function bfxo_Log(s)
	ScriptCB_GetPlatform("BFXI: " .. s)
end

function bfxo_Ask(s)
	return ScriptCB_GetPlatform("BFXQ: " .. s)
end

bfxo_MaxUnits = 32
bfxo_Reinf = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 125, 150, 175, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000 }
gBFXO = gBFXO or { units = -1, reinf = 0, heroes = 0 }

-- a reinforcement count -> its step (the largest step not above it; 0 = MAP DEFAULT)
function bfxo_ReinfIdx(r)
	if ((not r) or (r < 1)) then
		return 0
	end
	local i = 1
	local n = getn(bfxo_Reinf)
	while ((i < n) and (bfxo_Reinf[i + 1] <= r)) do
		i = i + 1
	end
	return i
end

function bfxo_ReinfValue()
	if (gBFXO.reinf < 1) then
		return -1
	end
	return bfxo_Reinf[gBFXO.reinf]
end

function bfxo_HeroesNow()
	if (ScriptCB_AreHeroesOn and ScriptCB_AreHeroesOn()) then
		return 1
	end
	return 0
end

-- player 1's saved numbers (BF1GameExt's answers are value + 1: 0 = MAP DEFAULT / not saved); bApply: HEROES goes into the
-- profile too (21i-r7: OFF when nothing is saved)
function bfxo_Load(bApply)
	local found = bfxo_Ask("iaopt load")
	local u = (bfxo_Ask("iaopt units") or 0) - 1
	local r = (bfxo_Ask("iaopt reinf") or 0) - 1
	local h = (bfxo_Ask("iaopt heroes") or 0) - 1
	if (u > bfxo_MaxUnits) then
		u = bfxo_MaxUnits
	end
	gBFXO.units = u
	gBFXO.reinf = bfxo_ReinfIdx(r)
	if (h ~= 1) then
		h = 0
	end
	gBFXO.heroes = h
	if (bApply and ScriptCB_SetHeroesOn and (h ~= bfxo_HeroesNow())) then
		if (h == 1) then
			ScriptCB_SetHeroesOn(1)
		else
			ScriptCB_SetHeroesOn(nil)
		end
	end
	bfxo_Log(format("player 1's Instant Action options %s: AI units per team %d, reinforcements %d, heroes %d (-1 = MAP DEFAULT)",
		(found == 1) and "loaded" or "not saved yet - the defaults", gBFXO.units, bfxo_ReinfValue(), gBFXO.heroes))
end

function bfxo_Save()
	if (ScriptCB_SetHeroesOn) then
		if (gBFXO.heroes == 1) then
			ScriptCB_SetHeroesOn(1)
		else
			ScriptCB_SetHeroesOn(nil)
		end
	end
	bfxo_Ask(format("iaopt save %d %d %d", gBFXO.units, bfxo_ReinfValue(), gBFXO.heroes))
end

function bfxo_SetText(this)
	local b = this.buttons
	local us = "MAP DEFAULT"
	if (gBFXO.units >= 0) then
		us = format("%d", gBFXO.units)
	end
	local rs = "MAP DEFAULT"
	if (gBFXO.reinf >= 1) then
		rs = format("%d", bfxo_ReinfValue())
	end
	local hs = "OFF"
	if (gBFXO.heroes == 1) then
		hs = "ON"
	end
	RoundIFButtonLabel_fnSetUString(b.units, ScriptCB_tounicode("AI UNITS PER TEAM: " .. us))
	RoundIFButtonLabel_fnSetUString(b.reinf, ScriptCB_tounicode("REINFORCEMENTS: " .. rs))
	RoundIFButtonLabel_fnSetUString(b.hero, ScriptCB_tounicode("HEROES: " .. hs))
	RoundIFButtonLabel_fnSetUString(b.reset, ScriptCB_tounicode("RESET TO DEFAULTS"))
end

-- one step (d = 1 / -1; bBig: the triggers - 10 units / 5 steps, no wrap)
function bfxo_Change(this, d, bBig)
	local c = this.CurButton
	local n = getn(bfxo_Reinf)
	if (c == "units") then
		local u = gBFXO.units
		if (bBig) then
			if (u < 0) then
				u = 0
			end
			u = max(0, min(bfxo_MaxUnits, u + 10 * d))
		elseif (d > 0) then
			if (u < 0) then
				u = 0
			elseif (u >= bfxo_MaxUnits) then
				u = -1
			else
				u = u + 1
			end
		else
			if (u < 0) then
				u = bfxo_MaxUnits
			elseif (u == 0) then
				u = -1
			else
				u = u - 1
			end
		end
		gBFXO.units = u
	elseif (c == "reinf") then
		local i = gBFXO.reinf
		if (bBig) then
			if (i < 1) then
				i = 1
			end
			i = max(1, min(n, i + 5 * d))
		elseif (d > 0) then
			i = i + 1
			if (i > n) then
				i = 0
			end
		else
			i = i - 1
			if (i < 0) then
				i = n
			end
		end
		gBFXO.reinf = i
	elseif (c == "hero") then
		gBFXO.heroes = 1 - gBFXO.heroes
	else
		return nil
	end
	bfxo_Save()
	bfxo_SetText(this)
	return 1
end

function bfxo_Reset(this)
	gBFXO.units = -1
	gBFXO.reinf = 0
	gBFXO.heroes = 0
	bfxo_Save()
	bfxo_SetText(this)
	bfxo_Log("RESET TO DEFAULTS: AI units per team MAP DEFAULT, reinforcements MAP DEFAULT, heroes OFF")
end

-- the page itself (made once the game has finished ifs_missionselect)
function bfxo_Build()
	if (ifs_bfx_iaopts) then
		return
	end
	local w, h = ScriptCB_GetSafeScreenInfo()
	ifs_bfx_iaopts = NewIFShellScreen {
		nologo = 1,
		bg_texture = "iface_bgmeta_space",
		movieIntro = nil,
		movieBackground = nil,
		bNohelptext_backPC = 1,
		buttons = NewIFContainer {
			ScreenRelativeX = 0.7, -- right column onscreen (as SET GAME OPTIONS)
			ScreenRelativeY = 0.5,
			y = 20,
		},
		Enter = function(this, bFwd)
			gIFShellScreenTemplate_fnEnter(this, bFwd)
			if (bFwd) then
				bfxo_Load(nil)
				this.CurButton = ShowHideVerticalButtons(this.buttons, bfxo_Layout)
				SetCurButton(this.CurButton)
			end
			bfxo_SetText(this)
		end,
		Exit = function(this, bFwd)
			if (gCurHiliteButton) then
				IFButton_fnSelect(gCurHiliteButton, nil)
			end
		end,
		Input_Accept = function(this)
			if (gShellScreen_fnDefaultInputAccept(this)) then
				return
			end
			if (this.CurButton == "reset") then
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				bfxo_Reset(this)
			elseif (bfxo_Change(this, 1, nil)) then
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
			end
		end,
		Input_Back = function(this)
			bfxo_Save()
			bfxo_Log(format("GAME OPTIONS closed: AI units per team %d, reinforcements %d, heroes %d (-1 = MAP DEFAULT)",
				gBFXO.units, bfxo_ReinfValue(), gBFXO.heroes))
			gBFXO_Back = 1
			ScriptCB_PopScreen()
		end,
		Input_GeneralUp = function(this)
			if (gShellScreen_fnDefaultInputUp(this)) then
				return
			end
			gDefault_Input_GeneralUp(this)
		end,
		Input_GeneralDown = function(this)
			if (gShellScreen_fnDefaultInputDown(this)) then
				return
			end
			gDefault_Input_GeneralDown(this)
		end,
		Input_GeneralLeft = function(this)
			if (bfxo_Change(this, -1, nil)) then
				ifelm_shellscreen_fnPlaySound(this.selectSound)
			end
		end,
		Input_GeneralRight = function(this)
			if (bfxo_Change(this, 1, nil)) then
				ifelm_shellscreen_fnPlaySound(this.selectSound)
			end
		end,
		Input_LTrigger = function(this)
			if (bfxo_Change(this, -1, 1)) then
				ifelm_shellscreen_fnPlaySound(this.selectSound)
			end
		end,
		Input_RTrigger = function(this)
			if (bfxo_Change(this, 1, 1)) then
				ifelm_shellscreen_fnPlaySound(this.selectSound)
			end
		end,
	}
	bfxo_Layout = {
		ySpacing = 5,
		font = "gamefont_medium",
		width = w * 0.85,
		title = "ifs.GameOpt.title",
		rotY = 40,
		buttonlist = {
			{ tag = "units" },
			{ tag = "reinf" },
			{ tag = "hero" },
			{ tag = "reset" },
		},
	}
	if (ScriptCB_GetLanguage() ~= "english") then
		bfxo_Layout.font = "gamefont_small"
	end
	ifs_bfx_iaopts.CurButton = AddVerticalButtons(ifs_bfx_iaopts.buttons, bfxo_Layout)
	ifs_bfx_iaopts.cancelbutton = CreateNegativeButton("common.back", "B", "_back")
	bfxo_SetText(ifs_bfx_iaopts)
	AddIFScreen(ifs_bfx_iaopts, "ifs_bfx_iaopts")
	bfxo_Log("the GAME OPTIONS page made (ifs_bfx_iaopts: AI UNITS PER TEAM, REINFORCEMENTS, HEROES, RESET TO DEFAULTS)")
end

-- the map list: its X button, page 0 only (not for multiplayer)
function bfxo_MsShow(this)
	if (this.bfxoOptButton) then
		IFObj_fnSetVis(this.bfxoOptButton, (this.iPage == 0) and (not this.bForMP))
	end
end

function bfxo_Open(this)
	if ((this.iPage ~= 0) or this.bForMP or (not ifs_bfx_iaopts)) then
		return nil
	end
	ifelm_shellscreen_fnPlaySound(this.acceptSound)
	ifs_movietrans_PushScreen(ifs_bfx_iaopts)
	return 1
end

function bfxo_MsEnter(this, bFwd)
	if ((not bFwd) and gBFXO_Back) then
		-- back from GAME OPTIONS: the game's own Enter would leave the map list (it pops itself when entered backwards)
		gBFXO_Back = nil
		gIFShellScreenTemplate_fnEnter(this, bFwd)
		-- 21i-r7: the base Enter fades this.buttons (the era buttons) in - page 0 keeps them hidden (as the game's own Enter)
		if (this.buttons and (this.iPage == 0)) then
			AnimationMgr_AddAnimation(this.buttons, { fStartAlpha = 0, fEndAlpha = 0, })
			IFObj_fnSetAlpha(this.buttons, 0)
			this.buttons.bHidden = 1
		end
		ifelem_shellscreen_fnStopMovie()
		this.lastMovieFile = "-bfxo-" -- its Exit closed the preview's movie file: opened again
		ifs_missionselect_fnSetMapPreview(this)
		bfxo_MsShow(this)
		return
	end
	gBFXO_Back = nil
	this.bfxoOrigEnter(this, bFwd)
	if (bFwd and (not this.bForMP)) then
		bfxo_Load(1)
	end
	bfxo_MsShow(this)
end

function bfxo_MsAccept(this)
	if ((this.CurButton == "bfxopts") and (not ScriptCB_IsLastInputFromJoystick())) then
		if (bfxo_Open(this)) then
			return
		end
	end
	return this.bfxoOrigAccept(this)
end

function bfxo_MsMisc(this)
	gShellScreen_fnUpdateGamepadIcons()
	if (bfxo_Open(this)) then
		return
	end
	if (this.bfxoOrigMisc) then
		return this.bfxoOrigMisc(this)
	end
end

function bfxo_SetPage(this, p)
	gBFXO_OrigSetPage(this, p)
	bfxo_MsShow(this)
end

-- while the game makes ifs_missionselect (before its objects are made): the button in, its functions wrapped, the page
-- made right after the game's ScriptCB_EndIFScreen("ifs_missionselect")
function bfxo_MsSetup()
	local m = ifs_missionselect
	if ((not m) or m.bfxoOptButton or (not CreateMiddleButton)) then
		return
	end
	m.bfxoOptButton = CreateMiddleButton("ifs.GameOpt.title", "X", "bfxopts")
	m.bfxoOrigEnter = m.Enter
	m.bfxoOrigAccept = m.Input_Accept
	m.bfxoOrigMisc = m.Input_Misc
	m.Enter = bfxo_MsEnter
	m.Input_Accept = bfxo_MsAccept
	m.Input_Misc = bfxo_MsMisc
	if (ifs_missionselect_fnSetPage and (not gBFXO_OrigSetPage)) then
		gBFXO_OrigSetPage = ifs_missionselect_fnSetPage
		ifs_missionselect_fnSetPage = bfxo_SetPage
	end
	if (not gBFXO_EndWrapped) then
		gBFXO_EndWrapped = 1
		gBFXO_OrigEnd = ScriptCB_EndIFScreen
		ScriptCB_EndIFScreen = function(name)
			gBFXO_OrigEnd(name)
			if (name == "ifs_missionselect") then
				ScriptCB_EndIFScreen = gBFXO_OrigEnd
				bfxo_Build()
			end
		end
	end
	bfxo_Log("the map list's X button GAME OPTIONS in (ifs_missionselect: Enter / Input_Accept / Input_Misc wrapped)")
end

)BFXLUA"
R"BFXLUA(-- the GAME OPTIONS page (options menu, the main menu's and a match's): 21i-r7: no HEROES row with the Instant Action page
-- (gBFXO_IAPage) - hidden as the game hides it in a match. 21i-r8: two rows of BF1GameExt's after DIFFICULTY: SPLIT SCREEN
-- HORIZONTAL / VERTICAL (gBFXO_SplitRow; the 2-player split - 21i-r9: on the main menu's page only, the next match takes it)
-- and AUTO AIM ON / OFF (gBFXO_AimRow; the game's own profile option and its own row code,
-- which the PC page left out). Copies of the DIFFICULTY row made before the page's objects are.
function bfxo_Has(key)
	if (ScriptCB_getlocalizestr and ScriptCB_getlocalizestr(key, 1)) then
		return 1
	end
	return nil
end

function bfxo_Copy(t)
	if (type(t) ~= "table") then
		return t
	end
	local c = {}
	local k, v
	for k, v in t do
		c[k] = bfxo_Copy(v)
	end
	return c
end

-- a row of ours right after afterTag: the button and its title copied from afterTag's (not made yet), linked in
function bfxo_AddRow(this, l, afterTag, tag, titleKey, titleEnglish)
	local b = this.buttons
	local t = this.buttonlabels
	if ((not b[afterTag]) or (not t[afterTag]) or b[tag]) then
		return nil
	end
	local i = 1
	local at = nil
	while (l.buttonlist[i]) do
		if (l.buttonlist[i].tag == afterTag) then
			at = i
		end
		i = i + 1
	end
	if (not at) then
		return nil
	end
	b[tag] = bfxo_Copy(b[afterTag])
	b[tag].tag = tag
	t[tag] = bfxo_Copy(t[afterTag])
	if (bfxo_Has(titleKey)) then
		t[tag].string = titleKey
		t[tag].ustring = nil
	else
		t[tag].string = nil
		t[tag].ustring = ScriptCB_tounicode(titleEnglish)
	end
	b[tag].label.string = nil
	b[tag].label.ustring = nil
	local down = b[afterTag].link_down
	b[afterTag].link_down = tag
	b[tag].link_up = afterTag
	b[tag].link_down = down
	if (down and b[down]) then
		b[down].link_up = tag
	end
	-- (moved up by hand: Lua 4's tinsert would give the list an "n" field, and the game's own Enter walks it with "for k, v")
	local j = getn(l.buttonlist)
	while (j > at) do
		l.buttonlist[j + 1] = l.buttonlist[j]
		j = j - 1
	end
	l.buttonlist[at + 1] = { tag = tag, title = titleKey, string = "" }
	return 1
end

function bfxo_SplitAsk(q)
	return bfxo_Ask("vsplit " .. q) == 1
end

function bfxo_InMatch()
	return not (ScriptCB_GetShellActive and ScriptCB_GetShellActive())
end

-- 21i-r10: player 2's own GAME OPTIONS (his pause menu in a match: the pausing viewport 1) - his AUTO AIM is BF1GameExt's
-- (AutoAimP2), not the profile's (player 1's); nil when BF1GameExt does not keep it (AutoAimPlayers=0)
function bfxo_AimP2()
	if ((not bfxo_InMatch()) or (not ScriptCB_GetPausingViewport) or (ScriptCB_GetPausingViewport() ~= 1)) then
		return nil
	end
	local v = bfxo_Ask("aimp get 2")
	if ((not v) or (v < 0)) then
		return nil
	end
	return v
end

-- the rows' values (after the game's own ifs_opt_general_fnUpdateStrings)
function bfxo_OgStrings(this)
	local b = this.buttons
	if (b.bfxsplit) then
		local s = "HORIZONTAL"
		if (bfxo_SplitAsk("on")) then
			s = "VERTICAL"
		end
		if (bfxo_SplitAsk("next")) then
			s = s .. " - NEXT MATCH"
		end
		RoundIFButtonLabel_fnSetUString(b.bfxsplit, ScriptCB_tounicode(s))
	end
	local p2 = nil
	if (b.autoaim and gBFXO_AimRow) then
		p2 = bfxo_AimP2()
	end
	if (b.autoaim and gBFXO_AimRow and p2) then
		if (p2 == 1) then
			RoundIFButtonLabel_fnSetString(b.autoaim, "common.on")
		else
			RoundIFButtonLabel_fnSetString(b.autoaim, "common.off")
		end
	elseif (b.autoaim and gBFXO_AimRow) then
		if (ScriptCB_IsAutoAimOn and ScriptCB_IsAutoAimOn()) then
			RoundIFButtonLabel_fnSetString(b.autoaim, "common.on")
		else
			RoundIFButtonLabel_fnSetString(b.autoaim, "common.off")
		end
	end
	if (b.bfxwheel and (not bfxo_InMatch())) then
		-- 21j-r18: SPAWN SCREEN - 1-player games' spawn screen (BF1GameExt's [ClassWheel] Enabled); the main menu's only
		bfxo_WheelText(b.bfxwheel, bfxo_Ask("wheel get") ~= 0)
	end
end

-- 21j-r18: SPAWN SCREEN's two values
function bfxo_WheelValue(on)
	if (on) then
		return "CLASS WHEEL (SINGLEPLAYER ONLY)"
	end
	return "PC MENU (SINGLEPLAYER ONLY)"
end

-- 21j-r18: the SPAWN SCREEN value in the rows' font, or the small one when the longer value is wider than the row - as the game
-- does its CAMERA value (ifs_opt_general_fnUpdateStrings); chosen once, so both values keep one font. The extent is the whole
-- line's width (the game adds every letter's); a text wider than its box (the row, 350) the game wraps onto a second line
function bfxo_WheelText(btn, on)
	if (not gBFXO_WheelFont) then
		local l = ifs_opt_general_vbutton_layout
		local font = (l and l.font) or "gamefont_medium"
		local box = (l and l.width) or 350
		local e = 0
		local k, v
		IFText_fnSetFont(btn.label, font)
		for k, v in { 1, 0 } do
			RoundIFButtonLabel_fnSetUString(btn, ScriptCB_tounicode(bfxo_WheelValue(v == 1)))
			if (ScriptCB_IFText_GetTextExtent and btn.label.cpointer) then
				e = max(e, ScriptCB_IFText_GetTextExtent(btn.label.cpointer) or 0)
			end
		end
		gBFXO_WheelFont = font
		if ((e > box - 10) and (font ~= "gamefont_small")) then
			gBFXO_WheelFont = "gamefont_small"
		end
		bfxo_Log(format("GAME OPTIONS: SPAWN SCREEN's longer value %d wide in %s (the row %d): %s", e, font, box,
			((gBFXO_WheelFont == font) and "fits") or "too wide - gamefont_small"))
	end
	IFText_fnSetFont(btn.label, gBFXO_WheelFont)
	RoundIFButtonLabel_fnSetUString(btn, ScriptCB_tounicode(bfxo_WheelValue(on)))
end

function bfxo_UpdateStrings(this)
	gBFXO_OrigStrings(this)
	if (this == ifs_opt_general) then
		bfxo_OgStrings(this)
		if (gBFXO_AimRow and ScriptCB_IsAutoAimOn) then
			local now = 0
			if (ScriptCB_IsAutoAimOn()) then
				now = 1
			end
			if (gBFXO_AimWas and (now ~= gBFXO_AimWas)) then
				bfxo_Log(format("GAME OPTIONS: AUTO AIM %s (the profile's option - the aim assist reads it offline)", (now == 1) and "ON" or "OFF"))
			end
			gBFXO_AimWas = now
		end
	end
end

-- which rows show: HEROES none with the Instant Action page; SPLIT SCREEN where it can be switched (the main menu's page);
-- AUTO AIM not online (the server's setting decides there)
function bfxo_OgLayout(this)
	local l = ifs_opt_general_vbutton_layout
	if ((not this.buttons) or (not this.buttonlabels) or (not l)) then
		return
	end
	local b = this.buttons
	local t = this.buttonlabels
	if (gBFXO_IAPage and b.hero and t.hero) then
		b.hero.hidden = 1
		t.hero.hidden = 1
	end
	if (b.bfxsplit and t.bfxsplit) then
		-- 21i-r9: the main menu's GAME OPTIONS only (a match keeps the layout it was made with)
		local show = bfxo_SplitAsk("can") and (not bfxo_InMatch())
		if (show) then
			b.bfxsplit.hidden = nil
			t.bfxsplit.hidden = nil
		else
			b.bfxsplit.hidden = 1
			t.bfxsplit.hidden = 1
		end
	end
	if (b.autoaim and t.autoaim and gBFXO_AimRow) then
		local online = ScriptCB_IsNetworkOn and ScriptCB_IsNetworkOn()
		if ((not online) and ScriptCB_SetAutoAimOn and ScriptCB_IsAutoAimOn) then
			b.autoaim.hidden = nil
			t.autoaim.hidden = nil
		else
			b.autoaim.hidden = 1
			t.autoaim.hidden = 1
		end
	end
	if (b.bfxwheel and t.bfxwheel) then
		-- 21j-r18: the main menu's GAME OPTIONS only (the next 1-player match takes it); hidden if BF1GameExt has no class wheel
		local v = bfxo_Ask("wheel get")
		if ((not bfxo_InMatch()) and v and (v >= 0)) then
			b.bfxwheel.hidden = nil
			t.bfxwheel.hidden = nil
		else
			b.bfxwheel.hidden = 1
			t.bfxwheel.hidden = 1
		end
	end
	ShowHideVerticalText(t, l)
	local first = ShowHideVerticalButtons(b, l)
	local k, v
	for k, v in l.buttonlist do
		if (t[v.tag]) then
			IFObj_fnSetPos(t[v.tag], 0, nil)
		end
	end
	if (this.CurButton and b[this.CurButton] and b[this.CurButton].hidden and first) then
		SetCurButton(first)
	end
	bfxo_OgStrings(this)
end

function bfxo_OgEnter(this, bFwd)
	this.bfxoOrigEnter(this, bFwd)
	bfxo_OgLayout(this)
end

-- HEROES does nothing here (the Instant Action page has it); SPLIT SCREEN switched through BF1GameExt
function bfxo_OgOwn(this)
	if (gBFXO_IAPage and (this.CurButton == "hero")) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		return 1
	end
	if ((this.CurButton == "autoaim") and gBFXO_AimRow) then
		local p2 = bfxo_AimP2()
		if (p2) then
			gShellScreen_fnUpdateGamepadIcons()
			bfxo_Ask(format("aimp set 2 %d", 1 - p2))
			ifelm_shellscreen_fnPlaySound(this.acceptSound)
			bfxo_OgStrings(this)
			return 1
		end
	end
	if (this.CurButton == "bfxsplit") then
		gShellScreen_fnUpdateGamepadIcons()
		local now = "now"
		if (bfxo_InMatch()) then
			now = "match"
		end
		local v = 1
		if (bfxo_SplitAsk("on")) then
			v = 0
		end
		bfxo_Ask(format("vsplit set %d %s", v, now))
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxo_OgStrings(this)
		return 1
	end
	if (this.CurButton == "bfxwheel") then
		-- 21j-r18: SPAWN SCREEN: CLASS WHEEL <-> PC MENU (A, left, right)
		gShellScreen_fnUpdateGamepadIcons()
		local v = bfxo_Ask("wheel get")
		if ((v ~= 0) and (v ~= 1)) then
			ifelm_shellscreen_fnPlaySound(this.errorSound)
			return 1
		end
		local n = bfxo_Ask(format("wheel set %d", 1 - v))
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		bfxo_OgStrings(this)
		bfxo_Log(format("GAME OPTIONS: SPAWN SCREEN %s (1-player games; BF1GameExt answered %s)",
			((v == 0) and "CLASS WHEEL") or "PC MENU", tostring(n)))
		return 1
	end
	return nil
end

function bfxo_OgAccept(this)
	if (bfxo_OgOwn(this)) then
		return
	end
	if ((this.CurButton == "reset") and this.buttons.bfxwheel and (not this.buttons.bfxwheel.hidden) and
			(bfxo_Ask("wheel get") == 0)) then
		-- 21j-r18: RESET TO DEFAULTS puts SPAWN SCREEN back to its default (CLASS WHEEL) too
		bfxo_Ask("wheel set 1")
		bfxo_Log("GAME OPTIONS: RESET TO DEFAULTS - SPAWN SCREEN back to CLASS WHEEL")
	end
	return this.bfxoOrigAccept(this)
end

function bfxo_OgLeft(this)
	if (bfxo_OgOwn(this)) then
		return
	end
	return this.bfxoOrigLeft(this)
end

function bfxo_OgRight(this)
	if (bfxo_OgOwn(this)) then
		return
	end
	return this.bfxoOrigRight(this)
end

function bfxo_OgSetup()
	local o = ifs_opt_general
	local l = ifs_opt_general_vbutton_layout
	if ((not o) or (not l) or o.bfxoOrigEnter or (not o.Enter) or (not o.Input_Accept) or (not o.Input_GeneralLeft) or
			(not o.Input_GeneralRight) or (not o.buttons) or (not o.buttonlabels)) then
		return
	end
	local rows = ""
	if (gBFXO_AimRow and bfxo_AddRow(o, l, "diff", "autoaim", "ifs.gameopt.autoaim", "AUTO AIM")) then
		rows = rows .. " AUTO AIM"
	end
	if (gBFXO_SplitRow and bfxo_AddRow(o, l, "diff", "bfxsplit", "ifs.gameopt.splitscreen", "SPLIT SCREEN")) then
		rows = rows .. " SPLIT SCREEN"
	end
	if (gBFXO_WheelRow) then
		-- 21j-r18: SPAWN SCREEN (CLASS WHEEL / PC MENU, single player), the last of BF1GameExt's rows
		local after = "diff"
		if (o.buttons.autoaim and o.buttonlabels.autoaim) then
			after = "autoaim"
		elseif (o.buttons.bfxsplit and o.buttonlabels.bfxsplit) then
			after = "bfxsplit"
		end
		if (bfxo_AddRow(o, l, after, "bfxwheel", "ifs.gameopt.spawnscreen", "SPAWN SCREEN")) then
			rows = rows .. " SPAWN SCREEN"
		end
	end
	o.bfxoOrigEnter = o.Enter
	o.bfxoOrigAccept = o.Input_Accept
	o.bfxoOrigLeft = o.Input_GeneralLeft
	o.bfxoOrigRight = o.Input_GeneralRight
	o.Enter = bfxo_OgEnter
	o.Input_Accept = bfxo_OgAccept
	o.Input_GeneralLeft = bfxo_OgLeft
	o.Input_GeneralRight = bfxo_OgRight
	if (ifs_opt_general_fnUpdateStrings and (not gBFXO_OrigStrings)) then
		gBFXO_OrigStrings = ifs_opt_general_fnUpdateStrings
		ifs_opt_general_fnUpdateStrings = bfxo_UpdateStrings
	end
	local hero = "kept"
	if (gBFXO_IAPage) then
		hero = "hidden (set on the Instant Action page)"
	end
	bfxo_Log(format("GAME OPTIONS (ifs_opt_general, %s): HEROES row %s; rows added after DIFFICULTY:%s; auto aim now %s",
		bfxo_InMatch() and "a match" or "the main menu", hero, (rows == "") and " none" or rows,
		(ScriptCB_IsAutoAimOn and ScriptCB_IsAutoAimOn()) and "ON" or "OFF"))
end

if (gBFXO_Now == "ifs_missionselect") then
	bfxo_MsSetup()
elseif (gBFXO_Now == "ifs_opt_general") then
	bfxo_OgSetup()
end
)BFXLUA";

unsigned long g_iaOptsRuns = 0;

// 21i-r9: the aim assist's mouse / keyboard exit (GOG 1.3 0x001F2EE0: thiscall (float dt, bool mouseKeyboard, 4 pointers);
// "cmp byte [ebx+0Ch], 0 / ... / jne <out>" at +1Ch / +24h). The Classic Collection runs it anyway with /mouseaim.
const char* const kAimFnBody = "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ?? ?? 00 00 80 7B 0C 00 56 57 8B F1 "
                               "0F 85 ?? ?? ?? ?? 8B 4E 04 E8";
constexpr uint32_t kAimKbmJne = 0x24;
// inside it: the profile's auto aim byte (+2Ah) offline, the server's flag online (GOG 1.3 0x001F30FE)
const char* const kAimProfileTest = "80 79 2A 00 0F B6 C0 0F 45 C2 89 45 FC E8 ?? ?? ?? ?? 84 C0 74 0B 80 3D ?? ?? ?? ?? 00 75 08 EB ?? "
                                    "80 7D FC 00";
constexpr uint32_t kAimTestWithin = 0x400;

// 21i-r10: the aim assist's GetCurrentProfile call (its auto aim byte read right after): "call <GetCurrentProfile> / mov ecx, eax /
// test ecx, ecx / je / mov eax, [ebp-4] / mov edx, 1", 13h bytes before the +2Ah test; GetCurrentProfile itself "mov eax,
// [index] / cmp eax, 1 / ja / imul eax, eax, 458h / add eax, <profiles> / ret"
constexpr uint32_t kAimProfCallBack = 0x13;
const char* const kAimProfCall = "E8 ?? ?? ?? ?? 8B C8 85 C9 74 ?? 8B 45 FC BA 01 00 00 00 80 79 2A 00";
const char* const kGetCurProfile = "A1 ?? ?? ?? ?? 83 F8 01 77 ?? 69 C0 58 04 00 00 05 ?? ?? ?? ?? C3";
constexpr uint32_t kAimCtrl = 0x04;        // the aim assist's object: its controller
constexpr uint32_t kCtrlNetId = 0xD4;      // the controller's net player id (GOG 1.3 0x000B7330)
constexpr uint32_t kProfAutoAim = 0x2A;    // the profile's auto aim byte
typedef uintptr_t(__cdecl* GetCurProfile_t)();
GetCurProfile_t g_getCurProfile = nullptr;
unsigned char* g_aimStub = nullptr;
unsigned char g_aimFake[4][0x40] = {};     // what the aim assist reads for players 2 - 4 (only +2Ah)
uintptr_t g_aimSeen[8] = {};               // the objects logged so far
int g_aimSeenN = 0;

// the stub's C half (the game thread, inside the aim assist): the profile-like answer for this object's player
uintptr_t __cdecl AimProfileFor(uintptr_t self)
{
    const uintptr_t prof = g_getCurProfile();
    const uintptr_t ctrl = self ? ReadU32(self + kAimCtrl) : 0;
    const int id = ctrl ? static_cast<int>(static_cast<int32_t>(ReadU32(ctrl + kCtrlNetId))) : -1;
    bool known = false;
    for (int i = 0; i < g_aimSeenN; ++i)
        if (g_aimSeen[i] == self)
            known = true;
    if (!known && g_aimSeenN < 8)
    {
        g_aimSeen[g_aimSeenN++] = self;
        Log("[autoaim] %lu ms: 21i-r10: the aim assist for object 0x%08X - its controller 0x%08X, net player id %d -> %s",
            ScanMsSinceAttach(), static_cast<unsigned>(self), static_cast<unsigned>(ctrl), id,
            id == 0 ? "player 1: the game's profile (GAME OPTIONS' AUTO AIM)" : (id >= 1 && id <= 3) ? "players 2 - 4: AutoAimP2 / P3 / P4"
                                                                             : "not a local player 1 - 4: the game's profile");
    }
    if (id < 1 || id > 3)
        return prof;
    g_aimFake[id][kProfAutoAim] = static_cast<unsigned char>(g_aimP[id] ? 1 : 0);
    return reinterpret_cast<uintptr_t>(&g_aimFake[id][0]);
}

uint32_t AimFollow(uint32_t rva)
{
    for (int hop = 0; hop < 8; ++hop)
    {
        const unsigned char* c = reinterpret_cast<const unsigned char*>(Rva(rva));
        if (c[0] != 0xE9)
            break;
        int32_t rel = 0;
        memcpy(&rel, c + 1, sizeof(rel));
        rva = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + rel);
    }
    return rva;
}

bool AimWrite(uint32_t rva, const void* bytes, size_t n)
{
    void* at = reinterpret_cast<void*>(Rva(rva));
    DWORD old = 0;
    if (!VirtualProtect(at, n, PAGE_EXECUTE_READWRITE, &old))
        return false;
    memcpy(at, bytes, n);
    DWORD ignored = 0;
    VirtualProtect(at, n, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, n);
    return true;
}

// 21i-r11: the auto aim gate - what the mouse / keyboard exit (+24h) became: the aim assist leaves at once for a player whose
// auto aim is OFF (r9's NOP let the console's look scaling - both look inputs x0.5 - and its pull run for player 1 with auto
// aim off: the "aim resist"), runs as the console's for a player whose auto aim is ON. AutoAimMouse=0: the game's own exit.
// "push esi / movzx eax, byte [ebx+0Ch] / push eax / call AimGate / add esp, 8 / test eax, eax / jne +1 / ret / add esp, 4 /
// mov eax, <out> / jmp eax"
unsigned char* g_aimGateStub = nullptr;
uintptr_t g_aimGateSeen[8] = {};
int g_aimGateSeenN = 0;
// 21i-r11: the pull's share (the console's 0.75 at GOG 1.3 0x001F315A, read through BF1GameExt's value): [Splitscreen]
// AutoAimStrength percent of the Xbox's (the Xbox's aim assist, default.xbe 0x000DB8EC, is the same code with the same 0.75)
constexpr uint32_t kAimPullAt = 0x5C;     // from the +2Ah test: "mulss xmm0, [0.75] / push [ebp-1Ch]"
const char* const kAimPull = "F3 0F 59 05 ?? ?? ?? ?? FF 75 E4";
float g_aimPull = 0.75f;
// 21i-r12: the look slow-down over a target (both look inputs x0.5, from the function's start) and the strafe follow's 0.75
constexpr uint32_t kAimSlowYawAt = 0x94;   // "mulss xmm0, [0.5] / add esp, 4"            (GOG 1.3 0x001F2F74)
const char* const kAimSlowYaw = "F3 0F 59 05 ?? ?? ?? ?? 83 C4 04";
constexpr uint32_t kAimSlowPitchAt = 0xB4; // "mulss xmm0, [0.5] / movss [eax], xmm0"     (GOG 1.3 0x001F2F94)
const char* const kAimSlowPitch = "F3 0F 59 05 ?? ?? ?? ?? F3 0F 11 00";
constexpr uint32_t kAimTrackAt = 0x1DD;    // "mulss xmm1, [0.75] / mulss xmm1, [ebp-0Ch]" (GOG 1.3 0x001F30BD)
const char* const kAimTrack = "F3 0F 59 0D ?? ?? ?? ?? F3 0F 59 4D F4";
float g_aimSlow = 0.5f;

// 21i-r12: an instruction's float operand ([abs32] at +4) pointed at BF1GameExt's value - only over the expected constant
bool AimRepoint(uint32_t rva, const char* pattern, float expect, float* ours, float* was)
{
    *was = 0.0f;
    if (!MatchPatternAt(rva, pattern))
        return false;
    uint32_t cst = 0;
    memcpy(&cst, reinterpret_cast<const void*>(Rva(rva + 4)), sizeof(cst));
    if (cst < ExeBase() || cst >= ExeBase() + 0x01000000)
        return false;
    memcpy(was, reinterpret_cast<const void*>(static_cast<uintptr_t>(cst)), sizeof(*was));
    if (*was != expect)
        return false;
    const uint32_t to = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ours));
    return AimWrite(rva + 4, &to, sizeof(to));
}

int AimNetId(uintptr_t self)
{
    const uintptr_t ctrl = self ? ReadU32(self + kAimCtrl) : 0;
    return ctrl ? static_cast<int>(static_cast<int32_t>(ReadU32(ctrl + kCtrlNetId))) : -1;
}

// a player's auto aim: player 1 (and anyone not 2 - 4, or AutoAimPlayers=0) the game's profile; players 2 - 4 BF1GameExt's
bool AimOnFor(int id)
{
    if (g_aimPlayersIni && g_aimStub && id >= 1 && id <= 3)
        return g_aimP[id] != 0;
    if (!g_getCurProfile)
        return true;
    const uintptr_t prof = g_getCurProfile();
    return prof && *reinterpret_cast<volatile const unsigned char*>(prof + kProfAutoAim) != 0;
}

// 1 = leave the aim assist now
int __cdecl AimGate(int kbm, uintptr_t self)
{
    const int id = AimNetId(self);
    const bool on = AimOnFor(id);
    const bool out = (kbm && !g_aimMouseIni) || !on;
    bool known = false;
    for (int i = 0; i < g_aimGateSeenN; ++i)
        if (g_aimGateSeen[i] == self)
            known = true;
    if (!known && g_aimGateSeenN < 8)
    {
        g_aimGateSeen[g_aimGateSeenN++] = self;
        Log("[autoaim] %lu ms: 21i-r11: the aim assist's first run for net player id %d (object 0x%08X): his controller %s the mouse / "
            "keyboard one, his auto aim %s -> %s", ScanMsSinceAttach(), id, static_cast<unsigned>(self), kbm ? "IS" : "is not",
            on ? "ON" : "OFF", out ? "it leaves at once (no look scaling, no pull)" : "it runs (the console's look scaling and pull)");
    }
    return out ? 1 : 0;
}

void AimMouseInstall()
{
    g_aimMouseTried = true;
    if (!g_aimMouseIni && !g_aimPlayersIni)
    {
        Log("[autoaim] [Splitscreen] AutoAimMouse=0, AutoAimPlayers=0 - the aim assist as the game has it");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kAimFnBody, hits, 4);
    uint32_t tests[4] = {};
    const int nt = FindPattern(kAimProfileTest, tests, 4);
    const uint32_t fn = n == 1 ? hits[0] : 0;
    const bool testIn = fn && nt == 1 && tests[0] > fn && tests[0] < fn + kAimTestWithin;
    const uint32_t at = fn + kAimKbmJne;
    const unsigned char* p = fn ? reinterpret_cast<const unsigned char*>(Rva(at)) : nullptr;
    const bool jneOk = p && p[0] == 0x0F && p[1] == 0x85;
    if (!fn || !testIn || !jneOk)
    {
        Log("[autoaim] 21i-r9: the aim assist NOT found (function %d hit(s), its profile test %d hit(s)%s) - nothing changed", n, nt,
            fn && !testIn ? ", not inside it" : "");
        return;
    }
    // its GetCurrentProfile call (the profile's auto aim byte) - the gate and the per-player answer both need the getter
    const uint32_t call = tests[0] - kAimProfCallBack;
    const bool callOk = MatchPatternAt(call, kAimProfCall);
    int32_t rel = 0;
    if (callOk)
        memcpy(&rel, reinterpret_cast<const void*>(Rva(call + 1)), sizeof(rel));
    const uint32_t getter = callOk ? AimFollow(static_cast<uint32_t>(static_cast<int64_t>(call) + 5 + rel)) : 0;
    const bool getterOk = getter && MatchPatternAt(getter, kGetCurProfile);
    if (!callOk || !getterOk)
    {
        Log("[autoaim] 21i-r11: its GetCurrentProfile call at rva 0x%08X %s (target rva 0x%08X %s) - nothing changed", call,
            callOk ? "OK" : "does NOT match", getter, getterOk ? "OK" : "does NOT match");
        return;
    }
    g_getCurProfile = reinterpret_cast<GetCurProfile_t>(Rva(getter));
    unsigned char* mem = static_cast<unsigned char*>(VirtualAlloc(nullptr, 128, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!mem)
    {
        Log("[autoaim] 21i-r11: no memory for the stubs - nothing changed");
        return;
    }
    // (1) 21i-r11: the gate in place of the mouse / keyboard exit
    {
        int32_t jrel = 0;
        memcpy(&jrel, p + 2, sizeof(jrel));
        const uintptr_t out = Rva(at) + 6 + static_cast<intptr_t>(jrel);
        unsigned char* q = mem;
        g_aimGateStub = q;
        *q++ = 0x56;                                 // push esi (the aim assist's object)
        *q++ = 0x0F; *q++ = 0xB6; *q++ = 0x43; *q++ = 0x0C; // movzx eax, byte ptr [ebx+0Ch] (mouse / keyboard)
        *q++ = 0x50;                                 // push eax
        *q++ = 0xE8;
        const int32_t toGate = static_cast<int32_t>(reinterpret_cast<intptr_t>(&AimGate) - reinterpret_cast<intptr_t>(q + 4));
        memcpy(q, &toGate, 4);
        q += 4;
        *q++ = 0x83; *q++ = 0xC4; *q++ = 0x08;       // add esp, 8
        *q++ = 0x85; *q++ = 0xC0;                    // test eax, eax
        *q++ = 0x75; *q++ = 0x01;                    // jne +1
        *q++ = 0xC3;                                 // ret (go on at +29h)
        *q++ = 0x83; *q++ = 0xC4; *q++ = 0x04;       // add esp, 4 (its return address)
        *q++ = 0xB8;                                 // mov eax, <out>
        const uint32_t out32 = static_cast<uint32_t>(out);
        memcpy(q, &out32, 4);
        q += 4;
        *q++ = 0xFF; *q++ = 0xE0;                    // jmp eax
        unsigned char was[6];
        memcpy(was, p, sizeof(was));
        unsigned char call6[6] = { 0xE8, 0, 0, 0, 0, 0x90 };
        const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(g_aimGateStub) - static_cast<intptr_t>(Rva(at) + 5));
        memcpy(call6 + 1, &toStub, 4);
        FlushInstructionCache(GetCurrentProcess(), mem, 128);
        if (AimWrite(at, call6, sizeof(call6)))
            Log("[autoaim] 21i-r11: the aim assist at rva 0x%08X: its mouse / keyboard exit at rva 0x%08X (%02X %02X %02X %02X %02X %02X, "
                "out to rva 0x%08X) -> the auto aim gate: a player with auto aim OFF - it leaves at once (as the PC game for player 1), ON - "
                "it runs as the console's%s", fn, at, was[0], was[1], was[2], was[3], was[4], was[5], static_cast<unsigned>(out - ExeBase()),
                g_aimMouseIni ? " (the mouse / keyboard player too)" : "; AutoAimMouse=0: never for the mouse / keyboard player");
        else
        {
            Log("[autoaim] 21i-r11: VirtualProtect failed (error %lu) - nothing changed", GetLastError());
            return;
        }
    }
    // (2) 21i-r11: the pull's share
    {
        const uint32_t pa = tests[0] + kAimPullAt;
        const bool pullOk = MatchPatternAt(pa, kAimPull);
        uint32_t cst = 0;
        if (pullOk)
            memcpy(&cst, reinterpret_cast<const void*>(Rva(pa + 4)), sizeof(cst));
        float was = 0.0f;
        const bool cstOk = pullOk && cst >= ExeBase() && cst < ExeBase() + 0x01000000;
        if (cstOk)
            memcpy(&was, reinterpret_cast<const void*>(static_cast<uintptr_t>(cst)), sizeof(was));
        int pct = g_aimStrengthIni;
        if (pct < 10)
            pct = 10;
        if (pct > 100)
            pct = 100;
        g_aimPull = 0.75f * static_cast<float>(pct) / 100.0f;
        const uint32_t ours = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_aimPull));
        if (cstOk && was == 0.75f && AimWrite(pa + 4, &ours, sizeof(ours)))
            Log("[autoaim] 21i-r11: the pull's share at rva 0x%08X: the console's %.2f -> %.3f ([Splitscreen] AutoAimStrength=%d%% of the Xbox's)",
                pa, static_cast<double>(was), static_cast<double>(g_aimPull), pct);
        else
            Log("[autoaim] 21i-r11: the pull's share at rva 0x%08X %s (%.2f) - left as the game has it", pa, pullOk ? "found" : "does NOT match",
                static_cast<double>(was));
    }
    // (2b) 21i-r12: the look slow-down over a target, the strafe follow
    {
        int pct = g_aimSlowIni;
        if (pct < 0)
            pct = 0;
        if (pct > 100)
            pct = 100;
        g_aimSlow = 1.0f - 0.5f * static_cast<float>(pct) / 100.0f;
        float w1 = 0.0f, w2 = 0.0f, w3 = 0.0f;
        const bool y = AimRepoint(fn + kAimSlowYawAt, kAimSlowYaw, 0.5f, &g_aimSlow, &w1);
        const bool pi = y && AimRepoint(fn + kAimSlowPitchAt, kAimSlowPitch, 0.5f, &g_aimSlow, &w2);
        if (y && pi)
            Log("[autoaim] 21i-r12: the look slow-down over a target at rva 0x%08X / 0x%08X: the console's x0.50 -> x%.3f ([Splitscreen] "
                "AutoAimSlowdown=%d%% of the console's%s)", fn + kAimSlowYawAt, fn + kAimSlowPitchAt, static_cast<double>(g_aimSlow), pct,
                pct == 0 ? ": full look speed, no resist" : "");
        else
            Log("[autoaim] 21i-r12: the look slow-down at rva 0x%08X / 0x%08X NOT changed (%s %.2f / %s %.2f)%s", fn + kAimSlowYawAt,
                fn + kAimSlowPitchAt, y ? "OK" : "no match", static_cast<double>(w1), pi ? "OK" : "no match", static_cast<double>(w2),
                y ? " - the yaw one is BF1GameExt's, the pitch one the game's" : "");
        const bool t = AimRepoint(fn + kAimTrackAt, kAimTrack, 0.75f, &g_aimPull, &w3);
        Log("[autoaim] 21i-r12: the strafe follow's share at rva 0x%08X: %s", fn + kAimTrackAt,
            t ? "the console's 0.75 -> AutoAimStrength's (as the pull)" : "NOT changed (no match) - the console's");
    }
    // (3) 21i-r10: auto aim per player (the profile byte the pull is gated on)
    if (!g_aimPlayersIni)
    {
        Log("[autoaim] 21i-r10: [Splitscreen] AutoAimPlayers=0 - one auto aim for everybody (the current profile's)");
        return;
    }
    g_aimStub = mem + 64;
    unsigned char* q = g_aimStub;
    *q++ = 0x56;
    *q++ = 0xE8;
    const int32_t toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&AimProfileFor) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toC, 4);
    q += 4;
    *q++ = 0x83;
    *q++ = 0xC4;
    *q++ = 0x04;
    *q++ = 0xC3;
    FlushInstructionCache(GetCurrentProcess(), mem, 128);
    unsigned char e8[5] = { 0xE8, 0, 0, 0, 0 };
    const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(g_aimStub) - static_cast<intptr_t>(Rva(call) + 5));
    memcpy(e8 + 1, &toStub, 4);
    if (!AimWrite(call, e8, sizeof(e8)))
    {
        g_aimStub = nullptr;
        Log("[autoaim] 21i-r10: VirtualProtect failed (error %lu) - one auto aim for everybody", GetLastError());
        return;
    }
    Log("[autoaim] 21i-r10: auto aim per player - the aim assist's GetCurrentProfile call at rva 0x%08X (-> rva 0x%08X) goes "
        "through BF1GameExt: player 1 the game's profile, players 2 / 3 / 4 AutoAimP2 / P3 / P4 = %d / %d / %d",
        call, getter, g_aimP[1], g_aimP[2], g_aimP[3]);
}

// 21i-r10: "aimp get K" (K = 1 - 4; player 1: the game's profile through ScriptCB - -1 here), "aimp set K V" (K = 2 - 4)
int AimQuery(const char* q)
{
    if (!g_aimPlayersIni || !g_aimStub)
        return -1;
    char* end = nullptr;
    const bool set = strncmp(q, "set ", 4) == 0;
    if (!set && strncmp(q, "get ", 4) != 0)
        return -1;
    const long k = strtol(q + 4, &end, 10);
    if (end == q + 4 || k < 2 || k > 4)
        return -1;
    if (!set)
        return g_aimP[k - 1];
    const char* v = end;
    const long on = strtol(v, &end, 10);
    if (end == v)
        return -1;
    g_aimP[k - 1] = on ? 1 : 0;
    char key[16];
    snprintf(key, sizeof(key), "AutoAimP%ld", k);
    const bool wrote = g_mainIni[0] && WritePrivateProfileStringA("Splitscreen", key, on ? "1" : "0", g_mainIni) != 0;
    Log("[autoaim] %lu ms: player %ld's AUTO AIM %s (%s %s)", ScanMsSinceAttach(), k, on ? "ON" : "OFF", key, wrote ? "written" : "NOT written");
    return g_aimP[k - 1];
}

// ---------------------------------------------------------------------------------------------------------------------
// 21i-r13: the pad as Battlefront II's - dead zone, look curve, sensitivity style
// ---------------------------------------------------------------------------------------------------------------------
// the game's pad class Update (GOG 1.3 0x002BA620; pad2.cpp finds the same function)
const char* const kPadUpdBody = "53 56 8B F1 57 0F B6 46 08 8D BE C0 00 00 00 57 50 E8";
constexpr uint32_t kPadDzAt = 0x16;     // "movss xmm1, [+7864] / test eax, eax / movss xmm0, [-7864] / sete al"
const char* const kPadDz = "F3 0F 10 0D ?? ?? ?? ?? 85 C0 F3 0F 10 05 ?? ?? ?? ?? 0F 94 C0";
constexpr uint32_t kPadDzLoAt = 0x20;   // the second movss (its operand at +4)
constexpr uint32_t kPadPassAt = 0x112;  // "movups xmm0, [edi] / movups [eax], xmm0 / call <buttons>" (eax = esp)
const char* const kPadPass = "0F 10 07 0F 11 00 E8";
constexpr uint32_t kPadMovEaxEsp = 0x104; // "mov eax, esp" before it
// PlayerInputs' look: "movsd xmm1, [3.32] / add esp, 4 / fstp [ebp-x] / movsd xmm0, [ebp-x] / call pow / movss xmm1, [ebp-14h]"
// - twice (yaw, pitch); the scale [ebp-10h] is read right after each ("movss xmm2, [ebp-10h]")
const char* const kPadLookPow = "F2 0F 10 0D ?? ?? ?? ?? 83 C4 04 DD 5D ?? F2 0F 10 45 ?? E8 ?? ?? ?? ?? F3 0F 10 4D EC";
constexpr uint32_t kPadPowCallAt = 0x13;
const char* const kPadScaleRead = "F3 0F 10 55 F0";
const char* const kPadPowBody = "83 EC 10 0F AE 5C 24 04";   // the CRT's pow (SSE2: x xmm0, y xmm1 -> xmm0)

float g_padDzZero = 0.0f;      // the game's +-7864 test reads these: never true -> its square dead zone never zeroes
float g_padDz = 0.02f;         // ours (fraction)
bool g_padDzSquare = false;    // 21i-r14: BF1's own square 24 % (PadDeadZone=-1 with the hook in)
constexpr float kPadSquare = 7864.08f / 32767.5f;
int g_padDzNow = 2;            // 21i-r14: the dead zone in use (percent, -1 = BF1's square)
double g_padCurve = 3.3218998908996582; // the exponent in use (the game's until installed)
double g_padCurveGame = 3.3218998908996582;
unsigned char* g_padStubs = nullptr;
bool g_padFeelTried = false;
bool g_padDzOn = false;
bool g_padLookOn = false;
int g_padFeelLines = 0;
constexpr int kPadFeelMaxLines = 80;
uintptr_t g_padSeen[6] = {};
DWORD g_padSeenAt[6] = {};
int g_padSeenLines[6] = {};
int g_padLookLogs = 0;

bool PadFeelBudget()
{
    if (g_padFeelLines >= kPadFeelMaxLines)
        return false;
    ++g_padFeelLines;
    if (g_padFeelLines == kPadFeelMaxLines)
        Log("[padfeel] line limit reached, going quiet");
    return true;
}

// one stick: a round dead zone, the rest rescaled from 0 to 1 (Battlefront II's dead zone starts at 0 past its edge too)
void PadShapeStick(float* x, float* y)
{
    const float ax = *x;
    const float ay = *y;
    if (g_padDzSquare)
    {   // 21i-r14: BF1's own - both axes inside +-7864 -> both 0, else both as they are
        if (std::fabs(ax) < kPadSquare && std::fabs(ay) < kPadSquare)
        {
            *x = 0.0f;
            *y = 0.0f;
        }
        return;
    }
    const float mag = std::sqrt(ax * ax + ay * ay);
    if (mag <= g_padDz || mag <= 0.0f)
    {
        *x = 0.0f;
        *y = 0.0f;
        return;
    }
    float out = (mag - g_padDz) / (1.0f - g_padDz);
    if (out > 1.0f)
        out = 1.0f;
    const float k = out / mag;
    *x = ax * k;
    *y = ay * k;
}

// called from the pad's Update after its four stick floats are written (+0Ch LX, +10h LY, +14h RX, +18h RY)
void __cdecl PadShapeC(uintptr_t pad)
{
    float* f = reinterpret_cast<float*>(pad + 0x0C);
    const float rx0 = f[2];
    const float ry0 = f[3];
    PadShapeStick(&f[0], &f[1]);
    PadShapeStick(&f[2], &f[3]);
    // a few samples per pad: the right stick before / after, at most every 3 s
    if (std::fabs(rx0) + std::fabs(ry0) < 0.15f)
        return;
    int slot = -1;
    for (int i = 0; i < 6; ++i)
        if (g_padSeen[i] == pad || (slot < 0 && g_padSeen[i] == 0))
        {
            slot = i;
            if (g_padSeen[i] == pad)
                break;
        }
    if (slot < 0)
        return;
    g_padSeen[slot] = pad;
    const DWORD now = GetTickCount();
    if (g_padSeenLines[slot] >= 6 || (g_padSeenLines[slot] > 0 && now - g_padSeenAt[slot] < 3000) || !PadFeelBudget())
        return;
    ++g_padSeenLines[slot];
    g_padSeenAt[slot] = now;
    Log("[padfeel] %lu ms: pad 0x%08X (XInput %u): right stick %.3f %.3f (the game's square dead zone would give %.3f %.3f) -> %.3f %.3f",
        ScanMsSinceAttach(), static_cast<unsigned>(pad), static_cast<unsigned>(*reinterpret_cast<const unsigned char*>(pad + 8)),
        static_cast<double>(rx0), static_cast<double>(ry0),
        static_cast<double>((std::fabs(rx0) < 0.24f && std::fabs(ry0) < 0.24f) ? 0.0f : rx0),
        static_cast<double>((std::fabs(rx0) < 0.24f && std::fabs(ry0) < 0.24f) ? 0.0f : ry0), static_cast<double>(f[2]),
        static_cast<double>(f[3]));
}

// Battlefront II's sensitivity shape (0x0052B510): gain k up to t (85 % - or 1 / k when k is larger), blended to 1 at full
double PadBf2Shape(double x, double k)
{
    double t = 0.85;
    if (k > 1.0 && k * 0.85 > 1.0)
        t = 1.0 / k;
    double gain = k;
    if (x >= t)
    {
        double u = (x - t) / (1.0 - t);
        if (u < 0.0)
            u = 0.0;
        if (u > 1.0)
            u = 1.0;
        gain = k * (1.0 - u) + u;
    }
    double out = gain * x;
    return out > 1.0 ? 1.0 : out;
}

// in place of PlayerInputs' pow(|stick|, 3.32): x = |stick|, k = the sensitivity scale the game multiplies by next
double __cdecl PadLookC(double x, float k)
{
    const double y = x > 0.0 ? std::pow(x, g_padCurve) : 0.0;
    double ret = y;
    if (g_padSensBf2Ini && k > 0.0001f)
        ret = PadBf2Shape(y, static_cast<double>(k)) / static_cast<double>(k);
    if (x > 0.2 && g_padLookLogs < 12 && PadFeelBudget())
    {
        ++g_padLookLogs;
        Log("[padfeel] %lu ms: look |stick| %.3f, sensitivity scale %.2f: the game's %.3f -> %.3f (curve %.2f%s)", ScanMsSinceAttach(), x,
            static_cast<double>(k), std::pow(x, g_padCurveGame) * static_cast<double>(k), ret * static_cast<double>(k), g_padCurve,
            g_padSensBf2Ini ? ", Battlefront II's sensitivity" : "");
    }
    return ret;
}

void PadFeelInstall()
{
    g_padFeelTried = true;
    if (!g_padExpIni)
    {   // 21i-r15: the experiment is off - the game's own dead zone, look curve, sensitivity and gamepad page
        Log("[padfeel] 21i-r15: [Splitscreen] PadExperiment=0 - the pads, the look and the gamepad page as the game has them (BF1's "
            "square 24 %% dead zone, |stick| ^ 3.32, its own sensitivity; no DEAD ZONE / LOOK CURVE rows). PadExperiment=1 turns on "
            "PadDeadZone=%d, PadLookCurve=%.2f, PadSensStyle=%d, PadMenu=%d", g_padDzIni, g_padCurveIni, g_padSensBf2Ini ? 1 : 0,
            g_padMenuIni ? 1 : 0);
        return;
    }
    Log("[padfeel] 21i-r13: [Splitscreen] PadDeadZone=%d, PadLookCurve=%.2f%s, PadSensStyle=%d", g_padDzIni, g_padCurveIni,
        g_padCurveIni <= 0.0 ? " (the game's)" : "", g_padSensBf2Ini ? 1 : 0);
    // 21i-r14: with the gamepad page's rows both hooks go in whatever the INI says (any choice works live)
    const bool wantDz = g_padDzIni >= 0 || g_padMenuIni;
    const bool wantLook = g_padCurveIni > 0.0 || g_padSensBf2Ini || g_padMenuIni;
    if (!wantDz && !wantLook)
    {
        Log("[padfeel] the pads and the look as the game has them");
        return;
    }
    g_padStubs = static_cast<unsigned char*>(VirtualAlloc(nullptr, 128, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_padStubs)
    {
        Log("[padfeel] no memory for the stubs - nothing changed");
        return;
    }
    // (1) the dead zone
    if (wantDz)
    {
        uint32_t hits[4] = {};
        const int n = FindPattern(kPadUpdBody, hits, 4);
        const uint32_t fn = n == 1 ? hits[0] : 0;
        const bool dzOk = fn && MatchPatternAt(fn + kPadDzAt, kPadDz);
        const bool passOk = fn && MatchPatternAt(fn + kPadPassAt, kPadPass) && MatchPatternAt(fn + kPadMovEaxEsp, "8B C4");
        float hi = 0.0f, lo = 0.0f;
        if (dzOk)
        {
            uint32_t a1 = 0, a2 = 0;
            memcpy(&a1, reinterpret_cast<const void*>(Rva(fn + kPadDzAt + 4)), 4);
            memcpy(&a2, reinterpret_cast<const void*>(Rva(fn + kPadDzLoAt + 4)), 4);
            if (a1 >= ExeBase() && a1 < ExeBase() + 0x01000000 && a2 >= ExeBase() && a2 < ExeBase() + 0x01000000)
            {
                memcpy(&hi, reinterpret_cast<const void*>(static_cast<uintptr_t>(a1)), 4);
                memcpy(&lo, reinterpret_cast<const void*>(static_cast<uintptr_t>(a2)), 4);
            }
        }
        const bool valsOk = hi > 7000.0f && hi < 9000.0f && lo == -hi;
        if (!fn || !dzOk || !passOk || !valsOk)
            Log("[padfeel] the pad's Update %s (%d hit(s)): its dead zone test %s (%.2f / %.2f), its stick copy %s - the game's dead zone kept",
                fn ? "found" : "NOT found", n, dzOk ? "found" : "does NOT match", static_cast<double>(hi), static_cast<double>(lo),
                passOk ? "found" : "does NOT match");
        else
        {
            g_padDzNow = g_padDzIni;
            g_padDzSquare = g_padDzIni < 0;
            g_padDz = g_padDzIni < 0 ? 0.0f : static_cast<float>(g_padDzIni) / 100.0f;
            // stub: pushad / push esi / call PadShapeC / add esp, 4 / popad / movups xmm0, [edi] / movups [eax], xmm0 / ret
            unsigned char* q = g_padStubs;
            unsigned char* stub = q;
            *q++ = 0x60;
            *q++ = 0x56;
            *q++ = 0xE8;
            const int32_t toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&PadShapeC) - reinterpret_cast<intptr_t>(q + 4));
            memcpy(q, &toC, 4);
            q += 4;
            *q++ = 0x83; *q++ = 0xC4; *q++ = 0x04;
            *q++ = 0x61;
            *q++ = 0x0F; *q++ = 0x10; *q++ = 0x07;
            *q++ = 0x0F; *q++ = 0x11; *q++ = 0x00;
            *q++ = 0xC3;
            FlushInstructionCache(GetCurrentProcess(), g_padStubs, 128);
            const uint32_t zero = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_padDzZero));
            unsigned char call6[6] = { 0xE8, 0, 0, 0, 0, 0x90 };
            const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(stub) - static_cast<intptr_t>(Rva(fn + kPadPassAt) + 5));
            memcpy(call6 + 1, &toStub, 4);
            if (AimWrite(fn + kPadDzAt + 4, &zero, 4) && AimWrite(fn + kPadDzLoAt + 4, &zero, 4) && AimWrite(fn + kPadPassAt, call6, 6))
            {
                g_padDzOn = true;
                Log("[padfeel] 21i-r13: the pad's dead zone (Update rva 0x%08X): the game's square +-%.2f of 32767.5 (%.1f %%, no rescale) at "
                    "rva 0x%08X -> never; rva 0x%08X (0F 10 07 0F 11 00) -> BF1GameExt's %s, both sticks, every pad", fn,
                    static_cast<double>(hi), static_cast<double>(hi / 32767.5f * 100.0f), fn + kPadDzAt, fn + kPadPassAt,
                    g_padDzSquare ? "copy of the game's square 24 % (PadDeadZone=-1)" : "round dead zone (PadDeadZone %, rescaled from 0)");
                Log("[padfeel] 21i-r14: the dead zone now %d %s", g_padDzIni, g_padDzSquare ? "(BF1's square 24 %)" : "% (round)");
            }
            else
                Log("[padfeel] VirtualProtect failed (error %lu) - the dead zone may be half done", GetLastError());
        }
    }
    else
        Log("[padfeel] PadDeadZone=-1: the game's square 24 %% dead zone");
    // (2) the look curve / sensitivity
    if (!wantLook)
    {
        Log("[padfeel] PadLookCurve=0, PadSensStyle=0: the game's look (|stick| ^ 3.32 x the scale)");
        return;
    }
    uint32_t pw[4] = {};
    const int np = FindPattern(kPadLookPow, pw, 4);
    bool ok = np == 2;
    uint32_t powTarget[2] = {};
    uint32_t cst[2] = {};
    for (int i = 0; ok && i < 2; ++i)
    {
        const uint32_t call = pw[i] + kPadPowCallAt;
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(Rva(call + 1)), 4);
        powTarget[i] = AimFollow(static_cast<uint32_t>(static_cast<int64_t>(call) + 5 + rel));
        memcpy(&cst[i], reinterpret_cast<const void*>(Rva(pw[i] + 4)), 4);
        bool scale = false;
        for (uint32_t d = 0x18; d <= 0x22; ++d)
            if (MatchPatternAt(pw[i] + d, kPadScaleRead))
                scale = true;
        ok = scale && MatchPatternAt(powTarget[i], kPadPowBody) && cst[i] >= ExeBase() && cst[i] < ExeBase() + 0x01000000;
    }
    double gameExp = 0.0;
    if (ok)
    {
        memcpy(&gameExp, reinterpret_cast<const void*>(static_cast<uintptr_t>(cst[0])), 8);
        ok = powTarget[0] == powTarget[1] && cst[0] == cst[1] && gameExp > 3.3 && gameExp < 3.34;
    }
    if (!ok)
    {
        Log("[padfeel] the look's pow calls: %d hit(s) (want 2)%s - the game's look kept", np, np == 2 ? ", not as expected" : "");
        return;
    }
    g_padCurveGame = gameExp;
    g_padCurve = g_padCurveIni > 0.0 ? g_padCurveIni : gameExp;
    // stub: sub esp, 14h / movsd [esp], xmm0 / mov eax, [ebp-10h] / mov [esp+8], eax / call PadLookC / fstp qword [esp] /
    // movsd xmm0, [esp] / add esp, 14h / ret
    unsigned char* q = g_padStubs + 64;
    unsigned char* stub = q;
    *q++ = 0x83; *q++ = 0xEC; *q++ = 0x14;
    *q++ = 0xF2; *q++ = 0x0F; *q++ = 0x11; *q++ = 0x04; *q++ = 0x24;
    *q++ = 0x8B; *q++ = 0x45; *q++ = 0xF0;
    *q++ = 0x89; *q++ = 0x44; *q++ = 0x24; *q++ = 0x08;
    *q++ = 0xE8;
    const int32_t toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&PadLookC) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toC, 4);
    q += 4;
    *q++ = 0xDD; *q++ = 0x1C; *q++ = 0x24;
    *q++ = 0xF2; *q++ = 0x0F; *q++ = 0x10; *q++ = 0x04; *q++ = 0x24;
    *q++ = 0x83; *q++ = 0xC4; *q++ = 0x14;
    *q++ = 0xC3;
    FlushInstructionCache(GetCurrentProcess(), g_padStubs, 128);
    bool wrote = true;
    for (int i = 0; i < 2; ++i)
    {
        const uint32_t call = pw[i] + kPadPowCallAt;
        unsigned char e8[5] = { 0xE8, 0, 0, 0, 0 };
        const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(stub) - static_cast<intptr_t>(Rva(call) + 5));
        memcpy(e8 + 1, &toStub, 4);
        wrote = wrote && AimWrite(call, e8, 5);
    }
    g_padLookOn = wrote;
    Log("[padfeel] 21i-r13: the look's pow calls at rva 0x%08X / 0x%08X (-> rva 0x%08X, the game's exponent %.4f at its rva 0x%08X; the "
        "sensitivity scale 2 ^ ((s - 5) / 5) from [ebp-10h]) -> %s: |stick| ^ %.2f%s", pw[0] + kPadPowCallAt, pw[1] + kPadPowCallAt,
        powTarget[0], gameExp, cst[0] - static_cast<uint32_t>(ExeBase()), wrote ? "BF1GameExt's" : "NOT changed (VirtualProtect)",
        g_padCurve, g_padSensBf2Ini ? ", Battlefront II's sensitivity (the scale moves small / middle pushes, a full push stays 100 %)"
                                    : ", the game's sensitivity (everything times the scale)");
}

// ---------------------------------------------------------------------------------------------------------------------
// 21i-r16: the FIELD OF VIEW - single player only
// ---------------------------------------------------------------------------------------------------------------------
// the default FOV getter: "fld dword ptr [<48 deg in radians>] / ret" (GOG 1.3 0x00094AA0, its constant 0x007DAA40)
const char* const kFovGetter = "D9 05 ?? ?? ?? ?? C3";
constexpr float kFov48 = 0.83775806f;
float g_fovBase = kFov48;       // the game's (read from its constant)
volatile LONG g_fovPct = 100;   // in use (VIDEO OPTIONS / FovScale)
bool g_fovTried = false;
bool g_fovOn = false;
int g_fovLines = 0;
constexpr int kFovMaxLines = 120;
int g_fovLastState = -1;        // the last answer kind logged (0 = the game's 48, 1 = scaled)
int g_fovLastPct = -1;

bool FovBudget()
{
    if (g_fovLines >= kFovMaxLines)
        return false;
    ++g_fovLines;
    if (g_fovLines == kFovMaxLines)
        Log("[fov] line limit reached, going quiet");
    return true;
}

// the game's local player count (SetNumLocalPlayers' int: 1 = single player, 2 = split screen; players 3 / 4 answer more
// through the getter, the int stays 2) - 0 if unknown
int FovLocalPlayers()
{
    const BuildInfo* b = CurrentBuild();
    if (!b || !b->localPlayerCountRva)
        return 0;
    return static_cast<int>(ReadU32(Rva(b->localPlayerCountRva)));
}

// 21i-r17: the chase camera's FOV site (GOG 1.3 0x0013AB41): "mov [ebp-14h], 1.0 / push eax / mov edx, [ecx] / call [edx+8]
// (the zoom) / call <getter thunk> / mov ecx, [edi+0Ch] / lea edx, [ebp-24h] / fstp [ebp-24h] / push edx / mov eax, [ecx] /
// call [eax+0Ch] (the unit's FOV) / mov ecx, [edi+0Ch] / movss xmm0, [ebx+0Ch]" - the last two (8 bytes) become a call to
// the stub: pushad / pushfd / push ebp / call FovChaseC / add esp, 4 / popfd / popad / mov ecx, [edi+0Ch] /
// movss xmm0, [ebx+0Ch] / ret
const char* const kFovChase = "C7 45 EC 00 00 80 3F 50 8B 11 FF 52 08 E8 ?? ?? ?? ?? 8B 4F 0C 8D 55 DC D9 5D DC 52 8B 01 FF 50 0C "
                              "8B 4F 0C F3 0F 10 43 0C";
constexpr uint32_t kFovChaseGetterCall = 0x0D; // the getter's call (its thunk -> the 48 deg getter)
constexpr uint32_t kFovChaseAt = 0x21;         // "mov ecx, [edi+0Ch] / movss xmm0, [ebx+0Ch]"
unsigned char* g_fovStub = nullptr;
int g_fovLastZoomed = -1;

// the chase camera's FOV, right after the unit answered it: [ebp-24h] the FOV (radians), [ebp-14h] the zoom (1 = none)
void __cdecl FovChaseC(uintptr_t frame)
{
    float* fov = reinterpret_cast<float*>(frame - 0x24);
    const float zoom = *reinterpret_cast<const float*>(frame - 0x14);
    const int pct = static_cast<int>(g_fovPct);
    const int n = FovLocalPlayers();
    const bool zoomed = zoom > 1.0f;
    const bool scaled = pct != 100 && n == 1 && !zoomed;
    const float was = *fov;
    if (scaled)
        *fov = was * static_cast<float>(pct) / 100.0f;
    const int state = scaled ? 1 : 0;
    if ((state != g_fovLastState || (scaled && pct != g_fovLastPct) || (zoomed ? 1 : 0) != g_fovLastZoomed) && FovBudget())
    {
        g_fovLastState = state;
        g_fovLastPct = pct;
        g_fovLastZoomed = zoomed ? 1 : 0;
        Log("[fov] %lu ms: the chase camera's FOV %.1f deg -> %.1f deg (%s; zoom %.2f, local players %d, FovScale %d %%)",
            ScanMsSinceAttach(), static_cast<double>(was) * 57.29578, static_cast<double>(*fov) * 57.29578,
            scaled ? "single player: scaled" : (zoomed ? "zoomed: the game's own" : (n > 1 ? "split screen: the game's own" : "the game's own")),
            static_cast<double>(zoom), n, pct);
    }
}

void FovInstall()
{
    g_fovTried = true;
    if (!g_fovMenuIni && g_fovPctIni == 100)
    {
        Log("[fov] 21i-r16: [Splitscreen] FovMenu=0, FovScale=100 - the FOV as the game has it");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kFovChase, hits, 4);
    // the getter its call reaches (through the jump thunk): "fld [48 deg] / ret"
    uint32_t getter = 0;
    float g48 = 0.0f;
    if (n == 1)
    {
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(Rva(hits[0] + kFovChaseGetterCall + 1)), 4);
        getter = AimFollow(static_cast<uint32_t>(static_cast<int64_t>(hits[0] + kFovChaseGetterCall) + 5 + rel));
        if (MatchPatternAt(getter, kFovGetter))
        {
            uint32_t cst = 0;
            memcpy(&cst, reinterpret_cast<const void*>(Rva(getter + 2)), 4);
            if (cst >= ExeBase() && cst < ExeBase() + 0x01000000)
                memcpy(&g48, reinterpret_cast<const void*>(static_cast<uintptr_t>(cst)), 4);
        }
    }
    const bool ok = n == 1 && g48 > 0.8377f && g48 < 0.8379f && FovLocalPlayers() != 0;
    if (!ok)
    {
        Log("[fov] 21i-r17: the chase camera's FOV site %s (%d hit(s)), its getter %s (%.4f), the local player count %s - the FOV as "
            "the game has it", n == 1 ? "found" : "NOT found", n, (g48 > 0.8377f && g48 < 0.8379f) ? "48 deg" : "NOT 48 deg",
            static_cast<double>(g48), FovLocalPlayers() ? "known" : "NOT known");
        return;
    }
    g_fovBase = g48;
    g_fovPct = g_fovPctIni;
    g_fovStub = static_cast<unsigned char*>(VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_fovStub)
    {
        Log("[fov] 21i-r17: no memory for the stub - the FOV as the game has it");
        return;
    }
    unsigned char* q = g_fovStub;
    *q++ = 0x60;                                   // pushad
    *q++ = 0x9C;                                   // pushfd
    *q++ = 0x55;                                   // push ebp (the chase camera's frame)
    *q++ = 0xE8;
    const int32_t toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&FovChaseC) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toC, 4);
    q += 4;
    *q++ = 0x83; *q++ = 0xC4; *q++ = 0x04;         // add esp, 4
    *q++ = 0x9D;                                   // popfd
    *q++ = 0x61;                                   // popad
    *q++ = 0x8B; *q++ = 0x4F; *q++ = 0x0C;         // mov ecx, [edi+0Ch]
    *q++ = 0xF3; *q++ = 0x0F; *q++ = 0x10; *q++ = 0x43; *q++ = 0x0C; // movss xmm0, [ebx+0Ch]
    *q++ = 0xC3;                                   // ret
    FlushInstructionCache(GetCurrentProcess(), g_fovStub, 64);
    const uint32_t at = hits[0] + kFovChaseAt;
    unsigned char call8[8] = { 0xE8, 0, 0, 0, 0, 0x90, 0x90, 0x90 };
    const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(g_fovStub) - static_cast<intptr_t>(Rva(at) + 5));
    memcpy(call8 + 1, &toStub, 4);
    if (!AimWrite(at, call8, sizeof(call8)))
    {
        Log("[fov] 21i-r17: VirtualProtect failed (error %lu) - the FOV as the game has it", GetLastError());
        return;
    }
    g_fovOn = true;
    Log("[fov] 21i-r17: the chase camera's FOV (its site rva 0x%08X, the getter rva 0x%08X = %.1f deg, left as it is) -> x FovScale "
        "%d %% at rva 0x%08X (8B 4F 0C F3 0F 10 43 0C -> a call), with one local player and no zoom (as the Classic Collection); "
        "VIDEO OPTIONS' FIELD OF VIEW slider %s", hits[0], getter, static_cast<double>(g48) * 57.29578, g_fovPctIni, at,
        g_fovMenuIni ? "on (FovMenu=1)" : "off (FovMenu=0)");
}

// "fov get" (percent; -1 = not in), "fov set N" (80 - 120, in effect at once, written to FovScale), "fov split" (1 = a match
// with 2 - 4 local players: the row is hidden), "fov menu" (1 = make the row), "fov log TEXT"
int FovQuery(const char* q)
{
    if (!g_fovTried)
        FovInstall();
    if (strncmp(q, "log ", 4) == 0)
    {
        if (FovBudget())
            Log("[fov] lua %lu ms: %s", ScanMsSinceAttach(), q + 4);
        return 0;
    }
    if (strcmp(q, "menu") == 0)
        return (g_fovMenuIni && g_fovOn) ? 1 : 0;
    if (strcmp(q, "get") == 0)
        return g_fovOn ? static_cast<int>(g_fovPct) : -1;
    if (strcmp(q, "split") == 0)
        return FovLocalPlayers() > 1 ? 1 : 0;
    if (strncmp(q, "set ", 4) == 0 && g_fovOn)
    {
        char* end = nullptr;
        const long v = strtol(q + 4, &end, 10);
        if (end == q + 4)
            return -1;
        const int pct = static_cast<int>(v < 80 ? 80 : (v > 120 ? 120 : v));
        g_fovPct = pct;
        char t[16];
        snprintf(t, sizeof(t), "%d", pct);
        const bool wrote = g_mainIni[0] && WritePrivateProfileStringA("Splitscreen", "FovScale", t, g_mainIni) != 0;
        if (FovBudget())
            Log("[fov] %lu ms: VIDEO OPTIONS' FIELD OF VIEW -> %d %% (%.1f deg in single player; FovScale %s)", ScanMsSinceAttach(), pct,
                static_cast<double>(g_fovBase) * 57.29578 * pct / 100.0, wrote ? "written" : "NOT written");
        return pct;
    }
    return -1;
}

// 21i-r14: the gamepad page's DEAD ZONE / LOOK CURVE rows: "on" (1 = show them), "get dz" (percent, -1 = BF1's square),
// "get curve" (x 100), "set dz N", "set curve N" (in effect at once, written to BF1GameExt.ini)
int PadQuery(const char* q)
{
    if (strcmp(q, "on") == 0)
    {
        if (!g_padFeelTried)
            PadFeelInstall();
        return (g_padMenuIni && g_padDzOn && g_padLookOn) ? 1 : 0;
    }
    if (strcmp(q, "get dz") == 0)
        return g_padDzNow;
    if (strcmp(q, "get curve") == 0)
        return static_cast<int>(g_padCurve * 100.0 + 0.5);
    char* end = nullptr;
    if (strncmp(q, "set dz ", 7) == 0)
    {
        const long v = strtol(q + 7, &end, 10);
        if (end == q + 7 || !g_padDzOn)
            return -2;
        g_padDzNow = v < 0 ? -1 : static_cast<int>(v > 50 ? 50 : v);
        g_padDz = g_padDzNow < 0 ? 0.0f : static_cast<float>(g_padDzNow) / 100.0f;
        g_padDzSquare = g_padDzNow < 0;
        char t[16];
        snprintf(t, sizeof(t), "%d", g_padDzNow);
        const bool wrote = g_mainIni[0] && WritePrivateProfileStringA("Splitscreen", "PadDeadZone", t, g_mainIni) != 0;
        Log("[padfeel] %lu ms: 21i-r14: the gamepad page's DEAD ZONE -> %d %s (PadDeadZone %s)", ScanMsSinceAttach(), g_padDzNow,
            g_padDzSquare ? "(BF1's square 24 %)" : "% (round)", wrote ? "written" : "NOT written");
        return g_padDzNow;
    }
    if (strncmp(q, "set curve ", 10) == 0)
    {
        const long v = strtol(q + 10, &end, 10);
        if (end == q + 10 || !g_padLookOn)
            return -2;
        const long c = v < 50 ? 50 : (v > 500 ? 500 : v);
        g_padCurve = c == 332 ? g_padCurveGame : static_cast<double>(c) / 100.0;
        char t[16];
        snprintf(t, sizeof(t), "%.2f", g_padCurve);
        const bool wrote = g_mainIni[0] && WritePrivateProfileStringA("Splitscreen", "PadLookCurve", t, g_mainIni) != 0;
        g_padLookLogs = 0; // a few samples with the new curve
        Log("[padfeel] %lu ms: 21i-r14: the gamepad page's LOOK CURVE -> %.2f%s (PadLookCurve %s)", ScanMsSinceAttach(), g_padCurve,
            c == 332 ? " (BF1's own)" : (c == 100 ? " (Battlefront II's straight line)" : ""), wrote ? "written" : "NOT written");
        return static_cast<int>(g_padCurve * 100.0 + 0.5);
    }
    return -2;
}

// =====================================================================================================================
// 21j-r1: THE GRASS SCAN (log only, [Splitscreen] GrassScan=1, [grass] lines) - bug list #1. On GOG 1.3 / Steam the grass
// never fades in (1.2 grows it in out of the ground before it stands at full size) and flickers as you walk closer.
// Every grass patch is drawn through one function (GOG 1.3 0x00066EA0; GOG 1.2 0x001B3960 - the same code): cut beyond its
// maxDist; in the last 20 m before maxDist fade = (maxDist - dist) x 0.05, then the patch's up axis x (1 - (1 - fade)^2)
// (it grows out of the ground), drawn opaque. Two callers, compared with 1.2:
//   (1) a grasspatch object (GrassPatch::Render, call at 0x0014E8DA): maxDist = its class's MaxDistance (+90h) - as 1.2.
//   (2) a .prp terrain layer's grass (PropObjectMesh::Render, its type 2, call at 0x001FF641): 1.2 passes the layer's own
//       distance (its info +18h); 1.3 passes the near scene end (0x0028DE90: the sky's NearSceneRange picked by a 2020
//       option - the same value that already culled the prop: its range).
// The scan: two calls inside that function (5 bytes each, byte-checked - "movss xmm3, [ebp-4]" after the distance, "movss
// xmm0, [eax+0Ch]" after the fade) feed [grass] lines: per caller every 5 s the calls a frame, cut / drawn / in the grow
// band, the distances (10 m buckets), maxDist against the layer's own distance and the prop range, blinks (a patch missing
// 1 - 8 frames, then back - inside its range or at its edge); each grass class / .prp layer the first time it is drawn;
// the near scene range (the sky's 4 numbers and the 2020 option between them). Nothing in the game changes.
const char* const kGrassFadeAnchor = "F3 0F 10 47 5C 83 C4 08 0F 57 05 ?? ?? ?? ?? D9 5D FC F3 0F 10 5D FC 0F 2F C3 0F 87 ?? ?? ?? ?? "
                                     "F3 0F 10 45 0C 0F 2F D8 0F 87";
const char* const kGrassFadeBody = "55 8B EC 83 EC 18 56 57 8B F9 E8";
const char* const kGrassFadeAt = "8B 45 10 51 8D 4D E8 F3 0F 10 40 0C F3 0F 59 C1 F3 0F 11 04 24 50 E8";
const char* const kGrassFlagGetX = "E8 ?? ?? ?? ?? F3 0F 10 45 0C D9 5D 14 F3 0F 5D 45 14";
const char* const kGrassFlagGetY = "E8 ?? ?? ?? ?? F3 0F 10 45 0C F3 0F 10 5D FC 0F 2F D8 F3 0F 10 55 F8 D9 5D 14 F3 0F 5D 55 14";
const char* const kGrassGlobalGet = "D9 05 ?? ?? ?? ?? C3";
const char* const kGrassPatchSite = "F3 0F 10 86 90 00 00 00 50 51 F3 0F 11 04 24 8B CE FF 75 08 E8";
const char* const kGrassPropSite = "8D 45 D4 56 50 E8 ?? ?? ?? ?? 51 8B 4F 14 8D 45 90 D9 1C 24 50 8B 49 14 E8";
const char* const kGrassNearGet = "C7 44 24 04 00 00 80 3F C7 04 24 00 00 00 00 E8 ?? ?? ?? ?? 51 F3 0F 10 40 28 F3 0F 11 04 24 E8 "
                                  "?? ?? ?? ?? F3 0F 10 0D ?? ?? ?? ?? 83 C4 0C F3 0F 5C 0D ?? ?? ?? ?? D9 5D FC F3 0F 59 4D FC "
                                  "F3 0F 58 0D";
const char* const kGrassOptGet = "B8 ?? ?? ?? ?? C3";
constexpr uint32_t kGrassFnFromAnchor = 0x43; // the function's start, before the anchor
constexpr uint32_t kGrassEntryAt = 0x12;      // "movss xmm3, [ebp-4]" (the distance, right after it is stored)
constexpr uint32_t kGrassFadeBlock = 0xBD;    // kGrassFadeAt
constexpr uint32_t kGrassFadeHook = 0xC4;     // "movss xmm0, [eax+0Ch]" (the fade in xmm1)
constexpr uint32_t kGrassGetXCall = 0x51;     // the near scene flag path: its end getter
constexpr uint32_t kGrassGetYCall = 0x68;     // and its start getter
constexpr uint32_t kGrassPatchCall = 0x14;    // in kGrassPatchSite: the call into the function
constexpr uint32_t kGrassPropXCall = 0x05;    // in kGrassPropSite: the near scene end's getter (its maxDist)
constexpr uint32_t kGrassPropFadeCall = 0x18; // and the call into the function
constexpr uint32_t kGrassNearHi = 0x28;       // in kGrassNearGet: the far number's address (movss xmm1, [hi])
constexpr uint32_t kGrassNearLo = 0x33;       // the near number's (subss xmm1, [lo])
constexpr uint32_t kGrassNearOptCall = 0x0F;  // the options getter ("mov eax, <options> / ret"); the value at +28h
constexpr uint32_t kGrassNearFromFn = 7;      // kGrassNearGet starts 7 bytes into its getter
constexpr int kGrassMaxLines = 800;
// 21j-r3: the stubs' places in their page - each its own 64 bytes (r2 put the fix's stub at +96, inside the fade stub's 34
// bytes from +64: the fade stub's last 2 bytes overwrote the fix stub's first 2 and the .prp call ran into its middle - the crash)
constexpr size_t kGrassStubEntry = 0;
constexpr size_t kGrassStubFade = 64;
constexpr size_t kGrassStubFix = 128;
constexpr size_t kGrassStubBytes = 256;
constexpr int kGrassMaxSummaries = 60;
constexpr int kGrassSlots = 4096;             // the blink table (a power of 2)
constexpr int kGrassBuckets = 12;             // 10 m buckets of the calls' distances; the last 110 m and more

enum GrassKind { kGkPatch = 0, kGkProp = 1, kGkOther = 2, kGkCount = 3 };
const char* const kGrassKindName[kGkCount] = { "grasspatch objects", "the .prp layers' grass (type 2)", "other callers" };

struct GrassStats
{
    unsigned calls, drawn, band, flagged;
    unsigned blink1, blink2, blink3to8, blinkInside, blinkEdge, longReturns, moved;
    float distMin, distMax, drawnMax, fadeMin;
    float maxDistMin, maxDistMax, ownMin, ownMax, rangeMin, rangeMax;
    unsigned buckets[kGrassBuckets];
};

struct GrassSlot
{
    uintptr_t key;
    uint32_t frame;
    uint32_t epoch;
    float dist;
    float x; // 21j-r2: where it stood (a .prp object the game moves is a new patch, not a blink)
    float z;
};

int g_grassLines = 0;
bool g_grassTried = false;
bool g_grassOn = false;
unsigned char* g_grassStub = nullptr;
uintptr_t g_grassPatchRet = 0; // return addresses into the two callers
uintptr_t g_grassPropRet = 0;
uintptr_t g_grassX = 0;        // the near scene end / start (the flag path's getters read them)
uintptr_t g_grassY = 0;
uintptr_t g_grassNearA = 0;    // the sky's NearSceneRange: start a - b, end c - d (the 2020 option picks between them)
uintptr_t g_grassNearB = 0;
uintptr_t g_grassNearC = 0;
uintptr_t g_grassNearD = 0;
uintptr_t g_grassOpt = 0;      // the 2020 options (the value at +28h)
uintptr_t g_grassOtherRet[4] = {};
int g_grassOtherN = 0;
GrassStats g_gs[kGkCount];
GrassSlot g_grassSlot[kGrassSlots];
int g_grassSlotsUsed = 0;
double g_grassMsPerTick = 0.0;
double g_grassLastCallMs = -1.0; // the end of the last hook call
uint32_t g_grassTimeFrame = 0;    // the frame by the scan's clock (when the Present count is not there)
double g_grassWindowMs = -1.0;
uint32_t g_grassFrame = 0;
uint32_t g_grassEpoch = 1;
uint32_t g_grassWinFrame = 0;
unsigned g_grassWinFrames = 0;
int g_grassSummaries = 0;
unsigned g_grassMismatch = 0;
uintptr_t g_grassSeen[64] = {};  // the classes / layers logged so far
int g_grassSeenN = 0;
int g_grassBandLines[kGkCount] = {};
int g_grassBlinkLines = 0;
bool g_grassPresentFrames = false; // frames from the Present count (else the scan's clock)
// the call in progress (the entry hook -> the fade hook)
uintptr_t g_gcFrame = 0;
int g_gcKind = kGkOther;
uintptr_t g_gcKey = 0;
float g_gcDist = 0.0f;
float g_gcFadeEnd = 0.0f;
float g_gcEdge = 0.0f;
float g_gcMaxDist = 0.0f;
float g_gcOwn = 0.0f;
float g_gcX = 0.0f;
float g_gcZ = 0.0f;
// 21j-r2: the .prp grass's maxDist (GrassFadeFix)
typedef float(__cdecl* GrassGetF)();
GrassGetF g_grassNearEndFn = nullptr; // the near scene end's getter (what the .prp caller called)
bool g_grassFixOn = false;

bool GrassBudget()
{
    if (g_grassLines >= kGrassMaxLines)
        return false;
    ++g_grassLines;
    if (g_grassLines == kGrassMaxLines)
    {
        Log("[grass] line limit reached, the grass scan goes quiet");
        return false;
    }
    return true;
}

double GrassNowMs()
{
    LARGE_INTEGER c;
    if (g_grassMsPerTick == 0.0)
    {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        g_grassMsPerTick = f.QuadPart ? 1000.0 / static_cast<double>(f.QuadPart) : 1.0;
    }
    QueryPerformanceCounter(&c);
    return static_cast<double>(c.QuadPart) * g_grassMsPerTick;
}

float GrassF(uintptr_t va)
{
    float f = 0.0f;
    if (va)
        memcpy(&f, reinterpret_cast<const void*>(va), sizeof(f));
    return f;
}

bool GrassPtr(uintptr_t p)
{
    return p >= 0x10000 && p < 0x7FFF0000 && (p & 3) == 0;
}

bool GrassInExe(uintptr_t va)
{
    return va >= ExeBase() && va < ExeBase() + 0x03000000;
}

uint32_t GrassCallTarget(uint32_t rva)
{
    int32_t rel = 0;
    memcpy(&rel, reinterpret_cast<const void*>(Rva(rva + 1)), 4);
    return AimFollow(static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + rel));
}

void GrassResetStats()
{
    for (int k = 0; k < kGkCount; ++k)
    {
        GrassStats& s = g_gs[k];
        memset(&s, 0, sizeof(s));
        s.distMin = s.maxDistMin = s.ownMin = s.rangeMin = 1.0e30f;
        s.distMax = s.drawnMax = s.maxDistMax = s.ownMax = s.rangeMax = -1.0e30f;
        s.fadeMin = 2.0f;
    }
    g_grassWinFrames = 0;
    g_grassWinFrame = 0xFFFFFFFFu; // the next call's frame counts
    g_grassMismatch = 0;
}

void GrassMinMax(float v, float* lo, float* hi)
{
    if (v < *lo)
        *lo = v;
    if (v > *hi)
        *hi = v;
}

// first-seen lines only (up to 64 times): the pages are there and readable
bool GrassReadable(uintptr_t p, size_t n)
{
    if (!GrassPtr(p))
        return false;
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!VirtualQuery(reinterpret_cast<const void*>(p), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) != 0)
        return false;
    return p + n <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

bool GrassSeen(uintptr_t key)
{
    for (int i = 0; i < g_grassSeenN; ++i)
        if (g_grassSeen[i] == key)
            return true;
    if (g_grassSeenN >= static_cast<int>(sizeof(g_grassSeen) / sizeof(g_grassSeen[0])))
        return true; // full: no more first-seen lines
    g_grassSeen[g_grassSeenN++] = key;
    return false;
}

// the near scene range as the game holds it now (end X / start Y, the sky's 4 numbers, the 2020 option)
void GrassNearLine()
{
    if (!GrassBudget())
        return;
    const float a = GrassF(g_grassNearA), b = GrassF(g_grassNearB), c = GrassF(g_grassNearC), d = GrassF(g_grassNearD);
    const float t = g_grassOpt ? GrassF(g_grassOpt + 0x28) : -1.0f;
    Log("[grass]   the near scene now: end X %.1f (the sky's %.1f - %.1f), start Y %.1f (the sky's %.1f - %.1f), the 2020 option "
        "between them %.2f (clamped 0 - 1)",
        static_cast<double>(GrassF(g_grassX)), static_cast<double>(c), static_cast<double>(d),
        static_cast<double>(GrassF(g_grassY)), static_cast<double>(a), static_cast<double>(b), static_cast<double>(t));
}

void GrassSummary(double now)
{
    const double secs = (now - g_grassWindowMs) / 1000.0;
    bool any = false;
    for (int k = 0; k < kGkCount; ++k)
        any = any || g_gs[k].calls != 0;
    if (any && g_grassSummaries < kGrassMaxSummaries && kGrassMaxLines - g_grassLines > 12)
    {
        ++g_grassSummaries;
        for (int k = 0; k < kGkCount; ++k)
        {
            const GrassStats& s = g_gs[k];
            if (!s.calls || !GrassBudget())
                continue;
            const double perFrame = g_grassWinFrames ? static_cast<double>(s.calls) / g_grassWinFrames : 0.0;
            char extra[320] = {};
            if (k == kGkProp)
                snprintf(extra, sizeof(extra), "; the layers' own distance (+18h, what 1.2 passed) %.1f - %.1f; the prop range %.1f - %.1f",
                         static_cast<double>(s.ownMin), static_cast<double>(s.ownMax), static_cast<double>(s.rangeMin),
                         static_cast<double>(s.rangeMax));
            Log("[grass] %lu ms: %s: %u frames in %.1f s (%.0f a second), %u calls (%.1f a frame), %u drawn, "
                "%u cut (beyond the fade's maxDist), %u in the grow band (fade below 1; the lowest fade %.2f); calls at %.1f - %.1f m, drawn out to %.1f m; "
                "maxDist %.1f - %.1f%s; %u with the near scene flag (20000000h)",
                ScanMsSinceAttach(), kGrassKindName[k], g_grassWinFrames, secs, secs > 0.0 ? g_grassWinFrames / secs : 0.0,
                s.calls, perFrame, s.drawn, s.calls - s.drawn, s.band, s.band ? static_cast<double>(s.fadeMin) : 1.0, static_cast<double>(s.distMin), static_cast<double>(s.distMax),
                s.drawn ? static_cast<double>(s.drawnMax) : 0.0, static_cast<double>(s.maxDistMin),
                static_cast<double>(s.maxDistMax), extra, s.flagged);
            if (GrassBudget())
                Log("[grass]   %s - blinks (a patch missing 1 - 8 frames, then back): %u (1 frame %u, 2 frames %u, 3 - 8 frames %u; "
                    "inside its range %u, at its edge %u), back after more than 8 frames %u, moved by the game %u; calls by distance (10 m): "
                    "%u | %u | %u | %u | %u | %u | %u | %u | %u | %u | %u | 110+ %u",
                    kGrassKindName[k], s.blink1 + s.blink2 + s.blink3to8, s.blink1, s.blink2, s.blink3to8, s.blinkInside,
                    s.blinkEdge, s.longReturns, s.moved, s.buckets[0], s.buckets[1], s.buckets[2], s.buckets[3], s.buckets[4],
                    s.buckets[5], s.buckets[6], s.buckets[7], s.buckets[8], s.buckets[9], s.buckets[10], s.buckets[11]);
            // the .prp grass: where its grow band is against where its patches are drawn
            if (k == kGkProp && s.drawn && GrassBudget())
            {
                const float bandFrom = s.maxDistMin - 20.0f;
                if (s.band == 0 && s.drawnMax < bandFrom)
                    Log("[grass]   verdict: the grow band (%.1f - %.1f m from the camera) is beyond every drawn patch (out to %.1f m): "
                        "nothing grows in, each patch stands at full size the frame it is drawn (1.2's band ends at the layer's own "
                        "distance, %.1f - %.1f)",
                        static_cast<double>(bandFrom), static_cast<double>(s.maxDistMax), static_cast<double>(s.drawnMax),
                        static_cast<double>(s.ownMin), static_cast<double>(s.ownMax));
                else
                    Log("[grass]   verdict: %u draws in the grow band (it starts %.1f m from the camera at the nearest), out to %.1f m; "
                        "maxDist %s", s.band, static_cast<double>(bandFrom), static_cast<double>(s.drawnMax),
                        g_grassFixOn ? "the layer's own distance (GrassFadeFix=1, as 1.2)" : "the near scene end (the 2020 port's)");
            }
        }
        GrassNearLine();
        if (GrassBudget())
            Log("[grass]   frames by %s; %d patches in the blink table%s; fade calls that did not match the call in progress (not "
                "counted): %u", g_grassPresentFrames ? "the Present count" : "the scan's clock (no Present count)", g_grassSlotsUsed,
                g_grassSlotsUsed > kGrassSlots / 2 ? " - cleared (half full)" : "", g_grassMismatch);
    }
    if (g_grassSlotsUsed > kGrassSlots / 2)
    {
        memset(g_grassSlot, 0, sizeof(g_grassSlot));
        g_grassSlotsUsed = 0;
    }
    GrassResetStats();
    g_grassWindowMs = now;
}

GrassSlot* GrassFind(uintptr_t key)
{
    const uint32_t mask = kGrassSlots - 1;
    uint32_t i = (static_cast<uint32_t>(key) * 2654435761u >> 20) & mask;
    for (int n = 0; n < kGrassSlots; ++n, i = (i + 1) & mask)
    {
        GrassSlot& sl = g_grassSlot[i];
        if (sl.key == key)
            return &sl;
        if (sl.key == 0)
        {
            if (g_grassSlotsUsed >= kGrassSlots * 3 / 4)
                return nullptr; // full until the next stall clears it
            ++g_grassSlotsUsed;
            sl.key = key;
            sl.frame = 0;
            sl.epoch = 0;
            sl.dist = 0.0f;
            sl.x = 0.0f;
            sl.z = 0.0f;
            return &sl;
        }
    }
    return nullptr;
}

// the first time a grass class (grasspatch objects) or a .prp layer is drawn: its numbers (the seen check first - this runs
// for every call)
void GrassClassLine(int kind, uintptr_t frame, uintptr_t cls, float dist, float maxDist)
{
    uintptr_t info = 0;
    if (kind == kGkProp)
    {
        const uintptr_t obj = ReadU32(frame - 0x20);
        info = GrassPtr(obj) ? ReadU32(obj + 0x14) : 0;
        if (!GrassPtr(info) || GrassSeen(info))
            return;
    }
    else if (!GrassPtr(cls) || GrassSeen(cls))
        return;
    if (!GrassBudget())
        return;
    char nums[360] = "its class: not readable";
    if (GrassReadable(cls, 0xF0))
    {
        const uintptr_t rend = ReadU32(cls + 0xD8);
        const bool rendOk = GrassReadable(rend, 0xEC);
        const float rendNear = rendOk ? GrassF(rend + 0x1C) : 0.0f;
        const unsigned rendE8 = rendOk ? *reinterpret_cast<const unsigned char*>(rend + 0xE8) : 0xFFu;
        const unsigned clsEC = *reinterpret_cast<const unsigned char*>(cls + 0xEC);
        const uint32_t vt = ReadU32(cls) - static_cast<uint32_t>(ExeBase());
        snprintf(nums, sizeof(nums), "its class 0x%08X (vtable rva 0x%08X): MaxDistance %.1f, radius %.1f, RadiusFadeMin / Max %.1f / %.1f, "
                 "MinSize / MaxSize %.2f / %.2f, Alpha %.2f, blades %d; its renderable's +1Ch %g, +E8h %u, the class's +ECh %u",
                 static_cast<unsigned>(cls), vt, static_cast<double>(GrassF(cls + 0x90)), static_cast<double>(GrassF(cls + 0x5C)),
                 static_cast<double>(GrassF(cls + 0x98)), static_cast<double>(GrassF(cls + 0x9C)),
                 static_cast<double>(GrassF(cls + 0x80)), static_cast<double>(GrassF(cls + 0x84)),
                 static_cast<double>(GrassF(cls + 0x8C)), static_cast<int>(ReadU32(cls + 0x7C)), static_cast<double>(rendNear),
                 rendE8, clsEC);
    }
    if (kind == kGkProp)
    {
        const bool infoOk = GrassReadable(info, 0x20);
        Log("[grass] %lu ms: a .prp grass layer first drawn (its info 0x%08X, type %u): its own distance %.1f (+18h - 1.2's maxDist), "
            "+1Ch %.1f; the fade gets maxDist %.1f (%s); first at %.1f m; %s",
            ScanMsSinceAttach(), static_cast<unsigned>(info), infoOk ? ReadU32(info + 0x10) : 0u,
            static_cast<double>(infoOk ? GrassF(info + 0x18) : 0.0f), static_cast<double>(infoOk ? GrassF(info + 0x1C) : 0.0f),
            static_cast<double>(maxDist), g_grassFixOn ? "GrassFadeFix: its own, held to the near scene end" : "the near scene end",
            static_cast<double>(dist), nums);
        return;
    }
    Log("[grass] %lu ms: %s - a class first drawn: the fade gets maxDist %.1f; first at %.1f m; %s", ScanMsSinceAttach(),
        kGrassKindName[kind], static_cast<double>(maxDist), static_cast<double>(dist), nums);
}

// the entry hook: the distance is stored ([ebp-4]); cls = the function's this (the grass class)
void __cdecl GrassEntryC(uintptr_t frame, uintptr_t cls)
{
    const double now = GrassNowMs();
    const bool first = g_grassLastCallMs < 0.0;
    const double gap = first ? 0.0 : now - g_grassLastCallMs;
    if (g_grassWindowMs < 0.0)
        g_grassWindowMs = now;
    if (!first && gap > 2000.0)
    {
        // a long stall (a menu, a pause, a load): the window closes at the last call, the blink table starts over
        GrassSummary(g_grassLastCallMs);
        g_grassWindowMs = now;
        memset(g_grassSlot, 0, sizeof(g_grassSlot));
        g_grassSlotsUsed = 0;
    }
    else if (now - g_grassWindowMs >= 5000.0)
        GrassSummary(now);
    if (first || gap > 100.0)
        ++g_grassEpoch; // blinks are counted again from here
    if (first || gap > 3.0)
        ++g_grassTimeFrame; // the calls of one frame come together (the gap from the end of the last hook call)
    const long presents = FrameCapturePresents();
    if ((presents > 0) != g_grassPresentFrames)
        ++g_grassEpoch; // the frame numbers change source: blinks are counted again from here
    g_grassPresentFrames = presents > 0;
    g_grassFrame = g_grassPresentFrames ? static_cast<uint32_t>(presents) + 1u : g_grassTimeFrame;
    if (g_grassFrame != g_grassWinFrame)
    {
        g_grassWinFrame = g_grassFrame;
        ++g_grassWinFrames;
    }

    const uintptr_t ret = ReadU32(frame + 4);
    const int kind = (g_grassPatchRet && ret == g_grassPatchRet) ? kGkPatch : ((g_grassPropRet && ret == g_grassPropRet) ? kGkProp : kGkOther);
    const float dist = GrassF(frame - 4);
    const float maxDist = GrassF(frame + 0x0C);
    const uint32_t flags = ReadU32(frame + 0x14);
    float fadeEnd = maxDist;
    if (flags & 0x20000000u)
    {
        const float x = GrassF(g_grassX);
        if (g_grassX && x < fadeEnd)
            fadeEnd = x;
    }
    GrassStats& s = g_gs[kind];
    ++s.calls;
    GrassMinMax(dist, &s.distMin, &s.distMax);
    GrassMinMax(maxDist, &s.maxDistMin, &s.maxDistMax);
    if (flags & 0x20000000u)
        ++s.flagged;
    const int b = !(dist >= 0.0f) ? 0 : (dist >= 10.0f * (kGrassBuckets - 1) ? kGrassBuckets - 1 : static_cast<int>(dist / 10.0f));
    ++s.buckets[b];
    uintptr_t key = ReadU32(frame + 8); // the patch's matrix (a grasspatch object's own)
    const uintptr_t mat = key;
    g_gcX = GrassPtr(mat) ? GrassF(mat + 0x30) : 0.0f; // its place (the matrix's position row)
    g_gcZ = GrassPtr(mat) ? GrassF(mat + 0x38) : 0.0f;
    float edge = fadeEnd;
    float own = 0.0f;
    if (kind == kGkProp)
    {
        const uintptr_t obj = ReadU32(frame - 0x20);        // the caller's edi (pushed by the function)
        const uintptr_t callerFrame = ReadU32(frame);
        const uintptr_t info = GrassPtr(obj) ? ReadU32(obj + 0x14) : 0;
        own = GrassPtr(info) ? GrassF(info + 0x18) : 0.0f;
        const float range = GrassPtr(callerFrame) ? GrassF(callerFrame - 8) : 0.0f; // the prop's range (its cull before)
        GrassMinMax(own, &s.ownMin, &s.ownMax);
        GrassMinMax(range, &s.rangeMin, &s.rangeMax);
        key = obj; // the matrix is the caller's copy - the prop object is the patch
        if (range > 0.0f && range < edge)
            edge = range;
        if (own > 0.0f && own < edge)
            edge = own;
    }
    else if (kind == kGkOther)
    {
        bool known = false;
        for (int i = 0; i < g_grassOtherN; ++i)
            known = known || g_grassOtherRet[i] == ret;
        if (!known && g_grassOtherN < 4)
        {
            g_grassOtherRet[g_grassOtherN++] = ret;
            if (GrassBudget())
                Log("[grass] %lu ms: another caller of the grass draw: returns to rva 0x%08X (this 0x%08X, maxDist %.1f, at %.1f m)",
                    ScanMsSinceAttach(), static_cast<unsigned>(ret - ExeBase()), static_cast<unsigned>(cls),
                    static_cast<double>(maxDist), static_cast<double>(dist));
        }
    }
    g_gcFrame = frame;
    g_gcKind = kind;
    g_gcKey = key;
    g_gcDist = dist;
    g_gcFadeEnd = fadeEnd;
    g_gcEdge = edge;
    g_gcMaxDist = maxDist;
    g_gcOwn = own;
    if (kind != kGkOther)
        GrassClassLine(kind, frame, cls, dist, maxDist);
    g_grassLastCallMs = GrassNowMs();
}

// the fade hook: the patch is drawn; *fade = its fade (1 = full size, below 1 = growing in)
void GrassFadeTrack(uintptr_t frame, const float* fadePtr)
{
    if (frame != g_gcFrame)
    {
        ++g_grassMismatch;
        return;
    }
    g_gcFrame = 0;
    const float fade = *fadePtr;
    GrassStats& s = g_gs[g_gcKind];
    ++s.drawn;
    if (g_gcDist > s.drawnMax)
        s.drawnMax = g_gcDist;
    if (fade < s.fadeMin)
        s.fadeMin = fade;
    if (fade < 0.999f)
    {
        ++s.band;
        if (g_grassBandLines[g_gcKind] < 6 && GrassBudget())
        {
            ++g_grassBandLines[g_gcKind];
            const int a = static_cast<int>(fade * 255.0f) & 0xFF;
            const float af = static_cast<float>(a) / 255.0f;
            const float up = 1.0f - (1.0f - af) * (1.0f - af);
            Log("[grass] %lu ms: %s growing in: at %.1f m, maxDist %.1f, fade %.3f -> its up axis x %.3f", ScanMsSinceAttach(),
                kGrassKindName[g_gcKind], static_cast<double>(g_gcDist), static_cast<double>(g_gcFadeEnd),
                static_cast<double>(fade), static_cast<double>(up));
        }
    }
    if (!g_gcKey)
        return;
    GrassSlot* sl = GrassFind(g_gcKey);
    if (!sl)
        return;
    const float moved = std::fabs(sl->x - g_gcX) + std::fabs(sl->z - g_gcZ);
    if (sl->frame != 0 && moved > 1.0f)
        ++s.moved; // the game put this object somewhere else: a new patch, not a blink
    else if (sl->epoch == g_grassEpoch && sl->frame != 0 && sl->frame + 1 < g_grassFrame)
    {
        const uint32_t missed = g_grassFrame - sl->frame - 1;
        if (missed <= 8)
        {
            if (missed == 1)
                ++s.blink1;
            else if (missed == 2)
                ++s.blink2;
            else
                ++s.blink3to8;
            const bool inside = g_gcDist < g_gcEdge - 5.0f && sl->dist < g_gcEdge - 5.0f;
            if (inside)
                ++s.blinkInside;
            else
                ++s.blinkEdge;
            if (g_grassBlinkLines < 16 && GrassBudget())
            {
                ++g_grassBlinkLines;
                Log("[grass] %lu ms: %s blink: patch 0x%08X missing %u frame(s), back at %.1f m (it was at %.1f m) - %s (its edge %.1f m: "
                    "maxDist %.1f%s%.1f)",
                    ScanMsSinceAttach(), kGrassKindName[g_gcKind], static_cast<unsigned>(g_gcKey), missed,
                    static_cast<double>(g_gcDist), static_cast<double>(sl->dist), inside ? "inside its range" : "at its edge",
                    static_cast<double>(g_gcEdge), static_cast<double>(g_gcMaxDist),
                    g_gcKind == kGkProp ? ", the layer's own distance " : ", the fade's end ", static_cast<double>(g_gcKind == kGkProp ? g_gcOwn : g_gcFadeEnd));
            }
        }
        else
            ++s.longReturns;
    }
    sl->frame = g_grassFrame;
    sl->epoch = g_grassEpoch;
    sl->dist = g_gcDist;
    sl->x = g_gcX;
    sl->z = g_gcZ;
}

void __cdecl GrassFadeC(uintptr_t frame, const float* fadePtr)
{
    GrassFadeTrack(frame, fadePtr);
    g_grassLastCallMs = GrassNowMs();
}

// 21j-r2: the .prp grass's maxDist (its "call <near scene end>" goes here through a stub with the prop object): the layer's
// own distance (its info +18h - what 1.2 passes), held to the near scene end when that is nearer
float __cdecl GrassPropMaxDistC(uintptr_t obj)
{
    const float x = g_grassNearEndFn();
    const uintptr_t info = GrassPtr(obj) ? ReadU32(obj + 0x14) : 0;
    const float own = GrassPtr(info) ? GrassF(info + 0x18) : 0.0f;
    return (own > 0.0f && own < x) ? own : x;
}

void GrassScanInstall()
{
    g_grassTried = true;
    if (!g_grassScanIni && !g_grassFixIni)
    {
        Log("[grass] 21j-r2: [Splitscreen] GrassScan=0, GrassFadeFix=0 - the grass as the 2020 port has it, no grass scan");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kGrassFadeAnchor, hits, 4);
    const uint32_t anchor = n == 1 ? hits[0] : 0;
    const uint32_t fn = anchor ? anchor - kGrassFnFromAnchor : 0;
    const bool body = anchor && MatchPatternAt(fn, kGrassFadeBody);
    const bool fadeAt = anchor && MatchPatternAt(anchor + kGrassFadeBlock, kGrassFadeAt);
    if (!body || !fadeAt)
    {
        Log("[grass] 21j-r2: the grass draw %s (%d hit(s)), its start %s, its fade %s - no grass scan, the .prp grass as the 2020 port has it",
            n == 1 ? "found" : "NOT found", n,
            body ? "OK" : "NOT matched", fadeAt ? "OK" : "NOT matched");
        return;
    }
    // the flag path's near scene getters ("fld [x] / ret")
    if (MatchPatternAt(anchor + kGrassGetXCall, kGrassFlagGetX))
    {
        const uint32_t g = GrassCallTarget(anchor + kGrassGetXCall);
        if (MatchPatternAt(g, kGrassGlobalGet) && GrassInExe(ReadU32(Rva(g + 2))))
            g_grassX = ReadU32(Rva(g + 2));
    }
    if (MatchPatternAt(anchor + kGrassGetYCall, kGrassFlagGetY))
    {
        const uint32_t g = GrassCallTarget(anchor + kGrassGetYCall);
        if (MatchPatternAt(g, kGrassGlobalGet) && GrassInExe(ReadU32(Rva(g + 2))))
            g_grassY = ReadU32(Rva(g + 2));
    }
    // the two callers
    uint32_t h2[4] = {};
    const int n2 = FindPattern(kGrassPatchSite, h2, 4);
    const bool patchOk = n2 == 1 && GrassCallTarget(h2[0] + kGrassPatchCall) == fn;
    if (patchOk)
        g_grassPatchRet = Rva(h2[0] + kGrassPatchCall + 5);
    uint32_t h3[4] = {};
    const int n3 = FindPattern(kGrassPropSite, h3, 4);
    const bool propOk = n3 == 1 && GrassCallTarget(h3[0] + kGrassPropFadeCall) == fn;
    uint32_t propEnd = 0;
    if (propOk)
    {
        g_grassPropRet = Rva(h3[0] + kGrassPropFadeCall + 5);
        propEnd = GrassCallTarget(h3[0] + kGrassPropXCall);
    }
    // the near scene's raw numbers: two getters alike - the end's is the one the .prp caller calls
    uint32_t h4[4] = {};
    const int n4 = FindPattern(kGrassNearGet, h4, 4);
    for (int i = 0; i < n4 && i < 4 && propEnd; ++i)
    {
        const uintptr_t hi = ReadU32(Rva(h4[i] + kGrassNearHi));
        const uintptr_t lo = ReadU32(Rva(h4[i] + kGrassNearLo));
        if (!GrassInExe(hi) || !GrassInExe(lo))
            continue;
        if (h4[i] - kGrassNearFromFn == propEnd) // the end's getter (the one the .prp caller calls)
        {
            g_grassNearC = lo;
            g_grassNearD = hi;
        }
        else
        {
            g_grassNearA = lo;
            g_grassNearB = hi;
        }
        const uint32_t og = GrassCallTarget(h4[i] + kGrassNearOptCall);
        if (MatchPatternAt(og, kGrassOptGet) && GrassInExe(ReadU32(Rva(og + 1))))
            g_grassOpt = ReadU32(Rva(og + 1));
    }
    g_grassStub = static_cast<unsigned char*>(VirtualAlloc(nullptr, kGrassStubBytes, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_grassStub)
    {
        Log("[grass] 21j-r2: no memory for the stubs - no grass scan, the .prp grass as the 2020 port has it");
        return;
    }
    unsigned char call5[5] = { 0xE8, 0, 0, 0, 0 };
    int32_t rel = 0;
    // 21j-r2: the .prp grass's maxDist - stub 3: push edi (the prop object) / call GrassPropMaxDistC / add esp, 4 / ret (the
    // float in st0, as the near scene end's getter returns it); its "call <near scene end>" goes to the stub
    const bool nearEndOk = propOk && propEnd && g_grassNearC && g_grassNearD; // the call's target is the near scene end's getter
    const uint32_t atProp = propOk ? h3[0] + kGrassPropXCall : 0;
    if (g_grassFixIni && nearEndOk)
    {
        g_grassNearEndFn = reinterpret_cast<GrassGetF>(Rva(propEnd));
        unsigned char* p3 = g_grassStub + kGrassStubFix;
        p3[0] = 0x57; // push edi
        p3[1] = 0xE8;
        const int32_t toFix = static_cast<int32_t>(reinterpret_cast<intptr_t>(&GrassPropMaxDistC) - reinterpret_cast<intptr_t>(p3 + 6));
        memcpy(p3 + 2, &toFix, 4);
        p3[6] = 0x83; // add esp, 4
        p3[7] = 0xC4;
        p3[8] = 0x04;
        p3[9] = 0xC3; // ret
        static_assert(kGrassStubFix + 10 <= kGrassStubBytes, "the fix stub runs past its page");
        FlushInstructionCache(GetCurrentProcess(), p3, 16);
        rel = static_cast<int32_t>(reinterpret_cast<intptr_t>(p3) - static_cast<intptr_t>(Rva(atProp) + 5));
        memcpy(call5 + 1, &rel, 4);
        g_grassFixOn = AimWrite(atProp, call5, sizeof(call5));
        Log("[grass] 21j-r2: [Splitscreen] GrassFadeFix=1 - %s: the .prp grass's maxDist call at rva 0x%08X (was the near scene end, rva "
            "0x%08X) -> the layer's own distance (its info +18h, as 1.2), held to the near scene end when that is nearer",
            g_grassFixOn ? "in" : "NOT in (VirtualProtect failed)", atProp, propEnd);
    }
    else if (g_grassFixIni)
        Log("[grass] 21j-r2: [Splitscreen] GrassFadeFix=1 but the .prp caller %s, its maxDist getter %s - the .prp grass as the 2020 port "
            "has it", propOk ? "found" : "NOT found", nearEndOk ? "known" : "NOT matched");
    else
        Log("[grass] 21j-r2: [Splitscreen] GrassFadeFix=0 - the .prp grass gets the near scene end as maxDist (the 2020 port's)");
    if (!g_grassScanIni)
    {
        Log("[grass] 21j-r1: [Splitscreen] GrassScan=0 - no grass scan");
        return;
    }
    // stub 1 (the entry): pushad / pushfd / sub esp, 16 / movups [esp], xmm0 / push edi / push ebp / call GrassEntryC /
    // add esp, 8 / movups xmm0, [esp] / add esp, 16 / popfd / popad / movss xmm3, [ebp-4] / ret
    unsigned char* q = g_grassStub + kGrassStubEntry;
    const unsigned char e1[] = { 0x60, 0x9C, 0x83, 0xEC, 0x10, 0x0F, 0x11, 0x04, 0x24, 0x57, 0x55, 0xE8 };
    memcpy(q, e1, sizeof(e1));
    q += sizeof(e1);
    int32_t toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&GrassEntryC) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toC, 4);
    q += 4;
    const unsigned char e2[] = { 0x83, 0xC4, 0x08, 0x0F, 0x10, 0x04, 0x24, 0x83, 0xC4, 0x10, 0x9D, 0x61,
                                 0xF3, 0x0F, 0x10, 0x5D, 0xFC, 0xC3 };
    static_assert(sizeof(e1) + 4 + sizeof(e2) <= kGrassStubFade - kGrassStubEntry, "the entry stub runs into the fade stub");
    memcpy(q, e2, sizeof(e2));
    // stub 2 (the fade): pushad / pushfd / sub esp, 16 / movups [esp], xmm1 / push esp (the fade) / push ebp / call
    // GrassFadeC / add esp, 8 / movups xmm1, [esp] / add esp, 16 / popfd / popad / movss xmm0, [eax+0Ch] / ret
    unsigned char* f = g_grassStub + kGrassStubFade;
    q = f;
    const unsigned char f1[] = { 0x60, 0x9C, 0x83, 0xEC, 0x10, 0x0F, 0x11, 0x0C, 0x24, 0x54, 0x55, 0xE8 };
    memcpy(q, f1, sizeof(f1));
    q += sizeof(f1);
    toC = static_cast<int32_t>(reinterpret_cast<intptr_t>(&GrassFadeC) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toC, 4);
    q += 4;
    const unsigned char f2[] = { 0x83, 0xC4, 0x08, 0x0F, 0x10, 0x0C, 0x24, 0x83, 0xC4, 0x10, 0x9D, 0x61,
                                 0xF3, 0x0F, 0x10, 0x40, 0x0C, 0xC3 };
    static_assert(sizeof(f1) + 4 + sizeof(f2) <= kGrassStubFix - kGrassStubFade, "the fade stub runs into the fix stub");
    memcpy(q, f2, sizeof(f2));
    FlushInstructionCache(GetCurrentProcess(), g_grassStub, kGrassStubBytes);
    GrassResetStats();
    const uint32_t atEntry = anchor + kGrassEntryAt;
    const uint32_t atFade = anchor + kGrassFadeHook;
    rel = static_cast<int32_t>(reinterpret_cast<intptr_t>(g_grassStub + kGrassStubEntry) - static_cast<intptr_t>(Rva(atEntry) + 5));
    memcpy(call5 + 1, &rel, 4);
    const bool w1 = AimWrite(atEntry, call5, sizeof(call5));
    rel = static_cast<int32_t>(reinterpret_cast<intptr_t>(f) - static_cast<intptr_t>(Rva(atFade) + 5));
    memcpy(call5 + 1, &rel, 4);
    const bool w2 = w1 && AimWrite(atFade, call5, sizeof(call5));
    if (!w1 || !w2)
    {
        const DWORD err = GetLastError();
        const unsigned char movss5[5] = { 0xF3, 0x0F, 0x10, 0x5D, 0xFC }; // the entry call's own instruction back
        const bool back = w1 && AimWrite(atEntry, movss5, sizeof(movss5));
        Log("[grass] 21j-r1: VirtualProtect failed (error %lu) - no grass scan%s", err,
            w1 ? (back ? " (the entry call taken out again)" : " (the entry call could NOT be taken out)") : "");
        return;
    }
    g_grassOn = true;
    Log("[grass] 21j-r1: THE GRASS SCAN is in (log only): the grass draw rva 0x%08X - its entry call at 0x%08X (F3 0F 10 5D FC), its fade "
        "call at 0x%08X (F3 0F 10 40 0C); the grasspatch caller %s (returns to rva 0x%08X), the .prp caller %s (returns to rva 0x%08X, "
        "its maxDist from rva 0x%08X); the near scene end / start %s / %s, the sky's end numbers %s, start numbers %s, the 2020 option %s",
        fn, atEntry, atFade, patchOk ? "found" : "NOT found", patchOk ? static_cast<unsigned>(g_grassPatchRet - ExeBase()) : 0u,
        propOk ? "found" : "NOT found", propOk ? static_cast<unsigned>(g_grassPropRet - ExeBase()) : 0u, propEnd,
        g_grassX ? "found" : "NOT found", g_grassY ? "found" : "NOT found", (g_grassNearC && g_grassNearD) ? "found" : "NOT found",
        (g_grassNearA && g_grassNearB) ? "found" : "NOT found", g_grassOpt ? "found" : "NOT found");
    GrassNearLine();
}

const char kFovLua[] = R"BFXLUA(-- BF1GameExt VIDEO OPTIONS' FIELD OF VIEW (21i-r16): run while the game makes ifs_opt_pcvideo (ScriptCB_AddIFScreen - the
-- shell's and a match's options), before the screen's objects are made. A FIELD OF VIEW row under LOD DISTANCE: copies of the
-- LOD DISTANCE title, button and slider, one row down (the rows after it are laid out again by the game's own Enter). 80 -
-- 120 % of the game's 48 deg in 5 % steps (left / right; A one step up, round; the mouse on the slider). Single player only:
-- in a split screen match the row is hidden ("fov split"). BF1GameExt keeps the value ("fov get / set", FovScale).
function bfxf_Ask(s)
	return ScriptCB_GetPlatform("BFXQ: fov " .. s)
end

function bfxf_Log(s)
	bfxf_Ask("log " .. s)
end

function bfxf_Copy(t)
	if (type(t) ~= "table") then
		return t
	end
	local c = {}
	local k, v
	for k, v in t do
		c[k] = bfxf_Copy(v)
	end
	return c
end

function bfxf_Show(this)
	if (not this.buttons.bfxfov) then
		return
	end
	local pct = bfxf_Ask("get")
	if ((not pct) or (pct < 80)) then
		pct = 100
	end
	this.sliders.bfxfov.thumbposn = (pct - 80) / 40
	HSlider_MoveThumb(this.sliders.bfxfov)
	IFText_fnSetUString(this.buttonlabels.bfxfov, ScriptCB_tounicode(format("FIELD OF VIEW  %d%%", pct)))
end

function bfxf_Set(this, pct)
	if (pct < 80) then
		pct = 80
	elseif (pct > 120) then
		pct = 120
	end
	bfxf_Ask(format("set %d", pct))
	bfxf_Show(this)
end

-- shown in the shell and in a single player match; hidden in a split screen match (2 - 4 players keep the game's FOV)
function bfxf_Refresh(this)
	local b = this.buttons
	if (not b.bfxfov) then
		return
	end
	local hide = (bfxf_Ask("split") == 1)
	if (hide) then
		b.bfxfov.hidden = 1
		this.buttonlabels.bfxfov.hidden = 1
		IFObj_fnSetVis(this.sliders.bfxfov, nil)
	else
		b.bfxfov.hidden = nil
		this.buttonlabels.bfxfov.hidden = nil
		IFObj_fnSetVis(this.sliders.bfxfov, 1)
	end
	ShowHideVerticalText(this.buttonlabels, ifs_opt_pcvideo_vbutton_layout)
	ShowHideVerticalButtons(this.buttons, ifs_opt_pcvideo_vbutton_layout)
	bfxf_Show(this)
	return hide
end

local this = ifs_opt_pcvideo
local l = ifs_opt_pcvideo_vbutton_layout
if (this and l and l.buttonlist and this.buttons and this.buttonlabels and this.sliders and this.buttons.loddistance and
	this.buttons.viewdistance and this.buttonlabels.loddistance and this.buttonlabels.viewdistance and this.sliders.loddistance and
	(not this.buttons.bfxfov) and (bfxf_Ask("menu") == 1)) then
	local b = this.buttons
	local t = this.buttonlabels
	local at = nil
	local i = 1
	while (l.buttonlist[i]) do
		if (l.buttonlist[i].tag == "loddistance") then
			at = i
		end
		i = i + 1
	end
	if (at) then
		local step = b.loddistance.y - b.viewdistance.y
		local tstep = t.loddistance.y - t.viewdistance.y
		b.bfxfov = bfxf_Copy(b.loddistance)
		b.bfxfov.tag = "bfxfov"
		b.bfxfov.y = b.loddistance.y + step
		if (b.bfxfov.label) then
			b.bfxfov.label.string = ""
		end
		t.bfxfov = bfxf_Copy(t.loddistance)
		t.bfxfov.y = t.loddistance.y + tstep
		t.bfxfov.string = nil
		t.bfxfov.ustring = ScriptCB_tounicode("FIELD OF VIEW")
		local down = b.loddistance.link_down
		b.loddistance.link_down = "bfxfov"
		b.bfxfov.link_up = "loddistance"
		b.bfxfov.link_down = down
		if (down and b[down]) then
			b[down].link_up = "bfxfov"
		end
		-- (moved down by hand: Lua 4's tinsert would give the list an "n" field)
		local j = getn(l.buttonlist)
		while (j > at) do
			l.buttonlist[j + 1] = l.buttonlist[j]
			j = j - 1
		end
		l.buttonlist[at + 1] = { tag = "bfxfov", title = "", string = "" }
		this.sliders.bfxfov = bfxf_Copy(this.sliders.loddistance)
		this.sliders.bfxfov.y = this.sliders.loddistance.y + step
		this.sliders.bfxfov.tag = "bfxfov"

		this.bfxfOrig = {
			Enter = this.Enter,
			Input_Accept = this.Input_Accept,
			Input_GeneralLeft = this.Input_GeneralLeft,
			Input_GeneralRight = this.Input_GeneralRight,
			Update = this.Update,
		}
		this.Enter = function(this, bFwd)
			this.bfxfOrig.Enter(this, bFwd)
			local hidden = bfxf_Refresh(this)
			bfxf_Log(format("VIDEO OPTIONS up: FIELD OF VIEW %s", (hidden and "hidden (a split screen match)") or "shown"))
		end
		this.Input_Accept = function(this)
			if (gMouseOverHorz and gMouseOverHorz.MousePercentage and (gMouseOverHorz.tag == "bfxfov")) then
				bfxf_Set(this, 80 + floor(gMouseOverHorz.MousePercentage * 8 + 0.5) * 5)
				return
			end
			if ((this.CurButton == "bfxfov") and (this.res_menu_state == 0) and (this.fsaa_menu_state == 0)) then
				local pct = (bfxf_Ask("get") or 100) + 5
				if (pct > 120) then
					pct = 80
				end
				ScriptCB_SndPlaySound("shell_menu_ok")
				bfxf_Set(this, pct)
				return
			end
			this.bfxfOrig.Input_Accept(this)
		end
		this.Input_GeneralLeft = function(this)
			if ((this.CurButton == "bfxfov") and (this.res_menu_state == 0) and (this.fsaa_menu_state == 0)) then
				ScriptCB_SndPlaySound("shell_menu_ok")
				bfxf_Set(this, (bfxf_Ask("get") or 100) - 5)
				return
			end
			this.bfxfOrig.Input_GeneralLeft(this)
		end
		this.Input_GeneralRight = function(this)
			if ((this.CurButton == "bfxfov") and (this.res_menu_state == 0) and (this.fsaa_menu_state == 0)) then
				ScriptCB_SndPlaySound("shell_menu_ok")
				bfxf_Set(this, (bfxf_Ask("get") or 100) + 5)
				return
			end
			this.bfxfOrig.Input_GeneralRight(this)
		end
		this.Update = function(this, fDt)
			this.bfxfOrig.Update(this, fDt)
			local a = 0.5
			if ((this.CurButton == "bfxfov") or (gMouseOverHorz == this.sliders.bfxfov)) then
				a = 1.0
			end
			HSlider_fnSetAlpha(this.sliders.bfxfov, a)
		end
		bfxf_Log(format("FIELD OF VIEW row added to VIDEO OPTIONS under LOD DISTANCE (row %d, %g apart)", at + 1, step))
	else
		bfxf_Log("LOD DISTANCE not in the layout - no FIELD OF VIEW row")
	end
end
)BFXLUA";

// 21i-r16: VIDEO OPTIONS' FIELD OF VIEW row, run while the game makes ifs_opt_pcvideo
void FovRun(void* L)
{
    const int r = RunLua(L, kFovLua, sizeof(kFovLua) - 1, "=BF1GameExt fov", true);
    if (FovBudget())
        Log("[fov] %lu ms: the FIELD OF VIEW script %s (the game making ifs_opt_pcvideo)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message");
}

void IAOptsRun(void* L, const char* screen)
{
    ++g_iaOptsRuns;
    char head[256];
    snprintf(head, sizeof(head),
             "gBFXO_Now = \"%s\"\ngBFXO_IAPage = %s\ngBFXO_SplitRow = %s\ngBFXO_AimRow = %s\ngBFXO_WheelRow = %s\n", screen,
             g_iaPageIni ? "1" : "nil", g_splitMenuIni ? "1" : "nil", g_aimMenuIni ? "1" : "nil", g_wheelMenuIni ? "1" : "nil");
    if (RunLua(L, head, strlen(head), "=BF1GameExt ia options settings", true) != 0)
        return;
    const int r = RunLua(L, kIAOptsLua, sizeof(kIAOptsLua) - 1, "=BF1GameExt ia options", true);
    if (g_iaLines < kIaMaxLines)
    {
        ++g_iaLines;
        Log("[iaopts] lua %lu ms: the Instant Action GAME OPTIONS script %s (the game making %s, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", screen, g_iaOptsRuns);
    }
}

// ---------------------------------------------------------------------------------------------------------------------------
// 21j-r17: THE OPTIONS MENU'S MODS ROW ([Splitscreen] ModsMenu=1). kModsLua runs while the game makes ifs_opt_contmain (the
// main menu's and a match's OPTIONS); its questions are "BFXQ: mods ..." (ModsQuery; "mods addons" needs the Lua state:
// ModsAddonsRun), its lines "BFXM: " ([mods] lua lines).
// ---------------------------------------------------------------------------------------------------------------------------
const char kModsLua[] = R"BFXLUA(-- BF1GameExt MODS (21j-r17 / r18): the options menu's MODS row ([Splitscreen] ModsMenu=1) - 21j-r18: in the main menu's OPTIONS
-- only (player 1's, before a match; never in a match - nobody switches the mods mid-game). MODS: SPRINT ON / OFF and RAGDOLL
-- EFFECTS ON / OFF (BF1GameExt's own, in effect at once, saved in BF1GameExt.ini - "BFXQ: mods sprint | ragdoll [set N]"),
-- ADDON FOLDER: what GameData\AddOn holds - every folder, the missions each adds, how many mods are installed and how many of
-- the game's addon mission slots are used ("BFXQ: mods addons": BF1GameExt reads the folder and the game's own mission list and
-- hands them over as gBFXM_A / gBFXM_L). Run by BF1GameExt while the game makes ifs_opt_contmain (before its objects are made);
-- both pages are made right after it (its ScriptCB_EndIFScreen). [mods] lua lines ("BFXM: ").
function bfxm_Log(s)
	ScriptCB_GetPlatform("BFXM: " .. s)
end

function bfxm_Ask(s)
	return ScriptCB_GetPlatform("BFXQ: mods " .. s)
end

function bfxm_Copy(t)
	if (type(t) ~= "table") then
		return t
	end
	local c = {}
	local k, v
	for k, v in t do
		c[k] = bfxm_Copy(v)
	end
	return c
end

function bfxm_InMatch()
	return not (ScriptCB_GetShellActive and ScriptCB_GetShellActive())
end

function bfxm_Where()
	if (bfxm_InMatch()) then
		return "a match"
	end
	return "the main menu"
end

-- 21j-r18: the main menu's OPTIONS only (in a match the row is not even made; this keeps it hidden there anyway)
function bfxm_Shown()
	if (bfxm_InMatch()) then
		return nil
	end
	return 1
end

-- ---------------------------------------------------------------------------------------------------------------------------
-- OPTIONS (ifs_opt_contmain): the MODS row - a copy of CREDITS' button (not made yet), right above it in the list
-- ---------------------------------------------------------------------------------------------------------------------------
function bfxm_AddRow(this, l)
	local b = this.buttons
	if (b.bfxmods or (not b.credits) or (type(b.credits.label) ~= "table") or (not l.buttonlist)) then
		return nil
	end
	local at = nil
	local i = 1
	while (l.buttonlist[i]) do
		if (l.buttonlist[i].tag == "credits") then
			at = i
		end
		i = i + 1
	end
	if (not at) then
		return nil
	end
	b.bfxmods = bfxm_Copy(b.credits)
	b.bfxmods.tag = "bfxmods"
	b.bfxmods.hidden = nil
	b.bfxmods.label.string = nil
	b.bfxmods.label.ustring = ScriptCB_tounicode("MODS")
	local up = b.credits.link_up
	b.bfxmods.link_up = up
	b.bfxmods.link_down = "credits"
	b.credits.link_up = "bfxmods"
	if (up and b[up]) then
		b[up].link_down = "bfxmods"
	end
	-- (moved down by hand: Lua 4's tinsert would give the list an "n" field, and the game walks it with "for k, v")
	local j = getn(l.buttonlist)
	while (j >= at) do
		l.buttonlist[j + 1] = l.buttonlist[j]
		j = j - 1
	end
	l.buttonlist[at] = { tag = "bfxmods", string = "" }
	return at
end

-- after the game's own Enter: MODS shown in the main menu (21j-r18: hidden in a match), the rows laid out again; back from MODS
-- the highlight goes back to MODS
function bfxm_OptsLayout(this, bFwd)
	local b = this.buttons
	if ((not b) or (not b.bfxmods) or (not ifs_opt_top_vbutton_layout)) then
		return
	end
	local show = bfxm_Shown() and ifs_bfx_mods
	if (show) then
		b.bfxmods.hidden = nil
	else
		b.bfxmods.hidden = 1
	end
	local cur = this.CurButton
	if ((not bFwd) and gBFXM_BackToOpts and show) then
		cur = "bfxmods"
	end
	gBFXM_BackToOpts = nil
	local first = ShowHideVerticalButtons(b, ifs_opt_top_vbutton_layout)
	if ((not cur) or (not b[cur]) or b[cur].hidden) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(cur)
	local now = 0
	if (show) then
		now = 1
	end
	if (gBFXM_OptsShown ~= now) then
		gBFXM_OptsShown = now
		local vp = 0
		if (bfxm_InMatch() and ScriptCB_GetPausingViewport) then
			vp = ScriptCB_GetPausingViewport()
		end
		bfxm_Log(format("OPTIONS up (%s, the pausing viewport %s): MODS %s", bfxm_Where(), tostring(vp),
			(show and "shown") or "hidden (a match)"))
	end
end

-- ---------------------------------------------------------------------------------------------------------------------------
-- MODS (ifs_bfx_mods): SPRINT, RAGDOLL EFFECTS, ADDON FOLDER
-- ---------------------------------------------------------------------------------------------------------------------------
function bfxm_State(what)
	local v = bfxm_Ask(what)
	if ((v ~= 0) and (v ~= 1)) then
		return -1
	end
	return v
end

function bfxm_OnOff(v)
	if (v == 1) then
		return "ON"
	elseif (v == 0) then
		return "OFF"
	end
	return "NOT AVAILABLE"
end

function bfxm_SetText(this)
	local b = this.buttons
	gBFXM_Sprint = bfxm_State("sprint")
	gBFXM_Ragdoll = bfxm_State("ragdoll")
	local n = bfxm_Ask("count")
	local a = "ADDON FOLDER"
	if (n == 0) then
		a = "ADDON FOLDER: NO MODS"
	elseif (n == 1) then
		a = "ADDON FOLDER: 1 MOD"
	elseif (n and (n > 1)) then
		a = format("ADDON FOLDER: %d MODS", n)
	end
	RoundIFButtonLabel_fnSetUString(b.sprint, ScriptCB_tounicode("SPRINT: " .. bfxm_OnOff(gBFXM_Sprint)))
	RoundIFButtonLabel_fnSetUString(b.ragdoll, ScriptCB_tounicode("RAGDOLL EFFECTS: " .. bfxm_OnOff(gBFXM_Ragdoll)))
	RoundIFButtonLabel_fnSetUString(b.addons, ScriptCB_tounicode(a))
end

-- A / left / right on SPRINT or RAGDOLL EFFECTS: switched (nil: not one of them)
function bfxm_Toggle(this, sound)
	local c = this.CurButton
	local what = nil
	local name = nil
	local now = nil
	if (c == "sprint") then
		what = "sprint"
		name = "SPRINT"
		now = gBFXM_Sprint
	elseif (c == "ragdoll") then
		what = "ragdoll"
		name = "RAGDOLL EFFECTS"
		now = gBFXM_Ragdoll
	end
	if (not what) then
		return nil
	end
	if ((now ~= 0) and (now ~= 1)) then
		ifelm_shellscreen_fnPlaySound(this.errorSound)
		bfxm_Log(format("MODS: %s is not available this session - nothing changed", name))
		return 1
	end
	ifelm_shellscreen_fnPlaySound(sound or this.acceptSound)
	local v = bfxm_Ask(format("%s set %d", what, 1 - now))
	bfxm_SetText(this)
	bfxm_Log(format("MODS (%s): %s %s -> %s (BF1GameExt answered %s)", bfxm_Where(), name, bfxm_OnOff(now),
		bfxm_OnOff(1 - now), tostring(v)))
	return 1
end

function bfxm_ModsEnter(this, bFwd)
	gIFShellScreenTemplate_fnEnter(this, bFwd)
	local cur = this.CurButton
	local first = ShowHideVerticalButtons(this.buttons, bfxm_Layout)
	if (bFwd or (not cur) or (not this.buttons[cur])) then
		cur = first
	end
	this.CurButton = cur
	SetCurButton(cur)
	bfxm_SetText(this)
	if (bFwd) then
		bfxm_Log(format("MODS up (%s): SPRINT %s, RAGDOLL EFFECTS %s", bfxm_Where(), bfxm_OnOff(gBFXM_Sprint),
			bfxm_OnOff(gBFXM_Ragdoll)))
	end
end

function bfxm_ModsAccept(this)
	if (gShellScreen_fnDefaultInputAccept(this)) then
		return
	end
	if (this.CurButton == "addons") then
		ifelm_shellscreen_fnPlaySound(this.acceptSound)
		ifs_movietrans_PushScreen(ifs_bfx_addons)
		return
	end
	bfxm_Toggle(this, nil)
end

)BFXLUA"
R"BFXLUA(-- ---------------------------------------------------------------------------------------------------------------------------
-- ADDON FOLDER (ifs_bfx_addons): the folders in a list box, the selected one's missions, the totals and the limit
-- ---------------------------------------------------------------------------------------------------------------------------
function bfxm_ItemCreate(layout)
	local t = NewIFContainer {
		x = layout.x - 0.5 * layout.width,
		y = layout.y,
	}
	t.name = NewIFText {
		x = 8,
		y = layout.height * -0.5 + 2,
		halign = "left",
		valign = "vcenter",
		font = "gamefont_small",
		textw = layout.width * 0.62,
		texth = layout.height,
		nocreatebackground = 1,
		startdelay = random() * 0.5,
	}
	t.info = NewIFText {
		x = layout.width * 0.62,
		y = layout.height * -0.5 + 2,
		halign = "right",
		valign = "vcenter",
		font = "gamefont_small",
		textw = layout.width * 0.38 - 14,
		texth = layout.height,
		nocreatebackground = 1,
		startdelay = random() * 0.5,
	}
	return t
end

function bfxm_ItemPopulate(Dest, Data)
	if (Data) then
		IFText_fnSetUString(Dest.name, ScriptCB_tounicode(Data.name or ""))
		IFText_fnSetUString(Dest.info, ScriptCB_tounicode(Data.info or ""))
	end
	IFObj_fnSetVis(Dest, Data)
end

-- the selected folder's line under the box
function bfxm_ShowSel(this)
	local i = bfxm_ListLayout.SelectedIdx or 1
	local d = gBFXM_L[i]
	local s = ""
	if (d and d.m) then
		s = d.m
	end
	IFText_fnSetUString(this.bfxInfo.sel, ScriptCB_tounicode(s))
end

-- the folder and the game's list, read by BF1GameExt now (gBFXM_A / gBFXM_L), into the box and the lines under it
function bfxm_AddonsFill(this, bFwd)
	local rows = bfxm_Ask("addons")
	if ((not rows) or (type(gBFXM_A) ~= "table") or (type(gBFXM_L) ~= "table")) then
		gBFXM_A = { dir = 0, folders = 0, mods = 0, noaddme = 0, count = 0, cap = 0, stock = 512, raise = 4096, known = 0 }
		gBFXM_L = {}
		bfxm_Log("ADDON FOLDER: BF1GameExt handed nothing over (no answer) - an empty page")
	end
	local a = gBFXM_A
	if (getn(gBFXM_L) == 0) then
		local s = "NO ADDON FOLDER"
		if (a.dir == 1) then
			s = "THE ADDON FOLDER IS EMPTY"
		end
		gBFXM_L = { { name = s, info = "", m = "" } }
	end
	if (bFwd or (not bfxm_ListLayout.SelectedIdx) or (bfxm_ListLayout.SelectedIdx > getn(gBFXM_L))) then
		bfxm_ListLayout.SelectedIdx = 1
		bfxm_ListLayout.FirstShownIdx = 1
		bfxm_ListLayout.CursorIdx = 1
	end
	ListManager_fnFillContents(this.listbox, gBFXM_L, bfxm_ListLayout)
	local mods = a.mods or 0
	local s1 = format("%d MODS INSTALLED", mods)
	if (mods == 0) then
		s1 = "NO MODS INSTALLED"
	elseif (mods == 1) then
		s1 = "1 MOD INSTALLED"
	end
	if ((a.noaddme or 0) > 0) then
		s1 = s1 .. format(" - %d FOLDER%s NOT LOADED", a.noaddme, (a.noaddme == 1 and "") or "S")
	end
	if ((a.more or 0) > 0) then
		s1 = s1 .. format(" (THE FIRST %d LISTED)", getn(gBFXM_L))
	end
	local s2 = "ADDON MISSIONS: THE LIMIT IS NOT KNOWN ON THIS BUILD"
	local s3 = ""
	local frac = 0
	if ((a.known == 1) and (a.cap > 0)) then
		frac = a.count / a.cap
		local pct = format("%d%%", floor(frac * 100))
		if ((a.count > 0) and (frac < 0.01)) then
			pct = "UNDER 1%"
		end
		s2 = format("ADDON MISSIONS: %d OF %d USED (%s) - %d LEFT", a.count, a.cap, pct, a.cap - a.count)
		if (a.count >= a.cap) then
			s3 = "FULL - MISSIONS PAST THE LIMIT ARE NOT ADDED"
		elseif (a.cap > a.stock) then
			s3 = format("THE LIMIT RAISED BY BF1GAMEEXT (THE GAME'S OWN: %d)", a.stock)
		else
			s3 = format("THE GAME'S OWN LIMIT - [LIMITS] DLCMISSIONS=1 RAISES IT TO %d", a.raise or 4096)
		end
	end
	if (frac > 1) then
		frac = 1
	end
	IFText_fnSetUString(this.bfxInfo.mods, ScriptCB_tounicode(s1))
	IFText_fnSetUString(this.bfxInfo.missions, ScriptCB_tounicode(s2))
	IFText_fnSetUString(this.bfxInfo.limit, ScriptCB_tounicode(s3))
	local w = gBFXM_BarW
	local r = w * -0.5 + w * frac
	if ((a.count or 0) > 0) then
		r = max(r, w * -0.5 + 3)
	end
	IFImage_fnSetTexturePos(this.bfxInfo.barfg, w * -0.5, nil, r, nil)
	if (this.listbox.titleBarElement) then
		IFText_fnSetUString(this.listbox.titleBarElement, ScriptCB_tounicode("ADDON FOLDER"))
	end
	bfxm_ShowSel(this)
	if (bFwd) then
		bfxm_Log(format("ADDON FOLDER up (%s): %d row(s); %s; %s; %s", bfxm_Where(), getn(gBFXM_L), s1, s2,
			(s3 == "" and "-") or s3))
	end
end

-- ---------------------------------------------------------------------------------------------------------------------------
-- the two pages, made once per Lua state right after the game has finished OPTIONS
-- ---------------------------------------------------------------------------------------------------------------------------
function bfxm_Build()
	if (ifs_bfx_mods) then
		return
	end
	local w, h = ScriptCB_GetSafeScreenInfo()
	ifs_bfx_mods = NewIFShellScreen {
		nologo = 1,
		movieIntro = nil,
		movieBackground = nil,
		bNohelptext_backPC = 1,
		buttons = NewIFContainer {
			ScreenRelativeX = 0.6, -- where OPTIONS stands
			ScreenRelativeY = 0.55,
			y = 20,
		},
		Enter = function(this, bFwd)
			bfxm_ModsEnter(this, bFwd)
		end,
		Input_Accept = function(this)
			bfxm_ModsAccept(this)
		end,
		Input_Back = function(this)
			gBFXM_BackToOpts = 1
			ScriptCB_PopScreen()
		end,
		Input_GeneralUp = function(this)
			if (gShellScreen_fnDefaultInputUp(this)) then
				return
			end
			gDefault_Input_GeneralUp(this)
		end,
		Input_GeneralDown = function(this)
			if (gShellScreen_fnDefaultInputDown(this)) then
				return
			end
			gDefault_Input_GeneralDown(this)
		end,
		Input_GeneralLeft = function(this)
			bfxm_Toggle(this, this.selectSound)
		end,
		Input_GeneralRight = function(this)
			bfxm_Toggle(this, this.selectSound)
		end,
	}
	bfxm_Layout = {
		ySpacing = 5,
		width = 400,
		font = "gamefont_large",
		title = "ifs.Main.options",
		rotY = 40,
		buttonlist = {
			{ tag = "sprint" },
			{ tag = "ragdoll" },
			{ tag = "addons" },
		},
	}
	ifs_bfx_mods.CurButton = AddVerticalButtons(ifs_bfx_mods.buttons, bfxm_Layout)
	if (ifs_bfx_mods.buttons._titlebar_) then
		ifs_bfx_mods.buttons._titlebar_.string = nil
		ifs_bfx_mods.buttons._titlebar_.ustring = ScriptCB_tounicode("MODS")
	end
	ifs_bfx_mods.cancelbutton = CreateNegativeButton("common.back", "B", "_back")
	bfxm_SetText(ifs_bfx_mods)
	AddIFScreen(ifs_bfx_mods, "ifs_bfx_mods")

	-- ADDON FOLDER: the box (the map list's NewButtonWindow + ListManager), the lines and the bar under it
	local lh = 24
	local count = max(5, min(12, floor((h - 330) / lh)))
	local bw = min(620, floor(w * 0.62))
	local bh = count * lh + 30
	local by = -60
	gBFXM_BarW = bw - 40
	bfxm_ListLayout = {
		showcount = count,
		yHeight = lh,
		ySpacing = 0,
		width = bw - 35,
		x = 0,
		slider = 1,
		CreateFn = bfxm_ItemCreate,
		PopulateFn = bfxm_ItemPopulate,
	}
	ifs_bfx_addons = NewIFShellScreen {
		nologo = 1,
		movieIntro = nil,
		movieBackground = nil,
		bNohelptext_backPC = 1,
		listbox = NewButtonWindow {
			ZPos = 200,
			x = 0,
			y = by,
			ScreenRelativeX = 0.5,
			ScreenRelativeY = 0.5,
			width = bw,
			height = bh,
			titleText = "ifs.Main.options",
		},
		Enter = function(this, bFwd)
			gIFShellScreenTemplate_fnEnter(this, bFwd)
			bfxm_AddonsFill(this, bFwd)
		end,
		Input_Accept = function(this)
			if (gShellScreen_fnDefaultInputAccept(this)) then
				bfxm_ShowSel(this)
				return
			end
		end,
		Input_Back = function(this)
			ScriptCB_PopScreen()
		end,
		Input_GeneralUp = function(this)
			if (not gShellScreen_fnDefaultInputUp(this)) then
				ListManager_fnNavUp(this.listbox, gBFXM_L, bfxm_ListLayout)
			end
			bfxm_ShowSel(this)
		end,
		Input_GeneralDown = function(this)
			if (not gShellScreen_fnDefaultInputDown(this)) then
				ListManager_fnNavDown(this.listbox, gBFXM_L, bfxm_ListLayout)
			end
			bfxm_ShowSel(this)
		end,
		Input_LTrigger = function(this)
			ListManager_fnPageUp(this.listbox, gBFXM_L, bfxm_ListLayout)
			bfxm_ShowSel(this)
		end,
		Input_RTrigger = function(this)
			ListManager_fnPageDown(this.listbox, gBFXM_L, bfxm_ListLayout)
			bfxm_ShowSel(this)
		end,
		Input_GeneralLeft = function(this)
		end,
		Input_GeneralRight = function(this)
		end,
	}
	if (ifs_bfx_addons.listbox.titleBarElement) then
		ifs_bfx_addons.listbox.titleBarElement.string = nil
		ifs_bfx_addons.listbox.titleBarElement.ustring = ScriptCB_tounicode("ADDON FOLDER")
	end
	ListManager_fnInitList(ifs_bfx_addons.listbox, bfxm_ListLayout)
	local sh = ScriptCB_GetFontHeight("gamefont_small")
	local mh = ScriptCB_GetFontHeight("gamefont_medium")
	local y = by + bh * 0.5 + 6
	local info = NewIFContainer {
		ScreenRelativeX = 0.5,
		ScreenRelativeY = 0.5,
	}
	info.sel = NewIFText { y = y, textw = bw, texth = sh + 4, font = "gamefont_small", halign = "hcenter", valign = "vcenter",
		string = "", nocreatebackground = 1, }
	y = y + sh + 12
	info.mods = NewIFText { y = y, textw = bw, texth = mh + 4, font = "gamefont_medium", halign = "hcenter", valign = "vcenter",
		string = "", nocreatebackground = 1, }
	y = y + mh + 6
	info.missions = NewIFText { y = y, textw = bw, texth = mh + 4, font = "gamefont_medium", halign = "hcenter",
		valign = "vcenter", string = "", nocreatebackground = 1, }
	y = y + mh + 10
	info.barbg = NewIFImage { ZPos = 160, texture = "slider_bg", localpos_l = gBFXM_BarW * -0.5, localpos_t = y,
		localpos_r = gBFXM_BarW * 0.5, localpos_b = y + 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	info.barfg = NewIFImage { ZPos = 159, texture = "slider_fg", localpos_l = gBFXM_BarW * -0.5, localpos_t = y,
		localpos_r = gBFXM_BarW * -0.5, localpos_b = y + 12, uvs_l = 0, uvs_t = 0, uvs_r = 1, uvs_b = 1, }
	y = y + 18
	info.limit = NewIFText { y = y, textw = bw, texth = sh + 4, font = "gamefont_small", halign = "hcenter", valign = "vcenter",
		string = "", nocreatebackground = 1, }
	ifs_bfx_addons.bfxInfo = info
	ifs_bfx_addons.cancelbutton = CreateNegativeButton("common.back", "B", "_back")
	AddIFScreen(ifs_bfx_addons, "ifs_bfx_addons")
	bfxm_Log(format("the MODS pages made (%s): ifs_bfx_mods (SPRINT, RAGDOLL EFFECTS, ADDON FOLDER) and ifs_bfx_addons (a box "
		.. "%d x %d at %d, %d rows of %d, the safe screen %s x %s)", bfxm_Where(), bw, bh, by, count, lh, tostring(w),
		tostring(h)))
end

function bfxm_WrapEnd()
	if (gBFXM_EndWrapped) then
		return
	end
	gBFXM_EndWrapped = 1
	gBFXM_OrigEnd = ScriptCB_EndIFScreen
	ScriptCB_EndIFScreen = function(name)
		gBFXM_OrigEnd(name)
		if (name == "ifs_opt_contmain") then
			ScriptCB_EndIFScreen = gBFXM_OrigEnd
			bfxm_Build()
		end
	end
end

-- while the game makes ifs_opt_contmain: the row in, its Enter / Input_Accept wrapped (each kept on the screen's own table).
-- 21j-r18: the shell's only - the main menu (ifs_main) is made before OPTIONS there and never in a match's Lua state
local o = ifs_opt_contmain
local l = ifs_opt_top_vbutton_layout
if (not ifs_main) then
	bfxm_Log("OPTIONS in a match (no main menu in this Lua state): no MODS row - the main menu's OPTIONS only")
elseif ((type(o) ~= "table") or (type(l) ~= "table") or (type(o.buttons) ~= "table") or (not o.Enter) or (not o.Input_Accept)) then
	bfxm_Log("OPTIONS (ifs_opt_contmain) or its layout not as expected - no MODS row")
elseif (o.bfxmOrig) then
	bfxm_Log("OPTIONS already has the MODS row in this Lua state - left as it is")
else
	local at = bfxm_AddRow(o, l)
	if (not at) then
		bfxm_Log("OPTIONS: no CREDITS button to copy - no MODS row")
	else
		o.bfxmOrig = { Enter = o.Enter, Input_Accept = o.Input_Accept }
		o.Enter = function(this, bFwd)
			this.bfxmOrig.Enter(this, bFwd)
			bfxm_OptsLayout(this, bFwd)
		end
		o.Input_Accept = function(this)
			if ((this.CurButton == "bfxmods") and ifs_bfx_mods and bfxm_Shown()) then
				if (gShellScreen_fnDefaultInputAccept(this)) then
					return
				end
				ifelm_shellscreen_fnPlaySound(this.acceptSound)
				ifs_movietrans_PushScreen(ifs_bfx_mods)
				return
			end
			this.bfxmOrig.Input_Accept(this)
		end
		bfxm_WrapEnd()
		bfxm_Log(format("OPTIONS (%s): MODS row added above CREDITS (list entry %d of %d); the MODS pages are made right after "
			.. "OPTIONS", bfxm_Where(), at, getn(l.buttonlist)))
	end
end
)BFXLUA";

bool ModsBudget()
{
    if (g_modsLines >= kModsMaxLines)
        return false;
    ++g_modsLines;
    if (g_modsLines == kModsMaxLines)
        Log("[mods] line limit reached, going quiet");
    return true;
}

void ModsRun(void* L)
{
    ++g_modsRuns;
    const int r = RunLua(L, kModsLua, sizeof(kModsLua) - 1, "=BF1GameExt mods", true);
    if (ModsBudget())
        Log("[mods] %lu ms: the MODS script %s (the game making ifs_opt_contmain, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", g_modsRuns);
}

// 21j-r21 / r22: the console's tutorials on PC ([Splitscreen] TutorialsMenu / TutorialsMovieFit / TutorialsGC / TutorialsCampaign).
// Run while the game makes ifs_main and ifs_tutorials (ScriptCB_AddIFScreen, before the objects are made), after a settings chunk
// (gBFXU_Menu / gBFXU_Fit / gBFXU_GC / gBFXU_Camp); each part once per screen table, the others' names untouched (bfxu_* /
// gBFXU_* - no other script uses them). Two pieces (each under MSVC's 16380-byte literal limit), one array.
const char kTutorialsLua[] = R"BFXLUA(-- BF1GameExt TUTORIALS (21j-r21 / r22): the console's tutorials on PC. Run by BF1GameExt while the game makes ifs_main and
-- ifs_tutorials (before their objects are made); each part once per screen table. Set before it by BF1GameExt ([Splitscreen]):
--   gBFXU_Menu  TutorialsMenu: the main menu's TUTORIALS button (ifs_tutorials: 8 tutorials; ifs_viewtutorial plays one from
--               movies\tut<lang>.mvs, else movies\tuteng.mvs) - only ifs_main's Enter hides it on PC
--   gBFXU_Fit   TutorialsMovieFit: 0 = the game's own place, 1 = the movie in the background's black box at 4:3, 2 = the
--               whole box. The 2020 game reports a 4:3 screen to its scripts (ScriptCB_GetScreenInfo's 4th value is always
--               1.0), so the console's box (90, 60, 460 x 350 - made for 640 x 480) is taken in the 1024 x 624 interface:
--               the movie stood in the box's top left, its left and top under the frame. Every movie started with that box
--               (the tutorials viewer, the galactic conquest tutorial, the campaign's tutorials) goes in the box instead.
--   gBFXU_GC    TutorialsGC: picking GALACTIC CONQUEST plays its tutorial movie (tutorial08, the game's ifs_meta_tutorial) as
--               on the consoles before their new game's setup - the PC skips the consoles' new / load screen that pushed it
--   gBFXU_Camp  TutorialsCampaign: the campaigns' command post tutorials after Yoda's briefing of the first mission (Clone
--               Wars: tutorial01cw before nab1c_h, Galactic Civil War: tutorial01gcw before tat1i_h) - the 2020 missionlist
--               has them commented out ("no tutorials on the pc"), its Yoda screen still plays them
-- [tutorials] lua lines ("BFXU: ").
function bfxu_Log(s)
	ScriptCB_GetPlatform("BFXU: " .. s)
end

function bfxu_HasText(key)
	if (ScriptCB_getlocalizestr(key, 1)) then
		return 1
	end
	return nil
end

-- English names where the game has no text (the movies' own names in the shell's movie list)
gBFXU_Names = { "Command Posts", "Soldiers", "Vehicles", "Pilots", "Snipers", "Heavy Weapons", "Special Units",
	"Galactic Conquest" }

-- ---------------------------------------------------------------------------------------------------------------------
-- the movie's place: the console box -> the black box of the game's movie_BG2 (made for 640 x 480, stretched over the
-- screen - its inside 93, 66, 450 x 338 there, measured on a 3840 x 2160 screen); the movies are 800 x 600 (4:3)
-- ---------------------------------------------------------------------------------------------------------------------
function bfxu_FitBox(name, l, t, w, h)
	local right, bottom = ScriptCB_GetScreenInfo()
	right = right or 640
	bottom = bottom or 480
	-- the screen's height / width (ScriptCB_GetCurrentAspect: 0.5625 at 16:9)
	local ca = 0.5625
	if (ScriptCB_GetCurrentAspect) then
		local a = ScriptCB_GetCurrentAspect()
		if (a and (a > 0.2) and (a < 1.5)) then
			ca = a
		end
	end
	local fl, ft, fw, fh = 93, 66, 450, 338
	if (gBFXU_Fit == 1) then
		-- a 640-unit is (screen width / 640) pixels, a 480-unit (screen height / 480): 4:3 on the screen is
		-- fw = fh * (4 / 3) * (640 / 480) * (height / width)
		local wfit = fh * (16 / 9) * ca
		if (wfit <= fw) then
			fl = fl + (fw - wfit) * 0.5
			fw = wfit
		else
			local hfit = fw / ((16 / 9) * ca)
			ft = ft + (fh - hfit) * 0.5
			fh = hfit
		end
	end
	local nl, nt, nw, nh = fl * right / 640, ft * bottom / 480, fw * right / 640, fh * bottom / 480
	if ((gBFXU_FitLines or 0) < 24) then
		gBFXU_FitLines = (gBFXU_FitLines or 0) + 1
		bfxu_Log(format("movie %s: the console box (%d, %d, %d x %d) -> %.1f, %.1f, %.1f x %.1f (interface %d x %d, screen %.3f:1, "
			.. "TutorialsMovieFit=%d)", tostring(name), l, t, w, h, nl, nt, nw, nh, floor(right), floor(bottom), 1 / ca, gBFXU_Fit))
	end
	return nl, nt, nw, nh
end

-- the game's movie start, the console box moved (only that box: 90, 60, 460 / 450 x 350, not full screen)
if (not gBFXU_MovieWrap) then
	gBFXU_MovieWrap = function(movieName, loop, nextMovieName, fullscreen, left, top, width, height)
		if (gBFXU_Fit and (gBFXU_Fit > 0) and (not fullscreen) and (left == 90) and (top == 60) and (height == 350) and
			((width == 460) or (width == 450))) then
			left, top, width, height = bfxu_FitBox(movieName, left, top, width, height)
		end
		return gBFXU_StartMovie(movieName, loop, nextMovieName, fullscreen, left, top, width, height)
	end
end
if (gBFXU_Fit and (gBFXU_Fit > 0) and ifelem_shellscreen_fnStartMovie and
	(ifelem_shellscreen_fnStartMovie ~= gBFXU_MovieWrap)) then
	gBFXU_StartMovie = ifelem_shellscreen_fnStartMovie
	ifelem_shellscreen_fnStartMovie = gBFXU_MovieWrap
	bfxu_Log(format("movie box: ifelem_shellscreen_fnStartMovie wrapped - the console box goes in the background's black box "
		.. "(TutorialsMovieFit=%d)", gBFXU_Fit))
end

-- ---------------------------------------------------------------------------------------------------------------------
-- the main menu's TUTORIALS button
-- ---------------------------------------------------------------------------------------------------------------------
-- after the game's ifs_main Enter (it hides TUTORIALS on PC, every Enter)
function bfxu_MainShow(this)
	local b = this.buttons
	if ((not b) or (not b.tutorials) or (not ifsmain_vbutton_layout)) then
		if (not gBFXU_NoButton) then
			gBFXU_NoButton = 1
			bfxu_Log("main menu: NO \"tutorials\" button in ifs_main - nothing shown")
		end
		return
	end
	if ((not ifs_tutorials) or (not ifs_viewtutorial)) then
		if (not gBFXU_NoScreen) then
			gBFXU_NoScreen = 1
			local t, v = "there", "there"
			if (not ifs_tutorials) then
				t = "NOT there"
			end
			if (not ifs_viewtutorial) then
				v = "NOT there"
			end
			bfxu_Log(format("main menu: TUTORIALS left hidden - ifs_tutorials %s, ifs_viewtutorial %s in this shell", t, v))
		end
		return
	end
	if (b.tutorials.hidden) then
		b.tutorials.hidden = nil
		ShowHideVerticalButtons(b, ifsmain_vbutton_layout)
	end
	local label = "the game's text"
	if (not bfxu_HasText("ifs.main.tutorials")) then
		if (b.tutorials.label) then
			IFText_fnSetUString(b.tutorials.label, ScriptCB_tounicode("Tutorials"))
		end
		label = "\"Tutorials\" (the game's ifs.main.tutorials is missing)"
	end
	if (not gBFXU_MainShown) then
		gBFXU_MainShown = 1
		local n = 0
		if (ifs_tutorials_vbutton_layout and ifs_tutorials_vbutton_layout.buttonlist) then
			n = getn(ifs_tutorials_vbutton_layout.buttonlist)
		end
		bfxu_Log(format("main menu: TUTORIALS button shown (label %s); TUTORIALS -> ifs_tutorials, %d tutorials", label, n))
	end
end

-- after the game's ifs_tutorials Enter (it sets the 8 names): English where the game has no text
function bfxu_TutTexts(this)
	local l = ifs_tutorials_vbutton_layout
	if ((not l) or (not l.buttonlist) or (not this.buttons)) then
		return
	end
	local n = getn(l.buttonlist)
	local have = 0
	local i
	for i = 1, n do
		local item = l.buttonlist[i]
		local btn = this.buttons[item.tag]
		if (item.name and bfxu_HasText(item.name)) then
			have = have + 1
		elseif (btn) then
			RoundIFButtonLabel_fnSetUString(btn, ScriptCB_tounicode(gBFXU_Names[i] or format("Tutorial %d", i)))
		end
	end
	local title = "the game's"
	if (not bfxu_HasText("ifs.tutorials.title")) then
		title = "\"Tutorials\" (ifs.tutorials.title is missing)"
		if (this.buttons._titlebar_) then
			IFText_fnSetUString(this.buttons._titlebar_, ScriptCB_tounicode("Tutorials"))
		end
	end
	if (not gBFXU_TutShown) then
		gBFXU_TutShown = 1
		local miss = ""
		if (have < n) then
			miss = format(" - %d in English (the game's text is missing)", n - have)
		end
		bfxu_Log(format("TUTORIALS page: %d of %d names the game's text%s, title %s", have, n, miss, title))
	end
end

-- the viewer: which movie, which file, the interface's size
function bfxu_ViewLog(this)
	local item = this.curItem
	if ((not item) or (not item.movie)) then
		bfxu_Log("tutorial viewer: opened with no tutorial picked (curItem nil)")
		return
	end
	local lang = ScriptCB_GetLanguage() or "english"
	local post = gMovieTutorialPostFix or ""
	local file = "movies\\tut" .. strlower(strsub(lang, 1, 3)) .. post .. ".mvs"
	local used = file
	local there = "there"
	if (ScriptCB_IsFileExist(file) == 0) then
		there = "not there"
		used = "movies\\tuteng" .. post .. ".mvs"
	end
	local right, bottom, b, w = ScriptCB_GetScreenInfo()
	bfxu_Log(format("tutorial viewer: %s from %s (%s %s), interface %d x %d, aspect %.3f", item.movie, used, file, there,
		floor(right or 0), floor(bottom or 0), w or 0))
end
)BFXLUA"
R"BFXLUA(
-- ---------------------------------------------------------------------------------------------------------------------
-- the main menu, the tutorials menu, the viewer (TutorialsMenu)
-- ---------------------------------------------------------------------------------------------------------------------
if (gBFXU_Menu and ifs_main and (not ifs_main.bfxtut)) then
	ifs_main.bfxtut = 1
	gBFXU_MainEnter = ifs_main.Enter
	ifs_main.Enter = function(this, bFwd)
		gBFXU_MainEnter(this, bFwd)
		bfxu_MainShow(this)
	end
	bfxu_Log("ifs_main: Enter wrapped - TUTORIALS shown after the game's Enter hides it")
end

if (gBFXU_Menu and ifs_tutorials and (not ifs_tutorials.bfxtut)) then
	ifs_tutorials.bfxtut = 1
	gBFXU_TutEnter = ifs_tutorials.Enter
	gBFXU_TutAccept = ifs_tutorials.Input_Accept
	ifs_tutorials.Enter = function(this, bFwd)
		gBFXU_TutEnter(this, bFwd)
		bfxu_TutTexts(this)
	end
	ifs_tutorials.Input_Accept = function(this)
		-- the PC's mouse first (BACK is "_back"), as the game's other PC screens - the game's own looks "_back" up as a
		-- tutorial number: tonumber gives nil, a Lua error (table index is nil), BACK does nothing
		if (gShellScreen_fnDefaultInputAccept and gShellScreen_fnDefaultInputAccept(this)) then
			return
		end
		local l = ifs_tutorials_vbutton_layout
		local i = this.CurButton and tonumber(this.CurButton)
		local item = nil
		if (i and l and l.buttonlist) then
			item = l.buttonlist[i]
		end
		if ((not item) or (not item.movie)) then
			bfxu_Log(format("TUTORIALS: accept on \"%s\" - not a tutorial, nothing played", tostring(this.CurButton)))
			return
		end
		bfxu_Log(format("TUTORIALS: tutorial %d picked - movie %s", i, item.movie))
		gBFXU_TutAccept(this)
	end
	bfxu_Log("ifs_tutorials: Enter / Input_Accept wrapped")
end

if (gBFXU_Menu and ifs_viewtutorial and (not ifs_viewtutorial.bfxtut)) then
	ifs_viewtutorial.bfxtut = 1
	gBFXU_ViewEnter = ifs_viewtutorial.Enter
	gBFXU_ViewUpdate = ifs_viewtutorial.Update
	gBFXU_ViewExit = ifs_viewtutorial.Exit
	gBFXU_ViewAccept = ifs_viewtutorial.Input_Accept
	gBFXU_ViewBack = ifs_viewtutorial.Input_Back
	ifs_viewtutorial.Enter = function(this, bFwd)
		gBFXU_ViewTime = 0
		gBFXU_ViewSkip = nil
		bfxu_ViewLog(this)
		gBFXU_ViewEnter(this, bFwd)
	end
	-- a click / Enter / Esc (A / B) stops it: logged as skipped, not as a movie that did not play
	if (gBFXU_ViewAccept) then
		ifs_viewtutorial.Input_Accept = function(this)
			gBFXU_ViewSkip = 1
			gBFXU_ViewAccept(this)
		end
	end
	if (gBFXU_ViewBack) then
		ifs_viewtutorial.Input_Back = function(this)
			gBFXU_ViewSkip = 1
			gBFXU_ViewBack(this)
		end
	end
	if (gBFXU_ViewUpdate) then
		ifs_viewtutorial.Update = function(this, fDt)
			gBFXU_ViewTime = (gBFXU_ViewTime or 0) + (fDt or 0)
			gBFXU_ViewUpdate(this, fDt)
		end
	end
	if (gBFXU_ViewExit) then
		ifs_viewtutorial.Exit = function(this, bFwd)
			local t = gBFXU_ViewTime or 0
			local name = "?"
			if (this.curItem and this.curItem.movie) then
				name = this.curItem.movie
			end
			if (gBFXU_ViewSkip) then
				bfxu_Log(format("tutorial viewer: %s stopped by the player after %.1f s", name, t))
			elseif (t < 0.5) then
				bfxu_Log(format("tutorial viewer: %s closed after %.2f s - its movie did NOT play (the game closes the viewer "
					.. "when its movie is not playing)", name, t))
			else
				bfxu_Log(format("tutorial viewer: %s closed after %.1f s - the movie's end", name, t))
			end
			gBFXU_ViewExit(this, bFwd)
		end
	end
	bfxu_Log("ifs_viewtutorial: Enter / Update / Exit / Input_Accept / Input_Back wrapped (its log lines)")
end

-- ---------------------------------------------------------------------------------------------------------------------
-- galactic conquest's tutorial movie (TutorialsGC): GALACTIC CONQUEST in single player (ifs_sp's "meta") -> the setup
-- screen (ifs_meta_opts) as before, and right over it the game's ifs_meta_tutorial (tutorial08 from tut<lang>.mvs); the
-- movie over, or a click / Enter / Esc (A / B), and the setup screen is back (the game's own fnQuit pushed another
-- ifs_meta_opts and its BACK popped to ifs_meta_new_load, the consoles' screen the PC never opens)
-- ---------------------------------------------------------------------------------------------------------------------
if (gBFXU_GC and ifs_sp and (not ifs_sp.bfxtut)) then
	ifs_sp.bfxtut = 1
	gBFXU_SpAccept = ifs_sp.Input_Accept
	ifs_sp.Input_Accept = function(this)
		if (this.CurButton == "meta") then
			gBFXU_GCPending = 1
		end
		gBFXU_SpAccept(this)
	end
	bfxu_Log("ifs_sp: Input_Accept wrapped - GALACTIC CONQUEST plays its tutorial movie first")
end

if (gBFXU_GC and ifs_meta_opts and (not ifs_meta_opts.bfxtut)) then
	ifs_meta_opts.bfxtut = 1
	gBFXU_OptsEnter = ifs_meta_opts.Enter
	ifs_meta_opts.Enter = function(this, bFwd)
		gBFXU_OptsEnter(this, bFwd)
		if (bFwd and gBFXU_GCPending) then
			gBFXU_GCPending = nil
			if (not ifs_meta_tutorial) then
				bfxu_Log("galactic conquest: NO ifs_meta_tutorial in this shell - no tutorial movie")
			elseif (ScriptCB_IsMetagameStateSaved()) then
				bfxu_Log("galactic conquest: a conquest in progress - no tutorial movie")
			else
				gBFXU_GCIntro = 1
				gBFXU_GCQuit = nil
				gBFXU_GCSkip = nil
				gBFXU_GCTime = 0
				local split = "single player"
				if (ScriptCB_IsSplitscreen()) then
					split = "split screen"
				end
				bfxu_Log(format("galactic conquest (%s): its tutorial movie (ifs_meta_tutorial, tutorial08) over the setup screen", split))
				ScriptCB_PushScreen("ifs_meta_tutorial")
			end
		end
	end
	bfxu_Log("ifs_meta_opts: Enter wrapped - the tutorial movie goes over it when GALACTIC CONQUEST was picked")
end

if (gBFXU_GC and ifs_meta_tutorial and (not ifs_meta_tutorial.bfxtut)) then
	ifs_meta_tutorial.bfxtut = 1
	gBFXU_MetaTutQuit = ifs_meta_tutorial_fnQuit
	gBFXU_MetaTutEnter = ifs_meta_tutorial.Enter
	gBFXU_MetaTutBack = ifs_meta_tutorial.Input_Back
	gBFXU_MetaTutAccept = ifs_meta_tutorial.Input_Accept
	gBFXU_MetaTutUpdate = ifs_meta_tutorial.Update
	gBFXU_MetaTutExit = ifs_meta_tutorial.Exit
	ifs_meta_tutorial.Enter = function(this, bFwd)
		if (not gBFXU_GCIntro) then
			gBFXU_GCQuit = nil
		end
		gBFXU_MetaTutEnter(this, bFwd)
	end
	-- the movie over (its Update) or skipped (its Input_Accept): back to the setup screen under it - once, also when
	-- asked again before the screen has gone (gBFXU_GCQuit stays until the movie screen is opened again)
	ifs_meta_tutorial_fnQuit = function(this)
		if (gBFXU_GCQuit) then
			return
		end
		if (not gBFXU_GCIntro) then
			gBFXU_MetaTutQuit(this)
			return
		end
		gBFXU_GCQuit = 1
		ScriptCB_PopScreen()
		-- the setup screen has no music of its own: single player's again (the movie screen stopped it)
		ScriptCB_SetShellMusic((ifs_sp and ifs_sp.music) or "shell_soundtrack")
	end
	ifs_meta_tutorial.Input_Back = function(this)
		if (gBFXU_GCIntro) then
			gBFXU_GCSkip = 1
			ifs_meta_tutorial_fnQuit(this)
		else
			gBFXU_MetaTutBack(this)
		end
	end
	ifs_meta_tutorial.Input_Accept = function(this)
		if (gBFXU_GCIntro) then
			gBFXU_GCSkip = 1
		end
		gBFXU_MetaTutAccept(this)
	end
	ifs_meta_tutorial.Update = function(this, fDt)
		gBFXU_GCTime = (gBFXU_GCTime or 0) + (fDt or 0)
		gBFXU_MetaTutUpdate(this, fDt)
	end
	ifs_meta_tutorial.Exit = function(this, bFwd)
		if (gBFXU_GCIntro) then
			local t = gBFXU_GCTime or 0
			if (gBFXU_GCSkip) then
				bfxu_Log(format("galactic conquest: tutorial08 skipped by the player after %.1f s - the setup screen", t))
			elseif (t < 0.5) then
				bfxu_Log(format("galactic conquest: tutorial08 closed after %.2f s - its movie did NOT play", t))
			else
				bfxu_Log(format("galactic conquest: tutorial08 closed after %.1f s - the setup screen", t))
			end
		end
		gBFXU_GCIntro = nil
		gBFXU_MetaTutExit(this, bFwd)
	end
	bfxu_Log("ifs_meta_tutorial: its quit / Enter / Input_Accept / Input_Back / Update / Exit wrapped (back to the setup screen)")
end

-- ---------------------------------------------------------------------------------------------------------------------
-- the campaigns' command post tutorials (TutorialsCampaign): the consoles' missionlist lines, put back
-- ---------------------------------------------------------------------------------------------------------------------
function bfxu_CampTut(list, listName, map, movie)
	if (not list) then
		return
	end
	local i
	for i = 1, getn(list) do
		local m = list[i]
		if (m and (m.mapluafile == map)) then
			if (m.outtromovie) then
				if (m.outtromovie ~= movie) then
					bfxu_Log(format("campaign: %s mission %d (%s) has outtromovie %s already - left as it is", listName, i, map,
						tostring(m.outtromovie)))
				end
				return
			end
			m.outtromovie = movie
			m.outtromovie_left = 90
			m.outtromovie_top = 60
			m.outtromovie_width = 460
			m.outtromovie_height = 350
			m.outtromovielocalized = 1
			bfxu_Log(format("campaign: %s mission %d (%s) - %s after Yoda's briefing, before the mission", listName, i, map, movie))
			return
		end
	end
	bfxu_Log(format("campaign: %s has no %s - no tutorial there", listName, map))
end

if (gBFXU_Camp) then
	bfxu_CampTut(SPCampaign_CW, "the Clone Wars campaign", "nab1c_h", "tutorial01cw")
	bfxu_CampTut(SPCampaign_GCW, "the Galactic Civil War campaign", "tat1i_h", "tutorial01gcw")
end
)BFXLUA";

bool TutBudget()
{
    if (g_tutLines >= kTutMaxLines)
        return false;
    ++g_tutLines;
    if (g_tutLines == kTutMaxLines)
        Log("[tutorials] line limit reached (%d), going quiet", kTutMaxLines);
    return true;
}

// 21j-r22: any of the tutorials' switches on - the script runs
bool TutorialsAnyOn()
{
    return g_tutorialsIni || g_tutFitIni > 0 || g_tutGCIni || g_tutCampIni;
}

void TutorialsRun(void* L, const char* screen)
{
    ++g_tutRuns;
    char head[128];
    snprintf(head, sizeof(head), "gBFXU_Menu = %s\ngBFXU_Fit = %d\ngBFXU_GC = %s\ngBFXU_Camp = %s\n",
             g_tutorialsIni ? "1" : "nil", g_tutFitIni, g_tutGCIni ? "1" : "nil", g_tutCampIni ? "1" : "nil");
    if (RunLua(L, head, strlen(head), "=BF1GameExt tutorials settings", true) != 0)
    {
        if (TutBudget())
            Log("[tutorials] %lu ms: the TUTORIALS settings chunk FAILED - script not run (the game making %s)",
                ScanMsSinceAttach(), screen);
        return;
    }
    const int r = RunLua(L, kTutorialsLua, sizeof(kTutorialsLua) - 1, "=BF1GameExt tutorials", true);
    if (TutBudget())
        Log("[tutorials] %lu ms: the TUTORIALS script %s (the game making %s, run %lu)", ScanMsSinceAttach(),
            r == 0 ? "ran" : "FAILED - the game's Lua error output has the message", screen, g_tutRuns);
}

// one folder of GameData\AddOn
struct ModsFolder
{
    char name[MAX_PATH]; // as on disk
    bool addme;          // <name>\addme.script is there: the game runs it (its addon scan, GOG 1.3 0x004C6E60)
    int missions;        // entries of the game's addon mission list that came from this folder
    bool listFull;
    char list[176]; // their names, ", " between (", ..." when the rest does not fit)
};
constexpr int kModsMaxFolders = 1024;
ModsFolder* g_modsFolders = nullptr; // on the process heap, kModsMaxFolders of them
int g_modsFolderCount = 0;
int g_modsFolderMore = 0; // folders past kModsMaxFolders (not listed - counted still)
int g_modsNoAddme = 0;    // folders without an addme.script (the game does not load them)
char g_modsAddonDir[MAX_PATH] = "";
bool g_modsDirFound = false;
bool g_modsFoldersLogged = false;

int ModsNameCmp(const void* a, const void* b)
{
    return _stricmp(static_cast<const ModsFolder*>(a)->name, static_cast<const ModsFolder*>(b)->name);
}

// GameData\AddOn's folders as the game's addon scan finds them (its _fullpath of "AddOn\*" from the current directory: every
// folder but . and ..), sorted by name. The answer: how many have an addme.script - the mods the game loads.
int ModsScanFolders()
{
    g_modsFolderCount = 0;
    g_modsFolderMore = 0;
    g_modsNoAddme = 0;
    g_modsDirFound = false;
    if (!g_modsFolders)
        g_modsFolders = static_cast<ModsFolder*>(
            HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ModsFolder) * static_cast<size_t>(kModsMaxFolders)));
    if (!g_modsFolders)
        return 0;
    const DWORD n = GetFullPathNameA("AddOn", static_cast<DWORD>(sizeof(g_modsAddonDir)), g_modsAddonDir, nullptr);
    if (!n || n >= sizeof(g_modsAddonDir))
    {
        g_modsAddonDir[0] = 0;
        return 0;
    }
    const DWORD attr = GetFileAttributesA(g_modsAddonDir);
    g_modsDirFound = attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
    if (!g_modsDirFound)
        return 0;
    char pattern[MAX_PATH + 8];
    snprintf(pattern, sizeof(pattern), "%s\\*", g_modsAddonDir);
    WIN32_FIND_DATAA fd;
    const HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return 0;
    int mods = 0;
    do
    {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || !strcmp(fd.cFileName, ".") || !strcmp(fd.cFileName, ".."))
            continue;
        char script[MAX_PATH * 2 + 32];
        snprintf(script, sizeof(script), "%s\\%s\\addme.script", g_modsAddonDir, fd.cFileName);
        const DWORD sa = GetFileAttributesA(script);
        const bool addme = sa != INVALID_FILE_ATTRIBUTES && !(sa & FILE_ATTRIBUTE_DIRECTORY);
        if (addme)
            ++mods;
        else
            ++g_modsNoAddme;
        if (g_modsFolderCount >= kModsMaxFolders)
        {
            ++g_modsFolderMore;
            continue;
        }
        ModsFolder& f = g_modsFolders[g_modsFolderCount++];
        memset(&f, 0, sizeof(f));
        snprintf(f.name, sizeof(f.name), "%s", fd.cFileName);
        f.addme = addme;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    qsort(g_modsFolders, static_cast<size_t>(g_modsFolderCount), sizeof(ModsFolder), ModsNameCmp);
    return mods;
}

// the AddOn folder an entry's path names (its last part - the addon scan hands every addme the full path of its folder);
// nullptr when it is not one of the folders there now
ModsFolder* ModsFolderOf(const char* path)
{
    size_t len = strlen(path);
    while (len && (path[len - 1] == '\\' || path[len - 1] == '/'))
        --len;
    size_t start = len;
    while (start && path[start - 1] != '\\' && path[start - 1] != '/')
        --start;
    if (len == start || !g_modsFolderCount)
        return nullptr;
    static ModsFolder key; // only its name is read
    snprintf(key.name, sizeof(key.name), "%.*s", static_cast<int>(len - start), path + start);
    return static_cast<ModsFolder*>(
        bsearch(&key, g_modsFolders, static_cast<size_t>(g_modsFolderCount), sizeof(ModsFolder), ModsNameCmp));
}

void ModsListAdd(ModsFolder& f, const char* mission)
{
    if (f.listFull)
        return;
    const size_t used = strlen(f.list);
    const size_t need = strlen(mission) + (used ? 2 : 0);
    if (used + need + 6 >= sizeof(f.list))
    {
        snprintf(f.list + used, sizeof(f.list) - used, "%s...", used ? ", " : "");
        f.listFull = true;
        return;
    }
    snprintf(f.list + used, sizeof(f.list) - used, "%s%s", used ? ", " : "", mission);
}

// text for the page's Lua: upper case (the game's menu fonts), printable ASCII only (others '?'), at most maxChars of it
// (cut ones end in "..."), " and \ escaped for a Lua string
void ModsLuaText(char* out, size_t outLen, const char* in, size_t maxChars)
{
    if (!outLen)
        return;
    const size_t inLen = strlen(in);
    const bool cut = inLen > maxChars;
    const size_t take = cut ? (maxChars > 3 ? maxChars - 3 : 0) : inLen;
    size_t o = 0;
    for (size_t k = 0; k < take; ++k)
    {
        const unsigned char c = static_cast<unsigned char>(in[k]);
        const char ch = (c >= 0x20 && c < 0x7F) ? static_cast<char>(toupper(c)) : '?';
        const bool esc = ch == '"' || ch == '\\';
        if (o + (esc ? 2 : 1) >= outLen)
            break;
        if (esc)
            out[o++] = '\\';
        out[o++] = ch;
    }
    for (int d = 0; cut && d < 3 && o + 1 < outLen; ++d)
        out[o++] = '.';
    out[o] = 0;
}

void ModsAppend(char* buf, size_t size, size_t* at, const char* fmt, ...)
{
    if (*at + 1 >= size)
        return;
    va_list args;
    va_start(args, fmt);
    const int n = vsnprintf(buf + *at, size - *at, fmt, args);
    va_end(args);
    if (n > 0)
        *at += static_cast<size_t>(n) < size - *at ? static_cast<size_t>(n) : size - *at - 1;
}

// "BFXQ: mods addons": the ADDON FOLDER page's data - AddOn's folders, the game's addon mission list matched to them (each
// entry's folder path), the count against its cap - run into the asking script's Lua state as gBFXM_A (the totals) and
// gBFXM_L (one row per folder: name, info, m = the line under the box, ok = loaded, n = missions). From inside its
// ScriptCB_GetPlatform call, as the AddIFScreen scripts run from inside that call. The answer: the rows.
int ModsAddonsRun(void* L)
{
    if (!g_luaDoBuffer)
        return 0;
    const int mods = ModsScanFolders();
    uint32_t count = 0;
    uint32_t cap = 0;
    uint32_t stock = 512;
    uintptr_t list = 0;
    const bool known = LimitsDlcView(&count, &cap, &stock, &list);
    int other = 0;
    int unreadable = 0;
    char otherPath[MAX_PATH] = "";
    if (known)
    {
        char mission[64];
        char path[MAX_PATH];
        for (uint32_t i = 0; i < count; ++i)
        {
            if (!LimitsDlcEntry(list, i, mission, sizeof(mission), path, sizeof(path)))
            {
                ++unreadable;
                continue;
            }
            ModsFolder* f = ModsFolderOf(path);
            if (!f)
            {
                if (!other++)
                    snprintf(otherPath, sizeof(otherPath), "%s", path);
                continue;
            }
            ++f->missions;
            ModsListAdd(*f, mission[0] ? mission : "?");
        }
    }
    const size_t size = 2048 + static_cast<size_t>(g_modsFolderCount) * 760;
    char* text = static_cast<char*>(HeapAlloc(GetProcessHeap(), 0, size));
    if (!text)
        return 0;
    size_t at = 0;
    ModsAppend(text, size, &at,
               "gBFXM_A = { dir = %d, folders = %d, mods = %d, noaddme = %d, count = %u, cap = %u, stock = %u, raise = %u, "
               "known = %d, other = %d, more = %d }\ngBFXM_L = {\n",
               g_modsDirFound ? 1 : 0, g_modsFolderCount + g_modsFolderMore, mods, g_modsNoAddme,
               static_cast<unsigned>(count), static_cast<unsigned>(cap), static_cast<unsigned>(stock), 4096u, known ? 1 : 0,
               other, g_modsFolderMore);
    for (int k = 0; k < g_modsFolderCount; ++k)
    {
        const ModsFolder& f = g_modsFolders[k];
        char name[128];
        ModsLuaText(name, sizeof(name), f.name, 34);
        char info[48];
        char line[256];
        if (!f.addme)
        {
            snprintf(info, sizeof(info), "NOT LOADED");
            snprintf(line, sizeof(line), "NO ADDME.SCRIPT IN THIS FOLDER - THE GAME DOES NOT LOAD IT");
        }
        else if (!known)
        {
            snprintf(info, sizeof(info), "LOADED");
            snprintf(line, sizeof(line), "LOADED (THE ADDON MISSION LIST IS NOT READ ON THIS BUILD)");
        }
        else if (!f.missions)
        {
            snprintf(info, sizeof(info), "NO MISSIONS");
            snprintf(line, sizeof(line), "LOADED - ITS ADDME.SCRIPT ADDS NO MISSIONS");
        }
        else
        {
            snprintf(info, sizeof(info), f.missions == 1 ? "%d MISSION" : "%d MISSIONS", f.missions);
            char missions[232];
            ModsLuaText(missions, sizeof(missions), f.list, 72); // the line under the box: about 80 characters fit
            snprintf(line, sizeof(line), "MISSIONS: %s", missions);
        }
        ModsAppend(text, size, &at, "{ name = \"%s\", info = \"%s\", m = \"%s\", ok = %d, n = %d },\n", name, info, line,
                   f.addme ? 1 : 0, f.missions);
    }
    ModsAppend(text, size, &at, "}\n");
    const int r = RunLua(L, text, at, "=BF1GameExt mods data", true);
    HeapFree(GetProcessHeap(), 0, text);
    if (ModsBudget())
        Log("[mods] %lu ms: ADDON FOLDER read: %s (%s): %d folder(s)%s, %d with addme.script (the mods the game loads); the "
            "game's addon mission list %s%u of %u (the game's own cap %u%s), %d mission(s) from folders not in AddOn now%s%s%s; "
            "the page's tables %s",
            ScanMsSinceAttach(), g_modsAddonDir[0] ? g_modsAddonDir : "?", g_modsDirFound ? "found" : "NOT FOUND",
            g_modsFolderCount + g_modsFolderMore, g_modsFolderMore ? " (the ones past 1024 not listed)" : "", mods,
            known ? "" : "NOT READ (not on this build) - ", static_cast<unsigned>(count), static_cast<unsigned>(cap),
            static_cast<unsigned>(stock), cap > stock ? ", raised by BF1GameExt" : "", other, other ? " (e.g. " : "",
            other ? otherPath : "", other ? ")" : "", r == 0 ? "handed over" : "FAILED (the game's Lua error output has it)");
    if (unreadable && ModsBudget())
        Log("[mods] %d entr%s of the list NOT readable (skipped)", unreadable, unreadable == 1 ? "y" : "ies");
    if (!g_modsFoldersLogged)
    {
        g_modsFoldersLogged = true;
        for (int k = 0; k < g_modsFolderCount && k < 60; ++k)
        {
            const ModsFolder& f = g_modsFolders[k];
            if (!ModsBudget())
                break;
            Log("[mods]   %s: addme.script %s, %d mission(s)%s%s", f.name, f.addme ? "yes" : "NO (not loaded)", f.missions,
                f.missions ? " - " : "", f.list);
        }
        if (g_modsFolderCount > 60 && ModsBudget())
            Log("[mods]   ... and %d more folder(s)", g_modsFolderCount - 60);
    }
    return g_modsFolderCount;
}

// The MODS pages' questions ("BFXQ: mods <what>"):
//   sprint / sprint set N     SPRINT: 1 on, 0 off, -1 not available; set: at once and saved (sprint.cpp, [Sprint] TestBoost)
//   ragdoll / ragdoll set N   RAGDOLL EFFECTS, the same (ragdoll.cpp, [Ragdoll] Enabled)
//   count                     the mods the game loads (AddOn's folders with an addme.script) - the MODS page's ADDON FOLDER row
//   (addons: ModsAddonsRun, with the Lua state)
int ModsQuery(const char* q)
{
    if (strcmp(q, "sprint") == 0)
        return SprintMenuState();
    if (strncmp(q, "sprint set ", 11) == 0)
        return SprintMenuSet(strtol(q + 11, nullptr, 10) != 0);
    if (strcmp(q, "ragdoll") == 0)
        return RagdollMenuState();
    if (strncmp(q, "ragdoll set ", 12) == 0)
        return RagdollMenuSet(strtol(q + 12, nullptr, 10) != 0);
    if (strcmp(q, "count") == 0)
        return ModsScanFolders();
    if (ModsBudget())
        Log("[mods] %lu ms: unknown question \"mods %s\" - -1 back", ScanMsSinceAttach(), q);
    return -1;
}

int SplitPlayersInstalled(); // 21g (below)

// The script, after its settings lines: the screen and the phase ("build": ScriptCB_AddIFScreen, "push":
// ScriptCB_PushScreen). It changes each screen once per Lua state (its own guards); every run defines its helpers.
void SplitRun(void* L, const char* screen, const char* phase)
{
    if (!g_luaDoBuffer || !g_splitSrc)
        return;
    ++g_splitRuns;
    // 21g: the players a split screen match has now (2, or 3 / 4 with players 3 / 4 on) and SignIn4
    const int players = SplitPlayersInstalled();
    if (players != g_splitPlayersLogged && SplitBudget())
    {
        g_splitPlayersLogged = players;
        Log("[splitmenu] %lu ms: up to %d players in a split screen match (playern_scan.cpp: %s) - the sign-in screen: %s",
            ScanMsSinceAttach(), players, players > 2 ? "players 3 / 4 on" : "players 3 / 4 not on (Players=2)",
            (players > 2 && g_signIn4) ? "the 4-player one (SignIn4=1)" : "the 2-player one");
    }
    char head[512];
    snprintf(head, sizeof(head),
             "gBFX_SplitSignIn = %s\ngBFX_Screen = \"%s\"\ngBFX_Phase = \"%s\"\ngBFX_Players = %d\ngBFX_SignIn4 = %s\n"
             "gBFX_TempProfiles = %s\ngBFX_GCTurnScan = %s\ngBFX_GCPickLook = %s\ngBFX_GCCmd2 = %s\n"
             "gBFX_PadIcons = %s\ngBFX_MouseBack = %s\ngBFX_SoundFix = %s\n",
             g_splitSignIn ? "1" : "nil", screen, phase, players, g_signIn4 ? "1" : "nil",
             (g_tempProfilesIni && g_profBase) ? "1" : "nil", g_gcTurnScanIni ? "1" : "nil", g_gcPickLookIni ? "1" : "nil",
             (g_gcPickLookIni && g_gcCmd2Ini) ? "1" : "nil", g_padIconsIni ? "1" : "nil", g_mouseBackIni ? "1" : "nil",
             g_soundFixIni ? "1" : "nil");
    if (RunLua(L, head, strlen(head), "=BF1GameExt_split settings", true) != 0)
        return;
    const int r = RunLua(L, g_splitSrc, g_splitLen, "=BF1GameExt_split", true);
    if (r == 0 && g_splitRuns <= 12 && SplitBudget())
        Log("[splitmenu] %lu ms: script run (%s, %s, run %lu)", ScanMsSinceAttach(), screen, phase, g_splitRuns);
}

// ---- pad 2 for the sign-in screen (the game thread: the script asks from the screen's Update) ----
struct XGamepad
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
};
struct XInputState
{
    DWORD dwPacketNumber;
    XGamepad Gamepad;
};
typedef DWORD(WINAPI* XInputGetState_t)(DWORD index, XInputState* state);
XInputGetState_t g_xGetState = nullptr;
bool g_xLoaded = false;

constexpr WORD kXDpadUp = 0x0001;
constexpr WORD kXDpadDown = 0x0002;
constexpr WORD kXDpadLeft = 0x0004;
constexpr WORD kXDpadRight = 0x0008;
constexpr WORD kXStart = 0x0010;
constexpr WORD kXA = 0x1000;
constexpr WORD kXB = 0x2000;
constexpr int kStickOn = 16000;      // of 32767: a direction once the left stick is past about half way
constexpr DWORD kRepeatFirstMs = 450; // a held direction repeats after this, then every kRepeatMs
constexpr DWORD kRepeatMs = 150;
constexpr DWORD kRepickMs = 1000;     // pad 2's XInput index is looked for again at most this often
constexpr DWORD kStaleMs = 250;       // asked after a longer gap (the screen was not up): start from what is held now
enum MenuEvent
{
    kEvNone = 0,
    kEvA = 1,
    kEvB = 2,
    kEvStart = 3,
    kEvUp = 4,
    kEvDown = 5,
    kEvLeft = 6,
    kEvRight = 7,
};
constexpr int kMenuQueue = 16;

struct MenuPad
{
    int index;          // XInput index (-1: none)
    bool picked;        // index looked for at least once
    DWORD pickedAt;
    bool primed;        // lastButtons / lastDir hold what was held at the last read
    WORD lastButtons;
    int lastDir;
    DWORD lastRead;
    DWORD dirSince;
    DWORD lastRepeat;
    int queue[kMenuQueue];
    int queued;
};
// 21g: pads 2, 3 and 4 (slot 0 / 1 / 2) - pads 3 and 4 for the 4-player sign-in screen
constexpr int kMenuPads = 3;
MenuPad g_menuPads[kMenuPads] = {
    { -1, false, 0, false, 0, 0, 0, 0, 0, {}, 0 },
    { -1, false, 0, false, 0, 0, 0, 0, 0, {}, 0 },
    { -1, false, 0, false, 0, 0, 0, 0, 0, {}, 0 },
};
MenuPad& g_menuPad2 = g_menuPads[0];

void XInputLoad()
{
    if (g_xLoaded)
        return;
    g_xLoaded = true;
    const char* const names[] = { "xinput1_4.dll", "xinput9_1_0.dll", "xinput1_3.dll" };
    for (const char* name : names)
    {
        HMODULE h = GetModuleHandleA(name);
        if (!h)
            h = LoadLibraryA(name);
        if (!h)
            continue;
        g_xGetState = reinterpret_cast<XInputGetState_t>(reinterpret_cast<void*>(GetProcAddress(h, "XInputGetState")));
        if (g_xGetState)
        {
            if (SplitBudget())
                Log("[splitmenu] pad 2 is read with %s's XInputGetState", name);
            return;
        }
    }
    if (SplitBudget())
        Log("[splitmenu] XInput NOT found - pad 2 cannot be read (player 2 cannot join)");
}

bool XRead(int index, XInputState* st)
{
    XInputLoad();
    memset(st, 0, sizeof(*st));
    if (!g_xGetState || index < 0 || index > 3)
        return false;
    return g_xGetState(static_cast<DWORD>(index), st) == ERROR_SUCCESS;
}

// Pad 2's XInput index: pad2.cpp's pick (player 2's pad in a match) if it answers, else the first pad that answers and
// is not the game's (player 1's). Looked for again at most once a second (XInput is slow on empty indexes).
int MenuPad2Pick()
{
    MenuPad& p = g_menuPad2;
    const DWORD now = GetTickCount();
    if (p.picked && now - p.pickedAt < kRepickMs)
        return p.index;
    p.picked = true;
    p.pickedAt = now;
    XInputState st;
    int index = -1;
    const char* why = "";
    const int p2 = Pad2XInputIndex();
    int game = GamePadXInputIndex();
    if (p2 >= 0 && p2 != game && XRead(p2, &st))
    {
        index = p2;
        why = "pad2.cpp's pad 2";
    }
    else
    {
        if (game < 0)
            game = 0;
        for (int i = 0; i < 4 && index < 0; ++i)
            if (i != game && XRead(i, &st))
            {
                index = i;
                why = "the first pad that is not the game's";
            }
    }
    if (index != p.index)
    {
        p.primed = false;
        p.queued = 0;
        if (SplitBudget())
        {
            if (index >= 0)
                Log("[splitmenu] %lu ms: pad 2 = XInput %d (%s; the game's pad: XInput %d)", ScanMsSinceAttach(), index,
                    why, GamePadXInputIndex());
            else
                Log("[splitmenu] %lu ms: pad 2: no second pad answers (the game's pad: XInput %d)", ScanMsSinceAttach(),
                    GamePadXInputIndex());
        }
    }
    p.index = index;
    return index;
}

// 21g: pad 3's / pad 4's XInput index (slot 1 / 2), as players 3 and 4's pads are picked in a match (playern_scan.cpp
// PickPad34): the first pad that answers and is not the game's, not pad 2's and not the other extra pad's - pad 3 picks
// first (pad 4 is asked for pad 3's pick first), lowest index first. A pick is kept while it answers and nobody else
// takes it. Looked for again at most once a second.
int MenuPad34Pick(int slot)
{
    if (slot == 2)
        MenuPad34Pick(1); // pad 3 first
    MenuPad& p = g_menuPads[slot];
    MenuPad& other = g_menuPads[3 - slot];
    const DWORD now = GetTickCount();
    if (p.picked && now - p.pickedAt < kRepickMs)
        return p.index;
    p.picked = true;
    p.pickedAt = now;
    int game = GamePadXInputIndex();
    if (game < 0)
        game = 0; // as pad 2's pick: with no pad of the game's, XInput 0 is left to player 1
    const int p2 = MenuPad2Pick();
    XInputState st;
    int index = p.index;
    if (index >= 0 && (index == game || index == p2 || index == other.index || !XRead(index, &st)))
        index = -1;
    for (int i = 0; i < 4 && index < 0; ++i)
        if (i != game && i != p2 && i != other.index && XRead(i, &st))
            index = i;
    if (index != p.index)
    {
        p.primed = false;
        p.queued = 0;
        if (SplitBudget())
        {
            if (index >= 0)
                Log("[splitmenu] %lu ms: pad %d = XInput %d (the first free pad: the game's pad XInput %d, pad 2 XInput %d, "
                    "pad %d XInput %d)",
                    ScanMsSinceAttach(), slot + 2, index, GamePadXInputIndex(), p2, 5 - slot, other.index);
            else
                Log("[splitmenu] %lu ms: pad %d: no free pad answers (the game's pad XInput %d, pad 2 XInput %d, pad %d XInput "
                    "%d)",
                    ScanMsSinceAttach(), slot + 2, GamePadXInputIndex(), p2, 5 - slot, other.index);
        }
    }
    p.index = index;
    return index;
}

int MenuPadPick(int slot)
{
    return slot == 0 ? MenuPad2Pick() : MenuPad34Pick(slot);
}

void MenuPadPush(MenuPad& p, int ev)
{
    if (p.queued < kMenuQueue)
        p.queue[p.queued++] = ev;
}

// The d-pad first, else the left stick's larger axis past half way (XInput: Y up is positive).
int MenuPadDir(const XInputState& st)
{
    const WORD b = st.Gamepad.wButtons;
    if (b & kXDpadUp)
        return kEvUp;
    if (b & kXDpadDown)
        return kEvDown;
    if (b & kXDpadLeft)
        return kEvLeft;
    if (b & kXDpadRight)
        return kEvRight;
    const int lx = st.Gamepad.sThumbLX;
    const int ly = st.Gamepad.sThumbLY;
    const int ax = lx < 0 ? -lx : lx;
    const int ay = ly < 0 ? -ly : ly;
    if (ay >= ax && ay > kStickOn)
        return ly > 0 ? kEvUp : kEvDown;
    if (ax > ay && ax > kStickOn)
        return lx > 0 ? kEvRight : kEvLeft;
    return kEvNone;
}

// One read of a pad: presses (A, B, START) and directions (new, or held long enough to repeat) go in its queue.
void MenuPadSample(int slot)
{
    MenuPad& p = g_menuPads[slot];
    const int index = MenuPadPick(slot);
    XInputState st;
    if (index < 0 || !XRead(index, &st))
    {
        p.primed = false;
        if (index >= 0)
            p.picked = false; // read failed: look for it again
        return;
    }
    const DWORD now = GetTickCount();
    const WORD b = st.Gamepad.wButtons;
    const int dir = MenuPadDir(st);
    if (!p.primed || now - p.lastRead > kStaleMs)
    {
        // the first read (or after a gap): what is held now is not a press
        p.primed = true;
        p.lastButtons = b;
        p.lastDir = dir;
        p.lastRead = now;
        p.dirSince = now;
        p.lastRepeat = now;
        return;
    }
    p.lastRead = now;
    const WORD down = static_cast<WORD>(b & ~p.lastButtons);
    if (down & kXA)
        MenuPadPush(p, kEvA);
    if (down & kXB)
        MenuPadPush(p, kEvB);
    if (down & kXStart)
        MenuPadPush(p, kEvStart);
    if (dir != p.lastDir)
    {
        if (dir != kEvNone)
            MenuPadPush(p, dir);
        p.dirSince = now;
        p.lastRepeat = now;
    }
    else if (dir != kEvNone && now - p.dirSince >= kRepeatFirstMs && now - p.lastRepeat >= kRepeatMs)
    {
        MenuPadPush(p, dir);
        p.lastRepeat = now;
    }
    p.lastButtons = b;
    p.lastDir = dir;
}

// 21g: how many players the match will have with players 3 / 4 on (playern_scan.cpp: Players=3 / 4 installed), else 2.
// 21g-r2: the installed most (the sign-in screen is built for it), not this match's count (PlayerNLocalPlayers follows
// the sign-in now)
int SplitPlayersInstalled()
{
    const int n = PlayerNPlayersMax();
    return (n == 3 || n == 4) ? n : 2;
}

// 21g-r2: a profile's name (16 wide chars at +2) as ASCII, for the log
void ProfileNameA(uintptr_t profile, char* out, size_t n)
{
    size_t j = 0;
    const uint16_t* w = reinterpret_cast<const uint16_t*>(profile + kProfileNameAt);
    for (int i = 0; i < kProfileNameChars && w[i] && j + 1 < n; ++i)
        out[j++] = (w[i] >= 32 && w[i] < 127) ? static_cast<char>(w[i]) : '?';
    out[j] = 0;
}

// 21g-r2: player 2's temporary profile (the sign-in's PLAYER 2 / 3 / 4 rows, called right before the game loads the
// profiles): what the game's own GetLoginList does when split screen has fewer than 2 profiles - the current profile
// (player 1's, index 0) copied into the second (index 1), the copy named, both local player slots' profile pointers
// written with the game's SetCurrentProfile calls around them (it applies each profile's controls to its slot). It is never saved: the script keeps ifs_saveop from saving profile 2 while it is
// the temporary one. 1 = made, 0 = not (not found, or the current profile is not the first).
int TempProfile2(const char* name)
{
    if (!g_profBase || !g_profCurIdx || !g_profSlot0 || !g_profSlot1 || !g_profSetCur)
    {
        if (SplitBudget())
            Log("[splitmenu] %lu ms: temporary profile \"%s\" for player 2: the game's profiles were NOT found - not made",
                ScanMsSinceAttach(), name);
        return 0;
    }
    const int32_t cur = *reinterpret_cast<volatile const int32_t*>(g_profCurIdx);
    const uintptr_t p0 = g_profBase;
    const uintptr_t p1 = g_profBase + kProfileBytes;
    char before[kProfileNameChars + 1];
    char first[kProfileNameChars + 1];
    ProfileNameA(p1, before, sizeof(before));
    ProfileNameA(p0, first, sizeof(first));
    if (cur != 0)
    {
        if (SplitBudget())
            Log("[splitmenu] %lu ms: temporary profile \"%s\" for player 2: the current profile is index %ld, not 0 - not made "
                "(profiles: \"%s\", \"%s\")",
                ScanMsSinceAttach(), name, static_cast<long>(cur), first, before);
        return 0;
    }
    const uintptr_t was0 = *reinterpret_cast<volatile const uintptr_t*>(g_profSlot0);
    const uintptr_t was1 = *reinterpret_cast<volatile const uintptr_t*>(g_profSlot1);
    memcpy(reinterpret_cast<void*>(p1), reinterpret_cast<const void*>(p0), kProfileBytes);
    uint16_t* w = reinterpret_cast<uint16_t*>(p1 + kProfileNameAt);
    for (int i = 0; i < kProfileNameChars; ++i)
        w[i] = 0;
    for (int i = 0; i < kProfileNameChars - 1 && name[i]; ++i)
        w[i] = static_cast<unsigned char>(name[i]);
    // as the game's own code (GetLoginList GOG 1.3 0x00569BFE): Set(1), local slot 2's pointer, Set(1) - his controls
    // applied to his slot -, Set(0), local slot 1's pointer, Set(0) - the first profile current again
    g_profSetCur(1);
    *reinterpret_cast<volatile uintptr_t*>(g_profSlot1) = p1;
    g_profSetCur(1);
    g_profSetCur(0);
    *reinterpret_cast<volatile uintptr_t*>(g_profSlot0) = p0;
    g_profSetCur(0);
    char after[kProfileNameChars + 1];
    ProfileNameA(p1, after, sizeof(after));
    g_tempOn = true;
    memcpy(g_tempName, after, sizeof(g_tempName));
    if (SplitBudget())
        Log("[splitmenu] %lu ms: temporary profile for player 2: profile 2 (was \"%s\") = a copy of profile 1 (\"%s\") named "
            "\"%s\" (never saved); the local slots' profile pointers %08X %08X (were %08X %08X)",
            ScanMsSinceAttach(), before, first, after, static_cast<unsigned>(p0), static_cast<unsigned>(p1),
            static_cast<unsigned>(was0), static_cast<unsigned>(was1));
    return 1;
}

// 21i-r6: player 1's profile name (local slot 1's) as the store's key: letters / digits / - kept, anything else '_';
// false when there is no profile
bool IaProfileKey(char* key, size_t n)
{
    const uintptr_t p = g_profSlot0 ? *reinterpret_cast<volatile const uintptr_t*>(g_profSlot0) : 0;
    char name[kProfileNameChars + 1] = {};
    if (p)
        ProfileNameA(p, name, sizeof(name));
    if (!p || !name[0] || n < 2)
    {
        if (n)
            key[0] = 0;
        return false;
    }
    size_t j = 0;
    for (size_t i = 0; name[i] && j + 1 < n; ++i)
    {
        const char c = name[i];
        const bool keep = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-';
        key[j++] = keep ? c : '_';
    }
    key[j] = 0;
    return true;
}

// 21i-r6: one number out of text (strtol: /sdl turns sscanf into an error); def when there is none
long IaNum(const char** q, long def)
{
    char* end = nullptr;
    const long v = strtol(*q, &end, 10);
    if (end == *q)
        return def;
    *q = end;
    return v;
}

void IaClamp()
{
    if (g_iaUnits < -1)
        g_iaUnits = -1;
    if (g_iaUnits > kIaMaxUnits)
        g_iaUnits = kIaMaxUnits;
    if (g_iaReinf < 1)
        g_iaReinf = -1;
    if (g_iaReinf > 9999)
        g_iaReinf = 9999;
    if (g_iaHeroes < -1 || g_iaHeroes > 1)
        g_iaHeroes = -1;
}

// 21i-r6: the GAME OPTIONS page's questions ("BFXQ: iaopt <what>"). load: player 1's saved numbers read (1 = saved, 0 = the
// defaults); units / reinf / heroes: the number + 1 (0 = MAP DEFAULT / not saved); save U R H: saved for player 1's profile
// and used from now on (1 = written)
int IaQuery(const char* what)
{
    char key[kProfileNameChars + 1];
    if (strcmp(what, "load") == 0)
    {
        g_iaUnits = -1;
        g_iaReinf = -1;
        g_iaHeroes = -1;
        const bool named = IaProfileKey(key, sizeof(key));
        char v[64] = {};
        if (named && g_iaStore[0])
            GetPrivateProfileStringA("IAOptions", key, "", v, sizeof(v), g_iaStore);
        const bool found = v[0] != 0;
        if (found)
        {
            const char* q = v;
            g_iaUnits = static_cast<int>(IaNum(&q, -1));
            g_iaReinf = static_cast<int>(IaNum(&q, -1));
            g_iaHeroes = static_cast<int>(IaNum(&q, -1));
            IaClamp();
        }
        if (g_iaLines < kIaMaxLines)
        {
            ++g_iaLines;
            Log("[iaopts] %lu ms: player 1's profile \"%s\": %s - AI units per team %d, reinforcements %d, heroes %d (-1 = MAP DEFAULT / "
                "not saved)", ScanMsSinceAttach(), named ? key : "(none)", found ? "saved numbers read" : "nothing saved - MAP DEFAULT",
                g_iaUnits, g_iaReinf, g_iaHeroes);
        }
        return found ? 1 : 0;
    }
    if (strcmp(what, "units") == 0)
        return g_iaUnits + 1;
    if (strcmp(what, "reinf") == 0)
        return g_iaReinf + 1;
    if (strcmp(what, "heroes") == 0)
        return g_iaHeroes + 1;
    if (strncmp(what, "save ", 5) == 0)
    {
        const char* q = what + 5;
        g_iaUnits = static_cast<int>(IaNum(&q, -1));
        g_iaReinf = static_cast<int>(IaNum(&q, -1));
        g_iaHeroes = static_cast<int>(IaNum(&q, -1));
        IaClamp();
        const bool named = IaProfileKey(key, sizeof(key));
        char v[64];
        snprintf(v, sizeof(v), "%d %d %d", g_iaUnits, g_iaReinf, g_iaHeroes);
        const bool ok = named && g_iaStore[0] && WritePrivateProfileStringA("IAOptions", key, v, g_iaStore) != 0;
        if (g_iaLines < kIaMaxLines)
        {
            ++g_iaLines;
            Log("[iaopts] %lu ms: GAME OPTIONS: AI units per team %d, reinforcements %d, heroes %d (-1 = MAP DEFAULT) - %s", ScanMsSinceAttach(),
                g_iaUnits, g_iaReinf, g_iaHeroes,
                ok ? "saved for player 1's profile (BF1GameExt_IAOptions.ini)" : named ? "NOT saved (the file could not be written) - used this session"
                                                                                       : "NOT saved (no profile) - used this session");
        }
        return ok ? 1 : 0;
    }
    return 0;
}

// The script's questions (ScriptCB_GetPlatform("BFXQ: <what>")): the number handed back.
//   padN       pad N's next event (0: none)          padNconn   1 if pad N answers, else 0
//   padNindex  pad N's XInput index (-1: none)        padNreset  forget what pad N held (the screen just opened)
//   (N = 2, 3, 4 - 21g: 3 and 4 for the 4-player sign-in)
//   players    21g: the players a split screen match has now (2, or 3 / 4 with players 3 / 4 on)
//   signin N X3 X4   21g-r2: the sign-in's LAUNCH - N players signed in, X3 / X4 the XInput pads of players 3 / 4 (-1:
//              none): the count of the next matches (playern_scan.cpp PlayerNSignIn; the count set back, 0 = not on)
//   tempprofile NAME 21g-r2: player 2's temporary profile named NAME (1 = made)
int FakeConsoleQuery(const char* name, bool toggle); // 21j-r24 (the free camera section below)

int SplitQuery(const char* what)
{
    if (strncmp(what, "rp ", 3) == 0) // 21h-r2: players 3 / 4's real pause menus
        return P2SpawnRealPauseAsk(what + 3);
    // 21j-r24: the FAKE CONSOLE - con NAME (1 on, 0 off, -1 not a debug flag BF1GameExt knows), con! NAME switches it
    if (strncmp(what, "con! ", 5) == 0)
        return FakeConsoleQuery(what + 5, true);
    if (strncmp(what, "con ", 4) == 0)
        return FakeConsoleQuery(what + 4, false);
    if (strncmp(what, "iaopt ", 6) == 0) // 21i-r6: Instant Action's GAME OPTIONS page
        return IaQuery(what + 6);
    if (strncmp(what, "aimp ", 5) == 0) // 21i-r10: auto aim per player
        return AimQuery(what + 5);
    if (strncmp(what, "padf ", 5) == 0) // 21i-r14: the gamepad page's DEAD ZONE / LOOK CURVE rows
        return PadQuery(what + 5);
    if (strncmp(what, "fov ", 4) == 0) // 21i-r16: VIDEO OPTIONS' FIELD OF VIEW
        return FovQuery(what + 4);
    if (strncmp(what, "mods ", 5) == 0) // 21j-r17: the MODS pages
        return ModsQuery(what + 5);
    // 21j-r18: GAME OPTIONS' SPAWN SCREEN row - wheel get (1 CLASS WHEEL, 0 PC MENU, -1 no class wheel on this build), wheel set N
    if (strcmp(what, "wheel get") == 0)
        return ClassWheelMenuState();
    if (strncmp(what, "wheel set ", 10) == 0)
        return ClassWheelMenuSet(strtol(what + 10, nullptr, 10) != 0);
    // 21i-r8: GAME OPTIONS' SPLIT SCREEN row - can (the edits went in), on (what the row shows), next (chosen in a match, waits
    // for the main menu), p34 (this match has 3 / 4 players), set N now|match
    if (strcmp(what, "vsplit can") == 0)
        return (g_splitMenuIni && VerticalSwitchable()) ? 1 : 0;
    if (strcmp(what, "vsplit on") == 0)
        return VerticalWanted() ? 1 : 0;
    if (strcmp(what, "vsplit next") == 0)
        return VerticalPendingDiffers() ? 1 : 0;
    if (strcmp(what, "vsplit p34") == 0)
        return PlayerNLocalPlayers() >= 3 ? 1 : 0;
    if (strncmp(what, "vsplit set ", 11) == 0 && (what[11] == '0' || what[11] == '1') && what[12] == ' ')
        return VerticalSet(what[11] == '1', strcmp(what + 13, "now") == 0) ? 1 : 0;
    // 21i-r4: player 1's profile (local slot 1's) - logged; 1 = found (its name will key the Instant Action options)
    if (strcmp(what, "iaprofile") == 0)
    {
        const uintptr_t p = g_profSlot0 ? *reinterpret_cast<volatile const uintptr_t*>(g_profSlot0) : 0;
        char name[kProfileNameChars + 1] = {};
        if (p)
            ProfileNameA(p, name, sizeof(name));
        if (g_iaLines < kIaMaxLines)
        {
            ++g_iaLines;
            Log("[iaopts] %lu ms: player 1's profile: %s (local slot 1's pointer 0x%08X, the current profile index %ld)", ScanMsSinceAttach(),
                p ? name : "NOT found", static_cast<unsigned>(p), g_profCurIdx ? static_cast<long>(*reinterpret_cast<volatile const int32_t*>(g_profCurIdx)) : -1L);
        }
        return p ? 1 : 0;
    }
    if (strcmp(what, "players") == 0)
        return SplitPlayersInstalled();
    if (strncmp(what, "signin ", 7) == 0)
    {
        // (strtol: /sdl turns sscanf into an error)
        char* end = nullptr;
        const char* q = what + 7;
        const long ln = strtol(q, &end, 10);
        const bool gotN = end != q;
        q = end;
        long lx3 = strtol(q, &end, 10);
        if (end == q)
            lx3 = -1;
        q = end;
        long lx4 = strtol(q, &end, 10);
        if (end == q)
            lx4 = -1;
        const int n = static_cast<int>(ln);
        const int x3 = static_cast<int>(lx3);
        const int x4 = static_cast<int>(lx4);
        if (!gotN)
        {
            if (SplitBudget())
                Log("[splitmenu] %lu ms: \"%.40s\" - no count - answered -1", ScanMsSinceAttach(), what);
            return -1;
        }
        const int set = PlayerNSignIn(n, x3, x4);
        if (SplitBudget())
            Log("[splitmenu] %lu ms: the sign-in's LAUNCH: %d players signed in (player 3's pad XInput %d, player 4's XInput %d) - "
                "%s",
                ScanMsSinceAttach(), n, x3, x4,
                set ? "the next matches have that many (playern_scan.cpp [players] line)" : "players 3 / 4 are not on: as before");
        return set;
    }
    if (strncmp(what, "tempprofile ", 12) == 0)
        return TempProfile2(what + 12);
    // 21g-r3: playersnow - this match's player count (the sign-in's; 2 while players 3 / 4 are not on)
    if (strcmp(what, "playersnow") == 0)
    {
        const int n = PlayerNLocalPlayers();
        return n >= 3 ? n : 2;
    }
    // 21g-r3: gcset S1 S2 S3 S4 C1 C2 - Galactic Conquest's sides (players 1 - 4) and commanders (sides 1 / 2); gcget I -
    // value I (1 - 6) back
    if (strncmp(what, "gcset ", 6) == 0)
    {
        long v[6] = {};
        const char* q = what + 6;
        int got = 0;
        for (int i = 0; i < 6; ++i)
        {
            char* end = nullptr;
            const long x = strtol(q, &end, 10);
            if (end == q)
                break;
            v[i] = x;
            q = end;
            ++got;
        }
        if (got != 6)
        {
            if (SplitBudget())
                Log("[splitmenu] %lu ms: \"%.60s\" - 6 numbers needed - nothing set", ScanMsSinceAttach(), what);
            return 0;
        }
        for (int i = 0; i < 4; ++i)
            InterlockedExchange(&g_gcSide[i], (v[i] == 1 || v[i] == 2) ? v[i] : 0);
        for (int i = 0; i < 2; ++i)
            InterlockedExchange(&g_gcCmd[i], (v[4 + i] >= 1 && v[4 + i] <= 4) ? v[4 + i] : 0);
        if (g_gcIni[0])
        {
            char line[64];
            snprintf(line, sizeof(line), "%ld %ld %ld %ld %ld %ld", static_cast<long>(g_gcSide[0]), static_cast<long>(g_gcSide[1]),
                     static_cast<long>(g_gcSide[2]), static_cast<long>(g_gcSide[3]), static_cast<long>(g_gcCmd[0]),
                     static_cast<long>(g_gcCmd[1]));
            WritePrivateProfileStringA("Splitscreen", "GCLastPick", line, g_gcIni);
        }
        if (SplitBudget())
            Log("[splitmenu] %lu ms: Galactic Conquest: sides - player 1 %ld, player 2 %ld, player 3 %ld, player 4 %ld (0: not "
                "playing); side 1 commanded by player %ld, side 2 by player %ld (0: the AI) - players 3 / 4 are locked to "
                "their sides in its battles",
                ScanMsSinceAttach(), static_cast<long>(g_gcSide[0]), static_cast<long>(g_gcSide[1]),
                static_cast<long>(g_gcSide[2]), static_cast<long>(g_gcSide[3]), static_cast<long>(g_gcCmd[0]),
                static_cast<long>(g_gcCmd[1]));
        return 1;
    }
    // gcclear: the picks are not this conquest's - players 3 / 4 take player 1's side in its battles (the INI keeps its line)
    if (strcmp(what, "gcclear") == 0)
    {
        for (int i = 0; i < 4; ++i)
            InterlockedExchange(&g_gcSide[i], 0);
        InterlockedExchange(&g_gcCmd[0], 0);
        InterlockedExchange(&g_gcCmd[1], 0);
        if (SplitBudget())
            Log("[splitmenu] %lu ms: Galactic Conquest: the picks are not this conquest's - cleared (players 3 / 4 take player 1's "
                "side in its battles)", ScanMsSinceAttach());
        return 1;
    }
    if (strncmp(what, "gcget ", 6) == 0)
    {
        const long i = strtol(what + 6, nullptr, 10);
        if (i >= 1 && i <= 4)
            return static_cast<int>(g_gcSide[i - 1]);
        if (i == 5 || i == 6)
            return static_cast<int>(g_gcCmd[i - 5]);
        return -1;
    }
    // tempok: 1 = player 2's temporary profile can be made now (the profiles found, the current one the first)
    if (strcmp(what, "tempok") == 0)
        return (g_profBase && g_profCurIdx && g_profSetCur && *reinterpret_cast<volatile const int32_t*>(g_profCurIdx) == 0) ? 1 : 0;
    // tempmark NAME: profile 2 is the game's own temporary one (its GetLoginList made it: "active slot 2") - kept unsaved too
    // tempclear: player 2 signed in with a saved profile - profile 2 is saved as the game does it
    // temp2: 1 = profile 2 is still the temporary one (its name as made / marked)
    if (strncmp(what, "tempmark ", 9) == 0 || strcmp(what, "tempclear") == 0)
    {
        const bool mark = what[4] == 'm';
        g_tempOn = mark;
        g_tempName[0] = 0;
        if (mark)
        {
            size_t i = 0;
            for (; i < kProfileNameChars && what[9 + i]; ++i)
                g_tempName[i] = what[9 + i];
            g_tempName[i] = 0;
        }
        if (SplitBudget())
            Log("[splitmenu] %lu ms: %s", ScanMsSinceAttach(),
                mark ? "player 2 signed in with the game's own temporary profile (made by its profile list) - it is not saved either"
                     : "player 2 signed in with a saved profile - profile 2 is saved as the game does it");
        return 1;
    }
    if (strcmp(what, "temp2") == 0)
    {
        if (!g_tempOn || !g_profBase)
            return 0;
        char now[kProfileNameChars + 1];
        ProfileNameA(g_profBase + kProfileBytes, now, sizeof(now));
        return _stricmp(now, g_tempName) == 0 ? 1 : 0;
    }
    if (strncmp(what, "pad", 3) != 0 || what[3] < '2' || what[3] > '4')
    {
        if (SplitBudget())
            Log("[splitmenu] %lu ms: unknown question \"%.40s\" - answered -1", ScanMsSinceAttach(), what);
        return -1;
    }
    const int slot = what[3] - '2';
    const char* rest = what + 4;
    MenuPad& p = g_menuPads[slot];
    if (*rest == 0)
    {
        if (!p.queued)
            MenuPadSample(slot);
        if (!p.queued)
            return kEvNone;
        const int ev = p.queue[0];
        --p.queued;
        memmove(p.queue, p.queue + 1, static_cast<size_t>(p.queued) * sizeof(p.queue[0]));
        return ev;
    }
    if (strcmp(rest, "conn") == 0)
    {
        const int index = MenuPadPick(slot);
        XInputState st;
        return (index >= 0 && XRead(index, &st)) ? 1 : 0;
    }
    if (strcmp(rest, "index") == 0)
        return MenuPadPick(slot);
    if (strcmp(rest, "reset") == 0)
    {
        p.primed = false;
        p.queued = 0;
        return 0;
    }
    if (SplitBudget())
        Log("[splitmenu] %lu ms: unknown question \"%.40s\" - answered -1", ScanMsSinceAttach(), what);
    return -1;
}

// The screens the trace logs everything for: the options screens, and (21e) the split screen ones (split: their
// IFObj / IFText calls are not logged - kTraceQuietInSplit).
bool TraceArmScreen(const char* screen, bool* split)
{
    *split = false;
    if (!screen)
        return false;
    if (strncmp(screen, "ifs_opt", 7) == 0)
        return true;
    if (strncmp(screen, "ifs_split", 9) == 0 || strcmp(screen, "ifs_sp") == 0 || strcmp(screen, "ifs_saveop") == 0 ||
        strcmp(screen, "ifs_meta_opts") == 0 || strcmp(screen, "ifs_meta_top") == 0 ||
        strcmp(screen, "ifs_sp_briefing") == 0) // 21f: the two locked-side modes
    {
        *split = true;
        return true;
    }
    return false;
}

// 21f: the sides as the game has them now (after one of the calls that change them)
void SidesLog(const char* call, const char* args)
{
    if (!SplitBudget())
        return;
    const int p1 = g_sideTable ? static_cast<int>(*reinterpret_cast<volatile const int32_t*>(g_sideTable)) : -1;
    const int p2 = g_sideTable ? static_cast<int>(*reinterpret_cast<volatile const int32_t*>(g_sideTable + kSideStride)) : -1;
    const int can = g_canSwitchByte ? (*g_canSwitchByte ? 1 : 0) : -1;
    const int meta = g_metaRulesByte ? (*g_metaRulesByte ? 1 : 0) : -1;
    const bool locked = can == 0 || meta == 1;
    Log("[splitmenu] %lu ms: sides: %s(%s) - can switch sides %s, metagame rules %s, player 1's side %d, player 2's side %d%s",
        ScanMsSinceAttach(), call, args, can < 0 ? "?" : can ? "on" : "OFF", meta < 0 ? "?" : meta ? "ON" : "off", p1, p2,
        (locked && g_sideLockIni) ? " - sides locked: no side screens for players 1 and 2" : "");
}

// ---- 21j-r27: THE AUDIO SCANNER ([Splitscreen] AudioScan, default 1; [audio] lines; log only) -------------------------------
// The music stops at the split screen sign-in and the sign-in is silent (21j-r26's report) - nothing in the scripts explains
// it (their shell bus fades all end at 1.0: ScriptCB_SndBusFade(bus, time, target[, start]) - GOG 1.3 0x005806D0 -> the
// bus's fade 0x00704DB0: +0 start, +4 target, +8 time, +0Ch elapsed, the gain at +10h), so this logs what the sound does:
//   - the scripts' sound / music / movie calls (the last kTraceAudioCount trace names: SndBusFade, SetShellMusic,
//     SetShellMusicInterval, SoundEnable / Disable, SetVolumes, ResetSoundToDefault, Play / Stop / Close / OpenMovie,
//     SetMovieAudioBus, ShellPlayDelayedStream) on every screen, each with the sound state right after it;
//   - the sound state every 250 ms on the game thread (the menu layer's frame), a line when it changes: the sound on / off
//     byte (ScriptCB_SoundEnable's, GOG 1.3 0x00839C60 - SetShellMusic does nothing while it is 0), the shell music playing
//     (its name's hash, IsShellMusicPlaying's 0x008B8FB0 - known names shown) and its interval (SetShellMusicInterval's),
//     the volumes the sound engine was given (the profile's volumes x 0.1: SetVolumes -> the profile's apply 0x005F3C80 ->
//     0x00544490, its four floats), the current profile and both profiles' volumes (+26Ch music, +26Dh sfx, +26Eh voice,
//     +272h master - 0..10), and the buses' volume (+20h) and gain (+10h, a fade's target / time when one runs): master
//     (the bus tree's root, the lookup's ecx), music, shellmusic, shellmovies, soundfx, ingame, ambience, voiceover - found
//     with the game's own lookup (0x00704E30, the hash: FNV-1a of the lower-cased name).
// Nothing in the game is changed.
bool g_audioScanIni = true;          // [Splitscreen] AudioScan
uint32_t TraceCallTarget(uint32_t rva); // (below)
int TraceIndex(const char* name);       // (below)
bool g_auOn = false;
uint8_t* g_auSoundOn = nullptr;      // ScriptCB_SoundEnable's byte
uint32_t* g_auMusic = nullptr;       // the shell music playing (its name's hash, 0 = none)
uint8_t* g_auRestarted = nullptr;    // 21j-r28: "the sound engine was restarted" (GetMixConfigChanged's byte - read, not cleared)
// 21j-r28: the shell music's stream, found as the game's own music update does it (GOG 1.3 0x005838D0): the music's entry
// (its hash), the entry's stream id, the stream with that id, the stream's state (+68h; 4 = done, the update starts it again
// after the interval) - none after a sound engine restart until the shell's streams are opened again
typedef uintptr_t(__cdecl* AuMusicEntry_t)(uint32_t hash);
typedef uint32_t(__fastcall* AuThis_t)(uintptr_t self, void* edx);
typedef uintptr_t(__cdecl* AuStream_t)(uint32_t id);
AuMusicEntry_t g_auMusicEntry = nullptr;
AuThis_t g_auEntryStream = nullptr;
AuStream_t g_auStreamById = nullptr;
AuThis_t g_auStreamState = nullptr;
float* g_auInterval = nullptr;       // SetShellMusicInterval's
float* g_auEngine[4] = {};           // the engine's volumes: music, sfx, voice, master
typedef uintptr_t(__cdecl* AuBusGet_t)(uint32_t hash);
AuBusGet_t g_auBusGet = nullptr;
uintptr_t g_auMaster = 0;            // the bus tree's root (the master bus)
int g_auLines = 0;
constexpr int kAudioMaxLines = 1500;
DWORD g_auLastSample = 0;
char g_auLast[640] = {};
const char* const kAuBusNames[] = { "music", "shellmusic", "shellmovies", "soundfx", "ingame", "ambience", "voiceover" };
const char* const kAuMusicNames[] = { "shell_soundtrack", "shell_galacticconquest", "shell_droidinvasion", "shell_clonewars",
                                      "shell_imperialmarch", "shell_clash", "shell_music" };
constexpr uint32_t kAuBusTarget = 0x04, kAuBusTime = 0x08, kAuBusElapsed = 0x0C, kAuBusGain = 0x10, kAuBusVolume = 0x20;
constexpr uint32_t kAuProfMusic = 0x26C, kAuProfSfx = 0x26D, kAuProfVoice = 0x26E, kAuProfMaster = 0x272;

bool AudioBudget()
{
    if (g_auLines < kAudioMaxLines)
    {
        ++g_auLines;
        return true;
    }
    if (g_auLines == kAudioMaxLines)
    {
        ++g_auLines;
        Log("[audio] line limit reached (%d), going quiet", kAudioMaxLines);
    }
    return false;
}

// the game's name hash (GOG 1.3 0x006FA0D0): FNV-1a of each byte | 20h
uint32_t AuHash(const char* s)
{
    uint32_t h = 2166136261u;
    for (; *s; ++s)
        h = (static_cast<uint32_t>(static_cast<unsigned char>(*s) | 0x20u) ^ h) * 16777619u;
    return h;
}

bool AuReadable(const void* p, size_t n)
{
    MEMORY_BASIC_INFORMATION mbi = {};
    if (!p || !VirtualQuery(p, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) == 0 ||
        (mbi.Protect & PAGE_GUARD))
        return false;
    return reinterpret_cast<uintptr_t>(p) + n <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

// a bus's volume / gain (and its fade while one runs), "-" if the game has no such bus; game thread
int AuBusText(char* out, size_t size, const char* name, uintptr_t bus)
{
    if (!bus || !AuReadable(reinterpret_cast<const void*>(bus), kAuBusVolume + 4))
        return snprintf(out, size, " %s -", name);
    float vol = 0, gain = 0, target = 0, time = 0, elapsed = 0;
    memcpy(&vol, reinterpret_cast<const void*>(bus + kAuBusVolume), 4);
    memcpy(&gain, reinterpret_cast<const void*>(bus + kAuBusGain), 4);
    memcpy(&target, reinterpret_cast<const void*>(bus + kAuBusTarget), 4);
    memcpy(&time, reinterpret_cast<const void*>(bus + kAuBusTime), 4);
    memcpy(&elapsed, reinterpret_cast<const void*>(bus + kAuBusElapsed), 4);
    if (time > 0.0f && elapsed < time)
        return snprintf(out, size, " %s %.2fx%.2f(->%.2f %.1fs)", name, static_cast<double>(vol), static_cast<double>(gain),
                        static_cast<double>(target), static_cast<double>(time));
    return snprintf(out, size, " %s %.2fx%.2f", name, static_cast<double>(vol), static_cast<double>(gain));
}

// the sound state in one line (no time in it: equal lines = no change); game thread
void AudioStateText(char* out, size_t size)
{
    size_t at = 0;
    auto add = [&](int n) {
        if (n > 0)
            at = at + static_cast<size_t>(n) < size ? at + static_cast<size_t>(n) : size - 1;
    };
    out[0] = 0;
    const uint32_t music = g_auMusic ? *g_auMusic : 0;
    const char* mname = nullptr;
    for (const char* m : kAuMusicNames)
        if (music && AuHash(m) == music)
            mname = m;
    char mtext[40];
    if (mname)
        snprintf(mtext, sizeof(mtext), "%s", mname);
    else if (music)
        snprintf(mtext, sizeof(mtext), "%08X", music);
    else
        snprintf(mtext, sizeof(mtext), "none");
    add(snprintf(out + at, size - at, "sound %s | shell music %s, interval %.1f s", g_auSoundOn ? (*g_auSoundOn ? "ON" : "OFF") : "?",
                 mtext, g_auInterval ? static_cast<double>(*g_auInterval) : -1.0));
    // 21j-r28: its stream (as the game's music update finds it) and the engine's "restarted" byte
    if (g_auMusicEntry && music && g_auSoundOn && *g_auSoundOn)
    {
        const uintptr_t entry = g_auMusicEntry(music);
        const uintptr_t stream = entry ? g_auStreamById(g_auEntryStream(entry, nullptr)) : 0;
        if (!entry)
            add(snprintf(out + at, size - at, ", its stream: no entry"));
        else if (!stream)
            add(snprintf(out + at, size - at, ", its stream: NONE (gone)"));
        else
            add(snprintf(out + at, size - at, ", its stream: state %u", g_auStreamState(stream + 0x68, nullptr)));
    }
    if (g_auRestarted)
        add(snprintf(out + at, size - at, " | engine restarted %u", *g_auRestarted));
    if (g_auEngine[0])
        add(snprintf(out + at, size - at, " | engine music %.2f sfx %.2f voice %.2f master %.2f", static_cast<double>(*g_auEngine[0]),
                     static_cast<double>(*g_auEngine[1]), static_cast<double>(*g_auEngine[2]), static_cast<double>(*g_auEngine[3])));
    if (g_profBase && g_profCurIdx)
    {
        const int cur = static_cast<int>(ReadU32(g_profCurIdx));
        const uintptr_t p0 = g_profBase;
        const uintptr_t p1 = g_profBase + kProfileBytes;
        add(snprintf(out + at, size - at, " | profile %d; volumes (music sfx voice master) 1: %u %u %u %u, 2: %u %u %u %u", cur,
                     ReadU8(p0 + kAuProfMusic), ReadU8(p0 + kAuProfSfx), ReadU8(p0 + kAuProfVoice), ReadU8(p0 + kAuProfMaster),
                     ReadU8(p1 + kAuProfMusic), ReadU8(p1 + kAuProfSfx), ReadU8(p1 + kAuProfVoice), ReadU8(p1 + kAuProfMaster)));
    }
    if (g_auBusGet)
    {
        add(snprintf(out + at, size - at, " | buses vol x gain:"));
        add(AuBusText(out + at, size - at, "master", g_auMaster));
        for (const char* b : kAuBusNames)
            add(AuBusText(out + at, size - at, b, g_auBusGet(AuHash(b))));
    }
}

// the scripts' sound calls (LuaTraceCall: the audio names) - with the state right after; exact repeats within 1 s counted
int AudioTraceCall(TraceFn& f, const char* name, void* L)
{
    if (!g_auOn || !g_luaIndex)
        return f.orig(L);
    char args[200];
    FormatValues(L, 1, g_luaGetTop(L), args, sizeof(args));
    const int r = f.orig(L);
    // 21j-r28: its results too (GetMixConfigChanged's answer)
    char res[80] = "";
    const int top1 = g_luaGetTop(L);
    if (r > 0 && r <= top1)
    {
        char v[64];
        FormatValues(L, top1 - r + 1, top1, v, sizeof(v));
        snprintf(res, sizeof(res), " = %s", v);
    }
    uint32_t sig = Fnv(Fnv(2166136261u, args), res);
    const DWORD now = GetTickCount();
    if (f.calls > 1 && sig == f.lastSig && now - f.lastMs < 1000)
    {
        ++f.repeats;
        return r;
    }
    f.lastSig = sig;
    f.lastMs = now;
    if (AudioBudget())
    {
        char st[sizeof(g_auLast)];
        AudioStateText(st, sizeof(st));
        char rep[48] = "";
        if (f.repeats)
            snprintf(rep, sizeof(rep), " (after %lu repeat(s))", f.repeats);
        Log("[audio] %lu ms: lua %s(%s)%s%s -> %s", ScanMsSinceAttach(), name, args, res, rep, st);
    }
    f.repeats = 0;
    return r;
}

// the game's function behind trace name `name` (its registration entry's, or the one the trace keeps), 0 if none
uint32_t AuFnRva(const char* name)
{
    const int i = TraceIndex(name);
    if (i >= 0 && g_tf[i].orig)
        return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[i].orig) - ExeBase());
    const uintptr_t slot = ScanLuaFuncSlot(name);
    return slot ? static_cast<uint32_t>(ReadU32(slot) - ExeBase()) : 0;
}

uint32_t AuOperand(uint32_t rva)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

// found once the code is unlocked (LuaTraceHelpersInit), each piece byte-checked
void AudioScanInit()
{
    if (!g_audioScanIni)
    {
        Log("[audio] [Splitscreen] AudioScan=0 - no audio scanner");
        return;
    }
    // the sound on / off byte: ScriptCB_SoundEnable "mov byte [x], 1" - SoundDisable's "mov byte [x], 0" the same byte
    const uint32_t en = AuFnRva("ScriptCB_SoundEnable");
    const uint32_t dis = AuFnRva("ScriptCB_SoundDisable");
    if (en && dis && MatchPatternAt(en, "C6 05 ?? ?? ?? ?? 01 33 C0 C3") &&
        MatchPatternAt(dis, "E8 ?? ?? ?? ?? 51 C7 04 24 CD CC CC 3D E8 ?? ?? ?? ?? 83 C4 04 C6 05 ?? ?? ?? ?? 00 33 C0 C3") &&
        AuOperand(en + 2) == AuOperand(dis + 0x17))
        g_auSoundOn = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(AuOperand(en + 2)));
    // the shell music playing: ScriptCB_IsShellMusicPlaying's "cmp eax, [x]"
    const uint32_t playing = AuFnRva("ScriptCB_IsShellMusicPlaying");
    if (playing && MatchPatternAt(playing, "55 8B EC 51 6A 01 FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 8D 4D FC 50 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? "
                                           "?? ?? 33 C9 3B 05"))
        g_auMusic = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(AuOperand(playing + 0x25)));
    // its interval: ScriptCB_SetShellMusicInterval's first "movss [x], xmm0"
    const uint32_t ivl = AuFnRva("ScriptCB_SetShellMusicInterval");
    if (ivl && MatchPatternAt(ivl, "55 8B EC 51 FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 75 16 50 FF 75 08 E8 ?? ?? ?? ?? D9 5D FC F3 "
                                   "0F 10 45 FC 83 C4 08 EB 08 F3 0F 10 05 ?? ?? ?? ?? 0F 2E 05 ?? ?? ?? ?? F3 0F 11 05 ?? ?? ?? ?? F3 0F 11 05"))
        g_auInterval = reinterpret_cast<float*>(static_cast<uintptr_t>(AuOperand(ivl + 0x3D)));
    // the engine's volumes: ScriptCB_SetVolumes -> the profile's apply (+D1h) -> the engine (+AFh) - and the bus lookup (+3Ch)
    const uint32_t setv = AuFnRva("ScriptCB_SetVolumes");
    uint32_t apply = 0, engine = 0, lookup = 0, find = 0, setVol = 0;
    if (setv && MatchPatternAt(setv + 0x82, "E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 8B F8 83 C4 24 85 FF 74 40 F3 0F 2C 55 F8 F3 0F 2C 4D FC 88 "
                                            "97 6D 02 00 00 F3 0F 2C 55 F4 88 8F 6C 02 00 00 8B CF F3 0F 2C 45 F0 88 97 6E 02 00 00 88 "
                                            "87 72 02 00 00 E8 ?? ?? ?? ?? 6A 00 8D 8F 6C 02 00 00 E8"))
        apply = TraceCallTarget(setv + 0xD1);
    if (apply && MatchPatternAt(apply, "55 8B EC 80 7D 08 00 56 8B F1 74 18 80 7E 05 00 0F 95 C0") &&
        MatchPatternAt(apply + 0xAA, "F3 0F 11 04 24 E8"))
        engine = TraceCallTarget(apply + 0xAF);
    if (engine && MatchPatternAt(engine, "55 8B EC F3 0F 10 45 10 F3 0F 11 05 ?? ?? ?? ?? F3 0F 10 45 08 F3 0F 11 05 ?? ?? ?? ?? F3 0F "
                                         "10 45 0C F3 0F 11 05 ?? ?? ?? ?? F3 0F 10 45 14 68 4B AA 13 3B F3 0F 11 05 ?? ?? ?? ?? E8"))
    {
        g_auEngine[0] = reinterpret_cast<float*>(static_cast<uintptr_t>(AuOperand(engine + 0x26))); // music ([ebp+0Ch])
        g_auEngine[1] = reinterpret_cast<float*>(static_cast<uintptr_t>(AuOperand(engine + 0x19))); // sfx ([ebp+8])
        g_auEngine[2] = reinterpret_cast<float*>(static_cast<uintptr_t>(AuOperand(engine + 0x0C))); // voice ([ebp+10h])
        g_auEngine[3] = reinterpret_cast<float*>(static_cast<uintptr_t>(AuOperand(engine + 0x38))); // master ([ebp+14h])
        lookup = TraceCallTarget(engine + 0x3C);
        setVol = TraceCallTarget(engine + 0x62);
    }
    if (lookup && MatchPatternAt(lookup, "55 8B EC FF 75 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3"))
        find = TraceCallTarget(lookup + 0x0B);
    // the bus's own find (its hash at +28h) and SetVolume (its volume at +20h) - the fields read above
    if (find && MatchPatternAt(find, "55 8B EC 56 8B 75 08 39 71 28 75 07 8B C1 5E 5D C2 04 00") && setVol &&
        MatchPatternAt(setVol, "55 8B EC 56 8B F1 F6 46 40 02 75 56 80 7D 0C 00 F3 0F 10 45 08 F3 0F 11 46 20"))
    {
        g_auBusGet = reinterpret_cast<AuBusGet_t>(Rva(lookup));
        g_auMaster = AuOperand(lookup + 7);
    }
    // 21j-r28: the restarted byte - ScriptCB_GetMixConfigChanged's getter ("mov al, [x]; mov byte [x], 0")
    const uint32_t mix = AuFnRva("ScriptCB_GetMixConfigChanged");
    const uint32_t mixGet = (mix && MatchPatternAt(mix, "55 8B EC E8 ?? ?? ?? ?? 0F B6 C0 50 FF 75 08 E8")) ? TraceCallTarget(mix + 3) : 0;
    if (mixGet && MatchPatternAt(mixGet, "A0 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 00 C3") && AuOperand(mixGet + 1) == AuOperand(mixGet + 7))
        g_auRestarted = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(AuOperand(mixGet + 1)));
    // 21j-r28: the shell music's stream, through the game's music update (found by its body: the sound on byte and the music
    // global as above)
    uint32_t upd[4] = {};
    const int nu = FindPattern("55 8B EC 80 3D ?? ?? ?? ?? 00 74 6E A1 ?? ?? ?? ?? 85 C0 74 65 56 50 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 74 54 "
                               "8B CE E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 40 8D 48 68 E8 ?? ?? ?? ?? 83 F8 04 75 33",
                               upd, 4);
    const uint32_t mu = nu == 1 ? upd[0] : 0;
    if (mu && g_auSoundOn && g_auMusic && AuOperand(mu + 5) == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_auSoundOn)) &&
        AuOperand(mu + 0x0D) == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_auMusic)))
    {
        const uint32_t idFn = TraceCallTarget(mu + 0x27);
        if (idFn && MatchPatternAt(idFn, "8B 81 ?? ?? 00 00 C3"))
        {
            g_auMusicEntry = reinterpret_cast<AuMusicEntry_t>(Rva(TraceCallTarget(mu + 0x17)));
            g_auEntryStream = reinterpret_cast<AuThis_t>(Rva(idFn));
            g_auStreamById = reinterpret_cast<AuStream_t>(Rva(TraceCallTarget(mu + 0x2D)));
            g_auStreamState = reinterpret_cast<AuThis_t>(Rva(TraceCallTarget(mu + 0x3C)));
        }
    }
    Log("[audio] 21j-r28: the sound engine's restarted byte %08X (GetMixConfigChanged rva 0x%08X) %s; the shell music's stream "
        "through the music update rva 0x%08X (%d hit(s)) %s",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_auRestarted)), mix, g_auRestarted ? "OK" : "NOT found", mu, nu,
        g_auMusicEntry ? "OK" : "NOT found");
    g_auOn = g_auSoundOn || g_auMusic || g_auEngine[0] || g_auBusGet;
    Log("[audio] 21j-r27: THE AUDIO SCANNER (AudioScan=1, log only): the scripts' sound / music / movie calls on every screen with "
        "the sound right after them, and every 250 ms (a line when it changes): the sound on / off byte %08X (SoundEnable rva "
        "0x%08X) %s, the shell music %08X (IsShellMusicPlaying rva 0x%08X) %s, its interval %08X %s, the engine's volumes %s "
        "(SetVolumes rva 0x%08X -> apply rva 0x%08X -> rva 0x%08X), the profiles' volumes %s, the buses %s (lookup rva 0x%08X, "
        "master bus %08X, find rva 0x%08X, SetVolume rva 0x%08X)%s",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_auSoundOn)), en, g_auSoundOn ? "OK" : "NOT found",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_auMusic)), playing, g_auMusic ? "OK" : "NOT found",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_auInterval)), g_auInterval ? "OK" : "NOT found",
        g_auEngine[0] ? "OK" : "NOT found", setv, apply, engine, (g_profBase && g_profCurIdx) ? "OK" : "NOT found (no profiles)",
        g_auBusGet ? "OK" : "NOT found", lookup, static_cast<unsigned>(g_auMaster), find, setVol,
        g_auOn ? "" : " - NOTHING found: the scanner is off");
}

// game thread, each frame (the menu layer's, p2spawn_scan.cpp): the sound state every 250 ms, a line when it changes
void AudioScanFrame()
{
    if (!g_auOn)
        return;
    const DWORD now = GetTickCount();
    if (now - g_auLastSample < 250)
        return;
    g_auLastSample = now;
    char st[sizeof(g_auLast)];
    AudioStateText(st, sizeof(st));
    if (strcmp(st, g_auLast) == 0)
        return;
    snprintf(g_auLast, sizeof(g_auLast), "%s", st);
    if (AudioBudget())
        Log("[audio] %lu ms: %s", ScanMsSinceAttach(), st);
}

int __cdecl LuaTraceCall(uint32_t index, void* L)
{
    TraceFn& f = g_tf[index];
    ++f.calls;
    if (!g_traceHelpersOk)
        return f.orig(L);
    if (f.audio) // 21j-r27: the audio scanner's calls - its own lines
        return AudioTraceCall(f, kTraceNames[index] + 9, L);

    const char* name = kTraceNames[index] + 9; // without "ScriptCB_"
    const bool screenCall = strcmp(name, "PushScreen") == 0 || strcmp(name, "SetIFScreen") == 0;
    const bool platformCall = strcmp(name, "GetPlatform") == 0;
    const bool addScreenCall = strcmp(name, "AddIFScreen") == 0;
    const bool resetCall = strcmp(name, "ResetControl") == 0;
    // 21f: the calls that change the sides: logged with the sides after them
    const bool sideCall = strcmp(name, "SetCanSwitchSides") == 0 || strcmp(name, "SetPlayerSide") == 0 ||
                          strcmp(name, "SetMetagameTeams") == 0 || strcmp(name, "SetMetagameRulesOn") == 0;
    const bool armed = g_traceArmed && !(g_traceSplit && f.quiet); // 21e: some calls not logged on the split screens
    if (f.calls != 1 && !armed && !screenCall && !platformCall && !addScreenCall && !resetCall && !sideCall)
        return f.orig(L); // nothing to log: as cheap as the stub
    if (sideCall)
    {
        char sargs[120];
        FormatValues(L, 1, g_luaGetTop(L), sargs, sizeof(sargs));
        const int r0 = f.orig(L);
        SidesLog(name, sargs);
        return r0;
    }

    // 21d: the page script's log lines (ScriptCB_GetPlatform("BFX: ...")) - not passed to the game
    // 21e: the split screen menu script's log lines ("BFXS: ...") and questions ("BFXQ: ...", a number back)
    if (platformCall && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* s = g_luaToString(L, 1);
        if (s && strncmp(s, "BFX: ", 5) == 0)
        {
            if (PageBudget())
                Log("[ctrlpage] %lu ms: %s", ScanMsSinceAttach(), s + 5);
            return 0;
        }
        if (s && strncmp(s, "BFXS: ", 6) == 0)
        {
            if (SplitBudget())
                Log("[splitmenu] %lu ms: %s", ScanMsSinceAttach(), s + 6);
            return 0;
        }
        // 21g-r4: the galaxy turn scanner's lines - their own budget
        if (s && strncmp(s, "BFXT: ", 6) == 0)
        {
            if (g_gcTurnLines < kGCTurnMaxLines)
            {
                ++g_gcTurnLines;
                Log("[gcturn] %lu ms: %s", ScanMsSinceAttach(), s + 6);
                if (g_gcTurnLines == kGCTurnMaxLines)
                    Log("[gcturn] line limit reached (%d), going quiet", kGCTurnMaxLines);
            }
            return 0;
        }
        // 21i-r4: the Instant Action options scan's Lua lines - their own budget
        if (s && strncmp(s, "BFXI: ", 6) == 0)
        {
            if (g_iaLines < kIaMaxLines)
            {
                ++g_iaLines;
                Log("[iaopts] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
                if (g_iaLines == kIaMaxLines)
                    Log("[iaopts] lua line limit reached (%d), going quiet", kIaMaxLines);
            }
            return 0;
        }
        // 21h-r2: players 3 / 4's real pause menus' Lua lines - their own budget
        if (s && strncmp(s, "BFXR: ", 6) == 0)
        {
            if (g_rpLuaLines < kRpLuaMaxLines)
            {
                ++g_rpLuaLines;
                Log("[rp34] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
                if (g_rpLuaLines == kRpLuaMaxLines)
                    Log("[rp34] lua line limit reached (%d), going quiet", kRpLuaMaxLines);
            }
            return 0;
        }
        // 21h-r4: the options pages scan's Lua lines - their own budget
        if (s && strncmp(s, "BFXD: ", 6) == 0)
        {
            if (g_optScanLines < kOptScanMaxLines)
            {
                ++g_optScanLines;
                Log("[optscan] %s", s + 6);
                if (g_optScanLines == kOptScanMaxLines)
                    Log("[optscan] line limit reached (%d), going quiet", kOptScanMaxLines);
            }
            return 0;
        }
        // 21h-r1: the pause scan's Lua lines - their own budget
        if (s && strncmp(s, "BFXP: ", 6) == 0)
        {
            if (g_pauseLuaLines < kPauseLuaMaxLines)
            {
                ++g_pauseLuaLines;
                Log("[pause34] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
                if (g_pauseLuaLines == kPauseLuaMaxLines)
                    Log("[pause34] lua line limit reached (%d), going quiet", kPauseLuaMaxLines);
            }
            return 0;
        }
        // 21j-r23: the FREE CAMERA / FAKE CONSOLE script's lines - their own budget
        if (s && strncmp(s, "BFXK: ", 6) == 0)
        {
            if (FcBudget())
                Log("[freecam] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
            return 0;
        }
        // 21j-r21: the TUTORIALS script's lines - their own budget
        if (s && strncmp(s, "BFXU: ", 6) == 0)
        {
            if (TutBudget())
                Log("[tutorials] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
            return 0;
        }
        // 21j-r17: the MODS pages' Lua lines - their own budget
        if (s && strncmp(s, "BFXM: ", 6) == 0)
        {
            if (ModsBudget())
                Log("[mods] lua %lu ms: %s", ScanMsSinceAttach(), s + 6);
            return 0;
        }
        if (s && strncmp(s, "BFXQ: ", 6) == 0)
        {
            // 21j-r17: "mods addons" hands the ADDON FOLDER page its tables, run in this Lua state
            const int answer = strcmp(s + 6, "mods addons") == 0 ? ModsAddonsRun(L) : SplitQuery(s + 6);
            if (!g_luaPushInt)
                return 0; // the script sees nil
            g_luaPushInt(L, answer);
            return 1;
        }
    }
    // 21e: the split screen menu script goes in while the game makes ifs_main / ifs_split_profile, and when it opens them
    if ((addScreenCall || strcmp(name, "PushScreen") == 0) && g_splitSrc && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        const char* known = nullptr;
        if (screen && strcmp(screen, "ifs_main") == 0)
            known = "ifs_main";
        else if (screen && strcmp(screen, "ifs_split_profile") == 0)
            known = "ifs_split_profile";
        else if (screen && strcmp(screen, "ifs_meta_opts") == 0) // 21f: Galactic Conquest's maps
            known = "ifs_meta_opts";
        else if (screen && strcmp(screen, "ifs_meta_top") == 0) // 21f: PICK YOUR SIDE
            known = "ifs_meta_top";
        else if (addScreenCall && screen && strcmp(screen, "ifs_saveop") == 0) // 21g-r2: no save of a temporary profile 2
            known = "ifs_saveop";
        else if (screen && strcmp(screen, "ifs_meta_main") == 0) // 21g-r3: the galaxy map's commanders
            known = "ifs_meta_main";
        if (known)
            SplitRun(L, known, addScreenCall ? "build" : "push");
    }
    // 21h-r1: the pause scan's Lua side - while the game makes ifs_pausemenu2 (split screen; ifs_pausemenu is made already)
    if (addScreenCall && g_pauseScanIni && g_luaDoBuffer && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_pausemenu2") == 0)
            PauseProbeRun(L);
    }
    // 21h-r2: players 3 / 4's real pause menus - made right after the game finishes ifs_pausemenu2 (3 / 4 players this match)
    if (addScreenCall && g_realPauseIni && g_rpSrc && g_luaDoBuffer && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_pausemenu2") == 0)
            RealPauseRun(L);
    }
    // 21j-r15: player 2's pause menu as player 1's - the 2-player split (while the game makes ifs_pausemenu2 in a 2-player match;
    // with 3 / 4 players BF1GameExt_pause34.lua does it)
    if (addScreenCall && g_luaDoBuffer && P2SpawnP2PauseLike() && PlayerNLocalPlayers() < 3 && g_luaGetTop(L) >= 1 &&
        LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_pausemenu2") == 0)
            P2PauseLuaRun(L);
    }
    // 21j-r23: player 1's pause menu - FREE CAMERA / FAKE CONSOLE (while the game makes ifs_pausemenu / ifs_fakeconsole)
    if (addScreenCall && g_luaDoBuffer && (g_pauseCamIni > 0 || g_pauseConIni > 0) && g_luaGetTop(L) >= 1 &&
        LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && (strcmp(screen, "ifs_pausemenu") == 0 || strcmp(screen, "ifs_fakeconsole") == 0))
            PauseExtrasRun(L, screen);
    }
    // 21h-r11: player 1's pause menu - no RESPAWN over the stats screen (every match; while the game makes ifs_pausemenu)
    if (addScreenCall && g_luaDoBuffer && P2SpawnStatsPauseOn() && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_pausemenu") == 0)
            StatsPauseLuaRun(L);
    }
    // 21i-r6: Instant Action's GAME OPTIONS page - while the game makes the Instant Action / split screen map list, and the
    // options menu's GAME OPTIONS page (its HEROES row)
    // 21i-r9: auto aim for the mouse / keyboard player - once, while the game makes its first screen (before any match)
    if (addScreenCall && !g_aimMouseTried)
        AimMouseInstall();
    // 21i-r13: the pads' dead zone and the look curve - once, with the first screen
    if (addScreenCall && !g_padFeelTried)
        PadFeelInstall();
    // 21i-r16: the FOV getter - once, with the first screen
    if (addScreenCall && !g_fovTried)
        FovInstall();
    // 21j-r1: the grass scan - once, with the first screen
    if (addScreenCall && !g_grassTried)
        GrassScanInstall();
    // 21i-r8: and GAME OPTIONS' SPLIT SCREEN / AUTO AIM rows; a SPLIT SCREEN chosen in a match goes in when the main menu is
    // made again
    if (addScreenCall && g_luaDoBuffer && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_missionselect") == 0 && g_iaPageIni)
            IAOptsRun(L, screen);
        else if (screen && strcmp(screen, "ifs_opt_general") == 0 &&
                 (g_iaPageIni || g_splitMenuIni || g_aimMenuIni || g_wheelMenuIni)) // 21j-r18: SPAWN SCREEN
            IAOptsRun(L, screen);
        else if (screen && strcmp(screen, "ifs_main") == 0)
        {
            VerticalApplyPending();
            if (TutorialsAnyOn()) // 21j-r21: the TUTORIALS button; 21j-r22: the movie box, the campaigns' tutorials
                TutorialsRun(L, screen);
        }
        else if (screen && strcmp(screen, "ifs_tutorials") == 0 && TutorialsAnyOn()) // 21j-r21 / r22: the tutorials
                                                                                           // menu / viewer, GC, the campaigns
            TutorialsRun(L, screen);
        else if (screen && strcmp(screen, "ifs_opt_pcvideo") == 0 && g_fovMenuIni) // 21i-r16: the FIELD OF VIEW row
            FovRun(L);
        else if (screen && strcmp(screen, "ifs_opt_contmain") == 0 && g_modsMenuIni) // 21j-r17: the MODS row and pages
            ModsRun(L);
    }
    // 21d: the page script goes in right before the controller page opens
    if (strcmp(name, "PushScreen") == 0 && g_pageSrc && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_opt_controller") == 0)
            PageRun(L, "push");
    }
    // 21d-r2: the game making the controller page - the script goes in first (its RESET button is made with the rest)
    if (addScreenCall && g_pageSrc && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        if (screen && strcmp(screen, "ifs_opt_controller") == 0)
            PageRun(L, "build");
    }
    // 21h-r4: the options pages scan - the game's GAME OPTIONS / gamepad pages as it builds them (once each this session)
    if (addScreenCall && g_optScanIni && g_luaDoBuffer && g_luaGetTop(L) >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        const int which = !screen ? -1 : strcmp(screen, "ifs_opt_general") == 0 ? 0 : strcmp(screen, "ifs_opt_controller") == 0 ? 1 : -1;
        if (which >= 0 && !g_optScanned[which] && PlayerNPlayersMax() >= 3)
        {
            g_optScanned[which] = true;
            OptScanRun(L, screen);
        }
    }
    // 21d-r2: the game's own reset (PC controls page): the pad layout of the same tab follows
    if (resetCall && g_pageSrc)
    {
        const int r0 = f.orig(L);
        PadResetAfterGameReset(L);
        if (TraceBudget())
            Log("[luacb] %lu ms: ResetControl() -> %d result(s) [player %d] - then the pad layout of the same tab",
                ScanMsSinceAttach(), r0, g_curPlayer ? g_curPlayer() : -99);
        return r0;
    }
    if (f.calls != 1 && !armed && !screenCall)
        return f.orig(L);
    const int top0 = g_luaGetTop(L);
    char args[200];
    FormatValues(L, 1, top0, args, sizeof(args));
    const int player = (static_cast<int>(index) < kTraceControllerGroup && g_curPlayer) ? g_curPlayer() : -99;

    // Screen changes arm / disarm the full logging (the argument is the screen's name).
    if (screenCall && top0 >= 1 && LuaArgStr(L, 1))
    {
        const char* screen = g_luaToString(L, 1);
        bool split = false;
        const bool opt = TraceArmScreen(screen, &split); // 21e: the split screen screens too
        if ((opt != g_traceArmed || (opt && split != g_traceSplit)) && TraceBudget())
            Log("[luacb] %lu ms: %s(\"%s\") - full logging %s", ScanMsSinceAttach(), name, screen ? screen : "",
                !opt ? "OFF" : split ? "ON (a split screen screen; its IFObj / IFText calls not logged)" : "ON (an options screen)");
        if (!opt && g_traceArmed)
        {
            for (int i = 0; i < kTraceCount; ++i)
            {
                if (g_tf[i].repeats && TraceBudget())
                    Log("[luacb]   %s: %lu repeat(s) not logged", kTraceNames[i] + 9, g_tf[i].repeats);
                g_tf[i].repeats = 0;
                g_tf[i].lastSig = 0;
            }
        }
        g_traceArmed = opt;
        g_traceSplit = split;
    }

    const int r = f.orig(L);

    const int top1 = g_luaGetTop(L);
    char res[200] = "";
    if (r > 0 && r <= top1)
        FormatValues(L, top1 - r + 1, top1, res, sizeof(res));

    const bool first = f.calls == 1;
    if (!first && !(g_traceArmed && !(g_traceSplit && f.quiet)))
        return r;
    uint32_t sig = Fnv(Fnv(2166136261u, args), res);
    sig = sig * 31u + static_cast<uint32_t>(player + 100);
    if (!first && sig == f.lastSig)
    {
        ++f.repeats;
        return r;
    }
    f.lastSig = sig;
    if (!TraceBudget())
        return r;
    char who[24] = "";
    if (player != -99)
        snprintf(who, sizeof(who), " [player %d]", player);
    if (f.repeats)
        Log("[luacb] %lu ms: %s%s(%s) -> %d result(s)%s%s%s (after %lu repeat(s))", ScanMsSinceAttach(),
            first ? "FIRST " : "", name, args, r, r ? ": " : "", res, who, f.repeats);
    else
        Log("[luacb] %lu ms: %s%s(%s) -> %d result(s)%s%s%s", ScanMsSinceAttach(), first ? "FIRST " : "", name, args,
            r, r ? ": " : "", res, who);
    f.repeats = 0;
    return r;
}

// 21j-r27: [Splitscreen] AudioScan (read before the trace goes in: its calls are wrapped only with it on)
bool AudioScanIniOn()
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
    return GetPrivateProfileIntA("Splitscreen", "AudioScan", 1, ini) != 0;
}

bool LuaTraceIniOn()
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
    return GetPrivateProfileIntA("Controls", "LuaTrace", 1, ini) != 0;
}

uint32_t TraceFollowThunks(uint32_t t)
{
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
    {
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(Rva(t + 1)), sizeof(rel));
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + rel);
    }
    return t;
}

uint32_t TraceCallTarget(uint32_t rva)
{
    if (*reinterpret_cast<const unsigned char*>(Rva(rva)) != 0xE8)
        return 0;
    int32_t rel = 0;
    memcpy(&rel, reinterpret_cast<const void*>(Rva(rva + 1)), sizeof(rel));
    return TraceFollowThunks(static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + rel));
}

bool TraceCheck(const char* what, uint32_t rva, const char* pattern)
{
    if (rva && MatchPatternAt(rva, pattern))
    {
        Log("[luacb] %-40s rva 0x%08X  OK", what, rva);
        return true;
    }
    Log("[luacb] %-40s rva 0x%08X  does NOT match - arguments not logged (calls pass through)", what, rva);
    return false;
}

int TraceIndex(const char* name)
{
    for (int i = 0; i < kTraceCount; ++i)
        if (strcmp(kTraceNames[i], name) == 0)
            return i;
    return -1;
}

// After the code is unlocked: the Lua helpers, from the two wrapped functions' own calls.
void LuaTraceHelpersInit()
{
    const int gb = TraceIndex("ScriptCB_GetFunctionIdForButtonId");
    const int ps = TraceIndex("ScriptCB_PushScreen");
    if (gb < 0 || ps < 0 || !g_tf[gb].orig || !g_tf[ps].orig)
    {
        Log("[luacb] the trace is not installed - no helpers needed");
        return;
    }
    const uint32_t getBtn =
        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[gb].orig) - ExeBase());
    const uint32_t push = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[ps].orig) - ExeBase());
    if (!TraceCheck("ScriptCB_GetFunctionIdForButtonId", getBtn,
                    "55 8B EC 56 8B 75 08 56 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 75 3B 50 56 E8 ?? ?? ?? ?? 83 C4 08 84 C0 "
                    "74 2D 6A 01 56 E8 ?? ?? ?? ?? 48 50 E8 ?? ?? ?? ?? 50 E8") ||
        !TraceCheck("ScriptCB_PushScreen", push,
                    "55 8B EC 56 8B 75 08 56 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 75 46 50 56 E8 ?? ?? ?? ?? 83 C4 08 84 C0 "
                    "74 38 53 57 6A 01 56 E8"))
        return;
    const uint32_t getTop = TraceCallTarget(getBtn + 0x08);
    const uint32_t isNum = TraceCallTarget(getBtn + 0x17);
    const uint32_t toInt = TraceCallTarget(getBtn + 0x26);
    const uint32_t getCur = TraceCallTarget(getBtn + 0x33);
    const uint32_t isStr = TraceCallTarget(push + 0x17);
    const uint32_t toStr = TraceCallTarget(push + 0x28);
    if (!TraceCheck("lua gettop", getTop, "55 8B EC 5D E9") ||
        !TraceCheck("lua isnumber", isNum, "55 8B EC FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 0F 95 C0 5D C3") ||
        !TraceCheck("lua isstring", isStr, "55 8B EC FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 0F 95 C0 5D C3") ||
        !TraceCheck("lua tonumber (as int)", toInt, "55 8B EC 51 FF 75 0C FF 75 08 E8 ?? ?? ?? ?? D9 5D FC F3 0F 2C 45 FC") ||
        !TraceCheck("lua tostring", toStr, "55 8B EC 6A 00 FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 0C 5D C3"))
        return;
    const uint32_t toNum = TraceCallTarget(toInt + 0x0A);
    if (!TraceCheck("lua tonumber", toNum, "55 8B EC 51 56 8B 75 0C 57 8B 7D 08 56 57 E8"))
        return;
    // 21j-r27: the stack index helper, through isnumber (+9: the API's lua_isnumber, GOG 1.3 0x0065C110; its +9: the helper
    // 0x0065BCA0) - the arguments are read as they are (LuaValueText); without it none is read
    {
        const uint32_t isNumApi = TraceCallTarget(isNum + 0x09);
        const uint32_t index = isNumApi ? TraceCallTarget(isNumApi + 0x09) : 0;
        if (TraceCheck("lua isnumber (the API's)", isNumApi,
                       "55 8B EC FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 75 04 33 C0 5D C3 83 38 02 74 0D 50 E8") &&
            TraceCheck("lua stack index helper", index,
                       "55 8B EC 8B 45 0C 8D 14 C5 00 00 00 00 85 C0 78 13 8B 4D 08 8B 41 10 83 C0 F8 03 D0 3B 11 1B C0 23 C2 5D C3"))
        {
            g_luaIndex = reinterpret_cast<LuaIndex_t>(Rva(index));
            Log("[luacb] 21j-r27: the arguments are read straight from the Lua stack (index helper rva 0x%08X) - nothing is "
                "converted (the slider numbers' \"40%%\" stayed a string)", index);
        }
        else
        {
            Log("[luacb] 21j-r27: the Lua stack index helper NOT found - the calls are logged without their arguments");
        }
    }
    uint32_t curPlayer = 0;
    if (TraceCheck("GetBinding for the current player", getCur, "55 8B EC 51 56 57 E8 ?? ?? ?? ?? 8B F0 8B 3E E8"))
    {
        curPlayer = TraceCallTarget(getCur + 0x0F);
        if (!TraceCheck("current player getter", curPlayer, "A1 ?? ?? ?? ?? C3"))
            curPlayer = 0;
    }
    // 21e: pushnumber of an int (the split screen script's questions get a number back), through
    // ScriptCB_GetFunctionIdForButtonId's own result: "push eax; push esi; call" at +3Eh
    uint32_t pushInt = 0;
    if (g_splitSrc)
    {
        if (MatchPatternAt(getBtn + 0x3E, "50 56 E8"))
        {
            pushInt = TraceCallTarget(getBtn + 0x40);
            if (!TraceCheck("lua pushnumber (an int)", pushInt,
                            "55 8B EC 66 0F 6E 45 0C 0F 5B C0 51 F3 0F 11 04 24 FF 75 08 E8"))
                pushInt = 0;
        }
        else
        {
            Log("[luacb] ScriptCB_GetFunctionIdForButtonId +3Eh is not \"push eax; push esi; call\" - no pushnumber");
        }
    }
    // 21f: the "can switch sides" byte: ScriptCB_SetCanSwitchSides = toboolean(arg 1) -> its setter (call at +14h):
    // "mov al, [ebp+8]; mov [byte], al" (GOG 1.3 0x0056F4A0 -> 0x0056FE10, byte 0x008382F9); the metagame rules byte the
    // same way through ScriptCB_SetMetagameRulesOn (0x0056F470 -> 0x0056FE80, byte 0x008A4DA8) - the game's own "can switch
    // sides" (GOG 1.3 0x00477270, offline) is: the first on and the second off. The side table:
    // ScriptCB_SetPlayerSide's "lea eax, [eax+eax*2]; shl eax, 4; mov [eax+table], ebx" at +67h (GOG 1.3 0x0056A180,
    // table 0x00968FEC - player 1 at +0, player 2 at +30h; SetMetagameTeams writes the same two)
    {
        const int cs = TraceIndex("ScriptCB_SetCanSwitchSides");
        const int mr = TraceIndex("ScriptCB_SetMetagameRulesOn");
        const int ps = TraceIndex("ScriptCB_SetPlayerSide");
        for (int k = 0; k < 2; ++k)
        {
            const int idx = k == 0 ? cs : mr;
            if (idx < 0 || !g_tf[idx].orig)
                continue;
            const uint32_t fn = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[idx].orig) - ExeBase());
            if (!TraceCheck(kTraceNames[idx], fn,
                            "55 8B EC 51 6A 01 FF 75 08 E8 ?? ?? ?? ?? 88 45 FC FF 75 FC E8 ?? ?? ?? ?? 83 C4 0C 33 C0 8B E5 5D C3"))
                continue;
            const uint32_t setter = TraceCallTarget(fn + 0x14);
            if (!TraceCheck(k == 0 ? "its setter (the can-switch-sides byte)" : "its setter (the metagame rules byte)", setter,
                            "55 8B EC 8A 45 08 A2 ?? ?? ?? ?? 5D C3"))
                continue;
            uint32_t va = 0;
            memcpy(&va, reinterpret_cast<const void*>(Rva(setter + 7)), sizeof(va));
            (k == 0 ? g_canSwitchByte : g_metaRulesByte) = reinterpret_cast<volatile const uint8_t*>(static_cast<uintptr_t>(va));
        }
        if (ps >= 0 && g_tf[ps].orig)
        {
            const uint32_t fn = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[ps].orig) - ExeBase());
            if (TraceCheck("ScriptCB_SetPlayerSide", fn, "55 8B EC 53 56 8B 75 08 57 56 E8 ?? ?? ?? ?? 8B F8 83 C4 04 8D 4F FF "
                                                         "83 F9 01 77 57") &&
                TraceCheck("its side table store (+67h)", fn + 0x67, "8D 04 40 C1 E0 04 89 98"))
            {
                uint32_t va = 0;
                memcpy(&va, reinterpret_cast<const void*>(Rva(fn + 0x6F)), sizeof(va));
                g_sideTable = static_cast<uintptr_t>(va);
            }
        }
        Log("[splitmenu] sides: the can-switch-sides byte %s, the metagame rules byte %s, the side table %s; [Splitscreen] "
            "SideLock=%d%s",
            g_canSwitchByte ? "found" : "NOT found", g_metaRulesByte ? "found" : "NOT found", g_sideTable ? "found" : "NOT found",
            g_sideLockIni ? 1 : 0,
            !g_sideLockIni     ? " - the side screens always come up"
            : g_canSwitchByte ? " - players 1 and 2 get no side screen while the game says sides cannot be switched "
                                "(campaign, Galactic Conquest)"
                              : " - NOT known: the side screens always come up");
    }
    // 21g-r2: the game's two profiles, through ScriptCB_GetLoginList's own "make player 2 a profile" code (split screen with
    // fewer than 2 profiles, GOG 1.3 0x00569B56): GetProfile(1) (0x005F4000: profiles at 0x009690A0, 0x458 bytes each) gets
    // the current profile copied in (GetCurrentProfile 0x005F3F10: the index at 0x00969958; "mov ecx, 116h; rep movsd"),
    // its name set (0x005F49E0: 16 wide chars at +2) and the local player slots' profile pointers written (0x00969028 /
    // 0x00968FF8). Each piece found once in the function's first 300h bytes, then its target checked.
    if (g_splitSrc)
    {
        const int gl = TraceIndex("ScriptCB_GetLoginList");
        const uint32_t fn = (gl >= 0 && g_tf[gl].orig)
                                ? static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_tf[gl].orig) - ExeBase())
                                : 0;
        const char* const pats[5] = {
            "6A 01 E8 ?? ?? ?? ?? 8B 75 F8 83 C4 04 8B F8 89 45 F0 B9 16 01 00 00 F3 A5", // GetProfile(1), the copy
            "E8 ?? ?? ?? ?? 89 45 F8 85 C0 74",                                           // GetCurrentProfile
            "8B CF 50 E8 ?? ?? ?? ?? 8B CF E8",                                           // SetName, then GetName
            "6A 01 A3 ?? ?? ?? ?? E8",                                                    // local slot 2's profile, Set(1)
            "6A 00 A3 ?? ?? ?? ?? E8",                                                    // local slot 1's profile, Set(0)
        };
        uint32_t at[5] = {};
        bool ok = fn && TraceCheck("ScriptCB_GetLoginList", fn, "55 8B EC 83 EC 18 53 8B 5D 08 56 53 E8");
        for (int k = 0; ok && k < 5; ++k)
        {
            int hits = 0;
            for (uint32_t i = 0; i < 0x300; ++i)
                if (MatchPatternAt(fn + i, pats[k]))
                {
                    if (!hits)
                        at[k] = fn + i;
                    ++hits;
                }
            if (hits != 1)
            {
                Log("[splitmenu] profiles: ScriptCB_GetLoginList's piece %d found %d times (need 1) - no temporary profiles", k + 1,
                    hits);
                ok = false;
            }
        }
        uint32_t base = 0;
        uint32_t base2 = 0;
        uint32_t cur = 0;
        uint32_t slot0 = 0;
        uint32_t slot1 = 0;
        uint32_t setCur = 0;
        if (ok)
        {
            const uint32_t getProf = TraceCallTarget(at[0] + 2);
            const uint32_t getCur = TraceCallTarget(at[1]);
            const uint32_t setName = TraceCallTarget(at[2] + 3);
            ok = TraceCheck("GetProfile (profiles, 0x458 bytes)", getProf, "55 8B EC 8B 45 08 83 F8 01 77 0D 69 C0 58 04 00 00 05") &&
                 TraceCheck("GetCurrentProfile", getCur, "A1 ?? ?? ?? ?? 83 F8 01 77 0C 69 C0 58 04 00 00 05") &&
                 TraceCheck("SetName (16 wide chars at +2)", setName, "55 8B EC 6A 10 FF 75 08 8D 41 02 50 E8");
            if (ok)
            {
                memcpy(&base, reinterpret_cast<const void*>(Rva(getProf + 0x12)), sizeof(base));
                memcpy(&cur, reinterpret_cast<const void*>(Rva(getCur + 1)), sizeof(cur));
                memcpy(&base2, reinterpret_cast<const void*>(Rva(getCur + 0x11)), sizeof(base2));
                memcpy(&slot1, reinterpret_cast<const void*>(Rva(at[3] + 3)), sizeof(slot1));
                memcpy(&slot0, reinterpret_cast<const void*>(Rva(at[4] + 3)), sizeof(slot0));
                ok = base && base == base2 && cur && slot0 && slot1 == slot0 + kSideStride;
            }
            // the call right after each store: SetCurrentProfile (GOG 1.3 0x005F4870 - "mov [index], ecx" at +12h, then each
            // local slot that has that profile gets its controls) - the same function both times
            if (ok)
            {
                setCur = TraceCallTarget(at[3] + 7);
                uint32_t idxOp = 0;
                ok = setCur && TraceCallTarget(at[4] + 7) == setCur &&
                     TraceCheck("SetCurrentProfile", setCur, "55 8B EC 8B 4D 08 8D 41 01 83 F8 02 0F 87 ?? ?? ?? ?? 89 0D");
                if (ok)
                {
                    memcpy(&idxOp, reinterpret_cast<const void*>(Rva(setCur + 0x14)), sizeof(idxOp));
                    ok = idxOp == cur;
                    if (!ok)
                        Log("[splitmenu] profiles: SetCurrentProfile writes %08X, not the current index %08X", idxOp, cur);
                }
            }
        }
        if (ok)
        {
            g_profSetCur = reinterpret_cast<SetCurProfile_t>(Rva(setCur));
            g_profBase = base;
            g_profCurIdx = cur;
            g_profSlot0 = slot0;
            g_profSlot1 = slot1;
        }
        Log("[splitmenu] profiles: %s (profiles at %08X, the current one's index at %08X, the local slots' profile pointers "
            "%08X / %08X); [Splitscreen] TempProfiles=%d",
            ok ? "found" : "NOT found - no temporary profiles", base, cur, slot0, slot1, g_tempProfilesIni ? 1 : 0);
    }
    // 21d: lua_dobuffer, through the base library's dostring; and the real gettop, for the stack top at L + 0
    const uint32_t realTop = TraceFollowThunks(getTop + 4);
    const bool topOk = TraceCheck("lua gettop (real: top at L+0, 8-byte values)", realTop,
                                  "55 8B EC 8B 4D 08 8B 01 2B 41 10 C1 F8 03 5D C3");
    uint32_t doBuffer = 0;
    if ((g_pageSrc || g_splitSrc) && topOk) // 21e: the split screen menu script needs it too
    {
        uint32_t hits[2] = {};
        const int n = FindPattern("55 8B EC 51 53 56 8B 75 08 57 56 E8 ?? ?? ?? ?? 8B D8 8D 45 FC 50 6A 01 56 E8 ?? ?? ?? "
                                  "?? 8B F8 83 C4 10 80 3F 1B 75 0E 68 ?? ?? ?? ?? 56 E8 ?? ?? ?? ?? 83 C4 08 53 6A 00 57 "
                                  "6A 02 56 E8 ?? ?? ?? ?? 50 FF 75 FC 57 56 E8", hits, 2);
        if (n != 1)
            Log("[ctrlpage] the base library's dostring: %d pattern hits (need 1) - the controller page and the split "
                "screen menu are left as the game has them", n);
        else if (TraceCheck("the base library's dostring", hits[0], "55 8B EC 51 53 56 8B 75 08 57 56 E8"))
        {
            doBuffer = TraceCallTarget(hits[0] + 0x48);
            if (!TraceCheck("lua_dobuffer", doBuffer,
                            "55 8B EC FF 75 14 FF 75 10 FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 10 85 C0 75 0E 6A FF 50 FF "
                            "75 08 E8"))
                doBuffer = 0;
        }
    }
    g_luaDoBuffer = doBuffer ? reinterpret_cast<LuaDoBuffer_t>(Rva(doBuffer)) : nullptr;
    if (g_pageSrc)
        Log("[ctrlpage] %s", g_luaDoBuffer ? "ready: the script runs when the game makes ifs_opt_controller and each "
                                             "time it opens; the game's ScriptCB_ResetControl resets the pad too"
                                           : "NOT ready - the controller page is left as the game has it");
    g_luaPushInt = pushInt ? reinterpret_cast<LuaPushInt_t>(Rva(pushInt)) : nullptr; // 21e
    if (g_splitSrc)
        Log("[splitmenu] %s", !g_luaDoBuffer ? "NOT ready - the main menu is left as the game has it"
                              : g_luaPushInt ? "ready: the script runs when the game makes / opens ifs_main and "
                                               "ifs_split_profile; pad 2 is handed to it"
                                             : "ready, but WITHOUT pushnumber: the SPLIT SCREEN button works, pad 2 "
                                               "cannot be handed to the sign-in screen (player 2 cannot join)");
    g_luaGetTop = reinterpret_cast<LuaGetTop_t>(Rva(getTop));
    g_luaIsNumber = reinterpret_cast<LuaIs_t>(Rva(isNum));
    g_luaIsString = reinterpret_cast<LuaIs_t>(Rva(isStr));
    g_luaToNumber = reinterpret_cast<LuaToNumber_t>(Rva(toNum));
    g_luaToString = reinterpret_cast<LuaToString_t>(Rva(toStr));
    g_curPlayer = curPlayer ? reinterpret_cast<CurPlayer_t>(Rva(curPlayer)) : nullptr;
    InterlockedExchange(&g_traceHelpersOk, 1);
    Log("[luacb] helpers OK: arguments and results are logged%s", g_curPlayer ? ", with the current player" : "");
    AudioScanInit(); // 21j-r27
}

// ---- 21i: THE BOT / REINFORCEMENT SCAN (log only, [Splitscreen] BotScanIA=1, [bots] lines) ----
// For Instant Action's AI units per team / reinforcements (1 - 4 players; never the campaign or Galactic Conquest). Every
// map's script sets them itself in its ScriptInit: SetUnitCount(team, n) and SetReinforcementCount(team, n) - the game's
// Lua functions (GOG 1.3 0x00579420 / 0x00579480), wrapped here before the game registers them, as the ScriptCB ones above.
// Each call is logged with its team and number, what the game holds after it (the team manager [0x009ECA20]: units at
// +44h + team * 4; the team at +18h + team * 4, its reinforcements +14h now / +18h start), the rules in force (the campaign:
// AreHistoricalRulesOn's byte 0x008A4DA9, Galactic Conquest: AreMetagameRulesOn's 0x008A4DA8), the map (the mission queue's
// entry) and the local players. Nothing is changed: the numbers go to the game as the map has them.
LuaCFn_t g_botUnitsFn = nullptr;
LuaCFn_t g_botReinfFn = nullptr;
LuaCFn_t g_botHistFn = nullptr;
LuaCFn_t g_botMetaFn = nullptr;
bool g_botResolved = false;
bool g_botUnitsOk = false;  // the unit count setter matches (+44h)
bool g_botReinfOk = false;  // the team getter (+18h) and its reinforcement set (+14h / +18h) match
uintptr_t g_botMgrPtr = 0;  // -> the team manager
volatile const uint8_t* g_botHistByte = nullptr;
volatile const uint8_t* g_botMetaByte = nullptr;
int g_botLines = 0;
constexpr int kBotMaxLines = 400;
constexpr uint32_t kBotUnitsMgrCall = 0x2E;   // SetUnitCount +2Eh: call -> mov eax, [manager]; ret
constexpr uint32_t kBotUnitsSetCall = 0x35;   // +35h: call -> the unit count setter
constexpr uint32_t kBotReinfMgrCall = 0x6B;   // SetReinforcementCount +6Bh: the same getter
constexpr uint32_t kBotReinfTeamCall = 0x72;  // +72h: call -> the team (manager +18h + team * 4)
constexpr uint32_t kBotReinfSetCall = 0x79;   // +79h: call -> the team's reinforcements (+14h now, +18h start)
constexpr uint32_t kBotRulesCall = 0x03;      // AreHistoricalRulesOn / AreMetagameRulesOn +3: call -> mov al, [flag]; ret
const char* const kBotUnitsSetBody = "55 8B EC 8B 45 08 8B 55 0C 89 54 81 44 5D C2 08 00";
const char* const kBotTeamGetBody = "55 8B EC 8B 45 08 8B 44 81 18 5D C2 04 00";
const char* const kBotReinfSetBody = "55 8B EC 8B 45 08 89 41 14 89 41 18 5D C2 04 00";

bool BotBudget()
{
    if (g_botLines >= kBotMaxLines)
        return false;
    ++g_botLines;
    if (g_botLines == kBotMaxLines)
    {
        Log("[bots] line limit reached (%d), going quiet", kBotMaxLines);
        return false;
    }
    return true;
}

uint32_t BotRva(LuaCFn_t fn)
{
    return fn ? static_cast<uint32_t>(reinterpret_cast<uintptr_t>(fn) - ExeBase()) : 0u;
}

// a getter "mov al / eax, [imm32]; ret" (op A0 / A1) -> the address it reads
uintptr_t BotGetterAddr(uint32_t rva, unsigned char op)
{
    if (!rva)
        return 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(rva));
    if (p[0] != op || p[5] != 0xC3)
        return 0;
    uint32_t a = 0;
    memcpy(&a, p + 1, sizeof(a));
    return a;
}

// the first call (the game's code is unlocked by then): the setters and the rule flags, byte-checked
void BotResolve()
{
    g_botResolved = true;
    const uint32_t u = BotRva(g_botUnitsFn);
    const uint32_t r = BotRva(g_botReinfFn);
    const uint32_t mg = u ? TraceCallTarget(u + kBotUnitsMgrCall) : 0;
    const uint32_t us = u ? TraceCallTarget(u + kBotUnitsSetCall) : 0;
    const uint32_t mg2 = r ? TraceCallTarget(r + kBotReinfMgrCall) : 0;
    const uint32_t tg = r ? TraceCallTarget(r + kBotReinfTeamCall) : 0;
    const uint32_t rs = r ? TraceCallTarget(r + kBotReinfSetCall) : 0;
    const uintptr_t mgr = BotGetterAddr(mg, 0xA1);
    const uintptr_t mgr2 = BotGetterAddr(mg2, 0xA1);
    g_botMgrPtr = (mgr && mgr == mgr2) ? mgr : 0;
    g_botUnitsOk = g_botMgrPtr && us && MatchPatternAt(us, kBotUnitsSetBody);
    g_botReinfOk = g_botMgrPtr && tg && rs && MatchPatternAt(tg, kBotTeamGetBody) && MatchPatternAt(rs, kBotReinfSetBody);
    const uint32_t h = BotRva(g_botHistFn);
    const uint32_t m = BotRva(g_botMetaFn);
    g_botHistByte = reinterpret_cast<volatile const uint8_t*>(h ? BotGetterAddr(TraceCallTarget(h + kBotRulesCall), 0xA0) : 0);
    g_botMetaByte = reinterpret_cast<volatile const uint8_t*>(m ? BotGetterAddr(TraceCallTarget(m + kBotRulesCall), 0xA0) : 0);
    Log("[bots] 21i: SetUnitCount rva 0x%08X: the team manager [rva 0x%08X], unit count setter rva 0x%08X %s | SetReinforcementCount "
        "rva 0x%08X: team rva 0x%08X, reinforcement set rva 0x%08X %s | the campaign's rule byte rva 0x%08X, Galactic Conquest's rva "
        "0x%08X", u, static_cast<unsigned>(g_botMgrPtr ? g_botMgrPtr - ExeBase() : 0), us, g_botUnitsOk ? "OK" : "does NOT match", r, tg,
        rs, g_botReinfOk ? "OK" : "does NOT match",
        static_cast<unsigned>(g_botHistByte ? reinterpret_cast<uintptr_t>(g_botHistByte) - ExeBase() : 0),
        static_cast<unsigned>(g_botMetaByte ? reinterpret_cast<uintptr_t>(g_botMetaByte) - ExeBase() : 0));
}

void BotArgs(void* L, int* team, int* n)
{
    *team = -1;
    *n = -1;
    if (!g_traceHelpersOk || !g_luaGetTop || !g_luaIsNumber || !g_luaToNumber || g_luaGetTop(L) < 2)
        return;
    if (g_luaIsNumber(L, 1))
        *team = static_cast<int>(g_luaToNumber(L, 1));
    if (g_luaIsNumber(L, 2))
        *n = static_cast<int>(g_luaToNumber(L, 2));
}

const char* BotMode()
{
    const bool hist = g_botHistByte && *g_botHistByte != 0;
    const bool meta = g_botMetaByte && *g_botMetaByte != 0;
    return meta ? "GALACTIC CONQUEST" : hist ? "CAMPAIGN" : "INSTANT ACTION";
}

void BotLogCall(const char* what, int team, int n, bool units)
{
    if (!BotBudget())
        return;
    char map[40] = {};
    if (!P2SpawnMissionName(map, sizeof(map)))
        snprintf(map, sizeof(map), "?");
    char now[96] = "(not read)";
    const uintptr_t mgr = g_botMgrPtr ? ReadU32(g_botMgrPtr) : 0;
    if (mgr && team >= 0 && team < 8)
    {
        if (units && g_botUnitsOk)
            snprintf(now, sizeof(now), "the game holds %ld", static_cast<long>(static_cast<int32_t>(ReadU32(mgr + 0x44 + team * 4))));
        else if (!units && g_botReinfOk)
        {
            const uintptr_t t = ReadU32(mgr + 0x18 + team * 4);
            if (t)
                snprintf(now, sizeof(now), "the team holds %ld now / %ld start", static_cast<long>(static_cast<int32_t>(ReadU32(t + 0x14))),
                         static_cast<long>(static_cast<int32_t>(ReadU32(t + 0x18))));
        }
    }
    Log("[bots] %lu ms: %s(team %d, %d) - %s | %s, map \"%s\", %d local player(s) (the game's), rules: campaign %d, conquest %d",
        ScanMsSinceAttach(), what, team, n, now, BotMode(), map, P2SpawnGameLocalPlayers(),
        g_botHistByte ? static_cast<int>(*g_botHistByte) : -1, g_botMetaByte ? static_cast<int>(*g_botMetaByte) : -1);
}

// 21i-r2: Instant Action's own numbers - only when both rule bytes were found and are 0 (not the campaign, not Galactic
// Conquest), the setter behind the call matched, and the setting is not empty. Written right after the game's own call, at
// the same places its setters write: units [manager + 44h + team * 4]; reinforcements the team's +14h (now) and +18h (start).
void BotApply(const char* what, int team, int n, bool units)
{
    // 21i-r6: the GAME OPTIONS page's numbers (player 1's profile) with IAGameOptions=1, else the INI's
    const int want = g_iaPageIni ? (units ? g_iaUnits : g_iaReinf) : (units ? g_botUnitsIA : g_botReinfIA);
    const char* src = g_iaPageIni ? (units ? "GAME OPTIONS' AI UNITS PER TEAM" : "GAME OPTIONS' REINFORCEMENTS")
                                  : (units ? "[Splitscreen] AIUnitsIA" : "[Splitscreen] ReinforcementsIA");
    if (want < 0 || team < 0 || team >= 8)
        return;
    const char* skip = nullptr;
    if (!g_botHistByte || !g_botMetaByte)
        skip = "the campaign / Galactic Conquest flags were not found";
    else if (*g_botMetaByte)
        skip = "Galactic Conquest - left as the map has it";
    else if (*g_botHistByte)
        skip = "the campaign - left as the map has it";
    else if (units ? !g_botUnitsOk : !g_botReinfOk)
        skip = "its setter did not match";
    const uintptr_t mgr = (!skip && g_botMgrPtr) ? ReadU32(g_botMgrPtr) : 0;
    if (!skip && !mgr)
        skip = "no team manager yet";
    uintptr_t t = 0;
    if (!skip && !units)
    {
        t = ReadU32(mgr + 0x18 + team * 4);
        if (!t)
            skip = "no such team";
    }
    if (skip)
    {
        if (BotBudget())
            Log("[bots] %lu ms: %s(team %d, %d) - Instant Action's %s %d NOT applied: %s", ScanMsSinceAttach(), what, team, n,
                src, want, skip);
        return;
    }
    // 21i-r3: a team's unit count is its share of the match's characters, and the human players take theirs from it too (r2:
    // AIUnitsIA=0 with 3 players - 0 / 0 units, the players had no room, the game read a unit that was never made and crashed
    // at Steam +0x224198 / GOG 0x0021EB78). Every team keeps room for all the humans (any of them can pick either side).
    int put = want;
    int humans = 1;
    if (units)
    {
        const int game = P2SpawnGameLocalPlayers();
        const int split = (game >= 2) ? PlayerNLocalPlayers() : 0;
        humans = game >= 2 ? (split > game ? split : game) : (game > 0 ? game : 1);
        if (humans > 4)
            humans = 4;
        if (put < humans)
            put = humans;
        *reinterpret_cast<volatile int32_t*>(mgr + 0x44 + team * 4) = put;
    }
    else
    {
        *reinterpret_cast<volatile int32_t*>(t + 0x14) = want;
        *reinterpret_cast<volatile int32_t*>(t + 0x18) = want;
    }
    if (BotBudget())
        Log("[bots] %lu ms: %s(team %d): Instant Action - the map's %d -> %d (%s %d%s)", ScanMsSinceAttach(), what, team, n,
            put, src, want,
            (units && put > want) ? " - raised: every team keeps room for the human players" : "");
    if (units && put > want && BotBudget())
        Log("[bots]   (%d human player(s) this match: a team's units include its players - fewer than that crashed the game in r2)", humans);
}

int __cdecl BotSetUnitCount(void* L)
{
    if (!g_botResolved && g_traceHelpersOk)
        BotResolve();
    int team = -1;
    int n = -1;
    BotArgs(L, &team, &n);
    const int r = g_botUnitsFn(L);
    if (g_botScanIni)
        BotLogCall("SetUnitCount", team, n, true);
    BotApply("SetUnitCount", team, n, true);
    return r;
}

int __cdecl BotSetReinforcementCount(void* L)
{
    if (!g_botResolved && g_traceHelpersOk)
        BotResolve();
    int team = -1;
    int n = -1;
    BotArgs(L, &team, &n);
    const int r = g_botReinfFn(L);
    if (g_botScanIni)
        BotLogCall("SetReinforcementCount", team, n, false);
    BotApply("SetReinforcementCount", team, n, false);
    return r;
}

// 21i-r4: the bleed scan (log only) - AddBleedThreshold(team, reinforcements, rate): below that many reinforcements the team
// bleeds at that rate (the map's thresholds are made for its own reinforcement count - with fewer the team starts below them)
LuaCFn_t g_botBleedFn = nullptr;

int __cdecl BotAddBleedThreshold(void* L)
{
    int team = -1;
    int at = -1;
    float rate = -1.0f;
    if (g_traceHelpersOk && g_luaGetTop && g_luaIsNumber && g_luaToNumber && g_luaGetTop(L) >= 3)
    {
        if (g_luaIsNumber(L, 1))
            team = static_cast<int>(g_luaToNumber(L, 1));
        if (g_luaIsNumber(L, 2))
            at = static_cast<int>(g_luaToNumber(L, 2));
        if (g_luaIsNumber(L, 3))
            rate = g_luaToNumber(L, 3);
    }
    const int r = g_botBleedFn(L);
    if (g_botScanIni && BotBudget())
    {
        char map[40] = {};
        if (!P2SpawnMissionName(map, sizeof(map)))
            snprintf(map, sizeof(map), "?");
        Log("[bots] %lu ms: AddBleedThreshold(team %d, %d, rate %.2f) - %s, map \"%s\" (passed on unchanged)", ScanMsSinceAttach(), team, at,
            static_cast<double>(rate), BotMode(), map);
    }
    return r;
}

// before the game registers its Lua functions (ControllerLuaTraceInstall): the two map functions through BotSet*, the rule
// getters read (not wrapped)
void BotScanInstall(uintptr_t textLo, uintptr_t textHi)
{
    if (!g_botScanIni && !g_iaPageIni && g_botUnitsIA < 0 && g_botReinfIA < 0)
    {
        Log("[bots] 21i: [Splitscreen] BotScanIA=0, IAGameOptions=0, AIUnitsIA / ReinforcementsIA empty - nothing installed (the maps' own numbers)");
        return;
    }
    if (g_iaPageIni)
        Log("[bots] 21i-r6: [Splitscreen] IAGameOptions=1 - Instant Action's numbers come from its GAME OPTIONS page (X on the map list; "
            "saved for player 1's profile in BF1GameExt_IAOptions.ini); AIUnitsIA / ReinforcementsIA not used");
    char u[16] = "the map's";
    char r[16] = "the map's";
    if (g_botUnitsIA >= 0)
        snprintf(u, sizeof(u), "%d", g_botUnitsIA);
    if (g_botReinfIA >= 0)
        snprintf(r, sizeof(r), "%d", g_botReinfIA);
    Log("[bots] 21i-r2: Instant Action (1 - 4 players; never the campaign or Galactic Conquest): AI units per team %s, "
        "reinforcements %s ([Splitscreen] AIUnitsIA / ReinforcementsIA); [bots] scan lines %s", u, r, g_botScanIni ? "on" : "off");
    struct W
    {
        const char* name;
        LuaCFn_t* orig;
        LuaCFn_t wrap;
    };
    const W ws[] = {
        { "SetUnitCount", &g_botUnitsFn, &BotSetUnitCount },
        { "SetReinforcementCount", &g_botReinfFn, &BotSetReinforcementCount },
        { "ScriptCB_AreHistoricalRulesOn", &g_botHistFn, nullptr },
        { "ScriptCB_AreMetagameRulesOn", &g_botMetaFn, nullptr },
        { "AddBleedThreshold", &g_botBleedFn, &BotAddBleedThreshold }, // 21i-r4
    };
    int done = 0;
    for (const W& w : ws)
    {
        const uintptr_t slot = ScanLuaFuncSlot(w.name);
        uint32_t fn = 0;
        if (slot)
            memcpy(&fn, reinterpret_cast<const void*>(slot), sizeof(fn));
        if (!slot || fn < textLo || fn >= textHi)
        {
            Log("[bots] 21i: %s - registration entry %s - not used", w.name, slot ? "holds no game code" : "NOT FOUND");
            continue;
        }
        *w.orig = reinterpret_cast<LuaCFn_t>(static_cast<uintptr_t>(fn));
        if (w.wrap)
        {
            const uint32_t to = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(w.wrap));
            memcpy(reinterpret_cast<void*>(slot), &to, sizeof(to));
        }
        ++done;
    }
    Log("[bots] 21i: %d of 5 found - SetUnitCount / SetReinforcementCount go through BotSet* (the game's own call first, then "
        "Instant Action's numbers if set); the campaign / Galactic Conquest rule getters read; 21i-r4: AddBleedThreshold logged", done);
}

// 21j-r23: THE FREE CAMERA SCAN (log only; [Splitscreen] FreeCamScan=1, [freecam] lines). Player 1's free camera (the pause
// menu's FREE CAMERA - ScriptCB_Freecamera, GOG 1.3 0x0016AB50 -> 0x0016FE20 -> the camera mode switch 0x001E6F30 on
// [0x0043A6EC] + player * 24h: +20h asked, +19h in effect) moves with player 1's own move / look controls (FreeCamera's
// update, 0x0013BEF0: buffer A +0 / +4, the PlayerInput's look +5Ch / +60h) and reads six slots of his interface buffer
// (buffer B, the input manager + 77Ch; a slot counts from 0.125): +B0h up, +B4h down (3 a second, 0x0013CC30), +C0h / +C8h
// move speed x1.25 / x0.8 and +C4h / +CCh turn speed x1.25 / x0.8 (on release, 0x0013CA60). 21j-r24 (corrected): buffer B is
// the interface's 26 inputs (slots 0 - 25, filled by the game's setter 0x001363D0), their processed copy (slots 26 - 51: a
// press, or held for 16 - 19) and last frame's; the free camera reads the copy of interface inputs 18 / 19 / 22 - 25, which
// the game's own tables fill (built at start-up, 0x0002D160 / 0x0002DB50, the same as Battlefront II's): Home / End and the
// pad's RT / LT (18 / 19), = and - and the d-pad's up / down (22 / 24), ] and [ and the d-pad's right / left (23 / 25). This
// logs, while the free camera is on, every change of slots 26 - 51 with the keys and pad buttons held at that moment, and the
// three debug flags its keys toggle (FreeCameraQuakeMode Q, FreeCameraInvertAxis I, FreeCameraStop ' - the exe's key binds;
// 0x00139EB0 / 0x00139E90 / 0x00139ED0). Nothing in the game changes.
const char* const kFcToggleBody =
    "55 8B EC 51 80 7D 08 00 53 56 8B F1 74 0C 80 7E 20 00 0F 94 C3 88 5E 20 EB 03 8A 5E 20 88 5D 08 38 5E 19 0F 84";
const char* const kFcHelperBody = "55 8B EC 8B 45 08 FF 75 0C 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kFcHelperModes = 0x0D; // the A1 operand: the camera mode array's global
constexpr uint32_t kFcHelperCall = 0x14;  // call -> the camera mode switch
const char* const kFcFlagBody = "80 3D ?? ?? ?? ?? 00 0F 94 05 ?? ?? ?? ?? 33 C0 C3"; // a debug toggle: flag = !flag
const char* const kFcMgrSite = "E8 ?? ?? ?? ?? FF 76 08 8B C8 8B 10 8B 42 20 FF D0 8B F8 8B CF E8"; // the up / down handler
const char* const kFcMgrBody = "B8 ?? ?? ?? ?? C3"; // mov eax, <the input manager>; ret
constexpr uint32_t kFcBufB = 0x77C;               // the input manager's buffer B for player 1 (player2_input.cpp)
constexpr uint32_t kFcModeStride = 0x24;
constexpr uint32_t kFcModeOn = 0x19;    // in effect
constexpr uint32_t kFcModeAsked = 0x20; // asked (the toggle)
constexpr int kFcSlots = 52;            // buffer B's current values (the game clears 0xD0 bytes a frame)
constexpr int kFcFirstSlot = 26;        // 0 - 25: the interface's inputs as set; 26 - 51: their processed copy (what is read)
constexpr float kFcOn = 0.125f;         // the game's threshold (0x007D9704)

bool g_fcTried = false;
bool g_fcOk = false;
uintptr_t g_fcModesVa = 0;
uintptr_t g_fcFlagVa[3] = {}; // quake mode, invert axis, stop
uintptr_t g_fcBufB = 0;
uintptr_t g_fcMgr = 0; // 21j-r26: the input manager (player 1's PlayerInput at +1Ch)
bool g_fcOn = false;
bool g_fcSlot[kFcSlots] = {};
uint8_t g_fcFlag[3] = {};
DWORD g_fcOnSince = 0;

bool FcRead(uintptr_t va, void* out, size_t n)
{
    MEMORY_BASIC_INFORMATION mbi = {};
    if (va < 0x10000 || !VirtualQuery(reinterpret_cast<const void*>(va), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) == 0 ||
        (mbi.Protect & PAGE_GUARD) != 0)
        return false;
    const uintptr_t end = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    if (va + n > end)
        return false;
    memcpy(out, reinterpret_cast<const void*>(va), n);
    return true;
}

const char* FcSlotName(int s)
{
    switch (s)
    {
    case 44: return "UP";
    case 45: return "DOWN";
    case 48: return "MOVE FASTER (x1.25 on release)";
    case 49: return "TURN FASTER (x1.25 on release)";
    case 50: return "MOVE SLOWER (x0.8 on release)";
    case 51: return "TURN SLOWER (x0.8 on release)";
    default: return "not read by the free camera";
    }
}

// a held key's name (Windows virtual key)
void FcKeyName(int vk, char* out, size_t n)
{
    if ((vk >= '0' && vk <= '9') || (vk >= 'A' && vk <= 'Z'))
    {
        snprintf(out, n, "%c", vk);
        return;
    }
    if (vk >= VK_F1 && vk <= VK_F24)
    {
        snprintf(out, n, "F%d", vk - VK_F1 + 1);
        return;
    }
    if (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9)
    {
        snprintf(out, n, "Num%d", vk - VK_NUMPAD0);
        return;
    }
    const char* name = nullptr;
    switch (vk)
    {
    case VK_LBUTTON: name = "MouseLeft"; break;
    case VK_RBUTTON: name = "MouseRight"; break;
    case VK_MBUTTON: name = "MouseMiddle"; break;
    case VK_XBUTTON1: name = "Mouse4"; break;
    case VK_XBUTTON2: name = "Mouse5"; break;
    case VK_BACK: name = "Backspace"; break;
    case VK_TAB: name = "Tab"; break;
    case VK_RETURN: name = "Enter"; break;
    case VK_SHIFT: name = "Shift"; break;
    case VK_CONTROL: name = "Ctrl"; break;
    case VK_MENU: name = "Alt"; break;
    case VK_PAUSE: name = "Pause"; break;
    case VK_CAPITAL: name = "CapsLock"; break;
    case VK_ESCAPE: name = "Esc"; break;
    case VK_SPACE: name = "Space"; break;
    case VK_PRIOR: name = "PageUp"; break;
    case VK_NEXT: name = "PageDown"; break;
    case VK_END: name = "End"; break;
    case VK_HOME: name = "Home"; break;
    case VK_LEFT: name = "Left"; break;
    case VK_UP: name = "Up"; break;
    case VK_RIGHT: name = "Right"; break;
    case VK_DOWN: name = "Down"; break;
    case VK_INSERT: name = "Insert"; break;
    case VK_DELETE: name = "Delete"; break;
    case VK_MULTIPLY: name = "Num*"; break;
    case VK_ADD: name = "Num+"; break;
    case VK_SUBTRACT: name = "Num-"; break;
    case VK_DECIMAL: name = "Num."; break;
    case VK_DIVIDE: name = "Num/"; break;
    case VK_LSHIFT: name = "LShift"; break;
    case VK_RSHIFT: name = "RShift"; break;
    case VK_LCONTROL: name = "LCtrl"; break;
    case VK_RCONTROL: name = "RCtrl"; break;
    case VK_LMENU: name = "LAlt"; break;
    case VK_RMENU: name = "RAlt"; break;
    case VK_OEM_1: name = ";"; break;
    case VK_OEM_PLUS: name = "="; break;
    case VK_OEM_COMMA: name = ","; break;
    case VK_OEM_MINUS: name = "-"; break;
    case VK_OEM_PERIOD: name = "."; break;
    case VK_OEM_2: name = "/"; break;
    case VK_OEM_3: name = "`"; break;
    case VK_OEM_4: name = "["; break;
    case VK_OEM_5: name = "\\"; break;
    case VK_OEM_6: name = "]"; break;
    case VK_OEM_7: name = "'"; break;
    default: break;
    }
    if (name)
        snprintf(out, n, "%s", name);
    else
        snprintf(out, n, "vk%02X", vk);
}

// what is held right now: the keyboard / mouse buttons, and every pad that answers (buttons, triggers)
void FcHeld(char* out, size_t n)
{
    size_t used = 0;
    out[0] = 0;
    int keys = 0;
    for (int vk = 0x01; vk <= 0xFE && used + 16 < n; ++vk)
    {
        if (vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU) // the left / right ones say which
            continue;
        if ((GetAsyncKeyState(vk) & 0x8000) == 0)
            continue;
        char name[16];
        FcKeyName(vk, name, sizeof(name));
        used += static_cast<size_t>(snprintf(out + used, n - used, "%s%s", keys ? " + " : "keys ", name));
        ++keys;
    }
    if (!keys && used + 16 < n)
        used += static_cast<size_t>(snprintf(out + used, n - used, "no keys"));
    for (int i = 0; i < 4 && used + 48 < n; ++i)
    {
        XInputState st;
        if (!XRead(i, &st))
            continue;
        used += static_cast<size_t>(snprintf(out + used, n - used, ", pad %d buttons %04X LT %u RT %u", i,
                                             static_cast<unsigned>(st.Gamepad.wButtons),
                                             static_cast<unsigned>(st.Gamepad.bLeftTrigger),
                                             static_cast<unsigned>(st.Gamepad.bRightTrigger)));
    }
}

void FreeCamScanInit()
{
    g_fcTried = true;
    uint32_t hits[16];
    const int nToggle = FindPattern(kFcToggleBody, hits, 16);
    const uint32_t toggle = nToggle == 1 ? hits[0] : 0;
    uint32_t helper = 0;
    const int nHelper = toggle ? FindPattern(kFcHelperBody, hits, 16) : 0;
    for (int i = 0; i < nHelper && i < 16 && !helper; ++i)
        if (TraceCallTarget(hits[i] + kFcHelperCall) == toggle)
            helper = hits[i];
    // the three debug toggles, 20h apart: invert axis, quake mode, stop (each reads and writes the same flag)
    uint32_t flags[3] = {};
    const int nFlag = FindPattern(kFcFlagBody, hits, 16);
    for (int i = 0; i < nFlag && i < 16 && !flags[0]; ++i)
    {
        bool next = false, after = false;
        for (int j = 0; j < nFlag && j < 16; ++j)
        {
            next = next || hits[j] == hits[i] + 0x20;
            after = after || hits[j] == hits[i] + 0x40;
        }
        if (!next || !after)
            continue;
        const uint32_t inv = ReadU32(Rva(hits[i] + 2)), quake = ReadU32(Rva(hits[i] + 0x22)), stop = ReadU32(Rva(hits[i] + 0x42));
        if (inv != ReadU32(Rva(hits[i] + 10)) || quake != ReadU32(Rva(hits[i] + 0x2A)) || stop != ReadU32(Rva(hits[i] + 0x4A)))
            continue;
        if (quake + 1 != inv || quake + 2 != stop) // the game's three flags side by side (0x008A4C30 / 31 / 32 on GOG 1.3)
            continue;
        flags[0] = quake;
        flags[1] = inv;
        flags[2] = stop;
    }
    uint32_t mgrSite = 0;
    const int nMgr = FindPattern(kFcMgrSite, hits, 16);
    if (nMgr == 1)
        mgrSite = hits[0];
    const uint32_t mgrFn = mgrSite ? TraceCallTarget(mgrSite) : 0;
    const uint32_t mgr = mgrFn && MatchPatternAt(mgrFn, kFcMgrBody) ? ReadU32(Rva(mgrFn + 1)) : 0;
    g_fcModesVa = helper ? ReadU32(Rva(helper + kFcHelperModes)) : 0;
    for (int k = 0; k < 3; ++k)
        g_fcFlagVa[k] = flags[k];
    g_fcBufB = mgr ? mgr + kFcBufB : 0;
    g_fcMgr = mgr;
    g_fcOk = toggle && helper && g_fcModesVa && mgr;
    Log("[freecam] 21j-r23 scan %s: the camera mode switch rva 0x%08X (%d hit(s)), ScriptCB_Freecamera's helper rva 0x%08X -> "
        "the mode array at [%08X] (player 1: +19h in effect, +20h asked); the debug flags quake / invert / stop at %08X / "
        "%08X / %08X (%s); the input manager %08X (its handler site %d hit(s)) -> player 1's interface buffer %08X",
        g_fcOk ? "ready" : "NOT ready - no [freecam] state lines", toggle, nToggle, helper,
        static_cast<unsigned>(g_fcModesVa), flags[0], flags[1], flags[2], flags[0] ? "found" : "NOT found - not logged", mgr,
        nMgr, static_cast<unsigned>(g_fcBufB));
}

// game thread, each frame (ControllerBotFrame): player 1's free camera on / off; while on, slots 26 - 51 of his interface
// buffer and the debug flags, with what is held when one changes
void FreeCamScanFrame()
{
    if (!g_freeCamScanIni || g_pauseCamIni <= 0)
        return;
    if (!g_fcTried)
    {
        if (!CodeLooksUnlocked())
            return;
        FreeCamScanInit();
    }
    if (!g_fcOk)
        return;
    uint32_t arr = 0;
    uint8_t mode[kFcModeStride] = {};
    const bool have = FcRead(g_fcModesVa, &arr, sizeof(arr)) && arr && FcRead(arr, mode, sizeof(mode));
    const bool on = have && mode[kFcModeOn] != 0;
    const DWORD now = GetTickCount();
    if (on != g_fcOn)
    {
        g_fcOn = on;
        char held[320];
        FcHeld(held, sizeof(held));
        if (on)
        {
            g_fcOnSince = now;
            memset(g_fcSlot, 0, sizeof(g_fcSlot));
            for (int k = 0; k < 3; ++k)
                if (!g_fcFlagVa[k] || !FcRead(g_fcFlagVa[k], &g_fcFlag[k], 1))
                    g_fcFlag[k] = 0;
            if (FcBudget())
                Log("[freecam] %lu ms: player 1's free camera ON (asked %u) - quake mode %u, invert %u, stop %u | %s",
                    ScanMsSinceAttach(), static_cast<unsigned>(mode[kFcModeAsked]), g_fcFlag[0], g_fcFlag[1], g_fcFlag[2], held);
        }
        else if (FcBudget())
            Log("[freecam] %lu ms: player 1's free camera OFF after %.1f s%s", ScanMsSinceAttach(),
                static_cast<double>(now - g_fcOnSince) / 1000.0, have ? "" : " (no camera modes - the match is gone)");
        return;
    }
    if (!on)
        return;
    for (int k = 0; k < 3; ++k)
    {
        uint8_t f = 0;
        if (!g_fcFlagVa[k] || !FcRead(g_fcFlagVa[k], &f, 1) || f == g_fcFlag[k])
            continue;
        g_fcFlag[k] = f;
        static const char* const kFlag[3] = { "quake mode (FreeCameraQuakeMode, Q)", "invert axis (FreeCameraInvertAxis, I)",
                                              "stop (FreeCameraStop, ')" };
        char held[320];
        FcHeld(held, sizeof(held));
        if (FcBudget())
            Log("[freecam] %lu ms: %s now %s | %s", ScanMsSinceAttach(), kFlag[k], f ? "ON" : "OFF", held);
    }
    float v[kFcSlots] = {};
    if (!g_fcBufB || !FcRead(g_fcBufB, v, sizeof(v)))
        return;
    for (int s = kFcFirstSlot; s < kFcSlots; ++s)
    {
        const bool a = v[s] >= kFcOn;
        if (a == g_fcSlot[s])
            continue;
        g_fcSlot[s] = a;
        if (!a)
            continue;
        char held[320];
        FcHeld(held, sizeof(held));
        if (FcBudget())
            Log("[freecam] %lu ms: interface slot %d (+%03Xh, %s) pressed (%.2f) | %s", ScanMsSinceAttach(), s, s * 4,
                FcSlotName(s), static_cast<double>(v[s]), held);
    }
}

// 21j-r24: THE FAKE CONSOLE'S COMMANDS, SWITCHED BY BF1GAMEEXT ("BFXQ: con NAME" -> 1 on / 0 off / -1 not known; "con! NAME"
// switches it and answers the new value). The game's ScriptCB_DoConsoleCmd (GOG 1.3 0x00175570) runs "debugmenu.NAME" through
// the exe's debug menu (0x0027B600): a variable is switched (a bool: v = !v, 0x0027BD34) and then printed - "NAME = 1" - to
// the exe's debug console ([0x00A973D0], made only by 0x00279450, which nothing in the 2020 exe calls): the print writes to a
// null console (+442Ch) - the crash. Every command in the list is one debug flag: five bool variables (invincible 0x008A4D23,
// unlimitedammo 0x008A4D28, framerate 0x009419F5, rendercollision 0x008A4C33, skipplayer 0x0085EA87) and ToggleHolos, a
// command whose handler (0x000F0910) is flag = !flag on 0x00837EEE. They are found the way ScriptCB_GetConsoleCmds finds them:
// its "debugmenu" lookup (0x0027AD00 - called here with the same name), then that node's entries (count +10Ch, entry i at
// +110h + i * 38h: +0 kind - 0 a variable, 1 a command; +1 the name; +2Ch a command's handler; +30h a variable's type; +34h its
// address). Only a bool variable (type 1) or a flag = !flag command is ever touched, its byte written as the game's own does.
const char* const kConCmdsBody =
    "55 8B EC 51 56 8B 75 08 57 56 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 33 FF 89 45 FC 83 C4 08 39 B8 0C 01 00 00 7E ?? "
    "53 8D 98 11 01 00 00";
constexpr uint32_t kConCmdsName = 0x10; // push "debugmenu"
constexpr uint32_t kConCmdsFind = 0x14; // call the debug menu's lookup
const char* const kConFindBody =
    "55 8B EC 56 8B 35 ?? ?? ?? ?? 85 F6 74 ?? 68 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 74 ?? 50 8D 8E 14 1D 00 00 E8";
constexpr uint32_t kConCount = 0x10C;
constexpr uint32_t kConFirst = 0x110;
constexpr uint32_t kConStride = 0x38;
constexpr uint32_t kConHandler = 0x2C;
constexpr uint32_t kConType = 0x30;
constexpr uint32_t kConAddr = 0x34;
constexpr int kConMax = 0x80;     // the game's own limit
constexpr int kConNameMax = 0x22; // +1 .. +23h
constexpr int kConKeep = 24;

struct ConFlag
{
    char name[kConNameMax + 1];
    uintptr_t flag;
    bool command;
};
bool g_conTried = false;
int g_conCount = 0;
ConFlag g_con[kConKeep] = {};

bool ConWritable(uintptr_t va)
{
    MEMORY_BASIC_INFORMATION mbi = {};
    return va >= 0x10000 && VirtualQuery(reinterpret_cast<const void*>(va), &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT &&
           (mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) != 0 && (mbi.Protect & PAGE_GUARD) == 0;
}

// a command's handler: flag = !flag on one byte (80 3D A 00 0F 94 05 A 33 C0 C3) -> that byte, else 0
uintptr_t ConHandlerFlag(uint32_t handlerVa)
{
    uint32_t codeRva = 0, codeSize = 0;
    if (!CodeSection(&codeRva, &codeSize) || handlerVa < ExeBase())
        return 0;
    const uint32_t rva = static_cast<uint32_t>(handlerVa - ExeBase());
    if (rva < codeRva || rva >= codeRva + codeSize)
        return 0;
    const uint32_t body = TraceFollowThunks(rva);
    if (body < codeRva || body + 0x20 >= codeRva + codeSize || !MatchPatternAt(body, kFcFlagBody))
        return 0;
    const uint32_t a = ReadU32(Rva(body + 2)), b = ReadU32(Rva(body + 10));
    return a == b ? a : 0;
}

void FakeConsoleInit()
{
    g_conTried = true;
    uint32_t hits[4];
    const int n = FindPattern(kConCmdsBody, hits, 4);
    const uint32_t fn = n == 1 ? hits[0] : 0;
    const uintptr_t nameVa = fn ? ReadU32(Rva(fn + kConCmdsName)) : 0;
    char name[12] = {};
    const bool nameOk = nameVa && FcRead(nameVa, name, 10) && memcmp(name, "debugmenu", 10) == 0;
    const uint32_t find = fn ? TraceCallTarget(fn + kConCmdsFind) : 0;
    const bool findOk = find && MatchPatternAt(find, kConFindBody);
    uintptr_t node = 0;
    if (nameOk && findOk)
    {
        char path[12] = {};
        memcpy(path, "debugmenu", 10); // its own copy: the lookup splits the name at '.' in place
        using FindFn = uintptr_t(__cdecl*)(char*);
        node = reinterpret_cast<FindFn>(Rva(find))(path);
    }
    int32_t count = -1;
    if (!node || !FcRead(node + kConCount, &count, sizeof(count)) || count < 0 || count > kConMax)
        count = -1;
    char list[900];
    size_t used = 0;
    list[0] = 0;
    for (int i = 0; i < count; ++i)
    {
        uint8_t e[kConStride];
        if (!FcRead(node + kConFirst + static_cast<uintptr_t>(i) * kConStride, e, sizeof(e)))
            break;
        ConFlag c = {};
        memcpy(c.name, e + 1, kConNameMax);
        c.name[kConNameMax] = 0;
        bool printable = c.name[0] != 0;
        for (const char* p = c.name; *p && printable; ++p)
            printable = *p > 0x20 && *p < 0x7F;
        c.command = e[0] != 0;
        uint32_t type = 0, addr = 0, handler = 0;
        memcpy(&type, e + kConType, sizeof(type));
        memcpy(&addr, e + kConAddr, sizeof(addr));
        memcpy(&handler, e + kConHandler, sizeof(handler));
        if (printable)
            c.flag = c.command ? ConHandlerFlag(handler) : (type == 1 ? addr : 0);
        if (c.flag && !ConWritable(c.flag))
            c.flag = 0;
        uint8_t v = 0;
        const bool have = c.flag && FcRead(c.flag, &v, 1);
        if (used + 80 < sizeof(list))
            used += static_cast<size_t>(snprintf(list + used, sizeof(list) - used, "%s%s (%s, %s%08X = %u)", i ? ", " : "",
                                                 printable ? c.name : "?", c.command ? "command" : "variable",
                                                 have ? "flag " : "NOT a flag - left alone, ",
                                                 static_cast<unsigned>(have ? c.flag : (c.command ? handler : type)),
                                                 static_cast<unsigned>(v)));
        if (have && g_conCount < kConKeep)
            g_con[g_conCount++] = c;
    }
    if (FcBudget())
        Log("[freecam] 21j-r24 fake console %s: ScriptCB_GetConsoleCmds rva 0x%08X (%d hit(s)), its \"debugmenu\" %s, the debug "
            "menu's lookup rva 0x%08X (%s) -> the debugmenu node %08X: %d entr%s - %s",
            g_conCount ? "ready" : "NOT ready - its commands are not run", fn, n, nameOk ? "found" : "NOT found", find,
            findOk ? "ok" : "does NOT match", static_cast<unsigned>(node), count, count == 1 ? "y" : "ies", list[0] ? list : "-");
}

// "con NAME" (toggle false) / "con! NAME" (toggle true); the game thread (a Lua call)
int FakeConsoleQuery(const char* name, bool toggle)
{
    if (g_pauseConIni <= 0 || !name)
        return -1;
    if (!g_conTried)
    {
        if (!CodeLooksUnlocked())
            return -1;
        FakeConsoleInit();
    }
    for (int i = 0; i < g_conCount; ++i)
    {
        if (strcmp(g_con[i].name, name) != 0)
            continue;
        if (!ConWritable(g_con[i].flag))
            return -1;
        volatile uint8_t* f = reinterpret_cast<volatile uint8_t*>(g_con[i].flag);
        if (!toggle)
            return *f ? 1 : 0;
        const uint8_t was = *f;
        *f = was ? 0 : 1; // as the game's own (sete): v = !v
        const uint8_t now = *f;
        if (FcBudget())
            Log("[freecam] %lu ms: fake console: %s %s -> %s (flag %08X, switched by BF1GameExt)", ScanMsSinceAttach(),
                g_con[i].name, was ? "ON" : "OFF", now ? "ON" : "OFF", static_cast<unsigned>(g_con[i].flag));
        return now ? 1 : 0;
    }
    if (toggle && FcBudget())
        Log("[freecam] %lu ms: fake console: \"%.40s\" is not a debug flag BF1GameExt knows - NOT run (the game's own would "
            "crash)", ScanMsSinceAttach(), name);
    return -1;
}

// 21j-r25: START / ESC IN PLAYER 1'S FREE CAMERA OPEN HIS PAUSE MENU (its CHASE CAMERA puts the camera back on his soldier).
// The game's own Start did not pause while the free camera was on (the r24 test). Why (GOG 1.3, static): the offline pause
// freezes the world (SetPaused(1) sets the freeze flag 0x008A4D2A, SetPaused(0) clears it); FREE CAMERA picked in the pause
// menu turns the pause off by hand (the camera mode switch 0x001E6F30 clears PauseState +18h itself) and leaves the world
// frozen - the console's photo camera - and PauseState::Update (0x001E6B30) never pauses while that flag is set. In the free
// camera the game's BACK (pad) / Backspace (interface input 3, Update +192h on) switches the world frozen / running, and only
// while it runs does Start pause. Now: each frame (ControllerBotFrame), while player 1's free camera is on (his PauseState -
// the camera mode array - +19h), a new press of Start on his pad (the game's pad object) or of Esc (the game's window in
// front), the game not paused then or the frame before: if the game has not paused by the next frame, BF1GameExt pauses it as
// the game's Start does (p2spawn_scan.cpp P2SpawnPauseP1: SetPaused(1)). RESUME GAME (SetPaused(0)) lets the world run; CHASE
// CAMERA (the switch's off path) unfreezes it too. The log line has the pause state and the conditions Update checks: the demo
// flag (+77h), the world freeze (+8Dh), the game-over flag (+9Ah), the game interface object's state (+18h: 2 = no pause) and
// busy byte (+237Eh), and interface input 2 (the one Esc and Start set, which Update reads) as set / read / last frame.
// [Splitscreen] PauseFreeCam > 0. Nothing changes while the free camera is off.
const char* const kFcpUpdateBody =
    "55 8B EC 56 8B F1 57 80 7E 18 00 74 13 F3 0F 10 45 08 51 F3 0F 11 04 24 E8 ?? ?? ?? ?? 83 C4 04 53 E8 ?? ?? ?? ?? FF 76 1C "
    "8B F8 8B C8 8B 17 FF 52 1C";
const uint32_t kFcpFlagCalls[3] = { 0x77, 0x8D, 0x9A }; // -> mov al, [flag]; ret (demo, the world freeze, game over)
constexpr uint32_t kFcpIfaceCall = 0x44;                 // -> the game interface object's getter
const char* const kFcpFlagGet = "A0 ?? ?? ?? ?? C3";
const char* const kFcpIfaceGet = "55 8B EC 83 7D 08 FF 75 0A 6A 00 E8 ?? ?? ?? ?? 83 C4 04 A1 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kFcpIfaceGlobal = 0x14;
constexpr uint32_t kFcpIfaceState = 0x18;
constexpr uint32_t kFcpIfaceBusy = 0x237E;
constexpr uint32_t kFcpPaused = 0x18; // PauseState: paused (byte)
constexpr int kFcpMaxLines = 30;

bool g_fcpTried = false;
uintptr_t g_fcpFlag[3] = {};
uintptr_t g_fcpIface = 0; // -> the game interface object
bool g_fcpPadWas = false;
bool g_fcpEscWas = false;
bool g_fcpPausedPrev = false;
bool g_fcpPending = false;
const char* g_fcpHow = "";
char g_fcpAtPress[300] = {};
int g_fcpLines = 0;

void FcpInit()
{
    g_fcpTried = true;
    uint32_t hits[4];
    const int n = FindPattern(kFcpUpdateBody, hits, 4);
    const uint32_t upd = n == 1 ? hits[0] : 0;
    for (int k = 0; k < 3 && upd; ++k)
    {
        const uint32_t t = TraceCallTarget(upd + kFcpFlagCalls[k]);
        g_fcpFlag[k] = t && MatchPatternAt(t, kFcpFlagGet) ? ReadU32(Rva(t + 1)) : 0;
    }
    const uint32_t ig = upd ? TraceCallTarget(upd + kFcpIfaceCall) : 0;
    g_fcpIface = ig && MatchPatternAt(ig, kFcpIfaceGet) ? ReadU32(Rva(ig + kFcpIfaceGlobal)) : 0;
    if (FcBudget())
        Log("[freecam] 21j-r25 Start / Esc in the free camera: player 1's PauseState from the camera mode array [%08X]; the game's "
            "PauseState::Update rva 0x%08X (%d hit(s)) - the demo flag %08X, the world freeze %08X, the game-over flag %08X, the game "
            "interface object [%08X] (logged at each press - why the game's own Start did not pause); SetPaused through "
            "p2spawn_scan.cpp", static_cast<unsigned>(g_fcModesVa), upd, n, static_cast<unsigned>(g_fcpFlag[0]),
            static_cast<unsigned>(g_fcpFlag[1]), static_cast<unsigned>(g_fcpFlag[2]), static_cast<unsigned>(g_fcpIface));
}

int FcpByte(uintptr_t va)
{
    uint8_t v = 0;
    return va && FcRead(va, &v, 1) ? v : -1;
}

float FcpSlot(int s)
{
    float v = -1.0f;
    if (!g_fcBufB || !FcRead(g_fcBufB + static_cast<uintptr_t>(s) * 4, &v, sizeof(v)))
        return -1.0f;
    return v;
}

// the pause state and the game's pause conditions, for the log
void FcpState(char* out, size_t n, const uint8_t* ps)
{
    uint32_t iface = 0;
    const bool haveIface = g_fcpIface && FcRead(g_fcpIface, &iface, sizeof(iface)) && iface;
    int32_t ifState = -1;
    if (haveIface)
        FcRead(iface + kFcpIfaceState, &ifState, sizeof(ifState));
    snprintf(out, n,
             "paused %u, free camera %u (asked %u), +21h %u, +22h %u; demo %d, world frozen %d, game over %d, interface state %ld busy %d; "
             "interface input 2 (Esc / Start) set %.2f read %.2f last frame %.2f",
             static_cast<unsigned>(ps[kFcpPaused]), static_cast<unsigned>(ps[kFcModeOn]), static_cast<unsigned>(ps[kFcModeAsked]),
             static_cast<unsigned>(ps[0x21]), static_cast<unsigned>(ps[0x22]), FcpByte(g_fcpFlag[0]), FcpByte(g_fcpFlag[1]),
             FcpByte(g_fcpFlag[2]), static_cast<long>(ifState), haveIface ? FcpByte(iface + kFcpIfaceBusy) : -1,
             static_cast<double>(FcpSlot(2)), static_cast<double>(FcpSlot(26 + 2)), static_cast<double>(FcpSlot(52 + 2)));
}

void FreeCamPauseFrame()
{
    if (g_pauseCamIni <= 0)
        return;
    if (!g_fcTried)
    {
        if (!CodeLooksUnlocked())
            return;
        FreeCamScanInit();
    }
    if (!g_fcOk)
        return;
    if (!g_fcpTried)
        FcpInit();
    uint32_t arr = 0;
    uint8_t ps[kFcModeStride] = {};
    if (!FcRead(g_fcModesVa, &arr, sizeof(arr)) || !arr || !FcRead(arr, ps, sizeof(ps)))
    {
        g_fcpPending = false;
        return;
    }
    const bool on = ps[kFcModeOn] != 0;
    const bool paused = ps[kFcpPaused] != 0;
    bool pad = false;
    const uintptr_t gp = GamePadAddress();
    if (gp && (PadButtons(reinterpret_cast<const void*>(gp)) & 0x0010) != 0) // START
        pad = true;
    bool esc = false;
    const HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (fg)
        GetWindowThreadProcessId(fg, &pid);
    if (pid == GetCurrentProcessId() && (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0)
        esc = true;
    const bool padPress = pad && !g_fcpPadWas;
    const bool escPress = esc && !g_fcpEscWas;
    g_fcpPadWas = pad;
    g_fcpEscWas = esc;
    const bool pausedPrev = g_fcpPausedPrev;
    g_fcpPausedPrev = paused;
    if (!on)
    {
        g_fcpPending = false;
        return;
    }
    if (g_fcpPending)
    {
        g_fcpPending = false;
        char now[300];
        FcpState(now, sizeof(now), ps);
        if (paused)
        {
            if (g_fcpLines < kFcpMaxLines && FcBudget())
            {
                ++g_fcpLines;
                Log("[freecam] %lu ms: %s in the free camera - the game paused by itself | at the press: %s", ScanMsSinceAttach(),
                    g_fcpHow, g_fcpAtPress);
            }
            return;
        }
        const int r = P2SpawnPauseP1();
        if (g_fcpLines < kFcpMaxLines && FcBudget())
        {
            ++g_fcpLines;
            Log("[freecam] %lu ms: %s in the free camera - the game did NOT pause (at the press: %s | a frame later: %s) - %s",
                ScanMsSinceAttach(), g_fcpHow, g_fcpAtPress, now,
                r == 1   ? "BF1GameExt paused it as the game's Start does (SetPaused): the pause menu, CHASE CAMERA in it"
                : r == 0 ? "it is paused already"
                         : "BF1GameExt could NOT pause it (the pause states / SetPaused not found - [Splitscreen] PauseScan34=1 "
                           "needed)");
        }
        return;
    }
    if ((padPress || escPress) && !paused && !pausedPrev)
    {
        g_fcpPending = true;
        g_fcpHow = padPress ? "Start on player 1's pad" : "Esc";
        FcpState(g_fcpAtPress, sizeof(g_fcpAtPress), ps);
    }
}

// 21j-r26: THE FREE CAMERA ON PLAYER 1'S PAD ([Splitscreen] FreeCamPad=1): R3 up, L3 down (the game's RT / LT no longer move
// it), the right stick looks (the game's free camera turns with the mouse only - PlayerInput +5Ch / +60h, the mouse's turn
// rates), X stops / starts the camera (the game's FreeCameraStop flag, its ' key: the camera holds still where it is; cleared
// each time the free camera comes on). Through the FreeCamera's update, its vtable's slot 1 (GOG 1.3 0x0013BEF0; the slot is
// pointed at FcUpdWrap, which calls the game's update unchanged): for player 1's camera only ([camera +8] = 0), right before
// the game's update reads them, his interface inputs 18 / 19 (buffer B +B0h / +B4h: up / down - R3 / L3 held, or the
// keyboard's Home / End as the game set them; RT / LT's dropped) and his PlayerInput's turn rates (plus the right stick: a 24 %
// round dead zone, squared, full = 150 degrees a second at the turn speed the d-pad sets) are set; after it they are put back
// as they were, so nothing else in the frame sees them. The keyboard and mouse work as before. 0 = the game's own pad controls.
const char* const kFcCamCtor =
    "68 ?? ?? ?? ?? 8D 4E 64 C7 06 ?? ?? ?? ?? C7 46 50 00 00 00 00 C7 46 54 00 00 00 00 C7 46 58 00 00 80 3F C7 46 5C 00 00 80 3F "
    "66 C7 46 60 00 00";
constexpr uint32_t kFcCamCtorVt = 0x0A;
const char* const kFcCamUpd =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ?? ?? 00 00 56 57 8B F9 89 7D ?? E8 ?? ?? ?? ?? D9 5B 08 "
    "E8";
constexpr uint32_t kFcCamUpdUpDown = 0xAD; // call -> the up / down handler (the free camera scan's manager site is at its +42h)
constexpr uint32_t kFcMgrSiteInHandler = 0x42;
constexpr uint32_t kFcCamPlayer = 0x08;
constexpr uint32_t kFcPiTurnX = 0x1C + 0x5C; // the input manager + 1Ch = player 1's PlayerInput; +5Ch / +60h its turn rates
constexpr uint32_t kFcPiTurnY = 0x1C + 0x60;
constexpr int kFcSlotUp = 44;   // buffer B +B0h: interface input 18 as read
constexpr int kFcSlotDown = 45; // +B4h: 19
constexpr float kFcLookRate = 1.745f; // the game turns 1.5 x this a second at full stick: 2.62 rad = 150 degrees
constexpr float kFcLookDead = 0.24f;
constexpr WORD kXL3 = 0x0040;
constexpr WORD kXR3 = 0x0080;
constexpr WORD kXX = 0x4000;
constexpr int kFcPadMaxLines = 40;

using FcUpd_t = void(__fastcall*)(void* self, void* edx, float dt);
bool g_fcPadTried = false;
volatile uintptr_t g_fcUpdOrig = 0; // the game's update (what the slot held)
bool g_fcXWas = false;
bool g_fcPadOnWas = false;
bool g_fcPadUsed = false;
int g_fcPadLines = 0;

bool FcPadBudget()
{
    if (g_fcPadLines >= kFcPadMaxLines || !FcBudget())
        return false;
    ++g_fcPadLines;
    return true;
}

// the right stick, a round dead zone, squared: -1 .. 1 each way
void FcStick(SHORT sx, SHORT sy, float* x, float* y)
{
    const float fx = static_cast<float>(sx) / 32767.0f;
    const float fy = static_cast<float>(sy) / 32767.0f;
    const float mag = sqrtf(fx * fx + fy * fy);
    *x = 0.0f;
    *y = 0.0f;
    if (mag <= kFcLookDead)
        return;
    float m = (mag - kFcLookDead) / (1.0f - kFcLookDead);
    if (m > 1.0f)
        m = 1.0f;
    const float k = m * m / mag;
    *x = fx * k;
    *y = fy * k;
}

struct FcPadSaved
{
    float up;
    float down;
    float turnX;
    float turnY;
};

// before the game's update reads them: player 1's up / down and turn rates from his pad (false = nothing set)
bool FcPadBefore(FcPadSaved* sv)
{
    const int idx = GamePadXInputIndex();
    XInputState st;
    if (idx < 0 || !XRead(idx, &st))
        return false;
    volatile float* const b = reinterpret_cast<volatile float*>(g_fcBufB);
    volatile float* const tx = reinterpret_cast<volatile float*>(g_fcMgr + kFcPiTurnX);
    volatile float* const ty = reinterpret_cast<volatile float*>(g_fcMgr + kFcPiTurnY);
    sv->up = b[kFcSlotUp];
    sv->down = b[kFcSlotDown];
    sv->turnX = *tx;
    sv->turnY = *ty;
    const WORD w = st.Gamepad.wButtons;
    // X: the camera stops / moves again (a new press)
    const bool x = (w & kXX) != 0;
    if (x && !g_fcXWas && g_fcFlagVa[2])
    {
        volatile uint8_t* const stop = reinterpret_cast<volatile uint8_t*>(g_fcFlagVa[2]);
        *stop = *stop ? 0 : 1;
        if (FcPadBudget())
            Log("[freecam] %lu ms: X on player 1's pad - the free camera %s (FreeCameraStop %08X = %u)", ScanMsSinceAttach(),
                *stop ? "STOPPED (it holds still where it is; X again to move it)" : "moves again",
                static_cast<unsigned>(g_fcFlagVa[2]), static_cast<unsigned>(*stop));
    }
    g_fcXWas = x;
    // up / down: R3 / L3 held, or what the keyboard (Home / End) set; RT / LT's own dropped
    bool home = false;
    bool end = false;
    const HWND fg = GetForegroundWindow();
    DWORD pid = 0;
    if (fg)
        GetWindowThreadProcessId(fg, &pid);
    if (pid == GetCurrentProcessId())
    {
        home = (GetAsyncKeyState(VK_HOME) & 0x8000) != 0;
        end = (GetAsyncKeyState(VK_END) & 0x8000) != 0;
    }
    const bool rt = st.Gamepad.bRightTrigger > 127;
    const bool lt = st.Gamepad.bLeftTrigger > 127;
    const bool keyUp = sv->up >= kFcOn && (home || !rt);
    const bool keyDown = sv->down >= kFcOn && (end || !lt);
    b[kFcSlotUp] = keyUp ? sv->up : ((w & kXR3) != 0 ? 1.0f : 0.0f);
    b[kFcSlotDown] = keyDown ? sv->down : ((w & kXL3) != 0 ? 1.0f : 0.0f);
    // the right stick looks, as the mouse does: +5Ch is the mouse's x (right = right), +60h its y negated by the game
    // (PlayerInput::Update 0x00063D5D: neg - the mouse moved away = up = positive), so the stick goes in as it is
    float sx = 0.0f;
    float sy = 0.0f;
    FcStick(st.Gamepad.sThumbRX, st.Gamepad.sThumbRY, &sx, &sy);
    *tx = sv->turnX + sx * kFcLookRate;
    *ty = sv->turnY + sy * kFcLookRate;
    if (!g_fcPadUsed && (sx != 0.0f || sy != 0.0f || (w & (kXR3 | kXL3)) != 0) && FcPadBudget())
    {
        g_fcPadUsed = true;
        Log("[freecam] %lu ms: player 1's pad drives the free camera (XInput %d): right stick %.2f / %.2f, R3 %u, L3 %u, RT %u, LT %u "
            "-> up %.2f, down %.2f, turn rates %.3f / %.3f (the mouse's %.3f / %.3f)", ScanMsSinceAttach(), idx,
            static_cast<double>(sx), static_cast<double>(sy), (w & kXR3) ? 1u : 0u, (w & kXL3) ? 1u : 0u, rt ? 1u : 0u,
            lt ? 1u : 0u, static_cast<double>(b[kFcSlotUp]), static_cast<double>(b[kFcSlotDown]), static_cast<double>(*tx),
            static_cast<double>(*ty), static_cast<double>(sv->turnX), static_cast<double>(sv->turnY));
    }
    return true;
}

// FreeCamera's update (its vtable slot 1): the game's, with player 1's pad set around it
void __fastcall FcUpdWrap(void* self, void* edx, float dt)
{
    const uintptr_t cam = reinterpret_cast<uintptr_t>(self);
    FcPadSaved sv = {};
    const bool did = g_freeCamPadIni && cam && g_fcBufB && g_fcMgr &&
                     *reinterpret_cast<volatile const int32_t*>(cam + kFcCamPlayer) == 0 && FcPadBefore(&sv);
    reinterpret_cast<FcUpd_t>(g_fcUpdOrig)(self, edx, dt);
    if (did)
    {
        volatile float* const b = reinterpret_cast<volatile float*>(g_fcBufB);
        b[kFcSlotUp] = sv.up;
        b[kFcSlotDown] = sv.down;
        *reinterpret_cast<volatile float*>(g_fcMgr + kFcPiTurnX) = sv.turnX;
        *reinterpret_cast<volatile float*>(g_fcMgr + kFcPiTurnY) = sv.turnY;
    }
}

void FreeCamPadInstall()
{
    g_fcPadTried = true;
    uint32_t hits[4];
    const int n = FindPattern(kFcCamCtor, hits, 4);
    const uintptr_t vt = n == 1 ? ReadU32(Rva(hits[0] + kFcCamCtorVt)) : 0;
    uint32_t slotFn = 0;
    uint32_t upd = 0;
    bool updOk = false;
    bool linkOk = false;
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (vt && FcRead(vt + 4, &slotFn, sizeof(slotFn)) && CodeSection(&codeRva, &codeSize) && slotFn >= ExeBase() &&
        slotFn - ExeBase() >= codeRva && slotFn - ExeBase() < codeRva + codeSize)
    {
        upd = TraceFollowThunks(static_cast<uint32_t>(slotFn - ExeBase()));
        updOk = upd >= codeRva && upd + 0x100 < codeRva + codeSize && MatchPatternAt(upd, kFcCamUpd);
        uint32_t mh[2];
        const int nm = updOk ? FindPattern(kFcMgrSite, mh, 2) : 0;
        linkOk = nm == 1 && TraceCallTarget(upd + kFcCamUpdUpDown) == mh[0] - kFcMgrSiteInHandler;
    }
    bool swapped = false;
    if (updOk && linkOk && g_fcBufB && g_fcMgr)
    {
        DWORD old = 0;
        uint32_t* const slot = reinterpret_cast<uint32_t*>(vt + 4);
        if (VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &old))
        {
            g_fcUpdOrig = slotFn;
            *slot = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&FcUpdWrap));
            DWORD tmp = 0;
            VirtualProtect(slot, sizeof(*slot), old, &tmp);
            FlushInstructionCache(GetCurrentProcess(), slot, sizeof(*slot));
            swapped = true;
        }
    }
    if (FcPadBudget())
        Log("[freecam] 21j-r26 the free camera on player 1's pad %s: FreeCamera's constructor %d hit(s) -> its vtable %08X, slot 1 "
            "%08X -> the update rva 0x%08X (%s; its up / down call %s); player 1's PlayerInput turn rates %08X / %08X, his interface "
            "inputs 18 / 19 at %08X / %08X, FreeCameraStop %08X",
            swapped ? "ON (R3 up, L3 down, the right stick looks, X stops / starts it)" : "NOT installed - the game's own pad controls",
            n, static_cast<unsigned>(vt), slotFn, upd, updOk ? "matches" : "does NOT match", linkOk ? "ok" : "NOT the one expected",
            static_cast<unsigned>(g_fcMgr + kFcPiTurnX), static_cast<unsigned>(g_fcMgr + kFcPiTurnY),
            static_cast<unsigned>(g_fcBufB + kFcSlotUp * 4), static_cast<unsigned>(g_fcBufB + kFcSlotDown * 4),
            static_cast<unsigned>(g_fcFlagVa[2]));
}

// each frame (ControllerBotFrame): the install, and the camera stop cleared each time player 1's free camera comes on
void FreeCamPadFrame()
{
    if (!g_freeCamPadIni || g_pauseCamIni <= 0)
        return;
    if (!g_fcTried)
    {
        if (!CodeLooksUnlocked())
            return;
        FreeCamScanInit();
    }
    if (!g_fcOk)
        return;
    if (!g_fcPadTried)
        FreeCamPadInstall();
    uint32_t arr = 0;
    uint8_t ps[kFcModeStride] = {};
    const bool on = FcRead(g_fcModesVa, &arr, sizeof(arr)) && arr && FcRead(arr, ps, sizeof(ps)) && ps[kFcModeOn] != 0;
    if (on && !g_fcPadOnWas && g_fcFlagVa[2])
    {
        volatile uint8_t* const stop = reinterpret_cast<volatile uint8_t*>(g_fcFlagVa[2]);
        if (*stop)
        {
            *stop = 0;
            if (FcPadBudget())
                Log("[freecam] %lu ms: player 1's free camera on - its stop (X) cleared: it moves", ScanMsSinceAttach());
        }
        g_fcXWas = true; // the press that may still be held is not a new one
    }
    g_fcPadOnWas = on;
}

} // namespace

// 21j-r27: the audio scanner's frame (log only) - the game thread, each frame (p2spawn_scan.cpp P2SpawnMenuLayer)
void ControllerAudioFrame()
{
    AudioScanFrame();
}

// 21i-r4: the bleed scan's other half (log only): each team's reinforcements (now / start) every 5 s while they change, in
// any match, so the drain can be seen against the thresholds. The game thread, each frame (p2spawn_scan.cpp EndScanFrame).
void ControllerBotFrame()
{
    FreeCamScanFrame(); // 21j-r23 (log only)
    FreeCamPauseFrame(); // 21j-r25: Start / Esc in player 1's free camera
    FreeCamPadFrame();   // 21j-r26: the free camera on player 1's pad (R3 / L3, the right stick, X)
    static DWORD s_last = 0;
    static int32_t s_was[2] = { -1, -1 };
    static int s_quiet = 0;
    if (!g_botScanIni || !g_botReinfOk || !g_botMgrPtr)
        return;
    const DWORD now = GetTickCount();
    if (now - s_last < 5000)
        return;
    s_last = now;
    const uintptr_t mgr = ReadU32(g_botMgrPtr);
    if (!mgr)
        return;
    int32_t cur[2] = { -1, -1 };
    int32_t start[2] = { -1, -1 };
    for (int t = 0; t < 2; ++t)
    {
        const uintptr_t team = ReadU32(mgr + 0x18 + (t + 1) * 4);
        MEMORY_BASIC_INFORMATION mbi = {};
        if (!team || !VirtualQuery(reinterpret_cast<const void*>(team), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) == 0)
            return;
        cur[t] = static_cast<int32_t>(ReadU32(team + 0x14));
        start[t] = static_cast<int32_t>(ReadU32(team + 0x18));
        if (cur[t] < -10 || cur[t] > 100000 || start[t] < 0 || start[t] > 100000)
            return;
    }
    if (cur[0] == s_was[0] && cur[1] == s_was[1])
    {
        if (++s_quiet != 6) // one "still" line after 30 s without a change
            return;
    }
    else
        s_quiet = 0;
    s_was[0] = cur[0];
    s_was[1] = cur[1];
    if (BotBudget())
        Log("[bots] %lu ms: reinforcements: team 1 %ld / %ld, team 2 %ld / %ld%s", ScanMsSinceAttach(), static_cast<long>(cur[0]),
            static_cast<long>(start[0]), static_cast<long>(cur[1]), static_cast<long>(start[1]), s_quiet ? " (no change for 30 s)" : "");
}

void ControllerLuaTraceInstall()
{
    if (!CurrentBuild())
    {
        Log("[luacb] NOT installed - unknown exe");
        return;
    }
    if (!LuaTraceIniOn())
    {
        Log("[luacb] BF1GameExt.ini [Controls] LuaTrace=0: the controller page scan, the changeable controller page "
            "and the split screen menu (21e) are not installed");
        return;
    }
    PageLoad(); // 21d
    SplitLoad(); // 21e
    g_audioScanIni = AudioScanIniOn(); // 21j-r27
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize))
    {
        Log("[luacb] NOT installed - no code section");
        return;
    }
    g_traceStubs = static_cast<unsigned char*>(VirtualAlloc(nullptr, kTraceCount * kTraceStubBytes,
                                                            MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_traceStubs)
    {
        Log("[luacb] NOT installed - VirtualAlloc failed (error %lu)", GetLastError());
        return;
    }
    memset(g_traceStubs, 0xCC, kTraceCount * kTraceStubBytes);
    const uintptr_t textLo = Rva(textRva);
    const uintptr_t textHi = textLo + textSize;
    int wrapped = 0;
    for (int i = 0; i < kTraceCount; ++i)
    {
        for (const char* quiet : kTraceQuietInSplit) // 21e
            if (strcmp(kTraceNames[i], quiet) == 0)
                g_tf[i].quiet = true;
        g_tf[i].audio = i >= kTraceCount - kTraceAudioCount; // 21j-r27
        if (g_tf[i].audio && !g_audioScanIni)
            continue; // (AudioScan=0: not wrapped)
        const uintptr_t slot = ScanLuaFuncSlot(kTraceNames[i]);
        if (!slot)
        {
            Log("[luacb] %-36s registration entry NOT FOUND - not wrapped", kTraceNames[i]);
            continue;
        }
        uint32_t fn = 0;
        memcpy(&fn, reinterpret_cast<const void*>(slot), sizeof(fn));
        if (fn < textLo || fn >= textHi)
        {
            Log("[luacb] %-36s entry holds 0x%08X, not the game's code - left alone", kTraceNames[i],
                static_cast<unsigned>(fn));
            continue;
        }
        // mov eax, [esp+4]; push eax; push i; call LuaTraceCall; add esp, 8; ret
        unsigned char* s = g_traceStubs + i * kTraceStubBytes;
        const uint32_t index = static_cast<uint32_t>(i);
        s[0] = 0x8B; s[1] = 0x44; s[2] = 0x24; s[3] = 0x04;
        s[4] = 0x50;
        s[5] = 0x68;
        memcpy(s + 6, &index, 4);
        s[10] = 0xE8;
        const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&LuaTraceCall) -
                                                 reinterpret_cast<uintptr_t>(s + 15));
        memcpy(s + 11, &rel, 4);
        s[15] = 0x83; s[16] = 0xC4; s[17] = 0x08;
        s[18] = 0xC3;
        g_tf[i].orig = reinterpret_cast<LuaCFn_t>(static_cast<uintptr_t>(fn));
        g_tf[i].slot = slot;
        const uint32_t stub = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(s));
        memcpy(reinterpret_cast<void*>(slot), &stub, sizeof(stub));
        ++wrapped;
    }
    FlushInstructionCache(GetCurrentProcess(), g_traceStubs, kTraceCount * kTraceStubBytes);
    BotScanInstall(textLo, textHi); // 21i
    Log("[luacb] controller_scan.cpp file build p21j-r31 - controller page scan (21c, log only): %d of %d ScriptCB functions wrapped before the game registers "
        "them with Lua (the options screens' controller, screen, popup, visibility and text calls; 21e: the split "
        "screen sign-in's profile list, sides, profile loading and controller calls; 21f: switching sides, Galactic "
        "Conquest's teams, the campaign / conquest rules); their arguments are logged once the code is unlocked",
        wrapped, kTraceCount);
}

namespace {

} // namespace

void ControllerScanStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build)
    {
        Log("[controls] DISABLED - unknown exe");
        return;
    }
    LuaTraceHelpersInit(); // 21c
    if (!build->profileIndexRva || !build->profileArrayRva || !build->localSlotsRva ||
        !build->controlSiteCount)
    {
        Log("[controls] DISABLED - profile / binding layout not found on this build");
        return;
    }
    int ok = 0;
    for (size_t i = 0; i < build->controlSiteCount; ++i)
    {
        const ByteSite& site = build->controlSites[i];
        if (CheckSite(site, true))
        {
            ++ok;
            Log("[controls] byte-check %-56s rva 0x%08X  OK", site.name, site.rva);
        }
    }
    const int total = static_cast<int>(build->controlSiteCount);
    if (ok != total)
    {
        Log("[controls] DISABLED - %d of %d layout byte-checks failed", total - ok, total);
        return;
    }

    const HANDLE t = CreateThread(nullptr, 0, ControllerScanThread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[controls] scan started: %d of %d byte-checks OK, polling every %lu ms", ok, total,
            static_cast<unsigned long>(kPollMs));
    }
    else
    {
        Log("[controls] thread failed to start (error %lu)", GetLastError());
    }
}

// 21f
int SidesLockedNow()
{
    if (!g_sideLockIni)
        return 0;
    if (!g_canSwitchByte)
        return -1;
    // as the game's own check (offline): switching only while "can switch sides" is on and the metagame rules are off
    if (!*g_canSwitchByte)
        return 1;
    return (g_metaRulesByte && *g_metaRulesByte) ? 1 : 0;
}

int LockedSideP34(int k, const char** why)
{
    const bool gc = g_metaRulesByte && *g_metaRulesByte;
    const LONG mine = (k >= 0 && k <= 1) ? g_gcSide[2 + k] : 0;
    const LONG p1 = g_gcSide[0];
    int rel = 1;
    const char* w = "player 1's side (the campaign)";
    if (gc && (mine == 1 || mine == 2) && (p1 == 1 || p1 == 2))
    {
        rel = mine == p1 ? 1 : -1;
        w = rel > 0 ? "his pick on PICK YOUR SIDE: player 1's side" : "his pick on PICK YOUR SIDE: the side player 1 is not on";
    }
    else if (gc)
        w = "player 1's side (no pick of his for this conquest)";
    if (why)
        *why = w;
    return rel;
}

int PlayerSideNow(int player)
{
    if (!g_sideTable || player < 0 || player > 1)
        return -1;
    return static_cast<int>(*reinterpret_cast<volatile const int32_t*>(g_sideTable + kSideStride * static_cast<uint32_t>(player)));
}

} // namespace bf1
