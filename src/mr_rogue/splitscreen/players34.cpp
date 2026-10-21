// BF1GameExt - players34.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was playern_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r42: THE SPLIT HALVES BACK TO THE GAME'S SHAPE: r41's BF2-style halves (112 x 45 deg) looked hideous (Mr. Rogue's
// video). SplitFovH / SplitFovP3 are 0 by default now: each top / bottom half keeps the game's own shape (48 deg across, its
// height from the half) at SplitFovScale % of the game's FOV (110 by default: 52.8 x 15.9 deg at 16:9). 1 / 2 stay as r39 - r41.
// 21j-r41: SPLITFOVTALL ([Splitscreen] SplitFovTall, degrees): r40's halves (BF2's 51 deg tall, 119 wide) were too wide; with
// SplitFovH / SplitFovP3 = 2 each half is now SplitFovTall degrees tall (45 by default: 112 deg wide at 16:9). Nothing else
// changed.
// 21j-r40: PLAYER 3'S HALF'S FOV ([Splitscreen] SplitFovP3; SfhP3Fov / SfhP3Remember below InstallUnderwaterFix, used by
// 20g-r14's ChaseShapeFix): the 3-player bottom half gets the 2-player halves' FOV too, widened for its own shape. Nothing
// else changed.
// 21j-r39: THE 2-PLAYER TOP / BOTTOM FOV ([Splitscreen] SplitFovH; the section below InstallUnderwaterFix): BF1 keeps a view's
// 48 deg across, so a top / bottom half lost half of one player's height (48 x 14 deg at 16:9); now each half gets BF2's
// (SplitFovH=2: 119 x 51 deg, BF2's 2-player halves) or BF2's rule (1: one player's height, 83 x 28 deg). Nothing else
// changed (20g-r14's player 3 half check follows the game's SetFov behind ours).
// 21j-r38: THE UNDERWATER TINT ON STEAM: r37's log - its point loop pattern (GOG 1.3's) missed Steam's build, so it never went
// in. The SetPoint call is now found by its target inside the overlay function, and the function's bytes are logged once.
// 21j-r37: THE UNDERWATER TINT ([Splitscreen] UnderwaterFix; the section below InstallWaterFix): the game's underwater overlay
// put its points at the view's size from the screen's top left, never at the view's own place - only view 1 ever showed it.
// Each point now gets its view's left / top edge. Nothing else changed.
// 21j-r36: THE WATER FIX ([Splitscreen] WaterFix; the section below InstallWaterScan): r35's shaders - the water's second pass
// reads its wave normals over the whole 2048 x 2048 texture, the game's pre-pass drew each split view's into its share only
// (the waves stop at the view's middle, other views' normals smear in); the pre-pass now draws over the whole texture for
// each split view. Nothing else changed.
// 21j-r34: (1) PLAYERS 3 AND 4'S NAME TAGS ON STEAM (r33's log: "name display setup: 0 pattern hits" - none of r33's name tags
// went in; the section above InstallWaterScan): the setup is found by its anchors instead of one long GOG pattern (the name
// array's load, "gamefont_tiny", its 2280h / 4800h, its call through RootIndex's own thunk - checked against the thunk, which
// InstallHud34 has moved by then), and views 3 / 4's name tasks are made and set up by the game's own create and setup, run a
// second time right after the game's setup with the array holding ours and RootIndex(0 / 1) answering views 3 / 4's HUD roots
// (HudRootIndexWrap asks NameRootRemap first); the run's loop end is found by its calls; the sign-in names go in by
// themselves. If the setup is not found, every candidate's bytes are logged. (2) THE WATER SHOTS (render_hooks.cpp; [water34]
// shot lines): r33's scan found each view's water reading its own pictures; the pictures each view's water reads are now saved
// for one frame (BF1GameExt_water next to the exe) with the draws' shader constants - by itself once (WaterSummary) and at the
// capture key. Nothing else changed.
// 21j-r33: (1) PLAYERS 3 AND 4'S NAME TAGS ([Splitscreen] NamesP34=1; [names34] lines; the section above InstallWaterScan): the
// names over the human players' soldiers come from the game's NameDisplayThread, one per viewer [2]; its loop over the
// characters ended at max(2, the net player count) - 2 offline - so players 3 / 4's characters (2 / 3) never got a tag in any
// view, and views 3 / 4 had no name display at all. The loop's end now answers the live player count (3 / 4; 2-player
// unchanged); two more name displays are made with the game's two (viewers 2 / 3) and set up in views 3 / 4's HUD roots (the
// setup loop's body made again with the game's own functions, read out of its code); players 3 / 4's characters get the
// names they signed in with (the sign-in's LAUNCH, BF1GameExt_split.lua r33: "signname N NAME") instead of the game's
// "Player 3" / "Player 4". (2) THE WATER SCAN ([Splitscreen] WaterScan=1; [water34] lines; log only - the section below the
// names): each view's reflection pass, water draw and refraction copy run inside a render_hooks.cpp phase, which notes what
// each pass draws into and counts every water draw's textures by who wrote them (its own view this frame, another view, an
// older frame); 20 s summaries per view, and each view's water draw in full every 60 s. Nothing else changed.
// 21j-r15: THE END OF A MATCH - players 3 / 4's views on players 1 / 2's cameras ([Splitscreen] EndShotP34=1; EndShot34Frame,
// [shot] lines): from VICTORY / DEFEAT (split_spawn_menus.cpp's P2SpawnMatchEnded) until the next match, view 3 shows player 1's camera
// and view 4 player 2's - the game's end shots (the video after r14: player 3, alive at the end, stayed on his soldier). Nothing else
// changed.
// Phase 21j-r11: the frame-rate scan's parts in FrameWrap (render_hooks.cpp, [perf] lines): its own work before the game's
// render (players 3 / 4: cameras, spawn screens, HUD - kPerfOurFrame) and the game's render frame (kPerfRender). Log only:
// FrameWrap does what it did, in the same order.
// Phase 21j-r7 (r6's log; the user: "you didn't fix the double lens zoom problem" - r6 logged it):
//  - THE DOUBLE BLUR FIXED ([Splitscreen] ScopeBlurFix=1): r6's device trace - the blur pass draws its picture with the
//    blur target's whole viewport (0 0 2048 2048), at its top left (256 x 256: u v 0 - 0.125), for every view; the last pass
//    read it at (the view's share of the screen) x 0.125: in a split view only part of the picture (a quarter's quarter, a
//    half's half) stretched over the view - the blur round the scope a 2 x copy of part of the view. Its u v now run
//    0 - 0.125 over every split view, as 1 player's over the screen (ScBlurPass2C, the stub at the last quad's unlock).
//  - PLAYERS 3 / 4'S RETICLE BOB FIXED ([Splitscreen] ReticleFixP34=1): r6's scan - players 1 / 2's reticle height equals
//    their camera's as drawn (difference 0.0000); players 3 / 4's came from last frame's camera (sd 0.013 - 0.019, up to
//    0.07 of the view's height when zoomed), while this frame's camera's would hold still (player 4: 0.0000 a frame). In
//    FrameWrap, after their cameras are set, their reticle's height is the one this frame's camera gives (the HUD roots
//    are drawn after it, in the view loop). The scans stay (ScopeScan, ReticleScan).
// Phase 21j-r6 (r5's videos: "4 player looks better", but players 2 - 4's zoom blur "looks like it's seeing double - player
//    1 is not like that", "in the horizontal and vertical halves their scopes are smaller", players 3 / 4's "crosshairs
//    are moving a little bit up and down"):
//  - BIGGER SCOPES IN HALVES ([Splitscreen] ScopeHalfFill=96): the ring fills that % of the half's short side (r5: 1
//    player's proportion, the ring 0.815 of a horizontal half's height / 0.63 of a side-by-side half's width); quarters
//    as r5. A half's blur is drawn in passes that never overlap (the strips beside the scope all blurred, then the scope's
//    rectangle with the mask's hole round the ring) - r5's two passes left a sharp sliver beside the scope.
//  - THE DOUBLE BLUR, LOG ONLY ([scope] lines, ScopeScan=1): the blur's last pass samples its blur picture at (the view's
//    share of the screen) x (256 / the target's size) - right only for a view at the screen's top left (player 1). Each
//    view's last quad is logged, and render_hooks.cpp traces each view's blur draw on the device once ([scope] trace: the
//    screen copy, the render target and viewport the blur pass draws with, the last pass's) - where the picture is drawn
//    decides r7's fix.
//  - THE RETICLE BOB, LOG ONLY ([reticle] lines, ReticleScan=1; the comment block below InstallScopeScan): players 3 / 4's
//    reticle height is projected with their camera before FrameWrap moves it for the frame; each projection is noted and
//    made again with the camera as it is in FrameWrap (what r7 would set), with 15 s summaries per player.
// Phase 21j-r5: THE ZOOM SCOPE FITS EACH SPLIT VIEW ([Splitscreen] ScopeFit=1; the comment block below InstallScopeBlur). r4's
//    log proved it: the game draws the scope full-screen size in every view, its middle at the middle of the view's safe
//    area (lopsided in the quarters and side-by-side halves). Right after each ScopeDisplay's Update, in a HUD root smaller
//    than the screen: its group's anchor at the view's middle (the crosshair), its quarters scaled to the view at 1 player's
//    shape (a quarter: exactly 1 player's scope; a half: as big as its short side, two dark bands of the game's bitmap class
//    at the ends), and in a half the scope blur drawn twice - the second time its last quad over the scope's rectangle only
//    (the blurmask's hole round the ring as 1 player's; a stub at the quad's unlock, draw +597h). Put back in a full-screen
//    root; 1 player never touched. ScopeScan=1 keeps r4's [scope] lines (plus the fit's).
// Phase 21j-r4: THE SCOPE SCAN (log only, [scope] lines; InstallScopeScan, its comment block below InstallScopeBlur). The
//    zoom scope in split screen is cut off and its cross is off the reticle in every split view (the user's videos; 1
//    player is right): each ScopeDisplay's Update and the scope blur's draw go through their vtable slots (the game's
//    functions run as they are, then a look): the first times each player's scope is up, its HUD root, the root's safe
//    area, its group's anchor, its four quarters and what 21j-r5 would set (dry run); each view's viewport while a scope
//    is up; the sites 21j-r5 needs, checked. [Splitscreen] ScopeScan=1 (not in the INI yet). Nothing else changed.
// Phase 21h-r3: NO CROUCH AS PLAYERS 3 / 4 LEAVE THEIR PAUSE MENU. While a real pause menu of theirs is up (split_spawn_menus.cpp,
//    PlayerNMenuInput) his pad still drives the menu, but his soldier's control state gets a calm packet (no sticks, no buttons;
//    the bit every packet has kept); when the menu goes the calm packet is held until every button of his pad is up (3 s at
//    most) - the B (crouch) / A (jump) / Start that closed it never reaches his soldier. The same hold after BF1GameExt's own
//    pause screen (PlayerNSetPaused). [pad34] lines. Nothing else changed.
// Phase 21g-r2: THE PLAYER COUNT FOLLOWS THE SIGN-IN SCREEN. Players / Views are what is installed (the most a match can
//    have); the sign-in screen's LAUNCH (menus.cpp -> PlayerNSignIn) sets each following match's count:
//    2 = the game's own 2-player split (no quarters: SetNumCameras(2) is left as the game made it; the game told 2 local
//    players; players 3 / 4's pads, HUD, spawn screens and views stand aside - everything of theirs already asks for
//    "3 or more"), 3 = the 3-player layout (as Views=3: player 3 in the bottom half), 4 = quarters. The pads that signed
//    in as players 3 / 4 are theirs in the match (the first free pads as before when none). Until a sign-in: the INI's.
// Phase 20g-r14 (r13's video: "the 3rd screen on the bottom is too streched. fix it just like the 2 player bottom
//    horzanatal"): the game's ChaseCamera Apply gives whatever camera it runs on the shape of HUD root 0 - player 1's
//    view - every frame (GOG 1.3 0x0013B02A - 0x0013B17E, then SetFov 0x0028AB10). r13 set player 3's half its shape once
//    (ratio x 0.5); his chase Apply put a quarter's back each frame, so the half drew a quarter's picture twice as wide.
//    Now right after his chase Apply his ratio is scaled by (his root's h / w) / (root 0's h / w) - what the chase gives
//    the 2-player bottom half, where root 0 is a half too ([shape3] lines; ChaseShapeScan / ChaseShapeFix). Death and
//    free cameras never set the shape (only the zoom), so it holds while he is down. 4-player quarters: unchanged.
// Phase 20g-r13: 3-PLAYER STEP 1 ([Splitscreen] Views=3, Players=3; as BF2's 15a): players 1 and 2 in the top quarters
//    exactly as with 4 views, player 3 in the full-width bottom half (kHalfBottom: player 2's half in the 2-player
//    horizontal split), no fourth view. His camera gets the shape the game's own SetNumCameras(2) gives a half (GOG 1.3
//    0x00094FB1: the screen's height / width x 0.5, 48 deg - the 2-player bottom half's camera); AddViewport makes his
//    HUD root a half (1024 x 312) with its own safe area (split_spawn_menus.cpp gives it the Classic Collection's). (r13
//    checked only the camera projection 0x0028AC50's callers - SetNumCameras, the camera manager's and RedCamera's
//    constructors, three debug commands - and missed SetFov 0x0028AB10 in the chase Apply: r14.) Players is held to 3
//    with Views=3.
// Phase 20g-r10 (from r9's test: "the players cant see their killer before they go back to spawn screen", "fix all the
//    player cameras to match player 1 - finding their killer, the exact time when they go back to the spawn screen,
//    then the exact screen shot"):
//  - r9 put players 2 - 4 on player 1's camera shot 1.5 s into their death camera - right when the game's death camera
//    turns to the killer (DeathCamera::Update, GOG 1.3 0x0013BA70: 0 - 1 s it pulls out, from 1.5 s it turns to the
//    killer, fully on him at 3.5 s). Player 1's spawn screen opens when his respawn timer (character +C0h) runs out:
//    4.0 s after his death (r9's log: 219094 -> 223110 ms, 288578 -> 292578 ms), and the game puts his camera on a shot
//    in that frame (RandomCameraShot from the spawn screen). Players 2 - 4's spawn screens already came at exactly that
//    time (4000 ms in every death of r9's log). Now their view goes to player 1's shot in the frame their own spawn
//    screen comes up (split_spawn_menus.cpp's P2SpawnScreensUp; without our screens for him: when his respawn timer has run
//    out) - before that, the game's death camera, killer and all. DeathCamStill (the r7 hold at 1.5 s) is 0 by
//    default now. r9's one-frame flash of the shot in the frame of a death (before the death camera started) is gone.
//  - Player 2's view: camera 1's render camera set in the frame hook (after the camera manager applied whichever of
//    his cameras is current) - r9 set it only through DeathCamera::Apply, which is no longer hooked.
// Phase 20g-r9 (from r8's test: "2 3 and 4s add screen shot doesnt follow player 1s", "zoom is still busted", "player 3
//    and 4 go through vehicles and ai players"):
//  - CAMERA SHOTS: when player 1's chase camera is put on one of the level's camera shots (AddCameraShot; the camera
//    manager keeps them at +80h count, +84h rotations, +184h positions) - by his spawn screen (CameraManager
//    RandomCameraShot, GOG 1.3 0x00094D90) or at the match start (SetCameraShot 0x00094E20) - the shot is kept (his
//    chase camera's matrix right after). Players 2 - 4 show it while they have no soldier: once their death camera
//    has run DeathCamStillMs (through DeathCamera::Apply 0x0013B340, its thunk: the matrix set after the game's apply),
//    and before their first spawn (players 3 / 4's views). 4-player only. [Splitscreen] CamShotP234=1. [shot] lines.
//  - THEIR ZOOM: r7 let ScopeDisplay into players 3 / 4's HUD but its create / Setup loop / destroy had no wraps (the
//    log: "ScopeDisplay: NOT made (its Setup loop's thunk could not be moved)") - so their zoom used player 1's / 2's
//    scope (lent) and never reached their blur slots ("0 sets"). The wraps are there now.
//  - COLLISION: the collision manager collides an object only when it is within 60 m (3600 squared, GOG 1.3
//    0x007DACEC) of one of the camera manager's two cameras (its two update loops, 0x000A0930 / 0x000A01E0, copy the
//    cameras' positions into a 2-slot stack array and test each object). Players 3 / 4 far from players 1 / 2 walked
//    through vehicles and AI. Both loops' squared-distance call (0x0005F910, the object's position right after the
//    difference in both frames - byte-checked) now goes to CollDistWrap: the game's distance, or the distance to
//    player 3's / 4's camera when that is nearer. [Splitscreen] CollisionP34=1. [coll34] lines.
// Phase 20g-r8: PLAYERS 3 AND 4'S CONTROLLER OPTIONS (their pause screen's OPTIONS page, split_spawn_menus.cpp; nothing
//    changes for 2 players). Their PlayerInputs are the game's class (0x718), so they keep the game's settings per
//    control mode - mode [+4] (0 on foot, 1 ground vehicles, 2 flying; the PlayerController sets it, GOG 1.3
//    0x001F1C4D), the Y flip bytes +8 + mode (the update negates the look pitch with it, 0x00063EA8) and the pad look
//    scale floats +4D4h + mode * 4 (0x00063B36). The game writes them from the profile only for players 1 and 2
//    (0x001F39D0: flip = the profile's byte, scale = 2 ^ ((byte - 5) / 5)); its defaults (0x001F42D0): flip on for
//    flying only, sensitivity byte 5 (= 1.0). Players 3 / 4 had flip 0 for all three (flying was not inverted as
//    player 1's default) and 1.0. Now PlayerNControlsSet / Get: invert look (on foot + ground vehicles), invert flying,
//    look sensitivity 1-10 (5 = the game's default), written into their PlayerInputs every input frame (the two
//    reads byte-checked first), kept in BF1GameExt.ini [Splitscreen] InvertLookP3 / P4, InvertFlyingP3 / P4,
//    SensitivityP3 / P4 (PlayerNControlsSave). [pad34] lines; the heartbeat shows each one's mode, flips and scale.
// Phase 20g-r7 (from r6's test, videos and shots):
//  - "PLAYER 1 BACKS INTO A WALL, PLAYERS 3 AND 4'S CAMERAS GO IN TOO": the soldier's third-person camera eases its
//    wall distance through the camera manager (GOG 1.3 0x00095F50, [cm + 18h + i*4], i = the camera the manager is
//    working on, [cm + 7Ch]) - players 3 and 4's chase cameras run after the manager's loop, with i back at 0, so all
//    three shared player 1's distance. While their chase / death cameras run, that function (its guard) now eases a
//    distance of their own (put in the slot for the call, the manager's put back). [Splitscreen] WallCamP34=1.
//  - THEIR RED DAMAGE TINT: the screen filter holds 2 records (20d-r6 skipped players 3 / 4). Each of them now has a
//    record of his own: the game's Flash writes it (in slot 0 for the call) and view 3 / 4 draws it (slot 0 and view
//    0 for the call, the filter's own update run first). [Splitscreen] TintP34=1.
//  - COMMAND POST LIGHTS: an "align" hologram has two lights, coloured for players 1 / 2 (CharIndex(0 / 1)) and each
//    left out of the other's view only - views 3 / 4 got both. Each view now gets the light of its player's team (when
//    players 1 and 2 are on the same team, the second light is coloured for player 3 / 4 instead - the update's
//    CharIndex(1) answered with his slot). [Splitscreen] CpLightsP34=1.
//  - THEIR ZOOM: ScopeDisplays for players 3 / 4 (HudP34Mask bit 4, now in the default 255); the scope blur's on /
//    off (a 2-slot static, GOG 1.3 0x005EBCC0 - the word after it is the blur effect's pointer) and its draw (which
//    reads the slot of the view being drawn) go through BF1GameExt: players 3 / 4 have slots of their own.
//    [Splitscreen] ScopeP34=1.
//  - DEATH CAMERAS: players 2 - 4's death cameras hold their picture (a still shot) once DeathCamStillMs have
//    passed - after 1.5 s the game's death camera turns to the killer and follows him (the camera "followed a player").
//    Player 1's is left as the game has it. 4-player only. [Splitscreen] DeathCamStill=1, DeathCamStillMs=1500.
//  - DIAGNOSTIC (log only): [fx34] particle emitter draws per view every 30 s ("player 3 lost the explosions").
// Phase 20g-r6 (from r5's test and video):
//  - "LOW RES WHEN PLAYER 1 DIES": the soldier draw (GOG 1.3 0x0010F2C0, every soldier in every view) forces the low
//    detail level while the PC spawn screen object is up (SpawnDisplay +237Eh, player 1's spawn screens) - in EVERY
//    view, so while player 1 chose a spawn point everyone's soldiers (their own too) were the low-res models. Its call
//    of that getter (+33h) now goes to SpawnShownLodWrap: the flag counts only in player 1's own view (the camera
//    manager's renderer view of camera 0). [Splitscreen] SpawnLodP1Only=1 (2 = in 2-player split screen too, 0 = off).
//  - PLAYERS 3 AND 4'S DEATH CAMERAS: the game starts the death camera of camera 2 / 3 (CameraManager 0x00095260: the
//    guard dropped it since 20d) - now each of them gets a DeathCamera of the game's class in our memory (0x70 bytes,
//    its constructor from the camera manager's, byte-checked), started from his chase camera's matrix with the game's
//    own Setup(target, killer, matrix), then updated and applied to view 3 / 4 every frame instead of his chase
//    camera until he has a soldier again - player 1's dying camera. [Splitscreen] DeathCamP34=1. [death34] lines.
//  - their spawn point screen's map: PlayerNHudMap / PlayerNHudMapShow for split_spawn_menus.cpp (their own MapDisplays),
//    and HudFrame ends with P2SpawnP34MapFrame (after their HUD is shown / hidden).
//  - their pause screens (split_spawn_menus.cpp): PlayerNSetPaused - while one is up his PlayerInput updates with no pad
//    (its pad slot +4BCh empty for the call): his soldier stands still, the pad still drives the pause screen.
//  - THEIR SOUND: the sound engine has 4 listener slots and plays each 3D sound for the nearest active one, but only
//    the camera manager's two (players 1 and 2) were on - players 3 and 4 heard everything from players 1 / 2's
//    places. Each of them now has a listener of his own at view 3 / 4's camera, pointed every frame as the camera
//    manager does for its two (a free slot; else the sound set-up's own one is borrowed and put back). [Splitscreen]
//    SoundP34=1. [sound34] lines.
// Phase 20g-r5 (from r4's test: HUD working, a crash after the first round, more to do):
//  - the crash after the first round: players 3 and 4's spawn screens are deleted at the match teardown, while the
//    match is still loaded (P2SpawnP34Teardown, from the first HUD destroy) - split_spawn_menus.cpp has the details.
//  - their minimap: the command posts / soldiers / vehicles / markers (the game's own icon add / remove run for theirs
//    too, through the four functions' thunks; at each match start the ones already there are given to theirs), and
//    the small map itself (SetMode(0) each time their HUD is shown - it stayed unsized until a big map was opened).
//  - command post colours in views 3 / 4: CameraManager::ViewToCamera answers cameras 2 / 3 for views 3 / 4 (it
//    answered 0, player 1, so players 3 / 4 saw player 1's friend / enemy colours).
//  - their stamina bar (stamina_hud.cpp: their StatusDisplays recognised, PlayerNHudStatusPlayer).
//  - diagnostics, log only: [diag34] (each player's MapDisplay / CaptureDisplay state every 5 s - the capture bar),
//    [wall34] (cameras within 1.3 m of their soldiers - "player 1 backed into a wall pulls players 3 / 4's in").
// Phase 20g-r4: r3 crashed the moment player 3 was in the world (Steam 0x002AF863, a null read): his MapDisplay's
// objectives panel (GOG 1.3 0x001887A0) looks up his console spawn point screen by name, "ifs_mapselect3...", which
// does not exist, and uses the null it gets. That function now does nothing for a MapDisplay of player 3 / 4 (through
// its one jump thunk; its name format is byte-checked). Nothing else changed.
// Phase 20g-r3: PLAYERS 3 AND 4'S HUD ([Splitscreen] HudP34=1, HudP34Mask). Two more of each of the game's HUD display
// classes (Boot, Capture, Map, Reticule, StatIcon, Status, Team - not Scope yet), made with the game's new and the
// class's constructor right after the game's two (each class's create thunk), set up for players 3 / 4 right after the
// game's two at each match start (each class's per-camera Setup loop thunk: reset + Setup(2 / 3)) - in the HUD roots
// of views 3 / 4, which the camera manager's RootIndex(2 / 3) now answers (IsChase / GetChase(2 / 3): their chase
// cameras) - updated and drawn by the game from then on, hidden while their spawn screens are up (each class's own
// ForPlayer loop) and deleted before the game's two at the teardown (each class's destroy thunk). The display getters
// answer theirs for players 3 / 4; MapDisplay's map texture loop gives theirs the level's map. The HUD flag of view
// 3 / 4 is on while their displays are set up there (and while their spawn screens are up, as 20f). The 45 s / 3 min
// tables list their displays. The HUD section below has the details.
// Phase 20g-r2: r1 crashed the game at start on Steam: the scan of MapDisplay's Setup for its RootIndex call took an E8
// byte inside another instruction's address for a call and read its "target" far outside the exe. The code readers
// (CodeByte / CodeU32) now read only inside the exe's image, and the Setup scan counts an E8 only if its target is in the
// code section. Nothing else changed (still log only).
// Phase 20g-r1: THE HUD SCAN FOR PLAYERS 3 AND 4 (log only; [hud34] lines). The 8 HUD displays' create / destroy / reset
// / per-camera Setup functions, their jump thunks and the calls through them, the match teardown's destroy order, the
// camera manager's RootIndex / IsChase / GetChase (what the displays ask with their player), all found by pattern and
// byte-checked at start; the live displays (class, player +18h, root index +1Ch, in a task list) and what the camera
// manager answers for players 3 and 4 today in the 45 s / 3 min tables. The plan for 20g-r2 is in the 20g section
// below. Nothing else changed.
// 20f-r2: PLAYERS 3 AND 4'S SPAWN SCREENS (split_spawn_menus.cpp, [Splitscreen] SpawnP34=1). The frame hook runs them each
// frame (P2SpawnP34Frame, from PlayersFrame, before the views are drawn) and then turns the HUD flag of view 3 / 4 on
// while player 3's / 4's screen is up in that view's HUD root (the renderer draws roots[v] in view v only with the flag
// on - GOG 1.3 0x0028F0C8; MakeQuarters leaves it off, as before, the rest of the time). The test auto-spawn stands aside
// for a player whose screens look after him (his pad connected). PlayerNSpecView: his quarter's view, for his root.
// 20e-r2: (1) VIEWS 3 AND 4 THROUGH THE GAME'S OWN CHASE CAMERA. From the p20e-r1 run (Steam): the pads worked
//   (pad 3 = XInput 2, pad 4 = XInput 3, ~10 000 control packets each, no fault), but the bottom views were "way
//   off" - BF1GameExt's follow camera (built from the soldier's matrix) looked at the sky over the soldier and never
//   followed the aim. Now each of players 3 and 4 has a ChaseCamera of the game's class in BF1GameExt memory (0xE0
//   bytes, its constructor (camera manager, index 2 / 3) found in the camera manager's constructor, GOG 1.3
//   0x00139AC0 - it only uses its index for the network local slot, 4 in 20c), given its target by the game itself:
//   the camera manager's SetChaseTarget(target, 2 / 3) (caught since 20d-r2) now hands the target to their chase
//   camera (ChaseCamera::SetTarget 0x0013A960, read from SetChaseTarget's own call). Each frame, right before the
//   views are drawn, the chase camera's own Update(dt) (vtable +4) and Apply(view camera, dt) (vtable +8) run on view
//   3 / 4's camera - what the camera manager does for players 1 and 2 (its vtable +10h call, which writes the camera
//   manager's per-camera array, is not made). Without a target (dead, not spawned) the view keeps its last picture,
//   or the 20b spectator before the first one. [chase34] lines.
//   (2) sprint.cpp (r2): L3 sprint + stamina and third person for players 3 and 4, as players 1 and 2.
// Phase 20e: 4-PLAYER STEP 5, PADS 3 AND 4 ([Splitscreen] PadsP34=1, with Players=3 / 4; 0 = players 3 and 4 copy
//   pads 1 and 2 as in 20d). Everything from 20d still here.
//   From the p20d-r6 run (Steam): no fault in 100+ s; players 3 and 4 spawned, were shot, died and spawned again
//   (damage tint caught 13 times, rumble 65, death camera 963, HUD displays lent 17).
//   Player 2's chain (6a-7c, pads.cpp / player2_input.cpp) built again for players 3 and 4, with the same game code:
//   (1) PADS 3 AND 4: two more pads of the game's class (pads.cpp's PadBuild / PadPoint / PadUpdate), each pointed
//       at a connected XInput pad the game's pad, pad 2 and the other one are not using (lowest index first; again
//       once a second while not connected) and updated each frame. [pad34] lines.
//   (2) THEIR INPUT OBJECTS: a PlayerInput + buffers A / B each, built and wired as player 2's (player2_input.cpp's
//       PlayerInputBuild), updated each frame right after player 2's (inside the input manager's update), and their
//       buffer A copied into the control state of their player id (2 / 3) - what 7c does for player 2.
//   (3) THE GAME READS THEM: GetControlState (GOG 1.3 0x0019FA60) offline ANDs the id with 1 - for ids 2 and 3 our
//       census hook now answers the control state of that id itself (the table has one per player id, 50, all made
//       by 0x0019C4C7); and the input manager's getters (6c, vtable +18h / +1Ch / +20h) answer indexes 2 and 3 with
//       their objects (in r6 the PlayerController asked GetPlayerInput(2), GOG 1.3 0x001F1C4A, and got manager
//       memory past the end).
//   Players 3 and 4 still spawn by the test spawn (AutoSpawnP34) - their spawn screens are a later phase.
// 20d-r6: THE SHOOTING CRASH FOUND (the r5 [p34] trace). From the p20d-r5 run (Steam): the same fault as r4 - the
//   effects update (GOG 1.3 0x00131500) called an effect whose vtable was 0xDE020766 - and the trace showed what
//   ran for player 3 right before it: the damage code (0x000C2DD0) asked the local lookup for player 3 (0x000C3143)
//   and then flashed the damage screen tint for him. 0xDE020766 is a constant in exactly that code (0x000C31AC,
//   0x000C35EC): it is pushed as the tint's id to ScreenFilterEffect::Flash(local, ...) (0x00208850, thiscall, 6
//   arguments), which writes a 20h-byte record at +38h + local * 20h - and the ScreenFilterEffect (ctor 0x002084F0)
//   holds TWO records (+38h..+78h, its end). Player 3's record went past the object into the next heap block, an
//   other effect, whose first dword (its vtable) became the tint's id; player 4's goes further. Fixed: Flash does
//   nothing for local players 3 and 4 (no damage tint on their views yet). Also: the filter's draw (vtable +10h,
//   0x002086E0, through its jump thunk) reads the record of the view being drawn - views 3 and 4 read past the
//   object - so it draws nothing in views 3 and 4; and the motion blur's per-player pair (0x004C1254 / 0x004C125C,
//   [2] each, set by 0x0018FCF0 / 0x0018FD20 with the local player - 4 call sites in the soldier code) is left alone
//   for players 3 and 4 (player 3's flag landed on player 1's blur amount). The r5 rumble guards stay (they were
//   real out-of-bounds writes too). Found by following every caller of the local lookup into the functions it hands
//   the index to (the r5 scan stopped at the first call after the index was pushed; this one tracks the stack).
// 20d-r5: STILL A CRASH WHEN SHOOTING (p20d-r4, Steam: a damage on player 3's soldier, then a fault in the
//   effects update - GOG 1.3 0x00131500 walks the FLEffect table 0x004A32D0 and one effect's vtable was garbage).
//   The r3 and r4 faults are both "an object somewhere got overwritten" - the write happened earlier. Every code
//   path that acts for one local player gets his index from the net id -> local lookup (0x00197DD0, which 20c
//   opened to 4), so all 101 call sites of it and of the soldier -> local helpers were followed statically: the
//   ones that index a 2-slot table were already guarded, except the INPUT MANAGER's force feedback (rumble)
//   functions. FLInputManager (static, 0x004A3D90) holds ONE PlayerInput (0x718 at +1Ch) and ONE rumble object
//   (0x34 at +748h); player 2's are BF1GameExt's through the getters (6c), but these functions index inline:
//   (net id) -> local -> rumble object +748h + local * 34h (its "on" byte +30h), PlayerInput +1Ch + local * 718h.
//   Player 3's / 4's land inside buffer B and past the end of the manager - a weapon's fire (6 call sites, the
//   weapons 0x000E2E6D ... 0x0012A375, through 0x00135D00), a hit (0x0010C624 -> 0x00135ED0) and 0x00135A70 /
//   0x00135DD0 / 0x00135C80 rumble there, for a player 3 who fires whenever player 1 does. The 8 functions of that
//   kind (same opening: (net id) -> local -> the rumble object's test) now do nothing for players 3 and 4 (matched
//   by their net ids in our local slots). Their pads get rumble when they get pads.
//   DIAGNOSTIC (r5): the first time each call site asks / is answered for player 3 or 4 through any of our hooks
//   (the census entry points, the local lookup, the guards), a [p34] line with the time and the caller's rva is
//   written at once - so whatever ran for players 3 / 4 just before a fault is in the log, in order.
// 20d-r4: THE CRASH WHEN SHOOTING, fixed. From the p20d-r3 run (Steam): a fault in the renderer's end-of-frame pass
//   (GOG 1.3 0x0028F300 walks render layer 3, 0x006AB548, and calls each object's draw) - an entry there was a float
//   (0x420E72C6), not an object. Players 3 and 4 move with players 1 and 2's controls, so when player 1 fires, player
//   3 fires too, and a weapon's fire goes to its owner's first-person model: 0x0012D270 (player, x) =
//   FirstPersonRenderable[player]->(x), with no bound (six call sites, the weapons' fire code: 0x000DE8B5,
//   0x000E7CF7, 0x000FA7D7, 0x001141CE, 0x00126C87, 0x0019201A). FirstPersonRenderable[2] at 0x004A0FD8 has nothing
//   for player 3 / 4 - what lies after it was used as one, its state machine ran on it and the renderer's lists were
//   written through it. Two more first-person functions guarded (13 now): 0x0012D270 and 0x0012BDC0 (player -> its
//   model's vtable +0Ch, also unbounded). The two are told apart from a look-alike (0x0028EA30, the renderer's) by
//   their table: the one guard 0 (0x0012B640) reads.
// 20d-r3: THE CRASH WHEN PLAYER 3 / 4 IS SHOT, fixed. From the p20d-r2 run (Steam): players 3 and 4 spawned, died and
//   spawned again (camera manager guards caught SetChaseTarget 6x, death camera 2x); then a fault when one was hit:
//   the hit code asks GetStatusDisplay(local) for the damage-direction flash (GOG 1.3 0x000DD060 -> display +29D0h)
//   and StatusDisplay[2] (0x005ECBA0) has nothing in slot 2 / 3. Every per-player HUD display is reached through a
//   getter that indexes its 2-slot array without a bound: Boot 0x00078470, Map 0x000784E0, Team 0x00078540, Scope
//   0x00094B20, Status 0x000DB330, Capture 0x001A14F0 (the census's six) and GetMessageDisplay(player, k)
//   0x0018B0E0 (MessageDisplay[2][2]); Reticule and StatIcon are only reached by their 2-loops (camera count). Until
//   players 3 and 4 have HUDs of their own (a later phase), each getter answers player 1's display for player 3
//   and player 2's for player 4 - their hits, messages and captures show on the top players' HUDs for now.
// 20d-r2: THE CRASH AT PLAYER 3 / 4'S SPAWN, fixed. From the p20d-r1 run (Steam): Spawn pressed for players 3 and 4,
//   then an access violation in the spawn-point check (GOG 1.3 0x000A01E0: for i < the camera manager's camera count,
//   copy camera i's position into a 2-slot array on the stack). Cause: when a local player's soldier is given to him
//   (GOG 1.3 0x002246A0) the game hands the camera manager his local slot - SetChaseTarget(soldier, 2) - and the
//   camera manager's per-camera arrays have 2 slots: slot 2 of its fade times [2] at +18h is its camera count at +20h,
//   set to 1.0f (0x3F800000), and the next loop over "all cameras" ran off its arrays. The camera manager's methods
//   that take a camera index from outside now go through guards (each one's jump thunk; only for the camera manager
//   itself, indexes 2 and 3): SetChaseTarget (0x000947A0) and the death camera (0x00095260) do nothing, the fade
//   (0x00095F50, a float) answers its target, and GetCamera (0x00083EB0, 36 call sites) answers BF1GameExt's own
//   camera of view 3 / 4 (a RedCamera of the game's own constructor) instead of what lies past its cameras [2].
// Phase 20d: 4-PLAYER STEP 4, PLAYERS 3 AND 4 GET CAMERAS AND A SPAWN (Players=3 / 4, Views=4). Everything from 20c
// still here.
//   From the 20c run (Steam, Bespin, no faults): the match setup made "Player 3" and "Player 4" (characters 2 and 3,
//   index 2 / 3, team 1); the local slots read 0 1 2 3; the count was answered 4 about 726 000 times and kept at 2 at
//   the 10 sites; the per-view soldier code (GOG 1.3 0x0010F2C0) now asks the local slots for views 3 and 4 - the
//   bottom quarters already lead to players 3 and 4's characters.
//   20d: (1) PLAYERS 3 AND 4 SPAWN by themselves for this test, as player 2 first did (autospawn.cpp): every 3 s,
//   while player 1 is in the world and player 3 / 4 is not (and not ready), the game's own SetReady(1) on that
//   character (the PC Spawn button's call) - the game spawns him with his side's default class at his default
//   post. [Splitscreen] AutoSpawnP34=1. (2) THEIR CAMERAS: view 3 follows player 3's soldier and view 4 player 4's
//   - a third-person camera of BF1GameExt's own, behind and above the soldier, from the soldier's world matrix
//   (EntitySoldier +1B0h, ragdoll.cpp's; the unit a character holds is the soldier +250h, its class checked by RTTI)
//   - and the spectator (player 1's / player 2's camera turned round) while he has no soldier. (3) THE TWO 2-PLAYER
//   TABLES THEIR SOLDIERS REACH, guarded: (a) the pause states (PauseState[2]): a soldier of a local player sets its
//   player's pause state when it starts (GOG 1.3 0x00097EA0 -> SetPaused(local, 0)) - SetPaused A / B and IsPaused
//   do nothing for players 3 and 4 (IsPaused answers 0); (b) the first person / player music state (BF2's 14e crash:
//   FirstPersonRenderable, here 0x1650-byte objects [2] at GOG 1.3 0x004A0FD8, made per camera, and their 0x2C0-byte
//   per-player state [2] at 0x004A1030): eleven functions take the player first and index both without a bound -
//   0x0012B640 (19 call sites: a unit's death / exit), 0x0012CDA0 (17), 0x0012D2E0 (6), 0x0012B9D0 / 0x0012BEC0 /
//   0x0012C660 / 0x0012C6D0, 0x0012B980 / 0x0012BA60 / 0x0012BE10 / 0x0012CD30 - each one's jump thunk goes through
//   a guard that returns at once for players 3 and 4 (none returns a float; the call sites clean their arguments).
//   Players 3 and 4 have no first-person view and no music of their own yet.
//   Not yet: their own pads - offline GetControlState ANDs the index with 1, so player 3 moves with player 1's
//   controls and player 4 with player 2's (as BF2's 14d did) - their spawn screens, HUD, pause menus (20e on).
// Phase 20c: 4-PLAYER STEP 3, PLAYERS 3 AND 4 MADE TO EXIST ([Splitscreen] Players=3 or 4, with Views=4; Players=2 =
// nothing of 20c installed). [players] lines. Everything from 20a / 20b still here.
//   From the 20b run (Steam, Bespin, no faults): the four quarters drew - players 1 and 2 on top with their HUDs and
//   spawn screens, the two spectators below. Two things to fix: (1) GOG 1.3 0x00163CE0 sets the camera count to 1 and
//   back to 2 about 270 times a match (its own 1-camera pass); each time the four views were made again - fine, but
//   every one was logged and the [views4] budget ran out while loading: now the first 6 and then every 100th are
//   logged, with a running count. (2) The spectators were turned round the camera's own up axis, so a player looking
//   down gave a spectator looking up (one quarter of sky): now turned round the world's up (x and z of each axis
//   negated), so the pitch follows.
//   20c: the local player count is read in ONE place (GetNumLocalPlayers, its 24 call sites through one jump thunk -
//   the 20a census hook), so it is not written at all: while the real count is 2 the wrapper answers Players (3 or 4)
//   - except to the call sites that walk a 2-slot table, which keep getting 2 (each one found at start and logged):
//     the camera manager's creation and SetNumCameras(count) at match load (the camera manager stays at 2 cameras;
//     20b's quarters as before), the three pause loops (GOG 1.3 0x001E6A90 / 0x001E6AE0 / 0x001E6E90: PauseState[2],
//     what follows is a colour - the update loop calls a vtable on it), the message loop in 0x001A0A70
//     (MessageDisplay[2][2] - GetMessageDisplay(player, k) 0x0018B0E0 = [player*2 + k]).
//   The network's local slots (int[2] at GOG 1.3 0x004C35F0, local index -> net id) move to 4 of BF1GameExt's: every
//   operand that holds their address is found through the exe's own relocation table (GOG 1.3: 8 - 6 to slot 0, 2 to
//   slot 1), checked, and moved; the id -> local lookup's bound (0x00197DD0, cmp eax, 2) and the fill loop's bound
//   (0x00197590, cmp esi, 2) go to 4. So the match setup (0x0021A667, one character per local player) makes characters
//   for players 3 and 4, and GetLocalPlayer(2 / 3) finds them.
//   Not yet (as BF2's 14c): players 3 and 4 get no input (GetControlState still ANDs the index with 1), no camera of
//   their own (the bottom quarters stay spectators), no spawn; their pause states, menus and HUD are later phases.
//   The five places that name players 1 and 2 (0x00239380, 0x00239460, 0x0018D8E9, 0x0019E430, 0x001A5E3C) are
//   untouched (team checks and team choice for the two, not a 2-slot table).
// Phase 20b: 4-PLAYER STEP 2, THE DRAWING SIDE TO 4 VIEWS ([Splitscreen] Views=4; Views=2 = nothing of 20b installed,
// the 2-player split exactly as before). [views4] lines. Everything from 20a is still here (PlayerNScan).
//   From the 20a run (Steam, one split match, 4 min, no faults): every site found, census on all 16 entry points, the
//   tables as the static map said. The renderer is already a 4-view renderer: its view loop (GOG 1.3 0x0028EB60)
//   draws every view it has (scene with the view's camera, then roots[view], the HUD root), its alloc 0x0028C440
//   takes a rect (x, y, w, h fractions) and a camera and answers the view index (-1 when all 4 are taken), its free
//   0x0028EB10 empties a slot. Everything the game reads by the CURRENT view (0x0028DD60, 22 call sites) was
//   checked for views 2 and 3: tests of "is it my view", a 1 << view bit, the camera manager's view -> camera
//   lookup (0x00094A60: loops to 2 and answers camera 0 for any other view), reads of the motion blur amounts
//   (0x00070320, skipped in split by SplitMotionBlurOff) and a scope flag (0x00073730) - reads only, no writes.
//   What PC lacks is the camera manager side (2 cameras, the 1- and 2-camera layouts only).
//   20b, with Views=4: SetNumCameras' jump thunk goes through BF1GameExt. After the game's own SetNumCameras(2) the
//   two players' views are made again as quarters (player 1 top left, player 2 top right; each camera gets the
//   full screen's shape - a quarter has the screen's shape - and the PC's own 48 deg) with the game's own free /
//   alloc, their view indices written back (+6Ch, the HUD root indices: 0 and 1 again) and their draw flags kept;
//   then views 3 and 4 (bottom left / right) are added with cameras of BF1GameExt's own (the game's RedCamera ctor
//   and projection), SPECTATORS until players 3 and 4 exist: each frame, right before the game draws (the view
//   loop's jump thunk goes through BF1GameExt), player 1's / player 2's camera matrix turned round (right and
//   forward negated), their scene flag following the player's view, no HUD root drawn. Before any other
//   SetNumCameras, or when the camera manager changes, views 3 and 4 are taken out again. Vertical is ignored
//   with Views=4. Heartbeat line "[views4] heartbeat" every 30 s.
//   20a fix: the characters' names are 16-bit text (the 20a tables showed "P").
// Phase 20a: 4-PLAYER STEP 1, THE MAP (2020 build: GOG 1.3 / Steam). Log only: nothing in the game changes.
// [playerN] lines. [Splitscreen] PlayerNScan=1 (0 = nothing installed).
//
// THE STATIC MAP - every place the PC game stops at 2 local players, found in GOG 1.3 (its relocation table,
// RTTI and disassembly; the Steam exe is the same code, found here by pattern at start). GOG 1.3 rvas.
//
// (1) THE LOCAL PLAYER COUNT. One int at 0x004382FC. GetNumLocalPlayers 0x0012BBB0 (its jump thunk 0x00009E49,
//     24 call sites) is its only reader; SetNumLocalPlayers 0x0016FEC0 its only writer, called by
//     ScriptCB_SetSplitscreen 0x00168B00 (count = flag + 1: 1 or 2 only), the boot-split switch 0x001BA9A0 (2)
//     and the way back to the menus 0x00208CB0 (1). Its readers: the match setup 0x0021A667 (one character
//     per local player, i < count - the game itself makes characters 3 and 4 once the count is 4);
//     SetNumCameras(count) at match load 0x00163E36 and the camera manager's creation 0x00146EE6; the pause
//     loops 0x001E6A90 / 0x001E6AE0 / 0x001E6E90 (i < count over a 2-slot array, (5)); the network's local
//     slots fill 0x00197590 (bound 2, (3)); 0x0019FC13 (i < count); a tail jump 0x0019FF70; and eleven "more
//     than 1?" tests (split yes / no): 0x0012BF56, 0x0012CDCA (first person), 0x001507A2 and 0x0015268E
//     (a 0.7 scale in split), 0x00168B33 (IsSplitscreen), 0x001873CA (the map layout picker), 0x0018893B,
//     0x001A10E4 / 0x001BA9B3 (the boot-split flag), 0x00208CB9, 0x00221E17.
// (2) THE IF MANAGERS (menus, one per player). IFManager[2] (0x244 each) static at 0x004A3650 (vector ctor
//     at 0x00036310), reached through the pointer 0x00438270. GetIFManager(i) 0x000784B0 answers NULL for i
//     > 1; the Lua's current-manager getter 0x00134380 the same for its index 0x004A35EC. Manager 1's fields
//     are also written by name (0x0013435A, 0x00134A55 ...), and the IF update (0x00134DE0) runs the two.
//     The input step's held-button table 0x004A35D0 had ONE slot (18u moved it to 4 of ours).
// (3) THE NETWORK'S LOCAL PLAYERS (as BF2's 14a (1)). Local slots int[2] at 0x004C35F0 (local index -> net
//     player id, filled for i < 2 at 0x00197590, -1 past the count), read by 0x00197E40 (57 call sites);
//     the id -> local index lookup 0x00197DD0 loops to 2. Net players 0x004C1FC0, 0x60 bytes each.
// (4) THE CONTROL STATES (what a PlayerController reads). One per net player (50 pointers at 0x004C4CC8),
//     but GetControlState(i) 0x0019FA60 offline (byte 0x004C4BC2 == 0) takes i & 1: player 3 would get
//     player 1's controls and player 4 player 2's. Its callers: the PlayerController ctor 0x001F1780,
//     0x001A16E0.
// (5) THE PAUSE STATES. PauseState[2] (0x24 each) static at 0x0053FE10 (vector ctor 0x0003D210), reached
//     through the pointer 0x0043A6EC; a colour follows at 0x0053FE58 (what player 3's would be). SetPaused
//     (player, on) 0x00097D20, (player, x) 0x0016FE20, IsPaused(player) 0x000639B0.
// (6) INPUT. FLInputManager, static at 0x004A3D90: its ctor 0x00135250 builds its per-player arrays with ONE
//     slot each (PlayerInput 0x718 at +1Ch, buffer A 0x14 at +734h, a 0x34 object at +748h, buffer B 0x140
//     at +77Ch); its player count +14h (0x004A3DA4) is set to 1 at boot (InputMgrInit(1) 0x00135830, the push
//     at 0x00146D1D). Player 2's objects are BF1GameExt's (6b / 6c). The game polls 4 XInput pads.
// (7) LOCAL SLOTS AND PROFILES. Local slots 2 x 0x30 at 0x00568FE0 (vector ctor 0x0003DA90; set up at
//     0x001F3530: ids i, profile 0x001F4000(i) at +18h), searched to the table's end by 0x001F3470,
//     0x001F34B0, 0x001F34F0. Profiles 2 x 0x458 at 0x005690A0, the current index 0x00569958
//     (GetCurrentProfile 0x001F3F10: index <= 1).
// (8) CAMERAS AND VIEWS. CameraManager (0x244 bytes, RTTI CameraManager) at [0x0045FF50] (getter 0x00078490,
//     190 call sites), made by 0x00094850(count). Its ctor 0x00094410 makes exactly TWO cameras (cmp ebx, 2):
//     per camera a float at +18h, +24h, RedCamera +2Ch, ChaseCamera +34h, FreeCamera +3Ch, DeathCamera +44h,
//     a view object +4Ch, two 0xC entries at +54h, the renderer view +6Ch (= the HUD root index: RootIndex
//     0x00077600 reads +6Ch[player]) - 2-slot arrays packed back to back, so a third camera has no room in
//     it. SetNumCameras 0x00094F10 (count at +20h) has only the 1-camera (full) and 2-camera (top / bottom,
//     48 deg, rects 0x003DAA50 / 0x003DAA70) layouts: 3 and 4 do nothing (the Xbox's are not in the PC
//     build). The renderer side is already 4: its view alloc 0x0028C440 takes up to FOUR views (cmp esi, 4)
//     - rects 0x006AB330 [4], cameras 0x006AB370 [4], count 0x006AB384, flags 0x006AB3B0 / 0x006AB3B4 [4],
//     HUD roots 0x006AB3BC [4] (AddToRoot 0x0028BDD0).
// (9) THE HUD (C++, one object per player). Each display is an array of 2 made by a fixed 2-loop and updated
//     for i < the camera count (the loops 0x0013D010 ... 0x0013D1D0, one per display): BootDisplay
//     0x0045FE80, CaptureDisplay 0x00460090, MapDisplay 0x004BB984, ReticuleDisplay 0x005AD940, ScopeDisplay
//     0x005EBD20, StatIconDisplay 0x005ECAF0, StatusDisplay 0x005ECBA0, TeamDisplay 0x005ECDC0. Getters
//     (mov eax, [i*4 + array]): Boot 0x00078470, Map 0x000784E0, Team 0x00078540, Scope 0x00094B20, Status
//     0x000DB330, Capture 0x001A14F0. Also per player: MessageDisplay [2][2] 0x004BE9E0 (0x0018AF00),
//     NameDisplayThread [2] 0x004C14D0 (0x00195D50), MineDisplayThread [2] 0x004BFA90 (0x0018DEC0). One for
//     both: SpawnDisplay 0x0045E4A0, ChatMessageDisplay 0x00460150, EndDisplay 0x0049C470, ToolTipsDisplay
//     0x005F42B0.
// (10) FIRST PERSON (the soldier's first-person model, "human_animation_fp" / "hp_camera"). 2 x 0x2C0 at
//     0x004A1030 (vector ctor 0x00035EC0), indexed by player (0x0012B980 ...), with per-player values at
//     0x004A0FC4 / 0x004A0FCC / 0x004A0FD8 (BF2's 14e crash was its version of this).
// (11) OTHER STATIC 2-SLOT ARRAYS (vector ctor, not named yet): 0x004B8FB8 (2 x 1Ch), 0x004C5354 (2 x 0Ch),
//     0x004CE018 (2 x 20h), 0x0056A4D0 (2 x 40h), 0x008EF340 / 0x008EF410 / 0x008EF434 (2 x 10h).
//
// WHAT THIS SCANNER DOES (nothing it does changes what the game does):
//   (a) finds every site above by pattern on the running exe, each exactly once, and reads each table's
//       address out of the code ("OK" / "NOT FOUND" lines; the static 2-slot arrays listed from every
//       vector-constructor call with a count of 1-4);
//   (b) a census of the calls into the per-player entry points - GetNumLocalPlayers (count answered),
//       GetIFManager, the network's local slot getter, the six display getters, the three pause calls and
//       GetControlState (index asked), the id -> local lookup and two local slot searches (index answered) -
//       through their jump thunks (each thunk's target moves to a counting wrapper that calls the game's own
//       function; every call site goes through the thunk in GOG 1.3, none direct). By caller (Steam rva of
//       the return address) and player; reset when a split match starts, logged 45 s into it and when it ends;
//   (c) every table, live (read with ReadProcessMemory, never faulting), 45 s and 3 min into a split match:
//       what each slot holds (objects by their RTTI class name) and what lies right after each 2-slot table.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
// Return address of the current function (who called the wrapper). Declared directly instead of including
// <intrin.h>, as player2_input.cpp does.
#ifdef _MSC_VER
extern "C" void* _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
#define BF1_RETURN_ADDRESS() _ReturnAddress()
#else
#define BF1_RETURN_ADDRESS() __builtin_return_address(0)
#endif

#include "render_hooks.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "split_spawn_menus.hpp"
#include "pads.hpp"
#include "player2_input.hpp"
#include "players34.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

// ---------------------------------------------------------------------------------------------------------
// Patterns (GOG 1.3 bytes; ?? over addresses and call distances)
// ---------------------------------------------------------------------------------------------------------
// (2) GetIFManager(i): cmp eax, 1; ja; imul eax, 244h; add eax, [pointer]
const char* const kGetIfManager =
    "55 8B EC 8B 45 08 83 F8 01 77 0E 69 C0 44 02 00 00 03 05 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kGetIfManagerPtr = 0x13;
// the Lua's current-manager getter: mov eax, [index]; cmp eax, 1; ja; imul; add eax, [pointer]
const char* const kIfCurrent = "A1 ?? ?? ?? ?? 83 F8 01 77 0D 69 C0 44 02 00 00 03 05 ?? ?? ?? ?? C3";
constexpr uint32_t kIfCurrentIndex = 0x01;
// (3) id -> local index: for eax < 2: cmp [eax*4 + slots], ecx
const char* const kPatNetIdToLocal = "55 8B EC 8B 4D 08 85 C9 78 15 33 C0 0F 1F 40 00 39 0C 85 ?? ?? ?? ?? 74 09 40 83 F8 "
                                  "02 7C F1 83 C8 FF 5D C3";
constexpr uint32_t kNetIdToLocalSlots = 0x13;
// the local slots fill: call GetNumLocalPlayers; or ecx, -1; cmp esi, eax; cmovb; mov [esi*4 + slots], ecx;
// inc esi; cmp esi, 2; jb
const char* const kNetSlotsFill =
    "E8 ?? ?? ?? ?? 83 C9 FF 3B F0 0F 42 CE 89 0C B5 ?? ?? ?? ?? 46 83 FE 02 72 E6";
constexpr uint32_t kNetSlotsFillSlots = 0x10;
// (9) mov eax, [i*4 + array] - the display getters (and the net local slot getter; 11 hits in GOG 1.3)
const char* const kArrayGetter = "55 8B EC 8B 45 08 8B 04 85 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kArrayGetterArray = 0x09;
// (5) SetPaused (two of this shape) and IsPaused: lea ecx, [eax + eax*8]; mov eax, [pointer]; lea ecx, [eax+ecx*4]
const char* const kPauseCall = "55 8B EC 8B 45 08 FF 75 0C 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kPauseCallPtr = 0x0D;
const char* const kPatIsPaused = "55 8B EC 8B 45 08 8D 0C C0 A1 ?? ?? ?? ?? 8D 0C 88 5D E9";
// (4) GetControlState(i): cmp byte [online], 0; mov eax, [ebp+8]; jne; and eax, 1; mov eax, [eax*4 + table]
const char* const kPatControlState =
    "55 8B EC 80 3D ?? ?? ?? ?? 00 8B 45 08 75 03 83 E0 01 8B 04 85 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kControlStateOnline = 0x05;
constexpr uint32_t kControlStateTable = 0x15;
// (7) the local slot searches (three of this shape: by +8, +0, +4 of a 30h slot)
const char* const kSlotSearch = "55 8B EC 8B 55 08 33 C9 B8 ?? ?? ?? ?? 0F 1F 00 39 10 74 10 83 C0 30 41 3D ?? ?? ?? "
                                "?? 7C F1 83 C8 FF 5D C3";
constexpr uint32_t kSlotSearchStart = 0x09;
// (9) the per-camera display loops: call GetCameraManager; mov ecx, eax; call count; test; jle;
// mov ecx, [esi*4 + display array]; push esi; call (8 hits)
const char* const kHudLoop = "56 33 F6 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 85 C0 7E 1E 8B 0C B5 ?? ?? ?? ?? 56 E8";
constexpr uint32_t kHudLoopMgrCall = 0x03;
constexpr uint32_t kHudLoopArray = 0x16;
const char* const kMovEaxImmRet = "A1 ?? ?? ?? ?? C3";
const char* const kMessageMake = "BF ?? ?? ?? ?? 66 0F 1F 84 00 00 00 00 00 33 F6 68 00 05 00 00 E8";
constexpr uint32_t kMessageMakeArray = 0x01;
// NameDisplayThread / MineDisplayThread: mov [esi*4 + array], eax; mov [eax+18h], esi; inc esi; cmp esi, 2
const char* const kThreadMake = "89 04 B5 ?? ?? ?? ?? 89 70 18 46 83 FE 02 7C C2";
constexpr uint32_t kThreadMakeArray = 0x03;
// (6) InputMgrInit(count): mov eax, [ebp+8]; push 0; mov [manager + 14h], eax
const char* const kInputInit = "55 8B EC 8B 45 08 6A 00 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 00 A3";
constexpr uint32_t kInputInitCount = 0x09;
// the manager ctor's first array: push dtor; push 1; push 718h; lea eax, [esi+1Ch]
const char* const kInputArrays = "68 ?? ?? ?? ?? 6A 01 68 18 07 00 00 8D 46 1C";
// (8) the renderer's view alloc: up to 4 views
const char* const kViewAlloc = "A1 ?? ?? ?? ?? 56 33 F6 85 C0 74 20 83 3C B5 ?? ?? ?? ?? 00 74 05 46 3B F0 72 F1 83 "
                               "FE 04 72 08";
constexpr uint32_t kViewAllocCount = 0x01;
constexpr uint32_t kViewAllocCameras = 0x0F;
// SetNumCameras, and its two layouts: sub eax, 1; je (1 camera); sub eax, 1; jne (not 2: nothing)
const char* const kSetNumCameras = "55 8B EC 8B 45 08 83 EC 20 57 8B F9 8B 4F 20 3B C1 0F 84";
const char* const kSetNumCamerasLayouts =
    "E8 ?? ?? ?? ?? 8B 47 20 5E D9 5D 08 5B 83 E8 01 0F 84 ?? ?? ?? ?? 83 E8 01 0F 85";
const char* const kCameraCtorLoop = "C7 46 40 FF FF FF FF 43 C7 46 EC 00 00 80 3F 83 C6 04 83 FB 02 0F 8C";
// (1) ScriptCB_SetSplitscreen's count: setne cl; inc ecx; push ecx; call SetNumLocalPlayers
const char* const kSetSplitCount = "6A 01 FF 75 08 E8 ?? ?? ?? ?? 33 C9 84 C0 0F 95 C1 41 51 E8";
// (1) the match setup's local characters: for ebx < GetNumLocalPlayers: team = slot(ebx) +0Ch or 1
const char* const kLocalChars = "33 DB E8 ?? ?? ?? ?? 85 C0 7E 33 53 E8 ?? ?? ?? ?? 83 C4 04 85 C0 78 0E 8D 04 40 C1 "
                                "E0 04 8B 80 ?? ?? ?? ?? EB 05 B8 01 00 00 00 53 50 8B CF E8";
// (11) static arrays: push dtor; push count; push size; push array; call vector ctor
const char* const kVecCtor8 = "68 ?? ?? ?? ?? 6A ?? 6A ?? 68 ?? ?? ?? ?? E8";
const char* const kVecCtor32 = "68 ?? ?? ?? ?? 6A ?? 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8";
// the local slots' own vector ctor call (exactly one): its target is the vector constructor
const char* const kSlotsCtor = "68 ?? ?? ?? ?? 6A 02 6A 30 68 ?? ?? ?? ?? E8";
// (2) / (5) / (7) / (10) the 2-slot arrays by size
constexpr uint32_t kIfManagerSize = 0x244;
constexpr uint32_t kPauseSize = 0x24;
constexpr uint32_t kSlotSize = 0x30;
constexpr uint32_t kProfileSize = 0x458;
constexpr uint32_t kFirstPersonSize = 0x2C0;

// CameraManager fields (8)
constexpr uint32_t kCamCount = 0x20;

// ---------------------------------------------------------------------------------------------------------
// The census
// ---------------------------------------------------------------------------------------------------------
enum Kind
{
    kNumLocal = 0,  // count answered
    kIfMgr,         // index asked
    kNetSlot,       // index asked (0x00197E40)
    kNetIdToLocal,  // index answered (0x00197DD0)
    kBoot,
    kCapture,
    kMap,
    kScope,
    kStatus,
    kTeam,
    kSetPausedA,    // 0x00097D20
    kSetPausedB,    // 0x0016FE20
    kIsPaused,      // 0x000639B0
    kControlState,  // 0x0019FA60
    kSlotByFirst,   // 0x001F34B0 (index answered)
    kSlotBySecond,  // 0x001F34F0 (index answered)
    kKinds
};

const char* const kKindName[kKinds] = {
    "GetNumLocalPlayers", "GetIFManager", "net local slot (local -> net id)", "net id -> local index",
    "BootDisplay getter", "CaptureDisplay getter", "MapDisplay getter", "ScopeDisplay getter",
    "StatusDisplay getter", "TeamDisplay getter", "SetPaused A (player, on)", "SetPaused B (player, x)",
    "IsPaused", "GetControlState", "local slot search (+0)", "local slot search (+4)",
};

enum What
{
    kAsked,
    kAnswered,
    kCountAnswered
};

const What kKindWhat[kKinds] = {
    kCountAnswered, kAsked, kAsked, kAnswered, kAsked, kAsked, kAsked, kAsked, kAsked, kAsked,
    kAsked, kAsked, kAsked, kAsked, kAnswered, kAnswered,
};

constexpr int kBuckets = 5; // players 1-4 (or a count of 1-4), then anything else
constexpr int kRows = 1024;
constexpr uint32_t kSiteMask = 0x03FFFFFFu; // the exe is under 64 MB
constexpr uint32_t kNotExe = 0x03FFFFFFu;

struct Row
{
    volatile LONG key; // (kind + 1) << 26 | site rva
    volatile LONG n[kBuckets];
};

Row g_rows[kRows];
volatile LONG g_full = 0;
volatile LONG g_answered[kBuckets] = {}; // GetIFManager: NULL answers per index (0-3, other)
uintptr_t g_orig[kKinds] = {};
uint32_t g_thunk[kKinds] = {};
uint32_t g_fnRva[kKinds] = {};
bool g_hooked[kKinds] = {};
uintptr_t g_base = 0;
uintptr_t g_imageEnd = 0;

int Bucket(int32_t v)
{
    return (v >= 0 && v < 4) ? static_cast<int>(v) : 4;
}

void P34First(int kind, uint32_t site, int bucket); // 20d-r5 (below)

void Count(int kind, const void* ret, int bucket)
{
    const uintptr_t r = reinterpret_cast<uintptr_t>(ret);
    const uint32_t site = (r >= g_base && r < g_imageEnd) ? static_cast<uint32_t>(r - g_base) & kSiteMask : kNotExe;
    const LONG key = static_cast<LONG>((static_cast<uint32_t>(kind + 1) << 26) | site);
    uint32_t h = (site * 2654435761u) ^ static_cast<uint32_t>(kind * 40503);
    for (int probe = 0; probe < kRows; ++probe)
    {
        Row& row = g_rows[(h + static_cast<uint32_t>(probe)) & (kRows - 1)];
        const LONG k = row.key;
        if (k == key)
        {
            if (InterlockedIncrement(&row.n[bucket]) == 1)
                P34First(kind, site, bucket);
            return;
        }
        if (k == 0)
        {
            const LONG was = InterlockedCompareExchange(&row.key, key, 0);
            if (was == 0 || was == key)
            {
                if (InterlockedIncrement(&row.n[bucket]) == 1)
                    P34First(kind, site, bucket);
                return;
            }
        }
    }
    InterlockedIncrement(&g_full);
}

typedef uint32_t(__cdecl* Fn0_t)();
typedef uint32_t(__cdecl* Fn1_t)(int32_t);
typedef uint32_t(__cdecl* Fn2_t)(int32_t, int32_t);

// GetNumLocalPlayers: the count answered (1-4 -> buckets 0-3)
// 20c: Players 3 / 4 - while the real count is 2, every caller gets Players except the call sites listed in
// g_clampRet (they walk a 2-slot table and keep getting 2).
bool g_playersOn = false;
uint32_t g_players = 2;
// 21g-r2: the player count follows the sign-in. g_players is the count of this match (2 - g_playersMax); g_playersMax the
// installed one (Players); the views' layout of this match (2 = the game's own 2-player split, 3 = the 3-player layout, 4
// = quarters); the XInput pads that signed in as players 3 / 4 (-1: the first free one, as before).
uint32_t g_playersMax = 2;
int g_viewsLive = 2;
int g_padPref[2] = { -1, -1 };
volatile LONG g_signIns = 0;
constexpr int kMaxClamp = 24;
uintptr_t g_clampRet[kMaxClamp] = {};
int g_clampCount = 0;
volatile LONG g_answered4 = 0;
volatile LONG g_clamped = 0;

// 20d-r5: [p34] - the first time a call site asks for (or is answered) player 3 or 4, logged at once.
constexpr LONG kMaxP34Lines = 200;
volatile LONG g_p34Lines = 0;

void P34Line(const char* what, uint32_t site, int player)
{
    if (!g_playersOn || InterlockedIncrement(&g_p34Lines) > kMaxP34Lines)
        return;
    Log("[p34] %lu ms: %s for player %d, from rva 0x%08X (first time from there)", ScanMsSinceAttach(), what,
        player + 1, site);
}

const char* const kKindNameP34[] = {
    "GetNumLocalPlayers", "GetIFManager", "net local slot", "net id -> local index", "BootDisplay getter",
    "CaptureDisplay getter", "MapDisplay getter", "ScopeDisplay getter", "StatusDisplay getter", "TeamDisplay getter",
    "SetPaused A", "SetPaused B", "IsPaused", "GetControlState", "local slot search (+0)", "local slot search (+4)",
};

void P34First(int kind, uint32_t site, int bucket)
{
    if (kind == kNumLocal || (bucket != 2 && bucket != 3) || kind < 0 || kind >= kKinds)
        return;
    P34Line(kKindNameP34[kind], site, bucket);
}

// For the guards (one line per guard and call site).
constexpr int kP34Seen = 256;
volatile LONG g_p34Seen[kP34Seen] = {};

void P34Guard(const char* what, uint32_t tag, const void* ret, int player)
{
    if (!g_playersOn)
        return;
    const uintptr_t r = reinterpret_cast<uintptr_t>(ret);
    const uint32_t site = (r >= g_base && r < g_imageEnd) ? static_cast<uint32_t>(r - g_base) : 0;
    const LONG key = static_cast<LONG>(((tag & 0x3Fu) << 26) | (site & kSiteMask) | 0x80000000u);
    const uint32_t h = (site * 2654435761u) ^ (tag * 40503u);
    for (int probe = 0; probe < kP34Seen; ++probe)
    {
        volatile LONG& slot = g_p34Seen[(h + static_cast<uint32_t>(probe)) & (kP34Seen - 1)];
        const LONG k = slot;
        if (k == key)
            return;
        if (k == 0 && InterlockedCompareExchange(&slot, key, 0) == 0)
        {
            char text[96];
            snprintf(text, sizeof(text), "%s (caught)", what);
            P34Line(text, site, player);
            return;
        }
    }
}

uint32_t __cdecl NumLocalWrap()
{
    uint32_t r = reinterpret_cast<Fn0_t>(g_orig[kNumLocal])();
    const void* ret = BF1_RETURN_ADDRESS();
    if (g_playersOn && r == 2)
    {
        bool clamp = false;
        for (int i = 0; i < g_clampCount && !clamp; ++i)
            clamp = g_clampRet[i] == reinterpret_cast<uintptr_t>(ret);
        if (clamp)
            InterlockedIncrement(&g_clamped);
        else
        {
            r = g_players;
            InterlockedIncrement(&g_answered4);
        }
    }
    Count(kNumLocal, ret, Bucket(static_cast<int32_t>(r) - 1));
    return r;
}

// GetIFManager: the index asked, and NULL answers
uint32_t __cdecl IfManagerWrap(int32_t i)
{
    const uint32_t r = reinterpret_cast<Fn1_t>(g_orig[kIfMgr])(i);
    Count(kIfMgr, BF1_RETURN_ADDRESS(), Bucket(i));
    if (!r)
        InterlockedIncrement(&g_answered[Bucket(i)]);
    return r;
}

// one index argument, the index asked
volatile LONG g_pauseGuarded = 0; // 20d: pause calls for players 3 / 4 not made

volatile LONG g_hudAliased = 0;   // 20d-r3: display getters for players 3 / 4 answered with player 1's / 2's

constexpr bool IsDisplayKind(int k)
{
    return k == kBoot || k == kCapture || k == kMap || k == kScope || k == kStatus || k == kTeam;
}

uint32_t ControlStateOwn(int32_t id); // 20e (below): control state of player id 2 / 3, 0 while pads 3 / 4 are off
uintptr_t HudOwn(int kind, int k);    // 20g-r3 (below): player 3 / 4's own display of that getter's class, 0 if none

// 20g-r7: while a command post hologram's update runs on this thread (HoloUpdateWrap, below), CharIndex(0 / 1) - the
// net local slot getter - answers the slot of the player whose team colours that light
volatile int g_holoSrc[2] = { 0, 1 };
volatile DWORD g_holoThread = 0;

template <int K>
uint32_t __cdecl Asked1Wrap(int32_t i)
{
    if (K == kNetSlot && g_holoThread && (i == 0 || i == 1) && GetCurrentThreadId() == g_holoThread)
        i = g_holoSrc[i];
    if (IsDisplayKind(K) && g_playersOn && (i == 2 || i == 3))
    {
        // 20g-r3: player 3 / 4's own display (set up in his quarter) when BF1GameExt made one
        const uintptr_t own = HudOwn(K, i - 2);
        if (own)
        {
            Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
            return static_cast<uint32_t>(own);
        }
    }
    if (K == kIsPaused && g_playersOn && (i == 2 || i == 3))
    {
        InterlockedIncrement(&g_pauseGuarded);
        Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
        return 0; // not paused
    }
    if (K == kControlState && (i == 2 || i == 3) && ControlStateOwn(i))
    {
        // 20e: the control state of player id 2 / 3 itself (offline the game takes id & 1)
        Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
        return ControlStateOwn(i);
    }
    if (IsDisplayKind(K) && g_playersOn && (i == 2 || i == 3))
    {
        // 20d-r3: the display arrays have 2 slots; player 3 -> player 1's display, player 4 -> player 2's
        InterlockedIncrement(&g_hudAliased);
        Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
        return reinterpret_cast<Fn1_t>(g_orig[K])(i - 2);
    }
    const uint32_t r = reinterpret_cast<Fn1_t>(g_orig[K])(i);
    Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
    return r;
}

// two arguments, the first is the player
template <int K>
uint32_t __cdecl Asked2Wrap(int32_t i, int32_t a)
{
    if ((K == kSetPausedA || K == kSetPausedB) && g_playersOn && (i == 2 || i == 3))
    {
        InterlockedIncrement(&g_pauseGuarded);
        Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
        return 0;
    }
    const uint32_t r = reinterpret_cast<Fn2_t>(g_orig[K])(i, a);
    Count(K, BF1_RETURN_ADDRESS(), Bucket(i));
    return r;
}

// one argument (an id), the local index answered (-1 = none -> the last bucket)
template <int K>
uint32_t __cdecl Answered1Wrap(int32_t id)
{
    const uint32_t r = reinterpret_cast<Fn1_t>(g_orig[K])(id);
    Count(K, BF1_RETURN_ADDRESS(), Bucket(static_cast<int32_t>(r)));
    return r;
}

const void* WrapperFor(int kind)
{
    switch (kind)
    {
    case kNumLocal: return reinterpret_cast<const void*>(&NumLocalWrap);
    case kIfMgr: return reinterpret_cast<const void*>(&IfManagerWrap);
    case kNetSlot: return reinterpret_cast<const void*>(&Asked1Wrap<kNetSlot>);
    case kNetIdToLocal: return reinterpret_cast<const void*>(&Answered1Wrap<kNetIdToLocal>);
    case kBoot: return reinterpret_cast<const void*>(&Asked1Wrap<kBoot>);
    case kCapture: return reinterpret_cast<const void*>(&Asked1Wrap<kCapture>);
    case kMap: return reinterpret_cast<const void*>(&Asked1Wrap<kMap>);
    case kScope: return reinterpret_cast<const void*>(&Asked1Wrap<kScope>);
    case kStatus: return reinterpret_cast<const void*>(&Asked1Wrap<kStatus>);
    case kTeam: return reinterpret_cast<const void*>(&Asked1Wrap<kTeam>);
    case kSetPausedA: return reinterpret_cast<const void*>(&Asked2Wrap<kSetPausedA>);
    case kSetPausedB: return reinterpret_cast<const void*>(&Asked2Wrap<kSetPausedB>);
    case kIsPaused: return reinterpret_cast<const void*>(&Asked1Wrap<kIsPaused>);
    case kControlState: return reinterpret_cast<const void*>(&Asked1Wrap<kControlState>);
    case kSlotByFirst: return reinterpret_cast<const void*>(&Answered1Wrap<kSlotByFirst>);
    case kSlotBySecond: return reinterpret_cast<const void*>(&Answered1Wrap<kSlotBySecond>);
    default: return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------
// Reading
// ---------------------------------------------------------------------------------------------------------
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

uint8_t ReadU8(uintptr_t va) // 20g-r7
{
    uint8_t v = 0;
    if (!ReadMem(va, &v, sizeof(v)))
        return 0;
    return v;
}

float ReadF32(uintptr_t va) // 20g-r7
{
    float v = 0.0f;
    if (!ReadMem(va, &v, sizeof(v)))
        return 0.0f;
    return v;
}

// 20g-r2: both read only inside the exe's image (0 outside it). r1's Setup scan took an E8 byte inside another
// instruction's address (Steam's MapDisplay Setup) for a call, followed its "target" out of the image and crashed.
bool InCodeImage(uint32_t rva, uint32_t n)
{
    if (!g_imageEnd)
        return true; // not set up yet: as before
    const uintptr_t at = Rva(rva);
    return at >= g_base && at < g_imageEnd && g_imageEnd - at >= n;
}

uint8_t CodeByte(uint32_t rva)
{
    if (!InCodeImage(rva, 1))
        return 0;
    return *reinterpret_cast<const unsigned char*>(Rva(rva));
}

uint32_t CodeU32(uint32_t rva)
{
    uint32_t v = 0;
    if (!InCodeImage(rva, sizeof(v)))
        return 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

// rel32 jump / call at rva -> its target rva
uint32_t RelTarget(uint32_t rva)
{
    const int32_t rel = static_cast<int32_t>(CodeU32(rva + 1));
    return static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + rel);
}

// a call's target with jump thunks followed
uint32_t CallTarget(uint32_t rva)
{
    uint32_t t = RelTarget(rva);
    for (int hop = 0; hop < 8 && CodeByte(t) == 0xE9; ++hop)
        t = RelTarget(t);
    return t;
}

bool InImage(uintptr_t va)
{
    return va >= g_base && va < g_imageEnd;
}

uint32_t RvaOf(uintptr_t va)
{
    return InImage(va) ? static_cast<uint32_t>(va - g_base) : 0;
}

// An address read out of the code (an absolute operand) as an rva, 0 if it is not in the exe.
uint32_t OperandRva(uint32_t rva)
{
    return RvaOf(CodeU32(rva));
}

// The RTTI class name of an object (its vtable's complete object locator), "" if it has none.
void ClassName(uintptr_t obj, char* out, size_t n)
{
    out[0] = 0;
    if (!obj)
        return;
    const uint32_t vt = ReadU32(obj);
    if (!InImage(vt))
        return;
    const uint32_t col = ReadU32(vt - 4);
    if (!InImage(col) || ReadU32(col) != 0)
        return;
    const uint32_t td = ReadU32(col + 0x0C);
    if (!InImage(td))
        return;
    char name[96] = {};
    if (!ReadMem(td + 8, name, sizeof(name) - 1))
        return;
    name[sizeof(name) - 1] = 0;
    const char* s = name;
    if (strncmp(s, ".?AV", 4) == 0 || strncmp(s, ".?AU", 4) == 0)
        s += 4;
    size_t len = strlen(s);
    if (len >= 2 && s[len - 2] == '@' && s[len - 1] == '@')
        len -= 2;
    if (len >= n)
        len = n - 1;
    memcpy(out, s, len);
    out[len] = 0;
}

// "0x0B7014C0 (MapDisplay)" or "0x00000000" or "0x00000001 (value)"
void Describe(uint32_t v, char* out, size_t n)
{
    char cls[64];
    ClassName(v, cls, sizeof(cls));
    if (cls[0])
        snprintf(out, n, "0x%08X (%s)", v, cls);
    else if (InImage(v))
        snprintf(out, n, "0x%08X (exe rva 0x%08X)", v, v - static_cast<uint32_t>(g_base));
    else
        snprintf(out, n, "0x%08X", v);
}

void LogDwords(const char* label, uintptr_t va, int count)
{
    uint32_t v[16] = {};
    if (count > 16)
        count = 16;
    if (!va || !ReadMem(va, v, static_cast<size_t>(count) * 4))
    {
        Log("[playerN] table: %s (rva 0x%08X): unreadable", label, RvaOf(va));
        return;
    }
    char text[16 * 9 + 1] = {};
    size_t at = 0;
    for (int i = 0; i < count; ++i)
    {
        const int w = snprintf(text + at, sizeof(text) - at, i ? " %08X" : "%08X", v[i]);
        if (w > 0)
            at += static_cast<size_t>(w);
    }
    Log("[playerN] table: %s (rva 0x%08X): %s", label, RvaOf(va), text);
}

// ---------------------------------------------------------------------------------------------------------
// Finding
// ---------------------------------------------------------------------------------------------------------
// One hit, or 0 (logged either way).
uint32_t FindOne(const char* name, const char* pattern)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pattern, hits, 4);
    if (n == 1)
        return hits[0];
    Log("[playerN] %s: %d pattern hits (need exactly 1) - NOT FOUND", name, n);
    return 0;
}

// The jump thunk(s) going to fnRva: E9 rel32 in the code section with that target (the incremental link
// table). Writes up to max, returns how many. Also counts the direct E8 calls to the function.
int FindThunks(uint32_t fnRva, uint32_t* thunks, int max, int* directCalls)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    *directCalls = 0;
    if (!CodeSection(&codeRva, &codeSize) || codeSize < 5)
        return 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    int n = 0;
    for (uint32_t i = 0; i + 5 <= codeSize; ++i)
    {
        if (p[i] != 0xE9 && p[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, p + i + 1, 4);
        const uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel);
        if (t != fnRva)
            continue;
        if (p[i] == 0xE8)
        {
            ++*directCalls;
            continue;
        }
        // a link table entry: a neighbouring entry is a jump too
        const bool table = (i >= 5 && p[i - 5] == 0xE9) || (i + 10 <= codeSize && p[i + 5] == 0xE9);
        if (table && n < max)
            thunks[n++] = codeRva + i;
    }
    return n;
}

// Calls through a thunk (E8 to the thunk) - a raw count, for the log.
int CountCallsTo(uint32_t target)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!CodeSection(&codeRva, &codeSize) || codeSize < 5)
        return 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    int n = 0;
    for (uint32_t i = 0; i + 5 <= codeSize; ++i)
    {
        if (p[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, p + i + 1, 4);
        if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) == target)
            ++n;
    }
    return n;
}

// The census hook: the function's one jump thunk now goes to the wrapper. A thunk another part of
// BF1GameExt has already moved is chained (the wrapper calls whatever it pointed at).
bool g_censusOn = true; // [Splitscreen] PlayerNScan

void HookKind(int kind, uint32_t fnRva)
{
    const bool forPlayers = g_players > 2 && (kind == kNumLocal || kind == kSetPausedA || kind == kSetPausedB ||
                                              kind == kIsPaused || IsDisplayKind(kind) ||
                                              kind == kControlState); // 20c / 20d / 20e need these even
                                                                                        // without the census
    if (!fnRva || (!g_censusOn && !forPlayers))
        return;
    g_fnRva[kind] = fnRva;
    uint32_t thunks[4] = {};
    int direct = 0;
    const int nt = FindThunks(fnRva, thunks, 4, &direct);
    if (nt != 1)
    {
        Log("[playerN] census: %s rva 0x%08X: %d jump thunks (need exactly 1) - not counted", kKindName[kind], fnRva,
            nt);
        return;
    }
    const uint32_t thunk = thunks[0];
    const int via = CountCallsTo(thunk);
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(thunk));
    const uintptr_t dest = Rva(RelTarget(thunk));
    g_orig[kind] = dest;
    const void* wrap = WrapperFor(kind);
    DWORD old = 0;
    if (!wrap || !VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old))
    {
        Log("[playerN] census: %s: VirtualProtect failed (error %lu) - not counted", kKindName[kind], GetLastError());
        return;
    }
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(wrap) - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(at + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(at + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, 5);
    int32_t back = 0;
    memcpy(&back, at + 1, 4);
    g_thunk[kind] = thunk;
    g_hooked[kind] = back == rel;
    Log("[playerN] census: %s rva 0x%08X through its thunk rva 0x%08X (%d calls to the thunk, %d direct calls not "
        "counted)%s%s",
        kKindName[kind], fnRva, thunk, via, direct, dest == Rva(fnRva) ? "" : " - the thunk was already moved: CHAINED",
        g_hooked[kind] ? "" : " - READ BACK FAILED");
}

// ---------------------------------------------------------------------------------------------------------
// The map, as found on this exe
// ---------------------------------------------------------------------------------------------------------
struct StaticArray
{
    uint32_t rva;
    uint32_t count;
    uint32_t size;
    uint32_t site;
};

constexpr int kMaxStatic = 48;
StaticArray g_static[kMaxStatic];
int g_staticCount = 0;

uintptr_t g_countVa = 0;        // (1) the local player count
uint32_t g_numLocalRva = 0;
uint32_t g_ifPtrRva = 0;        // (2) pointer to the managers
uint32_t g_ifArrayRva = 0;      //     their static array
uint32_t g_ifIndexRva = 0;      //     the Lua's current index
uint32_t g_netSlotsRva = 0;     // (3)
uint32_t g_controlRva = 0;      // (4) the control state pointers
uint32_t g_onlineRva = 0;       //     the online flag
uint32_t g_pausePtrRva = 0;     // (5)
uint32_t g_pauseArrayRva = 0;
uint32_t g_inputMgrRva = 0;     // (6) the input manager (its count at +14h)
uint32_t g_slotsRva = 0;        // (7)
uint32_t g_profilesRva = 0;
uint32_t g_camMgrPtrRva = 0;    // (8)
uint32_t g_viewCountRva = 0;
uint32_t g_viewCamerasRva = 0;
uint32_t g_hudArray[8] = {};    // (9) in the loops' order
uint32_t g_messageRva = 0;
uint32_t g_threadRva[2] = {};
uint32_t g_firstPersonRva = 0;  // (10)

// The 8 per-camera display loops, in their order in the code (GOG 1.3 0x0013D010 ... 0x0013D1D0).
const char* const kHudName[8] = {
    "BootDisplay", "CaptureDisplay", "MapDisplay", "ReticuleDisplay",
    "ScopeDisplay", "StatIconDisplay", "StatusDisplay", "TeamDisplay",
};
// the getter kinds of those displays (-1: no getter)
const int kHudKind[8] = { kBoot, kCapture, kMap, -1, kScope, -1, kStatus, kTeam };

const char* StaticLabel(uint32_t rva, uint32_t count, uint32_t size)
{
    if (rva == g_ifArrayRva && size == kIfManagerSize)
        return "IF managers (2)";
    if (rva == g_pauseArrayRva && size == kPauseSize)
        return "pause states (5)";
    if (rva == g_slotsRva && size == kSlotSize)
        return "local slots (7)";
    if (rva == g_profilesRva && size == kProfileSize)
        return "profiles (7)";
    if (rva == g_firstPersonRva && size == kFirstPersonSize)
        return "first person (10)";
    if (count == 4 && size == 0x94)
        return "4 x 94h (not per player)";
    return "not named yet (11)";
}

void FindMap()
{
    const BuildInfo* build = CurrentBuild();
    uint32_t hits[16] = {};
    char pat[96];

    // (1) the count, its getter and setter
    g_countVa = build->localPlayerCountRva ? Rva(build->localPlayerCountRva) : 0;
    if (g_countVa)
    {
        const uint32_t c = static_cast<uint32_t>(g_countVa);
        snprintf(pat, sizeof(pat), "A1 %02X %02X %02X %02X C3", c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, c >> 24);
        g_numLocalRva = FindOne("GetNumLocalPlayers", pat);
        snprintf(pat, sizeof(pat), "55 8B EC 8B 45 08 A3 %02X %02X %02X %02X 5D C3", c & 0xFF, (c >> 8) & 0xFF,
                 (c >> 16) & 0xFF, c >> 24);
        const uint32_t setter = FindOne("SetNumLocalPlayers", pat);
        uint32_t thunks[4] = {};
        int direct = 0;
        const int nt = setter ? FindThunks(setter, thunks, 4, &direct) : 0;
        Log("[playerN] (1) local player count at rva 0x%08X: GetNumLocalPlayers rva 0x%08X %s, SetNumLocalPlayers rva "
            "0x%08X %s (%d calls through its thunk - GOG 1.3: 3)",
            build->localPlayerCountRva, g_numLocalRva, g_numLocalRva ? "OK" : "NOT FOUND", setter,
            setter ? "OK" : "NOT FOUND", nt == 1 ? CountCallsTo(thunks[0]) : -1);
    }
    else
        Log("[playerN] (1) the local player count is not known on this build - NOT FOUND");
    uint32_t s = FindOne("ScriptCB_SetSplitscreen's count", kSetSplitCount);
    Log("[playerN] (1) ScriptCB_SetSplitscreen: count = flag + 1 (1 or 2) at rva 0x%08X %s", s, s ? "OK" : "NOT FOUND");
    s = FindOne("match setup's local characters", kLocalChars);
    if (s)
        Log("[playerN] (1) match setup: one character per local player (i < GetNumLocalPlayers, team from local slot "
            "+0Ch or 1) at rva 0x%08X OK - calls GetNumLocalPlayers %s",
            s, CallTarget(s + 2) == g_numLocalRva ? "OK" : "(a different function?)");

    // (2) the IF managers
    s = FindOne("GetIFManager", kGetIfManager);
    if (s)
    {
        g_ifPtrRva = OperandRva(s + kGetIfManagerPtr);
        g_ifArrayRva = g_ifPtrRva ? RvaOf(ReadU32(Rva(g_ifPtrRva))) : 0;
        HookKind(kIfMgr, s);
    }
    const uint32_t cur = FindOne("the Lua's current-manager getter", kIfCurrent);
    if (cur)
        g_ifIndexRva = OperandRva(cur + kIfCurrentIndex);
    Log("[playerN] (2) IF managers: GetIFManager rva 0x%08X (NULL past 1), pointer rva 0x%08X -> the managers at rva "
        "0x%08X (0x%X each); current-manager getter rva 0x%08X (index rva 0x%08X, NULL past 1)",
        s, g_ifPtrRva, g_ifArrayRva, kIfManagerSize, cur, g_ifIndexRva);

    // (3) the network's local slots
    s = FindOne("net id -> local index", kPatNetIdToLocal);
    const uint32_t fill = FindOne("net local slots fill", kNetSlotsFill);
    if (s)
        g_netSlotsRva = OperandRva(s + kNetIdToLocalSlots);
    const uint32_t fillSlots = fill ? OperandRva(fill + kNetSlotsFillSlots) : 0;
    Log("[playerN] (3) network local slots int[2] at rva 0x%08X: id -> local index rva 0x%08X (loops to 2) %s, the fill "
        "rva 0x%08X (for i < 2) %s",
        g_netSlotsRva, s, s ? "OK" : "NOT FOUND", fill, fill && fillSlots == g_netSlotsRva ? "OK" : "NOT FOUND / differs");
    if (s)
        HookKind(kNetIdToLocal, s);

    // (4) the control states
    s = FindOne("GetControlState", kPatControlState);
    if (s)
    {
        g_onlineRva = OperandRva(s + kControlStateOnline);
        g_controlRva = OperandRva(s + kControlStateTable);
        HookKind(kControlState, s);
    }
    Log("[playerN] (4) GetControlState rva 0x%08X %s: offline (byte rva 0x%08X == 0) the player index is ANDed with 1 - "
        "player 3 reads player 1's controls, player 4 player 2's; pointers at rva 0x%08X (one per net player)",
        s, s ? "OK" : "NOT FOUND", g_onlineRva, g_controlRva);

    // (5) the pause states (two SetPaused of one shape, then IsPaused)
    int n = FindPattern(kPauseCall, hits, 4);
    if (n == 2)
    {
        g_pausePtrRva = OperandRva(hits[0] + kPauseCallPtr);
        const bool same = OperandRva(hits[1] + kPauseCallPtr) == g_pausePtrRva;
        HookKind(kSetPausedA, hits[0]);
        HookKind(kSetPausedB, hits[1]);
        Log("[playerN] (5) pause calls rva 0x%08X and 0x%08X (player, x) %s", hits[0], hits[1],
            same ? "OK - one pointer" : "- DIFFERENT pointers");
    }
    else
        Log("[playerN] (5) pause calls: %d pattern hits (need exactly 2) - NOT FOUND", n);
    s = FindOne("IsPaused", kPatIsPaused);
    if (s)
        HookKind(kIsPaused, s);
    g_pauseArrayRva = g_pausePtrRva ? RvaOf(ReadU32(Rva(g_pausePtrRva))) : 0;
    Log("[playerN] (5) pause states: pointer rva 0x%08X -> array rva 0x%08X (0x%X each)", g_pausePtrRva, g_pauseArrayRva,
        kPauseSize);

    // (6) input
    s = FindOne("InputMgrInit", kInputInit);
    if (s)
    {
        const uint32_t countRva = OperandRva(s + kInputInitCount);
        g_inputMgrRva = countRva ? countRva - 0x14 : 0;
        // who calls it, and with what (push 1 right before the call in GOG 1.3)
        uint32_t thunks[4] = {};
        int direct = 0;
        const int nt = FindThunks(s, thunks, 4, &direct);
        Log("[playerN] (6) InputMgrInit rva 0x%08X OK: input manager at rva 0x%08X, its player count +14h; %d calls "
            "through its thunk", s, g_inputMgrRva, nt == 1 ? CountCallsTo(thunks[0]) : -1);
    }
    n = FindPattern(kInputArrays, hits, 4);
    Log("[playerN] (6) input manager ctor: %d hit(s) of its \"push 1; push 718h\" (ctor and copy: GOG 1.3 has 2) - "
        "per-player arrays of ONE slot", n);

    // (8) cameras: the manager pointer from the display loops' first call, views, layouts, the ctor loop
    n = FindPattern(kHudLoop, hits, 16);
    if (n == 8)
    {
        const uint32_t getter = CallTarget(hits[0] + kHudLoopMgrCall);
        if (MatchPatternAt(getter, kMovEaxImmRet))
            g_camMgrPtrRva = OperandRva(getter + 1);
        for (int i = 0; i < 8; ++i)
            g_hudArray[i] = OperandRva(hits[i] + kHudLoopArray);
        Log("[playerN] (8) CameraManager: getter rva 0x%08X, pointer rva 0x%08X %s", getter, g_camMgrPtrRva,
            g_camMgrPtrRva ? "OK" : "NOT FOUND");
        for (int i = 0; i < 8; ++i)
            Log("[playerN] (9) %-16s [2] at rva 0x%08X - its per-camera loop rva 0x%08X", kHudName[i], g_hudArray[i],
                hits[i]);
    }
    else
        Log("[playerN] (9) per-camera display loops: %d pattern hits (need exactly 8) - NOT FOUND", n);
    s = FindOne("SetNumCameras", kSetNumCameras);
    const uint32_t lay = FindOne("SetNumCameras' layouts", kSetNumCamerasLayouts);
    const uint32_t loop = FindOne("CameraManager ctor's camera loop", kCameraCtorLoop);
    Log("[playerN] (8) SetNumCameras rva 0x%08X %s; its layouts (1 camera, 2 cameras, nothing else) at rva 0x%08X %s; "
        "the ctor's camera loop (cmp ebx, 2) at rva 0x%08X %s",
        s, s ? "OK" : "NOT FOUND", lay, lay ? "OK" : "NOT FOUND", loop, loop ? "OK" : "NOT FOUND");
    s = FindOne("the renderer's view alloc", kViewAlloc);
    if (s)
    {
        g_viewCountRva = OperandRva(s + kViewAllocCount);
        g_viewCamerasRva = OperandRva(s + kViewAllocCameras);
    }
    Log("[playerN] (8) renderer views: alloc rva 0x%08X %s (up to 4 views): count rva 0x%08X, cameras [4] rva 0x%08X",
        s, s ? "OK" : "NOT FOUND", g_viewCountRva, g_viewCamerasRva);

    // (9) the display getters: each getter's array matched to a display (or the net local slots)
    n = FindPattern(kArrayGetter, hits, 16);
    int named = 0;
    for (int i = 0; i < n && i < 16; ++i)
    {
        const uint32_t arr = OperandRva(hits[i] + kArrayGetterArray);
        int kind = -1;
        for (int d = 0; d < 8; ++d)
            if (arr && arr == g_hudArray[d])
                kind = kHudKind[d];
        if (arr && arr == g_netSlotsRva)
            kind = kNetSlot;
        if (kind >= 0)
        {
            ++named;
            HookKind(kind, hits[i]);
        }
    }
    Log("[playerN] (9) array getters: %d hits (GOG 1.3: 11), %d of them per player (GOG 1.3: 7 - six displays and the "
        "net local slots)", n, named);
    s = FindOne("MessageDisplay maker", kMessageMake);
    if (s)
        g_messageRva = OperandRva(s + kMessageMakeArray);
    n = FindPattern(kThreadMake, hits, 4);
    if (n == 2)
    {
        // MineDisplayThread comes first in the code (GOG 1.3 0x0018DEF2), NameDisplayThread second (0x00195D82)
        g_threadRva[0] = OperandRva(hits[0] + kThreadMakeArray);
        g_threadRva[1] = OperandRva(hits[1] + kThreadMakeArray);
    }
    Log("[playerN] (9) MessageDisplay [2][2] rva 0x%08X; per-player threads [2] rva 0x%08X and rva 0x%08X (%d hits, "
        "need 2)", g_messageRva, g_threadRva[0], g_threadRva[1], n);

    // (11) every static array a vector constructor makes with a count of 1-4 (the local slots' call gives the
    // vector constructor itself)
    s = FindOne("local slots ctor", kSlotsCtor);
    const uint32_t vecCtor = s ? CallTarget(s + 14) : 0;
    g_slotsRva = s ? OperandRva(s + 10) : 0;
    g_staticCount = 0;
    if (vecCtor)
    {
        uint32_t many[256] = {};
        for (int form = 0; form < 2; ++form)
        {
            const int m = FindPattern(form == 0 ? kVecCtor8 : kVecCtor32, many, 256);
            for (int i = 0; i < m && i < 256 && g_staticCount < kMaxStatic; ++i)
            {
                const uint32_t h = many[i];
                const uint32_t count = CodeByte(h + 6);
                const uint32_t size = form == 0 ? CodeByte(h + 8) : CodeU32(h + 8);
                const uint32_t arr = OperandRva(h + (form == 0 ? 10 : 13));
                const uint32_t call = h + (form == 0 ? 14 : 17);
                if (count < 1 || count > 4 || !arr || CallTarget(call) != vecCtor)
                    continue;
                StaticArray& a = g_static[g_staticCount++];
                a.rva = arr;
                a.count = count;
                a.size = size;
                a.site = h;
                if (count == 2 && size == kProfileSize)
                    g_profilesRva = arr;
                if (count == 2 && size == kFirstPersonSize)
                    g_firstPersonRva = arr;
            }
        }
    }
    Log("[playerN] (7) local slots [2] x 0x%X at rva 0x%08X; (11) vector constructor rva 0x%08X: %d static arrays made "
        "with a count of 1-4 (GOG 1.3: 16)",
        kSlotSize, g_slotsRva, vecCtor, g_staticCount);
    for (int i = 0; i < g_staticCount; ++i)
        Log("[playerN] (11)   rva 0x%08X: %u x 0x%X (made at rva 0x%08X) - %s", g_static[i].rva, g_static[i].count,
            g_static[i].size, g_static[i].site, StaticLabel(g_static[i].rva, g_static[i].count, g_static[i].size));

    // the count's getter, last: the census of the count
    if (g_numLocalRva)
        HookKind(kNumLocal, g_numLocalRva);
    // (7) the two local slot searches with callers (by +0, by +4; the one by +8 has none in GOG 1.3)
    n = FindPattern(kSlotSearch, hits, 4);
    if (n == 3 && g_slotsRva)
    {
        for (int i = 0; i < 3; ++i)
        {
            const uint32_t start = OperandRva(hits[i] + kSlotSearchStart);
            if (start == g_slotsRva)
                HookKind(kSlotByFirst, hits[i]);
            else if (start == g_slotsRva + 4)
                HookKind(kSlotBySecond, hits[i]);
        }
    }
    else
        Log("[playerN] (7) local slot searches: %d pattern hits (need exactly 3) - not counted", n);
}

// ---------------------------------------------------------------------------------------------------------
// Reports
// ---------------------------------------------------------------------------------------------------------
int LocalCount()
{
    return g_countVa ? static_cast<int>(ReadU32(g_countVa)) : 0;
}

void ResetCensus()
{
    for (int i = 0; i < kRows; ++i)
        for (int b = 0; b < kBuckets; ++b)
            InterlockedExchange(&g_rows[i].n[b], 0);
    for (int b = 0; b < kBuckets; ++b)
        InterlockedExchange(&g_answered[b], 0);
    InterlockedExchange(&g_full, 0);
}

struct Line
{
    int kind;
    uint32_t site;
    LONG n[kBuckets];
};

void ReportCensus(const char* when)
{
    static Line lines[kRows];
    int count = 0;
    for (int i = 0; i < kRows; ++i)
    {
        const LONG key = g_rows[i].key;
        if (!key)
            continue;
        Line& l = lines[count];
        l.kind = static_cast<int>((static_cast<uint32_t>(key) >> 26) - 1);
        l.site = static_cast<uint32_t>(key) & kSiteMask;
        LONG sum = 0;
        for (int b = 0; b < kBuckets; ++b)
        {
            l.n[b] = g_rows[i].n[b];
            sum += l.n[b];
        }
        if (sum && l.kind >= 0 && l.kind < kKinds)
            ++count;
    }
    for (int i = 1; i < count; ++i) // by kind, then by caller
    {
        const Line v = lines[i];
        int j = i - 1;
        while (j >= 0 && (lines[j].kind > v.kind || (lines[j].kind == v.kind && lines[j].site > v.site)))
        {
            lines[j + 1] = lines[j];
            --j;
        }
        lines[j + 1] = v;
    }
    Log("[playerN] census (%s): %d callers, %ld calls not counted (table full), local player count now %d; "
        "GetIFManager answered NULL for index 0:%ld 1:%ld 2:%ld 3:%ld other:%ld",
        when, count, static_cast<long>(g_full), LocalCount(), static_cast<long>(g_answered[0]),
        static_cast<long>(g_answered[1]), static_cast<long>(g_answered[2]), static_cast<long>(g_answered[3]),
        static_cast<long>(g_answered[4]));
    for (int k = 0; k < kKinds; ++k)
    {
        int callers = 0;
        int later = 0;
        LONG total[kBuckets] = {};
        for (int i = 0; i < count; ++i)
        {
            if (lines[i].kind != k)
                continue;
            ++callers;
            if (lines[i].n[1] + lines[i].n[2] + lines[i].n[3])
                ++later;
            for (int b = 0; b < kBuckets; ++b)
                total[b] += lines[i].n[b];
        }
        if (!g_hooked[k] && !callers)
            continue;
        const What w = kKindWhat[k];
        const char* what = w == kCountAnswered ? "count answered (1 2 3 4 other)"
                         : w == kAnswered      ? "index answered (0 1 2 3 none)"
                                               : "index asked (0 1 2 3 other)";
        Log("[playerN] census: %s (rva 0x%08X) - %d callers, %d reach %s; calls by %s: %ld %ld %ld %ld %ld",
            kKindName[k], g_fnRva[k], callers, later, w == kCountAnswered ? "a count of 2+" : "player 2+", what,
            static_cast<long>(total[0]), static_cast<long>(total[1]), static_cast<long>(total[2]),
            static_cast<long>(total[3]), static_cast<long>(total[4]));
        for (int i = 0; i < count; ++i)
        {
            const Line& l = lines[i];
            if (l.kind != k)
                continue;
            const LONG past = l.n[1] + l.n[2] + l.n[3];
            const char* tag = w == kCountAnswered ? "" : past ? "per player" : (l.n[0] ? "player 1 only" : "never a player");
            char site[40];
            if (l.site == kNotExe)
                snprintf(site, sizeof(site), "not the exe (BF1GameExt)");
            else
                snprintf(site, sizeof(site), "rva 0x%08X", l.site);
            Log("[playerN] census:   from %s  %ld %ld %ld %ld %ld  %s", site, static_cast<long>(l.n[0]),
                static_cast<long>(l.n[1]), static_cast<long>(l.n[2]), static_cast<long>(l.n[3]),
                static_cast<long>(l.n[4]), tag);
        }
    }
}

// A pointer table: its 2 slots by class, and the dword after them.
void LogPointerPair(const char* label, uint32_t rva)
{
    if (!rva)
    {
        Log("[playerN] table: %s: not found", label);
        return;
    }
    const uintptr_t va = Rva(rva);
    char a[96];
    char b[96];
    char c[96];
    Describe(ReadU32(va), a, sizeof(a));
    Describe(ReadU32(va + 4), b, sizeof(b));
    Describe(ReadU32(va + 8), c, sizeof(c));
    Log("[playerN] table: %s (rva 0x%08X): [0] %s, [1] %s | after: %s", label, rva, a, b, c);
}

// ---------------------------------------------------------------------------------------------------------
// Phase 20g-r1: THE HUD SCAN FOR PLAYERS 3 AND 4 (log only - nothing in the game changes). [hud34] lines.
// ---------------------------------------------------------------------------------------------------------
// What the static map found (GOG 1.3), and what 20g-r2 is planned to do with it:
//   Each of the 8 HUD displays (Boot, Capture, Map, Reticule, Scope, StatIcon, Status, Team) is a Thread (the
//   game's cooperative task class: vtable slot 0 the deleting destructor, slot 1 Update(dt), slot 2 AddToLayer
//   (priority, list), slot 3 RemoveFromLayer, slot 4 InLayer - its list node at +4) kept in a static [2] array, with
//   four functions of the same shape each:
//     CREATE  (the match load's HUD sequence, GOG 1.3 0x0013D795 ...): for both slots new(size) + its constructor;
//     DESTROY (the match teardown, GOG 1.3 0x00146840: Map, Reticule, StatIcon, Status, Team one after the other at
//             0x001468ED; Capture's and Scope's are never called): for both slots the deleting destructor (its base
//             destructor takes the display out of its task list, its elements leave their roots);
//     RESET   (at match start, GOG 1.3 0x0013DC53 ...): for both slots the display's reset, then a tail jump to
//     SETUP   (the per-camera loop): for i < the camera count, display[i]->Setup(i) - player +18h = i, root index +1Ch
//             = CameraManager::RootIndex(i) ([cm + 6Ch + i * 4], the HUD root of camera i's view), the display's
//             groups AddToRoot'ed there, then AddToLayer: from then on the game updates it every frame by itself.
//   Each Update reads the player through CharIndex(+18h) (the network local slots - 4 since 20c) and, for Reticule /
//   Scope, the camera manager's IsChase(i) ([cm + 24h + i*4] == [cm + 34h + i*4]) and GetChase(i) ([cm + 34h + i*4]),
//   and Reticule the Map display getter. RootIndex, IsChase and GetChase read the camera manager's 2-slot arrays -
//   for i = 2 / 3 past them (RootIndex(2) = [cm + 74h], a float of the next array). Each has one jump thunk (19, 5
//   and 4 call sites through it).
//   20g-r3 (done - the HUD section below; Scope left out): each CREATE's thunk through BF1GameExt (the game's two,
//   then two more of the same class for players 3 and 4, in the same heap); each SETUP loop's thunk through BF1GameExt (the game's loop, then reset +
//   Setup(2 / 3) on ours); RootIndex / IsChase / GetChase answering 2 / 3 themselves (their quarters' roots, the
//   chase cameras 20e-r2 made); the six display getters answering ours for players 3 and 4 (instead of 20d-r3's
//   lent displays); the Map DESTROY's thunk (the first display destroyed at teardown) deleting ours first; views 3
//   and 4's HUD roots drawn.
// This scan finds all of it by pattern on the running exe and byte-checks it (each function's body, its one thunk,
// the calls through it, the teardown order), and logs the live displays in the 45 s / 3 min tables: each slot's
// class, player (+18h), root index (+1Ch) and whether it is in a task list (+4).
const char* const kHudCreateFmt = "56 BE %s 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 09 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 89 06 "
                                  "83 C6 04 81 FE %s 7C D7";
constexpr uint32_t kHudCreateSize = 0x07;
constexpr uint32_t kHudCreateNew = 0x0B;
constexpr uint32_t kHudCreateCtor = 0x19;
const char* const kHudDestroyFmt = "56 BE %s 8B 0E 85 C9 74 0C 8B 01 6A 01 FF 10 C7 06 00 00 00 00 83 C6 04 81 FE %s 7C E3";
const char* const kHudResetFmt = "56 BE %s 8B 0E E8 ?? ?? ?? ?? 83 C6 04 81 FE %s 7C EE 5E E9";
constexpr uint32_t kHudResetCall = 0x08;
constexpr uint32_t kHudResetTail = 0x19;
constexpr uint32_t kHudLoopSetupCall = 0x1B;    // the per-camera loop's "push esi; call Setup" (kHudLoop)
const char* const kGameNewBody = "55 8B EC FF 75 08 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 5D C3";
const char* const kRootIndexBody = "55 8B EC 8B 45 08 8B 44 81 6C 5D C2 04 00";
const char* const kIsChaseBody = "55 8B EC 8B 55 08 8B 44 91 24 3B 44 91 34 0F 94 C0 5D C2 04 00";
const char* const kGetChaseBody = "55 8B EC 8B 45 08 8B 44 81 34 5D C2 04 00";
// the teardown destroys Map, Reticule, StatIcon, Status and Team one call after the other (GOG 1.3 0x001468ED)
const int kTeardownOrder[5] = { 2, 3, 5, 6, 7 };

struct Hud34Type
{
    uint32_t create = 0;
    uint32_t createThunk = 0;
    int createCalls = 0;
    uint32_t size = 0;
    uint32_t ctor = 0;
    uint32_t newFn = 0;
    uint32_t destroy = 0;
    uint32_t destroyThunk = 0;
    int destroyCalls = 0;
    uint32_t destroySite = 0;
    uint32_t resetLoop = 0;
    uint32_t reset = 0;
    uint32_t loop = 0;
    uint32_t loopThunk = 0;
    uint32_t setup = 0;
    uint32_t setupRootAt = 0; // the offset of Setup's RootIndex call (0: none found)
    bool ok = false;
};
Hud34Type g_h34[8];
uint32_t g_rootIndexRva = 0;
uint32_t g_rootIndexThunk = 0; // 21j-r34: RootIndex's jump thunk (the game's calls go through it)
uint32_t g_isChaseRva = 0;
uint32_t g_getChaseRva = 0;
bool g_hud34Ok = false; // everything 20g-r2 needs was found and checked

// "D0 CB 9E 00" - an address as pattern bytes
void PatBytes(uint32_t va, char* out, size_t n)
{
    snprintf(out, n, "%02X %02X %02X %02X", va & 0xFF, (va >> 8) & 0xFF, (va >> 16) & 0xFF, va >> 24);
}

// a camera manager getter: its body (one hit), its one thunk and the calls through it
uint32_t FindCamGetter(const char* name, const char* body, int* calls, uint32_t* thunkOut = nullptr)
{
    *calls = 0;
    if (thunkOut)
        *thunkOut = 0;
    uint32_t hits[4] = {};
    const int n = FindPattern(body, hits, 4);
    uint32_t thunks[4] = {};
    int direct = 0;
    const int nt = n == 1 ? FindThunks(hits[0], thunks, 4, &direct) : 0;
    *calls = nt == 1 ? CountCallsTo(thunks[0]) : 0;
    const bool ok = n == 1 && nt == 1 && direct == 0 && *calls > 0;
    if (ok && thunkOut)
        *thunkOut = thunks[0]; // 21j-r34: its own thunk (before InstallHud34 moves it - a call's target is checked against it)
    Log("[hud34] CameraManager::%s: %d pattern hit(s) rva 0x%08X, %d jump thunk(s) rva 0x%08X with %d call(s) through it, %d direct "
        "call(s) - %s",
        name, n, n == 1 ? hits[0] : 0u, nt, nt ? thunks[0] : 0u, *calls, direct,
        ok ? "OK (r2 can answer players 3 and 4 through its thunk)" : "does NOT check out");
    return ok ? hits[0] : 0;
}

void FindHud34()
{
    int ok = 0;
    uint32_t newFn = 0;
    bool sameNew = true;
    int rootCalls = 0;
    int chaseCalls = 0;
    int getChaseCalls = 0;
    g_rootIndexRva = FindCamGetter("RootIndex(i) ([cm + 6Ch + i*4], the HUD root of camera i's view)", kRootIndexBody, &rootCalls,
                                   &g_rootIndexThunk);
    g_isChaseRva = FindCamGetter("IsChase(i) ([cm + 24h + i*4] == [cm + 34h + i*4])", kIsChaseBody, &chaseCalls);
    g_getChaseRva = FindCamGetter("GetChase(i) ([cm + 34h + i*4])", kGetChaseBody, &getChaseCalls);
    for (int d = 0; d < 8; ++d)
    {
        Hud34Type& h = g_h34[d];
        if (!g_hudArray[d])
        {
            Log("[hud34] %s: its array was not found (the (9) lines) - NOT checked", kHudName[d]);
            continue;
        }
        const uint32_t va = static_cast<uint32_t>(Rva(g_hudArray[d]));
        char a[16];
        char b[16];
        char pat[256];
        PatBytes(va, a, sizeof(a));
        PatBytes(va + 8, b, sizeof(b));
        uint32_t hits[4] = {};
        // create
        snprintf(pat, sizeof(pat), kHudCreateFmt, a, b);
        int n = FindPattern(pat, hits, 4);
        if (n == 1)
        {
            h.create = hits[0];
            h.size = CodeU32(h.create + kHudCreateSize);
            h.newFn = CallTarget(h.create + kHudCreateNew);
            h.ctor = CallTarget(h.create + kHudCreateCtor);
            uint32_t thunks[4] = {};
            int direct = 0;
            if (FindThunks(h.create, thunks, 4, &direct) == 1 && direct == 0)
            {
                h.createThunk = thunks[0];
                h.createCalls = CountCallsTo(thunks[0]);
            }
        }
        const bool newOk = h.newFn && MatchPatternAt(h.newFn, kGameNewBody);
        if (newOk)
        {
            if (!newFn)
                newFn = h.newFn;
            else if (newFn != h.newFn)
                sameNew = false;
        }
        // destroy
        snprintf(pat, sizeof(pat), kHudDestroyFmt, a, b);
        n = FindPattern(pat, hits, 4);
        if (n == 1)
        {
            h.destroy = hits[0];
            uint32_t thunks[4] = {};
            int direct = 0;
            if (FindThunks(h.destroy, thunks, 4, &direct) == 1 && direct == 0)
            {
                h.destroyThunk = thunks[0];
                h.destroyCalls = CountCallsTo(thunks[0]);
            }
        }
        // reset, its tail jump to the per-camera loop, the loop's Setup call
        snprintf(pat, sizeof(pat), kHudResetFmt, a, b);
        n = FindPattern(pat, hits, 4);
        if (n == 1)
        {
            h.resetLoop = hits[0];
            h.reset = CallTarget(h.resetLoop + kHudResetCall);
            h.loop = CallTarget(h.resetLoop + kHudResetTail);
            if (h.loop && MatchPatternAt(h.loop, kHudLoop) && OperandRva(h.loop + kHudLoopArray) == g_hudArray[d])
            {
                h.setup = CallTarget(h.loop + kHudLoopSetupCall);
                uint32_t thunks[4] = {};
                int direct = 0;
                if (FindThunks(h.loop, thunks, 4, &direct) == 1 && direct == 0)
                    h.loopThunk = thunks[0];
            }
            else
                h.loop = 0;
        }
            // Setup's RootIndex call (in its first 60h bytes). 20g-r2: an E8 byte counts only if its rel32 lands in the code
        // section (a call's does; an E8 inside an address or immediate almost never does), and the thunk hop is bounded.
        {
            uint32_t codeRva = 0;
            uint32_t codeSize = 0;
            const bool haveCode = CodeSection(&codeRva, &codeSize);
            for (uint32_t i = 0; haveCode && h.setup && g_rootIndexRva && i < 0x60 && !h.setupRootAt; ++i)
            {
                if (CodeByte(h.setup + i) != 0xE8)
                    continue;
                const uint32_t t = RelTarget(h.setup + i);
                if (t < codeRva || t - codeRva >= codeSize)
                    continue;
                if (CallTarget(h.setup + i) == g_rootIndexRva)
                    h.setupRootAt = i;
            }
        }
        h.ok = h.create && h.createThunk && h.createCalls == 1 && newOk && h.size >= 0x20 && h.size < 0x40000 && h.ctor &&
               h.resetLoop && h.reset && h.loop && h.loopThunk && h.setup && h.setupRootAt;
        if (h.ok)
            ++ok;
        Log("[hud34] %-15s [2] rva 0x%08X: create rva 0x%08X (new(0x%X) rva 0x%08X %s, constructor rva 0x%08X; its thunk rva 0x%08X, "
            "%d call(s)), destroy rva 0x%08X (thunk rva 0x%08X, %d call(s)), reset loop rva 0x%08X (reset rva 0x%08X) -> per-camera "
            "loop rva 0x%08X (thunk rva 0x%08X) -> Setup rva 0x%08X (RootIndex call at +%Xh) - %s",
            kHudName[d], g_hudArray[d], h.create, h.size, h.newFn, newOk ? "OK" : "NOT the game's new", h.ctor, h.createThunk,
            h.createCalls, h.destroy, h.destroyThunk, h.destroyCalls, h.resetLoop, h.reset, h.loop, h.loopThunk, h.setup,
            h.setupRootAt, h.ok ? "OK" : "does NOT check out");
    }
    // the teardown: Map, Reticule, StatIcon, Status, Team destroyed one call after the other
    bool teardownOk = false;
    uint32_t site = 0;
    if (g_h34[2].destroyThunk && g_h34[2].destroyCalls == 1)
    {
        uint32_t codeRva = 0;
        uint32_t codeSize = 0;
        if (CodeSection(&codeRva, &codeSize))
        {
            const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
            for (uint32_t i = 0; i + 5 <= codeSize && !site; ++i)
                if (p[i] == 0xE8 && RelTarget(codeRva + i) == g_h34[2].destroyThunk)
                    site = codeRva + i;
        }
        teardownOk = site != 0;
        for (int k = 1; k < 5 && teardownOk; ++k)
        {
            const uint32_t s = site + static_cast<uint32_t>(k) * 5;
            const Hud34Type& t = g_h34[kTeardownOrder[k]];
            teardownOk = CodeByte(s) == 0xE8 && t.destroy && CallTarget(s) == t.destroy;
        }
        g_h34[2].destroySite = site;
    }
    Log("[hud34] the teardown: MapDisplay's destroy is called once (rva 0x%08X) and Reticule, StatIcon, Status and Team's right after "
        "it %s; Capture's and Scope's destroys are called %d / %d time(s) (GOG 1.3: never)",
        site, teardownOk ? "- OK (r2 deletes players 3 and 4's displays there, through the Map destroy's thunk)" : "- does NOT check out",
        g_h34[1].destroyCalls, g_h34[4].destroyCalls);
    g_hud34Ok = ok == 8 && sameNew && teardownOk && g_rootIndexRva && g_isChaseRva && g_getChaseRva;
    Log("[hud34] HUD SCAN: %d of 8 displays check out, %s; RootIndex / IsChase / GetChase %s (%d / %d / %d calls through their "
        "thunks; GOG 1.3: 19 / 5 / 4) - %s",
        ok, sameNew ? "all made with the same new" : "NOT all with the same new",
        (g_rootIndexRva && g_isChaseRva && g_getChaseRva) ? "found" : "NOT all found", rootCalls, chaseCalls, getChaseCalls,
        g_hud34Ok ? "EVERYTHING 20g-r2 NEEDS IS HERE" : "something is missing (lines above)");
}

void ReportHud34Own(); // 20g-r3 (below): players 3 and 4's own displays

// the live displays, in the tables (reporter thread, read with ReadProcessMemory)
void ReportHud34()
{
    for (int d = 0; d < 8; ++d)
    {
        if (!g_hudArray[d])
            continue;
        const uintptr_t arr = Rva(g_hudArray[d]);
        char text[2][160] = {};
        for (int i = 0; i < 2; ++i)
        {
            const uint32_t obj = ReadU32(arr + static_cast<uintptr_t>(i) * 4);
            char cls[64];
            ClassName(obj, cls, sizeof(cls));
            if (!obj)
                snprintf(text[i], sizeof(text[i]), "none");
            else
                snprintf(text[i], sizeof(text[i]), "0x%08X %s (vtable rva 0x%08X): player +18h %d, root index +1Ch %d, task list +4 %s",
                         obj, cls[0] ? cls : "no RTTI", RvaOf(ReadU32(obj)), static_cast<int>(ReadU32(obj + 0x18)),
                         static_cast<int>(ReadU32(obj + 0x1C)), ReadU32(obj + 4) ? "yes" : "no");
        }
        Log("[hud34] table: %-15s [0] %s | [1] %s", kHudName[d], text[0], text[1]);
    }
    if (g_camMgrPtrRva)
    {
        const uintptr_t cm = ReadU32(Rva(g_camMgrPtrRva));
        if (cm)
            Log("[hud34] table: what the camera manager answers today: RootIndex(0..3) %d %d | %d %d (2 / 3 read past its [2]); "
                "IsChase(0..1) %s %s; GetChase(0..3) 0x%08X 0x%08X | 0x%08X 0x%08X; views 3 / 4 are renderer views %d / %d",
                static_cast<int>(ReadU32(cm + 0x6C)), static_cast<int>(ReadU32(cm + 0x70)), static_cast<int>(ReadU32(cm + 0x74)),
                static_cast<int>(ReadU32(cm + 0x78)), ReadU32(cm + 0x24) == ReadU32(cm + 0x34) ? "yes" : "no",
                ReadU32(cm + 0x28) == ReadU32(cm + 0x38) ? "yes" : "no", ReadU32(cm + 0x34), ReadU32(cm + 0x38), ReadU32(cm + 0x3C),
                ReadU32(cm + 0x40), PlayerNSpecView(0), PlayerNSpecView(1));
    }
    ReportHud34Own(); // 20g-r3: players 3 and 4's own displays
}

void ReportTables(const char* when)
{
    Log("[playerN] tables (%s): local player count %d; player 2's input redirect %s", when, LocalCount(),
        Player2RedirectActive() ? "ON" : "off");

    // (2) the IF managers
    if (g_ifPtrRva)
    {
        const uintptr_t base = ReadU32(Rva(g_ifPtrRva));
        Log("[playerN] table: (2) IF managers pointer (rva 0x%08X) -> 0x%08X (rva 0x%08X)%s", g_ifPtrRva,
            static_cast<uint32_t>(base), RvaOf(base), RvaOf(base) == g_ifArrayRva ? "" : " - MOVED off the static array");
        for (int m = 0; m < 2 && base; ++m)
        {
            const uintptr_t mgr = base + static_cast<uintptr_t>(m) * kIfManagerSize;
            char top[33] = {};
            ReadMem(mgr + 0x34, top, sizeof(top) - 1);
            top[sizeof(top) - 1] = 0;
            Log("[playerN] table: (2)   manager %d at 0x%08X: top \"%s\", depth %u, player +18Ch %d, index +190h %d, "
                "countdown +194h %d",
                m, static_cast<uint32_t>(mgr), top, ReadU32(mgr + 0x178), static_cast<int>(ReadU32(mgr + 0x18C)),
                static_cast<int>(ReadU32(mgr + 0x190)), static_cast<int>(ReadU32(mgr + 0x194)));
        }
        if (base)
            LogDwords("(2) what a manager 2 would be (the 0x10 bytes after manager 1)", base + 2 * kIfManagerSize, 4);
        if (g_ifIndexRva)
            Log("[playerN] table: (2) the Lua's current manager index (rva 0x%08X): %d", g_ifIndexRva,
                static_cast<int>(ReadU32(Rva(g_ifIndexRva))));
    }

    // (3) / (4) the network's local slots, the control states
    if (g_netSlotsRva)
        LogDwords("(3) net local slots int[2] + the 2 dwords after", Rva(g_netSlotsRva), 4);
    if (g_controlRva)
    {
        char a[96];
        char b[96];
        char c[96];
        char d[96];
        const uintptr_t t = Rva(g_controlRva);
        Describe(ReadU32(t), a, sizeof(a));
        Describe(ReadU32(t + 4), b, sizeof(b));
        Describe(ReadU32(t + 8), c, sizeof(c));
        Describe(ReadU32(t + 12), d, sizeof(d));
        uint8_t online = 0xFF;
        ReadMem(Rva(g_onlineRva), &online, 1);
        Log("[playerN] table: (4) control states (rva 0x%08X): [0] %s [1] %s [2] %s [3] %s; online flag %u (0 = the "
            "index is ANDed with 1)", g_controlRva, a, b, c, d, static_cast<unsigned>(online));
    }

    // (5) the pause states
    if (g_pausePtrRva)
    {
        const uintptr_t arr = ReadU32(Rva(g_pausePtrRva));
        Log("[playerN] table: (5) pause pointer (rva 0x%08X) -> rva 0x%08X%s", g_pausePtrRva, RvaOf(arr),
            RvaOf(arr) == g_pauseArrayRva ? "" : " - MOVED");
        for (int p = 0; p < 3 && arr; ++p)
        {
            char label[80];
            snprintf(label, sizeof(label), p < 2 ? "(5)   pause state %d (9 dwords)" : "(5)   what a pause state %d would be",
                     p);
            LogDwords(label, arr + static_cast<uintptr_t>(p) * kPauseSize, 9);
        }
    }

    // (6) input
    if (g_inputMgrRva)
    {
        const uintptr_t mgr = Rva(g_inputMgrRva);
        char cls[64];
        ClassName(mgr, cls, sizeof(cls));
        Log("[playerN] table: (6) input manager (rva 0x%08X, %s): player count +14h %u; its own player input +1Ch, "
            "buffer A +734h, +748h, buffer B +77Ch - one slot each",
            g_inputMgrRva, cls[0] ? cls : "no RTTI", ReadU32(mgr + 0x14));
    }

    // (7) local slots, profiles
    if (g_slotsRva)
        for (int p = 0; p < 3; ++p)
        {
            char label[80];
            snprintf(label, sizeof(label), p < 2 ? "(7) local slot %d (12 dwords)" : "(7) what a local slot %d would be", p);
            LogDwords(label, Rva(g_slotsRva) + static_cast<uintptr_t>(p) * kSlotSize, 12);
        }
    if (g_profilesRva)
        for (int p = 0; p < 2; ++p)
        {
            char label[80];
            snprintf(label, sizeof(label), "(7) profile %d (first 8 dwords)", p);
            LogDwords(label, Rva(g_profilesRva) + static_cast<uintptr_t>(p) * kProfileSize, 8);
        }

    // (8) the camera manager and the renderer's views
    if (g_camMgrPtrRva)
    {
        const uintptr_t cm = ReadU32(Rva(g_camMgrPtrRva));
        char cls[64];
        ClassName(cm, cls, sizeof(cls));
        Log("[playerN] table: (8) CameraManager 0x%08X (%s): camera count +20h %d", static_cast<uint32_t>(cm),
            cls[0] ? cls : "no RTTI", cm ? static_cast<int>(ReadU32(cm + kCamCount)) : -1);
        if (cm)
        {
            static const struct
            {
                uint32_t ofs;
                const char* name;
            } kFields[] = {
                { 0x2C, "+2Ch RedCamera" }, { 0x34, "+34h ChaseCamera" }, { 0x3C, "+3Ch FreeCamera" },
                { 0x44, "+44h DeathCamera" }, { 0x4C, "+4Ch view object" },
            };
            for (const auto& f : kFields)
            {
                char a[96];
                char b[96];
                char c[96];
                Describe(ReadU32(cm + f.ofs), a, sizeof(a));
                Describe(ReadU32(cm + f.ofs + 4), b, sizeof(b));
                Describe(ReadU32(cm + f.ofs + 8), c, sizeof(c));
                Log("[playerN] table: (8)   %s: [0] %s, [1] %s | a camera 2's would be the next field: %s", f.name, a, b, c);
            }
            Log("[playerN] table: (8)   +6Ch renderer view / HUD root index: [0] %d, [1] %d | after: %d, %d",
                static_cast<int>(ReadU32(cm + 0x6C)), static_cast<int>(ReadU32(cm + 0x70)),
                static_cast<int>(ReadU32(cm + 0x74)), static_cast<int>(ReadU32(cm + 0x78)));
            LogDwords("(8)   CameraManager +18h..+2Bh (per-camera floats / +24h)", cm + 0x18, 5);
        }
    }
    if (g_viewCountRva && g_viewCamerasRva)
    {
        const uintptr_t cams = Rva(g_viewCamerasRva);
        Log("[playerN] table: (8) renderer views: count %u; cameras [4] %08X %08X %08X %08X",
            ReadU32(Rva(g_viewCountRva)), ReadU32(cams), ReadU32(cams + 4), ReadU32(cams + 8), ReadU32(cams + 12));
        float rects[16] = {};
        if (ReadMem(cams - 0x40, rects, sizeof(rects)))
            Log("[playerN] table: (8) renderer view rects [4] (x y w h): %.3f %.3f %.3f %.3f | %.3f %.3f %.3f %.3f | "
                "%.3f %.3f %.3f %.3f | %.3f %.3f %.3f %.3f",
                rects[0], rects[1], rects[2], rects[3], rects[4], rects[5], rects[6], rects[7], rects[8], rects[9],
                rects[10], rects[11], rects[12], rects[13], rects[14], rects[15]);
    }

    // (9) the HUD
    for (int d = 0; d < 8; ++d)
    {
        char label[64];
        snprintf(label, sizeof(label), "(9) %s [2]", kHudName[d]);
        LogPointerPair(label, g_hudArray[d]);
    }
    if (g_messageRva)
    {
        char a[96];
        char b[96];
        char c[96];
        char d[96];
        char e[96];
        const uintptr_t m = Rva(g_messageRva);
        Describe(ReadU32(m), a, sizeof(a));
        Describe(ReadU32(m + 4), b, sizeof(b));
        Describe(ReadU32(m + 8), c, sizeof(c));
        Describe(ReadU32(m + 12), d, sizeof(d));
        Describe(ReadU32(m + 16), e, sizeof(e));
        Log("[playerN] table: (9) MessageDisplay [2][2] (rva 0x%08X): %s, %s | %s, %s | after: %s", g_messageRva, a, b, c,
            d, e);
    }
    LogPointerPair("(9) display thread A [2] (GOG 1.3: MineDisplayThread)", g_threadRva[0]);
    LogPointerPair("(9) display thread B [2] (GOG 1.3: NameDisplayThread)", g_threadRva[1]);
    ReportHud34(); // 20g-r1

    // (10) first person
    if (g_firstPersonRva)
        for (int p = 0; p < 3; ++p)
        {
            char label[80];
            snprintf(label, sizeof(label), p < 2 ? "(10) first person %d (8 dwords)" : "(10) what a first person %d would be",
                     p);
            LogDwords(label, Rva(g_firstPersonRva) + static_cast<uintptr_t>(p) * kFirstPersonSize, 8);
        }

    // (11) the other static arrays: first 4 dwords of each slot, and of what follows the last
    for (int i = 0; i < g_staticCount; ++i)
    {
        const StaticArray& a = g_static[i];
        if (a.count < 2 || a.size == kIfManagerSize || a.size == kPauseSize || a.size == kSlotSize ||
            a.size == kProfileSize || a.size == kFirstPersonSize)
            continue;
        for (uint32_t k = 0; k <= a.count; ++k)
        {
            char label[96];
            snprintf(label, sizeof(label), k < a.count ? "(11) rva 0x%08X slot %u" : "(11) rva 0x%08X after slot %u",
                     a.rva, k < a.count ? k : a.count - 1);
            LogDwords(label, Rva(a.rva) + static_cast<uintptr_t>(k) * a.size, a.size < 16 ? static_cast<int>(a.size / 4) : 4);
        }
    }

    // the characters: the local players' (index 0-3) and the first AI
    uintptr_t chars = 0;
    uint32_t count = 0;
    if (SpawnScanCharacters(&chars, &count) && chars)
    {
        int shown = 0;
        for (uint32_t c = 0; c < count && c < 64 && shown < 6; ++c)
        {
            const uintptr_t ch = chars + static_cast<uintptr_t>(c) * 0xE8;
            const int index = static_cast<int>(ReadU32(ch + 0x9C));
            if (index < 0 && c > 4)
                continue;
            uint16_t wide[32] = {};
            ReadMem(ch + 0x1C, wide, sizeof(wide) - 2);
            char name[33] = {};
            for (int w = 0; w < 31 && wide[w]; ++w)
                name[w] = wide[w] < 0x80 ? static_cast<char>(wide[w]) : '?';
            Log("[playerN] table: character %u: index %d, team %d, unit 0x%08X, \"%s\"", c, index,
                static_cast<int>(ReadU32(ch + 0xA0)), ReadU32(ch + 0xB4), name);
            ++shown;
        }
    }
    else
        Log("[playerN] table: characters: none (no match running)");
}

// ---------------------------------------------------------------------------------------------------------
// The reporter: 45 s and 3 min into a split match, and at its end
// ---------------------------------------------------------------------------------------------------------
DWORD WINAPI ReporterThread(LPVOID)
{
    bool inMatch = false;
    DWORD start = 0;
    int reports = 0;
    unsigned long matches = 0;
    for (;;)
    {
        Sleep(500);
        const bool split = SpawnScanManager() != 0 && LocalCount() >= 2;
        if (split && !inMatch)
        {
            inMatch = true;
            start = GetTickCount();
            reports = 0;
            ++matches;
            ResetCensus();
            Log("[playerN] %lu ms: split screen match %lu started (%d local players) - census reset; tables and census "
                "45 s in, tables again at 3 min, census again at its end",
                ScanMsSinceAttach(), matches, LocalCount());
        }
        else if (!split && inMatch)
        {
            inMatch = false;
            char when[64];
            snprintf(when, sizeof(when), "end of split match %lu", matches);
            ReportCensus(when);
        }
        if (!inMatch)
            continue;
        const DWORD in = GetTickCount() - start;
        if (reports == 0 && in >= 45000)
        {
            reports = 1;
            ReportTables("45 s into the split match");
            ReportCensus("45 s into the split match");
        }
        else if (reports == 1 && in >= 180000)
        {
            reports = 2;
            ReportTables("3 min into the split match");
        }
    }
}

// ---------------------------------------------------------------------------------------------------------
// Phase 20b: the drawing side to 4 views ([Splitscreen] Views=4)
// ---------------------------------------------------------------------------------------------------------
// GOG 1.3 bodies of the game functions used (each found once, by pattern):
const char* const kPatCamCtor = // RedCamera ctor (thiscall): vector ctor of 5 x 0Ch at +D4h, vtable, projection
    "56 8B F1 68 ?? ?? ?? ?? 6A 05 6A 0C 8D 86 D4 00 00 00 C7 06 ?? ?? ?? ?? 50 E8";
const char* const kPatCamParams = // RedCamera projection (thiscall: near, far, fov, ratio; ret 10h)
    "55 8B EC F3 0F 10 45 10 F3 0F 59 05 ?? ?? ?? ?? 56 51 8B F1 F3 0F 11 04 24 E8";
const char* const kPatCamMatrix = // RedCamera set matrix (thiscall: const float[16]; ret 4) - rows at +10h..+40h
    "55 8B EC 8B 45 08 0F 10 00 0F 11 41 10 0F 10 40 10 0F 11 41 20 0F 10 40 20 0F 11 41 30 0F 10 40 30 0F 11 41 "
    "40 E8";
const char* const kPatViewFree = // free a view (cdecl: view): camera slot = 0, count trimmed
    "55 8B EC 8B 45 08 C7 04 85 ?? ?? ?? ?? 00 00 00 00 A1 ?? ?? ?? ?? 85 C0 74 19";
constexpr uint32_t kViewFreeCams = 0x09;
const char* const kViewAllocHead = "55 8B EC 51"; // the view alloc (cdecl: rect, camera), 4 bytes before kViewAlloc
// the view loop's head in the frame function: mov [current view], esi; cmp [count], esi; jbe
const char* const kPatViewLoop = "89 35 ?? ?? ?? ?? 39 35 ?? ?? ?? ?? 0F 86";
const char* const kFrameProlog = "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04"; // the frame function's start
const char* const kFldImmRet = "D9 05 ?? ?? ?? ?? C3"; // the screen shape getter: fld [ratio]; ret

constexpr uint32_t kRedCameraSize = 0x120;
constexpr uint32_t kCamMatrix = 0x10;      // RedCamera: 4 rows of 4 floats (right, up, forward, position)
constexpr uint32_t kCamCameras = 0x2C;     // CameraManager: RedCamera [2]
constexpr uint32_t kCamViews = 0x6C;       // CameraManager: renderer view [2] (= HUD root index)
constexpr float kNear = 0.7f;              // what SetNumCameras gives its cameras (0x3F333333)
constexpr float kFar = 120.0f;             // (0x42F00000)
constexpr uint32_t kPcFovBits = 0x3F567750u; // 48 deg, the PC's own (vertical.cpp's note)

typedef int(__cdecl* ViewAlloc_t)(const float* rect, uintptr_t camera);
typedef void(__cdecl* ViewFree_t)(int view);
typedef void*(__fastcall* CamCtor_t)(void* camera, void* edx);
typedef void(__fastcall* CamParams_t)(void* camera, void* edx, float nearZ, float farZ, float fov, float ratio);
typedef void(__fastcall* CamMatrix_t)(void* camera, void* edx, const float* m);
typedef void(__fastcall* SetNumCameras_t)(void* mgr, void* edx, int count);
typedef void(__cdecl* Frame_t)();

int g_viewsWanted = 2;           // [Splitscreen] Views
bool g_viewsReady = false;       // everything found and both thunks moved
ViewAlloc_t g_viewAlloc = nullptr;
ViewFree_t g_viewFree = nullptr;
CamCtor_t g_camCtor = nullptr;
CamParams_t g_camParams = nullptr;
CamMatrix_t g_camMatrix = nullptr;
SetNumCameras_t g_origSetNum = nullptr;
uint32_t g_setNumRva = 0;
uint32_t g_setNumThunk = 0; // 20c: its jump thunk (calls go to the thunk; the thunk goes to us)
Frame_t g_origFrame = nullptr;
uintptr_t g_ratioVa = 0;         // float: the screen's height / width (the getter SetNumCameras calls)
uintptr_t g_rendCams = 0;        // renderer cameras [4]; rects [4][4] at -40h, current +10h, count +14h,
                                 // scene flags +40h [4], HUD root flags +44h [4], flags +48h [4]
alignas(16) unsigned char g_specCam[2][kRedCameraSize + 0x20] = {}; // two RedCameras of ours (0x120 bytes each)
bool g_specMade[2] = {};
int g_specView[2] = { -1, -1 };
uintptr_t g_quarterMgr = 0;      // the camera manager the quarters were made for
volatile LONG g_specFrames = 0;
int g_viewLines = 0;
// 20c: the game sets 1 camera and back to 2 about 270 times a match (GOG 1.3 0x00163CE0) - only the first 6 calls
// and every 100th are logged.
volatile LONG g_setNumCalls = 0;
bool g_viewsQuiet = false;

// x, y, w, h: player 1 top left, player 2 top right, views 3 and 4 below (the Xbox's 4-player layout)
alignas(16) const float kQuarter[4][4] = {
    { 0.0f, 0.0f, 0.5f, 0.5f }, { 0.5f, 0.0f, 0.5f, 0.5f }, { 0.0f, 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f, 0.5f },
};
// 20g-r13: Views=3 - player 3's view: the full-width bottom half (the game's own 2-player bottom rect, GOG 1.3 0x007DAA70)
alignas(16) const float kHalfBottom[4] = { 0.0f, 0.5f, 1.0f, 0.5f };

float PcFov()
{
    float f = 0.0f;
    memcpy(&f, &kPcFovBits, sizeof(f));
    return f;
}

uintptr_t RendCamera(int view)
{
    return (view >= 0 && view < 4) ? ReadU32(g_rendCams + static_cast<uintptr_t>(view) * 4) : 0;
}

uint8_t* RendFlag(int table, int view) // table 0 = scene, 1 = HUD root, 2 = the third flag
{
    return reinterpret_cast<uint8_t*>(g_rendCams + 0x40 + static_cast<uintptr_t>(table) * 4 + static_cast<uintptr_t>(view));
}

uintptr_t CameraManager()
{
    return g_camMgrPtrRva ? ReadU32(Rva(g_camMgrPtrRva)) : 0;
}

void LogViews(const char* when)
{
    if (g_viewLines >= 200 || g_viewsQuiet)
        return;
    ++g_viewLines;
    Log("[views4] %s: renderer views %u - cameras %08X %08X %08X %08X, scene flags %u %u %u %u, HUD root flags %u %u "
        "%u %u; spectator views %d %d",
        when, ReadU32(g_rendCams + 0x14), RendCamera(0), RendCamera(1), RendCamera(2), RendCamera(3), *RendFlag(0, 0),
        *RendFlag(0, 1), *RendFlag(0, 2), *RendFlag(0, 3), *RendFlag(1, 0), *RendFlag(1, 1), *RendFlag(1, 2),
        *RendFlag(1, 3), g_specView[0], g_specView[1]);
}

// Views 3 and 4 out (only slots that still hold our cameras).
void Listener34Off(int k, const char* why); // 20g-r6 (below)

void FreeSpectators(const char* why)
{
    Listener34Off(0, why); // 20g-r6: their listeners follow views 3 / 4
    Listener34Off(1, why);
    bool any = false;
    for (int k = 0; k < 2; ++k)
    {
        const int v = g_specView[k];
        if (v < 0)
            continue;
        if (RendCamera(v) == reinterpret_cast<uintptr_t>(g_specCam[k]))
        {
            g_viewFree(v);
            any = true;
        }
        g_specView[k] = -1;
    }
    g_quarterMgr = 0;
    if (any)
    {
        char when[96];
        snprintf(when, sizeof(when), "views 3 and 4 taken out (%s)", why);
        LogViews(when);
    }
}

// After the game's own SetNumCameras(2): the two players' views made again as quarters, then views 3 and 4.
void MakeQuarters(uintptr_t mgr)
{
    const float ratio = g_ratioVa ? *reinterpret_cast<const float*>(g_ratioVa) : 0.75f;
    for (int i = 0; i < 2; ++i)
    {
        const int v = static_cast<int>(ReadU32(mgr + kCamViews + static_cast<uintptr_t>(i) * 4));
        const uintptr_t cam = ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(i) * 4);
        if (v < 0 || v > 3 || !cam || RendCamera(v) != cam)
        {
            Log("[views4] quarters NOT made: player %d's view %d / camera %08X does not match the renderer's (%08X)",
                i + 1, v, static_cast<uint32_t>(cam), RendCamera(v));
            return;
        }
    }
    for (int i = 0; i < 2; ++i)
    {
        uintptr_t* slot = reinterpret_cast<uintptr_t*>(mgr + kCamViews + static_cast<uintptr_t>(i) * 4);
        const int v = static_cast<int>(*slot);
        const uintptr_t cam = ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(i) * 4);
        const uint8_t f0 = *RendFlag(0, v);
        const uint8_t f1 = *RendFlag(1, v);
        const uint8_t f2 = *RendFlag(2, v);
        g_viewFree(v);
        g_camParams(reinterpret_cast<void*>(cam), nullptr, kNear, kFar, PcFov(), ratio);
        const int nv = g_viewAlloc(kQuarter[i], cam);
        if (nv < 0)
        {
            Log("[views4] player %d's quarter: the renderer had no free view (alloc -1)", i + 1);
            return;
        }
        *RendFlag(0, nv) = f0;
        *RendFlag(1, nv) = f1;
        *RendFlag(2, nv) = f2;
        *slot = static_cast<uintptr_t>(nv);
        if (nv != v)
            Log("[views4] player %d's view moved %d -> %d (written back to the camera manager)", i + 1, v, nv);
    }
    for (int k = 0; k < 2; ++k)
    {
        // 20g-r13: 3-player - view 3 is the bottom half: its camera the shape the game gives a half (ratio x 0.5, as
        // SetNumCameras(2) does for the 2-player halves), no view 4
        const bool half = g_viewsLive == 3; // 21g-r2: this match's layout
        if (half && k == 1)
        {
            g_specView[1] = -1;
            continue;
        }
        void* spec = g_specCam[k];
        if (!g_specMade[k])
        {
            g_camCtor(spec, nullptr);
            g_specMade[k] = true;
        }
        const uintptr_t player = ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(k) * 4);
        g_camMatrix(spec, nullptr, reinterpret_cast<const float*>(player + kCamMatrix));
        g_camParams(spec, nullptr, kNear, kFar, PcFov(), half ? ratio * 0.5f : ratio);
        const int nv = g_viewAlloc(half ? kHalfBottom : kQuarter[2 + k], reinterpret_cast<uintptr_t>(spec));
        g_specView[k] = nv;
        if (nv < 0)
        {
            Log("[views4] view %d: the renderer had no free view (alloc -1)", k + 3);
            continue;
        }
        const int pv = static_cast<int>(ReadU32(mgr + kCamViews + static_cast<uintptr_t>(k) * 4));
        *RendFlag(0, nv) = *RendFlag(0, pv);
        *RendFlag(1, nv) = 0; // no HUD root for a spectator
        *RendFlag(2, nv) = *RendFlag(2, pv);
    }
    g_quarterMgr = mgr;
    // 20g-r13: the views were made again (AddViewport wrote each HUD root's safe area): player 3's half gets Classic's again
    // before anything is drawn (split_spawn_menus.cpp; nothing for quarters)
    if (g_viewsLive == 3)
        P2SpawnHorizontalRoots();
    char when[128];
    snprintf(when, sizeof(when), "%s made (camera manager %08X, screen ratio %.4f)",
             g_viewsLive == 3 ? "3-player views (top quarters, view 3 the bottom half)" : "quarters", static_cast<uint32_t>(mgr),
             static_cast<double>(ratio));
    LogViews(when);
}

// SetNumCameras' jump thunk: views 3 and 4 out first, the game's own, then the quarters for 2 cameras.
void __fastcall SetNumCamerasWrap(void* mgr, void* edx, int count)
{
    (void)edx;
    const LONG call = InterlockedIncrement(&g_setNumCalls);
    g_viewsQuiet = call > 6 && call % 100 != 0;
    if (count > 2)
    {
        // 20c: the camera manager has room for 2 cameras only (its count call sites are kept at 2; this is the
        // safety net)
        if (!g_viewsQuiet)
            Log("[views4] SetNumCameras(%d) asked - the camera manager gets 2", count);
        count = 2;
    }
    FreeSpectators("SetNumCameras");
    g_origSetNum(mgr, nullptr, count);
    if (count == 2 && reinterpret_cast<uintptr_t>(mgr) == CameraManager() && g_viewsLive >= 3)
        MakeQuarters(reinterpret_cast<uintptr_t>(mgr));
    else if (count == 2 && reinterpret_cast<uintptr_t>(mgr) == CameraManager())
    {
        // 21g-r2: 2 players signed in - the game's own 2-player split (horizontal / vertical as BF1GameExt sets it)
        char when[128];
        snprintf(when, sizeof(when), "SetNumCameras(2) call %ld - 2 players signed in: the game's own 2-player split, no quarters",
                 static_cast<long>(call));
        LogViews(when);
    }
    else
    {
        char when[96];
        snprintf(when, sizeof(when), "SetNumCameras(%d) call %ld - the game's own layout", count, static_cast<long>(call));
        LogViews(when);
    }
    g_viewsQuiet = false;
}


// ---------------------------------------------------------------------------------------------------------
// 20g-r6: SOUND FOR PLAYERS 3 AND 4 - LISTENERS OF THEIR OWN
// ---------------------------------------------------------------------------------------------------------
// The sound engine has 4 listener slots (0x38 bytes each, GOG 1.3 0x02153CC0) and plays each 3D sound for the NEAREST
// active one (GOG 1.3 0x00305740: with more than one active it walks all 4 slots). The camera manager takes one per
// camera (its constructor, 0x00094539: find a free slot 0x00305EA0, Enable(on) 0x00094960 - bit 0 + the active count
// 0x00D02820) and every frame points it at its camera (0x000955FC: SetVectors(position, forward, up) 0x00095120 -
// pointers it keeps; the position = the camera's + 2 m along its forward row). So only players 1 and 2 had ears:
// everything near players 3 and 4 was as loud as it was at players 1 / 2. Now each of them gets a free slot of the 4
// (never one of the camera manager's), enabled while his view is drawn, pointed every frame at view 3 / 4's camera
// (its forward / up rows, our own position 2 m ahead) as the camera manager does for 1 and 2; disabled at the match
// teardown. [Splitscreen] SoundP34=1. [sound34] lines.
const char* const kPatCmListenerMake = "E8 ?? ?? ?? ?? 85 DB 89 46 20 0F 94 C1 0F B6 C9 51 8B C8 E8 ?? ?? ?? ?? FF 75 F8 8B 4E 20 E8 ?? "
                                       "?? ?? ?? C7 46 40 FF FF FF FF 43";
constexpr uint32_t kCmListenerEnableCall = 0x13;
const char* const kListenerAllocBody =
    "53 56 57 33 DB BF ?? ?? ?? ?? 33 F6 0F 1F 40 00 8B CF E8 ?? ?? ?? ?? 84 C0 74 15 83 C6 38 43 83 C7 38 81 FE E0 00 00 00 72 E6 "
    "5F 5E 33 C0 5B C3";
constexpr uint32_t kListenerAllocArray = 6;
const char* const kListenerEnableBody =
    "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 80 7D 08 00 74 12 84 C0 75 34 83 0E 01 FF 05 ?? ?? ?? ?? 5E 5D C2 04 00";
const char* const kPatCmListenerSet = "8B 4C B7 4C 8D 04 87 50 E8 ?? ?? ?? ?? 8B 45 D8 8B F7 85 C0 74 0D 8B 4E 7C 50 8B 4C 8E 4C E8";
constexpr uint32_t kCmListenerSetCall = 8;
const char* const kListenerVectorsBody =
    "55 8B EC 8B 45 08 83 EC 0C 85 C0 BA ?? ?? ?? ?? 0F 45 D0 8B 45 0C 56 8B F1 85 C0 B9 ?? ?? ?? ?? 89 56 04 89 56 08 BA ?? ?? ?? "
    "?? 0F 45 D0 8B 45 10 85 C0 89 56 0C 52 0F 45 C8 8D 45 F4 51 50 89 4E 10 E8";
const char* const kPatNearestListener =
    "E8 ?? ?? ?? ?? 83 F8 01 8B 45 E8 89 45 F8 0F 86 ?? ?? ?? ?? F3 0F 7E 4D E0 8D 7B 04 BE ?? ?? ?? ?? 66 0F D6 4D EC";
constexpr uint32_t kNearestArray = 0x1D;
constexpr int kListenerSlots = 4;
constexpr uint32_t kListenerBytes = 0x38;
constexpr uint32_t kListenerPosPtr = 0x04;   // its position pointer (the sound engine reads the position through it)
constexpr uint32_t kCmListeners = 0x4C;      // CameraManager: listener [2] (its "view objects")
constexpr float kListenerAhead = 2.0f;       // the camera manager's: 2 m along the camera's forward row

typedef void(__fastcall* ListenerEnable_t)(void* l, void* edx, int on);
typedef void(__fastcall* ListenerVectors_t)(void* l, void* edx, const float* pos, const float* fwd, const float* up);

bool g_lisWanted = true; // [Splitscreen] SoundP34
bool g_lisOk = false;
uintptr_t g_lisArray = 0;
uintptr_t g_lisCount = 0;          // the active listener count (VA)
ListenerEnable_t g_lisEnable = nullptr;
ListenerVectors_t g_lisVectors = nullptr;
uintptr_t g_lis[2] = {};           // the slot player 3's / 4's listener is (VA), 0 = none
bool g_lisBorrowed[2] = {};        // ... a slot that was already on (not the camera manager's): its vectors lent, put back after
unsigned char g_lisSaved[2][kListenerBytes] = {};
alignas(16) float g_lisPos[2][4] = {};
volatile LONG g_lisFrames = 0;
int g_lisLines = 0;

bool LisLog()
{
    if (g_lisLines >= 40)
        return false;
    ++g_lisLines;
    return true;
}

bool FindListeners()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kPatCmListenerMake, hits, 4);
    const uint32_t alloc = n == 1 ? CallTarget(hits[0]) : 0;
    const uint32_t enable = n == 1 ? CallTarget(hits[0] + kCmListenerEnableCall) : 0;
    const bool allocOk = alloc && MatchPatternAt(alloc, kListenerAllocBody);
    const bool enableOk = enable && MatchPatternAt(enable, kListenerEnableBody);
    const uintptr_t array = allocOk ? CodeU32(alloc + kListenerAllocArray) : 0;
    uint32_t h2[4] = {};
    const int n2 = FindPattern(kPatCmListenerSet, h2, 4);
    const uint32_t vectors = n2 == 1 ? CallTarget(h2[0] + kCmListenerSetCall) : 0;
    const bool vectorsOk = vectors && MatchPatternAt(vectors, kListenerVectorsBody);
    uint32_t h3[4] = {};
    const int n3 = FindPattern(kPatNearestListener, h3, 4);
    const uintptr_t nearArray = n3 == 1 ? CodeU32(h3[0] + kNearestArray) : 0;
    const bool arrayOk = array && array == nearArray && InImage(array);
    Log("[sound34] the camera manager's listeners (%d hit(s)): find a free slot rva 0x%08X %s (slots at rva 0x%08X), Enable rva "
        "0x%08X %s; SetVectors (%d hit(s)) rva 0x%08X %s; the sound engine's nearest-listener search (%d hit(s)) walks the slots at "
        "rva 0x%08X %s",
        n, alloc, allocOk ? "OK (4 x 38h)" : "NOT OK", RvaOf(array), enable, enableOk ? "OK" : "NOT OK", n2, vectors,
        vectorsOk ? "OK" : "NOT OK", n3, RvaOf(nearArray), arrayOk ? "(the same - OK)" : "- NOT the same");
    if (!allocOk || !enableOk || !vectorsOk || !arrayOk)
        return false;
    g_lisArray = array;
    g_lisCount = CodeU32(enable + 0x1A); // (Enable's inc dword ptr [count])
    if (!InImage(g_lisCount))
        g_lisCount = 0;
    g_lisEnable = reinterpret_cast<ListenerEnable_t>(Rva(enable));
    g_lisVectors = reinterpret_cast<ListenerVectors_t>(Rva(vectors));
    return true;
}

// SetVectors writes +4..+13h (the pointers) and +24h..+2Fh (their cross product): a borrowed slot gets those back
void ListenerRestore(int k, uintptr_t l)
{
    memcpy(reinterpret_cast<void*>(l + 0x04), g_lisSaved[k] + 0x04, 0x10);
    memcpy(reinterpret_cast<void*>(l + 0x24), g_lisSaved[k] + 0x24, 0x0C);
}

void Listener34Off(int k, const char* why)
{
    const uintptr_t l = g_lis[k];
    if (!l)
        return;
    g_lis[k] = 0;
    // only while it is still ours (its position pointer ours)
    const bool ours = *reinterpret_cast<const uintptr_t*>(l + kListenerPosPtr) == reinterpret_cast<uintptr_t>(g_lisPos[k]);
    if (ours && g_lisBorrowed[k])
        ListenerRestore(k, l);
    else if (ours && (*reinterpret_cast<const uint8_t*>(l) & 1))
        g_lisEnable(reinterpret_cast<void*>(l), nullptr, 0);
    if (LisLog())
        Log("[sound34] %lu ms: player %d's listener (slot %d%s) off (%s)%s", ScanMsSinceAttach(), k + 3,
            static_cast<int>((l - g_lisArray) / kListenerBytes), g_lisBorrowed[k] ? ", borrowed: its vectors put back" : "", why,
            ours ? "" : " - it was not ours any more: left alone");
    g_lisBorrowed[k] = false;
}

// game thread, each frame after view 3 / 4's camera was set (FrameWrap)
void Listener34Frame(int k, uintptr_t mgr)
{
    if (!g_lisOk)
        return;
    const uint8_t* const cam = g_specCam[k];
    const float* const fwd = reinterpret_cast<const float*>(cam + kCamMatrix + 0x20); // the camera manager's "forward" row
    const float* const up = reinterpret_cast<const float*>(cam + kCamMatrix + 0x10);
    const float* const pos = reinterpret_cast<const float*>(cam + kCamMatrix + 0x30);
    for (int i = 0; i < 3; ++i)
        g_lisPos[k][i] = pos[i] + kListenerAhead * fwd[i];
    const uintptr_t cm0 = mgr ? *reinterpret_cast<const uintptr_t*>(mgr + kCmListeners) : 0;
    const uintptr_t cm1 = mgr ? *reinterpret_cast<const uintptr_t*>(mgr + kCmListeners + 4) : 0;
    uintptr_t l = g_lis[k];
    if (l)
    {
        // still ours? (another listener's pointers there now, the camera manager's slot now, or turned off by the game -
        // the match teardown turns all 4 off): then it is left as it is and another one is found
        const bool other = *reinterpret_cast<const uintptr_t*>(l + kListenerPosPtr) != reinterpret_cast<uintptr_t>(g_lisPos[k]);
        const bool cm = l == cm0 || l == cm1;
        const bool off = !(*reinterpret_cast<const uint8_t*>(l) & 1);
        if (other || cm || off)
        {
            if (LisLog())
                Log("[sound34] %lu ms: player %d's listener slot %d is not his any more (%s) - left as it is",
                    ScanMsSinceAttach(), k + 3, static_cast<int>((l - g_lisArray) / kListenerBytes),
                    other ? "another listener's pointers" : (cm ? "the camera manager's now" : "turned off"));
            g_lis[k] = 0;
            g_lisBorrowed[k] = false;
            l = 0;
        }
    }
    if (!l)
    {
        // a free slot (off) first; else one that is on but not the camera manager's (the sound set-up's own, slot 0):
        // that one is borrowed - its vectors ours while he plays, put back after
        uintptr_t lent = 0;
        for (int i = 0; i < kListenerSlots && !l; ++i)
        {
            const uintptr_t c = g_lisArray + static_cast<uintptr_t>(i) * kListenerBytes;
            if (c == cm0 || c == cm1 || c == g_lis[0] || c == g_lis[1])
                continue;
            if (*reinterpret_cast<const uint8_t*>(c) & 1)
            {
                if (!lent)
                    lent = c;
                continue;
            }
            l = c;
        }
        const bool borrow = !l && lent;
        if (borrow)
            l = lent;
        if (!l)
        {
            if (LisLog())
                Log("[sound34] %lu ms: player %d: no listener slot left (4 taken) - he hears from players 1 / 2's ears", ScanMsSinceAttach(),
                    k + 3);
            return;
        }
        if (borrow)
            memcpy(g_lisSaved[k], reinterpret_cast<const void*>(l), kListenerBytes);
        g_lisVectors(reinterpret_cast<void*>(l), nullptr, g_lisPos[k], fwd, up); // pointed first, then enabled
        if (!borrow)
            g_lisEnable(reinterpret_cast<void*>(l), nullptr, 1);
        g_lis[k] = l;
        g_lisBorrowed[k] = borrow;
        if (LisLog())
            Log("[sound34] %lu ms: player %d's LISTENER ON: slot %d%s (the camera manager's are %d / %d), at view %d's camera, %.0f m "
                "ahead; it is %s, active listeners %ld",
                ScanMsSinceAttach(), k + 3, static_cast<int>((l - g_lisArray) / kListenerBytes),
                borrow ? " (on already - borrowed)" : "", cm0 ? static_cast<int>((cm0 - g_lisArray) / kListenerBytes) : -1,
                cm1 ? static_cast<int>((cm1 - g_lisArray) / kListenerBytes) : -1, k + 3, static_cast<double>(kListenerAhead),
                (*reinterpret_cast<const uint8_t*>(l) & 1) ? "on" : "NOT on",
                g_lisCount ? static_cast<long>(*reinterpret_cast<const volatile int32_t*>(g_lisCount)) : -1L);
        return;
    }
    g_lisVectors(reinterpret_cast<void*>(l), nullptr, g_lisPos[k], fwd, up); // every frame, as the camera manager does
    InterlockedIncrement(&g_lisFrames);
}

// The view loop's jump thunk: the spectators' cameras set right before the game draws.
bool PlayerCamera(int k, float* m); // 20d (below): player 3 / 4's own camera, false while he has no soldier
void PlayersFrame();                // 20d (below): their test spawn
bool PlayerChase(int k, float dt);  // 20e-r2 (below): player 3 / 4's chase camera on view 3 / 4
void Shot34Frame(int k);            // 20g-r9 (below): player 3 / 4's view on player 1's camera shot while he has no soldier
void EndShot34Frame(int k, uintptr_t mgr); // 21j-r15 (below): at the match end, player 1's / 2's camera
void Shot2Frame(uintptr_t cm);      // 20g-r10 (below): player 2's view on it (his spawn screens up)
void HudFrame();                    // 20g-r3 (below): players 3 and 4's HUD hidden / shown with their spawn screens
void HudDiagFrame();                // 20g-r5 (below): [diag34] / [wall34] lines
int HudRoot(int k);                 // 20g-r3 (below): the HUD root player 3 / 4's displays are set up in, -1 if none
void Names34Frame();                // 21j-r33 (below): players 3 / 4's characters get their sign-in names
void RetFrame();                    // 21j-r6 (below): the reticle scan's second projection
volatile LONG g_retDrawing = 0;     // 21j-r6: FrameWrap is in the game's draw (the reticle scan's note)

void __cdecl FrameWrap()
{
    const bool perfOurs = FramePerfEnter(kPerfOurFrame); // 21j-r11: the frame-rate scan (our work before the game's render)
    bool shot34[2] = {}; // 20g-r10: views 3 / 4 that are ours this frame (the shot and their ears set after PlayersFrame)
    uintptr_t shotMgr = 0;
    // 20e-r2: the frame's time for the chase cameras
    static LARGE_INTEGER s_freq = {};
    static LARGE_INTEGER s_last = {};
    LARGE_INTEGER nowQ;
    if (!s_freq.QuadPart)
        QueryPerformanceFrequency(&s_freq);
    QueryPerformanceCounter(&nowQ);
    float dt = s_last.QuadPart && s_freq.QuadPart
                   ? static_cast<float>(static_cast<double>(nowQ.QuadPart - s_last.QuadPart) /
                                        static_cast<double>(s_freq.QuadPart))
                   : 0.016f;
    s_last = nowQ;
    if (!(dt > 0.0f) || dt > 0.1f)
        dt = 0.016f;
    if (g_quarterMgr)
    {
        const uintptr_t mgr = CameraManager();
        bool ok = mgr == g_quarterMgr && static_cast<int>(ReadU32(mgr + kCamCount)) == 2;
        for (int i = 0; ok && i < 2; ++i)
        {
            const int v = static_cast<int>(ReadU32(mgr + kCamViews + static_cast<uintptr_t>(i) * 4));
            ok = RendCamera(v) == ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(i) * 4);
        }
        if (!ok)
            FreeSpectators("the camera manager changed");
        else
        {
            for (int k = 0; k < 2; ++k)
            {
                const int sv = g_specView[k];
                if (sv < 0 || RendCamera(sv) != reinterpret_cast<uintptr_t>(g_specCam[k]))
                    continue;
                if (!PlayerChase(k, dt)) // 20e-r2: the game's chase camera for player 3 / 4
                {
                    const uintptr_t player = ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(k) * 4);
                    float m[16];
                    memcpy(m, reinterpret_cast<const void*>(player + kCamMatrix), sizeof(m));
                    for (int r = 0; r < 3; ++r) // 20c: turned round the world's up: x and z of each axis negated
                    {
                        m[r * 4 + 0] = -m[r * 4 + 0];
                        m[r * 4 + 2] = -m[r * 4 + 2];
                    }
                    PlayerCamera(k, m); // 20d: player 3 / 4's soldier (only without the chase cameras)
                    g_camMatrix(g_specCam[k], nullptr, m);
                }
                shot34[k] = true; // 20g-r10: Shot34Frame after PlayersFrame (his spawn screen comes up there - same frame)
                shotMgr = mgr;
                const int pv = static_cast<int>(ReadU32(mgr + kCamViews + static_cast<uintptr_t>(k) * 4));
                *RendFlag(0, sv) = *RendFlag(0, pv);
                *RendFlag(2, sv) = *RendFlag(2, pv);
            }
            Shot2Frame(mgr); // 20g-r10: player 2's view (the camera manager applied his camera already this frame)
            InterlockedIncrement(&g_specFrames);
        }
    }
    PlayersFrame();
    for (int k = 0; k < 2; ++k)
    {
        if (!shot34[k])
            continue;
        Shot34Frame(k); // 20g-r9; 20g-r10: after PlayersFrame - in the frame his spawn screen comes up
        EndShot34Frame(k, shotMgr); // 21j-r15: the match is over - player 1's / 2's camera (the game's end shots)
        if (g_playersOn && static_cast<int>(g_players) >= k + 3)
            Listener34Frame(k, shotMgr); // 20g-r6: his own ears at view 3 / 4's camera (20g-r10: the shot's, when it is on)
    }
    HudFrame(); // 20g-r3: players 3 and 4's HUD hidden while their spawn screens are up, shown again after
    Names34Frame(); // 21j-r33: players 3 / 4's characters get their sign-in names
    HudDiagFrame(); // 20g-r5: [diag34] / [wall34] (log only)
    // 20f-r2: the HUD root of view 3 / 4 drawn while player 3 / 4's spawn screen (in that root) is up - set after
    // PlayersFrame (which puts the screens up / takes them down) so the frame drawn now has it. 20g-r3: and while his
    // own HUD displays are set up in it.
    for (int k = 0; k < 2; ++k)
    {
        const int sv = PlayerNSpecView(k);
        if (sv >= 0)
            *RendFlag(1, sv) = (P2SpawnP34Root(k) == sv || HudRoot(k) == sv) ? 1 : 0;
    }
    RetFrame(); // 21j-r6: the reticle scan (players 3 / 4's cameras are set by now)
    FramePerfLeave(perfOurs); // 21j-r11
    InterlockedExchange(&g_retDrawing, 1);
    const bool perfRender = FramePerfEnter(kPerfRender); // 21j-r11: the game's render frame
    g_origFrame();
    FramePerfLeave(perfRender);
    InterlockedExchange(&g_retDrawing, 0);
}

DWORD WINAPI ViewsHeartbeat(LPVOID)
{
    LONG last = 0;
    for (;;)
    {
        Sleep(30000);
        const LONG now = g_specFrames;
        if (now != last || g_quarterMgr)
        {
            char when[160];
            snprintf(when, sizeof(when), "heartbeat: %ld frames with views 3 and 4 in the last 30 s, SetNumCameras "
                     "calls %ld", static_cast<long>(now - last), static_cast<long>(g_setNumCalls));
            if (g_viewLines < 200)
                LogViews(when);
        }
        last = now;
    }
}

// A function's single jump thunk moved to 'to'; the old target in *orig. Logged.
bool MoveThunk(const char* name, uint32_t fnRva, const void* to, uintptr_t* orig, uint32_t* thunkOut = nullptr)
{
    uint32_t thunks[4] = {};
    int direct = 0;
    const int nt = FindThunks(fnRva, thunks, 4, &direct);
    if (nt != 1)
    {
        Log("[views4] %s rva 0x%08X: %d jump thunks (need exactly 1) - NOT installed", name, fnRva, nt);
        return false;
    }
    if (thunkOut)
        *thunkOut = thunks[0];
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(thunks[0]));
    const uintptr_t dest = Rva(RelTarget(thunks[0]));
    DWORD old = 0;
    if (!VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old))
    {
        Log("[views4] %s: VirtualProtect failed (error %lu) - NOT installed", name, GetLastError());
        return false;
    }
    *orig = dest;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(to) - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(at + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(at + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, 5);
    int32_t back = 0;
    memcpy(&back, at + 1, 4);
    Log("[views4] %s rva 0x%08X: its jump thunk rva 0x%08X now goes through BF1GameExt (%d direct calls)%s%s", name,
        fnRva, thunks[0], direct, dest == Rva(fnRva) ? "" : " - the thunk was already moved: CHAINED",
        back == rel ? "" : " - READ BACK FAILED");
    return back == rel;
}

void InstallViews()
{
    uint32_t hits[4] = {};
    const uint32_t ctor = FindOne("RedCamera ctor", kPatCamCtor);
    const uint32_t params = FindOne("RedCamera projection", kPatCamParams);
    const uint32_t matrix = FindOne("RedCamera set matrix", kPatCamMatrix);
    const uint32_t vfree = FindOne("view free", kPatViewFree);
    const uint32_t valloc4 = FindOne("view alloc", kViewAlloc);
    const uint32_t valloc = valloc4 >= 4 && MatchPatternAt(valloc4 - 4, kViewAllocHead) ? valloc4 - 4 : 0;
    const uint32_t setNum = FindOne("SetNumCameras", kSetNumCameras);
    const uint32_t layouts = FindOne("SetNumCameras' layouts", kSetNumCamerasLayouts);
    const uint32_t ratioFn = layouts ? CallTarget(layouts) : 0;
    g_ratioVa = ratioFn && MatchPatternAt(ratioFn, kFldImmRet) ? CodeU32(ratioFn + 2) : 0;
    g_rendCams = g_viewCamerasRva ? Rva(g_viewCamerasRva) : 0;
    const bool freeOk = vfree && OperandRva(vfree + kViewFreeCams) == g_viewCamerasRva;
    // the frame function: the view loop's head (its operands the renderer's current view and count), then back
    // to the function's start
    uint32_t frame = 0;
    int n = FindPattern(kPatViewLoop, hits, 4);
    if (n == 1 && OperandRva(hits[0] + 2) == g_viewCamerasRva + 0x10 && OperandRva(hits[0] + 8) == g_viewCamerasRva + 0x14)
    {
        for (uint32_t back = 1; back < 0x200 && !frame; ++back)
            if (CodeByte(hits[0] - back - 1) == 0xCC && MatchPatternAt(hits[0] - back, kFrameProlog))
                frame = hits[0] - back;
    }
    Log("[views4] found: RedCamera ctor 0x%08X, projection 0x%08X, set matrix 0x%08X, view alloc 0x%08X, view free 0x%08X "
        "%s, SetNumCameras 0x%08X, screen ratio float rva 0x%08X (%.4f now), renderer cameras rva 0x%08X, the frame "
        "function 0x%08X (view loop %d hit(s))",
        ctor, params, matrix, valloc, vfree, freeOk ? "OK" : "(table differs)", setNum, RvaOf(g_ratioVa),
        g_ratioVa ? static_cast<double>(*reinterpret_cast<const float*>(g_ratioVa)) : 0.0, g_viewCamerasRva, frame, n);
    if (!ctor || !params || !matrix || !valloc || !freeOk || !setNum || !g_ratioVa || !g_rendCams || !frame ||
        !g_camMgrPtrRva)
    {
        Log("[views4] Views=4 NOT installed: something above was not found - the 2-player split as before");
        return;
    }
    g_camCtor = reinterpret_cast<CamCtor_t>(Rva(ctor));
    g_camParams = reinterpret_cast<CamParams_t>(Rva(params));
    g_camMatrix = reinterpret_cast<CamMatrix_t>(Rva(matrix));
    g_viewAlloc = reinterpret_cast<ViewAlloc_t>(Rva(valloc));
    g_viewFree = reinterpret_cast<ViewFree_t>(Rva(vfree));
    uintptr_t origSet = 0;
    uintptr_t origFrame = 0;
    if (!MoveThunk("the frame function", frame, reinterpret_cast<const void*>(&FrameWrap), &origFrame))
        return;
    g_origFrame = reinterpret_cast<Frame_t>(origFrame);
    if (!MoveThunk("SetNumCameras", setNum, reinterpret_cast<const void*>(&SetNumCamerasWrap), &origSet, &g_setNumThunk))
        return;
    g_origSetNum = reinterpret_cast<SetNumCameras_t>(origSet);
    g_setNumRva = setNum;
    g_viewsReady = true;
    g_viewsLive = g_viewsWanted; // 21g-r2: until a sign-in says otherwise
    if (g_viewsWanted == 3) // 20g-r13
        Log("[views4] VIEWS=3 ON (3-player): after the game's SetNumCameras(2) the two players' views become the top quarters "
            "(player 1 left, player 2 right, 48 deg, the screen's shape) and view 3 the full-width bottom half (48 deg, the "
            "2-player halves' shape: the screen's height / width x 0.5) - player 1's camera turned round until player 3 "
            "exists. No view 4. Vertical is ignored.");
    else
        Log("[views4] VIEWS=4 ON: after the game's SetNumCameras(2) the two players' views become the top quarters (player 1 "
            "left, player 2 right, 48 deg, the screen's shape) and views 3 and 4 the bottom quarters - spectators (player "
            "1's / player 2's camera turned round, no HUD) until players 3 and 4 exist. Vertical is ignored.");
    // a match already running with 2 cameras (not at game start, but in case): quarters now
    const uintptr_t mgr = CameraManager();
    if (mgr && static_cast<int>(ReadU32(mgr + kCamCount)) == 2)
        MakeQuarters(mgr);
    else
        LogViews("at install");
    const HANDLE t = CreateThread(nullptr, 0, ViewsHeartbeat, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
}


// ---------------------------------------------------------------------------------------------------------
// Phase 20c: players 3 and 4 made to exist ([Splitscreen] Players=3 / 4, with Views=4)
// ---------------------------------------------------------------------------------------------------------
// the camera manager's creation (cdecl: count): new 244h, ctor, SetNumCameras(count)
const char* const kPatCamCreate = "55 8B EC 68 44 02 00 00 E8";
// the three pause loops (i < GetNumLocalPlayers over PauseState[2])
const char* const kPatPauseLoop[3] = {
    "56 33 F6 E8 ?? ?? ?? ?? 85 C0 7E 21 57 33 FF 90 8B 0D ?? ?? ?? ?? 56 8D 0C 0F E8",
    "57 33 FF E8 ?? ?? ?? ?? 85 C0 7E 2C 56 33 F6 90 8B 0D ?? ?? ?? ?? 8D 0C 0E E8",
    "55 8B EC 56 33 F6 E8 ?? ?? ?? ?? 85 C0 7E 29 57 33 FF 8B 0D ?? ?? ?? ?? F3 0F 10 45 08",
};
constexpr uint32_t kPauseLoopSpan = 0x48;
// the message loop in GOG 1.3 0x001A0A70: its head (GetNumLocalPlayers at +2) and its tail (at +19)
const char* const kPatMsgHead = "33 FF E8 ?? ?? ?? ?? 85 C0 74 53 57 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 3C";
const char* const kPatMsgTail = "6A 00 57 E8 ?? ?? ?? ?? 83 C4 08 8B C8 E8 ?? ?? ?? ?? 47 E8 ?? ?? ?? ?? 3B F8 72 AD";
constexpr uint32_t kMsgHeadCall = 2;
constexpr uint32_t kMsgTailCall = 19;
constexpr uint32_t kNetIdToLocalBound = 0x1C; // in kPatNetIdToLocal: cmp eax, 2
constexpr uint32_t kNetSlotsFillBound = 0x17; // in kNetSlotsFill: cmp esi, 2

int g_playersWanted = 2;     // [Splitscreen] Players
int32_t g_netSlots4[4] = { -1, -1, -1, -1 };
uintptr_t g_netSlotsOld = 0; // the game's int[2] (VA)

bool WriteCode(uintptr_t at, const void* bytes, size_t n)
{
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(at), n, PAGE_EXECUTE_READWRITE, &old))
        return false;
    memcpy(reinterpret_cast<void*>(at), bytes, n);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(at), n, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), n);
    return memcmp(reinterpret_cast<const void*>(at), bytes, n) == 0;
}

// Every relocated dword in the code section whose value is in [lo, hi): its rva. Through the exe's own base
// relocation table (the same table Windows used to load it).
int FindOperands(uintptr_t lo, uintptr_t hi, uint32_t* out, int max)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!CodeSection(&codeRva, &codeSize))
        return -1;
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(g_base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(g_base + static_cast<uintptr_t>(dos->e_lfanew));
    const IMAGE_DATA_DIRECTORY& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (!dir.VirtualAddress || dir.Size < 8)
        return -1;
    int n = 0;
    uint32_t at = 0;
    while (at + 8 <= dir.Size)
    {
        const auto* block = reinterpret_cast<const IMAGE_BASE_RELOCATION*>(g_base + dir.VirtualAddress + at);
        if (block->SizeOfBlock < 8 || at + block->SizeOfBlock > dir.Size)
            break;
        const auto* e = reinterpret_cast<const uint16_t*>(block + 1);
        const uint32_t entries = (block->SizeOfBlock - 8) / 2;
        for (uint32_t i = 0; i < entries; ++i)
        {
            if ((e[i] >> 12) != IMAGE_REL_BASED_HIGHLOW)
                continue;
            const uint32_t rva = block->VirtualAddress + (e[i] & 0x0FFFu);
            if (rva < codeRva || rva + 4 > codeRva + codeSize)
                continue;
            const uint32_t v = CodeU32(rva);
            if (v >= lo && v < hi)
            {
                if (n < max)
                    out[n] = rva;
                ++n;
            }
        }
        at += block->SizeOfBlock;
    }
    return n;
}

bool AddClamp(uint32_t callRva, const char* what)
{
    if (g_clampCount >= kMaxClamp)
        return false;
    for (int i = 0; i < g_clampCount; ++i)
        if (g_clampRet[i] == Rva(callRva + 5))
            return true;
    g_clampRet[g_clampCount++] = Rva(callRva + 5);
    Log("[players] call site rva 0x%08X (%s) keeps getting 2", callRva, what);
    return true;
}


// ---------------------------------------------------------------------------------------------------------
// Phase 20d: players 3 and 4's guards, cameras and test spawn
// ---------------------------------------------------------------------------------------------------------
// The first person / player music functions that take the player first (GOG 1.3 rvas in the header).
const char* const kPatGuard[5] = {
    // 0x0012B640: (player, on) - the FirstPersonRenderable's state, then the per-player state's
    "55 8B EC E8 ?? ?? ?? ?? 84 C0 74 09 E8 ?? ?? ?? ?? 84 C0 74 6C 56 8B 75 08 8B 0C B5 ?? ?? ?? ?? 85 C9 74 5C",
    // 0x0012B9D0 / 0x0012BEC0 / 0x0012C660 / 0x0012C6D0: per-player state, then its FirstPersonRenderable
    "55 8B EC 56 8B 75 08 69 CE C0 02 00 00 81 C1 ?? ?? ?? ?? E8 ?? ?? ?? ?? 51 8B 0C B5 ?? ?? ?? ?? D9 1C 24 6A 0A "
    "6A 09 E8",
    // 0x0012B980 / 0x0012BA60 / 0x0012BE10 / 0x0012CD30: per-player state only
    "55 8B EC 69 4D 08 C0 02 00 00 81 C1 ?? ?? ?? ?? E8 ?? ?? ?? ?? DD D8 5D C3",
    // 0x0012D2E0: (player, matrix)
    "55 8B EC 56 8B 75 0C 57 8B 7D 08 69 CF C0 02 00 00 56 81 C1",
    // 0x0012CDA0: (player, event, id, ...)
    "55 8B EC E8 ?? ?? ?? ?? 84 C0 74 0D E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? 56 8B 75 08 57 8B 7D 10 3B 3C B5",
};
const int kGuardHits[5] = { 1, 4, 4, 1, 1 }; // GOG 1.3
constexpr uint32_t kGuard0Table = 28;        // guard 0's "mov ecx, [esi*4 + FirstPersonRenderable]" operand
// 20d-r4: (player, x) -> FirstPersonRenderable[player]->(x), 0x0012D270 (weapon fire); 2 hits in GOG 1.3 - the other,
// 0x0028EA30, reads the renderer's table - told apart by the operand at +0Ch
const char* const kPatGuardFire = "55 8B EC 8B 4D 08 FF 75 0C 8B 0C 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kGuardFireTable = 12;
// 20d-r4: (player) -> FirstPersonRenderable[player] ? its vtable +0Ch : 0, 0x0012BDC0 (1 hit)
const char* const kPatGuardModel = "55 8B EC 8B 45 08 8B 0C 85 ?? ?? ?? ?? 85 C9 74 08 8B 01 8B 40 0C 5D FF E0";
constexpr uint32_t kGuardModelTable = 9;
constexpr int kMaxGuards = 14;
const char* const kSpawnPressed = "8B 8E 70 23 00 00 50 E8 ?? ?? ?? ?? 8B 8E 70 23 00 00 6A 01 E8 ?? ?? ?? ??";
constexpr uint32_t kSpawnPressedReady = 20; // E8 SetReady
const char* const kSetReadyBody = "55 8B EC 8A 45 08 88 81 CC 00 00 00 5D C2 04 00";

typedef uint32_t(__cdecl* Fn9_t)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void(__fastcall* SetReady_t)(void* character, void* edx, uint32_t ready);

uintptr_t g_guardOrig[kMaxGuards] = {};
uint32_t g_guardRva[kMaxGuards] = {};
volatile LONG g_guardHit[kMaxGuards] = {};
int g_guardCount = 0;
SetReady_t g_setReady = nullptr;
bool g_spawnWanted = true; // [Splitscreen] AutoSpawnP34
bool g_padsWanted = true; // [Splitscreen] PadsP34 (20e)

// Called like the game's function (cdecl - the call site cleans its arguments; up to 9 are passed on, the function
// reads what it takes). Players 3 and 4: nothing, 0.
template <int N>
uint32_t __cdecl GuardWrap(uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6,
                           uint32_t a7, uint32_t a8)
{
    if (g_playersOn && (a0 == 2 || a0 == 3))
    {
        InterlockedIncrement(&g_guardHit[N]);
        P34Guard("first-person / music guard", static_cast<uint32_t>(N), BF1_RETURN_ADDRESS(), static_cast<int>(a0));
        return 0;
    }
    return reinterpret_cast<Fn9_t>(g_guardOrig[N])(a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

const void* GuardFor(int n)
{
    switch (n)
    {
    case 0: return reinterpret_cast<const void*>(&GuardWrap<0>);
    case 1: return reinterpret_cast<const void*>(&GuardWrap<1>);
    case 2: return reinterpret_cast<const void*>(&GuardWrap<2>);
    case 3: return reinterpret_cast<const void*>(&GuardWrap<3>);
    case 4: return reinterpret_cast<const void*>(&GuardWrap<4>);
    case 5: return reinterpret_cast<const void*>(&GuardWrap<5>);
    case 6: return reinterpret_cast<const void*>(&GuardWrap<6>);
    case 7: return reinterpret_cast<const void*>(&GuardWrap<7>);
    case 8: return reinterpret_cast<const void*>(&GuardWrap<8>);
    case 9: return reinterpret_cast<const void*>(&GuardWrap<9>);
    case 10: return reinterpret_cast<const void*>(&GuardWrap<10>);
    case 11: return reinterpret_cast<const void*>(&GuardWrap<11>);
    case 12: return reinterpret_cast<const void*>(&GuardWrap<12>);
    case 13: return reinterpret_cast<const void*>(&GuardWrap<13>);
    default: return nullptr;
    }
}

bool InstallCamGuards(); // 20d-r2 (below)
bool InstallMsgAlias();  // 20d-r3 (below)
bool InstallRumbleGuards(); // 20d-r5 (below)
bool InstallFilterGuards(); // 20d-r6 (below)

// The guards in, before the count goes up (false: Players stays off).
bool InstallGuards()
{
    if (!g_hooked[kSetPausedA] || !g_hooked[kSetPausedB] || !g_hooked[kIsPaused])
    {
        Log("[players] the pause calls are not ours (SetPaused A %d, B %d, IsPaused %d) - Players NOT installed",
            g_hooked[kSetPausedA] ? 1 : 0, g_hooked[kSetPausedB] ? 1 : 0, g_hooked[kIsPaused] ? 1 : 0);
        return false;
    }
    uint32_t fns[kMaxGuards] = {};
    int total = 0;
    for (int k = 0; k < 5; ++k)
    {
        uint32_t hits[8] = {};
        const int n = FindPattern(kPatGuard[k], hits, 8);
        if (n != kGuardHits[k])
        {
            Log("[players] guard pattern %d: %d hits (GOG 1.3: %d) - Players NOT installed", k, n, kGuardHits[k]);
            return false;
        }
        for (int i = 0; i < n && total < kMaxGuards; ++i)
            fns[total++] = hits[i];
    }
    // 20d-r4: the weapon-fire and model functions, by the FirstPersonRenderable table guard 0 reads
    {
        const uint32_t table = CodeU32(fns[0] + kGuard0Table);
        uint32_t hits[4] = {};
        const int n = FindPattern(kPatGuardFire, hits, 4);
        uint32_t fire = 0;
        int same = 0;
        for (int i = 0; i < n && i < 4; ++i)
            if (CodeU32(hits[i] + kGuardFireTable) == table)
            {
                fire = hits[i];
                ++same;
            }
        uint32_t mhits[4] = {};
        const int nm = FindPattern(kPatGuardModel, mhits, 4);
        const bool modelOk = nm == 1 && CodeU32(mhits[0] + kGuardModelTable) == table;
        if (same != 1 || !modelOk)
        {
            Log("[players] first-person weapon fire: %d pattern hits, %d on the first-person table (need 1); model getter: "
                "%d hits%s - Players NOT installed",
                n, same, nm, nm == 1 ? (modelOk ? " OK" : ", not on the first-person table") : "");
            return false;
        }
        fns[total++] = fire;
        fns[total++] = mhits[0];
        Log("[players] first-person weapon fire rva 0x%08X and model getter rva 0x%08X (table %08X) - guarded too", fire,
            mhits[0], table);
    }
    int installed = 0;
    for (int i = 0; i < total; ++i)
    {
        uint32_t thunks[4] = {};
        int direct = 0;
        const int nt = FindThunks(fns[i], thunks, 4, &direct);
        if (nt != 1 || direct)
        {
            Log("[players] guard rva 0x%08X: %d jump thunks, %d direct calls (need 1, 0) - Players NOT installed", fns[i], nt,
                direct);
            return false;
        }
        uintptr_t orig = 0;
        char name[48];
        snprintf(name, sizeof(name), "player 3 / 4 guard %d", i);
        if (!MoveThunk(name, fns[i], GuardFor(i), &orig))
            return false;
        g_guardOrig[i] = orig;
        g_guardRva[i] = fns[i];
        ++installed;
    }
    g_guardCount = installed;
    if (!InstallCamGuards())
        return false;
    // 20d-r3: the six display getters (census kinds, hooked for Players even with PlayerNScan=0) and the message one
    for (int k = 0; k < kKinds; ++k)
        if (IsDisplayKind(k) && !g_hooked[k])
        {
            Log("[players] the %s is not ours - Players NOT installed", kKindName[k]);
            return false;
        }
    if (!InstallMsgAlias())
        return false;
    if (!InstallRumbleGuards()) // 20d-r5
        return false;
    if (!InstallFilterGuards()) // 20d-r6
        return false;
    Log("[players] HUD for players 3 and 4 (until they have their own): the Boot / Capture / Map / Scope / Status / Team "
        "display getters and GetMessageDisplay answer player 1's display for player 3 and player 2's for player 4");
    Log("[players] guards: %d first person / player music functions return at once for players 3 and 4 (GOG 1.3: 13); "
        "SetPaused A / B and IsPaused do nothing for them", installed);
    // the test spawn: the PC Spawn button's SetReady
    const uint32_t sp = FindOne("the Spawn button's calls", kSpawnPressed);
    const uint32_t ready = sp ? CallTarget(sp + kSpawnPressedReady) : 0;
    if (ready && MatchPatternAt(ready, kSetReadyBody))
        g_setReady = reinterpret_cast<SetReady_t>(Rva(ready));
    Log("[players] SetReady rva 0x%08X %s; AutoSpawnP34=%d", ready, g_setReady ? "OK" : "NOT FOUND", g_spawnWanted ? 1 : 0);
    return true;
}

// 20d-r2: the camera manager's methods that take a camera index (= a local player's slot) from outside. Its per-camera
// arrays have 2 slots (fade times +18h, current camera +24h, RedCamera +2Ch, chase +34h, free +3Ch, death +44h, view
// object +4Ch, renderer view +6Ch); the camera count is at +20h (slot 2 of the fade times).
const char* const kPatCamGet = "55 8B EC 8B 45 08 8B 44 81 2C 5D C2 04 00"; // GetCamera(i), GOG 1.3 0x00083EB0
const char* const kPatCamChase =                                           // SetChaseTarget(target, i), 0x000947A0
    "55 8B EC 53 56 57 8B 7D 0C 8B F1 8B 4C BE 34 E8 ?? ?? ?? ?? 8B 5D 08 3B C3 74 08 C7 44 BE 18 00 00 80 3F";
const char* const kPatCamDeath = "55 8B EC 56 57 8B 7D 0C 8B F1 8B 4C BE 34 39 4C BE 24 75 1D E8"; // (a, i, c), 0x00095260
const char* const kPatCamFade =                                                                  // (i, to, rate), 0x00095F50
    "55 8B EC F3 0F 10 4D 0C 56 57 8B 7D 08 8B F1 F3 0F 10 54 BE 18 0F 2F D1";

typedef uintptr_t(__fastcall* CamGet_t)(void* mgr, void* edx, int i);
typedef uint32_t(__fastcall* CamChase_t)(void* mgr, void* edx, uint32_t target, int i);
typedef uint32_t(__fastcall* CamDeath_t)(void* mgr, void* edx, uint32_t a, int i, uint32_t c);
typedef float(__fastcall* CamFade_t)(void* mgr, void* edx, int i, float to, float rate);

CamGet_t g_origCamGet = nullptr;
CamChase_t g_origCamChase = nullptr;
CamDeath_t g_origCamDeath = nullptr;
CamFade_t g_origCamFade = nullptr;
volatile LONG g_camGuardHit[4] = {}; // get, chase, death, fade

// 20e-r2: players 3 and 4's chase cameras (the game's ChaseCamera class in our memory).
const char* const kPatChaseMake =
    "68 E0 00 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0B 53 57 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 6A 70 89 46 08";
constexpr uint32_t kChaseMakeCtor = 21;                       // its E8 ChaseCamera(manager, index)
const char* const kChaseCtorBody = "55 8B EC 56 FF 75 0C 8B F1 FF 75 08 E8";
constexpr uint32_t kChaseSetTargetAt = 35;                    // in SetChaseTarget: mov ecx, [esi+edi*4+34h]; push ebx
const char* const kChaseSetTargetSite = "8B 4C BE 34 53 E8";  //  ; call ChaseCamera::SetTarget
const char* const kChaseSetTargetBody = "55 8B EC 56 8B F1 8B 4D 08 8B 46 0C 3B C1";
constexpr uint32_t kChaseBytes = 0xE0;

typedef void*(__fastcall* ChaseCtor_t)(void* self, void* edx, uintptr_t mgr, int index);
typedef void(__fastcall* ChaseSetTarget_t)(void* self, void* edx, uintptr_t target);
typedef void(__fastcall* ChaseUpdate_t)(void* self, void* edx, float dt);
typedef void(__fastcall* ChaseApply_t)(void* self, void* edx, void* camera, float dt);

alignas(16) unsigned char g_chase[2][kChaseBytes + 0x20] = {};
bool g_chaseMade[2] = {};
volatile uintptr_t g_chaseWanted[2] = {}; // the target the game gave (SetChaseTarget 2 / 3)
uintptr_t g_chaseTarget[2] = {};          // the target our chase camera has
bool g_chaseEver[2] = {};
bool g_chaseOk = false;
ChaseCtor_t g_chaseCtor = nullptr;
ChaseSetTarget_t g_chaseSetTarget = nullptr;
ChaseUpdate_t g_chaseUpdate = nullptr;
ChaseApply_t g_chaseApply = nullptr;
volatile LONG g_chaseFrames = 0;
int g_chaseLines = 0;

// 20g-r6: players 3 and 4's death cameras (the game's DeathCamera class in our memory, as their chase cameras). The
// camera manager makes one per camera (GOG 1.3 0x00094512: new(70h), DeathCamera(manager, index) 0x00139B80 - it only
// stores its fields, nothing allocated) and starts it (0x00095260, the guarded one) with Setup(target, killer, the
// chase camera's matrix) (0x0013A5D0) while the chase camera is current; then Update(dt) / Apply(camera, dt) (vtable
// +4 / +8, as the chase camera's) every frame until SetChaseTarget gives the chase camera a soldier again.
const char* const kPatDeathMake =
    "6A 70 89 46 10 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0B 53 57 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 89 46 18";
constexpr uint32_t kDeathMakeCtor = 21; // its E8 DeathCamera(manager, index)
const char* const kDeathCtorBody =
    "55 8B EC 56 FF 75 0C 8B F1 FF 75 08 E8 ?? ?? ?? ?? C7 06 ?? ?? ?? ?? 8B C6 C7 46 6C 00 00 00 00 5E 5D C2 08 00";
const char* const kPatCamDeathFull = "55 8B EC 56 57 8B 7D 0C 8B F1 8B 4C BE 34 39 4C BE 24 75 1D E8 ?? ?? ?? ?? 8B 4C BE "
                                     "44 50 FF 75 10 FF 75 08 E8 ?? ?? ?? ?? 8B 44 BE 44 89 44 BE 24 5F 5E 5D C2 0C 00";
constexpr uint32_t kCamDeathMatrixCall = 0x14; // E8 the chase camera's matrix getter (lea eax, [ecx+10h]; ret)
constexpr uint32_t kCamDeathSetupCall = 0x24;  // E8 DeathCamera::Setup(target, killer, start matrix)
const char* const kChaseMatrixBody = "8D 41 10 C3";
const char* const kDeathSetupBody = "55 8B EC 83 EC 30 53 8B 5D 10 57 8B F9 8B 4D 08 89 4F 0C 0F 10 03 0F 11 47 10";
constexpr uint32_t kDeathBytes = 0x70;
constexpr uint32_t kCamObjMatrix = 0x10; // a camera object's matrix (what the game starts a death camera from)

typedef void*(__fastcall* DeathCtor_t)(void* self, void* edx, uintptr_t mgr, int index);
typedef void(__fastcall* DeathSetup_t)(void* self, void* edx, uint32_t target, uint32_t killer, const void* start);

alignas(16) unsigned char g_death[2][kDeathBytes + 0x10] = {};
bool g_deathWanted = true; // [Splitscreen] DeathCamP34
bool g_deathOk = false;
DeathCtor_t g_deathCtor = nullptr;
DeathSetup_t g_deathSetup = nullptr;
ChaseUpdate_t g_deathUpdate = nullptr; // its vtable +4 / +8 (read after the first construction, class checked)
ChaseApply_t g_deathApply = nullptr;
volatile LONG g_deathOn[2] = {};       // his death camera draws view 3 / 4
uintptr_t g_deathMgr[2] = {};          // the camera manager it was started under (another one = another match: off)
volatile LONG g_deathStarts = 0;
volatile LONG g_deathFrames = 0;
int g_deathLines = 0;

bool DeathLog()
{
    if (g_deathLines >= 80)
        return false;
    ++g_deathLines;
    return true;
}

uintptr_t LocalCharacter(int p); // 20d (below): the character of local player p

// The game starts camera 2 / 3's death camera (game thread; the guard's caller): once per death - the game calls it
// every frame he is down, and only the first call (its chase camera current) starts it; never while he has a soldier.
void DeathStart34(int k, uintptr_t mgr, uint32_t target, uint32_t killer)
{
    if (!g_deathOk || !g_chaseOk || k < 0 || k > 1 || !g_chaseMade[k] || g_deathOn[k])
        return;
    {
        const uintptr_t ch = LocalCharacter(k + 2);
        if (ch && ReadU32(ch + 0xB4))
            return; // (his character has a unit: not his death)
    }
    void* const d = g_death[k];
    g_deathCtor(d, nullptr, mgr, k + 2); // its fields for this camera manager (made again for each death)
    if (!g_deathUpdate)
    {
        char cls[64] = "";
        ClassName(reinterpret_cast<uintptr_t>(d), cls, sizeof(cls));
        const uintptr_t vt = *reinterpret_cast<const uintptr_t*>(d);
        if (strcmp(cls, "DeathCamera") != 0)
        {
            Log("[death34] player %d's death camera is a \"%s\" (vtable %08X), not a DeathCamera - death cameras OFF", k + 3, cls,
                static_cast<uint32_t>(vt));
            g_deathOk = false;
            return;
        }
        g_deathUpdate = *reinterpret_cast<ChaseUpdate_t*>(vt + 4);
        g_deathApply = *reinterpret_cast<ChaseApply_t*>(vt + 8);
    }
    g_deathSetup(d, nullptr, target, killer, g_chase[k] + kCamObjMatrix);
    g_deathMgr[k] = mgr;
    InterlockedExchange(&g_deathOn[k], 1);
    InterlockedIncrement(&g_deathStarts);
    if (DeathLog())
        Log("[death34] %lu ms: player %d's DEATH CAMERA started (target %08X, killer %08X, from his chase camera's matrix) - "
            "view %d shows it until he has a soldier again",
            ScanMsSinceAttach(), k + 3, target, killer, k + 3);
}

void DeathStop34(int k, const char* why)
{
    if (k < 0 || k > 1 || !g_deathOn[k])
        return;
    InterlockedExchange(&g_deathOn[k], 0);
    if (DeathLog())
        Log("[death34] %lu ms: player %d's death camera off (%s)", ScanMsSinceAttach(), k + 3, why);
}

bool FindDeath()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kPatDeathMake, hits, 4);
    const uint32_t ctor = n == 1 ? CallTarget(hits[0] + kDeathMakeCtor) : 0;
    const bool ctorOk = ctor && MatchPatternAt(ctor, kDeathCtorBody);
    uint32_t h2[4] = {};
    const int n2 = FindPattern(kPatCamDeathFull, h2, 4);
    const uint32_t getter = n2 == 1 ? CallTarget(h2[0] + kCamDeathMatrixCall) : 0;
    const uint32_t setup = n2 == 1 ? CallTarget(h2[0] + kCamDeathSetupCall) : 0;
    const bool getterOk = getter && MatchPatternAt(getter, kChaseMatrixBody);
    const bool setupOk = setup && MatchPatternAt(setup, kDeathSetupBody);
    Log("[death34] DeathCamera constructor rva 0x%08X %s (%d hit(s) of its making in the camera manager); the death camera "
        "start (%d hit(s)): the chase camera's matrix getter rva 0x%08X %s, DeathCamera::Setup rva 0x%08X %s",
        ctor, ctorOk ? "OK" : "NOT OK", n, n2, getter, getterOk ? "OK (+10h)" : "NOT OK", setup, setupOk ? "OK" : "NOT OK");
    if (!ctorOk || !getterOk || !setupOk)
        return false;
    g_deathCtor = reinterpret_cast<DeathCtor_t>(Rva(ctor));
    g_deathSetup = reinterpret_cast<DeathSetup_t>(Rva(setup));
    return true;
}

void ChaseTargetFromGame(int i, uint32_t target)
{
    if (i == 2 || i == 3)
    {
        g_chaseWanted[i - 2] = target;
        if (target)
            DeathStop34(i - 2, "the game gave his chase camera a soldier"); // 20g-r6: as SetChaseTarget does for 1 / 2
    }
}

bool FindChase()
{
    uint32_t hits[4] = {};
    int n = FindPattern(kPatChaseMake, hits, 4);
    const uint32_t ctor = n == 1 ? CallTarget(hits[0] + kChaseMakeCtor) : 0;
    uint32_t chaseSet = 0;
    {
        uint32_t h2[4] = {};
        const int n2 = FindPattern(kPatCamChase, h2, 4);
        if (n2 == 1 && MatchPatternAt(h2[0] + kChaseSetTargetAt, kChaseSetTargetSite))
            chaseSet = CallTarget(h2[0] + kChaseSetTargetAt + 5);
    }
    const bool ctorOk = ctor && MatchPatternAt(ctor, kChaseCtorBody);
    const bool setOk = chaseSet && MatchPatternAt(chaseSet, kChaseSetTargetBody);
    Log("[chase34] ChaseCamera constructor rva 0x%08X %s (%d hit(s) of its making in the camera manager), SetTarget rva "
        "0x%08X %s", ctor, ctorOk ? "OK" : "NOT OK", n, chaseSet, setOk ? "OK" : "NOT OK");
    if (!ctorOk || !setOk)
        return false;
    g_chaseCtor = reinterpret_cast<ChaseCtor_t>(Rva(ctor));
    g_chaseSetTarget = reinterpret_cast<ChaseSetTarget_t>(Rva(chaseSet));
    return true;
}

bool CamGuarded(void* mgr, int i)
{
    return g_playersOn && i >= 2 && reinterpret_cast<uintptr_t>(mgr) == CameraManager();
}

uintptr_t __fastcall CamGetWrap(void* mgr, void* edx, int i)
{
    (void)edx;
    if (CamGuarded(mgr, i))
    {
        InterlockedIncrement(&g_camGuardHit[0]);
        P34Guard("camera manager GetCamera", 20u, BF1_RETURN_ADDRESS(), i);
        const int k = i - 2;
        if (k < 2 && g_specMade[k])
            return reinterpret_cast<uintptr_t>(g_specCam[k]);
        return g_origCamGet(mgr, nullptr, 0);
    }
    return g_origCamGet(mgr, nullptr, i);
}

uint32_t __fastcall CamChaseWrap(void* mgr, void* edx, uint32_t target, int i)
{
    (void)edx;
    if (CamGuarded(mgr, i))
    {
        InterlockedIncrement(&g_camGuardHit[1]);
        P34Guard("camera manager SetChaseTarget", 21u, BF1_RETURN_ADDRESS(), i);
        ChaseTargetFromGame(i, target); // 20e-r2: player 3 / 4's own chase camera
        return 0;
    }
    return g_origCamChase(mgr, nullptr, target, i);
}

uint32_t __fastcall CamDeathWrap(void* mgr, void* edx, uint32_t a, int i, uint32_t c)
{
    (void)edx;
    if (CamGuarded(mgr, i))
    {
        InterlockedIncrement(&g_camGuardHit[2]);
        P34Guard("camera manager death camera", 22u, BF1_RETURN_ADDRESS(), i);
        if (g_deathWanted && (i == 2 || i == 3))
            DeathStart34(i - 2, reinterpret_cast<uintptr_t>(mgr), a, c); // 20g-r6: his own death camera
        return 0;
    }
    return g_origCamDeath(mgr, nullptr, a, i, c);
}

// 20g-r7: players 3 and 4's own wall distance. The soldier's third-person camera (GOG 1.3 0x00222FE0) eases its
// distance from the wall through this function with i = the camera the manager is working on ([cm + 7Ch]) - 0 after
// the manager's loop, when players 3 / 4's chase cameras run: they shared player 1's [cm + 18h]. While PlayerChase
// runs player 3's / 4's camera (g_wallFor = 0 / 1) the slot holds his own distance for the call.
constexpr uint32_t kCamWallDist = 0x18;   // [cm + 18h + i*4] (kPatCamFade: F3 0F 10 54 BE 18)
bool g_wallWanted = true;                 // [Splitscreen] WallCamP34
int g_wallFor = -1;                       // PlayerChase is running player 3's (0) / 4's (1) camera
float g_wallDist[2] = { 1.0f, 1.0f };     // as the manager's: 1.0 when the chase camera gets a target
volatile LONG g_wallOwn = 0;              // eases made on players 3 / 4's own distance

float __fastcall CamFadeWrap(void* mgr, void* edx, int i, float to, float rate)
{
    (void)edx;
    if (g_wallFor >= 0 && g_wallFor <= 1 && g_wallWanted && (i == 0 || i == 1) &&
        reinterpret_cast<uintptr_t>(mgr) == CameraManager())
    {
        float* const slot =
            reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(mgr) + kCamWallDist + static_cast<uintptr_t>(i) * 4);
        const float keep = *slot;
        *slot = g_wallDist[g_wallFor];
        const float r = g_origCamFade(mgr, nullptr, i, to, rate);
        g_wallDist[g_wallFor] = *slot;
        *slot = keep;
        InterlockedIncrement(&g_wallOwn);
        return r;
    }
    if (CamGuarded(mgr, i))
    {
        InterlockedIncrement(&g_camGuardHit[3]);
        P34Guard("camera manager fade", 23u, BF1_RETURN_ADDRESS(), i);
        return to;
    }
    return g_origCamFade(mgr, nullptr, i, to, rate);
}

// One function: found once, one jump thunk and no direct calls, moved to its guard.
bool CamGuardOne(const char* name, const char* pat, const void* wrap, uintptr_t* orig)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pat, hits, 4);
    if (n != 1)
    {
        Log("[players] %s: %d pattern hits (need 1) - Players NOT installed", name, n);
        return false;
    }
    uint32_t thunks[4] = {};
    int direct = 0;
    const int nt = FindThunks(hits[0], thunks, 4, &direct);
    if (nt != 1 || direct)
    {
        Log("[players] %s rva 0x%08X: %d jump thunks, %d direct calls (need 1, 0) - Players NOT installed",
            name, hits[0], nt, direct);
        return false;
    }
    char what[64];
    snprintf(what, sizeof(what), "%s guard", name);
    return MoveThunk(what, hits[0], wrap, orig);
}

bool InstallCamGuards()
{
    uintptr_t o[4] = {};
    if (!CamGuardOne("camera manager GetCamera", kPatCamGet, reinterpret_cast<const void*>(&CamGetWrap), &o[0]) ||
        !CamGuardOne("camera manager SetChaseTarget", kPatCamChase, reinterpret_cast<const void*>(&CamChaseWrap), &o[1]) ||
        !CamGuardOne("camera manager death camera", kPatCamDeath, reinterpret_cast<const void*>(&CamDeathWrap), &o[2]) ||
        !CamGuardOne("camera manager fade", kPatCamFade, reinterpret_cast<const void*>(&CamFadeWrap), &o[3]))
        return false;
    g_origCamGet = reinterpret_cast<CamGet_t>(o[0]);
    g_origCamChase = reinterpret_cast<CamChase_t>(o[1]);
    g_origCamDeath = reinterpret_cast<CamDeath_t>(o[2]);
    g_origCamFade = reinterpret_cast<CamFade_t>(o[3]);
    Log("[players] camera manager guards: SetChaseTarget, death camera and fade do nothing for camera 2 / 3 (players 3 "
        "and 4 - its arrays have 2 slots, slot 2 of the fade times is its camera count); GetCamera(2 / 3) answers "
        "BF1GameExt's own camera of view 3 / 4");
    g_chaseOk = FindChase(); // 20e-r2 (without it: the 20d follow camera)
    // 20g-r6: players 3 and 4's death cameras
    if (!g_deathWanted)
        Log("[death34] [Splitscreen] DeathCamP34=0 - players 3 and 4 keep their last chase camera picture when they die");
    else
    {
        g_deathOk = g_chaseOk && FindDeath();
        Log("[death34] players 3 and 4's death cameras %s", g_deathOk ? "ON - the game's DeathCamera, as player 1's"
                                                                      : "OFF (something above did not check out)");
    }
    return true;
}

// 20d-r3: GetMessageDisplay(player, k) = MessageDisplay[player * 2 + k] (GOG 1.3 0x0018B0E0, cdecl, 9 call sites
// through one jump thunk): players 3 / 4 get player 1's / 2's.
const char* const kPatMsgGet = "55 8B EC 8B 4D 0C 8B 45 08 8D 04 41 8B 04 85 ?? ?? ?? ?? 5D C3";
typedef uint32_t(__cdecl* MsgGet_t)(int32_t player, int32_t k);
MsgGet_t g_origMsgGet = nullptr;

uint32_t __cdecl MsgGetWrap(int32_t player, int32_t k)
{
    if (g_playersOn && (player == 2 || player == 3))
    {
        InterlockedIncrement(&g_hudAliased);
        P34Guard("GetMessageDisplay (lent)", 30u, BF1_RETURN_ADDRESS(), player);
        player -= 2;
    }
    return g_origMsgGet(player, k);
}

bool InstallMsgAlias()
{
    uintptr_t o = 0;
    if (!CamGuardOne("GetMessageDisplay", kPatMsgGet, reinterpret_cast<const void*>(&MsgGetWrap), &o))
        return false;
    g_origMsgGet = reinterpret_cast<MsgGet_t>(o);
    return true;
}

// 20d-r5: the input manager's force feedback (rumble) functions - thiscall (net id, ...), the net id turned into a
// local index that indexes the manager's ONE rumble object (+748h, 34h) and ONE PlayerInput (+1Ch, 718h) inline.
// GOG 1.3: 0x00135B60 / 0x00135C80 / 0x00135ED0 (A), 0x00135A70 / 0x00135D00 (B), 0x00135DD0 (C) - each calls the
// rumble object's test (0x00135A50) right after the lookup; 0x00135940 / 0x001359A0 (D) index it directly.
const char* const kPatRumbleA = "55 8B EC 57 8B F9 E8 ?? ?? ?? ?? 84 C0 74 ?? E8 ?? ?? ?? ?? 84 C0 74 ?? 56 FF 75 08 E8 "
                                "?? ?? ?? ?? 8B F0 83 C4 04 85 F6 78 ?? 56 8B CF E8";
const char* const kPatRumbleB = "55 8B EC 57 8B F9 E8 ?? ?? ?? ?? 84 C0 74 ?? E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? 56 FF "
                                "75 08 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 78 ?? 56 8B CF E8";
const char* const kPatRumbleC = "55 8B EC 57 8B F9 E8 ?? ?? ?? ?? 84 C0 74 ?? E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? 56 FF "
                                "75 08 E8 ?? ?? ?? ?? 8B F0 83 C4 04 85 F6 0F 88 ?? ?? ?? ?? 56 8B CF E8";
const char* const kPatRumbleD = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 84 C0 74 0E E8 ?? ?? ?? ?? 84 C0 75 05 5E 5D C2 04 00 FF "
                                "75 08 E8 ?? ?? ?? ?? 6B C0 34 8D 8E 48 07 00 00";
const char* const kPatRumbleTest = "55 8B EC 6B 45 08 34 81 C1 48 07 00 00 03 C8 E8 ?? ?? ?? ?? 5D C2 04 00";
constexpr int kRumbleHits[4] = { 3, 2, 1, 2 }; // GOG 1.3
constexpr int kMaxRumble = 8;

uintptr_t g_rumbleOrig[kMaxRumble] = {};
uint32_t g_rumbleRva[kMaxRumble] = {};
int g_rumbleArgs[kMaxRumble] = {};
volatile LONG g_rumbleGuarded = 0;
int g_rumbleCount = 0;

// The net id belongs to local player 3 / 4 (our local slots).
int RumbleGuardedPlayer(uint32_t netId)
{
    if (!g_playersOn || static_cast<int32_t>(netId) < 0)
        return -1;
    for (int k = 2; k < 4; ++k)
        if (g_netSlots4[k] == static_cast<int32_t>(netId))
            return k;
    return -1;
}

#define BF1_RUMBLE_GUARD(K)                                                                                         \
    uint32_t __fastcall RumbleG1_##K(void* m, void*, uint32_t a0)                                                  \
    {                                                                                                               \
        const int p = RumbleGuardedPlayer(a0);                                                                      \
        if (p >= 0)                                                                                                 \
        {                                                                                                           \
            InterlockedIncrement(&g_rumbleGuarded);                                                                 \
            P34Guard("input manager rumble", 40u + K, BF1_RETURN_ADDRESS(), p);                                     \
            return 0;                                                                                               \
        }                                                                                                           \
        return reinterpret_cast<uint32_t(__fastcall*)(void*, void*, uint32_t)>(g_rumbleOrig[K])(m, nullptr, a0);    \
    }                                                                                                               \
    uint32_t __fastcall RumbleG2_##K(void* m, void*, uint32_t a0, uint32_t a1)                                     \
    {                                                                                                               \
        const int p = RumbleGuardedPlayer(a0);                                                                      \
        if (p >= 0)                                                                                                 \
        {                                                                                                           \
            InterlockedIncrement(&g_rumbleGuarded);                                                                 \
            P34Guard("input manager rumble", 40u + K, BF1_RETURN_ADDRESS(), p);                                     \
            return 0;                                                                                               \
        }                                                                                                           \
        return reinterpret_cast<uint32_t(__fastcall*)(void*, void*, uint32_t, uint32_t)>(g_rumbleOrig[K])(          \
            m, nullptr, a0, a1);                                                                                    \
    }                                                                                                               \
    uint32_t __fastcall RumbleG6_##K(void* m, void*, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,           \
                                     uint32_t a4, uint32_t a5)                                                      \
    {                                                                                                               \
        const int p = RumbleGuardedPlayer(a0);                                                                      \
        if (p >= 0)                                                                                                 \
        {                                                                                                           \
            InterlockedIncrement(&g_rumbleGuarded);                                                                 \
            P34Guard("input manager rumble", 40u + K, BF1_RETURN_ADDRESS(), p);                                     \
            return 0;                                                                                               \
        }                                                                                                           \
        return reinterpret_cast<uint32_t(__fastcall*)(void*, void*, uint32_t, uint32_t, uint32_t, uint32_t,         \
                                                      uint32_t, uint32_t)>(g_rumbleOrig[K])(m, nullptr, a0, a1, a2, \
                                                                                            a3, a4, a5);            \
    }                                                                                                               \
    uint32_t __fastcall RumbleG7_##K(void* m, void*, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,           \
                                     uint32_t a4, uint32_t a5, uint32_t a6)                                         \
    {                                                                                                               \
        const int p = RumbleGuardedPlayer(a0);                                                                      \
        if (p >= 0)                                                                                                 \
        {                                                                                                           \
            InterlockedIncrement(&g_rumbleGuarded);                                                                 \
            P34Guard("input manager rumble", 40u + K, BF1_RETURN_ADDRESS(), p);                                     \
            return 0;                                                                                               \
        }                                                                                                           \
        return reinterpret_cast<uint32_t(__fastcall*)(void*, void*, uint32_t, uint32_t, uint32_t, uint32_t,         \
                                                      uint32_t, uint32_t, uint32_t)>(g_rumbleOrig[K])(              \
            m, nullptr, a0, a1, a2, a3, a4, a5, a6);                                                                \
    }                                                                                                               \
    uint32_t __fastcall RumbleG9_##K(void* m, void*, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3,           \
                                     uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7, uint32_t a8)               \
    {                                                                                                               \
        const int p = RumbleGuardedPlayer(a0);                                                                      \
        if (p >= 0)                                                                                                 \
        {                                                                                                           \
            InterlockedIncrement(&g_rumbleGuarded);                                                                 \
            P34Guard("input manager rumble", 40u + K, BF1_RETURN_ADDRESS(), p);                                     \
            return 0;                                                                                               \
        }                                                                                                           \
        return reinterpret_cast<uint32_t(__fastcall*)(void*, void*, uint32_t, uint32_t, uint32_t, uint32_t,         \
                                                      uint32_t, uint32_t, uint32_t, uint32_t, uint32_t)>(           \
            g_rumbleOrig[K])(m, nullptr, a0, a1, a2, a3, a4, a5, a6, a7, a8);                                       \
    }

BF1_RUMBLE_GUARD(0)
BF1_RUMBLE_GUARD(1)
BF1_RUMBLE_GUARD(2)
BF1_RUMBLE_GUARD(3)
BF1_RUMBLE_GUARD(4)
BF1_RUMBLE_GUARD(5)
BF1_RUMBLE_GUARD(6)
BF1_RUMBLE_GUARD(7)
#undef BF1_RUMBLE_GUARD

#define BF1_RUMBLE_CASE(K)                                                                                          \
    case K:                                                                                                         \
        return args == 1   ? reinterpret_cast<const void*>(&RumbleG1_##K)                                           \
               : args == 2 ? reinterpret_cast<const void*>(&RumbleG2_##K)                                           \
               : args == 6 ? reinterpret_cast<const void*>(&RumbleG6_##K)                                           \
               : args == 7 ? reinterpret_cast<const void*>(&RumbleG7_##K)                                           \
               : args == 9 ? reinterpret_cast<const void*>(&RumbleG9_##K)                                           \
                           : nullptr;

const void* RumbleWrapFor(int k, int args)
{
    switch (k)
    {
        BF1_RUMBLE_CASE(0)
        BF1_RUMBLE_CASE(1)
        BF1_RUMBLE_CASE(2)
        BF1_RUMBLE_CASE(3)
        BF1_RUMBLE_CASE(4)
        BF1_RUMBLE_CASE(5)
        BF1_RUMBLE_CASE(6)
        BF1_RUMBLE_CASE(7)
    default:
        return nullptr;
    }
}
#undef BF1_RUMBLE_CASE

// Its arguments, from its first "pop esi; pop edi; pop ebp; ret N" (A / B / C); D: 1 (ret 4 in the pattern).
int RumbleArgs(uint32_t fn, bool kindD)
{
    if (kindD)
        return 1;
    const unsigned char* c = reinterpret_cast<const unsigned char*>(Rva(fn));
    for (uint32_t i = 0; i < 0x200; ++i)
        if (c[i] == 0x5E && c[i + 1] == 0x5F && c[i + 2] == 0x5D && c[i + 3] == 0xC2 && c[i + 5] == 0x00)
            return (c[i + 4] % 4 == 0) ? c[i + 4] / 4 : -1;
    return -1;
}

bool InstallRumbleGuards()
{
    uint32_t test = 0;
    {
        uint32_t hits[4] = {};
        const int n = FindPattern(kPatRumbleTest, hits, 4);
        if (n != 1)
        {
            Log("[players] input manager rumble test: %d pattern hits (need 1) - Players NOT installed", n);
            return false;
        }
        test = hits[0];
    }
    const char* const pats[4] = { kPatRumbleA, kPatRumbleB, kPatRumbleC, kPatRumbleD };
    uint32_t fns[kMaxRumble] = {};
    bool isD[kMaxRumble] = {};
    int total = 0;
    for (int k = 0; k < 4; ++k)
    {
        uint32_t hits[8] = {};
        const int n = FindPattern(pats[k], hits, 8);
        if (n != kRumbleHits[k])
        {
            Log("[players] input manager rumble pattern %c: %d hits (GOG 1.3: %d) - Players NOT installed", 'A' + k, n,
                kRumbleHits[k]);
            return false;
        }
        for (int i = 0; i < n && total < kMaxRumble; ++i)
        {
            if (k < 3)
            {
                // the call at the pattern's end must be the rumble object's test
                uint32_t len = 0;
                for (const char* q = pats[k]; *q; ++q)
                    if (*q == ' ')
                        ++len;
                const uint32_t callAt = hits[i] + len; // the E8 is the last token
                if (CallTarget(callAt) != test)
                {
                    Log("[players] input manager rumble rva 0x%08X: its test call goes to rva 0x%08X, not 0x%08X - "
                        "Players NOT installed", hits[i], CallTarget(callAt), test);
                    return false;
                }
            }
            isD[total] = k == 3;
            fns[total++] = hits[i];
        }
    }
    for (int i = 0; i < total; ++i)
    {
        const int args = RumbleArgs(fns[i], isD[i]);
        const void* wrap = RumbleWrapFor(i, args);
        uint32_t thunks[4] = {};
        int direct = 0;
        const int nt = FindThunks(fns[i], thunks, 4, &direct);
        if (!wrap || nt != 1 || direct)
        {
            Log("[players] input manager rumble rva 0x%08X: %d arguments, %d jump thunks, %d direct calls - Players NOT "
                "installed", fns[i], args, nt, direct);
            return false;
        }
        uintptr_t orig = 0;
        char name[48];
        snprintf(name, sizeof(name), "input manager rumble %d", i);
        if (!MoveThunk(name, fns[i], wrap, &orig))
            return false;
        g_rumbleOrig[i] = orig;
        g_rumbleRva[i] = fns[i];
        g_rumbleArgs[i] = args;
        g_rumbleCount = i + 1;
    }
    Log("[players] input manager rumble: %d functions (GOG 1.3: 8; the rumble test rva 0x%08X) do nothing for players 3 "
        "and 4 - their arguments %d %d %d %d %d %d %d %d", g_rumbleCount, test, g_rumbleArgs[0], g_rumbleArgs[1],
        g_rumbleArgs[2], g_rumbleArgs[3], g_rumbleArgs[4], g_rumbleArgs[5], g_rumbleArgs[6], g_rumbleArgs[7]);
    return true;
}

// 20d-r6: the screen filter (damage tint) and the motion blur, for players 3 and 4.
const char* const kPatFilterFlash = "55 8B EC F3 0F 10 55 10 0F 57 C0 53 8B 5D 08 56 8B F3 C1 E6 05 0F 2F D0 57 8B F9";
const char* const kPatFilterDraw = "55 8B EC F7 45 08 00 00 20 00 57 8B F9 75 19 E8 ?? ?? ?? ?? C1 E0 05 83 7C 38 38 00";
const char* const kPatBlurOn =
    "55 8B EC 8B 45 10 F3 0F 10 45 0C C7 04 85 ?? ?? ?? ?? 01 00 00 00 F3 0F 11 04 85 ?? ?? ?? ?? 5D C3";
const char* const kPatBlurOff =
    "55 8B EC 8B 45 0C C7 04 85 ?? ?? ?? ?? 00 00 00 00 C7 04 85 ?? ?? ?? ?? 00 00 00 00 5D C3";

typedef uint32_t(__fastcall* FilterFlash_t)(void* self, void* edx, int32_t local, uint32_t a1, uint32_t a2, uint32_t a3,
                                            uint32_t a4, uint32_t a5);
typedef uint32_t(__fastcall* FilterDraw_t)(void* self, void* edx, uint32_t flags);
typedef uint32_t(__cdecl* BlurOn_t)(uint32_t a0, uint32_t amount, int32_t local);
typedef uint32_t(__cdecl* BlurOff_t)(uint32_t a0, int32_t local);

FilterFlash_t g_origFilterFlash = nullptr;
FilterDraw_t g_origFilterDraw = nullptr;
BlurOn_t g_origBlurOn = nullptr;
BlurOff_t g_origBlurOff = nullptr;
volatile LONG g_filterGuarded = 0;    // Flash for players 3 / 4 not made
volatile LONG g_filterDrawSkipped = 0; // draws skipped in views 3 / 4
volatile LONG g_blurGuarded = 0;

// 20g-r7: players 3 and 4's own damage tint. The filter's 2 records (20h bytes each at +38h: state, times, value,
// colour) are the game's; each of players 3 / 4 has one of ours. Flash for local 2 / 3 writes his (in slot 0 for the
// call); the draw in view 3 / 4 runs the filter's own update (vtable +24h, GOG 1.3 0x00208910: both slots from the
// time - nothing else) and draw with his in slot 0 and the renderer's current view 0 for the call (the draw reads the
// view only to pick the record: 0x002086E0 / 0x00073FF0).
constexpr uint32_t kFilterRecs = 0x38;  // kPatFilterDraw: C1 E0 05 83 7C 38 38 00
constexpr uint32_t kFilterRecBytes = 0x20;
constexpr uint32_t kFilterUpdateSlot = 0x24;
const char* const kFilterUpdateBody =
    "55 8B EC 51 56 8B F1 8D 4E 28 E8 ?? ?? ?? ?? F3 0F 10 25 ?? ?? ?? ?? 83 C6 38 D9 5D FC F3 0F 10 5D FC B9 02 00 00 00";
typedef void(__fastcall* FilterUpdate_t)(void* self, void* edx, uint32_t unused);
bool g_tintWanted = true;              // [Splitscreen] TintP34
int g_tintUpdateState = 0;             // 0 = not looked at yet, 1 = OK, -1 = not the function expected (tint off)
FilterUpdate_t g_filterUpdate = nullptr;
alignas(4) unsigned char g_tintRec[2][kFilterRecBytes] = {};
volatile LONG g_tintFlashes = 0;       // tints flashed for players 3 / 4
volatile LONG g_tintDraws = 0;         // their views drawn with their own record

void* g_tintFilter = nullptr;          // the filter the records belong to (a new one: the records start empty)

void TintFor(void* self)
{
    if (self != g_tintFilter)
    {
        g_tintFilter = self;
        memset(g_tintRec, 0, sizeof(g_tintRec));
    }
}

void TintSwap(void* self, int k)
{
    unsigned char* const rec0 = reinterpret_cast<unsigned char*>(self) + kFilterRecs;
    unsigned char tmp[kFilterRecBytes];
    memcpy(tmp, rec0, kFilterRecBytes);
    memcpy(rec0, g_tintRec[k], kFilterRecBytes);
    memcpy(g_tintRec[k], tmp, kFilterRecBytes);
}

// the filter's update (vtable +24h), checked once (its body: the 2-slot loop from +38h)
bool TintUpdateReady(void* self)
{
    if (g_tintUpdateState == 0)
    {
        const uintptr_t vt = ReadU32(reinterpret_cast<uintptr_t>(self));
        const uintptr_t fn = InImage(vt) ? ReadU32(vt + kFilterUpdateSlot) : 0;
        uint32_t rva = InImage(fn) ? static_cast<uint32_t>(fn - g_base) : 0;
        for (int hop = 0; rva && hop < 3 && CodeByte(rva) == 0xE9; ++hop)
            rva = RelTarget(rva);
        const bool ok = rva && MatchPatternAt(rva, kFilterUpdateBody);
        g_tintUpdateState = ok ? 1 : -1;
        if (ok)
            g_filterUpdate = reinterpret_cast<FilterUpdate_t>(fn);
        Log("[players] 20g-r7: the screen filter's update (vtable +24h) rva 0x%08X %s", rva,
            ok ? "OK - players 3 and 4's damage tint ON (a record of their own each)" : "NOT the one expected - their tint stays off");
    }
    return g_tintUpdateState == 1;
}

uint32_t __fastcall FilterFlashWrap(void* self, void* edx, int32_t local, uint32_t a1, uint32_t a2, uint32_t a3,
                                    uint32_t a4, uint32_t a5)
{
    (void)edx;
    if (g_playersOn && (local == 2 || local == 3))
    {
        InterlockedIncrement(&g_filterGuarded);
        P34Guard("screen filter Flash (damage tint)", 50u, BF1_RETURN_ADDRESS(), local);
        if (g_tintWanted && TintUpdateReady(self))
        {
            const int k = local - 2;
            TintFor(self);
            TintSwap(self, k);
            const uint32_t r = g_origFilterFlash(self, nullptr, 0, a1, a2, a3, a4, a5);
            TintSwap(self, k);
            InterlockedIncrement(&g_tintFlashes);
            return r;
        }
        return 0;
    }
    return g_origFilterFlash(self, nullptr, local, a1, a2, a3, a4, a5);
}

uint32_t __fastcall FilterDrawWrap(void* self, void* edx, uint32_t flags)
{
    (void)edx;
    if (g_playersOn && g_rendCams)
    {
        const uint32_t view = ReadU32(g_rendCams + 0x10); // the renderer's current view
        if (view == 2 || view == 3)
        {
            int k = -1;
            for (int j = 0; j < 2; ++j)
                if (PlayerNSpecView(j) == static_cast<int>(view) && static_cast<int>(g_players) >= j + 3)
                    k = j;
            if (k < 0 || !g_tintWanted || (flags & 0x200000) || !TintUpdateReady(self))
            {
                InterlockedIncrement(&g_filterDrawSkipped);
                return 0;
            }
            uint32_t* const cur = reinterpret_cast<uint32_t*>(g_rendCams + 0x10);
            TintFor(self);
            TintSwap(self, k);
            g_filterUpdate(self, nullptr, 0);
            *cur = 0;
            const uint32_t r = g_origFilterDraw(self, nullptr, flags);
            *cur = view;
            TintSwap(self, k);
            InterlockedIncrement(&g_tintDraws);
            return r;
        }
    }
    return g_origFilterDraw(self, nullptr, flags);
}

uint32_t __cdecl BlurOnWrap(uint32_t a0, uint32_t amount, int32_t local)
{
    if (g_playersOn && (local == 2 || local == 3))
    {
        InterlockedIncrement(&g_blurGuarded);
        P34Guard("motion blur on", 51u, BF1_RETURN_ADDRESS(), local);
        return 0;
    }
    return g_origBlurOn(a0, amount, local);
}

uint32_t __cdecl BlurOffWrap(uint32_t a0, int32_t local)
{
    if (g_playersOn && (local == 2 || local == 3))
    {
        InterlockedIncrement(&g_blurGuarded);
        P34Guard("motion blur off", 52u, BF1_RETURN_ADDRESS(), local);
        return 0;
    }
    return g_origBlurOff(a0, local);
}

bool InstallFilterGuards()
{
    uintptr_t o[4] = {};
    if (!CamGuardOne("screen filter Flash", kPatFilterFlash, reinterpret_cast<const void*>(&FilterFlashWrap), &o[0]) ||
        !CamGuardOne("screen filter draw", kPatFilterDraw, reinterpret_cast<const void*>(&FilterDrawWrap), &o[1]) ||
        !CamGuardOne("motion blur on", kPatBlurOn, reinterpret_cast<const void*>(&BlurOnWrap), &o[2]) ||
        !CamGuardOne("motion blur off", kPatBlurOff, reinterpret_cast<const void*>(&BlurOffWrap), &o[3]))
        return false;
    g_origFilterFlash = reinterpret_cast<FilterFlash_t>(o[0]);
    g_origFilterDraw = reinterpret_cast<FilterDraw_t>(o[1]);
    g_origBlurOn = reinterpret_cast<BlurOn_t>(o[2]);
    g_origBlurOff = reinterpret_cast<BlurOff_t>(o[3]);
    Log("[players] screen filter (damage tint) Flash for players 3 and 4: %s (the filter holds 2 records); motion blur on "
        "/ off do nothing for them (2 slots)",
        g_tintWanted ? "a record of their own each, drawn in views 3 / 4 (20g-r7; the filter's update is checked at the "
                       "first use)"
                     : "nothing ([Splitscreen] TintP34=0 - views 3 / 4 draw no tint)");
    return true;
}

// Character with player index p (2 / 3), or 0.
uintptr_t LocalCharacter(int p)
{
    uintptr_t chars = 0;
    uint32_t count = 0;
    if (!SpawnScanCharacters(&chars, &count))
        return 0;
    for (uint32_t c = 0; c < count && c < 64; ++c)
    {
        const uintptr_t ch = chars + static_cast<uintptr_t>(c) * 0xE8;
        if (static_cast<int>(ReadU32(ch + 0x9C)) == p)
            return ch;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
// 20g-r7: COMMAND POST LIGHTS, PLAYERS 3 / 4'S SCOPE BLUR, STILL DEATH CAMERAS, [fx34]
// ---------------------------------------------------------------------------------------------------------
// (1) An EntityHologram of class mode 1 ("align": the command posts' holograms) has two RedOmniLights, +88h and +8Ch,
// made at its creation (GOG 1.3 0x000EF788: +8Ch left out of view 0, +88h out of view 1 - the light's byte +4Ch is the
// views it is NOT drawn in, the renderer's test 0x002B3090) and coloured by its update (0x000F1ED0, vtable slot 12)
// every frame for CharIndex(0) / CharIndex(1) - the team colours players 1 / 2 see. Views 3 / 4 got both. The update
// goes through BF1GameExt: (a) when players 1 and 2 are on the same team and player 3 / 4 is not, the update's
// CharIndex(1) answers his slot (the net local slot getter's census wrapper, while the update runs on this thread) -
// the second light takes his team's colour; (b) after it, each player's view leaves out the light that is not his
// team's colour.
const char* const kPatHoloUpdate = "55 8B EC 83 EC 10 F3 0F 10 5D 08 53 56 8B F1 F3 0F 10 0D ?? ?? ?? ?? 57 F3 0F 10 86 94 00 00 00 "
                                   "F3 0F 10 A6 98 00 00 00 F3 0F 58 C3 F3 0F 10 96 9C 00 00 00 F3 0F 58 E3 8B 7E 04";
const char* const kPatHoloMode = "8B 87 8C 00 00 00 83 F8 02 0F 85 ?? ?? ?? ?? 33 DB";            // its class's mode +8Ch
const char* const kPatHoloAlign = "33 FF 57 E8 ?? ?? ?? ?? 83 C4 04 85 C0 78 18 57 E8 ?? ?? ?? ?? 83 C4 04 50 E8 ?? ?? ?? ?? "
                                  "8B C8 E8";                                                     // CharIndex(i), i < 2
const char* const kPatHoloLights = "6A 01 6A 00 8B C8 89 86 8C 00 00 00 E8 ?? ?? ?? ?? 8B 8E 88 00 00 00 68 ?? ?? ?? ?? E8 "
                                   "?? ?? ?? ?? 8B 8E 88 00 00 00 6A 01 6A 01 E8";
constexpr uint32_t kHoloLightsNotIn = 12; // its E8: the light's "not in view" bit (0x000F0680)
const char* const kLightNotInBody = "55 8B EC 80 7D 0C 00 0F B6 51 4C 8B 45 08 74 0A 0F AB C2 88 51 4C";
const char* const kPatLightViewTest = "55 8B EC 8B D1 B8 01 00 00 00 8B 4D 08 D3 E0 84 42 4C 0F 95 C0 5D C2 04 00";
constexpr uint32_t kHoloClass = 0x04;
constexpr uint32_t kHoloClassMode = 0x8C;
constexpr uint32_t kHoloLight[2] = { 0x88, 0x8C };
constexpr uint32_t kLightNotIn = 0x4C;
constexpr uint32_t kCharTeamObj = 0xA4; // the character's team object (the colours it sees: GOG 1.3 0x000775B0)

typedef void(__fastcall* HoloUpdate_t)(void* self, void* edx, float dt);
bool g_cpWanted = true;                 // [Splitscreen] CpLightsP34
bool g_cpOk = false;
HoloUpdate_t g_origHoloUpdate = nullptr;
volatile LONG g_holoUpdates = 0;        // align holograms seen with 3+ players
volatile LONG g_holoRemapped = 0;       // ... with a light coloured for player 3 / 4
volatile LONG g_holoMasks = 0;          // light masks changed

uint32_t TeamOfPlayer(int p)
{
    const uintptr_t ch = LocalCharacter(p);
    return ch ? ReadU32(ch + kCharTeamObj) : 0;
}

// the renderer view player p is drawn in (-1 = none)
int ViewOfPlayer(int p, uintptr_t cm)
{
    if (p < 2)
    {
        const int v = cm ? static_cast<int>(ReadU32(cm + kCamViews + static_cast<uintptr_t>(p) * 4)) : -1;
        return (v >= 0 && v < 4) ? v : -1;
    }
    return PlayerNSpecView(p - 2);
}

void __fastcall HoloUpdateWrap(void* self, void* edx, float dt)
{
    (void)edx;
    const uintptr_t h = reinterpret_cast<uintptr_t>(self);
    const uintptr_t cls = ReadU32(h + kHoloClass);
    const int players = static_cast<int>(g_players);
    if (!g_cpOk || !g_playersOn || players < 3 || !cls || ReadU32(cls + kHoloClassMode) != 1)
    {
        g_origHoloUpdate(self, nullptr, dt);
        return;
    }
    const uintptr_t light[2] = { ReadU32(h + kHoloLight[0]), ReadU32(h + kHoloLight[1]) };
    if (!light[0] || !light[1])
    {
        g_origHoloUpdate(self, nullptr, dt);
        return;
    }
    uint32_t team[4] = {};
    for (int p = 0; p < players && p < 4; ++p)
        team[p] = TeamOfPlayer(p);
    int src[2] = { 0, 1 }; // whose team colours light +88h / +8Ch
    if (team[0] && (team[1] == team[0] || !team[1]))
    {
        for (int p = 2; p < players && p < 4; ++p)
            if (team[p] && team[p] != team[0])
            {
                src[1] = p;
                break;
            }
    }
    else if (!team[0] && team[1])
    {
        for (int p = 2; p < players && p < 4; ++p)
            if (team[p] && team[p] != team[1])
            {
                src[0] = p;
                break;
            }
    }
    g_holoSrc[0] = src[0];
    g_holoSrc[1] = src[1];
    g_holoThread = GetCurrentThreadId();
    g_origHoloUpdate(self, nullptr, dt);
    g_holoThread = 0;
    g_holoSrc[0] = 0;
    g_holoSrc[1] = 1;
    InterlockedIncrement(&g_holoUpdates);
    if (src[0] != 0 || src[1] != 1)
        InterlockedIncrement(&g_holoRemapped);
    // each player's view: the light of his team's colour only
    const uintptr_t cm = CameraManager();
    uint8_t m[2] = { ReadU8(light[0] + kLightNotIn), ReadU8(light[1] + kLightNotIn) };
    const uint8_t was[2] = { m[0], m[1] };
    for (int p = 0; p < players && p < 4; ++p)
    {
        const int v = ViewOfPlayer(p, cm);
        if (v < 0 || v > 3)
            continue;
        int pick = -1;
        if (team[p] && team[p] == team[src[0]])
            pick = 0;
        else if (team[p] && team[p] == team[src[1]])
            pick = 1;
        else if (p < 2)
            pick = p; // (no team yet: the game's own)
        const uint8_t bit = static_cast<uint8_t>(1u << v);
        for (int l = 0; l < 2; ++l)
            m[l] = pick == l ? static_cast<uint8_t>(m[l] & ~bit) : static_cast<uint8_t>(m[l] | bit);
    }
    for (int l = 0; l < 2; ++l)
        if (m[l] != was[l])
        {
            *reinterpret_cast<volatile uint8_t*>(light[l] + kLightNotIn) = m[l];
            InterlockedIncrement(&g_holoMasks);
        }
}

void InstallCpLights()
{
    if (!g_cpWanted)
    {
        Log("[players] 20g-r7: [Splitscreen] CpLightsP34=0 - command post lights as the game has them (views 3 / 4 get both)");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kPatHoloUpdate, hits, 4);
    const uint32_t fn = n == 1 ? hits[0] : 0;
    uint32_t mode[4] = {};
    uint32_t align[4] = {};
    uint32_t lights[4] = {};
    uint32_t test[4] = {};
    const int nMode = FindPattern(kPatHoloMode, mode, 4);
    const int nAlign = FindPattern(kPatHoloAlign, align, 4);
    const int nLights = FindPattern(kPatHoloLights, lights, 4);
    const int nTest = FindPattern(kPatLightViewTest, test, 4);
    const bool modeIn = fn && nMode == 1 && mode[0] > fn && mode[0] < fn + 0x600;
    const bool alignIn = fn && nAlign == 1 && align[0] > fn && align[0] < fn + 0x700;
    const uint32_t notIn = nLights == 1 ? CallTarget(lights[0] + kHoloLightsNotIn) : 0;
    const bool notInOk = notIn && MatchPatternAt(notIn, kLightNotInBody);
    const bool census = g_hooked[kNetSlot];
    Log("[players] 20g-r7: EntityHologram update %d hit(s) rva 0x%08X; its class mode test %s, its CharIndex(0 / 1) loop %s, "
        "the lights +88h / +8Ch made %d hit(s), their view bit rva 0x%08X %s, the renderer's light view test %d hit(s); the net "
        "local slot getter %s",
        n, fn, modeIn ? "OK" : "NOT OK", alignIn ? "OK" : "NOT OK", nLights, notIn, notInOk ? "OK (+4Ch)" : "NOT OK", nTest,
        census ? "ours (census)" : "NOT ours");
    if (!fn || !modeIn || !alignIn || !notInOk || nTest != 1 || !census)
    {
        Log("[players] 20g-r7: command post lights for players 3 and 4 NOT installed (something above did not check out)");
        return;
    }
    uintptr_t orig = 0;
    if (!MoveThunk("EntityHologram update", fn, reinterpret_cast<const void*>(&HoloUpdateWrap), &orig))
        return;
    g_origHoloUpdate = reinterpret_cast<HoloUpdate_t>(orig);
    g_cpOk = true;
    Log("[players] 20g-r7: command post lights ON - each view gets the light of its player's team (players 3 / 4's colour "
        "lent the second light when players 1 and 2 share a team)");
}

// (2) The scope blur: ScopeDisplay's show / hide (GOG 1.3 0x00208050 / 0x00207F70) write [T + player*4] (0x002080B0 (v,
// player) / 0x002080D0 (player), T = 0x009EBCC0, 2 slots - T + 8 holds the blur effect's pointer, 0x00207950 / 0x00207990)
// and the ScopeBlurEffect's draw (0x00073730, vtable slot 4) reads [T + view*4] for the view being drawn. Players 3 / 4's
// slots are ours; the draw in view 3 / 4 finds his in the slot of that view for the call.
const char* const kPatScopeBlurDraw =
    "55 8B EC 83 EC 34 53 56 8B F1 89 75 F8 E8 ?? ?? ?? ?? F7 45 08 00 00 20 00 8B D8 0F 85 ?? ?? ?? ?? 83 3C 9D ?? ?? ?? ?? 00";
constexpr uint32_t kScopeBlurDrawTable = 0x24;
const char* const kScopeBlurOnFmt = "55 8B EC 8B 4D 0C 8B 45 08 89 04 8D %s 5D C3";
const char* const kScopeBlurOffFmt = "55 8B EC 8B 45 08 C7 04 85 %s 00 00 00 00 5D C3";
typedef uint32_t(__cdecl* ScopeBlurOn_t)(uint32_t v, int32_t player);
typedef uint32_t(__cdecl* ScopeBlurOff_t)(int32_t player);
typedef void(__fastcall* ScopeBlurDraw_t)(void* self, void* edx, uint32_t flags);
bool g_scopeWanted = true;              // [Splitscreen] ScopeP34
bool g_scopeOk = false;
uintptr_t g_scopeTable = 0;             // T (address)
uint32_t g_scopeBlur[2] = {};           // players 3 / 4's slots
ScopeBlurOn_t g_origScopeBlurOn = nullptr;
ScopeBlurOff_t g_origScopeBlurOff = nullptr;
ScopeBlurDraw_t g_origScopeBlurDraw = nullptr;
volatile LONG g_scopeSets = 0;
volatile LONG g_scopeDraws = 0;

uint32_t __cdecl ScopeBlurOnWrap(uint32_t v, int32_t player)
{
    if (g_playersOn && (player == 2 || player == 3))
    {
        g_scopeBlur[player - 2] = v;
        InterlockedIncrement(&g_scopeSets);
        return v;
    }
    return g_origScopeBlurOn(v, player);
}

uint32_t __cdecl ScopeBlurOffWrap(int32_t player)
{
    if (g_playersOn && (player == 2 || player == 3))
    {
        g_scopeBlur[player - 2] = 0;
        InterlockedIncrement(&g_scopeSets);
        return 0;
    }
    return g_origScopeBlurOff(player);
}

void __fastcall ScopeBlurDrawWrap(void* self, void* edx, uint32_t flags)
{
    (void)edx;
    if (g_playersOn && g_rendCams && g_scopeTable)
    {
        const uint32_t view = ReadU32(g_rendCams + 0x10);
        if (view == 2 || view == 3)
        {
            int k = -1;
            for (int j = 0; j < 2; ++j)
                if (PlayerNSpecView(j) == static_cast<int>(view) && static_cast<int>(g_players) >= j + 3)
                    k = j;
            uint32_t* const slot = reinterpret_cast<uint32_t*>(g_scopeTable + static_cast<uintptr_t>(view) * 4);
            const uint32_t keep = *slot;
            *slot = k >= 0 ? g_scopeBlur[k] : 0;
            g_origScopeBlurDraw(self, nullptr, flags);
            *slot = keep;
            InterlockedIncrement(&g_scopeDraws);
            return;
        }
    }
    g_origScopeBlurDraw(self, nullptr, flags);
}

// the function's one pattern hit, with one jump thunk and no direct call (0 = not so)
uint32_t ScopeHookable(const char* name, const char* pat)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pat, hits, 4);
    uint32_t thunks[4] = {};
    int direct = 0;
    const int nt = n == 1 ? FindThunks(hits[0], thunks, 4, &direct) : 0;
    if (n != 1 || nt != 1 || direct)
    {
        Log("[players] 20g-r7: %s: %d hit(s), %d jump thunk(s), %d direct call(s) (need 1, 1, 0)", name, n, nt, direct);
        return 0;
    }
    return hits[0];
}

bool ScopeHookOne(const char* name, const char* pat, const void* wrap, uintptr_t* orig)
{
    const uint32_t fn = ScopeHookable(name, pat);
    return fn && MoveThunk(name, fn, wrap, orig);
}

bool InstallScopeBlur()
{
    if (!g_scopeWanted)
    {
        Log("[players] 20g-r7: [Splitscreen] ScopeP34=0 - no ScopeDisplay for players 3 and 4 (their zoom draws no scope)");
        return false;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kPatScopeBlurDraw, hits, 4);
    const uint32_t table = n == 1 ? CodeU32(hits[0] + kScopeBlurDrawTable) : 0;
    if (!table || !InImage(table))
    {
        Log("[players] 20g-r7: the scope blur's draw: %d hit(s), its table %08X - players 3 and 4's ScopeDisplays NOT made", n, table);
        return false;
    }
    char a[16];
    PatBytes(table, a, sizeof(a));
    char onPat[96];
    char offPat[96];
    snprintf(onPat, sizeof(onPat), kScopeBlurOnFmt, a);
    snprintf(offPat, sizeof(offPat), kScopeBlurOffFmt, a);
    uintptr_t o[3] = {};
    // both found before either moves (a half-installed pair would call a null original)
    const uint32_t onFn = ScopeHookable("scope blur on", onPat);
    const uint32_t offFn = ScopeHookable("scope blur off", offPat);
    if (!onFn || !offFn ||
        !MoveThunk("scope blur off", offFn, reinterpret_cast<const void*>(&ScopeBlurOffWrap), &o[1]))
    {
        Log("[players] 20g-r7: players 3 and 4's ScopeDisplays NOT made (their blur slot would land on the blur effect's pointer)");
        return false;
    }
    g_origScopeBlurOff = reinterpret_cast<ScopeBlurOff_t>(o[1]);
    if (!MoveThunk("scope blur on", onFn, reinterpret_cast<const void*>(&ScopeBlurOnWrap), &o[0]))
    {
        Log("[players] 20g-r7: players 3 and 4's ScopeDisplays NOT made (the blur's on could not be made ours; its off is ours "
            "and does the same for players 1 / 2)");
        return false;
    }
    g_origScopeBlurOn = reinterpret_cast<ScopeBlurOn_t>(o[0]);
    if (!ScopeHookOne("scope blur draw", kPatScopeBlurDraw, reinterpret_cast<const void*>(&ScopeBlurDrawWrap), &o[2]))
        Log("[players] 20g-r7: the scope blur's draw is not ours - views 3 / 4 draw no scope blur (their ScopeDisplays are made)");
    else
        g_origScopeBlurDraw = reinterpret_cast<ScopeBlurDraw_t>(o[2]);
    g_scopeTable = static_cast<uintptr_t>(table);
    g_scopeOk = true;
    Log("[players] 20g-r7: scope blur slots (va 0x%08X, 2 of them) - players 3 and 4's are BF1GameExt's; the draw %s", table,
        g_origScopeBlurDraw ? "reads theirs in views 3 / 4" : "is the game's");
    return true;
}

// =====================================================================================================================
// 21j-r4: THE SCOPE SCAN (log only, [scope] lines). The zoom scope in split screen ("the 1 player is fine in 1 player
// mode, the rest of split no"; the videos: in every split view the scope is cut off and its cross is off the reticle):
//  - ScopeDisplay (GOG 1.3: constructor 0x00207AD0, vtable 0x007EDB54, Update = slot 1 0x00208160, Setup 0x00207E40)
//    draws the scope texture as four quarters (RedBitmapElements +D0h / +1D0h / +2D0h / +3D0h, each W / 2 x H / 2
//    units round its group's (0, 0); W x H = the screen object's size, 0x00AAF750 - a full-screen HUD root's units),
//    and Setup puts the group (+20h) in the player's HUD root with AddToRoot(root, group, 0.5, 0.5, 1): the middle of
//    the root's SAFE AREA.
//  - A HUD root is W x H units times its view's share of the screen (AddViewport 0x0028C440: a half 1 x 0.5 or
//    0.5 x 1, a quarter 0.5 x 0.5), so the scope is the full screen's size in every view - twice a half's height (or
//    width), twice a quarter's both ways: cut off. And AddViewport's safe area is 5 % of the SCREEN clipped at the
//    view's edges (0.01 in from an edge inside the screen): a quarter's or a side-by-side half's is lopsided, so its
//    middle is off the view's middle (the reticle's place) towards the middle of the screen - about 38 / 22 px at
//    1920 x 1080 in the videos (the horizontal halves' safe areas are made even by HorizontalSafeArea: none there).
//  - ScopeBlurEffect (0x00073730, vtable 0x007EDB14 slot 4) blurs the view outside the scope: its last pass draws the
//    blurred picture over the whole view through "blurmask" (a round hole stretched 0..1 over the view).
// r4 (log only): each ScopeDisplay's Update (through its vtable slot) logs, the first times a player's scope is shown,
// its HUD root, the root's safe area, its group's anchor / flag, its four quarters and what r5 would set; the scope
// blur's draw (its vtable slot) logs each view's viewport while a scope is up. [Splitscreen] ScopeScan=1.
// r4's log (Steam, 4K) proved it: W x H 1024 x 624; quarters 512 x 312 with safe areas 0.10 / 0.02 (the middle 0.54 /
// 0.46 of the view), horizontal halves 1024 x 312 (even: 0.50), side-by-side halves 512 x 624 (0.54 / 0.46 across);
// the scope 2.00 x 2.00 of a quarter, 1.00 x 2.00 of a horizontal half, 2.00 x 1.00 of a side-by-side half.
// 21j-r5: THE SCOPE FITS EACH VIEW ([Splitscreen] ScopeFit=1). Right after the game's Update, for a scope whose HUD root
// is smaller than the screen (split screen only - 1 player's is never touched):
//  - its group's anchor flag 0: the anchor (0.5, 0.5) is the view's middle as it is - the reticle's place;
//  - its four quarters SetRect to the screen's W x H times s, s = the smaller of the view's shares of the screen: in a
//    quarter (s 0.5) exactly 1 player's scope in his quarter; in a half (s 0.5) as big as the half's short side, 1
//    player's shape - and two dark bands (RedBitmapElements of the game's, made once per scope with its new and
//    constructor, added to the scope's group, the scope texture's dark corner) fill the half's long ends as 1 player's
//    scope texture darkens the screen outside its ring;
//  - the scope blur in a half: its draw runs twice - the game's pass as it is (the blurmask's hole stretched over the
//    half: the bands blurred), then a second pass whose last quad (the unlock at the draw +597h through a stub: its 4
//    vertices' places and picture u v moved in, the mask's u v kept) covers the scope's own rectangle only - the hole
//    1 player's size round the ring, blurred outside it.
// Back in a full-screen root (the same display, 1 player again) the game's own values are put back. ScopeFit=0: as r4.
const char* const kScA = "55 8B EC 51 53 56 8B F1 57 89 75 FC E8 ?? ?? ?? ?? 8D 4E 20 C7 06 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? "
                         "6A 04 68 00 01 00 00 8D 9E D0 00 00 00 53 E8 ?? ?? ?? ?? 8D 8E DC 04 00 00 E8 ?? ?? ?? ?? C7 46 18 FF "
                         "FF FF FF C6 05 ?? ?? ?? ?? 01 E8 ?? ?? ?? ?? 8B F8 D1 EF E8 ?? ?? ?? ?? 8B 4D FC 8B F0";
const char* const kScB = "6A 00 D1 EE 8D 49 20 E8 ?? ?? ?? ?? 8B 4D FC 8D 49 20 E8 ?? ?? ?? ?? 6A 02 66 0F 6E C6 8B CB 66 0F 6E "
                         "CF 0F 5B C0 6A 02 83 EC 08 0F 5B C9 F3 0F 11 44 24 04 F3 0F 11 0C 24 E8 ?? ?? ?? ?? 6A 00 83 EC 10 8B "
                         "CB C7 44 24 0C 00 00 80 3F C7 44 24 08 00 00 80 3F C7 44 24 04 00 00 00 00 C7 04 24 00 00 00 00 E8 "
                         "?? ?? ?? ?? 53 8B 5D FC 8D 4B 20 E8";
constexpr uint32_t kScBAt = 0x63;        // kScB's place in the constructor
constexpr uint32_t kScVt = 0x16;         // mov [esi], ScopeDisplay's vtable
constexpr uint32_t kScBmpCtor = 0x20;    // push RedBitmapElement's constructor (a jump thunk) for the 4 quarters
constexpr uint32_t kScGetW = 0x50;       // call: the screen object's width (W)
constexpr uint32_t kScGetH = 0x59;       // call: its height (H)
constexpr uint32_t kScSetSize = 0x9C;    // call: quarter 1's SetSize(W / 2, H / 2, 2, 2)
constexpr uint32_t kScSetUv = 0xC7;      // call: its SetUV(0, 0, 1, 1)
constexpr uint32_t kScAddChild = 0xD3;   // call: the group's AddChild(quarter 1)
constexpr uint32_t kScSlotUpdate = 1;    // ScopeDisplay vtable slot: Update (thiscall, 1 argument, returns al)
const char* const kScUpdateBody = "55 8B EC 83 EC 0C 53 57 8B F9 FF 77 18 E8 ?? ?? ?? ?? 83 C4 04 50 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? "
                                  "85 C0 0F 84";
const char* const kScScreenObj = "B9 ?? ?? ?? ?? E9"; // the getter: mov ecx, screen; jmp
const char* const kScScreenW = "8B 01 C3";
const char* const kScScreenH = "8B 41 04 C3";
const char* const kScBmpCtorBody = "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? 6A 70 8D 86 88 00 00 00 C7 06 ?? ?? ?? ?? 6A 00 50";
constexpr uint32_t kScBmpVt = 0x19;
const char* const kScSetSizeBody = "55 8B EC 8B 45 10 0F 57 C0 F3 0F 10 2D ?? ?? ?? ?? 0F 57 D2 F3 0F 10 25 ?? ?? ?? ?? 0F 57 DB 0F "
                                   "57 C9 83 E8 00 74 25";
constexpr uint32_t kScSetRectCall = 0x9D; // SetSize's call of SetRect (x0, y0, x1, y1)
const char* const kScSetRectBody = "55 8B EC F3 0F 10 45 08 F3 0F 10 55 0C F3 0F 10 4D 14 F3 0F 11 81 88 00 00 00 F3 0F 11 81 A4 00 00 "
                                   "00 F3 0F 10 45 10 F3 0F 11 91 8C 00 00 00 F3 0F 11 89 A8 00 00 00 F3 0F 11 81 DC 00 00 00 F3 0F "
                                   "11 89 E0 00 00 00 F3 0F 11 81 C0 00 00 00 F3 0F 11 91 C4 00 00 00 5D C2 10 00";
const char* const kScSetUvBody = "55 8B EC 80 7D 18 00 F3 0F 10 45 14 F3 0F 10 5D 08 F3 0F 10 4D 10 F3 0F 10 55 0C F3 0F 11 99 B0 00 "
                                 "00 00";
const char* const kScAddChildBody = "55 8B EC 83 EC 08 53 57 8B 7D 08 8B D9 8D 4F 1C E8";
constexpr uint32_t kScBmpSlotTexture = 13; // RedBitmapElement vtable slot: SetTexture(texture) (its material +F8h)
const char* const kScSetTextureBody = "55 8B EC 56 8B F1 8B 8E F8 00 00 00 85 C9 75 0D 8B 06 8B CE FF 50 2C";
const char* const kScAddToRoot = "55 8B EC F3 0F 10 45 14 56 8B 75 0C 83 EC 08 8B CE F3 0F 11 44 24 04 F3 0F 10 45 10 F3 0F 11 04 "
                                 "24 E8 ?? ?? ?? ?? FF 75 18 8B CE E8 ?? ?? ?? ?? 8B 4D 08 56 8B 0C 8D";
constexpr uint32_t kScAddToRootTable = 0x37; // its roots[] (mov ecx, [ecx*4 + roots])
const char* const kScBlurCtor = "56 6A 09 68 ?? ?? ?? ?? 8B F1 E8 ?? ?? ?? ?? 6A 01 8B CE C7 06 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C6 C7 05 "
                                "?? ?? ?? ?? 00 00 00 00 C7 05 ?? ?? ?? ?? 00 00 00 00 5E C3";
constexpr uint32_t kScBlurCtorVt = 0x15;
constexpr uint32_t kScBlurCtorSlots = 0x22; // mov [slots], 0 (the 2 players' scope blur slots: kScopeBlurDrawTable's)
constexpr uint32_t kScBlurSlotDraw = 4;     // ScopeBlurEffect vtable slot: the draw (thiscall, 1 argument: flags)
constexpr uint32_t kScBlurViewCall = 0x0D;  // the draw's call of the renderer's current view (mov eax, [view]; ret)
const char* const kScViewGetter = "A1 ?? ?? ?? ?? C3";
const char* const kScBlurVp = "8D 45 CC 50 E8 ?? ?? ?? ?? 8D 45 E8 50 8D 45 E4 50 8D 45 EC 50 8D 45 F0 50 E8"; // in the draw
constexpr uint32_t kScBlurVpNorm = 4;       // its call: the viewport as a share of the screen (x, y, w, h)
constexpr uint32_t kScBlurVpPx = 0x19;      // its call: the viewport in pixels (left, top, right, bottom)
const char* const kScVpNormBody = "55 8B EC 8B 45 08 0F 10 05 ?? ?? ?? ?? 0F 11 00 5D C3";
const char* const kScVpPxBody = "55 8B EC A1 ?? ?? ?? ?? 66 0F 6E C0 F3 0F E6 C0 C1 E8 1F F2 0F 58 04 C5 ?? ?? ?? ?? 8B 45 08 66 0F 5A C0 "
                                "F3 0F 11 00 A1";
// the last pass's vertices unlocked (ecx = the buffer, esi = its 4 vertices: 20h each - x y z w, the blurred picture's
// u v, the mask's u v): 21j-r5's second pass goes in here
const char* const kScBlurCommit = "8B CF E8 ?? ?? ?? ?? 6A 00 6A 00 6A 00 68 00 02 00 00 8D 45 F4 50 8B 45 F8 68 ?? ?? ?? ?? FF 70 28 57 E8";
constexpr uint32_t kScBlurSearch = 0x700;   // the draw's length to look in
// ScopeDisplay / its group / quarters (GOG 1.3 constructor, Update, Setup)
constexpr uint32_t kScSdPlayer = 0x18;       // the player (-1 = none)
constexpr uint32_t kScSdGroup = 0x20;        // RedScreenGroupElement
constexpr uint32_t kScSdQuarter0 = 0xD0;     // 4 RedBitmapElements, 100h apart: top left, top right, bottom left, bottom right
constexpr uint32_t kScSdQuarterSpan = 0x100;
constexpr uint32_t kScSdTexture = 0x4D0;     // the scope texture the quarters got
constexpr uint32_t kScSdZoom = 0x4D4;
constexpr uint32_t kScSdShown = 0x4D9;       // the scope is up
constexpr uint32_t kScGrNodeOwner = 0x24;    // an element's list node owner (a root's child list: root + 18h)
constexpr uint32_t kScGrRootList = 0x18;
constexpr uint32_t kScGrX = 0x60;            // its place
constexpr uint32_t kScGrY = 0x64;
constexpr uint32_t kScGrFlags = 0x14;        // 100h = shown
constexpr uint32_t kScGrAnchorX = 0xA0;      // its anchor in the root (fractions of the view)
constexpr uint32_t kScGrAnchorY = 0xA4;
constexpr uint32_t kScGrAnchorFlag = 0xA8;   // 1 = through the root's safe area
constexpr uint32_t kScBmX0 = 0x88;           // RedBitmapElement: vertex 0 x / y (SetRect), vertex 3 x / y, their u v
constexpr uint32_t kScBmY0 = 0x8C;
constexpr uint32_t kScBmX1 = 0xDC;
constexpr uint32_t kScBmY1 = 0xE0;
constexpr uint32_t kScBmU0 = 0x94;
constexpr uint32_t kScBmV0 = 0x98;
constexpr uint32_t kScBmU1 = 0xE8;
constexpr uint32_t kScBmV1 = 0xEC;
constexpr int kScMaxLines = 320;
constexpr int kScDisplays = 8;
constexpr int kScLogsEach = 6;             // full logs per display (its first shows; a new HUD root again)
constexpr int kScViewLogs = 3;             // blur viewport logs per view (each new size; 21j-r6: g_scTraced's sizes)
constexpr int kScHud = 4;                  // ScopeDisplay in the HUD scan's order (g_h34)

typedef uint8_t(__fastcall* ScUpdate_t)(void* self, void* edx, uint32_t arg);
typedef void(__fastcall* ScSetRect_t)(uintptr_t self, void* edx, float x0, float y0, float x1, float y1);
typedef void(__fastcall* ScSetUv_t)(uintptr_t self, void* edx, float u0, float v0, float u1, float v1, uint32_t turned);
typedef void(__fastcall* ScSetTex_t)(uintptr_t self, void* edx, uint32_t texture);
typedef uint8_t(__fastcall* ScAddChild_t)(uintptr_t group, void* edx, uintptr_t element);
typedef uintptr_t(__cdecl* ScNew_t)(uint32_t size);
typedef uintptr_t(__fastcall* ScBmpCtor_t)(uintptr_t self, void* edx);
typedef void(__fastcall* ScBlurDraw_t)(void* self, void* edx, uint32_t flags);
typedef void(__cdecl* ScVpNorm_t)(float* xywh);
typedef void(__cdecl* ScVpPx_t)(float* l, float* t, float* r, float* b);

struct ScSeen
{
    uintptr_t d = 0;          // the ScopeDisplay
    DWORD lastTick = 0;       // its last Update (a display gone - players 3 / 4's at each teardown - gives its place up)
    int player = -1;
    int root = -1;            // its group's HUD root (roots[k], k = the renderer view)
    int32_t rw = 0;           // that root's size
    int32_t rh = 0;
    bool shown = false;
    LONG updates = 0;
    LONG shows = 0;
    int logs = 0;
    int32_t loggedRw = -1;    // the root size last logged
    int32_t loggedRh = -1;
    float s = 1.0f;           // 21j-r5's scale (the scope as a share of 1 player's)
    float fx = 1.0f;          // its share of the view (21j-r5)
    float fy = 1.0f;
    bool touched = false;     // 21j-r5: its quarters / anchor are BF1GameExt's (put back in a full-screen root)
    uintptr_t band[2] = {};   // 21j-r5: its two dark bands (RedBitmapElements of the game's, made once)
    uint32_t bandTex = 0;     // the texture they were given
    int fitLogs = 0;
};

bool g_scopeScanWanted = true;    // [Splitscreen] ScopeScan
bool g_scOn = false;
int g_scLines = 0;
uintptr_t g_scScreen = 0;         // the screen object (W, H)
uintptr_t g_scRoots = 0;          // roots[] (AddToRoot's)
uintptr_t g_scViewVar = 0;        // the renderer's current view (int)
uintptr_t g_scOrigUpdate = 0;     // the slots' old values (set before a slot moves)
uintptr_t g_scOrigBlur = 0;
ScVpNorm_t g_scVpNorm = nullptr;
ScVpPx_t g_scVpPx = nullptr;
ScSeen g_sc[kScDisplays];
int g_scViewLogged[4] = {};
float g_scViewW[4][kScViewLogs] = {};
float g_scViewH[4][kScViewLogs] = {};
DWORD g_scLastSummary = 0;
LONG g_scShowsSummed = 0;
volatile LONG g_scBlurDraws = 0;
// 21j-r5
constexpr uint32_t kScBmpSize = 0x100;     // sizeof(RedBitmapElement) (ScopeDisplay's array: 100h apart)
constexpr uint32_t kScElemShown = 0x100;   // an element's flags +14h: shown
constexpr uint32_t kScGrChildren = 0x80;   // a group's child list (an element in it: its node owner = group + 80h)
constexpr float kScBandU0 = 0.01f;         // the bands' texture corner (the quarter texture's outer corner: its dark
constexpr float kScBandU1 = 0.03f;         // fill outside the ring)
constexpr float kScBandPad = 2.0f;         // units past the view's edges (the view clips them)
constexpr int kScFitLogsEach = 4;
constexpr size_t kScStubBytes = 128;
bool g_scopeFitWanted = true;     // [Splitscreen] ScopeFit
bool g_scFitOn = false;           // the quarters / anchor / bands part
bool g_scPass2On = false;         // the blur's second pass (its stub in)
ScSetRect_t g_scSetRect = nullptr;
ScSetUv_t g_scSetUv = nullptr;
ScAddChild_t g_scAddChild = nullptr;
ScNew_t g_scNew = nullptr;
ScBmpCtor_t g_scBmpCtor = nullptr;
uint32_t g_scBmpVtRva = 0;        // RedBitmapElement's vtable (the bands' SetTexture, slot 13)
uintptr_t g_scCommitOrig = 0;     // the last pass's unlock (the call's old target)
bool g_scP2Active = false;        // the second pass is being drawn (game thread)
float g_scP2Fx = 1.0f;            // the scope's share of the view in it
float g_scP2Fy = 1.0f;
volatile LONG g_scP2Quads = 0;    // quads moved in
volatile LONG g_scP2Skipped = 0;  // quads not as expected (left as they are)
volatile LONG g_scBandsMade = 0;
volatile LONG g_scFits = 0;       // quarters set (each change)
// 21j-r6
constexpr float kScRingW = 0.63f;      // the scope texture's ring (its outer edge): its share of the texture's width (r5's
constexpr float kScRingH = 0.815f;     // quarters, measured in the user's video) and of its height
constexpr float kScMaskCorner = 0.02f; // the blurmask's corner (outside the hole: all blur), for the passes over a half's ends
int g_scHalfFill = 96;            // [Splitscreen] ScopeHalfFill: the ring's share of a half's short side, % (0 = r5's size)
int g_scP2Mode = 4;               // the pass being drawn: 0 / 1 left / right of the scope, 2 / 3 above / below it, 4 the scope
bool g_scStubOn = false;          // the stub at the blur's last-quad unlock is in (ScopeFit's passes, ScopeScan's quad log)
bool g_scQuadNext[4] = {};        // log the next last quad of view k (armed with that view's viewport line)
bool g_scTraced[4][3] = {};       // a device trace was armed for view k's size i (render_hooks.cpp)
volatile LONG g_scP2Passes = 0;   // passes drawn over halves' scopes
// 21j-r7
bool g_scBlurFixWanted = true;    // [Splitscreen] ScopeBlurFix
bool g_scBlurFixOn = false;
volatile LONG g_scFixQuads = 0;   // last quads whose picture u v were put right

bool ScLog()
{
    if (g_scLines >= kScMaxLines)
        return false;
    ++g_scLines;
    return true;
}

int32_t ScScreenW()
{
    return g_scScreen ? static_cast<int32_t>(ReadU32(g_scScreen)) : 0;
}

int32_t ScScreenH()
{
    return g_scScreen ? static_cast<int32_t>(ReadU32(g_scScreen + 4)) : 0;
}

// which of roots[0..3] the group is in (-1: none)
int ScRootOf(uintptr_t group, uintptr_t* rootOut)
{
    *rootOut = 0;
    const uintptr_t owner = ReadU32(group + kScGrNodeOwner);
    if (!owner || !g_scRoots)
        return -1;
    for (int k = 0; k < 4; ++k)
    {
        const uintptr_t r = ReadU32(g_scRoots + 4u * static_cast<uint32_t>(k));
        if (r && owner == r + kScGrRootList)
        {
            *rootOut = r;
            return k;
        }
    }
    return -1;
}

ScSeen* ScFind(uintptr_t d)
{
    for (int i = 0; i < kScDisplays; ++i)
        if (g_sc[i].d == d)
            return &g_sc[i];
    // a new one: a free place, else the one updated longest ago (a display deleted at a teardown)
    int at = -1;
    for (int i = 0; i < kScDisplays && at < 0; ++i)
        if (!g_sc[i].d)
            at = i;
    const DWORD now = GetTickCount();
    DWORD idlest = 5000;
    for (int i = 0; i < kScDisplays && !(at >= 0 && !g_sc[at].d); ++i)
        if (now - g_sc[i].lastTick > idlest) // (the one idle longest, at least 5 s)
        {
            idlest = now - g_sc[i].lastTick;
            at = i;
        }
    if (at < 0)
        return nullptr;
    g_sc[at] = ScSeen();
    g_sc[at].d = d;
    return &g_sc[at];
}

// 21j-r5 (dry run): the scope kept at 1 player's shape and scaled to fit the view - s = the smaller of the view's
// shares of the screen (a quarter 0.5: exactly 1 player's picture in it; a half 0.5: the scope as tall / wide as the
// half's short side, dark bands at the long side's ends)
void ScFit(ScSeen& s, int32_t rw, int32_t rh, int32_t W, int32_t H)
{
    s.s = 1.0f;
    s.fx = 1.0f;
    s.fy = 1.0f;
    if (rw <= 0 || rh <= 0 || W <= 0 || H <= 0)
        return;
    const float aw = static_cast<float>(rw) / static_cast<float>(W);
    const float ah = static_cast<float>(rh) / static_cast<float>(H);
    s.s = aw < ah ? aw : ah;
    // 21j-r6 ("in the horizontal and vertical halves their scopes are smaller"): in a half the ring fills ScopeHalfFill %
    // of the half's short side - its height across a horizontal half (the ring 0.815 of the texture's height), its width
    // across a side-by-side half (0.63 of its width); the texture's outer part runs past the view's long edges (cut off
    // there, as 1 player's is at the screen's)
    if (g_scHalfFill > 0 && fabsf(aw - ah) > 0.05f)
        s.s = (ah < aw ? ah / kScRingH : aw / kScRingW) * static_cast<float>(g_scHalfFill) / 100.0f;
    if (s.s < 0.05f)
        s.s = 0.05f;
    if (s.s > 1.0f)
        s.s = 1.0f;
    s.fx = s.s / aw;
    s.fy = s.s / ah;
}

void ScLogDisplay(ScSeen& s, uintptr_t d, int k, uintptr_t root)
{
    const int32_t W = ScScreenW();
    const int32_t H = ScScreenH();
    const uintptr_t g = d + kScSdGroup;
    const int32_t rw = root ? static_cast<int32_t>(ReadU32(root)) : 0;
    const int32_t rh = root ? static_cast<int32_t>(ReadU32(root + 4)) : 0;
    const float ox = root ? ReadF32(root + 8) : 0.0f;
    const float oy = root ? ReadF32(root + 0x0C) : 0.0f;
    const float sx = root ? ReadF32(root + 0x10) : 1.0f;
    const float sy = root ? ReadF32(root + 0x14) : 1.0f;
    const float ax = ReadF32(g + kScGrAnchorX);
    const float ay = ReadF32(g + kScGrAnchorY);
    const uint8_t flag = ReadU8(g + kScGrAnchorFlag);
    const float px = ReadF32(g + kScGrX);
    const float py = ReadF32(g + kScGrY);
    float cx = flag ? ox + sx * ax : ax;
    float cy = flag ? oy + sy * ay : ay;
    if (rw > 0 && rh > 0)
    {
        cx += px / static_cast<float>(rw);
        cy += py / static_cast<float>(rh);
    }
    if (ScLog())
        Log("[scope] %lu ms: player %d's scope up (ScopeDisplay 0x%08X%s): his HUD root roots[%d] %ld x %ld units = %.3f x %.3f "
            "of the screen (%ld x %ld units), safe area offset %.3f %.3f scale %.3f %.3f; its group: anchor %.3f %.3f %s (flag "
            "%u), place %.1f %.1f, %s -> its middle at %.3f %.3f of the view (the reticle's: 0.500 0.500)",
            ScanMsSinceAttach(), s.player + 1, static_cast<unsigned>(d), s.player >= 2 ? ", BF1GameExt's" : "", k,
            static_cast<long>(rw), static_cast<long>(rh), W > 0 ? static_cast<double>(rw) / W : 0.0,
            H > 0 ? static_cast<double>(rh) / H : 0.0, static_cast<long>(W), static_cast<long>(H), ox, oy, sx, sy, ax, ay,
            flag ? "through the safe area" : "of the view as it is", static_cast<unsigned>(flag), px, py,
            (ReadU32(g + kScGrFlags) & 0x100) ? "shown" : "HIDDEN", cx, cy);
    char q[4][96];
    float maxX = 0.0f;
    float maxY = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        const uintptr_t e = d + kScSdQuarter0 + kScSdQuarterSpan * static_cast<uint32_t>(i);
        const float x0 = ReadF32(e + kScBmX0);
        const float y0 = ReadF32(e + kScBmY0);
        const float x1 = ReadF32(e + kScBmX1);
        const float y1 = ReadF32(e + kScBmY1);
        snprintf(q[i], sizeof(q[i]), "%.0f..%.0f x %.0f..%.0f uv %.0f %.0f - %.0f %.0f", x0, x1, y0, y1, ReadF32(e + kScBmU0),
                 ReadF32(e + kScBmV0), ReadF32(e + kScBmU1), ReadF32(e + kScBmV1));
        const float ex = fabsf(x0) > fabsf(x1) ? fabsf(x0) : fabsf(x1);
        const float ey = fabsf(y0) > fabsf(y1) ? fabsf(y0) : fabsf(y1);
        if (ex > maxX)
            maxX = ex;
        if (ey > maxY)
            maxY = ey;
    }
    if (ScLog())
        Log("[scope]   its quarters (units round the group's 0, 0): TL %s | TR %s | BL %s | BR %s; the scope %.2f x %.2f of the view "
            "(1 player: 1.00 x 1.00); texture 0x%08X, zoom %.2f",
            q[0], q[1], q[2], q[3], rw > 0 ? 2.0 * maxX / rw : 0.0, rh > 0 ? 2.0 * maxY / rh : 0.0, ReadU32(d + kScSdTexture),
            ReadF32(d + kScSdZoom));
    ScFit(s, rw, rh, W, H);
    const float bandX = (1.0f - s.fx) * 0.5f;
    const float bandY = (1.0f - s.fy) * 0.5f;
    if (ScLog())
        Log("[scope]   21j-r5 ScopeFit=%d (21j-r6 ScopeHalfFill=%d): its middle at the view's middle (anchor 0.500 0.500, flag 0); "
            "scale x%.3f: quarters %.0f x %.0f units each (the scope %.2f x %.2f of the view, 1 player's shape); dark bands %s",
            g_scFitOn ? 1 : 0, g_scHalfFill, s.s, 0.5f * s.s * static_cast<float>(W), 0.5f * s.s * static_cast<float>(H), s.fx, s.fy,
            (bandX > 0.001f || bandY > 0.001f)
                ? (bandX > bandY ? "left and right (the texture's dark corner)" : "top and bottom (the texture's dark corner)")
                : "none (it fills the view as 1 player's fills the screen)");
    if (bandX > 0.001f || bandY > 0.001f)
        if (ScLog())
            Log("[scope]   bands: %.3f of the view's width at each side, %.3f of its height at the top and the bottom; made %s, "
                "shown %s / %s",
                bandX, bandY, (s.band[0] && s.band[1]) ? "yes" : "NO", s.band[0] && (ReadU32(s.band[0] + kScGrFlags) & kScElemShown) ? "yes" : "no",
                s.band[1] && (ReadU32(s.band[1] + kScGrFlags) & kScElemShown) ? "yes" : "no");
}

// ---- 21j-r5 fit begin (game thread: ScopeDisplay::Update, right after the game's) ----
// a dark band: the game's new + RedBitmapElement's constructor (as ScopeDisplay's quarters), the texture's outer corner,
// added to the scope's group (it shows and hides with the scope); 0 if it could not be made
uintptr_t ScBandMake(uintptr_t group)
{
    const uintptr_t mem = g_scNew(kScBmpSize);
    if (!mem)
        return 0;
    const uintptr_t e = g_scBmpCtor(mem, nullptr);
    if (!e)
        return 0;
    g_scSetUv(e, nullptr, kScBandU0, kScBandU0, kScBandU1, kScBandU1, 0);
    g_scSetRect(e, nullptr, 0.0f, 0.0f, 0.0f, 0.0f);
    g_scAddChild(group, nullptr, e);
    InterlockedIncrement(&g_scBandsMade);
    return e;
}

void ScShow(uintptr_t e, bool on)
{
    uint32_t* const f = reinterpret_cast<uint32_t*>(e + kScGrFlags);
    *f = on ? (*f | kScElemShown) : (*f & ~kScElemShown);
}

// an element's rectangle (SetRect's vertex 0 and 3) as asked, within 0.01
bool ScRectIs(uintptr_t e, float x0, float y0, float x1, float y1)
{
    const float* const r0 = reinterpret_cast<const float*>(e + kScBmX0);
    const float* const r3 = reinterpret_cast<const float*>(e + kScBmX1);
    return fabsf(r0[0] - x0) < 0.01f && fabsf(r0[1] - y0) < 0.01f && fabsf(r3[0] - x1) < 0.01f && fabsf(r3[1] - y1) < 0.01f;
}

// e is a child of the live group g (its child list walked - g's memory, never e's) and not one of the scope's own
// quarters: one of our bands for THIS scope. The game makes its scopes again for every match (in its level memory, the
// same addresses coming back), so a band pointer kept from an earlier match may point at anything now - it is only
// written to once it is found in the current group's list.
bool ScBandOurs(uintptr_t g, uintptr_t d, uintptr_t e)
{
    if (!e || (e >= d && e < d + kScSdQuarter0 + 4 * kScSdQuarterSpan))
        return false;
    const uintptr_t head = g + kScGrChildren;
    uintptr_t node = *reinterpret_cast<const uint32_t*>(head);
    for (int guard = 0; node && node != head && guard < 32; ++guard)
    {
        if (*reinterpret_cast<const uint32_t*>(node + 0x0C) == e)
            return true;
        node = *reinterpret_cast<const uint32_t*>(node);
    }
    return false;
}

// true if it was changed
bool ScRectSet(uintptr_t e, float x0, float y0, float x1, float y1)
{
    if (ScRectIs(e, x0, y0, x1, y1))
        return false;
    g_scSetRect(e, nullptr, x0, y0, x1, y1);
    return true;
}

// the scope fitted to its view (split screen), or put back as the game has it (a full-screen root again)
void ScFitApply(ScSeen& s, uintptr_t d, uintptr_t root)
{
    const int32_t W = ScScreenW();
    const int32_t H = ScScreenH();
    if (!root || W < 64 || H < 64)
        return;
    const int32_t rw = static_cast<int32_t>(ReadU32(root));
    const int32_t rh = static_cast<int32_t>(ReadU32(root + 4));
    if (rw < 16 || rh < 16 || rw > W + 1 || rh > H + 1)
        return; // (not a HUD root as AddViewport makes them)
    const bool split = rw < W - 1 || rh < H - 1;
    if (!split && !s.touched)
        return; // 1 player: the game's own, never touched
    const uintptr_t g = d + kScSdGroup;
    ScFit(s, rw, rh, W, H);
    // (put back: the constructor's own halves, W / 2 and H / 2 in whole units)
    const float hw = split ? 0.5f * s.s * static_cast<float>(W) : static_cast<float>(W >> 1);
    const float hh = split ? 0.5f * s.s * static_cast<float>(H) : static_cast<float>(H >> 1);
    // (1) the anchor: (0.5, 0.5) of the view itself (flag 0), or through the safe area again (the game's 1)
    const float ax = *reinterpret_cast<const float*>(g + kScGrAnchorX);
    const float ay = *reinterpret_cast<const float*>(g + kScGrAnchorY);
    if (fabsf(ax - 0.5f) < 0.001f && fabsf(ay - 0.5f) < 0.001f)
        *reinterpret_cast<uint8_t*>(g + kScGrAnchorFlag) = split ? 0 : 1;
    // (2) the quarters: top left, top right, bottom left, bottom right round the group's (0, 0)
    const float q[4][4] = { { -hw, -hh, 0.0f, 0.0f }, { 0.0f, -hh, hw, 0.0f }, { -hw, 0.0f, 0.0f, hh }, { 0.0f, 0.0f, hw, hh } };
    bool changed = false;
    for (int i = 0; i < 4; ++i)
        changed = ScRectSet(d + kScSdQuarter0 + kScSdQuarterSpan * static_cast<uint32_t>(i), q[i][0], q[i][1], q[i][2], q[i][3]) ||
                  changed;
    if (changed)
        InterlockedIncrement(&g_scFits);
    // (3) the dark bands at the long side's ends (a half)
    const float vw = 0.5f * static_cast<float>(rw);
    const float vh = 0.5f * static_cast<float>(rh);
    const bool across = split && hw < vw - 0.5f;
    const bool down = split && !across && hh < vh - 0.5f;
    const uint32_t tex = *reinterpret_cast<const uint32_t*>(d + kScSdTexture);
    // bands kept from an earlier scope at this address are forgotten (never written to)
    for (int k = 0; k < 2; ++k)
        if (s.band[k] && !ScBandOurs(g, d, s.band[k]))
        {
            s.band[k] = 0;
            s.bandTex = 0;
        }
    bool bands = (across || down) && tex != 0;
    for (int k = 0; k < 2 && bands; ++k)
    {
        if (!s.band[k])
        {
            s.band[k] = ScBandMake(g);
            s.bandTex = 0;
            if (s.band[k] && !ScBandOurs(g, d, s.band[k]))
                s.band[k] = 0; // (AddChild refused it: left alone)
        }
        if (!s.band[k] || *reinterpret_cast<const uint32_t*>(s.band[k]) != static_cast<uint32_t>(Rva(g_scBmpVtRva)))
            bands = false;
    }
    if (bands && s.bandTex != tex)
    {
        for (int k = 0; k < 2; ++k)
        {
            const uintptr_t vt = *reinterpret_cast<const uint32_t*>(s.band[k]);
            reinterpret_cast<ScSetTex_t>(*reinterpret_cast<const uint32_t*>(vt + 4u * kScBmpSlotTexture))(s.band[k], nullptr, tex);
        }
        s.bandTex = tex;
    }
    if (bands)
    {
        const float p = kScBandPad;
        if (across)
        {
            ScRectSet(s.band[0], -vw - p, -vh - p, -hw, vh + p);
            ScRectSet(s.band[1], hw, -vh - p, vw + p, vh + p);
        }
        else
        {
            ScRectSet(s.band[0], -vw - p, -vh - p, vw + p, -hh);
            ScRectSet(s.band[1], -vw - p, hh, vw + p, vh + p);
        }
    }
    for (int k = 0; k < 2; ++k)
        if (s.band[k])
            ScShow(s.band[k], bands);
    s.touched = split;
    if (changed && g_scopeScanWanted && s.fitLogs < kScFitLogsEach && ScLog())
    {
        ++s.fitLogs;
        Log("[scope] %lu ms: 21j-r5: player %d's scope %s (roots[] %ld x %ld units of %ld x %ld): quarters +-%.0f x +-%.0f units (the "
            "scope %.2f x %.2f of the view), its middle %s; dark bands %s",
            ScanMsSinceAttach(), s.player + 1, split ? "fitted to his view" : "put back as the game has it", static_cast<long>(rw),
            static_cast<long>(rh), static_cast<long>(W), static_cast<long>(H), hw, hh, split ? s.fx : 1.0f, split ? s.fy : 1.0f,
            split ? "at the view's middle (flag 0)" : "through the safe area (flag 1)",
            bands ? (across ? "left and right" : "top and bottom") : ((across || down) ? "NOT made" : "none"));
    }
}

// 21j-r6: the game's last quad of a view, logged once each time ScBlurWrap arms it (with the view's viewport line): its
// 4 vertices and what they say - the picture's u v at the view's edges against the view's share of the screen. The game
// gives u = (the view's share) x (256 / the target's width): right for a view at the screen's top left; for any other
// it is right only if the blur pass drew its picture at the target's top left too.
void ScQuadLog(const float* v)
{
    const int view = g_scViewVar ? *reinterpret_cast<const int32_t*>(g_scViewVar) : -1;
    if (view < 0 || view > 3 || !g_scQuadNext[view])
        return;
    g_scQuadNext[view] = false;
    float n[4] = {};
    if (g_scVpNorm)
        g_scVpNorm(n);
    const float u0 = v[4];
    const float w0 = v[5];
    const float u1 = v[28];
    const float w1 = v[29];
    const float sx = n[2] > 0.0f ? (u1 - u0) / n[2] : 0.0f;
    const float sy = n[3] > 0.0f ? (w1 - w0) / n[3] : 0.0f;
    if (ScLog())
        Log("[scope] %lu ms: view %d's scope blur, the game's last quad (x y | picture u v | mask u v): %.1f %.1f | %.4f %.4f | %.3f "
            "%.3f; %.1f %.1f | %.4f %.4f | %.3f %.3f; %.1f %.1f | %.4f %.4f | %.3f %.3f; %.1f %.1f | %.4f %.4f | %.3f %.3f",
            ScanMsSinceAttach(), view, v[0], v[1], v[4], v[5], v[6], v[7], v[8], v[9], v[12], v[13], v[14], v[15], v[16], v[17], v[20],
            v[21], v[22], v[23], v[24], v[25], v[28], v[29], v[30], v[31]);
    if (ScLog())
        Log("[scope]   view %d is %.3f %.3f %.3f x %.3f of the screen; its picture u v run %.4f..%.4f x %.4f..%.4f - the blur picture's "
            "scale %.4f x %.4f (256 / the target's size); the game's u v at the view's top left %.4f %.4f (its place x the scale); "
            "a picture drawn in the view's own place would need %.4f %.4f there, one at the target's top left 0 0",
            view, n[0], n[1], n[2], n[3], u0, u1, w0, w1, sx, sy, u0, w0, n[0], n[1]);
}

// the blur's last quad (its unlock, through the stub; esi = 4 vertices of 20h: x y z w, the picture's u v, the mask's u
// v). 21j-r5: in a half's second pass the quad covered the scope's rectangle (the mask's hole at 1 player's size round
// the ring). 21j-r6: a half's scope is drawn in up to 5 passes that never overlap - the strips left / right of the scope's
// rectangle (0 / 1) and above / below it (2 / 3) with the mask's corner on all 4 vertices (all blur), then the rectangle
// itself (4) with the mask stretched over the whole scope (its part inside the view where the scope runs past it). Places
// and the picture's u v are moved in; the game's quad is left as it is if it is not the shape expected.
// 21j-r7 ([Splitscreen] ScopeBlurFix=1): THE DOUBLE BLUR. r6's trace: the blur pass draws its picture with the blur
// target's whole viewport (0 0 2048 2048), at the target's top left (256 x 256 px: u v 0 - 0.125) for EVERY view; the game's
// last quad reads it at (the view's share of the screen) x 0.125 - for 1 player 0 - 0.125 (right), for a quarter only a
// quarter of the picture (player 1's top left, player 2's top right, ...) stretched over the view: the blur round the scope
// was a 2 x larger copy of part of the view. The picture's u v now run 0 - 0.125 over every view (u' = (u - u0) / the view's
// share). Only when the game blurred into its target (the scale under 1; without it the game reads the screen copy and its
// u v are right); 1 player's quad is never touched (its u v are 0 - 0.125 already).
void __cdecl ScBlurPass2C(float* v)
{
    if (!v)
        return;
    ScQuadLog(v); // 21j-r6 (the game's quad, before anything is moved)
    if (!g_scP2Active && !g_scBlurFixOn)
        return;
    const float x0 = v[0];
    const float y0 = v[1];
    const float x1 = v[24];
    const float y1 = v[25];
    float u0 = v[4];
    float w0 = v[5];
    float u1 = v[28];
    float w1 = v[29];
    const float m0 = v[6];  // the mask's u v at the view's top left / bottom right
    const float n0 = v[7];
    const float m1 = v[30];
    const float n1 = v[31];
    // the game's quad: vertex 1 (x0, y1), vertex 2 (x1, y0), at least 2 px each way; picture u v the same at shared edges
    const bool shape = x1 - x0 > 2.0f && y1 - y0 > 2.0f && fabsf(v[8] - x0) < 0.5f && fabsf(v[9] - y1) < 0.5f &&
                       fabsf(v[16] - x1) < 0.5f && fabsf(v[17] - y0) < 0.5f && fabsf(v[12] - u0) < 1e-5f && fabsf(v[13] - w1) < 1e-5f &&
                       fabsf(v[20] - u1) < 1e-5f && fabsf(v[21] - w0) < 1e-5f;
    if (!shape)
    {
        InterlockedIncrement(&g_scP2Skipped);
        return;
    }
    // 21j-r7: the picture's u v from the blur picture's top left, over the view
    if (g_scBlurFixOn && g_scVpNorm)
    {
        float n[4] = {};
        g_scVpNorm(n);
        const bool split = n[0] > 1e-4f || n[1] > 1e-4f || n[2] < 0.999f || n[3] < 0.999f;
        const float su = n[2] > 0.05f ? (u1 - u0) / n[2] : 0.0f;
        const float sv = n[3] > 0.05f ? (w1 - w0) / n[3] : 0.0f;
        if (split && n[2] <= 1.001f && n[3] <= 1.001f && su > 0.001f && su < 0.99f && sv > 0.001f && sv < 0.99f)
        {
            u0 = 0.0f;
            w0 = 0.0f;
            u1 = su;
            w1 = sv;
            InterlockedIncrement(&g_scFixQuads);
            if (!g_scP2Active)
            {
                const float u[4] = { u0, u0, u1, u1 };
                const float w[4] = { w0, w1, w0, w1 };
                for (int i = 0; i < 4; ++i)
                {
                    v[8 * i + 4] = u[i];
                    v[8 * i + 5] = w[i];
                }
                return;
            }
        }
    }
    if (!g_scP2Active)
        return;
    if (!(g_scP2Fx > 0.05f && g_scP2Fx <= 4.0f && g_scP2Fy > 0.05f && g_scP2Fy <= 4.0f))
    {
        InterlockedIncrement(&g_scP2Skipped);
        return;
    }
    const float cx = 0.5f * (x0 + x1);
    const float cy = 0.5f * (y0 + y1);
    const float hw = 0.5f * (x1 - x0) * g_scP2Fx;
    const float hh = 0.5f * (y1 - y0) * g_scP2Fy;
    const float sx0 = cx - hw; // the scope's rectangle
    const float sx1 = cx + hw;
    const float sy0 = cy - hh;
    const float sy1 = cy + hh;
    const float rx0 = sx0 > x0 ? sx0 : x0; // ... inside the view
    const float rx1 = sx1 < x1 ? sx1 : x1;
    const float ry0 = sy0 > y0 ? sy0 : y0;
    const float ry1 = sy1 < y1 ? sy1 : y1;
    float qx0 = rx0;
    float qx1 = rx1;
    float qy0 = ry0;
    float qy1 = ry1;
    bool corner = true;
    switch (g_scP2Mode)
    {
    case 0: qx0 = x0; qx1 = rx0; qy0 = y0; qy1 = y1; break;
    case 1: qx0 = rx1; qx1 = x1; qy0 = y0; qy1 = y1; break;
    case 2: qy0 = y0; qy1 = ry0; break;
    case 3: qy0 = ry1; qy1 = y1; break;
    default: corner = false; break;
    }
    if (qx1 - qx0 < 0.5f || qy1 - qy0 < 0.5f)
    {
        for (int i = 0; i < 4; ++i) // nothing to draw: all 4 vertices on one point
        {
            v[8 * i] = x0;
            v[8 * i + 1] = y0;
        }
        InterlockedIncrement(&g_scP2Skipped);
        return;
    }
    const float nu0 = u0 + (qx0 - x0) / (x1 - x0) * (u1 - u0);
    const float nu1 = u0 + (qx1 - x0) / (x1 - x0) * (u1 - u0);
    const float nw0 = w0 + (qy0 - y0) / (y1 - y0) * (w1 - w0);
    const float nw1 = w0 + (qy1 - y0) / (y1 - y0) * (w1 - w0);
    const float mu0 = corner ? m0 + kScMaskCorner * (m1 - m0) : m0 + (qx0 - sx0) / (sx1 - sx0) * (m1 - m0);
    const float mu1 = corner ? mu0 : m0 + (qx1 - sx0) / (sx1 - sx0) * (m1 - m0);
    const float mv0 = corner ? n0 + kScMaskCorner * (n1 - n0) : n0 + (qy0 - sy0) / (sy1 - sy0) * (n1 - n0);
    const float mv1 = corner ? mv0 : n0 + (qy1 - sy0) / (sy1 - sy0) * (n1 - n0);
    const float x[4] = { qx0, qx0, qx1, qx1 };
    const float y[4] = { qy0, qy1, qy0, qy1 };
    const float u[4] = { nu0, nu0, nu1, nu1 };
    const float w[4] = { nw0, nw1, nw0, nw1 };
    const float mu[4] = { mu0, mu0, mu1, mu1 };
    const float mv[4] = { mv0, mv1, mv0, mv1 };
    for (int i = 0; i < 4; ++i)
    {
        float* const p = v + 8 * i;
        p[0] = x[i];
        p[1] = y[i];
        p[4] = u[i];
        p[5] = w[i];
        p[6] = mu[i];
        p[7] = mv[i];
    }
    InterlockedIncrement(&g_scP2Quads);
}
// ---- 21j-r5 fit end ----

void ScSummary()
{
    const DWORD now = GetTickCount();
    if (g_scLastSummary && now - g_scLastSummary < 30000)
        return;
    g_scLastSummary = now;
    LONG shows = 0;
    for (int i = 0; i < kScDisplays; ++i)
        shows += g_sc[i].shows;
    if (shows == g_scShowsSummed)
        return;
    g_scShowsSummed = shows;
    char text[400] = {};
    size_t at = 0;
    for (int i = 0; i < kScDisplays && at < sizeof(text) - 80; ++i)
    {
        const ScSeen& s = g_sc[i];
        if (!s.d)
            continue;
        const int w = snprintf(text + at, sizeof(text) - at, "%sP%d 0x%08X root %d (%ld x %ld) %ld up / %ld updates", at ? "; " : "",
                               s.player + 1, static_cast<unsigned>(s.d), s.root, static_cast<long>(s.rw), static_cast<long>(s.rh),
                               static_cast<long>(s.shows), static_cast<long>(s.updates));
        if (w > 0)
            at += static_cast<size_t>(w);
    }
    if (ScLog())
        Log("[scope] %lu ms: summary - %s; scope blur draws %ld; 21j-r5: quarters set %ld time(s), bands made %ld; 21j-r6: a half's "
            "blur passes %ld, their quads moved in %ld (%ld left as they were or empty); 21j-r7: last quads read from the blur "
            "picture's top left %ld",
            ScanMsSinceAttach(), text, static_cast<long>(g_scBlurDraws), static_cast<long>(g_scFits), static_cast<long>(g_scBandsMade),
            static_cast<long>(g_scP2Passes), static_cast<long>(g_scP2Quads), static_cast<long>(g_scP2Skipped),
            static_cast<long>(g_scFixQuads));
}

// ScopeDisplay::Update (its vtable slot; game thread): the game's first, then a look (never a write)
uint8_t __fastcall ScUpdateWrap(void* self, void* edx, uint32_t arg)
{
    const uint8_t r = reinterpret_cast<ScUpdate_t>(g_scOrigUpdate)(self, edx, arg);
    const uintptr_t d = reinterpret_cast<uintptr_t>(self);
    ScSeen* s = d ? ScFind(d) : nullptr;
    if (!s)
        return r;
    ++s->updates;
    s->lastTick = GetTickCount();
    s->player = *reinterpret_cast<const int32_t*>(d + kScSdPlayer);
    uintptr_t root = 0;
    s->root = ScRootOf(d + kScSdGroup, &root);
    s->rw = root ? static_cast<int32_t>(ReadU32(root)) : 0;
    s->rh = root ? static_cast<int32_t>(ReadU32(root + 4)) : 0;
    if (g_scFitOn) // 21j-r5
        ScFitApply(*s, d, root);
    if (!g_scopeScanWanted)
    {
        s->shown = *reinterpret_cast<const uint8_t*>(d + kScSdShown) != 0;
        if (s->rw > 0)
            ScFit(*s, s->rw, s->rh, ScScreenW(), ScScreenH());
        return r;
    }
    const bool shown = *reinterpret_cast<const uint8_t*>(d + kScSdShown) != 0;
    if (shown && !s->shown)
        ++s->shows;
    const bool newRoot = s->rw != s->loggedRw || s->rh != s->loggedRh;
    // its first 2 shows, then a show (or a scope up) in a HUD root of another size - a new layout
    if (shown && s->logs < kScLogsEach && ((!s->shown && (s->logs < 2 || newRoot)) || (s->shown && newRoot)))
    {
        ++s->logs;
        s->loggedRw = s->rw;
        s->loggedRh = s->rh;
        ScLogDisplay(*s, d, s->root, root);
    }
    else if (s->rw > 0)
        ScFit(*s, s->rw, s->rh, ScScreenW(), ScScreenH());
    s->shown = shown;
    ScSummary();
    return r;
}

// the scope blur's draw (its vtable slot): the viewport of each view drawn with a scope up, then the game's draw
// 21j-r6: a half's scope in up to 5 passes (ScBlurPass2C); with the scan, each new viewport size of a view also arms the
// next last-quad log and a device trace (render_hooks.cpp) for that draw
void __fastcall ScBlurWrap(void* self, void* edx, uint32_t flags)
{
    const int view = g_scViewVar ? *reinterpret_cast<const int32_t*>(g_scViewVar) : -1;
    // the scope in this view: a display updated in the last second (one left from an earlier match keeps its last
    // values for ever), the newest if more than one
    const ScSeen* who = nullptr;
    const DWORD now = GetTickCount();
    for (int i = 0; i < kScDisplays && view >= 0; ++i)
        if (g_sc[i].d && g_sc[i].shown && g_sc[i].root == view && now - g_sc[i].lastTick < 1000 &&
            (!who || g_sc[i].lastTick - who->lastTick < 0x80000000u))
            who = &g_sc[i];
    bool traced = false;
    if (who && g_scopeScanWanted && !(flags & 0x200000) && view >= 0 && view < 4)
    {
        InterlockedIncrement(&g_scBlurDraws);
        float n[4] = {};
        float l = 0.0f;
        float t = 0.0f;
        float rr = 0.0f;
        float b = 0.0f;
        g_scVpNorm(n);
        g_scVpPx(&l, &t, &rr, &b);
        const float w = rr - l;
        const float h = b - t;
        bool seen = false;
        int size = -1; // 21j-r6: which of the view's sizes this is
        for (int i = 0; i < g_scViewLogged[view]; ++i)
            if (fabsf(g_scViewW[view][i] - w) < 0.5f && fabsf(g_scViewH[view][i] - h) < 0.5f)
            {
                seen = true;
                size = i;
            }
        if (!seen && g_scViewLogged[view] < kScViewLogs)
        {
            size = g_scViewLogged[view];
            g_scViewW[view][g_scViewLogged[view]] = w;
            g_scViewH[view][g_scViewLogged[view]] = h;
            ++g_scViewLogged[view];
            g_scQuadNext[view] = g_scStubOn; // 21j-r6: this draw's last quad logged
            const float sw = w * who->fx;
            const float sh = h * who->fy;
            const float cx = l + 0.5f * w;
            const float cy = t + 0.5f * h;
            if (ScLog())
                Log("[scope] %lu ms: view %d's scope blur (player %d): viewport %.0f %.0f - %.0f %.0f px (%.0f x %.0f; the screen "
                    "%.0f x %.0f), as a share of the screen %.3f %.3f %.3f x %.3f; the scope %.0f x %.0f px round %.0f %.0f (%.2f x %.2f "
                    "of the view)%s",
                    ScanMsSinceAttach(), view, who->player + 1, l, t, rr, b, w, h, n[2] > 0.0f ? w / n[2] : 0.0f,
                    n[3] > 0.0f ? h / n[3] : 0.0f, n[0], n[1], n[2], n[3], sw, sh, cx, cy, who->fx, who->fy,
                    (fabsf(who->fx - 1.0f) > 0.001f || fabsf(who->fy - 1.0f) > 0.001f)
                        ? (g_scPass2On ? "; 21j-r6: drawn in passes - the strips outside the scope's rectangle all blurred, then the "
                                         "rectangle with the mask's hole round the ring"
                                       : "; the blur as the game draws it (the passes are off)")
                        : "; the blur as the game draws it (the scope fills the view)");
        }
        // 21j-r6: a device trace of this draw - once for each size of each view (the first frame no trace is running in)
        if (size >= 0 && !g_scTraced[view][size] && FrameScopeTraceArm(view, who->player + 1))
        {
            g_scTraced[view][size] = true;
            traced = true;
            g_scQuadNext[view] = g_scStubOn;
        }
    }
    const bool passes = g_scPass2On && who && !(flags & 0x200000) && (fabsf(who->fx - 1.0f) > 0.001f || fabsf(who->fy - 1.0f) > 0.001f);
    if (!passes)
        reinterpret_cast<ScBlurDraw_t>(g_scOrigBlur)(self, edx, flags);
    else
    {
        // 21j-r6: a half's scope - the strips beside it (left / right where it is narrower than the view, above / below
        // where it is lower), then its own rectangle; ScBlurPass2C moves each pass's last quad (the stub)
        g_scP2Fx = who->fx;
        g_scP2Fy = who->fy;
        const bool need[5] = { who->fx < 0.999f, who->fx < 0.999f, who->fy < 0.999f, who->fy < 0.999f, true };
        for (int m = 0; m < 5; ++m)
        {
            if (!need[m])
                continue;
            g_scP2Mode = m;
            g_scP2Active = true;
            reinterpret_cast<ScBlurDraw_t>(g_scOrigBlur)(self, edx, flags);
            InterlockedIncrement(&g_scP2Passes);
        }
        g_scP2Active = false;
        g_scP2Mode = 4;
    }
    if (traced)
        FrameScopeTraceNote(passes ? "the scope blur's draw function returned (its passes made)" : "the scope blur's draw function returned");
}

// a vtable slot through BF1GameExt: its old value goes in *orig (the wrapper's way on) BEFORE the slot moves - the game
// thread may call through it the moment it does; false if it could not be written
bool ScSlotWrite(uintptr_t slot, const void* to, uintptr_t* orig)
{
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<void*>(slot), 4, PAGE_READWRITE, &old))
        return false;
    *orig = *reinterpret_cast<volatile const uintptr_t*>(slot);
    MemoryBarrier();
    *reinterpret_cast<volatile uintptr_t*>(slot) = reinterpret_cast<uintptr_t>(to);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<void*>(slot), 4, old, &ignored);
    return *reinterpret_cast<volatile const uintptr_t*>(slot) == reinterpret_cast<uintptr_t>(to);
}

// a vtable's RTTI class name ("" if none)
void ScVtName(uint32_t vt, char* out, size_t n)
{
    const uint32_t v = vt;
    ClassName(reinterpret_cast<uintptr_t>(&v), out, n);
}

// a slot's function: its value with jump thunks followed (rva; 0 if not in the code)
uint32_t ScSlotFn(uintptr_t vtVa, uint32_t slot)
{
    const uint32_t v = ReadU32(vtVa + 4u * slot);
    if (!InImage(v))
        return 0;
    uint32_t f = RvaOf(v);
    for (int hop = 0; hop < 8 && CodeByte(f) == 0xE9; ++hop)
        f = RelTarget(f);
    return f;
}

// 21j-r5: the stub at the blur's last-pass unlock (call -> here): registers, flags and xmm0 - 7 kept round
// ScBlurPass2C(esi = the 4 vertices), then on to the unlock with the call's own return address
bool ScPass2Install(uint32_t commit)
{
    const uint32_t site = commit + 2; // the E8 of "mov ecx, edi; call unlock"
    if (!MatchPatternAt(commit, kScBlurCommit) || CodeByte(site) != 0xE8)
        return false;
    const uintptr_t orig = Rva(RelTarget(site));
    if (!InImage(orig))
        return false;
    uint8_t* const stub = static_cast<uint8_t*>(VirtualAlloc(nullptr, kScStubBytes, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!stub)
        return false;
    static const uint8_t kHead[] = { 0x60, 0x9C, 0x81, 0xEC, 0x80, 0x00, 0x00, 0x00,                // pushad, pushfd, sub esp, 80h
                                     0x0F, 0x11, 0x04, 0x24, 0x0F, 0x11, 0x4C, 0x24, 0x10,          // movups [esp + 0 / 10h], xmm0 / 1
                                     0x0F, 0x11, 0x54, 0x24, 0x20, 0x0F, 0x11, 0x5C, 0x24, 0x30,    // xmm2 / 3
                                     0x0F, 0x11, 0x64, 0x24, 0x40, 0x0F, 0x11, 0x6C, 0x24, 0x50,    // xmm4 / 5
                                     0x0F, 0x11, 0x74, 0x24, 0x60, 0x0F, 0x11, 0x7C, 0x24, 0x70,    // xmm6 / 7
                                     0x56, 0xE8 };                                                  // push esi; call
    static const uint8_t kTail[] = { 0x83, 0xC4, 0x04,                                              // add esp, 4
                                     0x0F, 0x10, 0x04, 0x24, 0x0F, 0x10, 0x4C, 0x24, 0x10,          // movups xmm0 / 1, [esp + 0 / 10h]
                                     0x0F, 0x10, 0x54, 0x24, 0x20, 0x0F, 0x10, 0x5C, 0x24, 0x30,    // xmm2 / 3
                                     0x0F, 0x10, 0x64, 0x24, 0x40, 0x0F, 0x10, 0x6C, 0x24, 0x50,    // xmm4 / 5
                                     0x0F, 0x10, 0x74, 0x24, 0x60, 0x0F, 0x10, 0x7C, 0x24, 0x70,    // xmm6 / 7
                                     0x81, 0xC4, 0x80, 0x00, 0x00, 0x00, 0x9D, 0x61, 0xE9 };        // add esp, 80h; popfd; popad; jmp
    static_assert(sizeof(kHead) + 4 + sizeof(kTail) + 4 <= kScStubBytes, "the scope blur stub does not fit its page");
    size_t at = 0;
    memcpy(stub + at, kHead, sizeof(kHead));
    at += sizeof(kHead);
    const int32_t toC = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&ScBlurPass2C) - reinterpret_cast<uintptr_t>(stub + at + 4));
    memcpy(stub + at, &toC, 4);
    at += 4;
    memcpy(stub + at, kTail, sizeof(kTail));
    at += sizeof(kTail);
    const int32_t toOrig = static_cast<int32_t>(orig - reinterpret_cast<uintptr_t>(stub + at + 4));
    memcpy(stub + at, &toOrig, 4);
    FlushInstructionCache(GetCurrentProcess(), stub, kScStubBytes);
    g_scCommitOrig = orig;
    // the call goes to the stub
    uint8_t* const call = reinterpret_cast<uint8_t*>(Rva(site));
    DWORD old = 0;
    if (!VirtualProtect(call + 1, 4, PAGE_EXECUTE_READWRITE, &old))
        return false;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(stub) - reinterpret_cast<uintptr_t>(call + 5));
    memcpy(call + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(call + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    int32_t back = 0;
    memcpy(&back, call + 1, 4);
    return back == rel;
}

void InstallScopeScan()
{
    if (!g_scopeScanWanted && !g_scopeFitWanted && !g_scBlurFixWanted)
    {
        Log("[scope] 21j-r7: [Splitscreen] ScopeScan=0, ScopeFit=0, ScopeBlurFix=0 - the scope as the game has it, no scan");
        return;
    }
    // (1) ScopeDisplay's constructor: the HUD scan's (FindHud34), else by its pattern; both parts checked
    uint32_t ctor = g_h34[kScHud].ctor;
    const char* how = "the HUD scan's";
    if (!ctor || !MatchPatternAt(ctor, kScA))
    {
        uint32_t hits[4] = {};
        const int n = FindPattern(kScA, hits, 4);
        if (ctor)
            Log("[scope] the HUD scan's ScopeDisplay constructor rva 0x%08X does not match the scope scan's pattern (bytes: "
                "%02X %02X %02X %02X %02X %02X %02X %02X) - %d pattern hit(s)",
                ctor, CodeByte(ctor), CodeByte(ctor + 1), CodeByte(ctor + 2), CodeByte(ctor + 3), CodeByte(ctor + 4),
                CodeByte(ctor + 5), CodeByte(ctor + 6), CodeByte(ctor + 7), n);
        ctor = n == 1 ? hits[0] : 0;
        how = "by pattern";
    }
    const bool partB = ctor && MatchPatternAt(ctor + kScBAt, kScB);
    if (!ctor || !partB)
    {
        Log("[scope] 21j-r4: ScopeDisplay's constructor %s (rva 0x%08X%s) - the scope scan is NOT in", ctor ? "found" : "NOT found", ctor,
            ctor ? ", its second part does NOT match" : "");
        return;
    }
    const uint32_t vt = OperandRva(ctor + kScVt);
    char name[64] = {};
    if (vt)
        ScVtName(static_cast<uint32_t>(Rva(vt)), name, sizeof(name));
    const uint32_t update = vt ? ScSlotFn(Rva(vt), kScSlotUpdate) : 0;
    const bool updateOk = update && MatchPatternAt(update, kScUpdateBody);
    // the screen object: both getters "mov ecx, screen; jmp" to "mov eax, [ecx]; ret" / "mov eax, [ecx + 4]; ret"
    const uint32_t gw = RelTarget(ctor + kScGetW);
    const uint32_t gh = RelTarget(ctor + kScGetH);
    uint32_t gwt = gw;
    uint32_t ght = gh;
    for (int hop = 0; hop < 8 && CodeByte(gwt) == 0xE9; ++hop)
        gwt = RelTarget(gwt);
    for (int hop = 0; hop < 8 && CodeByte(ght) == 0xE9; ++hop)
        ght = RelTarget(ght);
    const bool screenOk = MatchPatternAt(gwt, kScScreenObj) && MatchPatternAt(ght, kScScreenObj) &&
                          CodeU32(gwt + 1) == CodeU32(ght + 1) && MatchPatternAt(CallTarget(gwt + 5), kScScreenW) &&
                          MatchPatternAt(CallTarget(ght + 5), kScScreenH) && InImage(CodeU32(gwt + 1));
    g_scScreen = screenOk ? static_cast<uintptr_t>(CodeU32(gwt + 1)) : 0;
    // 21j-r5's sites (checked, logged; nothing calls them here)
    const uint32_t bmpCtorThunk = OperandRva(ctor + kScBmpCtor);
    uint32_t bmpCtor = bmpCtorThunk;
    for (int hop = 0; hop < 8 && bmpCtor && CodeByte(bmpCtor) == 0xE9; ++hop)
        bmpCtor = RelTarget(bmpCtor);
    const bool bmpOk = bmpCtor && MatchPatternAt(bmpCtor, kScBmpCtorBody);
    const uint32_t bmpVt = bmpOk ? OperandRva(bmpCtor + kScBmpVt) : 0;
    char bmpName[64] = {};
    if (bmpVt)
        ScVtName(static_cast<uint32_t>(Rva(bmpVt)), bmpName, sizeof(bmpName));
    const uint32_t setTex = bmpVt ? ScSlotFn(Rva(bmpVt), kScBmpSlotTexture) : 0;
    const bool setTexOk = setTex && MatchPatternAt(setTex, kScSetTextureBody);
    const uint32_t setSize = CallTarget(ctor + kScSetSize);
    const bool setSizeOk = MatchPatternAt(setSize, kScSetSizeBody) && MatchPatternAt(CallTarget(setSize + kScSetRectCall), kScSetRectBody);
    const uint32_t setUv = CallTarget(ctor + kScSetUv);
    const bool setUvOk = MatchPatternAt(setUv, kScSetUvBody);
    const uint32_t addChild = CallTarget(ctor + kScAddChild);
    const bool addChildOk = MatchPatternAt(addChild, kScAddChildBody);
    const uint32_t newFn = g_h34[kScHud].newFn;
    uint32_t hits[4] = {};
    const int nRoot = FindPattern(kScAddToRoot, hits, 4);
    g_scRoots = nRoot == 1 && InImage(CodeU32(hits[0] + kScAddToRootTable)) ? static_cast<uintptr_t>(CodeU32(hits[0] + kScAddToRootTable)) : 0;
    Log("[scope] 21j-r4: THE SCOPE SCAN (log only): ScopeDisplay's constructor rva 0x%08X (%s, both parts match), its vtable rva 0x%08X "
        "(%s), Update (slot %u) rva 0x%08X %s; the screen object %s (rva 0x%08X: %ld x %ld units now); HUD roots[] %s (rva 0x%08X)",
        ctor, how, vt, name[0] ? name : "no RTTI name", kScSlotUpdate, update, updateOk ? "OK" : "does NOT match",
        screenOk ? "OK" : "NOT found", RvaOf(g_scScreen), static_cast<long>(ScScreenW()), static_cast<long>(ScScreenH()),
        g_scRoots ? "OK" : "NOT found", RvaOf(g_scRoots));
    Log("[scope] 21j-r5's sites: RedBitmapElement constructor rva 0x%08X (thunk rva 0x%08X) %s, its vtable rva 0x%08X (%s), SetTexture "
        "(slot %u) rva 0x%08X %s; SetSize rva 0x%08X %s (+ SetRect); SetUV rva 0x%08X %s; the group's AddChild rva 0x%08X %s; the game's "
        "new rva 0x%08X (the HUD scan's)",
        bmpCtor, bmpCtorThunk, bmpOk ? "OK" : "does NOT match", bmpVt, bmpName[0] ? bmpName : "no RTTI name", kScBmpSlotTexture, setTex,
        setTexOk ? "OK" : "does NOT match", setSize, setSizeOk ? "OK" : "does NOT match", setUv, setUvOk ? "OK" : "does NOT match",
        addChild, addChildOk ? "OK" : "does NOT match", newFn);
    // (2) the scope blur: its constructor -> vtable (slot 4 = the draw, which must be kPatScopeBlurDraw's function) and
    // its slots (the same as the draw's table)
    const int nb = FindPattern(kScBlurCtor, hits, 4);
    const uint32_t bctor = nb == 1 ? hits[0] : 0;
    const uint32_t bvt = bctor ? OperandRva(bctor + kScBlurCtorVt) : 0;
    char bname[64] = {};
    if (bvt)
        ScVtName(static_cast<uint32_t>(Rva(bvt)), bname, sizeof(bname));
    const uint32_t draw = bvt ? ScSlotFn(Rva(bvt), kScBlurSlotDraw) : 0;
    const bool drawOk = draw && MatchPatternAt(draw, kPatScopeBlurDraw) &&
                        CodeU32(draw + kScopeBlurDrawTable) == CodeU32(bctor + kScBlurCtorSlots);
    const uint32_t viewFn = drawOk ? CallTarget(draw + kScBlurViewCall) : 0;
    const bool viewOk = viewFn && MatchPatternAt(viewFn, kScViewGetter) && InImage(CodeU32(viewFn + 1));
    uint32_t vp = 0;
    uint32_t commit = 0;
    int nCommit = 0;
    for (uint32_t i = 0; drawOk && i + 0x40 < kScBlurSearch; ++i)
    {
        if (!vp && MatchPatternAt(draw + i, kScBlurVp))
            vp = draw + i;
        if (MatchPatternAt(draw + i, kScBlurCommit))
        {
            ++nCommit;
            commit = draw + i;
        }
    }
    const uint32_t vpNorm = vp ? CallTarget(vp + kScBlurVpNorm) : 0;
    const uint32_t vpPx = vp ? CallTarget(vp + kScBlurVpPx) : 0;
    const bool vpOk = vp && MatchPatternAt(vpNorm, kScVpNormBody) && MatchPatternAt(vpPx, kScVpPxBody);
    const bool commitOk = nCommit == 1 && CallTarget(commit + 2) != 0;
    Log("[scope] the scope blur: constructor %d hit(s) rva 0x%08X, vtable rva 0x%08X (%s), its draw (slot %u) rva 0x%08X %s; the current "
        "view rva 0x%08X %s; the viewport getters rva 0x%08X / 0x%08X %s; the last pass's unlock (21j-r5's second pass) at draw +%Xh %s "
        "(%d hit(s))",
        nb, bctor, bvt, bname[0] ? bname : "no RTTI name", kScBlurSlotDraw, draw,
        drawOk ? "OK (the draw's slots are the constructor's)" : "does NOT check out", viewFn, viewOk ? "OK" : "NOT found", vpNorm, vpPx,
        vpOk ? "OK" : "NOT found", commit ? commit - draw : 0u, commitOk ? "OK" : "NOT found", nCommit);
    // (3) 21j-r5: the fit's functions (all of them, or no fit)
    const bool newOk = newFn && MatchPatternAt(newFn, kGameNewBody);
    const bool fitSites = screenOk && bmpOk && bmpVt && setTexOk && setSizeOk && setUvOk && addChildOk && newOk;
    if (g_scopeFitWanted && fitSites && updateOk && g_scRoots)
    {
        g_scSetRect = reinterpret_cast<ScSetRect_t>(Rva(CallTarget(setSize + kScSetRectCall)));
        g_scSetUv = reinterpret_cast<ScSetUv_t>(Rva(setUv));
        g_scAddChild = reinterpret_cast<ScAddChild_t>(Rva(addChild));
        g_scNew = reinterpret_cast<ScNew_t>(Rva(newFn));
        g_scBmpCtor = reinterpret_cast<ScBmpCtor_t>(Rva(bmpCtor));
        g_scBmpVtRva = bmpVt;
        g_scFitOn = true; // (before the Update slot moves: the wrapper finds everything set)
    }
    else if (g_scopeFitWanted)
        Log("[scope] 21j-r5: ScopeFit NOT in (something above did not check out: the screen %d, the bitmap %d / %d / %d, SetSize %d, "
            "SetUV %d, AddChild %d, new %d, Update %d, roots %d)",
            screenOk ? 1 : 0, bmpOk ? 1 : 0, bmpVt ? 1 : 0, setTexOk ? 1 : 0, setSizeOk ? 1 : 0, setUvOk ? 1 : 0, addChildOk ? 1 : 0,
            newOk ? 1 : 0, updateOk ? 1 : 0, g_scRoots ? 1 : 0);
    // (4) the two slots through BF1GameExt
    if (!updateOk || !g_scRoots)
    {
        g_scFitOn = false;
        Log("[scope] 21j-r5: NOT in (ScopeDisplay's Update or the HUD roots not found) - the scope as the game has it");
        return;
    }
    if (!ScSlotWrite(Rva(vt) + 4u * kScSlotUpdate, reinterpret_cast<const void*>(&ScUpdateWrap), &g_scOrigUpdate))
    {
        g_scFitOn = false;
        Log("[scope] 21j-r5: ScopeDisplay's Update slot could not be written - NOT in, the scope as the game has it");
        return;
    }
    g_scOn = true;
    bool blurIn = false;
    if (drawOk && viewOk && vpOk)
    {
        g_scViewVar = static_cast<uintptr_t>(CodeU32(viewFn + 1));
        g_scVpNorm = reinterpret_cast<ScVpNorm_t>(Rva(vpNorm));
        g_scVpPx = reinterpret_cast<ScVpPx_t>(Rva(vpPx));
        blurIn = ScSlotWrite(Rva(bvt) + 4u * kScBlurSlotDraw, reinterpret_cast<const void*>(&ScBlurWrap), &g_scOrigBlur);
    }
    // (5) the stub at the blur's last-quad unlock: 21j-r5 / r6's passes over a half's scope (ScopeFit), 21j-r6's quad
    // log (ScopeScan)
    if ((g_scFitOn || g_scopeScanWanted || g_scBlurFixWanted) && blurIn && commitOk)
    {
        g_scBlurFixOn = g_scBlurFixWanted; // (before the stub goes in: it reads it at once)
        g_scStubOn = ScPass2Install(commit);
        if (!g_scStubOn)
            g_scBlurFixOn = false;
    }
    g_scPass2On = g_scStubOn && g_scFitOn;
    Log("[scope] 21j-r7: [Splitscreen] ScopeBlurFix=%d - %s", g_scBlurFixWanted ? 1 : 0,
        g_scBlurFixOn ? "THE DOUBLE BLUR FIXED: in every split view the blur's last pass reads the blur picture from its top left "
                        "(where the blur pass draws it for every view), over the whole view as 1 player's"
                      : (g_scBlurFixWanted ? "NOT in (the blur's stub did not go in)" : "off - the blur as the game reads it"));
    Log("[scope] 21j-r6: [Splitscreen] ScopeFit=%d ScopeScan=%d ScopeHalfFill=%d - %s; the blur in a half %s; the scan %s ([scope] "
        "lines, %d at most)",
        g_scopeFitWanted ? 1 : 0, g_scopeScanWanted ? 1 : 0, g_scHalfFill,
        g_scFitOn ? (g_scHalfFill > 0 ? "THE SCOPE FITS EACH SPLIT VIEW (its middle at the view's middle, 1 player's shape; in a half "
                                        "its ring fills ScopeHalfFill % of the half's short side)"
                                      : "THE SCOPE FITS EACH SPLIT VIEW (its middle at the view's middle, 1 player's shape; a half's "
                                        "as r5's)")
                  : "the scope as the game has it",
        g_scPass2On ? "drawn in passes (the strips beside the scope all blurred, then its rectangle with the mask's hole round the "
                      "ring; the unlock's call goes through the stub)"
                    : (g_scFitOn ? "as the game draws it (the blur did not check out)" : "as the game draws it"),
        g_scopeScanWanted ? (blurIn ? (g_scStubOn ? "logs each scope's first shows, each view's blur viewport, its last quad and a "
                                                    "device trace of its draw ([scope] trace)"
                                                  : "logs each scope's first shows and each view's blur viewport (no stub: no quad log)")
                                     : "logs each scope's first shows (the blur's draw did not check out)")
                          : "off", kScMaxLines);
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r6: THE RETICLE SCAN (log only, [reticle] lines; [Splitscreen] ReticleScan=1 - with players 3 / 4 in). "With players 3
// and 4 their crosshairs are moving a little bit up and down. That should not happen" (the r5 videos, zoomed: player 1's
// reticle steady to 0.3 px, player 3's -3..+2 px, player 4's more). ReticuleDisplay::Update (GOG 1.3 0x002029E0, vtable
// 0x007ED580 slot 1) puts its group (+30h) at anchor (0.5, 0.5 + 0.5 y), y = what its camera - the camera manager's
// GetCamera(player), 0x00083EB0: players 3 / 4's answer BF1GameExt's own camera of view 3 / 4 (20d-r2) - makes of a point
// 1024 ahead along the soldier's aim (the camera's projection 0x0018E260, called at Update +200h). The across place is
// always the middle: only the height comes from the camera. Players 1 / 2's cameras are moved by the game before the HUD
// updates; players 3 / 4's by FrameWrap (the view loop, right before the draw) - after it. If that is it, their reticle is
// placed with the last frame's camera and the picture drawn with this frame's: it moves whenever the camera does, and the
// more the narrower the zoom.
// The scan (nothing in the game changes): the projection's call goes through BF1GameExt - the game's function, then the
// point, the camera and the answer kept per player; in FrameWrap, after players 3 / 4's cameras are set, the same point
// is projected again with the same camera as it is now (what 21j-r7 would set). Per player: frames zoomed and
// not, how far the game's height moved each frame and how far this frame's camera's would, and the two's difference
// (every 15 s).
// r6's log: players 1 / 2 exactly their camera's (0.0000); players 3 / 4 off by up to 0.05 - 0.07 of the view's height
// zoomed, this frame's camera's steady. 21j-r7 (ReticleFixP34=1): their reticle placed with it - see RetFrame.
const char* const kRetCtorHead = "55 8B EC 83 EC 18 53 56 8B D9 57 89 5D E8 E8 ?? ?? ?? ?? 8D 73 30 C7 03";
constexpr uint32_t kRetCtorVt = 0x18;      // its mov [ebx], vtable
constexpr int kRetHud = 3;                 // ReticuleDisplay in the HUD scan's order (g_h34)
constexpr uint32_t kRetSlotUpdate = 1;
const char* const kRetUpdateBody = "55 8B EC 81 EC 88 00 00 00 53 8B D9 56 57 89 5D C4 80 7B 20 00 0F 84";
// mov ecx, [camera]; lea eax, [point]; add esp, 18h; push eax; lea eax, [out]; push eax; call projection; lea ecx, [out];
// call y; mov ecx, [group]; ...; mulss [0.5]; addss [0.5]; ... mov [esp], 0.5; call SetAnchor
const char* const kRetProjSite = "8B 4D E4 8D 45 84 83 C4 18 50 8D 85 78 FF FF FF 50 E8 ?? ?? ?? ?? 8D 8D 78 FF FF FF E8 ?? ?? ?? ?? "
                                 "8B 4D AC 83 EC 08 D9 5D C8 F3 0F 10 45 C8 F3 0F 59 05 ?? ?? ?? ?? F3 0F 58 05 ?? ?? ?? ?? F3 0F 11 "
                                 "44 24 04 C7 04 24 00 00 00 3F E8";
constexpr uint32_t kRetProjCall = 0x11;
constexpr uint32_t kRetSearch = 0x400;     // Update's length to look in
const char* const kRetProjBody = "55 8B EC 83 EC 0C 8D 45 F4 57 FF 75 0C 8B F9 50 E8"; // the camera's projection (this, out, point)
constexpr uint32_t kRetCamArray = 0x2C;    // the camera manager's RedCameras [2] (GetCamera's)
constexpr int kRetMaxLines = 240;
// 21j-r7: the projection's caller's frame (ReticuleDisplay::Update, ebp-based): out = [ebp-88h], point = [ebp-7Ch], the
// display [ebp-3Ch] (mov [ebp-3Ch], ebx = this at its start), its group [ebp-54h] (display + 30h, SetAnchor's this)
constexpr uint32_t kRetFramePoint = 0x0C;  // point - out
constexpr uint32_t kRetFrameDisplay = 0x4C; // [ebp-3Ch] - out
constexpr uint32_t kRetFrameGroup = 0x34;   // [ebp-54h] - out
constexpr uint32_t kRetGroup = 0x30;        // the display's reticle group
constexpr uint32_t kRetPlayer = 0x18;       // the display's player
constexpr uint32_t kRetAnchorX = 0xA0;      // the group's anchor (SetAnchor 0x00093850)
constexpr uint32_t kRetAnchorY = 0xA4;
constexpr DWORD kRetEveryMs = 15000;

typedef float*(__fastcall* RetProj_t)(void* cam, void* edx, float* out, const float* point);

struct RetSeen
{
    bool pending = false;     // projected since the last FrameWrap
    uintptr_t cam = 0;
    float pt[3] = {};
    float y = 0.0f;           // the game's answer (its reticle: anchor 0.5 + 0.5 y)
    LONG calls = 0;
    LONG inDraw = 0;          // ... while the game was drawing (FrameWrap's call of it)
    LONG twice = 0;           // a second one before FrameWrap came
    bool camLogged = false;
    uintptr_t group = 0;      // 21j-r7: players 3 / 4: the reticle group this projection was for (this frame's Update)
    bool prevOk = false;      // last frame's values (for the moves)
    LONG prevSeq = 0;
    float prevGame = 0.0f;
    float prevNow = 0.0f;
    // this window's sums - [0] not zoomed, [1] zoomed (shares of the view's height: 0.5 x the projection's y)
    int n[2] = {};
    int nMove[2] = {};
    double moveGame[2] = {};
    float moveGameMax[2] = {};
    double moveNow[2] = {};
    float moveNowMax[2] = {};
    double diff[2] = {};
    double diff2[2] = {};
    float diffMax[2] = {};
};

bool g_retWanted = true;          // [Splitscreen] ReticleScan
bool g_retOn = false;
bool g_retFixWanted = true;       // [Splitscreen] ReticleFixP34 (21j-r7)
bool g_retFixOn = false;
uintptr_t g_retVt = 0;            // ReticuleDisplay's vtable (va)
LONG g_retFixes = 0;              // players 3 / 4's reticle placed with this frame's camera
LONG g_retFixLeft = 0;            // ... not (the anchor was not the projection's)
LONG g_retNoGroup = 0;            // ... no display found in the projection's frame
RetProj_t g_retProj = nullptr;    // the call's old target (the projection's jump thunk)
RetSeen g_ret[4];
LONG g_retSeq = 0;                // FrameWraps seen
LONG g_retOther = 0;              // projections with a camera of none of the 4 players
DWORD g_retLast = 0;
int g_retLines = 0;

bool RetLog()
{
    if (g_retLines >= kRetMaxLines)
        return false;
    ++g_retLines;
    return true;
}

// player p's scope is up (his ScopeDisplay updated in the last second and shown)
bool RetZoomed(int p)
{
    const DWORD now = GetTickCount();
    for (int i = 0; i < kScDisplays; ++i)
        if (g_sc[i].d && g_sc[i].player == p && g_sc[i].shown && now - g_sc[i].lastTick < 1000)
            return true;
    return false;
}

// the reticle's projection (game thread, ReticuleDisplay::Update): the game's, then a note of what it was asked
float* __fastcall RetProjWrap(void* cam, void* edx, float* out, const float* point)
{
    float* const r = g_retProj(cam, edx, out, point);
    if (!g_retOn || !out || !point)
        return r;
    const uintptr_t c = reinterpret_cast<uintptr_t>(cam);
    int p = -1;
    if (c == reinterpret_cast<uintptr_t>(g_specCam[0]))
        p = 2;
    else if (c == reinterpret_cast<uintptr_t>(g_specCam[1]))
        p = 3;
    else
    {
        const uintptr_t cm = CameraManager();
        for (int i = 0; cm && i < 2 && p < 0; ++i)
            if (ReadU32(cm + kRetCamArray + 4u * static_cast<uint32_t>(i)) == c)
                p = i;
    }
    if (p < 0)
    {
        ++g_retOther;
        return r;
    }
    RetSeen& s = g_ret[p];
    ++s.calls;
    if (g_retDrawing)
        ++s.inDraw;
    if (s.pending)
        ++s.twice;
    s.pending = true;
    s.cam = c;
    memcpy(s.pt, point, sizeof(s.pt));
    s.y = out[1];
    // 21j-r7: players 3 / 4 - the display and its reticle group, from the caller's frame (checked three ways)
    s.group = 0;
    if (p >= 2 && g_retFixOn)
    {
        const uintptr_t o = reinterpret_cast<uintptr_t>(out);
        const uintptr_t disp = reinterpret_cast<uintptr_t>(point) == o + kRetFramePoint ? ReadU32(o + kRetFrameDisplay) : 0;
        if (disp && g_retVt && ReadU32(disp) == g_retVt && ReadU32(o + kRetFrameGroup) == disp + kRetGroup &&
            static_cast<int32_t>(ReadU32(disp + kRetPlayer)) == p)
            s.group = disp + kRetGroup;
        else
            ++g_retNoGroup;
    }
    if (!s.camLogged && g_retWanted && RetLog())
    {
        s.camLogged = true;
        Log("[reticle] %lu ms: player %d's reticle is placed with camera 0x%08X (%s)%s; the point %.1f %.1f %.1f, its height %.4f",
            ScanMsSinceAttach(), p + 1, static_cast<unsigned>(c), p >= 2 ? "BF1GameExt's camera of his view" : "the camera manager's",
            g_retDrawing ? " - while the game draws (after FrameWrap)" : "", point[0], point[1], point[2], 0.5f * out[1]);
    }
    return r;
}

void RetReport()
{
    const LONG other = g_retOther;
    for (int p = 0; p < 4; ++p)
    {
        RetSeen& s = g_ret[p];
        if (!s.n[0] && !s.n[1])
            continue;
        char part[2][220] = {};
        for (int z = 0; z < 2; ++z)
        {
            const int n = s.n[z];
            const int nm = s.nMove[z];
            if (!n)
            {
                snprintf(part[z], sizeof(part[z]), "none");
                continue;
            }
            const double mean = s.diff[z] / n;
            const double var = s.diff2[z] / n - mean * mean;
            snprintf(part[z], sizeof(part[z]),
                     "%d frames: the game's height moved %.4f a frame (most %.4f), with this frame's camera it would move %.4f (most "
                     "%.4f); the game's minus this frame's camera's %+.4f (sd %.4f, most %.4f)",
                     n, nm ? s.moveGame[z] / nm : 0.0, s.moveGameMax[z], nm ? s.moveNow[z] / nm : 0.0, s.moveNowMax[z], mean,
                     var > 0.0 ? static_cast<double>(sqrtf(static_cast<float>(var))) : 0.0, s.diffMax[z]);
        }
        if (g_retWanted && RetLog())
            Log("[reticle] %lu ms: player %d's reticle, the last %lu s (shares of his view's height) - ZOOMED %s | not zoomed %s | "
                "projections %ld (%ld while the game draws, %ld twice in one frame), with other cameras %ld%s",
                ScanMsSinceAttach(), p + 1, static_cast<unsigned long>(kRetEveryMs / 1000), part[1], part[0], static_cast<long>(s.calls),
                static_cast<long>(s.inDraw), static_cast<long>(s.twice), static_cast<long>(other),
                (p >= 2 && g_retFixOn) ? " (21j-r7: placed with this frame's camera - the line below)" : "");
        s.calls = 0; // (each line counts its own window)
        s.inDraw = 0;
        s.twice = 0;
        for (int z = 0; z < 2; ++z)
        {
            s.n[z] = 0;
            s.nMove[z] = 0;
            s.moveGame[z] = 0.0;
            s.moveGameMax[z] = 0.0f;
            s.moveNow[z] = 0.0;
            s.moveNowMax[z] = 0.0f;
            s.diff[z] = 0.0;
            s.diff2[z] = 0.0;
            s.diffMax[z] = 0.0f;
        }
    }
    g_retOther -= other;
    if (g_retFixOn && (g_retFixes || g_retFixLeft || g_retNoGroup) && RetLog())
        Log("[reticle] %lu ms: 21j-r7: players 3 / 4's reticle placed with this frame's camera %ld time(s) in the last %lu s; left as the "
            "game set it %ld (its anchor was not the projection's), no display found %ld",
            ScanMsSinceAttach(), static_cast<long>(g_retFixes), static_cast<unsigned long>(kRetEveryMs / 1000),
            static_cast<long>(g_retFixLeft), static_cast<long>(g_retNoGroup));
    g_retFixes = 0;
    g_retFixLeft = 0;
    g_retNoGroup = 0;
}

// FrameWrap, after players 3 / 4's cameras are set (game thread): each point the game projected this frame again, with
// the same camera as it is now
void RetFrame()
{
    if (!g_retOn)
        return;
    ++g_retSeq;
    const uintptr_t cm = CameraManager();
    for (int p = 0; p < 4; ++p)
    {
        RetSeen& s = g_ret[p];
        if (!s.pending)
            continue;
        s.pending = false;
        // players 1 / 2: still the camera manager's camera, and the one his view draws with (a new match / a teardown: not
        // compared)
        if (p < 2 && (!cm || ReadU32(cm + kRetCamArray + 4u * static_cast<uint32_t>(p)) != s.cam ||
                      RendCamera(static_cast<int>(ReadU32(cm + kCamViews + 4u * static_cast<uint32_t>(p)))) != s.cam))
        {
            s.prevOk = false;
            continue;
        }
        float o[4] = {};
        g_retProj(reinterpret_cast<void*>(s.cam), nullptr, o, s.pt);
        const float yg = 0.5f * s.y;
        const float yn = 0.5f * o[1];
        const uintptr_t group = s.group;
        s.group = 0;
        if (!(fabsf(yg) < 10.0f) || !(fabsf(yn) < 10.0f)) // (behind the camera, or nonsense)
        {
            s.prevOk = false;
            continue;
        }
        // 21j-r7 ([Splitscreen] ReticleFixP34=1): players 3 / 4's reticle at the height this frame's camera gives (the
        // HUD's roots are drawn after FrameWrap, in the view loop) - only while its anchor is still what the game's
        // Update set from the projection (0.5, 0.5 + 0.5 y)
        if (group && g_retFixOn && ReadU32(group - kRetGroup) == g_retVt)
        {
            float* const a = reinterpret_cast<float*>(group + kRetAnchorX);
            float* const b = reinterpret_cast<float*>(group + kRetAnchorY);
            if (fabsf(*a - 0.5f) < 1e-5f && fabsf(*b - (0.5f + yg)) < 1e-5f)
            {
                *b = 0.5f + yn;
                ++g_retFixes;
            }
            else
                ++g_retFixLeft;
        }
        const int z = RetZoomed(p) ? 1 : 0;
        ++s.n[z];
        const float d = yg - yn;
        s.diff[z] += d;
        s.diff2[z] += static_cast<double>(d) * d;
        if (fabsf(d) > s.diffMax[z])
            s.diffMax[z] = fabsf(d);
        if (s.prevOk && s.prevSeq == g_retSeq - 1)
        {
            const float mg = fabsf(yg - s.prevGame);
            const float mn = fabsf(yn - s.prevNow);
            ++s.nMove[z];
            s.moveGame[z] += mg;
            s.moveNow[z] += mn;
            if (mg > s.moveGameMax[z])
                s.moveGameMax[z] = mg;
            if (mn > s.moveNowMax[z])
                s.moveNowMax[z] = mn;
        }
        s.prevOk = true;
        s.prevSeq = g_retSeq;
        s.prevGame = yg;
        s.prevNow = yn;
    }
    const DWORD now = GetTickCount();
    if (!g_retLast)
        g_retLast = now;
    if (now - g_retLast >= kRetEveryMs)
    {
        g_retLast = now;
        RetReport();
    }
}

// a call's target moved to fn (its rel32); false if it is not a call or could not be written
bool RetCallTo(uint32_t call, const void* fn)
{
    uint8_t* const p = reinterpret_cast<uint8_t*>(Rva(call));
    if (*p != 0xE8)
        return false;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(fn) - reinterpret_cast<uintptr_t>(p + 5));
    DWORD old = 0;
    if (!VirtualProtect(p + 1, 4, PAGE_EXECUTE_READWRITE, &old))
        return false;
    memcpy(p + 1, &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(p + 1, 4, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), p, 5);
    int32_t back = 0;
    memcpy(&back, p + 1, 4);
    return back == rel;
}

void InstallReticleScan()
{
    if (!g_retWanted && !g_retFixWanted)
    {
        Log("[reticle] 21j-r7: [Splitscreen] ReticleScan=0, ReticleFixP34=0 - nothing installed");
        return;
    }
    const uint32_t ctor = g_h34[kRetHud].ctor;
    const bool ctorOk = ctor && MatchPatternAt(ctor, kRetCtorHead);
    const uint32_t vt = ctorOk ? OperandRva(ctor + kRetCtorVt) : 0;
    char name[64] = {};
    if (vt)
        ScVtName(static_cast<uint32_t>(Rva(vt)), name, sizeof(name));
    const uint32_t update = vt ? ScSlotFn(Rva(vt), kRetSlotUpdate) : 0;
    const bool updateOk = update && MatchPatternAt(update, kRetUpdateBody);
    uint32_t site = 0;
    int nSite = 0;
    for (uint32_t i = 0; updateOk && i < kRetSearch; ++i)
        if (MatchPatternAt(update + i, kRetProjSite))
        {
            ++nSite;
            site = update + i;
        }
    const uint32_t call = nSite == 1 ? site + kRetProjCall : 0;
    const uint32_t thunk = call ? RelTarget(call) : 0;
    const uint32_t proj = call ? CallTarget(call) : 0;
    const bool projOk = proj && MatchPatternAt(proj, kRetProjBody) && InImage(Rva(thunk));
    Log("[reticle] 21j-r6: THE RETICLE SCAN (log only): ReticuleDisplay's constructor rva 0x%08X %s, its vtable rva 0x%08X (%s), Update "
        "(slot %u) rva 0x%08X %s; the projection's call at Update +%Xh (%d hit(s)) -> rva 0x%08X (thunk rva 0x%08X) %s",
        ctor, ctorOk ? "OK" : "does NOT match", vt, name[0] ? name : "no RTTI name", kRetSlotUpdate, update,
        updateOk ? "OK" : "does NOT match", site ? site - update : 0u, nSite, proj, thunk, projOk ? "OK" : "does NOT match");
    if (!(ctorOk && updateOk && nSite == 1 && projOk))
    {
        Log("[reticle] 21j-r6: NOT in (something above did not check out)");
        return;
    }
    g_retProj = reinterpret_cast<RetProj_t>(Rva(thunk));
    g_retVt = Rva(vt);
    g_retFixOn = g_retFixWanted; // 21j-r7
    g_retOn = true; // (before the call moves: the wrapper finds everything set)
    if (!RetCallTo(call, reinterpret_cast<const void*>(&RetProjWrap)))
    {
        g_retOn = false;
        g_retFixOn = false;
        Log("[reticle] 21j-r6: the projection's call could not be written - NOT in");
        return;
    }
    if (g_retWanted)
        Log("[reticle] 21j-r6: ON - each player's reticle projection noted; in FrameWrap (players 3 / 4's cameras set) projected again "
            "with the camera as it is then; a summary every %lu s ([reticle] lines, %d at most)",
            static_cast<unsigned long>(kRetEveryMs / 1000), kRetMaxLines);
    else
        Log("[reticle] 21j-r6: [Splitscreen] ReticleScan=0 - no summaries (the fix's counts only)");
    Log("[reticle] 21j-r7: [Splitscreen] ReticleFixP34=%d - %s", g_retFixWanted ? 1 : 0,
        g_retFixOn ? "PLAYERS 3 / 4'S RETICLE PLACED WITH THIS FRAME'S CAMERA (in FrameWrap, after their cameras are set - as players 1 / "
                     "2's: no bob)"
                   : "off - their reticle as the game places it (last frame's camera)");
}

// (3) Still death cameras: DeathCamera::Update (GOG 1.3 0x0013BA70, vtable slot 1: its age +68h += dt; the camera
// follows its target, and once the age passes 1.5 s (0x008382E0) it turns to the killer Setup stored (+6Ch) - "it
// followed a player") is skipped for players 2 - 4's death cameras (camera 1's [cm + 44h + 4], and players 3 / 4's) once
// its age reaches DeathCamStillMs (1500 = right before that turn): the picture holds. Player 1's as the game has it.
// 20g-r10: off by default (DeathCamStill=0) - players 2 - 4 see their killer as player 1 does.
// 4-player only.
const char* const kPatDeathUpdate = "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC F3 0F 10 43 08 81 EC C8 00 00 "
                                    "00 56 8B F1 57 F3 0F 58 46 68 8B 4E 0C F3 0F 11 46 68";
constexpr uint32_t kDeathAge = 0x68;
constexpr uint32_t kCamDeaths = 0x44;   // CameraManager: DeathCamera [2]
bool g_stillWanted = false;             // [Splitscreen] DeathCamStill (20g-r10: 0 by default - they see their killer, as player 1)
float g_stillAfter = 1.5f;              // [Splitscreen] DeathCamStillMs / 1000
ChaseUpdate_t g_origDeathUpdate = nullptr;
volatile LONG g_stillFrames = 0;

void __fastcall DeathUpdateWrap(void* self, void* edx, float dt)
{
    (void)edx;
    if (g_stillWanted && g_playersOn && g_players >= 3)
    {
        const uintptr_t d = reinterpret_cast<uintptr_t>(self);
        const uintptr_t cm = CameraManager();
        const bool ours = self == g_death[0] || self == g_death[1];
        const bool p2 = cm && d == ReadU32(cm + kCamDeaths + 4);
        if ((ours || p2) && ReadF32(d + kDeathAge) >= g_stillAfter)
        {
            InterlockedIncrement(&g_stillFrames);
            return; // the picture holds (Apply keeps the matrix it has)
        }
    }
    g_origDeathUpdate(self, nullptr, dt);
}

void InstallDeathStill()
{
    if (!g_stillWanted)
    {
        Log("[players] 20g-r10: [Splitscreen] DeathCamStill=0 - players 2 - 4's death cameras as the game has them (as player 1's: "
            "they turn to the killer)");
        return;
    }
    uintptr_t orig = 0;
    if (!ScopeHookOne("DeathCamera update", kPatDeathUpdate, reinterpret_cast<const void*>(&DeathUpdateWrap), &orig))
    {
        Log("[players] 20g-r7: still death cameras NOT installed");
        return;
    }
    g_origDeathUpdate = reinterpret_cast<ChaseUpdate_t>(orig);
    Log("[players] 20g-r7: still death cameras ON - players 2 - 4's hold their picture %.1f s after the death (player 1's as "
        "the game has it)", static_cast<double>(g_stillAfter));
}

// (4) [fx34] DIAGNOSTIC: the particle emitters' draw (ParticleEmitterObject's renderable, GOG 1.3 0x001DC420, vtable slot
// 3) counted per renderer view - "player 3 lost the explosions" (views 3 / 4 against 1 / 2).
const char* const kPatEmitterDraw =
    "55 8B EC 83 EC 30 53 8B D9 83 7B 2C 00 0F 84 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 85 ?? ?? ?? ?? 56 38 43 5B";
typedef void(__fastcall* EmitterDraw_t)(void* self, void* edx, uint32_t a, uint32_t b, uint32_t c);
EmitterDraw_t g_origEmitterDraw = nullptr;
volatile LONG g_fxDraws[5] = {};        // views 0..3, other

void __fastcall EmitterDrawWrap(void* self, void* edx, uint32_t a, uint32_t b, uint32_t c)
{
    (void)edx;
    const uint32_t view = g_rendCams ? ReadU32(g_rendCams + 0x10) : 4;
    InterlockedIncrement(&g_fxDraws[view < 4 ? view : 4]);
    g_origEmitterDraw(self, nullptr, a, b, c);
}

void InstallFxDiag()
{
    uintptr_t orig = 0;
    if (ScopeHookOne("particle emitter draw (counted per view, log only)", kPatEmitterDraw,
                     reinterpret_cast<const void*>(&EmitterDrawWrap), &orig))
        g_origEmitterDraw = reinterpret_cast<EmitterDraw_t>(orig);
    Log("[fx34] particle emitter draws per view: %s", g_origEmitterDraw ? "counted ([fx34] heartbeat every 30 s)" : "NOT counted");
}

// ---------------------------------------------------------------------------------------------------------
// 20g-r9: CAMERA SHOTS FOR PLAYERS 2 - 4, COLLISION AROUND PLAYERS 3 AND 4 (see the top of this file)
// 20g-r10: the shot comes with his spawn screen (as player 1's: the game's death camera - it turns to the killer at
// 1.5 s - until his respawn timer runs out at 4 s, then his spawn screen and the shot on the same frame); player 2's
// view set in the frame hook (whichever of his cameras is current), not through the death camera's apply
// ---------------------------------------------------------------------------------------------------------
// CameraManager::RandomCameraShot(camera) / SetCameraShot(camera, index): thiscall, both through one jump thunk; each
// places camera's chase camera ([cm + 34h + camera * 4]) on a shot (its matrix +10h)
const char* const kPatShotRandom =
    "55 8B EC 53 8B D9 83 BB 80 00 00 00 00 7E 3F 57 E8 ?? ?? ?? ?? 33 D2 8B CB F7 B3 80 00 00 00";
const char* const kPatShotSet = "55 8B EC 53 8B D9 8B 83 80 00 00 00 85 C0 7E 42 57 48 50 6A 00 FF 75 0C E8";
constexpr uint32_t kCamChases = 0x34;     // CameraManager: ChaseCamera [2]
constexpr uint32_t kCharRespawn = 0xC0;   // character: his respawn timer (float s; <= 0: the game opens player 1's spawn screen)
typedef void(__fastcall* ShotRandom_t)(void* cm, void* edx, int camera);
typedef void(__fastcall* ShotSet_t)(void* cm, void* edx, int camera, int index);
ShotRandom_t g_origShotRandom = nullptr;
ShotSet_t g_origShotSet = nullptr;
bool g_shotWanted = true;               // [Splitscreen] CamShotP234
bool g_shotOk = false;
alignas(16) float g_p1Shot[16] = {};    // player 1's last camera shot (his chase camera's matrix right after it was placed)
volatile LONG g_p1ShotValid = 0;        // this match
volatile LONG g_shotCaptures = 0;
volatile LONG g_shotFrames[3] = {};     // players 2 / 3 / 4
bool g_shotOn[3] = {};
DWORD g_shotNoUnitAt[3] = {};           // when he lost his soldier (the respawn timer rule, without our screens)
int g_shotLines = 0;

bool ShotBudget()
{
    return g_shotLines++ < 60;
}

void ShotCapture(void* cm, int camera, const char* how)
{
    if (camera != 0 || !cm || reinterpret_cast<uintptr_t>(cm) != CameraManager())
        return;
    const uintptr_t chase = ReadU32(reinterpret_cast<uintptr_t>(cm) + kCamChases);
    float m[16];
    if (!chase || !ReadMem(chase + kCamObjMatrix, m, sizeof(m)))
        return;
    memcpy(g_p1Shot, m, sizeof(m));
    InterlockedExchange(&g_p1ShotValid, 1);
    InterlockedIncrement(&g_shotCaptures);
    if (ShotBudget())
        Log("[shot] %lu ms: player 1's camera shot (%s): at %.1f %.1f %.1f - players 2 - 4 show it on their spawn screens",
            ScanMsSinceAttach(), how, static_cast<double>(m[12]), static_cast<double>(m[13]), static_cast<double>(m[14]));
}

void __fastcall ShotRandomWrap(void* cm, void* edx, int camera)
{
    g_origShotRandom(cm, edx, camera);
    ShotCapture(cm, camera, "a random one, his spawn screen");
}

void __fastcall ShotSetWrap(void* cm, void* edx, int camera, int index)
{
    g_origShotSet(cm, edx, camera, index);
    ShotCapture(cm, camera, "the match start");
}

// player p (1 = player 2, 2 / 3 = players 3 / 4) shows player 1's shot now: 4-player, a shot this match, he has a
// character without a unit, and his spawn screens are up (split_spawn_menus.cpp: from the frame his respawn timer ran out -
// before that the game's death camera, which turns to his killer). Without our screens for him: his respawn timer has
// run out (the moment the game opens player 1's spawn screen).
bool ShotWanted(int p)
{
    if (!g_shotOk || !g_p1ShotValid || !g_playersOn || g_players < 3 || p < 1 || p > 3)
        return false;
    const uintptr_t ch = LocalCharacter(p);
    if (!ch || ReadU32(ch + 0xB4))
    {
        g_shotNoUnitAt[p - 1] = 0;
        return false;
    }
    const DWORD now = GetTickCount();
    if (!g_shotNoUnitAt[p - 1])
        g_shotNoUnitAt[p - 1] = now ? now : 1;
    const int up = P2SpawnScreensUp(p);
    if (up >= 0)
        return up == 1;
    if (now - g_shotNoUnitAt[p - 1] < 250) // (the game sets his respawn timer a frame or two after the unit went)
        return false;
    const float t = ReadF32(ch + kCharRespawn);
    return !(t == t && t > 0.0f && t < 120.0f);
}

void ShotLogChange(int p, bool on)
{
    if (on == g_shotOn[p - 1])
        return;
    g_shotOn[p - 1] = on;
    if (ShotBudget())
    {
        const uintptr_t ch = LocalCharacter(p);
        Log("[shot] %lu ms: player %d's view %s", ScanMsSinceAttach(), p + 1,
            on ? (P2SpawnScreensUp(p) == 1 ? "is on player 1's camera shot (his spawn screen is up)"
                                           : "is on player 1's camera shot (his respawn timer ran out)")
               : (ch && ReadU32(ch + 0xB4) ? "is his own again (he has a soldier)" : "is his own (his death camera)"));
    }
}

// player 2's view: camera 1's render camera (whichever of his cameras the camera manager applied this frame - the
// frame hook runs after it, right before the views are drawn)
void Shot2Frame(uintptr_t cm)
{
    if (!g_shotOk || !cm)
        return;
    const bool on = ShotWanted(1);
    ShotLogChange(1, on);
    const uintptr_t cam = on ? ReadU32(cm + kCamCameras + 4) : 0;
    if (cam)
    {
        g_camMatrix(reinterpret_cast<void*>(cam), nullptr, g_p1Shot);
        InterlockedIncrement(&g_shotFrames[0]);
    }
}

// players 3 / 4: after their chase / death camera was applied to view 3 / 4 (FrameWrap)
void Shot34Frame(int k)
{
    if (!g_shotOk || k < 0 || k > 1)
        return;
    const bool on = ShotWanted(k + 2);
    ShotLogChange(k + 2, on);
    if (on)
    {
        g_camMatrix(g_specCam[k], nullptr, g_p1Shot);
        InterlockedIncrement(&g_shotFrames[k + 1]);
    }
}

// 21j-r15: THE END OF A MATCH - PLAYERS 3 / 4'S VIEWS AS PLAYERS 1 / 2'S ([Splitscreen] EndShotP34). When the game shows VICTORY
// / DEFEAT it puts players 1 and 2's cameras on its end shots (the level's camera shots); players 3 / 4's views stayed their own
// - the video after r14: player 3, alive at the end, still on his soldier under his VICTORY. From that frame until the next match (split_spawn_menus.cpp's
// P2SpawnMatchEnded) view 3 shows player 1's camera and view 4 player 2's (camera 0 / 1's matrix as the camera manager applied
// it this frame), after Shot34Frame.
bool g_endShotWanted = true;   // [Splitscreen] EndShotP34
bool g_endShotOn[2] = {};
volatile LONG g_endShotFrames = 0;

void EndShot34Frame(int k, uintptr_t mgr)
{
    if (!g_endShotWanted || !g_camMatrix || !mgr || k < 0 || k > 1 || !g_playersOn || static_cast<int>(g_players) < k + 3)
        return;
    const bool on = P2SpawnMatchEnded();
    const uintptr_t cam = on ? ReadU32(mgr + kCamCameras + static_cast<uintptr_t>(k) * 4) : 0;
    float m[16];
    const bool got = cam && ReadMem(cam + kCamMatrix, m, sizeof(m));
    if (got != g_endShotOn[k])
    {
        g_endShotOn[k] = got;
        if (ShotBudget())
            Log("[shot] %lu ms: 21j-r15: player %d's view %s", ScanMsSinceAttach(), k + 3,
                got ? (k == 0 ? "shows player 1's camera (the match is over: the game's end shot, as players 1 / 2's views)"
                              : "shows player 2's camera (the match is over: the game's end shot, as players 1 / 2's views)")
                    : "is his own again");
    }
    if (!got)
        return;
    g_camMatrix(g_specCam[k], nullptr, m);
    InterlockedIncrement(&g_endShotFrames);
}

void InstallShots()
{
    if (!g_shotWanted)
    {
        Log("[shot] [Splitscreen] CamShotP234=0 - players 2 - 4's views after a death as before (their death cameras)");
        return;
    }
    if (!g_camMatrix)
    {
        Log("[shot] camera shots NOT installed (no render camera SetMatrix)");
        return;
    }
    if (!ScopeHookable("camera shot (random)", kPatShotRandom) || !ScopeHookable("camera shot (set)", kPatShotSet))
    {
        Log("[shot] camera shots NOT installed (a thunk is not hookable)");
        return;
    }
    // each original straight into its own pointer (MoveThunk writes it before the thunk moves: never called while null)
    static_assert(sizeof(ShotRandom_t) == sizeof(uintptr_t) && sizeof(ShotSet_t) == sizeof(uintptr_t), "function pointers are 4 bytes");
    if (!ScopeHookOne("camera shot (random)", kPatShotRandom, reinterpret_cast<const void*>(&ShotRandomWrap),
                      reinterpret_cast<uintptr_t*>(&g_origShotRandom)))
    {
        Log("[shot] camera shots NOT installed");
        return;
    }
    if (!ScopeHookOne("camera shot (set)", kPatShotSet, reinterpret_cast<const void*>(&ShotSetWrap),
                      reinterpret_cast<uintptr_t*>(&g_origShotSet)))
        Log("[shot] camera shots: SetCameraShot NOT hooked - player 1's shot is caught from his random shots only");
    g_shotOk = true;
    Log("[shot] 20g-r10: camera shots ON - players 2 - 4: the game's death camera (it turns to the killer), then player 1's "
        "camera shot from the frame their spawn screen comes up (their respawn timer, as player 1's), and before their first "
        "spawn");
}

// The collision manager's two "near a camera" loops (GOG 1.3 0x000A0A54 in 0x000A0930, 0x000A041C in 0x000A01E0): the
// difference camera - object, then its squared length (0x0005F910: push ebp; mov ebp,esp; pop ebp; jmp <dot>) against
// 3600 - the object's position is right after the difference in both frames ([ebp-30h] / [ebp-24h], [ebp-3Ch] /
// [ebp-30h])
const char* const kPatCollNear1 = "8D 45 DC 50 8D 45 AC 57 50 E8 ?? ?? ?? ?? F3 0F 7E 00 8B 40 08 89 45 D8 8D 45 D0 50 50 66 0F D6 45 "
                                  "D0 E8 ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? 83 C4 14";
const char* const kPatCollNear2 = "8D 45 D0 50 8D 85 70 FF FF FF 56 50 E8 ?? ?? ?? ?? F3 0F 7E 00 8B 40 08 89 45 CC 8D 45 C4 50 50 66 "
                                  "0F D6 45 C4 E8 ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? 83 C4 14";
constexpr uint32_t kCollCall[2] = { 0x22, 0x25 };
constexpr uint32_t kCollThr[2] = { 0x2B, 0x2E };
const char* const kCollDotWrapBody = "55 8B EC 5D E9";
const char* const kCollDotBody = "55 8B EC 8B 4D 08 8B 45 0C D9 41 04 D8 48 04 D9 01 D8 08 DE C1 D9 41 08 D8 48 08 DE C1 5D C3";
bool g_collWanted = true;               // [Splitscreen] CollisionP34
bool g_collOn = false;
volatile LONG g_collCalls = 0;
volatile LONG g_collNearer = 0;

float __cdecl CollDistWrap(const float* a, const float* b)
{
    float d = a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; // the game's (the difference's squared length)
    if (a != b || !g_playersOn)
        return d;
    ++g_collCalls;
    const float* const obj = a + 3; // the object's position (byte-checked frames)
    for (int k = 0; k < 2 && k + 3 <= static_cast<int>(g_players); ++k)
    {
        if (!g_specMade[k] || !g_quarterMgr || g_specView[k] < 0) // only the cameras of quarters that are up (no stale places)
            continue;
        const float* const c = reinterpret_cast<const float*>(g_specCam[k] + kCamMatrix + 0x30);
        const float dx = c[0] - obj[0];
        const float dy = c[1] - obj[1];
        const float dz = c[2] - obj[2];
        const float e = dx * dx + dy * dy + dz * dz;
        if (e < d)
        {
            d = e;
            ++g_collNearer;
        }
    }
    return d;
}

void InstallCollision34()
{
    if (!g_collWanted)
    {
        Log("[coll34] [Splitscreen] CollisionP34=0 - objects collide only near players 1 / 2's cameras (as the game has it)");
        return;
    }
    uint32_t h[2][4] = {};
    const int n0 = FindPattern(kPatCollNear1, h[0], 4);
    const int n1 = FindPattern(kPatCollNear2, h[1], 4);
    bool ok = n0 == 1 && n1 == 1;
    uint32_t dot[2] = {};
    for (int i = 0; ok && i < 2; ++i)
    {
        const uint32_t t = CallTarget(h[i][0] + kCollCall[i]);
        ok = MatchPatternAt(t, kCollDotWrapBody) && MatchPatternAt(CallTarget(t + 4), kCollDotBody);
        dot[i] = t;
    }
    ok = ok && dot[0] == dot[1] && CodeU32(h[0][0] + kCollThr[0]) == CodeU32(h[1][0] + kCollThr[1]);
    int patched = 0;
    for (int i = 0; ok && i < 2; ++i)
    {
        unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(h[i][0] + kCollCall[i]));
        DWORD old = 0;
        if (at[0] != 0xE8 || !VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old))
            break;
        const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&CollDistWrap) - (reinterpret_cast<uintptr_t>(at) + 5));
        memcpy(at + 1, &rel, 4);
        DWORD ignored = 0;
        VirtualProtect(at + 1, 4, old, &ignored);
        FlushInstructionCache(GetCurrentProcess(), at, 5);
        int32_t back = 0;
        memcpy(&back, at + 1, 4);
        if (back == rel)
            ++patched;
    }
    g_collOn = patched == 2;
    Log("[coll34] the collision manager's near-a-camera loops: %d / %d hit(s) (rva 0x%08X / 0x%08X), their squared distance rva "
        "0x%08X, the 3600 limit at va 0x%08X - %s",
        n0, n1, n0 == 1 ? h[0][0] : 0u, n1 == 1 ? h[1][0] : 0u, dot[0], ok ? CodeU32(h[0][0] + kCollThr[0]) : 0u,
        g_collOn ? "PATCHED (2 calls): objects near players 3 / 4's cameras collide too"
                 : (patched ? "only one call patched (the other did not check out)" : "NOT installed (as the game has it)"));
}

uintptr_t g_soldierChecked[2] = {};
bool g_soldierOk[2] = {};
int g_camLines = 0;

// View 3 / 4: behind and above player 3 / 4's soldier, from its world matrix (rows x, y, z, position at +1B0h).
bool PlayerCamera(int k, float* m)
{
    if (!g_playersOn || static_cast<int>(g_players) < k + 3)
        return false;
    const uintptr_t ch = LocalCharacter(k + 2);
    const uintptr_t unit = ch ? ReadU32(ch + 0xB4) : 0;
    if (!unit || unit <= 0x250)
        return false;
    const uintptr_t soldier = unit - 0x250;
    if (soldier != g_soldierChecked[k])
    {
        char cls[64];
        ClassName(soldier, cls, sizeof(cls));
        g_soldierChecked[k] = soldier;
        g_soldierOk[k] = strcmp(cls, "EntitySoldier") == 0;
        if (g_camLines < 40)
        {
            ++g_camLines;
            Log("[players] %lu ms: player %d's unit %08X -> %08X (%s)%s", ScanMsSinceAttach(), k + 3,
                static_cast<uint32_t>(unit), static_cast<uint32_t>(soldier), cls[0] ? cls : "no RTTI",
                g_soldierOk[k] ? " - his camera follows it" : " - not a soldier: view stays the spectator");
        }
    }
    if (!g_soldierOk[k])
        return false;
    float w[16];
    if (!ReadMem(soldier + 0x1B0, w, sizeof(w)))
        return false;
    // right, up, forward of the soldier; the camera looks a little down, from 3.6 m behind and 1.9 m up
    const float* x = w;
    const float* y = w + 4;
    const float* z = w + 8;
    const float* pos = w + 12;
    float f[3] = { z[0] - 0.25f * y[0], z[1] - 0.25f * y[1], z[2] - 0.25f * y[2] };
    float len = f[0] * f[0] + f[1] * f[1] + f[2] * f[2];
    if (len < 1e-6f)
        return false;
    // 1 / sqrt by two Newton steps from a rough start (no <cmath> needed for this)
    float inv = 1.0f / len;
    float r = 1.0f;
    for (int it = 0; it < 12; ++it)
        r = r * (1.5f - 0.5f * len * r * r);
    (void)inv;
    f[0] *= r;
    f[1] *= r;
    f[2] *= r;
    // up = forward x right ... kept simple: right from the soldier, up = right x forward (left-handed: f x x)
    float u[3] = { f[1] * x[2] - f[2] * x[1], f[2] * x[0] - f[0] * x[2], f[0] * x[1] - f[1] * x[0] };
    if (u[0] * y[0] + u[1] * y[1] + u[2] * y[2] < 0.0f)
    {
        u[0] = -u[0];
        u[1] = -u[1];
        u[2] = -u[2];
    }
    m[0] = x[0];
    m[1] = x[1];
    m[2] = x[2];
    m[3] = 0.0f;
    m[4] = u[0];
    m[5] = u[1];
    m[6] = u[2];
    m[7] = 0.0f;
    m[8] = f[0];
    m[9] = f[1];
    m[10] = f[2];
    m[11] = 0.0f;
    m[12] = pos[0] - 3.6f * z[0] + 1.9f * y[0];
    m[13] = pos[1] - 3.6f * z[1] + 1.9f * y[1];
    m[14] = pos[2] - 3.6f * z[2] + 1.9f * y[2];
    m[15] = 1.0f;
    return true;
}

// ---------------------------------------------------------------------------------------------------------
// 20g-r14: PLAYER 3'S HALF - HIS CHASE CAMERA'S SHAPE
// ---------------------------------------------------------------------------------------------------------
// The game's ChaseCamera Apply (GOG 1.3 0x0013AAD0, its vtable +8) ends by giving the camera it is applied to the shape
// of HUD root 0 - player 1's view - whichever camera that is (0x0013B02A - 0x0013B17E): ratio = the screen ratio (the
// float SetNumCameras reads) x root 0's h x the screen's w / (root 0's w x the screen's h), then SetFov(fov, ratio)
// (0x0028AB10: +98h = tan(fov / 2), +9Ch = that x ratio). In the 2-player game both halves have root 0's shape. In
// 3-player root 0 is a quarter and player 3's view the bottom half: every frame his camera got a quarter's shape - the
// half drew a quarter's picture twice as wide (r13's video: "too streched"). So right after his chase Apply his ratio
// is scaled by (his root's h / w) / (root 0's h / w): exactly what the game's chase gives the 2-player bottom half (root
// 0 is a half there), through the camera's own projection (0x0028AC50, its near / far / fov read back). The Apply's only
// early exit (no target, [chase+0Ch] = 0, +20h) skips its SetFov - nothing is scaled then. The death and free cameras'
// Applies (0x0013B340 / 0x0013B370) only set the zoom (+A0h): his half keeps its shape while he is down. Nothing else
// sets a RedCamera's shape in play (SetFov is only called by the chase Apply; the other frustum setter 0x0028AB70 by
// the "setviewrange" debug command, the shadow pass's own cameras and the HUD's 3D camera). A view with root 0's shape
// (4-player quarters) is left as the game gives it. Everything is byte-checked the first time a chase camera is made.
const char* const kChaseApplyHead = // its prologue and the early exit: cmp dword ptr [edi+0Ch], 0
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ?? ?? ?? ?? 56 57 8B F9 83 7F 0C 00";
// the shape: screen = (E8) the screen; (E8) its w; push 0; (E8 +2Dh) root(0)'s h ... (E8 +57h) the screen ratio; fmul;
// (E8) the screen's h; push 0; (E8 +88h) root(0)'s w; ... divss
const char* const kChaseShapeA =
    "E8 ?? ?? ?? ?? 8B F0 8B C8 E8 ?? ?? ?? ?? 6A 00 66 0F 6E C0 F3 0F E6 C0 C1 E8 1F F2 0F 58 04 C5 ?? ?? ?? ?? 66 0F "
    "5A C0 F3 0F 11 45 FC E8 ?? ?? ?? ?? 83 C4 04";
constexpr uint32_t kChaseShapeB = 0x35; // the rest of it, right after A
const char* const kChaseShapeBBody =
    "66 0F 6E C0 F3 0F E6 C0 C1 E8 1F F2 0F 58 04 C5 ?? ?? ?? ?? 66 0F 5A C0 F3 0F 59 45 FC F3 0F 11 45 FC E8 ?? ?? ?? "
    "?? D8 4D FC 8B CE D9 5D F4 E8 ?? ?? ?? ?? 6A 00 66 0F 6E C0 F3 0F E6 C0 C1 E8 1F F2 0F 58 04 C5 ?? ?? ?? ?? 66 0F "
    "5A C0 F3 0F 11 45 FC E8 ?? ?? ?? ?? F3 0F 10 4D F4 83 C4 04";
constexpr uint32_t kChaseShapeHCall = 0x2D;     // E8 root(i)'s h
constexpr uint32_t kChaseShapeRatioCall = 0x57; // E8 the screen ratio (fld [ratio]; ret)
constexpr uint32_t kChaseShapeWCall = 0x88;     // E8 root(i)'s w
// root(i)'s h / w: mov ecx, [i*4 + the root table]; jmp the root's getter
const char* const kRootGetterBody = "55 8B EC 8B 4D 08 8B 0C 8D ?? ?? ?? ?? 5D E9";
constexpr uint32_t kRootGetterTable = 9;
constexpr uint32_t kRootGetterJump = 0x0E;
const char* const kRootHBody = "8B 41 04 C3"; // mov eax, [ecx+4]; ret
const char* const kRootWBody = "8B 01 C3";    // mov eax, [ecx]; ret
// the Apply's end: add esp, 8; mov ecx, esi; fstp [esp]; call SetFov; pop edi; pop esi; ...; ret 8
const char* const kChaseSetFovSite = "83 C4 08 8B CE D9 1C 24 E8 ?? ?? ?? ?? 5F 5E 8B E5 5D 8B E3 5B C2 08 00";
constexpr uint32_t kChaseSetFovCall = 8;
const char* const kSetFovBody = "55 8B EC F3 0F 10 45 08 F3 0F 59 05 ?? ?? ?? ?? 56 51 8B F1 F3 0F 11 04 24 E8 ?? ?? ?? "
                                "?? D9 5D 08 F3 0F 10 45 08 83 C4 04 F3 0F 11 86 98 00 00 00 8B CE F3 0F 59 45 0C F3 0F "
                                "11 86 9C 00 00 00";
// the camera projection's (g_camParams, kPatCamParams) field writes: +90h near, +94h far, +98h tan(fov / 2), +9Ch x ratio
constexpr uint32_t kCamParamsFieldsAt = 0x1E;
const char* const kCamParamsFields = "F3 0F 10 45 08 83 C4 04 F3 0F 11 86 90 00 00 00 8B CE F3 0F 10 45 0C F3 0F 11 86 94 "
                                     "00 00 00 D9 5D 10 F3 0F 10 45 10 F3 0F 11 86 98 00 00 00 F3 0F 59 45 14 F3 0F 11 86 "
                                     "9C 00 00 00";
constexpr uint32_t kChaseApplyScan = 0x800; // the Apply is 6BEh long (GOG 1.3)
constexpr uint32_t kChaseTargetAt = 0x0C;   // ChaseCamera +0Ch: its target (SetTarget's cmp, kChaseSetTargetBody)
constexpr uint32_t kCamNear = 0x90;
constexpr uint32_t kCamFar = 0x94;
constexpr uint32_t kCamTanX = 0x98;
constexpr uint32_t kCamTanY = 0x9C;
constexpr int kShapeRoots = 4; // the game's 4 HUD roots

bool g_shapeChecked = false;
bool g_shapeOk = false;
uintptr_t g_shapeRoots = 0;  // the root table the Apply's getters read (va)
volatile LONG g_shapeFixes = 0;
int g_shapeLines = 0;
float g_shapeLastScale[2] = {};

uint32_t SplitFovCallTarget(uint32_t callAt); // 21j-r39: the Apply's SetFov call -> the game's SetFov (ours in between)

void ChaseShapeScan(uintptr_t apply)
{
    g_shapeChecked = true;
    uint32_t a = RvaOf(apply); // the vtable's entry: a jump thunk (GOG 1.3 0x0001235A -> 0x0013AAD0) - followed
    for (int hop = 0; hop < 8 && a && CodeByte(a) == 0xE9; ++hop)
        a = RelTarget(a);
    uint32_t block = 0;
    uint32_t site = 0;
    int nb = 0;
    int ns = 0;
    for (uint32_t i = 0; a && i < kChaseApplyScan; ++i)
    {
        if (CodeByte(a + i) == 0xE8 && MatchPatternAt(a + i, kChaseShapeA) &&
            MatchPatternAt(a + i + kChaseShapeB, kChaseShapeBBody))
        {
            if (!nb)
                block = a + i;
            ++nb;
        }
        if (CodeByte(a + i) == 0x83 && MatchPatternAt(a + i, kChaseSetFovSite))
        {
            if (!ns)
                site = a + i;
            ++ns;
        }
    }
    const bool headOk = a && MatchPatternAt(a, kChaseApplyHead);
    const uint32_t hGet = nb == 1 ? CallTarget(block + kChaseShapeHCall) : 0;
    const uint32_t wGet = nb == 1 ? CallTarget(block + kChaseShapeWCall) : 0;
    const uint32_t kGet = nb == 1 ? CallTarget(block + kChaseShapeRatioCall) : 0;
    const uint32_t table = hGet ? CodeU32(hGet + kRootGetterTable) : 0;
    const bool getOk = hGet && wGet && MatchPatternAt(hGet, kRootGetterBody) && MatchPatternAt(wGet, kRootGetterBody) &&
                       CodeU32(wGet + kRootGetterTable) == table && InImage(table) &&
                       MatchPatternAt(CallTarget(hGet + kRootGetterJump), kRootHBody) &&
                       MatchPatternAt(CallTarget(wGet + kRootGetterJump), kRootWBody);
    const bool ratioSame = kGet && MatchPatternAt(kGet, kFldImmRet) && g_ratioVa && CodeU32(kGet + 2) == g_ratioVa;
    const uint32_t setFov = ns == 1 && site > block ? SplitFovCallTarget(site + kChaseSetFovCall) : 0; // (21j-r39: ours followed)
    const bool fovOk = setFov && MatchPatternAt(setFov, kSetFovBody);
    const uint32_t params = g_camParams ? RvaOf(reinterpret_cast<uintptr_t>(g_camParams)) : 0;
    const bool paramsOk = params && MatchPatternAt(params + kCamParamsFieldsAt, kCamParamsFields);
    g_shapeOk = headOk && getOk && fovOk && paramsOk;
    g_shapeRoots = g_shapeOk ? table : 0;
    Log("[shape3] the chase camera's Apply rva 0x%08X: prologue + early exit %s; the shape from HUD root 0 at rva 0x%08X "
        "(%d hit(s)): root h / w getters 0x%08X / 0x%08X %s (root table rva 0x%08X), the screen ratio getter 0x%08X %s; "
        "SetFov at rva 0x%08X (%d hit(s)) -> 0x%08X %s; the camera projection's fields (rva 0x%08X) %s -> player 3's "
        "half %s",
        a, headOk ? "OK" : "NOT OK", block, nb, hGet, wGet, getOk ? "OK" : "NOT OK", RvaOf(table), kGet,
        ratioSame ? "OK (SetNumCameras' float)" : "(another float - not used)", site, ns, setFov,
        fovOk ? "OK" : "NOT OK", params, paramsOk ? "OK" : "NOT OK",
        g_shapeOk ? "gets its own root's shape after each chase Apply" : "keeps root 0's shape (NOT fixed)");
}

bool ShapeRoot(int index, float* w, float* h)
{
    const uintptr_t root = (g_shapeRoots && index >= 0 && index < kShapeRoots)
                               ? ReadU32(g_shapeRoots + 4u * static_cast<uint32_t>(index))
                               : 0;
    if (!root)
        return false;
    *w = static_cast<float>(static_cast<int32_t>(ReadU32(root)));
    *h = static_cast<float>(static_cast<int32_t>(ReadU32(root + 4)));
    return *w >= 64.0f && *h >= 64.0f && *w <= 8192.0f && *h <= 8192.0f;
}

float CamField(const void* cam, uint32_t off)
{
    float f = 0.0f;
    memcpy(&f, static_cast<const unsigned char*>(cam) + off, sizeof(f));
    return f;
}

float SfhP3Fov(int view, float tx, float ratio, float* f);           // 21j-r40: player 3's half's FOV (SplitFovP3)
void SfhP3Remember(void* cam, float base, float f, int view, float ratio); // 21j-r40: for the Apply's next read back

// Right after view 3 / 4's chase Apply (the Apply reached its SetFov): the shape of his own view's root.
void ChaseShapeFix(int k)
{
    const int v = g_specView[k];
    void* const cam = g_specCam[k];
    if (!g_shapeOk || !g_camParams || v < 0 || RendCamera(v) != reinterpret_cast<uintptr_t>(cam))
        return;
    float w0 = 0.0f;
    float h0 = 0.0f;
    float wv = 0.0f;
    float hv = 0.0f;
    if (!ShapeRoot(0, &w0, &h0) || !ShapeRoot(v, &wv, &hv))
        return;
    const float scale = (hv * w0) / (wv * h0);
    if (fabsf(scale - 1.0f) < 0.001f)
        return; // his view has root 0's shape (the 4-player quarters): as the game gave it
    const float tx = CamField(cam, kCamTanX);
    const float ty = CamField(cam, kCamTanY);
    const float nearZ = CamField(cam, kCamNear);
    const float farZ = CamField(cam, kCamFar);
    if (!(tx > 0.01f && tx < 50.0f && ty > 0.0f && ty < 50.0f && nearZ > 0.0f && farZ > nearZ))
    {
        if (g_shapeLines < 40)
        {
            ++g_shapeLines;
            Log("[shape3] player %d's camera: fields not as expected (tan %.4f / %.4f, near %.3f, far %.3f) - not changed",
                k + 3, static_cast<double>(tx), static_cast<double>(ty), static_cast<double>(nearZ),
                static_cast<double>(farZ));
        }
        return;
    }
    const float given = ty / tx;
    const float ratio = given * scale;
    // 21j-r40: player 3's half also gets BF2's FOV (SplitFovP3) - the game's FOV widened for his half's own shape
    float wide = 1.0f;
    const float fov = SfhP3Fov(v, tx, ratio, &wide);
    g_camParams(cam, nullptr, nearZ, farZ, fov, ratio);
    SfhP3Remember(cam, 2.0f * atanf(tx), wide, v, ratio);
    const LONG n = InterlockedIncrement(&g_shapeFixes);
    if ((n <= 3 || fabsf(scale - g_shapeLastScale[k]) > 0.001f) && g_shapeLines < 40)
    {
        ++g_shapeLines;
        Log("[shape3] %lu ms: player %d's camera (view %d): the chase gave ratio %.4f (root 0 %.0f x %.0f); his root %.0f x "
            "%.0f -> x %.4f = %.4f (fov %.2f deg, tan %.4f / %.4f now; fix %ld)",
            ScanMsSinceAttach(), k + 3, v, static_cast<double>(given), static_cast<double>(w0), static_cast<double>(h0),
            static_cast<double>(wv), static_cast<double>(hv), static_cast<double>(scale), static_cast<double>(ratio),
            static_cast<double>(2.0f * atanf(tx) * 57.29578f), static_cast<double>(CamField(cam, kCamTanX)),
            static_cast<double>(CamField(cam, kCamTanY)), static_cast<long>(n));
    }
    g_shapeLastScale[k] = scale;
}

// 20e-r2: view 3 / 4 through player 3 / 4's own chase camera (game thread, right before the views are drawn).
bool PlayerChase(int k, float dt)
{
    if (!g_chaseOk || !g_playersOn || static_cast<int>(g_players) < k + 3)
        return false;
    const uintptr_t mgr = CameraManager();
    if (!mgr)
        return false;
    void* const chase = g_chase[k];
    if (!g_chaseMade[k])
    {
        g_chaseCtor(chase, nullptr, mgr, k + 2);
        g_chaseMade[k] = true;
        char cls[64];
        ClassName(reinterpret_cast<uintptr_t>(chase), cls, sizeof(cls));
        const uintptr_t vt = ReadU32(reinterpret_cast<uintptr_t>(chase));
        if (strcmp(cls, "ChaseCamera") != 0 || !InImage(vt))
        {
            Log("[chase34] player %d's chase camera is a \"%s\" (vtable %08X), not a ChaseCamera - chase cameras OFF "
                "(the 20d follow camera)", k + 3, cls, static_cast<uint32_t>(vt));
            g_chaseOk = false;
            return false;
        }
        g_chaseUpdate = reinterpret_cast<ChaseUpdate_t>(ReadU32(vt + 4));
        g_chaseApply = reinterpret_cast<ChaseApply_t>(ReadU32(vt + 8));
        Log("[chase34] %lu ms: player %d's chase camera made (the game's ChaseCamera, index %d): Update rva 0x%08X, "
            "Apply rva 0x%08X", ScanMsSinceAttach(), k + 3, k + 2, RvaOf(reinterpret_cast<uintptr_t>(g_chaseUpdate)),
            RvaOf(reinterpret_cast<uintptr_t>(g_chaseApply)));
        if (!g_shapeChecked)
            ChaseShapeScan(reinterpret_cast<uintptr_t>(g_chaseApply)); // 20g-r14
    }
    uintptr_t want = g_chaseWanted[k];
    {
        // safety: no unit on his character (dead / not spawned) -> no target, whatever the game last gave
        const uintptr_t ch = LocalCharacter(k + 2);
        if (!ch || !ReadU32(ch + 0xB4))
            want = 0;
    }
    if (want != g_chaseTarget[k])
    {
        g_chaseSetTarget(chase, nullptr, want);
        g_chaseTarget[k] = want;
        g_wallDist[k] = 1.0f; // 20g-r7: as the camera manager's SetChaseTarget does for its own
        if (g_chaseLines < 60)
        {
            ++g_chaseLines;
            const uintptr_t ch = LocalCharacter(k + 2);
            const uintptr_t unit = ch ? ReadU32(ch + 0xB4) : 0;
            char cls[64] = "";
            if (want)
                ClassName(want, cls, sizeof(cls));
            Log("[chase34] %lu ms: player %d's chase camera target -> %08X (%s; his character's unit %08X%s)",
                ScanMsSinceAttach(), k + 3, static_cast<uint32_t>(want), want ? (cls[0] ? cls : "no RTTI") : "none",
                static_cast<uint32_t>(unit), want && unit == want ? ", the same" : "");
        }
    }
    // 20g-r6: his death camera while he is down (started by the game through the guard), until he has a soldier again
    if (g_deathOn[k])
    {
        if (want)
            DeathStop34(k, "he has a soldier again");
        else if (g_deathMgr[k] != mgr)
            DeathStop34(k, "another camera manager (a new match)");
        else if (g_deathUpdate && g_deathApply)
        {
            g_wallFor = k; // 20g-r7: his own wall distance
            g_deathUpdate(g_death[k], nullptr, dt);
            g_deathApply(g_death[k], nullptr, g_specCam[k], dt);
            g_wallFor = -1;
            InterlockedIncrement(&g_deathFrames);
            g_chaseEver[k] = true;
            return true;
        }
    }
    if (!want)
        return g_chaseEver[k]; // keeps its last picture once it had one
    g_chaseEver[k] = true;
    g_wallFor = k; // 20g-r7: his own wall distance (CamFadeWrap)
    g_chaseUpdate(chase, nullptr, dt);
    // 20g-r14: with a target the Apply reaches its SetFov (root 0's shape): then his own view's shape
    const bool shaped = ReadU32(reinterpret_cast<uintptr_t>(chase) + kChaseTargetAt) != 0;
    g_chaseApply(chase, nullptr, g_specCam[k], dt);
    if (shaped)
        ChaseShapeFix(k);
    g_wallFor = -1;
    InterlockedIncrement(&g_chaseFrames);
    return true;
}

// ---------------------------------------------------------------------------------------------------------
// 20g-r3: PLAYERS 3 AND 4'S HUD
// ---------------------------------------------------------------------------------------------------------
// Two more of each of the game's HUD display classes (one for player 3, one for player 4), made, set up, shown /
// hidden and deleted where the game does it for its own two (the [hud34] scan, 20g-r1 / r2, found and byte-checked
// each place):
//  - MADE: each class's create (one jump thunk, called once while a level loads) runs, then two more are made with
//    the game's own new and the class's constructor - the heap the game's two came from.
//  - SET UP: each class's per-camera Setup loop (one jump thunk, reached from the match start through the class's
//    reset loop) runs, then ours are reset and get Setup(2 / 3): their player (+18h) is 2 / 3 and their groups go
//    into the HUD root the camera manager's RootIndex(2 / 3) answers - the root of view 3 / 4. Setup puts them in the
//    game's task lists: the game updates and draws them every frame from then on, as its own. RootIndex, IsChase and
//    GetChase answer 2 / 3 here (their jump thunks): his view's root, his own chase camera (20e-r2).
//  - SHOWN / HIDDEN: the game hides a player's displays while a screen is up in his view with each class's ForPlayer
//    (viewport, on) loop over its 2 slots (GOG 1.3 0x00093780 ...); ours go through the same loops (ours in slot 0
//    for the call, the game's two put back right after) while player 3's / 4's spawn screen is up.
//  - DELETED: each class's destroy (the match teardown) deletes ours first (their own deleting destructor). The game
//    never destroys its CaptureDisplays (left to the level's heap) - ours are left the same way.
//  - The display getters (census) answer ours for players 3 / 4 once they are set up (lent player 1's / 2's before);
//    MapDisplay's "the level's map texture" loop (GOG 1.3 0x00167430, while the level loads) gives ours the texture.
// ScopeDisplay is NOT made: its show / hide write the player's slot of a 2-slot static (GOG 1.3 0x005EBCC0; the next
// word is a pointer of the game's) - a later phase.
typedef void(__cdecl* HudVoid_t)();
typedef uintptr_t(__cdecl* HudNew_t)(uint32_t size);
typedef uintptr_t(__fastcall* HudCtor_t)(uintptr_t self, void* edx);
typedef void(__fastcall* HudReset_t)(uintptr_t self, void* edx);
typedef void(__fastcall* HudSetup_t)(uintptr_t self, void* edx, int i);
typedef void(__fastcall* HudDelete_t)(uintptr_t self, void* edx, int flags);
typedef void(__cdecl* HudForPlayer_t)(int viewport, int on);
typedef void(__cdecl* HudMapTexAll_t)(const char* name);
typedef void(__fastcall* HudMapTex_t)(uintptr_t self, void* edx, const char* name);
typedef int(__fastcall* HudRootIndex_t)(void* cm, void* edx, int i);
typedef bool(__fastcall* HudIsChase_t)(void* cm, void* edx, int i);
typedef uintptr_t(__fastcall* HudGetChase_t)(void* cm, void* edx, int i);

// ForPlayer(viewport, on): for each of the class's 2 slots with +1Ch == viewport, its SetVisible(on) (and when off,
// its clear). GOG 1.3: Boot 0x00093780, Capture 0x000962B0, Map 0x001872A0, Reticule 0x00202710, StatIcon 0x0021BBB0,
// Status 0x0021C9D0, Team 0x0021FC60 (Scope has none).
const char* const kHudForPlayerFmt = "55 8B EC 53 8B 5D 0C 56 57 8B 7D 08 BE %s 8B 0E 85 C9 74 ?? 39 79 1C 75 ?? 53 E8";
// MapDisplay: the level's map texture given to both (GOG 1.3 0x00167430, cdecl (name)) through each one's
// SetMapTexture (0x00188740, "%s_map").
const char* const kHudMapTexAllFmt =
    "55 8B EC 56 57 8B 7D 08 BE %s 0F 1F 00 8B 0E 57 E8 ?? ?? ?? ?? 83 C6 04 81 FE %s 7C ED 5F 5E 5D C3";
constexpr uint32_t kHudMapTexAllCall = 0x13;
const char* const kHudMapTexBody = "55 8B EC 83 EC 40 8D 45 C0 56 FF 75 08 8B F1 68";
// 20g-r4: MapDisplay's objectives panel (GOG 1.3 0x001887A0, thiscall (x), one jump thunk, 2 calls: the display's
// own Update and a GetMapDisplay(i) caller) finds its player's console spawn point screen by name -
// "ifs_mapselect%i.SideBar.Objectives.Skin.ChunkC" with player + 1 - and reads the element it gets without checking
// it: ifs_mapselect3 / 4 do not exist, so for player 3's / 4's MapDisplay it read a null (r3's crash, Steam
// 0x002AF863, the moment player 3 was in the world). It does nothing for a MapDisplay of player 3 / 4 now.
const char* const kHudMapObjBody =
    "55 8B EC 81 EC 34 02 00 00 56 57 8B F9 89 7D F8 8B 47 18 40 50 8D 85 CC FD FF FF 68 ?? ?? ?? ?? 50 E8";
constexpr uint32_t kHudMapObjName = 0x1C; // its push of the name's format
const char* const kHudMapObjFormat = "ifs_mapselect%i.SideBar.Objectives.Skin.ChunkC";
typedef void(__fastcall* HudMapObj_t)(uintptr_t self, void* edx, uintptr_t a);
// 20g-r5: THE MINIMAP'S ICONS (command posts, soldiers, vehicles, markers). The game registers each object with both
// MapDisplays at once through four static functions (cdecl, each one jump thunk): a slot in a static table, then for
// each of the 2 displays its element for that slot set up (texture, hidden) or cleared. GOG 1.3: marker add 0x00186A00
// (obj; table 0x004BD5B0, 16 x 10h), unit icon add 0x00186C00 (obj, flag; table 0x004BB990, 450 x 10h), marker remove
// 0x00187E70 (obj), unit icon remove 0x00187FC0 (obj). Players 3 / 4's MapDisplays never got any (r4: no command posts
// on their minimaps). Each now also runs for ours: the game's own function again with ours in the 2 slots of the
// MapDisplay array (an add finds the object's slot again; a remove runs first for ours with the slot put back after,
// then for the game's two). At each match start the objects already registered are given to ours the same way.
const char* const kMapIconPat[4] = {
    "55 8B EC 83 EC 08 53 56 57 E8 ?? ?? ?? ?? 84 C0 74 25 E8 ?? ?? ?? ?? 84 C0 74 0D E8 ?? ?? ?? ?? 84 C0 74 04 32 DB EB "
    "02 B3 01 E8 ?? ?? ?? ?? 3A D8 0F 85 ?? ?? ?? ?? 8B 5D 08 85 DB 0F 84 ?? ?? ?? ?? 8B CB E8 ?? ?? ?? ?? 85 C0 0F 84 ?? "
    "?? ?? ?? 8B 10 8B C8 FF 52 28 85 C0 0F 84 ?? ?? ?? ?? 83 78 38 00 0F 84 ?? ?? ?? ?? 83 CF FF B8 ?? ?? ?? ??",
    "55 8B EC 51 53 56 57 E8 ?? ?? ?? ?? 84 C0 74 25 E8 ?? ?? ?? ?? 84 C0 74 0D E8 ?? ?? ?? ?? 84 C0 74 04 32 DB EB 02 B3 "
    "01 E8 ?? ?? ?? ?? 3A D8 0F 85 ?? ?? ?? ?? 8B 7D 08 85 FF 0F 84 ?? ?? ?? ?? 8B 07 8B CF FF 50 28 85 C0 0F 84 ?? ?? ?? "
    "?? 83 78 38 00 0F 84 ?? ?? ?? ?? 83 CB FF B8 ?? ?? ?? ??",
    "55 8B EC 53 E8 ?? ?? ?? ?? 84 C0 74 25 E8 ?? ?? ?? ?? 84 C0 74 0D E8 ?? ?? ?? ?? 84 C0 74 04 32 DB EB 02 B3 01 E8 ?? "
    "?? ?? ?? 3A D8 0F 85 ?? ?? ?? ?? 56 8B 75 08 85 F6 0F 84 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 85 C0 0F 84 ?? ?? ?? ?? 8B "
    "10 8B C8 FF 52 28 85 C0 0F 84 ?? ?? ?? ?? 83 78 38 00 0F 84 ?? ?? ?? ?? 33 C9 B8 ?? ?? ?? ??",
    "55 8B EC 53 56 57 E8 ?? ?? ?? ?? 84 C0 74 25 E8 ?? ?? ?? ?? 84 C0 74 0D E8 ?? ?? ?? ?? 84 C0 74 04 32 DB EB 02 B3 01 "
    "E8 ?? ?? ?? ?? 3A D8 0F 85 ?? ?? ?? ?? 8B 75 08 85 F6 0F 84 ?? ?? ?? ?? 8B 06 8B CE FF 50 28 85 C0 0F 84 ?? ?? ?? ?? "
    "83 78 38 00 0F 84 ?? ?? ?? ?? 33 FF B8 ?? ?? ?? ??",
};
const uint32_t kMapIconTableAt[4] = { 110, 93, 105, 91 }; // its "mov eax, <table>" operand
const uint32_t kMapIconEndAt[4] = { 136, 119, 121, 105 }; // its "cmp eax, <table end>" operand
const uint32_t kMapIconArrAt[4] = { 201, 183, 144, 123 }; // the MapDisplay array's operand (the per-display loop)
const uint32_t kMapIconTableBytes[4] = { 0x100, 0x1C20, 0x100, 0x1C20 };
const char* const kMapIconName[4] = { "marker add", "unit icon add", "marker remove", "unit icon remove" };
constexpr uint32_t kMapSlotBytes = 0x10;
typedef void(__cdecl* MapIcon1_t)(uintptr_t obj);
typedef void(__cdecl* MapIcon2_t)(uintptr_t obj, uint32_t flag);
// 20g-r5: MapDisplay SetMode(mode) (GOG 1.3 0x00188460, thiscall): sizes and places the map for its mode. The game calls
// it for players 1 / 2 when they spawn; never for ours - their small map stayed unsized (r4: no minimap until a player
// opened his big map). Ours get SetMode(0) each time their HUD is shown.
const char* const kMapSetModeBody =
    "55 8B EC 8B 45 08 53 8B D9 56 57 39 83 8C F2 01 00 74 06 FF 05 ?? ?? ?? ?? 89 83 8C F2 01 00 E8";
typedef void(__fastcall* MapSetMode_t)(uintptr_t self, void* edx, int mode);
// 20g-r5: CameraManager::ViewToCamera(view) (GOG 1.3 0x00094A60, thiscall, one jump thunk, 3 calls): the camera whose
// renderer view is `view` (its 2 slots of +6Ch), 0 if none - so in views 3 / 4 it answered player 1. Its callers: the
// command post hologram's draw (whose team colours it: "friend" / "enemy" from that camera's player - players 3 / 4 saw
// player 1's colours), a sky / lens draw (GetCamera of it) and a per-camera "seen" bit. Views 3 / 4 now answer 2 / 3.
const char* const kViewToCamBody =
    "55 8B EC 8B 55 08 33 C0 83 C1 6C ?? ?? ?? ?? ?? 39 11 74 0B 40 83 C1 04 83 F8 02 7C F3 33 C0 5D C2 04 00";
typedef int(__fastcall* ViewToCam_t)(void* cm, void* edx, int view);
constexpr uint32_t kHudShownByte = 0x20; // a display's "shown" byte (the first thing its ForPlayer call sets)
constexpr uint32_t kHudPlayer = 0x18;    // a display's player
constexpr uint32_t kHudViewport = 0x1C;  // the viewport its ForPlayer matches (its root, or its camera index)
constexpr uint32_t kHudTaskNode = 0x04;  // non-zero while it is in a task list
constexpr int kHudScope = 4;
constexpr int kHudMaskDefault = 0xFF;    // every class (20g-r7: Scope, bit 4, too - it was 0xEF)

bool g_hudWanted = true;          // [Splitscreen] HudP34
int g_hudMask = kHudMaskDefault;  // [Splitscreen] HudP34Mask
bool g_hudOn = false;             // the camera manager answers are in and at least one class is made for players 3 / 4

struct Hud34Run
{
    bool on = false;              // this class is made for players 3 / 4
    uintptr_t origCreate = 0;     // where the thunks went before (the game's functions)
    uintptr_t origLoop = 0;
    uintptr_t origDestroy = 0;
    HudForPlayer_t forPlayer = nullptr;
    uint32_t forPlayerRva = 0;
    uintptr_t ours[2] = {};       // player 3's, player 4's
    bool setUp[2] = {};           // set up for this match (cleared at the teardown)
    LONG made = 0;
    LONG setups = 0;
    LONG deleted = 0;
};
Hud34Run g_hr[8];
int g_hudRoot[2] = { -1, -1 };    // the root player 3's / 4's displays were set up in
int g_hudShown[2] = { -1, -1 };   // -1 = not applied yet, 0 = hidden, 1 = shown
uintptr_t g_origRootIndex = 0;
uintptr_t g_origIsChase = 0;
uintptr_t g_origGetChase = 0;
uintptr_t g_origMapTexAll = 0;
HudMapTex_t g_mapTex = nullptr;
uintptr_t g_origMapObj = 0;       // 20g-r4
volatile LONG g_hudMapObjSkipped = 0;
uintptr_t g_origMapIcon[4] = {};  // 20g-r5
uint32_t g_mapIconFn[4] = {};
uint32_t g_mapTable[4] = {};      // each function's table (rva)
bool g_mapIconsOn = false;
volatile LONG g_mapIconCalls[4] = {};
volatile LONG g_mapIconOurs[4] = {};
long g_mapReplayed = 0;
MapSetMode_t g_mapSetMode = nullptr; // 20g-r5
volatile LONG g_mapSetModes = 0;
uintptr_t g_origViewToCam = 0;    // 20g-r5
volatile LONG g_viewToCamAnswers = 0;
volatile LONG g_hudCamAnswers[3] = {}; // RootIndex, IsChase, GetChase answered for 2 / 3
volatile LONG g_hudMapTex = 0;
volatile LONG g_hudShowCalls = 0;
int g_hudLines = 0;

bool HudLog()
{
    if (g_hudLines >= 160)
        return false;
    ++g_hudLines;
    return true;
}

int HudKindClass(int kind)
{
    switch (kind)
    {
    case kBoot: return 0;
    case kCapture: return 1;
    case kMap: return 2;
    case kScope: return 4;
    case kStatus: return 6;
    case kTeam: return 7;
    default: return -1;
    }
}

uintptr_t HudOwn(int kind, int k)
{
    const int d = HudKindClass(kind);
    if (!g_hudOn || d < 0 || k < 0 || k > 1)
        return 0;
    const Hud34Run& r = g_hr[d];
    return (r.on && r.setUp[k]) ? r.ours[k] : 0;
}

int HudRoot(int k)
{
    if (!g_hudOn || k < 0 || k > 1)
        return -1;
    for (int d = 0; d < 8; ++d)
        if (g_hr[d].on && g_hr[d].setUp[k] && g_hr[d].ours[k])
            return g_hudRoot[k];
    return -1;
}

// The root player 3's / 4's displays go in: his view's (views 3 / 4 are the renderer's 2 / 3), else 2 / 3.
int HudRootFor(int k)
{
    const int v = PlayerNSpecView(k);
    return v >= 0 ? v : k + 2;
}

bool HudCamGuarded(void* cm, int i)
{
    return g_hudOn && g_playersOn && (i == 2 || i == 3) && i + 1 <= static_cast<int>(g_players) &&
           reinterpret_cast<uintptr_t>(cm) == CameraManager();
}

bool NameRootRemap(void* cm, int i, int* root); // 21j-r34 (the names section below): our name displays' setup

int __fastcall HudRootIndexWrap(void* cm, void* edx, int i)
{
    (void)edx;
    int remapped = 0;
    if (NameRootRemap(cm, i, &remapped)) // 21j-r34: only while the game's name setup runs on views 3 / 4's displays
        return remapped;
    if (HudCamGuarded(cm, i))
    {
        InterlockedIncrement(&g_hudCamAnswers[0]);
        P34Guard("camera manager RootIndex (his view's HUD root)", 60u, BF1_RETURN_ADDRESS(), i);
        return HudRootFor(i - 2);
    }
    return reinterpret_cast<HudRootIndex_t>(g_origRootIndex)(cm, nullptr, i);
}

bool __fastcall HudIsChaseWrap(void* cm, void* edx, int i)
{
    (void)edx;
    if (HudCamGuarded(cm, i))
    {
        InterlockedIncrement(&g_hudCamAnswers[1]);
        P34Guard("camera manager IsChase (his chase camera)", 61u, BF1_RETURN_ADDRESS(), i);
        const int k = i - 2;
        return g_chaseOk && g_chaseMade[k] && g_chaseTarget[k] != 0;
    }
    return reinterpret_cast<HudIsChase_t>(g_origIsChase)(cm, nullptr, i);
}

uintptr_t __fastcall HudGetChaseWrap(void* cm, void* edx, int i)
{
    (void)edx;
    if (HudCamGuarded(cm, i))
    {
        InterlockedIncrement(&g_hudCamAnswers[2]);
        P34Guard("camera manager GetChase (his chase camera)", 62u, BF1_RETURN_ADDRESS(), i);
        const int k = i - 2;
        if (g_chaseMade[k])
            return reinterpret_cast<uintptr_t>(g_chase[k]);
        return reinterpret_cast<HudGetChase_t>(g_origGetChase)(cm, nullptr, 0); // never null (IsChase says no anyway)
    }
    return reinterpret_cast<HudGetChase_t>(g_origGetChase)(cm, nullptr, i);
}

// Made right after the game's two (the class's create).
void HudMake(int d)
{
    Hud34Run& r = g_hr[d];
    const Hud34Type& h = g_h34[d];
    if (!r.on)
        return;
    const HudNew_t gameNew = reinterpret_cast<HudNew_t>(Rva(h.newFn));
    const HudCtor_t ctor = reinterpret_cast<HudCtor_t>(Rva(h.ctor));
    for (int k = 0; k < 2; ++k)
    {
        r.setUp[k] = false;
        r.ours[k] = 0; // the game's create writes over its own two the same way
        if (k + 3 > static_cast<int>(g_players))
            continue;
        const uintptr_t mem = gameNew(h.size);
        r.ours[k] = mem ? ctor(mem, nullptr) : 0;
        if (r.ours[k])
            InterlockedIncrement(&r.made);
    }
    if (HudLog())
    {
        const uintptr_t arr = Rva(g_hudArray[d]);
        const bool failed = !r.ours[0] || (g_players >= 4 && !r.ours[1]);
        Log("[hud34] %lu ms: %s made for players 3 / 4: %08X %08X (the game's own two: %08X %08X; new(0x%X)%s)",
            ScanMsSinceAttach(), kHudName[d], static_cast<uint32_t>(r.ours[0]), static_cast<uint32_t>(r.ours[1]),
            ReadU32(arr), ReadU32(arr + 4), h.size, failed ? " - the game's new FAILED" : "");
    }
}

void MapReplayIcons(); // 20g-r5 (below)

// Set up right after the game's two (the class's per-camera loop), when the game set up both of its own.
void HudSetupOurs(int d)
{
    Hud34Run& r = g_hr[d];
    const Hud34Type& h = g_h34[d];
    if (!r.on)
        return;
    const uintptr_t cm = CameraManager();
    const int count = cm ? static_cast<int>(ReadU32(cm + kCamCount)) : 0;
    int root[2] = { -1, -1 };
    for (int k = 0; k < 2; ++k)
    {
        r.setUp[k] = false;
        if (!r.ours[k] || count != 2)
            continue;
        reinterpret_cast<HudReset_t>(Rva(h.reset))(r.ours[k], nullptr);
        reinterpret_cast<HudSetup_t>(Rva(h.setup))(r.ours[k], nullptr, k + 2);
        r.setUp[k] = true;
        InterlockedIncrement(&r.setups);
        root[k] = HudRootFor(k);
        g_hudRoot[k] = root[k];
        g_hudShown[k] = -1; // shown / hidden from the next frame on (HudFrame)
    }
    if (d == 2 && (r.setUp[0] || r.setUp[1]))
        MapReplayIcons(); // 20g-r5
    if (HudLog())
        Log("[hud34] %lu ms: %s set up for players 3 / 4: %s root %d, %s root %d (the game's camera count %d; player +18h "
            "%d / %d, viewport +1Ch %d / %d)",
            ScanMsSinceAttach(), kHudName[d], r.setUp[0] ? "player 3 in" : "player 3 NOT -", root[0],
            r.setUp[1] ? "player 4 in" : "player 4 NOT -", root[1], count,
            r.setUp[0] ? static_cast<int>(ReadU32(r.ours[0] + kHudPlayer)) : -1,
            r.setUp[1] ? static_cast<int>(ReadU32(r.ours[1] + kHudPlayer)) : -1,
            r.setUp[0] ? static_cast<int>(ReadU32(r.ours[0] + kHudViewport)) : -1,
            r.setUp[1] ? static_cast<int>(ReadU32(r.ours[1] + kHudViewport)) : -1);
}

// Deleted right before the game's two (the class's destroy, the match teardown). The first destroy of a teardown
// also ends every class's "set up" (Capture's too: its displays stay, as the game's, but are not answered any more).
void DeathStop34(int k, const char* why); // 20g-r6 (above)

void HudDeleteOurs(int d)
{
    P2SpawnP34Teardown(); // 20g-r5: players 3 and 4's spawn screens deleted while this match is still loaded
    DeathStop34(0, "the match teardown"); // 20g-r6: their death cameras follow soldiers of this match
    DeathStop34(1, "the match teardown");
    Listener34Off(0, "the match teardown"); // 20g-r6: their listeners (the camera manager's go with it)
    Listener34Off(1, "the match teardown");
    // 20g-r7: their tint records and scope blur slots start empty in the next match (the game's are made again)
    memset(g_tintRec, 0, sizeof(g_tintRec));
    g_tintFilter = nullptr;
    g_scopeBlur[0] = g_scopeBlur[1] = 0;
    g_wallDist[0] = g_wallDist[1] = 1.0f;
    InterlockedExchange(&g_p1ShotValid, 0); // 20g-r9: the next match's shots are its own
    g_shotOn[0] = g_shotOn[1] = g_shotOn[2] = false;
    g_endShotOn[0] = g_endShotOn[1] = false; // 21j-r15
    for (int c = 0; c < 8; ++c)
        for (int k = 0; k < 2; ++k)
            g_hr[c].setUp[k] = false;
    g_hudRoot[0] = g_hudRoot[1] = -1;
    g_hudShown[0] = g_hudShown[1] = -1;
    for (int k = 2; k < 4; ++k) // 21j-r7: the reticle scan forgets players 3 / 4's displays (deleted below)
    {
        g_ret[k].group = 0;
        g_ret[k].pending = false;
    }
    Hud34Run& r = g_hr[d];
    uintptr_t gone[2] = {};
    for (int k = 0; k < 2; ++k)
    {
        const uintptr_t obj = r.ours[k];
        r.ours[k] = 0;
        if (!obj)
            continue;
        const uintptr_t vt = ReadU32(obj);
        const uintptr_t del = InImage(vt) ? ReadU32(vt) : 0;
        if (!InImage(del))
        {
            if (HudLog())
                Log("[hud34] %s %08X: its vtable %08X / destructor %08X is not the game's - NOT deleted", kHudName[d],
                    static_cast<uint32_t>(obj), static_cast<uint32_t>(vt), static_cast<uint32_t>(del));
            continue;
        }
        reinterpret_cast<HudDelete_t>(del)(obj, nullptr, 1);
        gone[k] = obj;
        InterlockedIncrement(&r.deleted);
    }
    if (HudLog())
        Log("[hud34] %lu ms: %s: players 3 / 4's deleted (%08X %08X) before the game's own (the teardown)",
            ScanMsSinceAttach(), kHudName[d], static_cast<uint32_t>(gone[0]), static_cast<uint32_t>(gone[1]));
}

template <int D>
void __cdecl HudCreateWrap()
{
    reinterpret_cast<HudVoid_t>(g_hr[D].origCreate)();
    HudMake(D);
}

template <int D>
void __cdecl HudLoopWrap()
{
    reinterpret_cast<HudVoid_t>(g_hr[D].origLoop)();
    HudSetupOurs(D);
}

template <int D>
void __cdecl HudDestroyWrap()
{
    HudDeleteOurs(D);
    reinterpret_cast<HudVoid_t>(g_hr[D].origDestroy)();
}

const void* HudWrap(int which, int d) // which: 0 create, 1 loop, 2 destroy
{
    switch (which * 8 + d)
    {
    case 0: return reinterpret_cast<const void*>(&HudCreateWrap<0>);
    case 1: return reinterpret_cast<const void*>(&HudCreateWrap<1>);
    case 2: return reinterpret_cast<const void*>(&HudCreateWrap<2>);
    case 3: return reinterpret_cast<const void*>(&HudCreateWrap<3>);
    case 4: return reinterpret_cast<const void*>(&HudCreateWrap<4>); // 20g-r9: Scope (r7 allowed it, but had no wraps)
    case 5: return reinterpret_cast<const void*>(&HudCreateWrap<5>);
    case 6: return reinterpret_cast<const void*>(&HudCreateWrap<6>);
    case 7: return reinterpret_cast<const void*>(&HudCreateWrap<7>);
    case 8: return reinterpret_cast<const void*>(&HudLoopWrap<0>);
    case 9: return reinterpret_cast<const void*>(&HudLoopWrap<1>);
    case 10: return reinterpret_cast<const void*>(&HudLoopWrap<2>);
    case 11: return reinterpret_cast<const void*>(&HudLoopWrap<3>);
    case 12: return reinterpret_cast<const void*>(&HudLoopWrap<4>); // 20g-r9
    case 13: return reinterpret_cast<const void*>(&HudLoopWrap<5>);
    case 14: return reinterpret_cast<const void*>(&HudLoopWrap<6>);
    case 15: return reinterpret_cast<const void*>(&HudLoopWrap<7>);
    case 16: return reinterpret_cast<const void*>(&HudDestroyWrap<0>);
    case 17: return reinterpret_cast<const void*>(&HudDestroyWrap<1>);
    case 18: return reinterpret_cast<const void*>(&HudDestroyWrap<2>);
    case 19: return reinterpret_cast<const void*>(&HudDestroyWrap<3>);
    case 20: return reinterpret_cast<const void*>(&HudDestroyWrap<4>); // 20g-r9 (the game never calls Scope's destroy)
    case 21: return reinterpret_cast<const void*>(&HudDestroyWrap<5>);
    case 22: return reinterpret_cast<const void*>(&HudDestroyWrap<6>);
    case 23: return reinterpret_cast<const void*>(&HudDestroyWrap<7>);
    default: return nullptr;
    }
}

void __cdecl HudMapTexAllWrap(const char* name)
{
    reinterpret_cast<HudMapTexAll_t>(g_origMapTexAll)(name);
    InterlockedIncrement(&g_hudMapTex);
    const Hud34Run& r = g_hr[2];
    int given = 0;
    for (int k = 0; k < 2; ++k)
        if (r.on && r.ours[k] && g_mapTex)
        {
            g_mapTex(r.ours[k], nullptr, name);
            ++given;
        }
    if (HudLog())
        Log("[hud34] %lu ms: the level's map texture \"%.40s\" given to the game's two MapDisplays and %d of ours",
            ScanMsSinceAttach(), name ? name : "(null)", given);
}

// 20g-r4: MapDisplay's objectives panel - nothing for player 3's / 4's (there is no ifs_mapselect3 / 4).
void __fastcall HudMapObjWrap(uintptr_t self, void* edx, uintptr_t a)
{
    (void)edx;
    const Hud34Run& r = g_hr[2];
    const bool ours = self && (self == r.ours[0] || self == r.ours[1]);
    const int player = self ? *reinterpret_cast<const int32_t*>(self + kHudPlayer) : 0;
    if (ours || player >= 2)
    {
        if (InterlockedIncrement(&g_hudMapObjSkipped) <= 4 && HudLog())
            Log("[hud34] %lu ms: MapDisplay %08X (player %d%s): its objectives panel skipped - no ifs_mapselect%d screen "
                "for it (from rva 0x%08X)",
                ScanMsSinceAttach(), static_cast<uint32_t>(self), player + 1, ours ? ", ours" : "", player + 1,
                RvaOf(reinterpret_cast<uintptr_t>(BF1_RETURN_ADDRESS())));
        return;
    }
    reinterpret_cast<HudMapObj_t>(g_origMapObj)(self, nullptr, a);
}

// 20g-r5: ours in the MapDisplay array's 2 slots for one call of the game's own function (player 4's display twice
// when only player 3 has one). Game thread only; put back right after.
struct MapSwap
{
    uintptr_t* arr = nullptr;
    uintptr_t a0 = 0;
    uintptr_t a1 = 0;
};

bool MapSwapIn(MapSwap& sw)
{
    const Hud34Run& r = g_hr[2];
    if (!g_hudOn || !g_mapIconsOn || !r.on || (!r.ours[0] && !r.ours[1]))
        return false;
    sw.arr = reinterpret_cast<uintptr_t*>(Rva(g_hudArray[2]));
    sw.a0 = sw.arr[0];
    sw.a1 = sw.arr[1];
    sw.arr[0] = r.ours[0] ? r.ours[0] : r.ours[1];
    sw.arr[1] = r.ours[1] ? r.ours[1] : r.ours[0];
    return true;
}

void MapSwapOut(const MapSwap& sw)
{
    sw.arr[0] = sw.a0;
    sw.arr[1] = sw.a1;
}

// the object's slot in function f's table (its 16 bytes), or null
unsigned char* MapSlotOf(int f, uintptr_t obj)
{
    if (!obj || !g_mapTable[f])
        return nullptr;
    unsigned char* const t = reinterpret_cast<unsigned char*>(Rva(g_mapTable[f]));
    for (uint32_t o = 0; o < kMapIconTableBytes[f]; o += kMapSlotBytes)
        if (*reinterpret_cast<const uint32_t*>(t + o) == static_cast<uint32_t>(obj))
            return t + o;
    return nullptr;
}

void __cdecl MapMarkerAddWrap(uintptr_t obj)
{
    reinterpret_cast<MapIcon1_t>(g_origMapIcon[0])(obj);
    InterlockedIncrement(&g_mapIconCalls[0]);
    MapSwap sw;
    if (MapSwapIn(sw))
    {
        reinterpret_cast<MapIcon1_t>(g_origMapIcon[0])(obj);
        MapSwapOut(sw);
        InterlockedIncrement(&g_mapIconOurs[0]);
    }
}

void __cdecl MapUnitAddWrap(uintptr_t obj, uint32_t flag)
{
    reinterpret_cast<MapIcon2_t>(g_origMapIcon[1])(obj, flag);
    InterlockedIncrement(&g_mapIconCalls[1]);
    MapSwap sw;
    if (MapSwapIn(sw))
    {
        reinterpret_cast<MapIcon2_t>(g_origMapIcon[1])(obj, flag);
        MapSwapOut(sw);
        InterlockedIncrement(&g_mapIconOurs[1]);
    }
}

// a remove: first for ours (the slot put back after), then the game's own
void MapRemove(int f, uintptr_t obj)
{
    InterlockedIncrement(&g_mapIconCalls[f]);
    unsigned char* const slot = MapSlotOf(f, obj);
    MapSwap sw;
    if (slot && MapSwapIn(sw))
    {
        unsigned char saved[kMapSlotBytes];
        memcpy(saved, slot, sizeof(saved));
        reinterpret_cast<MapIcon1_t>(g_origMapIcon[f])(obj);
        MapSwapOut(sw);
        memcpy(slot, saved, sizeof(saved));
        InterlockedIncrement(&g_mapIconOurs[f]);
    }
    reinterpret_cast<MapIcon1_t>(g_origMapIcon[f])(obj);
}

void __cdecl MapMarkerRemoveWrap(uintptr_t obj)
{
    MapRemove(2, obj);
}

void __cdecl MapUnitRemoveWrap(uintptr_t obj)
{
    MapRemove(3, obj);
}

// at the match start (our MapDisplays just set up): every object registered so far given to ours (the game's own add,
// ours in the slots)
void MapReplayIcons()
{
    MapSwap sw;
    if (!MapSwapIn(sw))
        return;
    int n[2] = {};
    for (int f = 0; f < 2; ++f)
    {
        const unsigned char* const t = reinterpret_cast<const unsigned char*>(Rva(g_mapTable[f]));
        for (uint32_t o = 0; o < kMapIconTableBytes[f]; o += kMapSlotBytes)
        {
            const uintptr_t obj = *reinterpret_cast<const uint32_t*>(t + o);
            if (!obj)
                continue;
            if (f == 0)
                reinterpret_cast<MapIcon1_t>(g_origMapIcon[0])(obj);
            else
                reinterpret_cast<MapIcon2_t>(g_origMapIcon[1])(obj, t[o + 0xC]);
            ++n[f];
        }
    }
    MapSwapOut(sw);
    g_mapReplayed += n[0] + n[1];
    if (HudLog())
        Log("[hud34] %lu ms: the minimap objects registered so far given to players 3 / 4's MapDisplays: %d marker(s), %d unit "
            "icon(s) (command posts, soldiers, vehicles)",
            ScanMsSinceAttach(), n[0], n[1]);
}

// 20g-r5: view 3 / 4 -> camera 2 / 3 (the game answered 0, player 1's, for every view not in its 2 slots)
int __fastcall ViewToCamWrap(void* cm, void* edx, int view)
{
    (void)edx;
    const int r = reinterpret_cast<ViewToCam_t>(g_origViewToCam)(cm, nullptr, view);
    const uintptr_t m = reinterpret_cast<uintptr_t>(cm);
    if (r != 0 || !g_playersOn || view < 0 || view > 3 || !g_quarterMgr || m != g_quarterMgr || !g_rendCams)
        return r;
    if (*reinterpret_cast<const int32_t*>(m + kCamViews) == view)
        return r; // player 1's own
    for (int k = 0; k < 2 && k + 3 <= static_cast<int>(g_players); ++k)
    {
        const int v = g_specView[k];
        if (v == view && *reinterpret_cast<const uintptr_t*>(g_rendCams + static_cast<uintptr_t>(v) * 4) ==
                             reinterpret_cast<uintptr_t>(g_specCam[k]))
        {
            InterlockedIncrement(&g_viewToCamAnswers);
            P34Guard("camera manager ViewToCamera (his view's camera)", 63u, BF1_RETURN_ADDRESS(), k + 2);
            return k + 2;
        }
    }
    return r;
}

// Player k's displays shown / hidden through each class's own ForPlayer loop (ours in slot 0 for the call).
// one class's display of player k + 3 shown / hidden through the class's own ForPlayer (ours in slot 0 for the call)
bool HudShowOne(int d, int k, bool on)
{
    Hud34Run& r = g_hr[d];
    if (!r.on || !r.forPlayer || !r.setUp[k] || !r.ours[k])
        return false;
    uintptr_t* const arr = reinterpret_cast<uintptr_t*>(Rva(g_hudArray[d]));
    const uintptr_t a0 = arr[0];
    const uintptr_t a1 = arr[1];
    arr[0] = r.ours[k];
    arr[1] = 0;
    r.forPlayer(*reinterpret_cast<const int32_t*>(r.ours[k] + kHudViewport), on ? 1 : 0);
    arr[0] = a0;
    arr[1] = a1;
    return true;
}

void HudShow(int k, bool on)
{
    int n = 0;
    for (int d = 0; d < 8; ++d)
        if (HudShowOne(d, k, on))
            ++n;
    // 20g-r5: his minimap sized and placed for the small map (the game does it for players 1 / 2 when they spawn)
    bool mode = false;
    if (on && g_mapSetMode && g_hr[2].on && g_hr[2].setUp[k] && g_hr[2].ours[k])
    {
        g_mapSetMode(g_hr[2].ours[k], nullptr, 0);
        InterlockedIncrement(&g_mapSetModes);
        mode = true;
    }
    InterlockedIncrement(&g_hudShowCalls);
    if (HudLog())
        Log("[hud34] %lu ms: player %d's HUD %s (%d display(s), through their own ForPlayer%s)", ScanMsSinceAttach(), k + 3,
            on ? "SHOWN - his spawn screen is down" : "hidden - his spawn screen is up", n,
            mode ? "; his minimap SetMode(0)" : "");
}

// Game thread, each frame (FrameWrap, after the spawn screens): hidden while his spawn screen is up.
void HudFrame()
{
    if (!g_hudOn)
        return;
    for (int k = 0; k < 2; ++k)
    {
        if (HudRoot(k) < 0)
        {
            g_hudShown[k] = -1;
            continue;
        }
        const int want = P2SpawnP34Root(k) >= 0 ? 0 : 1;
        if (want != g_hudShown[k])
        {
            HudShow(k, want != 0);
            g_hudShown[k] = want;
        }
    }
    P2SpawnP34MapFrame(); // 20g-r6: their maps on their spawn point screens (after the HUD is shown / hidden)
}

// A function's single jump thunk to `wrap`, the function's address kept in *orig first (MoveThunk writes it before
// the thunk changes).
bool HudMove(const char* name, uint32_t fnRva, const void* wrap, uintptr_t* orig)
{
    char what[80];
    snprintf(what, sizeof(what), "HUD 3 / 4: %s", name);
    return wrap && MoveThunk(what, fnRva, wrap, orig);
}

void InstallHud34()
{
    if (!g_hudWanted)
    {
        Log("[hud34] HudP34=0 - players 3 and 4 keep player 1's / 2's lent displays and no HUD of their own (as 20f)");
        return;
    }
    if (!g_hud34Ok || !g_playersOn || !g_viewsReady)
    {
        Log("[hud34] players 3 and 4's HUD NOT installed: the scan %s, Players %s, Views=4 %s",
            g_hud34Ok ? "checked out" : "did NOT check out", g_playersOn ? "on" : "off", g_viewsReady ? "on" : "off");
        return;
    }
    // 20g-r7: the scope blur's slots first (players 3 / 4's ScopeDisplays only with them ours)
    if (g_hudMask & (1 << kHudScope))
        InstallScopeBlur();
    // each class's ForPlayer loop (byte-checked with its array), MapDisplay's texture loop and SetMapTexture
    for (int d = 0; d < 8; ++d)
    {
        if (d == kHudScope)
            continue;
        char a[16];
        char pat[160];
        PatBytes(static_cast<uint32_t>(Rva(g_hudArray[d])), a, sizeof(a));
        snprintf(pat, sizeof(pat), kHudForPlayerFmt, a);
        uint32_t hits[4] = {};
        if (FindPattern(pat, hits, 4) == 1)
        {
            g_hr[d].forPlayerRva = hits[0];
            g_hr[d].forPlayer = reinterpret_cast<HudForPlayer_t>(Rva(hits[0]));
        }
    }
    uint32_t texAll = 0;
    uint32_t tex = 0;
    {
        char a[16];
        char b[16];
        char pat[200];
        const uint32_t va = static_cast<uint32_t>(Rva(g_hudArray[2]));
        PatBytes(va, a, sizeof(a));
        PatBytes(va + 8, b, sizeof(b));
        snprintf(pat, sizeof(pat), kHudMapTexAllFmt, a, b);
        uint32_t hits[4] = {};
        if (FindPattern(pat, hits, 4) == 1)
        {
            const uint32_t t = CallTarget(hits[0] + kHudMapTexAllCall);
            if (MatchPatternAt(t, kHudMapTexBody))
            {
                texAll = hits[0];
                tex = t;
            }
        }
    }
    uint32_t texThunks[4] = {};
    int texDirect = 0;
    const int texN = texAll ? FindThunks(texAll, texThunks, 4, &texDirect) : 0;
    const int texCalls = texN == 1 ? CountCallsTo(texThunks[0]) : 0;
    Log("[hud34] ForPlayer loops: Boot rva 0x%08X, Capture 0x%08X, Map 0x%08X, Reticule 0x%08X, StatIcon 0x%08X, Status "
        "0x%08X, Team 0x%08X (GOG 1.3: 0x00093780 0x000962B0 0x001872A0 0x00202710 0x0021BBB0 0x0021C9D0 0x0021FC60); "
        "MapDisplay's map texture loop rva 0x%08X (SetMapTexture rva 0x%08X; %d thunk, %d call(s); GOG 1.3: 0x00167430 / "
        "0x00188740, 1 call)",
        g_hr[0].forPlayerRva, g_hr[1].forPlayerRva, g_hr[2].forPlayerRva, g_hr[3].forPlayerRva, g_hr[5].forPlayerRva,
        g_hr[6].forPlayerRva, g_hr[7].forPlayerRva, texAll, tex, texN, texCalls);
    const bool texOk = texAll && texN == 1 && texDirect == 0 && texCalls >= 1;
    // 20g-r4: MapDisplay's objectives panel (its name format byte-checked too)
    uint32_t mapObj = 0;
    {
        uint32_t hits[4] = {};
        if (FindPattern(kHudMapObjBody, hits, 4) == 1)
        {
            const uint32_t fmt = OperandRva(hits[0] + kHudMapObjName);
            const size_t n = strlen(kHudMapObjFormat) + 1;
            char got[64] = {};
            if (fmt && ReadMem(Rva(fmt), got, n) && memcmp(got, kHudMapObjFormat, n) == 0)
                mapObj = hits[0];
        }
    }
    uint32_t objThunks[4] = {};
    int objDirect = 0;
    const int objN = mapObj ? FindThunks(mapObj, objThunks, 4, &objDirect) : 0;
    const int objCalls = objN == 1 ? CountCallsTo(objThunks[0]) : 0;
    Log("[hud34] MapDisplay's objectives panel rva 0x%08X (\"%s\"; %d thunk, %d call(s); GOG 1.3: 0x001887A0, 2 calls)",
        mapObj, kHudMapObjFormat, objN, objCalls);
    const bool objOk = mapObj && objN == 1 && objDirect == 0 && objCalls >= 1;
    // the camera manager's answers for 2 / 3 first (the displays' Setup asks RootIndex)
    if (!HudMove("camera manager RootIndex", g_rootIndexRva, reinterpret_cast<const void*>(&HudRootIndexWrap),
                 &g_origRootIndex) ||
        !HudMove("camera manager IsChase", g_isChaseRva, reinterpret_cast<const void*>(&HudIsChaseWrap), &g_origIsChase) ||
        !HudMove("camera manager GetChase", g_getChaseRva, reinterpret_cast<const void*>(&HudGetChaseWrap),
                 &g_origGetChase))
    {
        Log("[hud34] the camera manager's thunks could not all be moved - players 3 and 4's HUD NOT installed (any "
            "moved answer only once the HUD is on, so nothing changes)");
        return;
    }
    // 20g-r5: the minimap's icon functions (each byte-checked: its table, the table's end, the MapDisplay array) and
    // SetMode
    {
        const uint32_t mapArrVa = static_cast<uint32_t>(Rva(g_hudArray[2]));
        int found = 0;
        for (int f = 0; f < 4; ++f)
        {
            uint32_t hits[4] = {};
            const int n = FindPattern(kMapIconPat[f], hits, 4);
            uint32_t table = 0;
            bool ok = false;
            if (n == 1)
            {
                const uint32_t fn = hits[0];
                table = OperandRva(fn + kMapIconTableAt[f]);
                ok = table && CodeByte(fn + kMapIconEndAt[f] - 1) == 0x3D &&
                     OperandRva(fn + kMapIconEndAt[f]) == table + kMapIconTableBytes[f] &&
                     CodeU32(fn + kMapIconArrAt[f]) == mapArrVa;
                uint32_t th[4] = {};
                int direct = 0;
                ok = ok && FindThunks(fn, th, 4, &direct) == 1 && direct == 0;
                if (ok)
                {
                    g_mapIconFn[f] = fn;
                    g_mapTable[f] = table;
                    ++found;
                }
            }
            Log("[hud34] MapDisplay %s: %d pattern hit(s) rva 0x%08X, table rva 0x%08X (%u slots) - %s", kMapIconName[f], n,
                n == 1 ? hits[0] : 0u, table, kMapIconTableBytes[f] / kMapSlotBytes, ok ? "OK" : "does NOT check out");
        }
        // the two tables are the same for add and remove, and the unit icons' ends where the markers' begins (GOG 1.3)
        const bool tablesOk = found == 4 && g_mapTable[0] == g_mapTable[2] && g_mapTable[1] == g_mapTable[3] &&
                              g_mapTable[1] + kMapIconTableBytes[1] == g_mapTable[0];
        if (tablesOk)
        {
            const void* const wraps[4] = { reinterpret_cast<const void*>(&MapMarkerAddWrap),
                                           reinterpret_cast<const void*>(&MapUnitAddWrap),
                                           reinterpret_cast<const void*>(&MapMarkerRemoveWrap),
                                           reinterpret_cast<const void*>(&MapUnitRemoveWrap) };
            int moved = 0;
            for (int f = 0; f < 4; ++f)
                if (HudMove(kMapIconName[f], g_mapIconFn[f], wraps[f], &g_origMapIcon[f]))
                    ++moved;
            g_mapIconsOn = moved == 4; // (a partly moved set only does the game's own: MapSwapIn needs all 4)
        }
        Log("[hud34] the minimap's icons for players 3 / 4: %s", g_mapIconsOn ? "ON (the game's own add / remove run for "
            "theirs too)" : (tablesOk ? "NOT on (a thunk could not be moved)" : "NOT on (the functions did not check out)"));
        uint32_t hits[4] = {};
        if (FindPattern(kMapSetModeBody, hits, 4) == 1)
            g_mapSetMode = reinterpret_cast<MapSetMode_t>(Rva(hits[0]));
        Log("[hud34] MapDisplay SetMode rva 0x%08X %s", g_mapSetMode ? hits[0] : 0u,
            g_mapSetMode ? "OK (players 3 / 4's small map set up each time their HUD is shown)" : "NOT found");
    }
    if (texOk && objOk) // 20g-r4: both, or no MapDisplays for players 3 / 4
    {
        g_mapTex = reinterpret_cast<HudMapTex_t>(Rva(tex));
        if (!HudMove("MapDisplay objectives panel", mapObj, reinterpret_cast<const void*>(&HudMapObjWrap), &g_origMapObj) ||
            !HudMove("MapDisplay map texture loop", texAll, reinterpret_cast<const void*>(&HudMapTexAllWrap),
                     &g_origMapTexAll))
            g_mapTex = nullptr;
    }
    int made = 0;
    char list[160] = "";
    for (int d = 0; d < 8; ++d)
    {
        Hud34Run& r = g_hr[d];
        const Hud34Type& h = g_h34[d];
        const char* why = nullptr;
        if (!(g_hudMask & (1 << d)))
            why = "off ([Splitscreen] HudP34Mask)";
        else if (d == kHudScope && !g_scopeOk) // 20g-r7: its blur slots must be ours first
            why = "not made (ScopeP34=0, or its blur slots could not be made ours: they would land on the blur effect)";
        else if (!h.ok)
            why = "NOT made (the scan did not check it out)";
        else if (!r.forPlayer && d != kHudScope) // (Scope has no ForPlayer: it hides itself when not zoomed)
            why = "NOT made (its ForPlayer loop was not found)";
        else if (d == 2 && !g_mapTex)
            why = "NOT made (the map texture loop or the objectives panel was not found / moved)";
        else if (h.destroyCalls == 1 &&
                 !HudMove("destroy", h.destroy, HudWrap(2, d), &r.origDestroy))
            why = "NOT made (its destroy's thunk could not be moved)";
        else if (!HudMove("per-camera Setup loop", h.loop, HudWrap(1, d), &r.origLoop))
            why = "NOT made (its Setup loop's thunk could not be moved)";
        else
        {
            r.on = true; // before the create's thunk moves: from its next call on, ours are made
            if (!HudMove("create", h.create, HudWrap(0, d), &r.origCreate))
            {
                r.on = false;
                why = "NOT made (its create's thunk could not be moved)";
            }
        }
        if (why)
            Log("[hud34] %s: %s", kHudName[d], why);
        else
        {
            ++made;
            const size_t len = strlen(list);
            snprintf(list + len, sizeof(list) - len, "%s%s", len ? ", " : "", kHudName[d]);
        }
    }
    g_hudOn = made > 0;
    Log("[hud34] players 3 and 4's HUD %s: %d of 8 classes made for them (%s), set up in the HUD roots of views 3 / 4 "
        "at each match start, hidden while their spawn screens are up, deleted at the teardown; RootIndex / IsChase / "
        "GetChase answer 2 / 3 (his view's root, his chase camera); HudP34Mask=%d",
        g_hudOn ? "ON" : "NOT on", made, list[0] ? list : "none", g_hudMask);
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r33 / r34: PLAYERS 3 AND 4'S NAME TAGS ([Splitscreen] NamesP34=1)
// ---------------------------------------------------------------------------------------------------------
// The names over the human players' soldiers come from NameDisplayThread (GOG 1.3 RTTI; 0x4800 bytes): a task, one per
// VIEWER, kept in a static [2] (GOG 1.3 rva 0x004C14D0). Made at the level load by a fixed 2-loop (create 0x00195D50: new,
// memset, the class's constructor 0x00195C80, its viewer +18h = i), set up at the match start by a loop over the camera
// count (setup 0x00195E40: for camera i - the task's AddToLayer(0, 0), then for each of its 50 name slots (+20h, B0h each)
// and their texts (+2280h, C0h each) the slot's hide / text, the text's font "gamefont_tiny" and styles, and the slot put
// into the HUD root CameraManager::RootIndex(i) at scale 0.5), run every frame by the game's task list (its vtable +4,
// GOG 1.3 0x00196050), never destroyed (its destroy 0x00195DB0 has no caller; create and setup are called once per level,
// one after the other, by 0x0013D760). Its run: for character j (the SpawnManager's, 0xE8 each) up to max(the camera
// count, the net player count at GOG 1.3 0x004C502C) - j is shown over his soldier in this viewer's view (teammates; an
// enemy only when aimed at), by the camera of GetCamera(viewer).
// Three things kept players 3 / 4 out of it:
//  (1) the loop's end: max(2, the net count) - 2 offline - so characters 2 / 3 (players 3 / 4) were never looked at, in
//      any view. Its "camera count" call now goes to NameBoundWrap: the live player count (3 / 4) while players 3 / 4 are
//      in the match, the game's own answer otherwise (2-player: unchanged).
//  (2) no task for viewers 2 / 3 (views 3 / 4): two more are made and set up for them (below).
//  (3) their characters' names: the game names characters 2 / 3 "Player 3" / "Player 4"; the sign-in's LAUNCH tells
//      BF1GameExt the names players 3 / 4 signed in with (BF1GameExt_split.lua r33: "signname N NAME"), and from then
//      on each frame their characters (+1Ch, 16-bit text, 64) get them while the match has them.
// 21j-r34 (r33's log, Steam: "name display setup: 0 pattern hits" - none of it went in; the user: still no tags for players
// 3 / 4, and players 1 / 2's not in views 3 / 4): r33 looked for the setup with one 290-byte pattern made on GOG 1.3, and
// Steam's build of it differs (as vertical.cpp's function did); r33's check of its RootIndex call would also have failed
// once InstallHud34 had moved RootIndex's thunk (the call's target followed into BF1GameExt). r34 needs none of the setup's
// insides:
//  - the setup is found by ANCHORS: the one function that loads the name array's address (mov r32, imm32), pushes
//    "gamefont_tiny", holds the 2280h / 4800h of its 50-slot loop and calls RootIndex through RootIndex's own thunk. If
//    not exactly one, every candidate's anchors and bytes go into the log ([names34] lines).
//  - (2) views 3 / 4's two tasks are made and set up by the GAME'S OWN CODE: right after the game's setup, its create is
//    called once more (two new tasks in its array; the game's two put back), then its setup once more with the array
//    holding ours, while RootIndex(0 / 1) answers views 3 / 4's HUD roots (HudRootIndexWrap -> NameRootRemap). With 3
//    players only player 3's is wanted: the camera count reads 1 from the setup's first camera on (put back after it),
//    so its loop ends after one.
//  - (1) the run's loop end is found by its two calls - the camera count (mov eax, [ecx+20h]; ret), push eax, max(a, b) -
//    in the run's first 200h bytes (one site), and the run is checked against the class's vtable +4 at the first setup.
//  - (3) the sign-in names need nothing but NamesP34=1 and players 3 / 4 on.
// [names34] lines.
constexpr uint32_t kNameViewer = 0x18;       // a NameDisplayThread's viewer
constexpr uint32_t kCharNameAt = 0x1C;       // a character's name (16-bit text)
constexpr int kCharNameChars = 64;
const char* const kNameCreateBody =
    "56 57 33 F6 68 00 48 00 00 E8 ?? ?? ?? ?? 8B F8 83 C4 04 85 FF 74 19 68 00 48 00 00 6A 00 57 E8 ?? ?? ?? ?? 83 C4 0C "
    "8B CF E8 ?? ?? ?? ?? EB 02 33 C0 89 04 B5 ?? ?? ?? ?? 89 70 18 46 83 FE 02 7C C2 5F 5E C3";
constexpr uint32_t kNameCreateArray = 0x35;  // mov [esi*4 + array], eax (its viewer: mov [eax+18h], esi right after)
// the run's head; its loop end is looked for in its first 200h bytes: call count; push eax; call max
const char* const kNameRunHead = "55 8B EC 81 EC 94 00 00 00 53 56 57 8B F9 89 7D F8 FF 77 18 E8";
constexpr uint32_t kNameRunScan = 0x200;
const char* const kCamCountBody = "8B 41 20 C3";                                    // mov eax, [ecx+20h]; ret
const char* const kMaxBody = "55 8B EC 8B 45 0C 39 45 08 0F 4F 45 08 5D C3";       // max(a, b)
constexpr uint32_t kNameSetupBack = 0x100;   // a candidate's start: at most this far before its array load
constexpr uint32_t kNameSetupMax = 0x400;    // a candidate's length looked at (up to its int3 padding)
constexpr uint32_t kNameSlotsImm = 0x2280;   // the setup's 50-slot loop: the texts' start, its end
constexpr uint32_t kNameEndImm = 0x4800;
const char kNameFont[] = "gamefont_tiny";

typedef int(__fastcall* NameCount_t)(uintptr_t cm, void* edx);

bool g_namesWanted = true;       // [Splitscreen] NamesP34
bool g_namesDispOn = false;      // (2) views 3 / 4's tasks: made and set up at each setup
bool g_namesBoundOn = false;     // (1) the run's loop end patched
bool g_namesRunOk = true;        // (1) the run is the class's vtable +4 (checked at the first setup; false: the game's end)
bool g_namesRunChecked = false;
bool g_namesCharOn = false;      // (3) the sign-in names
uint32_t g_nameCreate = 0;       // the game's create (called directly)
uint32_t g_nameRun = 0;
uintptr_t g_nameOrigSetup = 0;
uint32_t g_nameArray = 0;        // the game's NameDisplayThread [2] (rva)
NameCount_t g_nameOrigCount = nullptr; // the camera count the run's loop end called
uintptr_t g_nameOurs[2] = {};    // views 3 / 4's tasks (viewers 2 / 3)
bool g_nameSetUp[2] = {};
int g_nameRoot[2] = { -1, -1 };
volatile LONG g_nameBoundCalls = 0;
volatile LONG g_nameBoundRaised = 0;
int g_nameLines = 0;
wchar_t g_signName[2][kCharNameChars] = {}; // (3) players 3 / 4's sign-in names ("" = none)
long g_signNameWrites = 0;
// the second setup call (NameRootRemap)
bool g_nameRemapOn = false;
int g_nameRemapWant = 0;         // tasks to set up: 1 (3 players) or 2
uintptr_t g_nameRemapCm = 0;
int g_nameRemapCalls[2] = {};
bool g_nameCountCut = false;     // the camera count reads 1 until the setup returns (3 players)
uint32_t g_nameCountWas = 0;
bool g_nameRemapExtra = false;   // RootIndex(1) came with 3 players (the setup did not read the count again)

bool NameLog()
{
    if (g_nameLines >= 120)
        return false;
    ++g_nameLines;
    return true;
}

// (1) the run's loop end: the live player count while players 3 / 4 are in this match
int __fastcall NameBoundWrap(uintptr_t cm, void* edx)
{
    (void)edx;
    const int game = g_nameOrigCount(cm, nullptr);
    InterlockedIncrement(&g_nameBoundCalls);
    const int live = (g_namesBoundOn && g_namesRunOk && g_playersOn) ? static_cast<int>(g_players) : 0;
    if (live >= 3 && live > game)
    {
        InterlockedIncrement(&g_nameBoundRaised);
        return live;
    }
    return game;
}

// (2) RootIndex(0 / 1) while the game's setup runs on views 3 / 4's tasks (HudRootIndexWrap asks first): their HUD roots
bool NameRootRemap(void* cm, int i, int* root)
{
    if (!g_nameRemapOn || reinterpret_cast<uintptr_t>(cm) != g_nameRemapCm || i < 0 || i > 1)
        return false;
    ++g_nameRemapCalls[i];
    if (i == 0 && g_nameRemapWant == 1 && !g_nameCountCut)
    {
        // 3 players: the setup's loop reads the camera count after each camera - 1 from here on ends it after player 3's
        g_nameCountWas = ReadU32(g_nameRemapCm + kCamCount);
        *reinterpret_cast<uint32_t*>(g_nameRemapCm + kCamCount) = 1;
        g_nameCountCut = true;
    }
    if (i == 1 && g_nameRemapWant == 1)
        g_nameRemapExtra = true; // (that task's viewer is 2 too: a second copy of player 3's tags, never another view's)
    *root = HudRootFor(g_nameRemapWant == 1 ? 0 : i);
    return true;
}

// the class's run (vtable +4) against the run found by pattern, with the game's first task
void NameCheckRun(uintptr_t task)
{
    if (g_namesRunChecked || !task)
        return;
    g_namesRunChecked = true;
    const uint32_t vt = RvaOf(ReadU32(task));
    uint32_t run = vt ? RvaOf(ReadU32(Rva(vt) + 4)) : 0;
    for (int hop = 0; run && hop < 8 && CodeByte(run) == 0xE9; ++hop)
        run = RelTarget(run);
    g_namesRunOk = run == g_nameRun;
    if (NameLog())
        Log("[names34] %lu ms: the game's name task 0x%08X: its class's vtable rva 0x%08X, its run (+4) rva 0x%08X - %s", ScanMsSinceAttach(),
            static_cast<uint32_t>(task), vt, run,
            g_namesRunOk ? "the run found by pattern: OK" : "NOT the run found by pattern - its loop end is left as the game's");
}

// (2) after the game's setup: views 3 / 4's tasks, made by the game's create and set up by the game's setup
void __cdecl NameSetupWrap()
{
    reinterpret_cast<HudVoid_t>(g_nameOrigSetup)();
    const uintptr_t arr = Rva(g_nameArray);
    const uint32_t game0 = ReadU32(arr);
    const uint32_t game1 = ReadU32(arr + 4);
    NameCheckRun(game0);
    for (int k = 0; k < 2; ++k)
    {
        g_nameOurs[k] = 0; // (a new level: the last level's stay in its heap, as the game's own)
        g_nameSetUp[k] = false;
        g_nameRoot[k] = -1;
    }
    if (!g_namesDispOn || !g_playersOn)
        return;
    const uintptr_t cm = CameraManager();
    const int count = cm ? static_cast<int>(ReadU32(cm + kCamCount)) : 0;
    const int want = g_players >= 4 ? 2 : (g_players == 3 ? 1 : 0);
    if (!want || count != 2 || !game0 || !game1)
    {
        if (want && NameLog())
            Log("[names34] %lu ms: name tasks for views 3 / 4 NOT made: the game's camera count %d (need 2), its tasks %08X %08X, "
                "%u players", ScanMsSinceAttach(), count, game0, game1, g_players);
        return;
    }
    // the game's create once more: two new tasks in its array (viewers 0 / 1), the game's own two put back
    reinterpret_cast<HudVoid_t>(Rva(g_nameCreate))();
    const uint32_t our0 = ReadU32(arr);
    const uint32_t our1 = ReadU32(arr + 4);
    *reinterpret_cast<uint32_t*>(arr) = game0;
    *reinterpret_cast<uint32_t*>(arr + 4) = game1;
    if (!our0 || !our1 || our0 == game0 || our1 == game1)
    {
        if (NameLog())
            Log("[names34] %lu ms: name tasks for views 3 / 4 NOT made: the game's create gave %08X %08X", ScanMsSinceAttach(), our0, our1);
        return;
    }
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(our0) + kNameViewer) = 2;
    *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(our1) + kNameViewer) = want == 2 ? 3u : 2u;
    g_nameOurs[0] = our0;
    g_nameOurs[1] = our1;
    // the game's setup once more, on ours: RootIndex(0 / 1) -> views 3 / 4's HUD roots
    *reinterpret_cast<uint32_t*>(arr) = our0;
    *reinterpret_cast<uint32_t*>(arr + 4) = our1;
    g_nameRemapCm = cm;
    g_nameRemapWant = want;
    g_nameRemapCalls[0] = 0;
    g_nameRemapCalls[1] = 0;
    g_nameCountCut = false;
    g_nameRemapExtra = false;
    g_nameRemapOn = true;
    reinterpret_cast<HudVoid_t>(g_nameOrigSetup)();
    g_nameRemapOn = false;
    if (g_nameCountCut)
    {
        *reinterpret_cast<uint32_t*>(cm + kCamCount) = g_nameCountWas;
        g_nameCountCut = false;
    }
    *reinterpret_cast<uint32_t*>(arr) = game0;
    *reinterpret_cast<uint32_t*>(arr + 4) = game1;
    for (int k = 0; k < want; ++k)
    {
        g_nameSetUp[k] = g_nameRemapCalls[k] > 0;
        g_nameRoot[k] = g_nameSetUp[k] ? HudRootFor(k) : -1;
    }
    if (NameLog())
        Log("[names34] %lu ms: name tasks for views 3 / 4 made by the game's create (%08X viewer 2, %08X viewer %d) and set up by "
            "the game's setup: player 3's %s (root %d), player 4's %s (root %d) - RootIndex(0 / 1) answered %d / %d time(s)%s; "
            "the game's own %08X %08X; the name loop's end: %ld calls so far, %ld raised to the player count",
            ScanMsSinceAttach(), our0, our1, want == 2 ? 3 : 2, g_nameSetUp[0] ? "IN" : "not", g_nameRoot[0],
            want < 2 ? "not wanted (3 players)" : (g_nameSetUp[1] ? "IN" : "not"), g_nameRoot[1], g_nameRemapCalls[0],
            g_nameRemapCalls[1], g_nameRemapExtra ? " (3 players: the setup did not read the count again - player 3's set up twice)" : "",
            game0, game1, static_cast<long>(g_nameBoundCalls), static_cast<long>(g_nameBoundRaised));
}

// (3) the sign-in's names (menus.cpp, "signname N NAME" / "signname clear" - PlayerNSignName below); player 3 / 4,
// Latin-1 text
int SignNameSet(int player, const char* name)
{
    if (player != 3 && player != 4)
        return 0;
    wchar_t* const w = g_signName[player - 3];
    int n = 0;
    for (; name && name[n] && n < kCharNameChars - 1; ++n)
        w[n] = static_cast<wchar_t>(static_cast<unsigned char>(name[n]));
    for (int i = n; i < kCharNameChars; ++i)
        w[i] = 0;
    return n > 0 ? 1 : 0;
}

void SignNamesClear()
{
    memset(g_signName, 0, sizeof(g_signName));
}

// Each frame: players 3 / 4's characters carry their sign-in names (the game made them "Player 3" / "Player 4"; written again
// whenever the character's name is not the sign-in's - a new match's characters, or the game writing its own back).
void Names34Frame()
{
    if (!g_namesCharOn || !g_playersOn)
        return;
    for (int k = 0; k < 2; ++k)
    {
        if (k + 3 > static_cast<int>(g_players) || !g_signName[k][0])
            continue;
        const uintptr_t ch = LocalCharacter(k + 2);
        wchar_t now[kCharNameChars] = {};
        if (!ch || !ReadMem(ch + kCharNameAt, now, sizeof(now)) || memcmp(now, g_signName[k], sizeof(now)) == 0)
            continue;
        char was[kCharNameChars + 1] = {};
        char to[kCharNameChars + 1] = {};
        for (int i = 0; i < kCharNameChars - 1 && now[i]; ++i)
            was[i] = now[i] < 0x80 ? static_cast<char>(now[i]) : '?';
        for (int i = 0; i < kCharNameChars - 1 && g_signName[k][i]; ++i)
            to[i] = g_signName[k][i] < 0x80 ? static_cast<char>(g_signName[k][i]) : '?';
        memcpy(reinterpret_cast<void*>(ch + kCharNameAt), g_signName[k], sizeof(now));
        ++g_signNameWrites;
        if (g_signNameWrites <= 40 && NameLog())
            Log("[names34] %lu ms: player %d's character 0x%08X named \"%s\" (the sign-in's name; it had \"%s\") - write %ld",
                ScanMsSinceAttach(), k + 3, static_cast<uint32_t>(ch), to, was, g_signNameWrites);
    }
}

// (1) the run's loop end: call count; push eax; call max - one site in the run's first 200h bytes
uint32_t FindNameBound(uint32_t run, int* hits)
{
    *hits = 0;
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!run || !CodeSection(&codeRva, &codeSize))
        return 0;
    uint32_t site = 0;
    for (uint32_t i = 0; i + 11 <= kNameRunScan; ++i)
    {
        const uint32_t a = run + i;
        if (CodeByte(a) != 0xE8 || CodeByte(a + 5) != 0x50 || CodeByte(a + 6) != 0xE8)
            continue;
        const uint32_t t1 = RelTarget(a);
        const uint32_t t2 = RelTarget(a + 6);
        if (t1 < codeRva || t1 - codeRva >= codeSize || t2 < codeRva || t2 - codeRva >= codeSize)
            continue;
        if (!MatchPatternAt(CallTarget(a), kCamCountBody) || !MatchPatternAt(CallTarget(a + 6), kMaxBody))
            continue;
        if (!site)
            site = a;
        ++*hits;
    }
    return *hits == 1 ? site : 0;
}

bool NameFontAt(uint32_t pushImm)
{
    const uintptr_t s = static_cast<uintptr_t>(pushImm);
    return InImage(s) && InImage(s + sizeof(kNameFont) - 1) && memcmp(reinterpret_cast<const void*>(s), kNameFont, sizeof(kNameFont)) == 0;
}

// (2) the setup by its anchors (see above): its rva, 0 if not exactly one function has all of them
uint32_t FindNameSetup()
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!g_nameArray || !CodeSection(&codeRva, &codeSize) || codeSize < 8)
        return 0;
    const unsigned char* const p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    const uint32_t arrVa = static_cast<uint32_t>(Rva(g_nameArray));
    uint32_t cand[8] = {};
    uint32_t loadAt[8] = {};
    int nc = 0;
    int loads = 0;
    for (uint32_t i = 1; i + 5 <= codeSize; ++i)
    {
        if (p[i] < 0xB8 || p[i] > 0xBF)
            continue;
        uint32_t v = 0;
        memcpy(&v, p + i + 1, 4);
        if (v != arrVa)
            continue;
        ++loads;
        // its function's start: the first byte after the int3 padding before it (at most 100h bytes back)
        uint32_t start = 0;
        for (uint32_t b = 0; b < kNameSetupBack && b < i; ++b)
        {
            const uint32_t s = i - b;
            if (p[s - 1] == 0xCC && p[s] != 0xCC)
            {
                start = codeRva + s;
                break;
            }
        }
        if (!start)
            continue;
        bool dup = false;
        for (int c = 0; c < nc; ++c)
            dup = dup || cand[c] == start;
        if (!dup && nc < 8)
        {
            loadAt[nc] = codeRva + i;
            cand[nc++] = start;
        }
    }
    uint32_t found = 0;
    int good = 0;
    for (int c = 0; c < nc; ++c)
    {
        const uint32_t f = cand[c];
        uint32_t len = 0;
        while (len < kNameSetupMax && !(CodeByte(f + len) == 0xCC && CodeByte(f + len + 1) == 0xCC))
            ++len;
        bool font = false;
        bool root = false;
        bool slots = false;
        bool end = false;
        for (uint32_t j = 0; j + 5 <= len; ++j)
        {
            const uint32_t a = f + j;
            const uint8_t op = CodeByte(a);
            if (op == 0x68 && NameFontAt(CodeU32(a + 1)))
                font = true;
            if (op == 0xE8)
            {
                const uint32_t t = RelTarget(a);
                if (g_rootIndexThunk && t == g_rootIndexThunk)
                    root = true;
                else if (t >= codeRva && t - codeRva < codeSize && CallTarget(a) == g_rootIndexRva)
                    root = true;
            }
            const uint32_t imm = CodeU32(a);
            slots = slots || imm == kNameSlotsImm;
            end = end || imm == kNameEndImm;
        }
        const bool all = font && root && slots && end;
        if (all)
        {
            found = f;
            ++good;
        }
        if (NameLog())
            Log("[names34] setup candidate %d: rva 0x%08X (the array loaded at rva 0x%08X), %u bytes: push \"%s\" %s, RootIndex call "
                "%s, 2280h %s, 4800h %s - %s",
                c + 1, f, loadAt[c], len, kNameFont, font ? "yes" : "no", root ? "yes" : "no", slots ? "yes" : "no", end ? "yes" : "no",
                all ? "ALL ANCHORS" : "not the setup");
    }
    if (good == 1)
        return found;
    Log("[names34] the setup: %d function(s) load the name array's address (%d load(s)), %d of them with every anchor (need 1) - "
        "their bytes:",
        nc, loads, good);
    for (int c = 0; c < nc; ++c)
    {
        uint32_t len = 0;
        while (len < 0x180 && !(CodeByte(cand[c] + len) == 0xCC && CodeByte(cand[c] + len + 1) == 0xCC))
            ++len;
        if (len && InCodeImage(cand[c], len))
            LogBytes("[names34]   ", cand[c], reinterpret_cast<const void*>(Rva(cand[c])), len);
    }
    return 0;
}

// a function's one jump thunk's one call site (0 if it has not exactly one of each)
uint32_t NameOneCallSite(uint32_t fn, uint32_t* thunkOut)
{
    *thunkOut = 0;
    uint32_t thunks[4] = {};
    int direct = 0;
    if (FindThunks(fn, thunks, 4, &direct) != 1 || direct != 0)
        return 0;
    *thunkOut = thunks[0];
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!CodeSection(&codeRva, &codeSize) || codeSize < 5)
        return 0;
    const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    uint32_t site = 0;
    int n = 0;
    for (uint32_t i = 0; i + 5 <= codeSize; ++i)
    {
        if (p[i] != 0xE8)
            continue;
        int32_t rel = 0;
        memcpy(&rel, p + i + 1, 4);
        if (static_cast<uint32_t>(static_cast<int64_t>(codeRva + i) + 5 + rel) == thunks[0])
        {
            site = codeRva + i;
            ++n;
        }
    }
    return n == 1 ? site : 0;
}

void InstallNames34()
{
    if (!g_namesWanted)
    {
        Log("[names34] [Splitscreen] NamesP34=0 - players 3 and 4 get no name tags (as the game has it)");
        return;
    }
    if (!g_playersOn)
    {
        Log("[names34] NOT installed: Players is off (2 players)");
        return;
    }
    g_namesCharOn = true; // (3) needs nothing else
    if (!g_viewsReady || !g_hudOn)
    {
        Log("[names34] only players 3 / 4's sign-in names: Views %s, players 3 / 4's HUD %s (the name tasks go in its roots)",
            g_viewsReady ? "on" : "off", g_hudOn ? "on" : "off");
        return;
    }
    g_nameCreate = FindOne("name display create", kNameCreateBody);
    g_nameRun = FindOne("name display run", kNameRunHead);
    g_nameArray = g_nameCreate ? OperandRva(g_nameCreate + kNameCreateArray) : 0;
    int boundHits = 0;
    const uint32_t bound = FindNameBound(g_nameRun, &boundHits);
    uint32_t setup = g_nameArray && g_rootIndexRva ? FindNameSetup() : 0;
    // the setup must be what the level load calls right after the create (GOG 1.3 0x0013D7D1 / 0x0013DCA1, one function): a
    // start the anchors found in a merged span (no int3 before the setup) would be another function's
    if (setup)
    {
        uint32_t ct = 0;
        uint32_t st = 0;
        const uint32_t createCall = g_nameCreate ? NameOneCallSite(g_nameCreate, &ct) : 0;
        const uint32_t setupCall = NameOneCallSite(setup, &st);
        const bool order = createCall && setupCall && setupCall > createCall && setupCall - createCall < 0x800;
        Log("[names34] the setup rva 0x%08X: its thunk rva 0x%08X called once at rva 0x%08X; the create's thunk rva 0x%08X called once at rva "
            "0x%08X - %s",
            setup, st, setupCall, ct, createCall,
            order ? "the setup is called right after the create (the level load): OK" : "NOT called right after the create - NOT used");
        if (!order)
            setup = 0;
    }
    Log("[names34] name displays: create rva 0x%08X, NameDisplayThread [2] at rva 0x%08X, run rva 0x%08X, its loop end (call "
        "count; push eax; call max) %d site(s) at rva 0x%08X (+%Xh), setup rva 0x%08X (by its anchors; RootIndex rva 0x%08X, its "
        "thunk rva 0x%08X) (GOG 1.3: 0x00195D50, 0x004C14D0, 0x00196050, +B5h, 0x00195E40)",
        g_nameCreate, g_nameArray, g_nameRun, boundHits, bound, bound ? bound - g_nameRun : 0u, setup, g_rootIndexRva, g_rootIndexThunk);
    // (1) the run's loop end: its call's rel32 to NameBoundWrap (the call goes through a thunk shared by the game's camera
    // count callers: only this call site changes)
    if (bound)
    {
        g_nameOrigCount = reinterpret_cast<NameCount_t>(Rva(RelTarget(bound)));
        unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(bound));
        unsigned char code[5] = { 0xE8, 0, 0, 0, 0 };
        const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&NameBoundWrap) - (reinterpret_cast<uintptr_t>(at) + 5));
        memcpy(code + 1, &rel, 4);
        g_namesBoundOn = at[0] == 0xE8 && WriteCode(reinterpret_cast<uintptr_t>(at), code, sizeof(code));
    }
    // (2) the setup's thunk (the game's create is called directly from it)
    bool moved = false;
    if (g_nameCreate && setup && g_nameArray)
    {
        g_namesDispOn = true; // before the thunk moves: from its next call on, ours are made and set up
        moved = MoveThunk("name display setup", setup, reinterpret_cast<const void*>(&NameSetupWrap), &g_nameOrigSetup);
        g_namesDispOn = moved;
    }
    Log("[names34] players 3 and 4's name tags: (1) the name loop's end %s; (2) name tasks for views 3 / 4 %s; (3) their characters "
        "get the sign-in's names (BF1GameExt_split.lua r33's \"signname\")",
        g_namesBoundOn ? "PATCHED (players 3 / 4's characters are looked at in every view)" : "NOT patched (not found)",
        moved ? "ON (the game's create and setup run once more for them at each match start, in views 3 / 4's HUD roots)"
              : "NOT on (the setup or the create not found, or its thunk not moved - the lines above)");
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r33: THE WATER SCAN (log only; [Splitscreen] WaterScan=1; [water34] lines)
// ---------------------------------------------------------------------------------------------------------
// The user's 4-player shots: the water in views 3 / 4 smeared and its reflections doubled; views 1 / 2 right. The renderer
// (a 4-view renderer, 20b) does the water's work per view, in its view loop (GOG 1.3 0x0028EB60) with the view's camera:
//   the reflection pass 0x0028F680 (+50h the reflection target [0x006AB3D4], a 512 x 256 render texture; the level's
//     reflection regions collected for this camera, count [0x006AB4AC], each one's scene drawn mirrored into it),
//   the water's draw 0x002F5990 (the water patches, count [0x006AFBCC]; its shader gets the reflection target and the
//     screen texture [0x005F7DA4]),
//   the refraction copy 0x00291CA0 (the view's part of the back buffer - the viewport - copied into render texture 2).
// Each has one jump thunk, one call (GOG 1.3 0x0028ED77 / 0x002F4527 / 0x0028EE9E). This scan moves the three thunks
// through BF1GameExt: each runs as before, inside a render_hooks.cpp phase (the view = the renderer's current view):
// what each pass drew into is noted, and every water draw's textures are counted against it - written in its own view
// this frame (right), in another view this frame or only in an earlier frame (what views 3 / 4 would show if their pass
// drew nothing or something else). Every 20 s while water is drawn: a line per view (passes, regions, draws, patches,
// copies and their viewport, the water draws' textures by who wrote them). At the start and every 60 s: each view's next
// water draw in full (render_hooks.cpp: stages, sizes, writers, viewport, render target, vertex shader c0 - c31).
const char* const kWaterReflBody =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 53 81 EC 08 "
    "04 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 EC 56 57 50 8D 45 F4 64 A3 00 00 00 00 83 3D ?? ?? ?? ?? 00 0F 86 ?? ?? ?? ?? 8B 35";
constexpr uint32_t kWaterReflTarget = 0x50;   // mov esi, [reflection target]
const char* const kWaterReflCount = "6A 01 C7 05 ?? ?? ?? ?? 00 00 00 00 E8";
constexpr uint32_t kWaterReflCountAt = 0x299; // push 1; mov [regions], 0; call (the count is filled after it)
const char* const kWaterDrawBody =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 53 81 EC B8 "
    "01 00 00 A1 ?? ?? ?? ?? 33 C5 89 45 EC 56 57 50 8D 45 F4 64 A3 00 00 00 00 68 ?? ?? ?? ?? 8D 8D 60 FF FF FF E8 ?? ?? ?? "
    "?? 68 ?? ?? ?? ?? 8D 4D A0 E8 ?? ?? ?? ?? 6A 01 6A 03 8D 4D A0 E8";
const char* const kWaterDrawPatches = "83 3D ?? ?? ?? ?? 00 A3 ?? ?? ?? ?? 0F 8E";
constexpr uint32_t kWaterDrawPatchesAt = 0x132; // cmp [patches], 0; mov [..], eax; jle
const char* const kWaterCopyBody =
    "55 8B EC 83 EC 3C A1 ?? ?? ?? ?? 33 C5 89 45 FC E8 ?? ?? ?? ?? F6 80 D4 00 00 00 10 0F 84 ?? ?? ?? ?? E8 ?? ?? ?? ?? 80 "
    "78 1E 00 0F 84 ?? ?? ?? ?? 8D 45 C4 50";
const char* const kWaterViewportBody = "55 8B EC A1 ?? ?? ?? ?? 66 0F 6E C0 F3 0F E6 C0"; // the renderer's viewport getter
constexpr uint32_t kWaterCopyViewportCall = 0x41; // call GetViewport(&x0, &y0, &x1, &y1)

bool g_waterWanted = true;  // [Splitscreen] WaterScan
bool g_waterOn = false;
uintptr_t g_waterOrig[3] = {};
uintptr_t g_waterRegionsVa = 0;
uintptr_t g_waterPatchesVa = 0;
uintptr_t g_waterTargetVa = 0;
uintptr_t g_waterViewportVa = 0; // the renderer's viewport: x, y, w, h (ints)
struct WaterView
{
    long refl;        // reflection passes
    long regions;     // regions drawn, summed / most in one pass
    long regionsMax;
    long reflDraws;   // draw calls in them
    long water;       // water draws
    long patches;     // patches, summed
    long waterDraws;  // draw calls in them
    long copies;      // refraction copies
    long copyX, copyY, copyW, copyH; // the last copy's viewport
};
WaterView g_wv[4] = {};
DWORD g_waterNextSum = 0;
DWORD g_waterNextDetail = 0;
int g_waterSumLines = 0;
int g_waterShotSums = 0;         // 21j-r34: summaries with water in 3 / 4 views (the shot goes at the second)
char g_waterShotWhy[64] = "";      // (kept: the shot's line reads it at the next Present)
long g_waterStaleSeen[4][3] = {};

int WaterCurrentView()
{
    if (!g_rendCams)
        return -1;
    const int v = static_cast<int>(ReadU32(g_rendCams + 0x10));
    return (v >= 0 && v < 4) ? v : -1;
}

void WaterSummary()
{
    const DWORD now = GetTickCount();
    if (!g_waterNextSum)
    {
        g_waterNextSum = now + 20000;
        g_waterNextDetail = now; // the first water draws in full at once
    }
    if (static_cast<LONG>(now - g_waterNextDetail) >= 0)
    {
        g_waterNextDetail = now + 60000;
        for (int v = 0; v < 4; ++v)
            FrameWaterDetail(v);
    }
    if (static_cast<LONG>(now - g_waterNextSum) < 0)
        return;
    g_waterNextSum = now + 20000;
    bool any = false;
    int withWater = 0;
    for (int v = 0; v < 4; ++v)
    {
        any = any || g_wv[v].water || g_wv[v].refl;
        withWater += g_wv[v].water ? 1 : 0;
    }
    // 21j-r34: one water shot by itself (render_hooks.cpp: the pictures each view's water reads, BF1GameExt_water\) at the
    // second summary that has water in 3 / 4 views (40 s in - the players are out of their spawn screens by then, mostly)
    if (withWater >= 3 && g_waterShotSums < 2 && ++g_waterShotSums == 2)
    {
        snprintf(g_waterShotWhy, sizeof(g_waterShotWhy), "by itself: water in %d views for 40 s", withWater);
        FrameWaterShotArm(g_waterShotWhy);
    }
    if (!any || g_waterSumLines >= 400)
        return;
    for (int v = 0; v < 4; ++v)
    {
        const WaterView& w = g_wv[v];
        if (v >= g_viewsLive && !w.water && !w.refl && !w.copies) // (a view of this match with nothing is logged too)
            continue;
        long st[3] = {};
        FrameWaterStale(v, &st[0], &st[1], &st[2]);
        const long dOther = st[0] - g_waterStaleSeen[v][0];
        const long dOld = st[1] - g_waterStaleSeen[v][1];
        const long dOwn = st[2] - g_waterStaleSeen[v][2];
        for (int i = 0; i < 3; ++i)
            g_waterStaleSeen[v][i] = st[i];
        ++g_waterSumLines;
        Log("[water34] %lu ms, last 20 s, view %d: reflection passes %ld (regions %.2f a pass, most %ld; %.1f draws a pass), water "
            "draws %ld (%.1f patches, %.1f draw calls each), refraction copies %ld (the last from x %ld y %ld w %ld h %ld); the "
            "water's draw calls sampled: their own view's pass / copy of this frame %ld, ANOTHER VIEW'S of this frame %ld, only "
            "an older frame's %ld",
            ScanMsSinceAttach(), v + 1, w.refl, w.refl ? static_cast<double>(w.regions) / static_cast<double>(w.refl) : 0.0,
            w.regionsMax, w.refl ? static_cast<double>(w.reflDraws) / static_cast<double>(w.refl) : 0.0, w.water,
            w.water ? static_cast<double>(w.patches) / static_cast<double>(w.water) : 0.0,
            w.water ? static_cast<double>(w.waterDraws) / static_cast<double>(w.water) : 0.0, w.copies, w.copyX, w.copyY,
            w.copyW, w.copyH, dOwn, dOther, dOld);
    }
    memset(g_wv, 0, sizeof(g_wv));
}

typedef void(__cdecl* WaterVoid_t)();

void __cdecl WaterReflWrap()
{
    const int v = WaterCurrentView();
    const long d0 = FrameDrawCalls();
    FrameWaterBegin(1, v);
    reinterpret_cast<WaterVoid_t>(g_waterOrig[0])();
    FrameWaterEnd();
    if (v < 0)
        return;
    const long n = static_cast<long>(ReadU32(g_waterRegionsVa));
    WaterView& w = g_wv[v];
    ++w.refl;
    w.regions += n;
    if (n > w.regionsMax)
        w.regionsMax = n;
    w.reflDraws += FrameDrawCalls() - d0;
}

void __cdecl WaterDrawWrap()
{
    const int v = WaterCurrentView();
    const long d0 = FrameDrawCalls();
    WaterSummary();
    FrameWaterBegin(2, v);
    reinterpret_cast<WaterVoid_t>(g_waterOrig[1])();
    FrameWaterEnd();
    if (v < 0)
        return;
    WaterView& w = g_wv[v];
    ++w.water;
    w.patches += static_cast<long>(ReadU32(g_waterPatchesVa));
    w.waterDraws += FrameDrawCalls() - d0;
}

void __cdecl WaterCopyWrap()
{
    const int v = WaterCurrentView();
    FrameWaterBegin(3, v);
    reinterpret_cast<WaterVoid_t>(g_waterOrig[2])();
    FrameWaterEnd();
    if (v < 0)
        return;
    WaterView& w = g_wv[v];
    ++w.copies;
    w.copyX = static_cast<long>(ReadU32(g_waterViewportVa));
    w.copyY = static_cast<long>(ReadU32(g_waterViewportVa + 4));
    w.copyW = static_cast<long>(ReadU32(g_waterViewportVa + 8));
    w.copyH = static_cast<long>(ReadU32(g_waterViewportVa + 12));
}

void InstallWaterScan()
{
    if (!g_waterWanted)
    {
        Log("[water34] [Splitscreen] WaterScan=0 - no water scan");
        return;
    }
    if (!g_viewsReady || !g_rendCams)
    {
        Log("[water34] the water scan NOT installed: the 4 views are not on");
        return;
    }
    const uint32_t refl = FindOne("water: the reflection pass", kWaterReflBody);
    const uint32_t draw = FindOne("water: the water draw", kWaterDrawBody);
    const uint32_t copy = FindOne("water: the refraction copy", kWaterCopyBody);
    bool ok = refl && draw && copy && MatchPatternAt(refl + kWaterReflCountAt, kWaterReflCount) &&
              MatchPatternAt(draw + kWaterDrawPatchesAt, kWaterDrawPatches) &&
              MatchPatternAt(CallTarget(copy + kWaterCopyViewportCall), kWaterViewportBody);
    const uint32_t regions = ok ? OperandRva(refl + kWaterReflCountAt + 4) : 0;
    const uint32_t patches = ok ? OperandRva(draw + kWaterDrawPatchesAt + 2) : 0;
    const uint32_t target = ok ? OperandRva(refl + kWaterReflTarget) : 0;
    const uint32_t viewport = ok ? OperandRva(CallTarget(copy + kWaterCopyViewportCall) + 4) : 0;
    ok = ok && regions && patches && target && viewport;
    Log("[water34] the water's per-view functions: reflection pass rva 0x%08X (regions count rva 0x%08X, the reflection target rva "
        "0x%08X), water draw rva 0x%08X (patches rva 0x%08X), refraction copy rva 0x%08X (the renderer's viewport rva 0x%08X) "
        "(GOG 1.3: 0x0028F680 / 0x006AB4AC / 0x006AB3D4, 0x002F5990 / 0x006AFBCC, 0x00291CA0 / 0x005F7598) - %s",
        refl, regions, target, draw, patches, copy, viewport, ok ? "every byte checks out" : "does NOT check out - no scan");
    if (!ok)
        return;
    g_waterRegionsVa = Rva(regions);
    g_waterPatchesVa = Rva(patches);
    g_waterTargetVa = Rva(target);
    g_waterViewportVa = Rva(viewport);
    const bool m0 = MoveThunk("water: the reflection pass", refl, reinterpret_cast<const void*>(&WaterReflWrap), &g_waterOrig[0]);
    const bool m1 = m0 && MoveThunk("water: the water draw", draw, reinterpret_cast<const void*>(&WaterDrawWrap), &g_waterOrig[1]);
    const bool m2 = m1 && MoveThunk("water: the refraction copy", copy, reinterpret_cast<const void*>(&WaterCopyWrap), &g_waterOrig[2]);
    g_waterOn = m2;
    if (m2)
        FrameWaterScanOn(reinterpret_cast<const volatile int*>(g_rendCams + 0x10)); // the renderer's current view
    Log("[water34] the water scan %s (log only: each view's reflection pass, water draw and refraction copy run as before)",
        m2 ? "ON" : (m0 ? "PARTLY on (a thunk could not be moved; the moved ones still run the game's own)" : "NOT on"));
    if (m0)
        Log("[water34] 21j-r34: water shots ON - the pictures each view's water reads, saved for one frame in BF1GameExt_water "
            "next to the exe: one by itself 40 s after water is in 3 / 4 views, one more at each press of the capture key (Insert, "
            "Home or LB+RB+Y on pad 1), 4 at most ([water34] shot lines)");
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r36: THE WATER FIX ([Splitscreen] WaterFix: 0 off, 1 = 3 / 4-player matches, 2 = every split screen; [water34] lines)
// ---------------------------------------------------------------------------------------------------------
// r35's shot and its shaders (the user's BF1GameExt_water folder) found it. The water's second pass (the game's "high"
// water) first draws the water's wave normals from the view's camera into the 2048 x 2048 screen texture 0 (the ripple
// pre-pass, GOG 1.3 0x002F4AD0: SetRenderTarget(texture 0), then SetViewport(the view's rect) - for a split view only its
// share: a quarter, a half), then draws the water again reading those normals back for its highlights and its fade
// (vertex shader: oT0 = (0.5 x + 0.5 w, -0.5 y + 0.5 w, 0, w) from c31 = (0, 0.5, -0.5, 0) - the view's own 0 - 1 over the
// WHOLE texture, no rect in it; pixel shader: dp3 of t0 with the half vector / the view vector). One player: the pre-pass
// fills the whole texture and that matches. Split: each view's water read all four quarters stretched over itself - its own
// normals in its top left, the other views' (this frame's or last frame's) and empty quarters in the rest: the waves and
// highlights stop at the middle of the view (an empty quarter below - r35's video, view 2), doubled / smeared where another
// view's normals are read (views 3 / 4's shots). The reflection is right (its own 512 x 256 texture per view, read the same
// way). Now the pre-pass draws over the whole texture for each split view (SetViewport(0, 0, 1, 1) at that one call; its
// own end puts the view's rect back from its saved copy, as before): the view's water reads its own normals everywhere -
// as one player's. The views draw one after the other, each its pre-pass right before its own water, so nothing else is
// changed (the blur's corner copy and the scope blur come after each view's water, as before).
const char* const kWaterPrePassSite =
    "8D 45 ?? 50 E8 ?? ?? ?? ?? 83 C4 04 6A 00 6A 00 E8 ?? ?? ?? ?? 83 C4 04 50 E8 ?? ?? ?? ?? 8D 45 ?? 50 E8 ?? ?? ?? ?? 83 C4 0C";
constexpr uint32_t kWaterPreGetRect = 0x04;   // call GetRect(&saved)       (the view's rect, 16 bytes)
constexpr uint32_t kWaterPreScreenTex = 0x10; // push 0; call ScreenTexture(0) (the 2048 x 2048 texture)
constexpr uint32_t kWaterPreSetRt = 0x19;     // call SetRenderTarget(it, 0)
constexpr uint32_t kWaterPreSetVp = 0x22;     // lea eax, [saved]; push eax; call SetViewportFromRect(&saved)  <- this call
const char* const kWaterGetRectBody = "55 8B EC 8B 45 08 0F 10 05 ?? ?? ?? ?? 0F 11 00 5D C3";
const char* const kWaterScreenTexBody = "55 8B EC 8B 45 08 8B 04 85 ?? ?? ?? ?? 5D C3";
const char* const kWaterSetRtBody = "55 8B EC 83 EC 28 A1 ?? ?? ?? ?? 33 C5 89 45 FC 56 8B 75 0C 57 8B 7D 08 85 F6 74";
const char* const kWaterSetVpBody =
    "55 8B EC 83 EC 30 A1 ?? ?? ?? ?? 33 C5 89 45 FC A1 ?? ?? ?? ?? 8D 55 D8 56 8B 75 08 52 8B 08 6A 00 50 FF 91 98 00 00 00";

typedef void(__cdecl* WaterSetVp_t)(const float* rect);
int g_waterFixMode = 1;           // [Splitscreen] WaterFix
bool g_waterFixOn = false;
WaterSetVp_t g_waterFixOrig = nullptr;
alignas(16) float g_waterFullRect[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
volatile LONG g_waterFixCalls = 0;
volatile LONG g_waterFixWhole = 0;
int g_waterFixLines = 0;

// the pre-pass's SetViewportFromRect(&the view's rect): the whole texture for a split view
void __cdecl WaterPrePassViewport(const float* rect)
{
    InterlockedIncrement(&g_waterFixCalls);
    const bool split = rect && (rect[2] < 0.999f || rect[3] < 0.999f);
    const bool on = split && (g_waterFixMode >= 2 || (g_waterFixMode == 1 && g_playersOn && g_players >= 3));
    if (!on)
    {
        g_waterFixOrig(rect);
        return;
    }
    const LONG n = InterlockedIncrement(&g_waterFixWhole);
    if ((n <= 4 || n % 36000 == 0) && g_waterFixLines < 40)
    {
        ++g_waterFixLines;
        Log("[water34] %lu ms: the water's ripple pre-pass for a split view (its rect %.2f %.2f %.2f %.2f) drawn over the whole "
            "texture (%ld time(s); %ld pre-passes in all) - its water reads its own wave normals everywhere",
            ScanMsSinceAttach(), static_cast<double>(rect[0]), static_cast<double>(rect[1]), static_cast<double>(rect[2]),
            static_cast<double>(rect[3]), static_cast<long>(n), static_cast<long>(g_waterFixCalls));
    }
    g_waterFixOrig(g_waterFullRect);
}

void InstallWaterFix()
{
    if (g_waterFixMode <= 0)
    {
        Log("[water34] [Splitscreen] WaterFix=0 - the water's ripple pre-pass as the game has it (split views read all of it)");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kWaterPrePassSite, hits, 4);
    const uint32_t site = n == 1 ? hits[0] : 0;
    bool ok = site != 0;
    uint32_t getRect = 0, screenTex = 0, setRt = 0, setVp = 0;
    if (ok)
    {
        getRect = CallTarget(site + kWaterPreGetRect);
        screenTex = CallTarget(site + kWaterPreScreenTex);
        setRt = CallTarget(site + kWaterPreSetRt);
        setVp = CallTarget(site + kWaterPreSetVp);
        ok = CodeByte(site + 2) == CodeByte(site + kWaterPreSetVp - 2) && // the same saved rect, [ebp+d8]
             MatchPatternAt(getRect, kWaterGetRectBody) && MatchPatternAt(screenTex, kWaterScreenTexBody) &&
             MatchPatternAt(setRt, kWaterSetRtBody) && MatchPatternAt(setVp, kWaterSetVpBody) &&
             CodeByte(site + kWaterPreSetVp) == 0xE8;
    }
    Log("[water34] 21j-r36: the water's ripple pre-pass: %d hit(s) at rva 0x%08X (GOG 1.3 0x002F4B69); GetRect rva 0x%08X, the screen "
        "texture getter rva 0x%08X (index 0), SetRenderTarget rva 0x%08X, SetViewportFromRect rva 0x%08X (GOG 1.3 0x00275D70 / "
        "0x00275A10 / 0x00276DC0 / 0x00277410) - %s",
        n, site, getRect, screenTex, setRt, setVp, ok ? "every byte checks out" : "does NOT check out - NOT installed");
    if (!ok)
        return;
    const uint32_t callAt = site + kWaterPreSetVp;
    g_waterFixOrig = reinterpret_cast<WaterSetVp_t>(Rva(RelTarget(callAt))); // (through its thunk, as the game calls it)
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(callAt));
    unsigned char code[5] = { 0xE8, 0, 0, 0, 0 };
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&WaterPrePassViewport) - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(code + 1, &rel, 4);
    g_waterFixOn = WriteCode(reinterpret_cast<uintptr_t>(at), code, sizeof(code));
    Log("[water34] 21j-r36: THE WATER FIX %s ([Splitscreen] WaterFix=%d: %s) - each split view's ripple pre-pass draws over the whole "
        "texture its water reads (its highlights and waves everywhere, as one player's; the game drew it in the view's share only)",
        g_waterFixOn ? "ON" : "NOT on (the call could not be written)", g_waterFixMode,
        g_waterFixMode >= 2 ? "every split screen, 2 players too" : "3 / 4-player matches");
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r37: THE UNDERWATER TINT ([Splitscreen] UnderwaterFix: 0 off, 1 = 3 / 4-player matches, 2 = every split screen; [water34])
// ---------------------------------------------------------------------------------------------------------
// r36's video: player 1's view goes dark green under water, players 2 - 4's never do (their seabed as clear as on land). The
// tint is the game's underwater overlay (UnderWaterRenderable, drawn in every view with that view's camera, GOG 1.3
// 0x0029B790): the screen's part below the water line as a polygon, drawn by 0x002F45F0 as pre-transformed points - pixels
// of the render target. That function asks for the view's viewport (0x00275D90: x0, y0, x1, y1) but uses only its size:
// x = (0.5 x + 0.5) * (x1 - x0), y = (0.5 - 0.5 y) * (y1 - y0) - the view's left / top edge is never added. View 1 (0, 0) is
// right; every other view's polygon lands where view 1 is and is cut off at its own viewport - nothing shows (the game's
// 2-player split has it too: player 2's half). Each point of the overlay now gets the viewport's left / top edge added (the
// game's own SetPoint for those points, 0x0005D7B0, called with the moved x / y) - its polygon lies over its own view. View 1
// and one player (edge 0, 0) are never touched.
const char* const kUwOverlayHead =
    "55 8B EC 83 EC 14 53 8B 5D 0C 56 6A 00 6A 00 53 6A 10 6A 03 6A 06 E8 ?? ?? ?? ?? 8B F0 83 C4 18 8B CE 89 75 EC "
    "E8 ?? ?? ?? ?? 89 45 0C 8D 45 FC 50 8D 45 F4 50 8D 45 F8 50 8D 45 F0 50 E8 ?? ?? ?? ?? 83 C4 10 85 DB 0F 8E";
constexpr uint32_t kUwHeadGetVp = 0x3D;   // call GetViewportPixels(&x0, &y0, &x1, &y1)
constexpr uint32_t kUwHeadLoopMax = 0x140; // the point loop's pattern starts within this of the head
constexpr uint32_t kUwScanMax = 0x180;     // 21j-r38: the function's length at most (GOG 1.3: 0x12A, its end CC CC)
// the point loop: y = (0.5 - 0.5 y) * (y1 - y0), x = (0.5 x + 0.5) * (x1 - x0) - no x0 / y0 - then SetPoint(x, y, 0, 1)
// 21j-r38: r37's pattern was 138 bytes - the pattern reader takes 128 at most, so it was refused and found nothing, on
// every exe (r37's own mistake: the byte-check did not hold patterns to that limit; it does now). Now from the height
// subtraction on (86 bytes; the same bytes on GOG 1.3 and Steam - Steam's exe decrypted for the byte-check), and the SetPoint
// call is also found by its target: both must be the same call.
const char* const kUwLoop =
    "F3 0F 10 45 FC F3 0F 5C 45 F8 F3 0F 59 C8 F3 0F 11 0C 24 E8 ?? ?? ?? ?? F3 0F 10 45 F4 F3 0F 5C 45 F0 D9 5D 0C "
    "F3 0F 10 4D 0C F3 0F 59 0D ?? ?? ?? ?? 51 8B CE F3 0F 58 0D ?? ?? ?? ?? F3 0F 59 C8 F3 0F 11 0C 24 E8 ?? ?? ?? "
    "?? 83 C7 0C 83 C6 10 83 EB 01 0F 85";
constexpr uint32_t kUwLoopSetPoint = 0x46; // call SetPoint (thiscall: the point, x, y, z, w)  <- this call
const char* const kUwSetPointBody =
    "55 8B EC F3 0F 10 45 08 F3 0F 11 01 F3 0F 10 45 0C F3 0F 11 41 04 F3 0F 10 45 10 F3 0F 11 41 08 F3 0F 10 45 14 F3 0F 11 41 "
    "0C 5D C2 10 00";
const char* const kUwGetVpBody =
    "55 8B EC A1 ?? ?? ?? ?? 66 0F 6E C0 F3 0F E6 C0 C1 E8 1F F2 0F 58 04 C5 ?? ?? ?? ?? 8B 45 08 66 0F 5A C0 F3 0F 11 00 A1";
constexpr uint32_t kUwGetVpX = 0x04; // (mov eax, [) the viewport's X  (the game's D3D viewport: X, Y, W, H in pixels)
constexpr uint32_t kUwGetVpY = 0x28; // (mov eax, [) the viewport's Y

typedef void(__fastcall* UwSetPoint_t)(float* point, void* edx, float x, float y, float z, float w);
int g_uwFixMode = 1;                 // [Splitscreen] UnderwaterFix
bool g_uwFixOn = false;
UwSetPoint_t g_uwOrig = nullptr;
const volatile int32_t* g_uwViewport = nullptr;
volatile LONG g_uwPoints = 0;
volatile LONG g_uwMoved = 0;
volatile LONG g_uwViewPoints[4] = {};
int g_uwLines = 0;

// the overlay's SetPoint(x, y, 0, 1): x / y moved by the view's viewport edge
void __fastcall UnderwaterPointWrap(float* point, void* edx, float x, float y, float z, float w)
{
    InterlockedIncrement(&g_uwPoints);
    const int32_t vx = g_uwViewport[0];
    const int32_t vy = g_uwViewport[1];
    const bool edge = vx != 0 || vy != 0;
    const bool on = edge && z == 0.0f && w == 1.0f && // (the overlay's points: z 0, w 1)
                    (g_uwFixMode >= 2 || (g_uwFixMode == 1 && g_playersOn && g_players >= 3));
    const int view = g_rendCams ? static_cast<int>(ReadU32(g_rendCams + 0x10)) : -1;
    const LONG nv = (view >= 0 && view < 4) ? InterlockedIncrement(&g_uwViewPoints[view]) : 0;
    const LONG nm = on ? InterlockedIncrement(&g_uwMoved) : 0;
    if ((nv == 1 || nv == 2 || (nm != 0 && nm % 20000 == 0)) && g_uwLines < 40)
    {
        ++g_uwLines;
        const char* const what = on ? "moved onto its own view"
                                 : edge ? "left where the game puts it (UnderwaterFix=1: a 2-player match as the game has it)"
                                        : "its viewport starts at the screen's corner - nothing to move";
        Log("[water34] %lu ms: view %d's underwater tint (the overlay below its water line): the game's point %.1f %.1f, its viewport "
            "%ld %ld, %ld x %ld - %s (%ld point(s) in this view, %ld moved in all)",
            ScanMsSinceAttach(), view + 1, static_cast<double>(x), static_cast<double>(y), static_cast<long>(vx),
            static_cast<long>(vy), static_cast<long>(g_uwViewport[2]), static_cast<long>(g_uwViewport[3]), what,
            static_cast<long>(nv), static_cast<long>(g_uwMoved));
    }
    if (on)
    {
        x += static_cast<float>(vx);
        y += static_cast<float>(vy);
    }
    g_uwOrig(point, edx, x, y, z, w);
}

void InstallUnderwaterFix()
{
    if (g_uwFixMode <= 0)
    {
        Log("[water34] [Splitscreen] UnderwaterFix=0 - the underwater tint as the game has it (view 1 only)");
        return;
    }
    uint32_t hits[4] = {};
    const int nh = FindPattern(kUwOverlayHead, hits, 4);
    const uint32_t head = nh == 1 ? hits[0] : 0;
    uint32_t lhits[4] = {};
    const int nl = FindPattern(kUwLoop, lhits, 4);
    const uint32_t loop = nl == 1 ? lhits[0] : 0;
    const bool inHead = head && loop > head && loop - head < kUwHeadLoopMax;
    // 21j-r38: the function's SetPoint calls - a call whose target (through its thunks) is SetPoint's body, before the
    // function's end (CC CC); there must be exactly one (the point loop's)
    uint32_t setAt[4] = {};
    int ns = 0;
    uint32_t fnLen = kUwScanMax;
    for (uint32_t o = 0; head && o + 5 <= kUwScanMax; ++o)
    {
        if (CodeByte(head + o) == 0xCC && CodeByte(head + o + 1) == 0xCC)
        {
            fnLen = o;
            break;
        }
        if (CodeByte(head + o) == 0xE8 && MatchPatternAt(CallTarget(head + o), kUwSetPointBody))
        {
            if (ns < 4)
                setAt[ns] = head + o;
            ++ns;
        }
    }
    const uint32_t callAt = ns == 1 ? setAt[0] : 0;
    const uint32_t getVp = head ? CallTarget(head + kUwHeadGetVp) : 0;
    const uint32_t setPt = callAt ? CallTarget(callAt) : 0;
    bool ok = head && callAt && inHead && callAt == loop + kUwLoopSetPoint && CodeByte(head + kUwHeadGetVp) == 0xE8 &&
              MatchPatternAt(getVp, kUwGetVpBody) && MatchPatternAt(setPt, kUwSetPointBody);
    const uint32_t vpX = ok ? OperandRva(getVp + kUwGetVpX) : 0;
    const uint32_t vpY = ok ? OperandRva(getVp + kUwGetVpY) : 0;
    ok = ok && vpX && vpY == vpX + 4;
    int callers = 0; // (through its thunks, and any direct calls)
    if (head)
    {
        uint32_t th[4] = {};
        const int nt = FindThunks(head, th, 4, &callers);
        for (int k = 0; k < nt; ++k)
            callers += CountCallsTo(th[k]);
    }
    Log("[water34] 21j-r38: the underwater overlay's points: the function %d hit(s) at rva 0x%08X (GOG 1.3 0x002F45F0, %d call(s) to it - "
        "GOG 1.3: 1, the underwater overlay's; 0x%X bytes long, GOG 1.3 0x12A), its SetPoint call(s) %d (need 1) at rva 0x%08X (GOG 1.3 "
        "0x002F46CA, +0xDA) -> SetPoint rva 0x%08X (GOG 1.3 0x0005D7B0), GetViewportPixels rva 0x%08X (GOG 1.3 0x00275D90; the viewport's "
        "X / Y at rva 0x%08X / 0x%08X, GOG 1.3 0x005F7598 / 0x005F759C); its point loop %d hit(s) at rva 0x%08X (GOG 1.3 0x002F4684)%s - %s",
        nh, head, callers, fnLen, ns, callAt, setPt, getVp, vpX, vpY, nl, loop,
        !inHead ? " (NOT in the function)"
                : (callAt == loop + kUwLoopSetPoint ? " (in it: its SetPoint is the one found)" : " (in it, but its SetPoint is NOT the one found)"),
        ok ? "every byte checks out" : "does NOT check out - NOT installed");
    // the function's bytes when it does not check out (for the next byte-check)
    for (uint32_t o = 0; !ok && head && o < fnLen; o += 32)
    {
        char hex[32 * 3 + 1] = {};
        for (uint32_t k = 0; k < 32 && o + k < fnLen; ++k)
            snprintf(hex + k * 3, 4, "%02X ", CodeByte(head + o + k));
        Log("[water34]   the overlay function +%03X: %s", o, hex);
    }
    if (!ok)
        return;
    g_uwViewport = reinterpret_cast<const volatile int32_t*>(Rva(vpX));
    g_uwOrig = reinterpret_cast<UwSetPoint_t>(Rva(RelTarget(callAt))); // (through its thunk, as the game calls it)
    unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(callAt));
    unsigned char code[5] = { 0xE8, 0, 0, 0, 0 };
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&UnderwaterPointWrap) - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(code + 1, &rel, 4);
    g_uwFixOn = WriteCode(reinterpret_cast<uintptr_t>(at), code, sizeof(code));
    Log("[water34] 21j-r38: THE UNDERWATER TINT FIX %s ([Splitscreen] UnderwaterFix=%d: %s) - each view's underwater overlay is drawn "
        "over its own view (the game drew every view's where view 1 is, so only view 1 ever had it)",
        g_uwFixOn ? "ON" : "NOT on (the call could not be written)", g_uwFixMode,
        g_uwFixMode >= 2 ? "every split screen, 2 players too" : "3 / 4-player matches");
}

// ---------------------------------------------------------------------------------------------------------
// 21j-r39: THE 2-PLAYER TOP / BOTTOM SPLIT'S FIELD OF VIEW ([Splitscreen] SplitFovH: 0 the game's, 1 = BF2's rule, 2 = BF2's
// angles; [fovh] lines)
// ---------------------------------------------------------------------------------------------------------
// Battlefront 1 keeps the WIDTH: the chase camera's Apply (GOG 1.3 0x0013AAD0) ends with SetFov(fov, the view's h / w)
// (0x0028AB10: +98h = tan(fov / 2), +9Ch = that x h / w), its 48 deg always across. A top / bottom half (3.56 : 1 at 16:9)
// keeps the 48 deg and loses half the height: 48 x 14 deg, against one player's 48 x 28 - the half shows the middle strip of
// one player's picture. Battlefront II keeps the HEIGHT (its FOV step 0x0044E540, Hor+): BF2's 2-player halves are 51 deg
// tall and 119 deg wide (its 15v log: tan x 1.6989, tan y 0.4778), the same height as one player. Now, for the two halves of
// the 2-player top / bottom split only (views wider than the screen's own shape; side by side, quarters and one player never
// change), the FOV the Apply hands SetFov is widened: SplitFovH=1 - one player's height (Hor+, BF2's rule: x 2 at the game's
// 48 deg, 83 x 28 deg at 16:9), 2 - SplitFovTall degrees tall (21j-r41; r39 / r40: BF2's own 51 - 119 x 51 deg at 16:9, too
// wide for Mr. Rogue; 45 by default: 112 x 45), for every FOV the Apply gives in
// proportion (vehicles, weapons; the zoom divides it afterwards, so a scope keeps its magnification). The Apply eases its FOV
// toward the new one from the one it reads back (0x0028AA70, right before): that read gives the game's own value back while
// the camera still holds ours, so the easing never feeds on the wider view.
const char* const kSfhSite = // the Apply's end: read back the FOV, ease it, SetFov(eased, h / w), return
    "F3 0F 10 45 FC 83 EC 08 8B CE F3 0F 11 44 24 04 F3 0F 10 45 F4 F3 0F 11 04 24 E8 ?? ?? ?? ?? F3 0F 10 45 DC 83 EC 08 D9 "
    "5C 24 04 F3 0F 11 04 24 E8 ?? ?? ?? ?? 83 C4 08 8B CE D9 1C 24 E8 ?? ?? ?? ?? 5F 5E 8B E5 5D 8B E3 5B C2 08 00";
constexpr uint32_t kSfhGetFovCall = 0x1A; // call GetFov (thiscall: 2 atan(+98h))
constexpr uint32_t kSfhLerpCall = 0x30;   // call Lerp(target, now, t)
constexpr uint32_t kSfhSetFovCall = 0x3D; // call SetFov (thiscall: fov, h / w)
constexpr uint32_t kSfhApplyMax = 0x800;  // the site is this far into the Apply at most (GOG 1.3: +671h)
const char* const kSfhGetFovBody =
    "55 8B EC 51 F3 0F 10 81 98 00 00 00 51 F3 0F 11 04 24 E8 ?? ?? ?? ?? D9 5D FC D9 45 FC 83 C4 04 DC C0 8B E5 5D C3";
const char* const kSfhLerpBody =
    "55 8B EC F3 0F 10 4D 0C F3 0F 5C 4D 08 F3 0F 59 4D 10 F3 0F 58 4D 08 F3 0F 11 4D 0C D9 45 0C 5D C3";
constexpr float kSfhBaseTan = 0.44522869f; // tan(24 deg): the game's 48 deg
constexpr float kSfhBf2TallDeg = 51.0f;    // BF2's 2-player half: 51 deg tall (its 15f log: tan y 0.4778)
constexpr float kSfhMaxFactor = 6.0f;

typedef float(__fastcall* SfhGetFov_t)(void* cam, void* edx);
typedef void(__fastcall* SfhSetFov_t)(void* cam, void* edx, float fov, float ratio);
int g_sfhMode = 0;                   // [Splitscreen] SplitFovH (21j-r42: 0 by default - the game's shape at SplitFovScale %)
int g_sfhPct = 110;                  // [Splitscreen] SplitFovScale (21j-r42): the game's FOV in the top / bottom halves, percent
int g_sfhTallDeg = 45;               // [Splitscreen] SplitFovTall (21j-r41): the halves' height with SplitFovH / SplitFovP3 = 2
float g_sfhTallTan = 0.41421356f;    // tan(SplitFovTall / 2)
int g_sfhP3Mode = 0;                 // [Splitscreen] SplitFovP3 (21j-r40: the 3-player bottom half; 21j-r42: 0 by default)
float g_sfhP3LastF = 0.0f;
int g_sfhP3Lines = 0;
volatile LONG g_sfhP3Wide = 0;
bool g_sfhOn = false;
SfhGetFov_t g_sfhGetFov = nullptr;
SfhSetFov_t g_sfhSetFov = nullptr;
uint32_t g_sfhSetFovAt = 0;          // the patched SetFov call (rva) and the game's SetFov behind it (rva, thunks followed)
uint32_t g_sfhSetFovRva = 0;
struct SfhCam
{
    void* cam;
    float base;  // the FOV the game handed SetFov
    float wrote; // +98h after ours
    float f;     // the factor on tan(fov / 2)
};
SfhCam g_sfhCams[4] = {};
volatile LONG g_sfhWide = 0;
volatile LONG g_sfhBack = 0;
int g_sfhLines = 0;
float g_sfhLastF[2] = {};

// the view a camera draws (0 / 1 in the 2-view split) and its rect (x, y, w, h); -1 for anything else
int SfhView(const void* cam, float rect[4])
{
    const uintptr_t cams = g_viewCamerasRva ? Rva(g_viewCamerasRva) : 0;
    if (!cams || ReadU32(cams + 0x14) != 2) // the renderer's view count: the 2-player split only
        return -1;
    for (int v = 0; v < 2; ++v)
        if (ReadU32(cams + 4u * static_cast<uint32_t>(v)) == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(cam)))
        {
            memcpy(rect, reinterpret_cast<const void*>(cams - 0x40 + 16u * static_cast<uint32_t>(v)), 16);
            return v;
        }
    return -1;
}

// the factor on tan(fov / 2) for a view (rect x, y, w, h; ratio = its own h / w as the game gives a camera that shape)
float SfhFactorRect(const float r[4], float ratio, int mode)
{
    if (mode <= 0 || !(r[3] > 0.05f && r[2] > r[3] * 1.01f) || !(ratio > 0.02f && ratio < 10.0f))
        return 1.0f; // not a top / bottom half (side by side and quarters are as tall as wide as the screen, or taller)
    float f = mode == 1 ? r[2] / r[3]                        // one player's height: tan y = tan x x the screen's h / w
                        : (g_sfhTallTan / ratio) / kSfhBaseTan; // SplitFovTall degrees tall at the game's 48 deg
    if (f > kSfhMaxFactor)
        f = kSfhMaxFactor;
    return f < 1.0f ? 1.0f : f;
}

// 21j-r42: the FOV a top / bottom half gets: mode 0 = the game's own shape (48 deg across, the height from the half's shape)
// at SplitFovScale % of the game's FOV (100 = the game's exactly), as single player's FovScale; 1 / 2 = BF2's (r39 - r41).
// *f = the factor that makes on tan(fov / 2) (1 = nothing changed).
float SfhWiden(const float r[4], float fov, float ratio, int mode, float* f)
{
    *f = 1.0f;
    if (!(r[3] > 0.05f && r[2] > r[3] * 1.01f) || !(fov > 0.01f && fov < 3.0f))
        return fov; // not a top / bottom half
    float wide = fov;
    if (mode >= 1)
        wide = 2.0f * atanf(SfhFactorRect(r, ratio, mode) * tanf(0.5f * fov));
    else if (g_sfhPct != 100)
        wide = fov * static_cast<float>(g_sfhPct) / 100.0f;
    if (wide > 3.0f)
        wide = 3.0f; // (under 172 deg)
    *f = tanf(0.5f * wide) / tanf(0.5f * fov);
    return wide;
}

SfhCam* SfhSlot(const void* cam, bool make)
{
    for (SfhCam& c : g_sfhCams)
        if (c.cam == cam)
            return &c;
    if (!make)
        return nullptr;
    for (SfhCam& c : g_sfhCams)
        if (!c.cam)
            return &c;
    return &g_sfhCams[0];
}

// the Apply's SetFov(fov, h / w): widened for a top / bottom half
void __fastcall SfhSetFovWrap(void* cam, void* edx, float fov, float ratio)
{
    float r[4] = {};
    const int view = SfhView(cam, r);
    float f = 1.0f;
    const float wide = view < 0 ? fov : SfhWiden(r, fov, ratio, g_sfhMode, &f);
    if (fabsf(f - 1.0f) <= 0.0001f)
    {
        if (SfhCam* c = SfhSlot(cam, false))
            c->cam = nullptr;
        g_sfhSetFov(cam, edx, fov, ratio);
        return;
    }
    g_sfhSetFov(cam, edx, wide, ratio);
    SfhCam* const c = SfhSlot(cam, true);
    c->cam = cam;
    c->base = fov;
    c->wrote = CamField(cam, kCamTanX);
    c->f = f;
    const LONG n = InterlockedIncrement(&g_sfhWide);
    if ((n <= 2 || fabsf(f - g_sfhLastF[view]) > 0.01f || n % 36000 == 0) && g_sfhLines < 40)
    {
        ++g_sfhLines;
        const float tx = CamField(cam, kCamTanX);
        const float ty = CamField(cam, kCamTanY);
        Log("[fovh] %lu ms: player %d's half (view %d, its h / w %.4f): the game's FOV %.1f deg (%.1f x %.1f deg) -> %.1f x %.1f "
            "deg (x %.3f on tan; tan %.4f / %.4f; SplitFovH=%d, SplitFovScale %d %%; %ld changed, %ld read backs)",
            ScanMsSinceAttach(), view + 1, view, static_cast<double>(ratio), static_cast<double>(fov) * 57.29578,
            static_cast<double>(fov) * 57.29578, 2.0 * atan(tan(0.5 * static_cast<double>(fov)) * static_cast<double>(ratio)) * 57.29578,
            2.0 * atan(static_cast<double>(tx)) * 57.29578, 2.0 * atan(static_cast<double>(ty)) * 57.29578,
            static_cast<double>(f), static_cast<double>(tx), static_cast<double>(ty), g_sfhMode, g_sfhPct, static_cast<long>(n),
            static_cast<long>(g_sfhBack));
    }
    g_sfhLastF[view] = f;
}

// the Apply's read back of the FOV (to ease from): the game's own while the camera still holds ours
float __fastcall SfhGetFovWrap(void* cam, void* edx)
{
    const float now = g_sfhGetFov(cam, edx);
    const SfhCam* const c = SfhSlot(cam, false);
    if (c && CamField(cam, kCamTanX) == c->wrote)
    {
        InterlockedIncrement(&g_sfhBack);
        return c->base;
    }
    return now;
}

// 20g-r14's check of the Apply's SetFov call: the game's SetFov behind ours
uint32_t SplitFovCallTarget(uint32_t callAt)
{
    return (g_sfhOn && callAt == g_sfhSetFovAt) ? g_sfhSetFovRva : CallTarget(callAt);
}

// 21j-r40: PLAYER 3'S HALF (the 3-player layout's bottom half; [Splitscreen] SplitFovP3). His camera is applied by
// BF1GameExt (PlayerChase): the Apply gives it root 0's shape (a quarter's: the factor above would be wrong there), then
// 20g-r14's ChaseShapeFix gives it his half's shape through the camera's projection (near, far, fov, ratio). The widening
// goes into that call - the game's FOV (2 atan of the tan the Apply wrote) widened for his half's own h / w, as the 2-player
// halves - and the camera is remembered as above, so the Apply's next read back (it eases from it) gets the game's own FOV.
float SfhP3Fov(int view, float tx, float ratio, float* f)
{
    *f = 1.0f;
    const float fov = 2.0f * atanf(tx);
    const uintptr_t cams = g_viewCamerasRva ? Rva(g_viewCamerasRva) : 0;
    if (!g_sfhOn || !cams || view < 0 || view > 3) // (the read back's wrapper must be in)
        return fov;
    float r[4] = {};
    memcpy(r, reinterpret_cast<const void*>(cams - 0x40 + 16u * static_cast<uint32_t>(view)), 16);
    return SfhWiden(r, fov, ratio, g_sfhP3Mode, f);
}

void SfhP3Remember(void* cam, float base, float f, int view, float ratio)
{
    if (fabsf(f - 1.0f) <= 0.0001f)
    {
        if (SfhCam* c = SfhSlot(cam, false))
            c->cam = nullptr;
        return;
    }
    SfhCam* const c = SfhSlot(cam, true);
    c->cam = cam;
    c->base = base;
    c->wrote = CamField(cam, kCamTanX);
    c->f = f;
    const LONG n = InterlockedIncrement(&g_sfhP3Wide);
    if ((n <= 2 || fabsf(f - g_sfhP3LastF) > 0.01f || n % 36000 == 0) && g_sfhP3Lines < 30)
    {
        ++g_sfhP3Lines;
        const float tx = CamField(cam, kCamTanX);
        const float ty = CamField(cam, kCamTanY);
        Log("[fovh] %lu ms: player 3's half (view %d, its h / w %.4f): the game's FOV %.1f deg (%.1f x %.1f deg) -> %.1f x %.1f deg "
            "(x %.3f on tan; tan %.4f / %.4f; SplitFovP3=%d, SplitFovScale %d %%; %ld changed, %ld read backs)",
            ScanMsSinceAttach(), view, static_cast<double>(ratio), static_cast<double>(base) * 57.29578,
            static_cast<double>(base) * 57.29578, 2.0 * atan(tan(0.5 * static_cast<double>(base)) * static_cast<double>(ratio)) * 57.29578,
            2.0 * atan(static_cast<double>(tx)) * 57.29578, 2.0 * atan(static_cast<double>(ty)) * 57.29578,
            static_cast<double>(f), static_cast<double>(tx), static_cast<double>(ty), g_sfhP3Mode, g_sfhPct, static_cast<long>(n),
            static_cast<long>(g_sfhBack));
    }
    g_sfhP3LastF = f;
}

void InstallSplitFovH()
{
    if (g_sfhMode <= 0 && g_sfhP3Mode <= 0 && g_sfhPct == 100)
    {
        Log("[fovh] [Splitscreen] SplitFovH=0, SplitFovP3=0, SplitFovScale=100 - the top / bottom halves keep the game's FOV (48 "
            "deg across, half the height)");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kSfhSite, hits, 4);
    const uint32_t site = n == 1 ? hits[0] : 0;
    // the Apply: back from the site to its start (CC CC before it), its prologue and early exit (20g-r14's kChaseApplyHead)
    uint32_t apply = 0;
    for (uint32_t back = 1; site && back < kSfhApplyMax; ++back)
        if (CodeByte(site - back - 1) == 0xCC && CodeByte(site - back - 2) == 0xCC)
        {
            apply = site - back;
            break;
        }
    const bool headOk = apply && MatchPatternAt(apply, kChaseApplyHead);
    const uint32_t getFov = site ? CallTarget(site + kSfhGetFovCall) : 0;
    const uint32_t lerp = site ? CallTarget(site + kSfhLerpCall) : 0;
    const uint32_t setFov = site ? CallTarget(site + kSfhSetFovCall) : 0;
    const bool ok = site && headOk && CodeByte(site + kSfhGetFovCall) == 0xE8 && CodeByte(site + kSfhLerpCall) == 0xE8 &&
                    CodeByte(site + kSfhSetFovCall) == 0xE8 && MatchPatternAt(getFov, kSfhGetFovBody) &&
                    MatchPatternAt(lerp, kSfhLerpBody) && MatchPatternAt(setFov, kSetFovBody) && g_viewCamerasRva;
    Log("[fovh] 21j-r39: the chase camera's SetFov: %d hit(s) at rva 0x%08X (GOG 1.3 0x0013B141), in the Apply at rva 0x%08X (GOG "
        "1.3 0x0013AAD0) %s; GetFov rva 0x%08X, Lerp rva 0x%08X, SetFov rva 0x%08X (GOG 1.3 0x0028AA70 / 0x000D3EC0 / 0x0028AB10); "
        "the renderer's views rva 0x%08X - %s",
        n, site, apply, headOk ? "OK" : "NOT OK", getFov, lerp, setFov, g_viewCamerasRva,
        ok ? "every byte checks out" : "does NOT check out - NOT installed");
    if (!ok)
        return;
    g_sfhGetFov = reinterpret_cast<SfhGetFov_t>(Rva(RelTarget(site + kSfhGetFovCall))); // (through their thunks, as the game)
    g_sfhSetFov = reinterpret_cast<SfhSetFov_t>(Rva(RelTarget(site + kSfhSetFovCall)));
    g_sfhSetFovRva = setFov;
    bool wrote = true;
    const struct
    {
        uint32_t at;
        const void* to;
    } calls[2] = { { site + kSfhGetFovCall, reinterpret_cast<const void*>(&SfhGetFovWrap) },
                   { site + kSfhSetFovCall, reinterpret_cast<const void*>(&SfhSetFovWrap) } };
    for (const auto& c : calls)
    {
        unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(c.at));
        unsigned char code[5] = { 0xE8, 0, 0, 0, 0 };
        const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(c.to) - (reinterpret_cast<uintptr_t>(at) + 5));
        memcpy(code + 1, &rel, 4);
        wrote = WriteCode(reinterpret_cast<uintptr_t>(at), code, sizeof(code)) && wrote;
    }
    g_sfhSetFovAt = site + kSfhSetFovCall;
    g_sfhOn = wrote;
    Log("[fovh] 21j-r39: THE 2-PLAYER TOP / BOTTOM FOV %s ([Splitscreen] SplitFovH=%d: %s) - side by side, quarters and one player "
        "keep the game's FOV",
        g_sfhOn ? "ON" : "NOT on (a call could not be written)", g_sfhMode,
        g_sfhMode <= 0   ? "the game's own shape at SplitFovScale %"
        : g_sfhMode == 1 ? "BF2's rule - each half as tall as one player's view, as wide as its shape (83 x 28 deg at 16:9)"
                         : "SplitFovTall degrees tall, as wide as its shape (BF2's own: 51)");
    Log("[fovh] 21j-r40: PLAYER 3'S HALF (the 3-player bottom half) %s ([Splitscreen] SplitFovP3=%d: %s)",
        g_sfhOn ? "ON" : "off", g_sfhP3Mode,
        g_sfhP3Mode <= 0   ? "the game's own shape at SplitFovScale %, as the 2-player halves"
        : g_sfhP3Mode == 1 ? "as tall as one player's view, as wide as its shape (83 x 28 deg at 16:9)"
                           : "SplitFovTall degrees tall, as wide as its shape (BF2's own: 51)");
    {
        const double fx = 48.0 * g_sfhPct / 100.0;
        Log("[fovh] 21j-r42: [Splitscreen] SplitFovScale=%d - with SplitFovH / SplitFovP3 = 0, each top / bottom half at %d %% of the "
            "game's FOV: %.1f x %.1f deg at 16:9 (the game's own 48.0 x 14.3)",
            g_sfhPct, g_sfhPct, fx, 2.0 * atan(tan(fx * 0.5 / 57.29578) * 9.0 / 32.0) * 57.29578);
    }
    Log("[fovh] 21j-r41: [Splitscreen] SplitFovTall=%d - with 2, each half %d deg tall (%.1f deg wide at 16:9; BF2's own %.0f tall, "
        "118.9 wide)",
        g_sfhTallDeg, g_sfhTallDeg, 2.0 * atan(static_cast<double>(g_sfhTallTan) * 32.0 / 9.0) * 57.29578,
        static_cast<double>(kSfhBf2TallDeg));
}

// 20g-r5: DIAGNOSTICS (log only) for what r4's test showed and the code does not explain yet:
//  [diag34] every 5 s of a match with players 3 / 4's HUD set up: each player's MapDisplay (shown +20h, mode +1F28Ch,
//    big-map flag +1F289h, unit +3F4h, its map element +30h shown) and CaptureDisplay (shown +20h, active +2F0h, its
//    group +30h shown) - players 1 / 2 the game's own, 3 / 4 ours (their capture bar never showed).
//  [wall34] while any player's camera is within 1.3 m of his soldier ("backed into a wall" - players 3 / 4's views went
//    in with player 1's): every 0.2 s each player's camera-to-soldier distance, and player 1's and players 3 / 4's
//    chase cameras' state words +90h..+D0h.
constexpr uint32_t kElemFlags = 0x14;        // an interface element's flags (bit 8 = shown)
constexpr uint32_t kSoldierPosFromUnit = 0x70; // the soldier's world position = unit - 70h (soldier + 1E0h, soldier = unit - 250h)
int g_diagLines = 0;
int g_wallLines = 0;
DWORD g_diagNext = 0;
DWORD g_wallNext = 0;

uintptr_t DiagDisplay(int d, int p) // p 0..3: the game's own for 0 / 1, ours for 2 / 3
{
    if (p < 2)
        return ReadU32(Rva(g_hudArray[d]) + static_cast<uintptr_t>(p) * 4);
    const Hud34Run& r = g_hr[d];
    return (r.on && r.setUp[p - 2]) ? r.ours[p - 2] : 0;
}

void HudDiagFrame()
{
    if (!g_hudOn || (HudRoot(0) < 0 && HudRoot(1) < 0))
        return;
    const DWORD now = GetTickCount();
    if (g_diagLines < 70 && static_cast<LONG>(now - g_diagNext) >= 0)
    {
        g_diagNext = now + 5000;
        ++g_diagLines;
        char text[4][120] = {};
        for (int p = 0; p < 4; ++p)
        {
            const uintptr_t map = DiagDisplay(2, p);
            const uintptr_t cap = DiagDisplay(1, p);
            snprintf(text[p], sizeof(text[p]), "P%d map %s%u m%d f%u u%c e%u cap %s%u a%u g%u", p + 1, map ? "" : "-",
                     map ? ReadU32(map + kHudShownByte) & 0xFFu : 0u, map ? static_cast<int>(ReadU32(map + 0x1F28C)) : -1,
                     map ? ReadU32(map + 0x1F289) & 0xFFu : 0u, map && ReadU32(map + 0x3F4) ? 'Y' : 'n',
                     map ? (ReadU32(map + 0x30 + kElemFlags) >> 8) & 1u : 0u, cap ? "" : "-",
                     cap ? ReadU32(cap + kHudShownByte) & 0xFFu : 0u, cap ? ReadU32(cap + 0x2F0) & 0xFFu : 0u,
                     cap ? (ReadU32(cap + 0x30 + kElemFlags) >> 8) & 1u : 0u);
        }
        Log("[diag34] %lu ms: %s | %s | %s | %s (map: shown, mode, big flag, unit, element shown; cap: shown, active, group "
            "shown)",
            ScanMsSinceAttach(), text[0], text[1], text[2], text[3]);
    }
    if (g_wallLines >= 80 || static_cast<LONG>(now - g_wallNext) < 0)
        return;
    g_wallNext = now + 200;
    const uintptr_t cm = g_quarterMgr;
    if (!cm)
        return;
    float dist[4] = { -1.0f, -1.0f, -1.0f, -1.0f };
    bool tooClose = false;
    for (int p = 0; p < 4 && p < static_cast<int>(g_players); ++p)
    {
        const uintptr_t cam = p < 2 ? ReadU32(cm + kCamCameras + static_cast<uintptr_t>(p) * 4)
                                    : (g_specMade[p - 2] ? reinterpret_cast<uintptr_t>(g_specCam[p - 2]) : 0);
        const uintptr_t ch = LocalCharacter(p);
        const uintptr_t unit = ch ? ReadU32(ch + 0xB4) : 0;
        float c[3] = {};
        float u[3] = {};
        if (!cam || !unit || unit <= kSoldierPosFromUnit || !ReadMem(cam + kCamMatrix + 0x30, c, sizeof(c)) ||
            !ReadMem(unit - kSoldierPosFromUnit, u, sizeof(u)))
            continue;
        const float dx = c[0] - u[0];
        const float dy = c[1] - u[1];
        const float dz = c[2] - u[2];
        const float d2 = dx * dx + dy * dy + dz * dz;
        if (!(d2 >= 0.0f) || d2 > 1.0e6f)
            continue;
        dist[p] = sqrtf(d2);
        if (dist[p] < 1.3f)
            tooClose = true;
    }
    if (!tooClose)
        return;
    ++g_wallLines;
    char words[3][200] = {};
    const uintptr_t chase[3] = { ReadU32(cm + 0x34), g_chaseMade[0] ? reinterpret_cast<uintptr_t>(g_chase[0]) : 0,
                                 g_chaseMade[1] ? reinterpret_cast<uintptr_t>(g_chase[1]) : 0 };
    for (int i = 0; i < 3; ++i)
    {
        size_t len = 0;
        for (uint32_t o = 0x90; chase[i] && o <= 0xD0 && len + 10 < sizeof(words[i]); o += 4)
            len += static_cast<size_t>(snprintf(words[i] + len, sizeof(words[i]) - len, "%08X ", ReadU32(chase[i] + o)));
    }
    Log("[wall34] %lu ms: camera to soldier P1 %.2f P2 %.2f P3 %.2f P4 %.2f m | chase +90..+D0: P1 %s| P3 %s| P4 %s",
        ScanMsSinceAttach(), static_cast<double>(dist[0]), static_cast<double>(dist[1]), static_cast<double>(dist[2]),
        static_cast<double>(dist[3]), words[0], words[1], words[2]);
}

// 20g-r5: views 3 / 4 answered with players 3 / 4's cameras (Players on, Views=4)
void InstallViewToCam()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kViewToCamBody, hits, 4);
    uint32_t th[4] = {};
    int direct = 0;
    const int nt = n == 1 ? FindThunks(hits[0], th, 4, &direct) : 0;
    const int calls = nt == 1 ? CountCallsTo(th[0]) : 0;
    bool ok = n == 1 && nt == 1 && direct == 0 && calls >= 1;
    if (ok)
        ok = MoveThunk("camera manager ViewToCamera", hits[0], reinterpret_cast<const void*>(&ViewToCamWrap), &g_origViewToCam);
    Log("[players] camera manager ViewToCamera: %d hit(s) rva 0x%08X, %d thunk(s), %d call(s) (GOG 1.3: 0x00094A60, 3) - %s", n,
        n == 1 ? hits[0] : 0u, nt, calls,
        ok ? "views 3 / 4 answer cameras 2 / 3 (command post colours from players 3 / 4's teams)" : "NOT installed");
}

// ---------------------------------------------------------------------------------------------------------
// 20g-r6: THE SPAWN SCREEN'S LOW DETAIL ONLY IN PLAYER 1'S VIEW
// ---------------------------------------------------------------------------------------------------------
// The soldier draw (EntitySoldier's renderable, vtable slot 3; GOG 1.3 0x0010F2C0) starts with
//   call GetCurrentView; push eax; call GetSpawnDisplay(view) (the one PC object, whatever the view);
//   mov ecx, eax; call SpawnDisplay::IsShown (mov al, [ecx+237Eh]; ret) - +33h
//   ... edx = shown ? 1 : the detail level it was given -> the soldier's detail for this draw
// so while player 1's spawn screens are up every soldier in every view is drawn at level 1 (the low-res models, no
// pose update). The call at +33h goes to SpawnShownLodWrap: the flag as it is in player 1's own view (the camera
// manager's renderer view of camera 0, +6Ch), 0 in every other view.
const char* const kPatSoldierDraw =
    "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC 58 06 00 00 56 8B F1 57 89 75 D0 E8 ?? ?? ?? ?? 50 "
    "E8 ?? ?? ?? ?? 83 C4 04 8B C8 E8 ?? ?? ?? ?? 0F B6 4B 08 8D BE D0 02 00 00 84 C0 C6 45 FF 00 8D 86 D4 00 00 00 BA 01 00 00 "
    "00 0F 44 D1";
constexpr uint32_t kSoldierDrawViewCall = 0x23; // E8 GetCurrentView (mov eax, [renderer current view]; ret)
constexpr uint32_t kSoldierDrawShownCall = 0x33; // E8 SpawnDisplay::IsShown
const char* const kSdIsShownBody = "8A 81 7E 23 00 00 C3";
constexpr uint32_t kSdShown = 0x237E;
int g_lodMode = 1;              // [Splitscreen] SpawnLodP1Only: 0 off, 1 = with players 3 / 4, 2 = any split screen
uintptr_t g_curViewVa = 0;      // the renderer's current view (its cameras +10h)
uintptr_t g_camMgrPtrVa = 0;
volatile LONG g_lodAsked = 0;   // calls while the spawn screen was up
volatile LONG g_lodSpared = 0;  // ... answered 0 (not player 1's view)

uint32_t __fastcall SpawnShownLodWrap(uintptr_t sd, void* edx)
{
    (void)edx;
    const uint8_t shown = *reinterpret_cast<const volatile uint8_t*>(sd + kSdShown);
    if (!shown)
        return 0;
    InterlockedIncrement(&g_lodAsked);
    const uintptr_t cm = g_camMgrPtrVa ? *reinterpret_cast<const volatile uintptr_t*>(g_camMgrPtrVa) : 0;
    if (!cm || !g_curViewVa)
        return shown;
    const int32_t view = *reinterpret_cast<const volatile int32_t*>(g_curViewVa);
    const int32_t p1 = *reinterpret_cast<const volatile int32_t*>(cm + kCamViews);
    if (view == p1)
        return shown;
    InterlockedIncrement(&g_lodSpared);
    return 0;
}

void InstallSpawnLod()
{
    if (g_lodMode == 0)
    {
        Log("[lod34] [Splitscreen] SpawnLodP1Only=0 - the spawn screen's low detail in every view, as the game has it");
        return;
    }
    if (g_lodMode == 1 && !g_playersOn)
    {
        Log("[lod34] SpawnLodP1Only=1: only with players 3 / 4 (Players=3 / 4) - not installed (2 = any split screen)");
        return;
    }
    uint32_t hits[4] = {};
    const int n = FindPattern(kPatSoldierDraw, hits, 4);
    const uint32_t site = (n == 1 && CodeByte(hits[0] + kSoldierDrawShownCall) == 0xE8) ? hits[0] + kSoldierDrawShownCall : 0;
    const uint32_t shownFn = site ? CallTarget(site) : 0;
    const bool shownOk = shownFn && MatchPatternAt(shownFn, kSdIsShownBody);
    const uint32_t viewFn = (n == 1 && CodeByte(hits[0] + kSoldierDrawViewCall) == 0xE8) ? CallTarget(hits[0] + kSoldierDrawViewCall) : 0;
    const bool viewShape = viewFn && MatchPatternAt(viewFn, kMovEaxImmRet);
    const uintptr_t viewVa = viewShape ? *reinterpret_cast<const uintptr_t*>(Rva(viewFn) + 1) : 0;
    const uintptr_t cams = g_viewCamerasRva ? Rva(g_viewCamerasRva) : 0;
    const bool viewOk = viewVa && cams && viewVa == cams + 0x10;
    const bool mgrOk = g_camMgrPtrRva != 0;
    bool ok = n == 1 && shownOk && viewOk && mgrOk;
    if (ok)
    {
        unsigned char* const at = reinterpret_cast<unsigned char*>(Rva(site));
        DWORD old = 0;
        ok = at[0] == 0xE8 && VirtualProtect(at + 1, 4, PAGE_EXECUTE_READWRITE, &old) != 0;
        if (ok)
        {
            g_curViewVa = viewVa;
            g_camMgrPtrVa = Rva(g_camMgrPtrRva);
            const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&SpawnShownLodWrap) - (reinterpret_cast<uintptr_t>(at) + 5));
            memcpy(at + 1, &rel, 4);
            DWORD ignored = 0;
            VirtualProtect(at + 1, 4, old, &ignored);
            FlushInstructionCache(GetCurrentProcess(), at, 5);
            int32_t back = 0;
            memcpy(&back, at + 1, 4);
            ok = back == rel;
        }
    }
    Log("[lod34] the soldier draw: %d hit(s) rva 0x%08X (GOG 1.3: 0x0010F2C0); its IsShown call +33h -> rva 0x%08X %s; its view "
        "getter -> rva 0x%08X %s; the camera manager pointer %s - %s",
        n, n == 1 ? hits[0] : 0u, shownFn, shownOk ? "OK (SpawnDisplay +237Eh)" : "does NOT match", viewFn,
        viewOk ? "OK (the renderer's current view)" : "does NOT match", mgrOk ? "OK" : "NOT found",
        ok ? "PATCHED: the spawn screen's low detail only in player 1's own view (no more low-res soldiers in the other views "
             "while player 1 is on his spawn screens)"
           : "NOT installed (as the game has it)");
}

// 20g-r6 (split_spawn_menus.cpp): player 3's / 4's own MapDisplay, set up this match, or 0
uintptr_t HudMapOf(int k)
{
    if (!g_hudOn || k < 0 || k > 1)
        return 0;
    const Hud34Run& r = g_hr[2];
    return (r.on && r.setUp[k]) ? r.ours[k] : 0;
}

void ReportHud34Own()
{
    if (!g_hudOn)
        return;
    for (int d = 0; d < 8; ++d)
    {
        const Hud34Run& r = g_hr[d];
        if (!r.on)
            continue;
        char text[2][160] = {};
        for (int k = 0; k < 2; ++k)
        {
            const uintptr_t obj = r.ours[k];
            if (!obj)
                snprintf(text[k], sizeof(text[k]), "none");
            else
                snprintf(text[k], sizeof(text[k]), "0x%08X %s: player +18h %d, viewport +1Ch %d, task list +4 %s, shown +20h %u",
                         static_cast<uint32_t>(obj), r.setUp[k] ? "set up" : "NOT set up",
                         static_cast<int>(ReadU32(obj + kHudPlayer)), static_cast<int>(ReadU32(obj + kHudViewport)),
                         ReadU32(obj + kHudTaskNode) ? "yes" : "no", ReadU32(obj + kHudShownByte) & 0xFFu);
        }
        Log("[hud34] table: ours %-15s P3 %s | P4 %s (made %ld, set up %ld, deleted %ld)", kHudName[d], text[0], text[1],
            static_cast<long>(r.made), static_cast<long>(r.setups), static_cast<long>(r.deleted));
    }
    Log("[hud34] table: roots P3 %d / P4 %d (views %d / %d), shown %d / %d, HUD flags of views 3 / 4 %u / %u; camera "
        "manager answers for 2 / 3 so far: RootIndex %ld, IsChase %ld, GetChase %ld; map textures %ld; show / hide %ld; "
        "MapDisplay objectives panels skipped %ld; minimap icons (calls / theirs too): marker add %ld / %ld, unit add %ld / %ld, "
        "marker remove %ld / %ld, unit remove %ld / %ld, replayed %ld; their SetMode(0) %ld; views 3 / 4 -> cameras %ld",
        g_hudRoot[0], g_hudRoot[1], PlayerNSpecView(0), PlayerNSpecView(1), g_hudShown[0], g_hudShown[1],
        PlayerNSpecView(0) >= 0 ? *RendFlag(1, PlayerNSpecView(0)) : 0u,
        PlayerNSpecView(1) >= 0 ? *RendFlag(1, PlayerNSpecView(1)) : 0u, static_cast<long>(g_hudCamAnswers[0]),
        static_cast<long>(g_hudCamAnswers[1]), static_cast<long>(g_hudCamAnswers[2]), static_cast<long>(g_hudMapTex),
        static_cast<long>(g_hudShowCalls), static_cast<long>(g_hudMapObjSkipped), static_cast<long>(g_mapIconCalls[0]),
        static_cast<long>(g_mapIconOurs[0]), static_cast<long>(g_mapIconCalls[1]), static_cast<long>(g_mapIconOurs[1]),
        static_cast<long>(g_mapIconCalls[2]), static_cast<long>(g_mapIconOurs[2]), static_cast<long>(g_mapIconCalls[3]),
        static_cast<long>(g_mapIconOurs[3]), g_mapReplayed, static_cast<long>(g_mapSetModes),
        static_cast<long>(g_viewToCamAnswers));
}

DWORD g_spawnTry = 0;
bool g_inWorld[2] = {};
int g_spawnLines = 0;
volatile LONG g_spawnPresses = 0;

// Game thread, each frame: every 3 s, players 3 / 4 not in the world (and not ready) while player 1 is -> SetReady(1).
void PlayersFrame()
{
    if (!g_playersOn)
        return;
    P2SpawnP34Frame(); // 20f-r2: players 3 and 4's spawn screens (split_spawn_menus.cpp)
    const DWORD now = GetTickCount();
    if (now - g_spawnTry < 3000)
        return;
    g_spawnTry = now;
    const uintptr_t p1 = LocalCharacter(0);
    for (int k = 0; k < 2 && k + 3 <= static_cast<int>(g_players); ++k)
    {
        const uintptr_t ch = LocalCharacter(k + 2);
        if (!ch)
            continue;
        const bool in = ReadU32(ch + 0xB4) != 0;
        if (in != g_inWorld[k] && g_spawnLines < 60)
        {
            ++g_spawnLines;
            Log("[players] %lu ms: player %d %s (team %d, class %08X, unit %08X)", ScanMsSinceAttach(), k + 3,
                in ? "IS IN THE WORLD" : "left the world", static_cast<int>(ReadU32(ch + 0xA0)), ReadU32(ch + 0xAC),
                ReadU32(ch + 0xB4));
        }
        g_inWorld[k] = in;
        if (in || !g_spawnWanted || !g_setReady || !p1 || !ReadU32(p1 + 0xB4))
            continue;
        if (P2SpawnP34Owns(k))
            continue; // 20f-r2: his spawn screens look after him
        if (*reinterpret_cast<const volatile uint8_t*>(ch + 0xCC))
            continue; // ready, the game will spawn him
        g_setReady(reinterpret_cast<void*>(ch), nullptr, 1);
        const LONG n = InterlockedIncrement(&g_spawnPresses);
        if (n <= 6 || n % 20 == 0)
            Log("[players] %lu ms: Spawn pressed for player %d (press %ld)", ScanMsSinceAttach(), k + 3, static_cast<long>(n));
    }
}

void InstallPlayers()
{
    if (!g_viewsReady)
    {
        Log("[players] Players=%d IGNORED: it needs Views=4 (or 3, 20g-r13) working - 2 players as before", g_playersWanted);
        return;
    }
    const uint32_t thunk = g_thunk[kNumLocal];
    if (!g_hooked[kNumLocal] || !thunk)
    {
        Log("[players] Players=%d NOT installed: GetNumLocalPlayers' thunk is not ours", g_playersWanted);
        return;
    }
    // (1) the call sites that keep getting 2
    const uint32_t create = FindOne("the camera manager's creation", kPatCamCreate);
    uint32_t pauseFn[3] = {};
    for (int k = 0; k < 3; ++k)
        pauseFn[k] = FindOne("a pause loop", kPatPauseLoop[k]);
    const uint32_t msgHead = FindOne("the message loop's head", kPatMsgHead);
    const uint32_t msgTail = FindOne("the message loop's tail", kPatMsgTail);
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    int sites = 0;
    int clampCreate = 0;
    int clampSetNum = 0;
    int clampPause = 0;
    int clampMsg = 0;
    if (CodeSection(&codeRva, &codeSize))
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(Rva(codeRva));
        for (uint32_t i = 0; i + 5 <= codeSize; ++i)
        {
            if (p[i] != 0xE8)
                continue;
            // to the thunk, or to a jump (chain) that ends at it (GOG 1.3: 0x0019FF70 is "jmp GetNumLocalPlayers",
            // through a thunk of its own - the message loop calls that one)
            uint32_t t = RelTarget(codeRva + i);
            bool reaches = t == thunk;
            for (int hop = 0; hop < 4 && !reaches && t >= codeRva && t + 5 <= codeRva + codeSize && CodeByte(t) == 0xE9;
                 ++hop)
            {
                t = RelTarget(t);
                reaches = t == thunk;
            }
            if (!reaches)
                continue;
            const uint32_t call = codeRva + i;
            const uint32_t ret = call + 5;
            ++sites;
            // push eax; call create
            if (create && CodeByte(ret) == 0x50 && CodeByte(ret + 1) == 0xE8 && CallTarget(ret + 1) == create)
            {
                clampCreate += AddClamp(call, "the camera manager's creation") ? 1 : 0;
                continue;
            }
            // push eax; call GetCameraManager; mov ecx, eax; call SetNumCameras
            if (g_setNumThunk && CodeByte(ret) == 0x50 && CodeByte(ret + 1) == 0xE8 && CodeByte(ret + 6) == 0x8B &&
                CodeByte(ret + 7) == 0xC8 && CodeByte(ret + 8) == 0xE8 && RelTarget(ret + 8) == g_setNumThunk)
            {
                clampSetNum += AddClamp(call, "SetNumCameras(count) at match load") ? 1 : 0;
                continue;
            }
            bool done = false;
            for (int k = 0; k < 3 && !done; ++k)
                if (pauseFn[k] && call >= pauseFn[k] && call < pauseFn[k] + kPauseLoopSpan)
                {
                    clampPause += AddClamp(call, "a pause loop (PauseState[2])") ? 1 : 0;
                    done = true;
                }
            if (done)
                continue;
            if ((msgHead && call == msgHead + kMsgHeadCall) || (msgTail && call == msgTail + kMsgTailCall))
                clampMsg += AddClamp(call, "the message loop (MessageDisplay[2][2])") ? 1 : 0;
        }
    }
    Log("[players] GetNumLocalPlayers is called from %d places (GOG 1.3: 32 - 23 direct, 9 through 0x0019FF70); keeping 2: camera manager "
        "creation %d (need 1), SetNumCameras %d (need 1), pause loops %d (need 6), message loop %d (need 2)",
        sites, clampCreate, clampSetNum, clampPause, clampMsg);
    if (clampCreate != 1 || clampSetNum != 1 || clampPause != 6 || clampMsg != 2)
    {
        Log("[players] Players=%d NOT installed: the call sites that must keep getting 2 were not all found",
            g_playersWanted);
        return;
    }
    // (2) the network's local slots: 4 of ours
    if (!g_netSlotsRva)
    {
        Log("[players] Players=%d NOT installed: the network's local slots were not found", g_playersWanted);
        return;
    }
    g_netSlotsOld = Rva(g_netSlotsRva);
    uint32_t ops[32] = {};
    const int n = FindOperands(g_netSlotsOld, g_netSlotsOld + 8, ops, 32);
    int at0 = 0;
    int at4 = 0;
    for (int i = 0; i < n && i < 32; ++i)
        (CodeU32(ops[i]) == g_netSlotsOld ? at0 : at4) += 1;
    const uint32_t lookup = FindOne("net id -> local index", kPatNetIdToLocal);
    const uint32_t fill = FindOne("net local slots fill", kNetSlotsFill);
    const bool boundsOk = lookup && fill && CodeByte(lookup + kNetIdToLocalBound) == 2 &&
                          CodeByte(lookup + kNetIdToLocalBound - 2) == 0x83 && CodeByte(fill + kNetSlotsFillBound) == 2 &&
                          CodeByte(fill + kNetSlotsFillBound - 2) == 0x83;
    Log("[players] network local slots (rva 0x%08X): %d operands through the relocation table (GOG 1.3: 8 - 6 to slot 0, "
        "2 to slot 1): %d to slot 0, %d to slot 1; the lookup's and the fill's bounds of 2 %s",
        g_netSlotsRva, n, at0, at4, boundsOk ? "OK" : "NOT FOUND");
    if (n < 1 || n > 32 || at0 < 1 || !boundsOk)
    {
        Log("[players] Players=%d NOT installed: the local slots' operands or bounds are not as expected", g_playersWanted);
        return;
    }
    g_netSlots4[0] = static_cast<int32_t>(ReadU32(g_netSlotsOld));
    g_netSlots4[1] = static_cast<int32_t>(ReadU32(g_netSlotsOld + 4));
    g_netSlots4[2] = -1;
    g_netSlots4[3] = -1;
    int moved = 0;
    for (int i = 0; i < n; ++i)
    {
        const uint32_t v = CodeU32(ops[i]);
        const uint32_t nv = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_netSlots4)) + (v - static_cast<uint32_t>(g_netSlotsOld));
        if (WriteCode(Rva(ops[i]), &nv, 4))
            ++moved;
        else
            Log("[players] operand rva 0x%08X NOT moved", ops[i]);
    }
    const unsigned char four = 4;
    const bool b1 = WriteCode(Rva(lookup + kNetIdToLocalBound), &four, 1);
    const bool b2 = WriteCode(Rva(fill + kNetSlotsFillBound), &four, 1);
    Log("[players] network local slots: %d of %d operands now point at BF1GameExt's int[4] (%08X), lookup bound 4 %s, fill "
        "bound 4 %s",
        moved, n, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_netSlots4)), b1 ? "OK" : "FAILED", b2 ? "OK" : "FAILED");
    if (moved != n || !b1 || !b2)
    {
        Log("[players] Players=%d NOT switched on (a write failed) - the count stays 2", g_playersWanted);
        return;
    }
    if (!InstallGuards())
    {
        Log("[players] Players=%d NOT switched on (the guards for players 3 and 4 are not all in) - the count stays 2",
            g_playersWanted);
        return;
    }
    g_players = static_cast<uint32_t>(g_playersWanted);
    g_playersMax = g_players; // 21g-r2: the sign-in picks 2 - this for each match
    g_playersOn = true;
    Log("[players] PLAYERS=%d ON: while split screen is on (2 local players) the game is told %d, except the call sites "
        "above; the match setup makes characters for players 3%s; views 3 and 4 follow their soldiers; %s",
        g_playersWanted, g_playersWanted, g_playersWanted == 4 ? " and 4" : "",
        g_padsWanted ? "PadsP34=1: pads 3 and 4 drive them (20e, [pad34] lines)"
                     : "PadsP34=0: they move with player 1's / player 2's controls");
}

// ---------------------------------------------------------------------------------------------------------
// Phase 20e: pads 3 and 4, players 3 and 4's input
// ---------------------------------------------------------------------------------------------------------
struct Pad34
{
    alignas(16) unsigned char pad[kPadBytes];
    alignas(16) unsigned char input[kPlayerInputBytes];
    alignas(16) unsigned char bufA[kBufABytes];
    alignas(16) unsigned char bufB[kBufBBytes];
    int xinput = -1;          // the XInput index it reads (-1: none yet)
    bool connected = false;
    DWORD lostSince = 0;
    uint16_t lastButtons = 0;
    uint32_t lastBits = 0;
    bool moving = false;
    unsigned long copies = 0;
    DWORD holdSince = 0;      // 21h-r3: when his menu went (the calm packet held until his buttons are up)
    unsigned long calm = 0;   // 21h-r3: calm packets given in place of his
};

Pad34 g_p34[2];
constexpr uint32_t kInputPadSlot = 0x4BC; // PlayerInput's pad device (player2_input.cpp: +4BCh pad, +4C0h keyboard, +4C4h mouse)
volatile LONG g_p34Paused[2] = {};        // 20g-r6: his pause screen is up (split_spawn_menus.cpp)
// 21h-r3: his real pause menu (split_spawn_menus.cpp): 1 up - his pad drives the menu, his soldier gets a calm packet (no sticks,
// no buttons); 2 it went - the calm packet held until every button of his pad is up (3 s at most), so the B / A / Start that
// closed it does not reach his soldier (B is crouch: he crouched as he left the menu)
volatile LONG g_p34Menu[2] = {};
constexpr uint32_t kPacketBits = 0x10;          // buffer A: 4 floats (sticks), then the button bits
constexpr uint32_t kPacketKeep = 0x80000000u;   // the bit every packet of his has (kept in the calm one)
constexpr DWORD kMenuHoldMs = 3000;
bool g_pauseSlotLogged = false;
bool g_padsOn = false;      // built and running
bool g_padsFailed = false;  // tried, could not
DWORD g_padsLastPick = 0;
int g_padLines = 0;
constexpr int kMaxPadLines = 200;
volatile LONG g_controlOwnAnswers = 0;

bool PadBudget()
{
    if (g_padLines >= kMaxPadLines)
        return false;
    ++g_padLines;
    return true;
}

// 20g-r8: their controller options (see the top of this file)
constexpr uint32_t kInputMode = 0x04;      // control mode 0 on foot, 1 ground vehicles, 2 flying
constexpr uint32_t kInputFlip = 0x08;      // [3] bytes, by mode
constexpr uint32_t kInputPadScale = 0x4D4; // [3] floats, by mode
constexpr int kInputModes = 3;
// PlayerInput::Update (player2_input.cpp's body pattern) and its two reads, both inside it:
//   mov eax, [esi+4] ; mov ecx, [esi+4B8h] ; movzx eax, byte [eax+esi+8] ; push eax ; call <flip the pitch>
//   mov ecx, [esi+4] ; movss xmm0, [esi+ecx*4+4D4h] ; ucomiss xmm0, xmm1
const char* const kPatInputUpdate34 =
    "55 8B EC 83 EC 30 56 57 8D 41 0C 89 4D F0 8D B9 8C 00 00 00 89 45 F4 68 80 00 00 00";
const char* const kPatInputFlipRead = "8B 46 04 8B 8E B8 04 00 00 0F B6 44 30 08 50 E8";
const char* const kPatInputScaleRead = "8B 4E 04 F3 0F 10 84 8E D4 04 00 00 0F 2E C1";
constexpr uint32_t kInputUpdateSpan = 0x500; // GOG 1.3: the update is 0x505 bytes, the reads at +0C3h / +42Fh
// the game's controller defaults (log only): cmp ebx, 2 ; push esi ; mov esi, ecx ; sete al ; push edi ; push ebx ;
// mov [ebx+esi+9], al ; ... mov byte [ebx+esi+22Ch], 5 (flip = mode 2, sensitivity byte 5)
const char* const kPatControlDefaults =
    "83 FB 02 56 8B F1 0F 94 C0 57 53 88 44 33 09 8D 85 ?? ?? ?? ?? C6 84 33 2C 02 00 00 05";
struct Ctl34
{
    int invert = 0;    // on foot and in ground vehicles (modes 0 / 1)
    int invertFly = 1; // flying (mode 2) - the game's default
    int sens = 5;      // 1-10, 5 = the game's default (scale 1.0)
};
Ctl34 g_ctl[2];
volatile LONG g_ctlStamp[2] = {}; // bumped by every change (the input frame logs the values it wrote)
LONG g_ctlWritten[2] = { -1, -1 };
bool g_ctlChecked = false;
bool g_ctlOk = false;
char g_iniPath[MAX_PATH + 32] = {};

int CtlClampSens(int s)
{
    return s < 1 ? 1 : (s > 10 ? 10 : s);
}

// the game's own conversion (0x001F3DD0): 2 ^ ((byte - 5) / 5)
float CtlScale(int sens)
{
    return static_cast<float>(std::pow(2.0, (static_cast<double>(CtlClampSens(sens)) - 5.0) / 5.0));
}

// once: both reads where they should be (else the options are not written - the page still shows)
void CtlCheck()
{
    if (g_ctlChecked)
        return;
    g_ctlChecked = true;
    uint32_t upd[4] = {};
    uint32_t flip[4] = {};
    uint32_t scale[4] = {};
    uint32_t defs[4] = {};
    const int nu = FindPattern(kPatInputUpdate34, upd, 4);
    const int nf = FindPattern(kPatInputFlipRead, flip, 4);
    const int ns = FindPattern(kPatInputScaleRead, scale, 4);
    const int nd = FindPattern(kPatControlDefaults, defs, 4);
    const bool in = nu == 1 && nf == 1 && ns == 1 && flip[0] > upd[0] && flip[0] - upd[0] < kInputUpdateSpan && scale[0] > upd[0] &&
                    scale[0] - upd[0] < kInputUpdateSpan;
    g_ctlOk = in;
    Log("[pad34] controller options: PlayerInput::Update rva 0x%08X (%d hit(s)), its flip read +%03Xh (%d), its look scale read "
        "+%03Xh (%d)%s; the game's defaults rva 0x%08X (%d hit(s): flip for flying only, sensitivity 5)",
        nu == 1 ? upd[0] : 0, nu, nf == 1 && nu == 1 ? flip[0] - upd[0] : 0, nf, ns == 1 && nu == 1 ? scale[0] - upd[0] : 0, ns,
        in ? " - OK, players 3 / 4's options are written into their PlayerInputs"
           : " - NOT as expected: their options are NOT written (the game's values stay)",
        nd == 1 ? defs[0] : 0, nd);
}

// every input frame, before his update (the values are tiny; writing them each frame keeps them whatever else runs)
void CtlApply(int k)
{
    if (!g_ctlOk)
        return;
    Pad34& p = g_p34[k];
    const Ctl34 c = g_ctl[k];
    const float scale = CtlScale(c.sens);
    for (int m = 0; m < kInputModes; ++m)
    {
        p.input[kInputFlip + m] = static_cast<unsigned char>(m == 2 ? (c.invertFly ? 1 : 0) : (c.invert ? 1 : 0));
        memcpy(p.input + kInputPadScale + m * 4, &scale, sizeof(scale));
    }
    const LONG stamp = g_ctlStamp[k];
    if (stamp != g_ctlWritten[k])
    {
        g_ctlWritten[k] = stamp;
        if (PadBudget())
            Log("[pad34] %lu ms: player %d's controls: invert look %s, invert flying %s, look sensitivity %d (scale %.2f) - in his "
                "PlayerInput (flip bytes %u %u %u, mode now %u)",
                ScanMsSinceAttach(), k + 3, c.invert ? "ON" : "off", c.invertFly ? "ON" : "off", CtlClampSens(c.sens),
                static_cast<double>(scale), static_cast<unsigned>(p.input[kInputFlip]),
                static_cast<unsigned>(p.input[kInputFlip + 1]), static_cast<unsigned>(p.input[kInputFlip + 2]),
                ReadU32(reinterpret_cast<uintptr_t>(p.input) + kInputMode));
    }
}

uint32_t ControlStateOwn(int32_t id)
{
    if (!g_padsOn || !g_playersOn || !g_controlRva || (id != 2 && id != 3) || id >= static_cast<int32_t>(g_players))
        return 0;
    if (g_onlineRva && *reinterpret_cast<const volatile uint8_t*>(Rva(g_onlineRva)))
        return 0; // a network game: the game's own path
    const uint32_t state = ReadU32(Rva(g_controlRva) + static_cast<uintptr_t>(id) * 4);
    if (state)
        InterlockedIncrement(&g_controlOwnAnswers);
    return state;
}

const char* PadButtonText(uint16_t b, char* out, size_t n)
{
    static const struct
    {
        uint16_t m;
        const char* t;
    } kB[] = { { 0x0001, "Up" },  { 0x0002, "Down" }, { 0x0004, "Left" }, { 0x0008, "Right" }, { 0x0010, "Start" },
               { 0x0020, "Back" }, { 0x0040, "L3" },  { 0x0080, "R3" },   { 0x0100, "LB" },    { 0x0200, "RB" },
               { 0x1000, "A" },    { 0x2000, "B" },   { 0x4000, "X" },    { 0x8000, "Y" } };
    size_t used = 0;
    out[0] = 0;
    for (const auto& k : kB)
        if (b & k.m)
        {
            const int w = snprintf(out + used, n - used, "%s%s", used ? "+" : "", k.t);
            if (w < 0 || static_cast<size_t>(w) >= n - used)
                break;
            used += static_cast<size_t>(w);
        }
    if (!used)
        snprintf(out, n, "none");
    return out;
}

// The first connected XInput index nobody uses (the game's pad, pad 2, the other extra pad).
int PickPad34(int k)
{
    const int game = GamePadXInputIndex();
    const int p2 = Pad2XInputIndex();
    const int other = g_p34[1 - k].xinput;
    // 21g-r2: the pad that signed in as this player, while it answers and nobody else has it
    const int pref = g_padPref[k];
    if (pref >= 0 && pref != game && pref != p2 && pref != other && XInputPadPresent(pref))
        return pref;
    for (int i = 0; i < 4; ++i)
        if (i != game && i != p2 && i != other && XInputPadPresent(i))
            return i;
    return -1;
}

void BuildPads34()
{
    for (int k = 0; k < 2; ++k)
    {
        Pad34& p = g_p34[k];
        if (!PadBuild(p.pad) ||
            !PlayerInputBuild(p.input, p.bufA, p.bufB, reinterpret_cast<uintptr_t>(p.pad)))
        {
            g_padsFailed = true;
            Log("[pad34] pads 3 / 4 NOT built: player 2's pad / input code is not available on this run (pad2 / "
                "p2input lines) - players 3 and 4 keep copying pads 1 and 2");
            return;
        }
    }
    g_padsOn = true;
    CtlCheck(); // 20g-r8: (done at start; here only if it was not)
    Log("[pad34] %lu ms: pads 3 and 4 and players 3 and 4's input objects built with the game's code (as player "
        "2's): the game's pad uses XInput %d, pad 2 XInput %d", ScanMsSinceAttach(), GamePadXInputIndex(),
        Pad2XInputIndex());
}

void Pads34Frame(float dt)
{
    if (!g_padsWanted || g_padsFailed || !g_playersOn)
        return;
    if (!g_padsOn)
    {
        BuildPads34();
        if (!g_padsOn)
            return;
    }
    const DWORD now = GetTickCount();
    // pad 2 picks first (its own rule: the first pad the game's pad is not using), then pads 3 and 4
    const bool pickNow = now - g_padsLastPick >= 1000 && Pad2XInputIndex() >= 0;
    if (pickNow)
        g_padsLastPick = now;
    for (int k = 0; k < 2 && k + 3 <= static_cast<int>(g_players); ++k)
    {
        Pad34& p = g_p34[k];
        // (re)point: none yet, or lost for 2 s, or now taken by the game's pad / pad 2
        const bool taken = p.xinput >= 0 && (p.xinput == GamePadXInputIndex() || p.xinput == Pad2XInputIndex());
        const bool lost = p.xinput >= 0 && !p.connected && p.lostSince && now - p.lostSince > 2000;
        // 21g-r2: the pad that signed in as this player is free and answers - he takes it
        const int pref = g_padPref[k];
        const bool toPref = pref >= 0 && pref != p.xinput && pref != GamePadXInputIndex() && pref != Pad2XInputIndex() &&
                            pref != g_p34[1 - k].xinput && XInputPadPresent(pref);
        if (pickNow && (p.xinput < 0 || lost || taken || toPref))
        {
            const int was = p.xinput;
            p.xinput = -1;
            const int next = PickPad34(k);
            if (next >= 0)
            {
                PadPoint(p.pad, next);
                p.xinput = next;
                p.lostSince = 0;
                if (PadBudget())
                    Log("[pad34] %lu ms: pad %d -> XInput index %d%s (the game's pad %d, pad 2 %d)", ScanMsSinceAttach(),
                        k + 3, next, was >= 0 ? " (re-pointed)" : "", GamePadXInputIndex(), Pad2XInputIndex());
            }
            else if (was >= 0 && PadBudget())
                Log("[pad34] %lu ms: pad %d lost XInput index %d, no other free pad", ScanMsSinceAttach(), k + 3, was);
        }
        if (p.xinput < 0)
            continue;
        PadUpdate(p.pad, dt);
        const bool conn = PadConnected(p.pad);
        if (conn != p.connected)
        {
            PlayerInputSetConnected(p.input, conn);
            if (PadBudget())
                Log("[pad34] %lu ms: pad %d (XInput %d) %s", ScanMsSinceAttach(), k + 3, p.xinput,
                    conn ? "connected" : "DISCONNECTED");
            p.connected = conn;
            p.lostSince = conn ? 0 : now;
        }
        const uint16_t b = conn ? PadButtons(p.pad) : 0;
        if (b != p.lastButtons && b && PadBudget())
        {
            char t[96];
            Log("[pad34] %lu ms: pad %d (XInput %d) sees: %s", ScanMsSinceAttach(), k + 3, p.xinput,
                PadButtonText(b, t, sizeof(t)));
        }
        p.lastButtons = b;
        // 20g-r6: his pause screen is up (split_spawn_menus.cpp): his PlayerInput updates with no pad in its slot (+4BCh; the
        // update skips a missing device), so his soldier stands still while the pad drives the pause screen
        uintptr_t* const padSlot = reinterpret_cast<uintptr_t*>(p.input + kInputPadSlot);
        uintptr_t padSaved = 0;
        if (g_p34Paused[k])
        {
            if (*padSlot == reinterpret_cast<uintptr_t>(p.pad))
            {
                padSaved = *padSlot;
                *padSlot = 0;
            }
            else if (!g_pauseSlotLogged)
            {
                g_pauseSlotLogged = true;
                Log("[pad34] player %d's PlayerInput pad slot +4BCh holds %08X, not his pad %08X - his soldier is NOT stopped while "
                    "paused",
                    k + 3, static_cast<uint32_t>(*padSlot), static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p.pad)));
            }
        }
        CtlApply(k); // 20g-r8
        PlayerInputUpdate(p.input, dt);
        if (padSaved)
            *padSlot = padSaved;
        // 21h-r3: his real pause menu up, or just gone and a button still down: the calm packet in place of his
        const LONG menu = g_p34Menu[k];
        bool calm = false;
        if (menu == 1)
        {
            calm = true;
            p.holdSince = 0;
        }
        else if (menu == 2)
        {
            if (!p.holdSince)
                p.holdSince = now ? now : 1;
            const bool timeUp = now - p.holdSince > kMenuHoldMs;
            if (b == 0 || timeUp)
            {
                InterlockedCompareExchange(&g_p34Menu[k], 0, 2);
                if (PadBudget())
                    Log("[pad34] %lu ms: player %d: his menu went - his own packet again (%s after %lu ms, %lu calm packet(s) given)",
                        ScanMsSinceAttach(), k + 3, timeUp ? "held too long" : "every button up",
                        static_cast<unsigned long>(now - p.holdSince), p.calm);
                p.holdSince = 0;
                p.calm = 0;
            }
            else
                calm = true;
        }
        alignas(16) unsigned char calmPacket[kBufABytes] = {};
        const unsigned char* packet = p.bufA;
        if (calm)
        {
            uint32_t keep = 0;
            memcpy(&keep, p.bufA + kPacketBits, sizeof(keep));
            keep &= kPacketKeep;
            memcpy(calmPacket + kPacketBits, &keep, sizeof(keep));
            packet = calmPacket;
            ++p.calm;
        }
        const int id = g_netSlots4[k + 2];
        if (id >= 0 && ControlStateFill(id, packet))
        {
            ++p.copies;
            float f[4];
            uint32_t bits = 0;
            memcpy(f, packet, sizeof(f));
            memcpy(&bits, packet + kPacketBits, sizeof(bits));
            bool moving = false;
            for (float v : f)
                if (v > 0.5f || v < -0.5f)
                    moving = true;
            if (p.copies == 1 && PadBudget())
                Log("[pad34] %lu ms: player %d's control packet now goes into the control state of player id %d every "
                    "frame", ScanMsSinceAttach(), k + 3, id);
            if ((bits != p.lastBits || moving != p.moving) && PadBudget())
                Log("[pad34] %lu ms: player %d control packet: %.2f %.2f %.2f %.2f, buttons 0x%08X", ScanMsSinceAttach(),
                    k + 3, static_cast<double>(f[0]), static_cast<double>(f[1]), static_cast<double>(f[2]),
                    static_cast<double>(f[3]), bits);
            p.lastBits = bits;
            p.moving = moving;
        }
    }
}

void* Pads34Object(int index, int which)
{
    if (!g_padsOn || !g_playersOn || (index != 2 && index != 3) || index >= static_cast<int>(g_players))
        return nullptr;
    Pad34& p = g_p34[index - 2];
    return which == 0 ? static_cast<void*>(p.input) : which == 1 ? static_cast<void*>(p.bufA) : static_cast<void*>(p.bufB);
}

DWORD WINAPI PlayersHeartbeat(LPVOID)
{
    for (int beat = 0; beat < 200; ++beat)
    {
        Sleep(30000);
        uintptr_t chars = 0;
        uint32_t count = 0;
        char who[200] = {};
        size_t at = 0;
        if (SpawnScanCharacters(&chars, &count))
            for (uint32_t c = 0; c < count && c < 64; ++c)
            {
                const uintptr_t ch = chars + static_cast<uintptr_t>(c) * 0xE8;
                const int index = static_cast<int>(ReadU32(ch + 0x9C));
                if (index < 0 || index > 3 || at + 40 >= sizeof(who))
                    continue;
                const int w = snprintf(who + at, sizeof(who) - at, " [char %u: player %d, team %d, unit %08X]", c,
                                       index + 1, static_cast<int>(ReadU32(ch + 0xA0)), ReadU32(ch + 0xB4));
                if (w > 0)
                    at += static_cast<size_t>(w);
            }
        LONG guarded = 0;
        for (int i = 0; i < g_guardCount; ++i)
            guarded += g_guardHit[i];
        Log("[players] heartbeat: told %u %ld times, kept 2 %ld times; local slots %d %d %d %d; guarded calls: first "
            "person / music %ld, pause %ld, camera manager GetCamera %ld / SetChaseTarget %ld / death %ld / fade %ld, "
            "HUD displays lent %ld, rumble %ld, damage tint %ld (draws skipped %ld), motion blur %ld; Spawn presses "
            "%ld; local characters:%s",
            g_players, static_cast<long>(g_answered4), static_cast<long>(g_clamped), g_netSlots4[0], g_netSlots4[1],
            g_netSlots4[2], g_netSlots4[3], static_cast<long>(guarded), static_cast<long>(g_pauseGuarded),
            static_cast<long>(g_camGuardHit[0]), static_cast<long>(g_camGuardHit[1]), static_cast<long>(g_camGuardHit[2]),
            static_cast<long>(g_camGuardHit[3]), static_cast<long>(g_hudAliased), static_cast<long>(g_rumbleGuarded),
            static_cast<long>(g_filterGuarded), static_cast<long>(g_filterDrawSkipped), static_cast<long>(g_blurGuarded),
            static_cast<long>(g_spawnPresses),
            at ? who : " none (no match)");
        if (g_padsWanted)
            Log("[pad34] heartbeat: %s; pad 3 XInput %d %s (%lu packets), pad 4 XInput %d %s (%lu packets); control "
                "states answered for ids 2 / 3: %ld", g_padsOn ? "ON" : (g_padsFailed ? "NOT built" : "not built yet"),
                g_p34[0].xinput, g_p34[0].connected ? "connected" : "-", g_p34[0].copies, g_p34[1].xinput,
                g_p34[1].connected ? "connected" : "-", g_p34[1].copies, static_cast<long>(g_controlOwnAnswers));
        if (g_padsOn) // 20g-r8
        {
            char t[2][120] = {};
            for (int k = 0; k < 2; ++k)
            {
                const uintptr_t in = reinterpret_cast<uintptr_t>(g_p34[k].input);
                snprintf(t[k], sizeof(t[k]), "player %d mode %u, flips %u %u %u, look scales %.2f %.2f %.2f", k + 3,
                         ReadU32(in + kInputMode), static_cast<unsigned>(ReadU8(in + kInputFlip)),
                         static_cast<unsigned>(ReadU8(in + kInputFlip + 1)), static_cast<unsigned>(ReadU8(in + kInputFlip + 2)),
                         static_cast<double>(ReadF32(in + kInputPadScale)), static_cast<double>(ReadF32(in + kInputPadScale + 4)),
                         static_cast<double>(ReadF32(in + kInputPadScale + 8)));
            }
            Log("[pad34] 20g-r8 heartbeat: controller options %s; %s; %s", g_ctlOk ? "written" : "NOT written (reads not found)",
                t[0], t[1]);
        }
        Log("[chase34] heartbeat: chase cameras %s, %ld frames drawn through them; targets %08X / %08X; 20g-r6: death cameras "
            "%s, %ld started, %ld frames drawn through them, now %s / %s; spawn screen low detail asked %ld times, %ld of them "
            "not player 1's view (drawn in full detail)",
            g_chaseOk ? "ON" : "OFF", static_cast<long>(g_chaseFrames), static_cast<uint32_t>(g_chaseTarget[0]),
            static_cast<uint32_t>(g_chaseTarget[1]), g_deathOk ? "ON" : "OFF", static_cast<long>(g_deathStarts),
            static_cast<long>(g_deathFrames), g_deathOn[0] ? "on" : "off", g_deathOn[1] ? "on" : "off",
            static_cast<long>(g_lodAsked), static_cast<long>(g_lodSpared));
        Log("[sound34] heartbeat: listeners %s, players 3 / 4's slots %d / %d, %ld frames pointed at their cameras, active "
            "listeners %ld",
            g_lisOk ? "ON" : "OFF", g_lis[0] ? static_cast<int>((g_lis[0] - g_lisArray) / kListenerBytes) : -1,
            g_lis[1] ? static_cast<int>((g_lis[1] - g_lisArray) / kListenerBytes) : -1, static_cast<long>(g_lisFrames),
            g_lisCount ? static_cast<long>(*reinterpret_cast<const volatile int32_t*>(g_lisCount)) : -1L);
        // 20g-r7
        Log("[players] 20g-r7 heartbeat: wall distance of their own %ld eases (now %.2f / %.2f); damage tint for players 3 / "
            "4: %ld flashes, %ld draws; command post lights %s: %ld updates, %ld with a light lent to player 3 / 4, %ld light "
            "masks changed; scope blur slots %s: %ld sets, %ld draws in views 3 / 4 (now %08X / %08X); still death cameras "
            "%ld frames held",
            static_cast<long>(g_wallOwn), static_cast<double>(g_wallDist[0]), static_cast<double>(g_wallDist[1]),
            static_cast<long>(g_tintFlashes), static_cast<long>(g_tintDraws), g_cpOk ? "ON" : "OFF",
            static_cast<long>(g_holoUpdates), static_cast<long>(g_holoRemapped), static_cast<long>(g_holoMasks),
            g_scopeOk ? "ON" : "OFF", static_cast<long>(g_scopeSets), static_cast<long>(g_scopeDraws), g_scopeBlur[0],
            g_scopeBlur[1], static_cast<long>(g_stillFrames));
        Log("[players] 20g-r10 heartbeat: camera shots %s (player 1's taken %ld times, %s; frames on it: player 2 %ld, player 3 %ld, "
            "player 4 %ld - only while their spawn screens are up); collision around players 3 / 4 %s (%ld tests, %ld nearer one of "
            "them)",
            g_shotOk ? "ON" : "OFF", static_cast<long>(g_shotCaptures), g_p1ShotValid ? "one this match" : "none this match",
            static_cast<long>(g_shotFrames[0]), static_cast<long>(g_shotFrames[1]), static_cast<long>(g_shotFrames[2]),
            g_collOn ? "ON" : "OFF", static_cast<long>(g_collCalls), static_cast<long>(g_collNearer));
        if (g_origEmitterDraw)
            Log("[fx34] heartbeat: particle emitter draws since the start - view 1: %ld, view 2: %ld, view 3: %ld, view 4: %ld, "
                "other: %ld (players 1 / 2 / 3 / 4 are views 1 / 2 / 3 / 4)",
                static_cast<long>(g_fxDraws[0]), static_cast<long>(g_fxDraws[1]), static_cast<long>(g_fxDraws[2]),
                static_cast<long>(g_fxDraws[3]), static_cast<long>(g_fxDraws[4]));
    }
    return 0;
}

} // namespace

// 21j-r33: the sign-in's names for players 3 / 4 (players34.hpp)
int PlayerNSignName(int player, const char* name)
{
    return SignNameSet(player, name);
}

void PlayerNSignNamesClear()
{
    SignNamesClear();
}

void PlayerNInputFrame(float dt)
{
    Pads34Frame(dt);
}

void* PlayerNInputObject(int index, int which)
{
    return Pads34Object(index, which);
}

int PlayerNPadPlayers()
{
    return (g_padsOn && g_playersOn) ? static_cast<int>(g_players) : 2;
}

uintptr_t PlayerNPadAddress(int p)
{
    if (!g_padsOn || !g_playersOn || p < 2 || p > 3 || p >= static_cast<int>(g_players))
        return 0;
    const Pad34& pad = g_p34[p - 2];
    return pad.connected ? reinterpret_cast<uintptr_t>(pad.pad) : 0;
}

int PlayerNSpecView(int k)
{
    if (k < 0 || k > 1 || !g_quarterMgr || !g_rendCams)
        return -1;
    const int v = g_specView[k];
    return (v >= 0 && RendCamera(v) == reinterpret_cast<uintptr_t>(g_specCam[k])) ? v : -1;
}

// 20g-r6 (ragdoll.cpp): 3 / 4 while players 3 / 4 are installed (Players=3 / 4), else 0. 21g-r2: this match's count (2 when
// 2 players signed in - every caller asks for 3 / 4 or more)
int PlayerNLocalPlayers()
{
    return g_playersOn ? static_cast<int>(g_players) : 0;
}

// 21g-r2: the most players a match can have (Players, installed) - 0 while players 3 / 4 are not on
int PlayerNPlayersMax()
{
    return g_playersOn ? static_cast<int>(g_playersMax) : 0;
}

// 21g-r2: the sign-in screen's LAUNCH (the game thread, in the shell): the next matches' player count and the pads that
// signed in as players 3 / 4. Answers the count set (0: players 3 / 4 are not on - nothing changed).
int PlayerNSignIn(int count, int padP3, int padP4)
{
    const LONG n = InterlockedIncrement(&g_signIns);
    if (!g_playersOn)
    {
        if (n <= 20)
            Log("[players] 21g-r2: the sign-in asked for %d players - players 3 / 4 are not on (Players=2 or not installed): "
                "nothing changed", count);
        return 0;
    }
    const int asked = count;
    if (count < 2)
        count = 2;
    if (count > static_cast<int>(g_playersMax))
        count = static_cast<int>(g_playersMax);
    const uint32_t was = g_players;
    const int viewsWas = g_viewsLive;
    g_players = static_cast<uint32_t>(count);
    g_viewsLive = count >= 4 ? 4 : count;
    if (!g_viewsReady)
        g_viewsLive = 2;
    g_padPref[0] = count >= 3 ? padP3 : -1;
    g_padPref[1] = count >= 4 ? padP4 : -1;
    // the pads looked for again at once: the signed-in ones first, a player who is not in this match lets his go
    for (int k = 0; k < 2; ++k)
        if (k + 3 > count || (g_padPref[k] >= 0 && g_p34[k].xinput != g_padPref[k]) ||
            (g_padPref[1 - k] >= 0 && g_p34[k].xinput == g_padPref[1 - k])) // (he holds the other's signed-in pad)
            g_p34[k].xinput = -1;
    g_padsLastPick = 0;
    if (n <= 40)
        Log("[players] 21g-r2: the sign-in: %d players signed in%s - the matches from now: %d players (was %u), %s (was %s); "
            "player 3's pad XInput %d, player 4's XInput %d (-1: the first free pad); Players=%u installed",
            asked, asked != count ? " (held to what is installed)" : "", count, was,
            g_viewsLive == 4 ? "quarters" : g_viewsLive == 3 ? "the 3-player layout (player 3 in the bottom half)"
                                                             : "the game's own 2-player split",
            viewsWas == 4 ? "quarters" : viewsWas == 3 ? "the 3-player layout" : "the 2-player split", g_padPref[0],
            g_padPref[1], g_playersMax);
    return count;
}

// 20g-r6: player k + 3's own MapDisplay (split_spawn_menus.cpp: his spawn point screen's map), and its show / hide
uintptr_t PlayerNHudMap(int k)
{
    return HudMapOf(k);
}

bool PlayerNHudMapShow(int k, bool on)
{
    return k >= 0 && k <= 1 && HudMapOf(k) && HudShowOne(2, k, on);
}

// 20g-r6: player k + 3's pause screen up / down (split_spawn_menus.cpp). 21h-r3: when it goes, the calm packet is held until his
// buttons are up (as after his real pause menu)
void PlayerNSetPaused(int k, bool on)
{
    if (k < 0 || k > 1)
        return;
    const LONG was = InterlockedExchange(&g_p34Paused[k], on ? 1 : 0);
    if (was && !on)
        InterlockedCompareExchange(&g_p34Menu[k], 2, 0);
}

// 21h-r3: player k + 3's real pause menu up (his pad drives it, his soldier gets a calm packet) / gone (the calm packet held
// until his buttons are up)
void PlayerNMenuInput(int k, bool up)
{
    if (k >= 0 && k <= 1)
        InterlockedExchange(&g_p34Menu[k], up ? 1 : 2);
}

// 20g-r8: players 3 and 4's controller options (split_spawn_menus.cpp's OPTIONS page)
PlayerNControls PlayerNControlsGet(int k)
{
    PlayerNControls c;
    if (k >= 0 && k <= 1)
    {
        c.invert = g_ctl[k].invert;
        c.invertFly = g_ctl[k].invertFly;
        c.sens = g_ctl[k].sens;
    }
    return c;
}

void PlayerNControlsSet(int k, const PlayerNControls& c)
{
    if (k < 0 || k > 1)
        return;
    g_ctl[k].invert = c.invert ? 1 : 0;
    g_ctl[k].invertFly = c.invertFly ? 1 : 0;
    g_ctl[k].sens = CtlClampSens(c.sens);
    InterlockedIncrement(&g_ctlStamp[k]);
}

bool PlayerNControlsLive()
{
    return g_ctlOk && g_padsOn;
}

bool PlayerNControlsSave(int k)
{
    if (k < 0 || k > 1 || !g_iniPath[0])
        return false;
    const Ctl34 c = g_ctl[k];
    const struct
    {
        const char* name;
        int value;
    } kKeys[3] = { { "InvertLookP%d", c.invert }, { "InvertFlyingP%d", c.invertFly }, { "SensitivityP%d", CtlClampSens(c.sens) } };
    bool ok = true;
    DWORD err = 0;
    for (const auto& kk : kKeys)
    {
        char key[32];
        char val[16];
        snprintf(key, sizeof(key), kk.name, k + 3);
        snprintf(val, sizeof(val), "%d", kk.value);
        if (!WritePrivateProfileStringA("Splitscreen", key, val, g_iniPath))
        {
            ok = false;
            err = GetLastError();
        }
    }
    Log("[pad34] %lu ms: player %d's controls saved to BF1GameExt.ini [Splitscreen]: InvertLookP%d=%d InvertFlyingP%d=%d "
        "SensitivityP%d=%d%s",
        ScanMsSinceAttach(), k + 3, k + 3, c.invert, k + 3, c.invertFly, k + 3, CtlClampSens(c.sens),
        ok ? "" : " - NOT all written (the game folder may be read-only): they last this session only");
    if (!ok)
        Log("[pad34] (WritePrivateProfileString error %lu)", static_cast<unsigned long>(err));
    return ok;
}

// 20g-r5: players 3 and 4's own StatusDisplays (stamina_hud.cpp)
int PlayerNHudStatusPlayer(uintptr_t display)
{
    if (!g_hudOn || !display)
        return -1;
    const Hud34Run& r = g_hr[6];
    for (int k = 0; k < 2; ++k)
        if (r.on && r.setUp[k] && r.ours[k] == display)
            return k + 2;
    return -1;
}

const char* PlayerNScanFileBuild()
{
    return kFileBuild;
}

void PlayerNScanStart(bool onGameThread)
{
    (void)onGameThread;
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[playerN] not used on this build");
        return;
    }
    bool wanted = true;
    {
        char exe[MAX_PATH] = {};
        char ini[MAX_PATH + 32] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
                *slash = 0;
            snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
            wanted = GetPrivateProfileIntA("Splitscreen", "PlayerNScan", 1, ini) != 0;
            g_viewsWanted = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "Views", 2, ini));
            g_playersWanted = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "Players", 2, ini));
            g_spawnWanted = GetPrivateProfileIntA("Splitscreen", "AutoSpawnP34", 1, ini) != 0;
            g_padsWanted = GetPrivateProfileIntA("Splitscreen", "PadsP34", 1, ini) != 0;
            g_hudWanted = GetPrivateProfileIntA("Splitscreen", "HudP34", 1, ini) != 0; // 20g-r3
            g_hudMask = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "HudP34Mask", kHudMaskDefault, ini)) & 0xFF;
            g_namesWanted = GetPrivateProfileIntA("Splitscreen", "NamesP34", 1, ini) != 0;   // 21j-r33
            g_waterWanted = GetPrivateProfileIntA("Splitscreen", "WaterScan", 1, ini) != 0;  // 21j-r33 (log only)
            g_waterFixMode = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "WaterFix", 1, ini)); // 21j-r36
            g_uwFixMode = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "UnderwaterFix", 1, ini)); // 21j-r37
            g_sfhMode = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "SplitFovH", 0, ini));          // 21j-r39
            g_sfhP3Mode = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "SplitFovP3", 0, ini));       // 21j-r40
            {
                const int pct = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "SplitFovScale", 110, ini)); // 21j-r42
                g_sfhPct = pct < 80 ? 80 : (pct > 150 ? 150 : pct);
            }
            {
                const int tall = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "SplitFovTall", 45, ini)); // 21j-r41
                g_sfhTallDeg = tall < 20 ? 20 : (tall > 70 ? 70 : tall);
                g_sfhTallTan = tanf(static_cast<float>(g_sfhTallDeg) * 0.5f * 0.017453293f);
            }
            g_lodMode = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "SpawnLodP1Only", 1, ini)); // 20g-r6
            g_deathWanted = GetPrivateProfileIntA("Splitscreen", "DeathCamP34", 1, ini) != 0;            // 20g-r6
            g_lisWanted = GetPrivateProfileIntA("Splitscreen", "SoundP34", 1, ini) != 0;                 // 20g-r6
            g_wallWanted = GetPrivateProfileIntA("Splitscreen", "WallCamP34", 1, ini) != 0;              // 20g-r7
            g_tintWanted = GetPrivateProfileIntA("Splitscreen", "TintP34", 1, ini) != 0;                 // 20g-r7
            g_cpWanted = GetPrivateProfileIntA("Splitscreen", "CpLightsP34", 1, ini) != 0;               // 20g-r7
            g_scopeWanted = GetPrivateProfileIntA("Splitscreen", "ScopeP34", 1, ini) != 0;               // 20g-r7
            g_stillWanted = GetPrivateProfileIntA("Splitscreen", "DeathCamStill", 0, ini) != 0;          // 20g-r7 (r10: 0)
            g_shotWanted = GetPrivateProfileIntA("Splitscreen", "CamShotP234", 1, ini) != 0;             // 20g-r9
            g_endShotWanted = GetPrivateProfileIntA("Splitscreen", "EndShotP34", 1, ini) != 0;           // 21j-r15
            g_collWanted = GetPrivateProfileIntA("Splitscreen", "CollisionP34", 1, ini) != 0;            // 20g-r9
            g_scopeScanWanted = GetPrivateProfileIntA("Splitscreen", "ScopeScan", 1, ini) != 0;          // 21j-r4
            g_scopeFitWanted = GetPrivateProfileIntA("Splitscreen", "ScopeFit", 1, ini) != 0;            // 21j-r5
            g_scHalfFill = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "ScopeHalfFill", 96, ini)); // 21j-r6
            if (g_scHalfFill < 0)
                g_scHalfFill = 0;
            if (g_scHalfFill > 125)
                g_scHalfFill = 125;
            g_retWanted = GetPrivateProfileIntA("Splitscreen", "ReticleScan", 1, ini) != 0;            // 21j-r6
            g_scBlurFixWanted = GetPrivateProfileIntA("Splitscreen", "ScopeBlurFix", 1, ini) != 0;     // 21j-r7
            g_retFixWanted = GetPrivateProfileIntA("Splitscreen", "ReticleFixP34", 1, ini) != 0;       // 21j-r7
            {
                int ms = static_cast<int>(GetPrivateProfileIntA("Splitscreen", "DeathCamStillMs", 1500, ini));
                if (ms < 0)
                    ms = 0;
                if (ms > 10000)
                    ms = 10000;
                g_stillAfter = static_cast<float>(ms) / 1000.0f;
            }
            if (!g_scopeWanted)
                g_hudMask &= ~(1 << kHudScope);
            // 20g-r8: players 3 and 4's controller options (their pause screen's OPTIONS page saves them here)
            snprintf(g_iniPath, sizeof(g_iniPath), "%s", ini);
            for (int k = 0; k < 2; ++k)
            {
                char key[32];
                snprintf(key, sizeof(key), "InvertLookP%d", k + 3);
                g_ctl[k].invert = GetPrivateProfileIntA("Splitscreen", key, 0, ini) != 0 ? 1 : 0;
                snprintf(key, sizeof(key), "InvertFlyingP%d", k + 3);
                g_ctl[k].invertFly = GetPrivateProfileIntA("Splitscreen", key, 1, ini) != 0 ? 1 : 0;
                snprintf(key, sizeof(key), "SensitivityP%d", k + 3);
                g_ctl[k].sens = CtlClampSens(static_cast<int>(GetPrivateProfileIntA("Splitscreen", key, 5, ini)));
            }
        }
    }
    if (g_lodMode < 0 || g_lodMode > 2)
        g_lodMode = 1;
    if (g_viewsWanted != 3 && g_viewsWanted != 4)
        g_viewsWanted = 2;
    if (g_playersWanted != 3 && g_playersWanted != 4)
        g_playersWanted = 2;
    if (g_viewsWanted == 3 && g_playersWanted == 4) // 20g-r13: the 3-player layout has no fourth view
        g_playersWanted = 3;
    g_players = static_cast<uint32_t>(g_playersWanted); // lets the count's thunk hook go in (HookKind)
    Log("[playerN] players34.cpp file build %s - 4-PLAYER MAP (20a, log only) + 4 VIEWS (20b) + PLAYERS 3 AND 4 "
        "(20c): [Splitscreen] PlayerNScan=%d, Views=%d (2 = nothing of 20b installed; 20g-r13: 3 = the 3-player layout, "
        "player 3 in the bottom half), Players=%d (2 = nothing of 20c installed; 3 with Views=3)",
        kFileBuild, wanted ? 1 : 0, g_viewsWanted, g_playersWanted);
    Log("[playerN] 20g-r7: [Splitscreen] WallCamP34=%d TintP34=%d CpLightsP34=%d ScopeP34=%d DeathCamStill=%d "
        "DeathCamStillMs=%d HudP34Mask=%d",
        g_wallWanted ? 1 : 0, g_tintWanted ? 1 : 0, g_cpWanted ? 1 : 0, g_scopeWanted ? 1 : 0, g_stillWanted ? 1 : 0,
        static_cast<int>(g_stillAfter * 1000.0f + 0.5f), g_hudMask);
    Log("[playerN] 20g-r9: [Splitscreen] CamShotP234=%d CollisionP34=%d (20g-r10: the shot from the frame their spawn screen "
        "comes up; DeathCamStill=%d); 21j-r15: EndShotP34=%d (%s)", g_shotWanted ? 1 : 0, g_collWanted ? 1 : 0, g_stillWanted ? 1 : 0,
        g_endShotWanted ? 1 : 0,
        g_endShotWanted ? "at the match end players 3 / 4's views show players 1 / 2's cameras - the game's end shots"
                        : "players 3 / 4's views stay their own at the match end");
    Log("[playerN] 20g-r8: [Splitscreen] InvertLookP3=%d InvertFlyingP3=%d SensitivityP3=%d, InvertLookP4=%d InvertFlyingP4=%d "
        "SensitivityP4=%d (players 3 / 4's controller options - their pause screen's OPTIONS page changes and saves them)",
        g_ctl[0].invert, g_ctl[0].invertFly, g_ctl[0].sens, g_ctl[1].invert, g_ctl[1].invertFly, g_ctl[1].sens);
    if (!wanted && g_viewsWanted < 3)
        return;
    g_censusOn = wanted;
    g_base = ExeBase();
    g_imageEnd = g_base + build->sizeOfImage;
    FindMap();
    FindHud34(); // 20g-r1: the HUD scan for players 3 and 4 (log only)
    InstallScopeScan(); // 21j-r4: the scope scan (log only); 21j-r5: the scope fits each split view (any player count)
    InstallSplitFovH(); // 21j-r39: the 2-player top / bottom halves' FOV (BF2's)
    if (wanted)
    {
        int hooked = 0;
        for (int k = 0; k < kKinds; ++k)
            if (g_hooked[k])
                ++hooked;
        Log("[playerN] census on %d of %d entry points (GOG 1.3: all %d)", hooked, static_cast<int>(kKinds),
            static_cast<int>(kKinds));
    }
    if (g_viewsWanted >= 3) // 20g-r13: 3 = the 3-player layout
        InstallViews();
    if (g_playersWanted > 2)
    {
        InstallPlayers();
        if (g_playersOn)
            InstallHud34(); // 20g-r3: players 3 and 4's HUD
        if (g_playersOn)
            InstallNames34(); // 21j-r33: players 3 and 4's name tags
        if (g_playersOn && g_viewsReady)
            InstallViewToCam(); // 20g-r5: views 3 / 4 -> cameras 2 / 3
        if (g_playersOn && g_viewsReady)
        {
            InstallCpLights();   // 20g-r7: command post lights per view
            InstallDeathStill(); // 20g-r7: players 2 - 4's death cameras hold still
            InstallFxDiag();     // 20g-r7: [fx34] (log only)
            InstallShots();       // 20g-r9: players 2 - 4 on player 1's camera shot while they have no soldier
            InstallCollision34(); // 20g-r9: objects near players 3 / 4's cameras collide too
            InstallReticleScan(); // 21j-r6: the reticle scan (log only)
            InstallWaterScan();   // 21j-r33: the water scan (log only)
            InstallWaterFix();    // 21j-r36: each split view's ripple pre-pass over the whole texture its water reads
            InstallUnderwaterFix(); // 21j-r37: each view's underwater tint over its own view
        }
        if (g_playersOn && g_padsWanted)
            CtlCheck(); // 20g-r8: players 3 / 4's controller options (the game's two reads, checked at start)
        if (g_playersOn)
        {
            const HANDLE h = CreateThread(nullptr, 0, PlayersHeartbeat, nullptr, 0, nullptr);
            if (h)
                CloseHandle(h);
        }
    }
    InstallSpawnLod(); // 20g-r6: the spawn screen's low detail only in player 1's view (logs why when not)
    if (g_playersOn && g_viewsReady) // 20g-r6: players 3 and 4's own sound listeners
    {
        if (!g_lisWanted)
            Log("[sound34] [Splitscreen] SoundP34=0 - players 3 and 4 hear through players 1 / 2's listeners");
        else
        {
            g_lisOk = FindListeners();
            Log("[sound34] players 3 and 4's own listeners %s", g_lisOk ? "ON (each one at his view's camera, as players 1 / 2's)"
                                                                       : "OFF (something above did not check out)");
        }
    }
    if (!wanted)
        return;
    ReportTables("at start");
    const HANDLE t = CreateThread(nullptr, 0, ReporterThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
    else
        Log("[playerN] reporter thread NOT started (error %lu)", GetLastError());
}

} // namespace bf1
