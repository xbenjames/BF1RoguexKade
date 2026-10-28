// BF1GameExt - class_wheel.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was classwheel_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r20: THE MOUSE READS THE GAME'S OWN CURSOR. r19's log: every click was taken at the Windows cursor, not the arrow the game
// draws (the game moves its cursor by the mouse's raw counts and leaves the Windows one alone), so the sides and the posts were
// missed. The game's cursor (the input manager's GetBufB(0) +138h / +13Ch) was never read: GetBufB is BF1GameExt's own since 6c
// (player 2's redirect, index 0 still the game's) and the vfunc check took only the exe's code. ReadMouse now takes an
// executable vfunc outside .text too, the window's size if the display's is not usable, keeps the cursor to the screen, and logs
// how it read it ([wheel] "mouse: the game's cursor used / NOT used: ..."); a missed side click logs both sides' boxes.
// Nothing else changed.
// 21j-r19: THE MOUSE AS THE CLASSIC COLLECTION'S ([ClassWheel] MouseClicks=1, 0 = as r18) - click what you mean. Soldier select:
// a click on the wheel left of the picked soldier turns it left, right of it right (one class, as the left / right keys; the
// wheel's area runs from the screen's left edge to a little past the ring's right-hand soldier - RingSideAt), "SELECT
// CHARACTER" spawns (the front soldier no longer does). Spawn point screen: a click on one of his team's posts on the map picks
// it - the game's own test, what its PC screen does with a click on its map (SpawnDisplay::PostAt, found from that click in
// PcInput: the post icons' hot-spots, set where each is drawn every frame, so it follows the moved map), then SetSpawnPoint as
// the keys do; "SELECT SPAWNPOINT" goes on; anywhere else nothing (it went on before). Side screen: only a click on a side
// joins it. BACK and the right button go back as before; players 3 / 4's look keeps r18's clicks. [wheel] "mouse: click"
// lines say what each click hit. Nothing else changed.
// 21j-r18: [ClassWheel] Enabled is single player only now - the main menu's GAME OPTIONS SPAWN SCREEN row (menus.cpp's
// script, [Splitscreen] SpawnScreenMenu=1) switches it: CLASS WHEEL (the console flow) / PC MENU (the game's own PC spawn screen),
// saved in BF1GameExt.ini, from the next spawn screen on (ClassWheelMenuState / ClassWheelMenuSet). Split screen keeps player 1's
// console flow by [ClassWheel] Split alone (ClassWheelSplitWanted no longer needs Enabled; the flow state starts at the side
// screen when either is on; ConsoleActive / ClassWheelPreviewScreenImpl take Enabled for 1 player, Split for split screen).
// Nothing else changed.
// 21j-r15: ClassWheelFlashyLike - the copy keeps the constructor's animation state (6: done - the whole bar and its text). It took
// the game's bar's with the rest of its fields: copied while the game's screen was flashing it in (its own Enter - player 1's
// windows on the 4-player spawn point screen are made in the frame his screen comes up), the copy stayed at the animation's
// start forever (nothing runs a copy's animation): a stub of orange, no title. Nothing else changed.
// 21j-r9: (1) THE 1-PLAYER CLASS RING FURTHER LEFT, off the class window ([ClassWheel] RingShift, in percent of the game's
// soldier's x: -31 now, -13 before - the ring's right-hand soldiers stood under the window's left edge); "SELECT SOLDIER"
// moves with it (half the shift, of the screen's width). The , / . keys still tune it live. (2) For split_spawn_menus.cpp's
// console windows on players 1 - 4's split screen spawn screens: ClassWheelBoxTextNew - a text in a window's box (left /
// top, its own font, wrapped in a box: RedTextElement +A0h / +A2h, what the IF Lua's SetTextBox writes - GOG 1.3
// 0x0029EEE0 - set before the string, which is laid out in it) and ClassWheelFlashyBarWidth - a FlashyTextElement's bar
// width (+F0h, a word: what IFFlashyText's Setup writes, GOG 1.3 0x00177F20).
// 21h-r6: PLAYER 1'S PAUSE ON HIS SPAWN SCREENS ([ClassWheel] PauseLatch=1). His console flow does not run while the game is
// paused (the spawn screen's update stops), so the A that picked RESUME GAME (or the B that closed the pause) was still
// down on its first frame back - a fresh press to it: the side was joined, the spawn point taken, or he was spawned. Now,
// when his flow's frames come back after a gap (a pause), the buttons held then are latched (as at the flow's start, and as
// player 2's screens do when they are on top again): only a new press counts. [wheel] "back after ... - buttons latched".
// 21f-r1: LOCKED SIDES - while the game says sides cannot be switched (the shell turns it off for the campaign and
// Galactic Conquest; menus.cpp's SidesLockedNow, [Splitscreen] SideLock=1) player 1's console flow has no side
// screen: a new match starts at the spawn point screen (his side is the game's), and B there does not go back to it.
// 1-player and split screen alike. Nothing else changed.
// 20g-r12 (from the user's horizontal shots against the Classic Collection's video): HORIZONTAL 2-PLAYER (MenusHorizontal=1
// with Vertical=0; ClassWheelHorizontal). Everything 2-player was laid out for the vertical halves (512 x 624 roots); the
// horizontal halves are 1024 x 312. Classic's horizontal split has the game's own spawn screens - the turned panels and
// class window, "B BACK", the spawn point map in its own layout - in a root whose safe area keeps 10% at the top and the
// bottom of each half (split_spawn_menus.cpp sets it). So in horizontal: player 1's panels, class window and "B BACK" are no
// longer moved or set flat; his side screen's soldiers and texts, his ring (Classic's arrangement: the picked one in front
// in the middle - ring style kRingStyleClassic, ClassicRingRel / ClassicRingRise) and his prompts are placed where Classic
// has them through his HUD root (split_spawn_menus.cpp's P2SpawnGroupPlace, the kHz* fractions in the header); both players'
// spawn point maps (layouts 5 / 4) at Classic's place and size. Vertical and 4-player unchanged.
// 20g-r11 (from r10's video): (1) THE DROIDEKA ON THE RINGS OF PLAYERS 2 - 4: it drew exactly where a soldier at
// fix + (x - fix) * (soldier +84h / droideka +84h) would (0.00469 / 0.00563 = 0.83) - its own +84h does not move it in
// those groups, so r9's mapping pulled it 17% toward (512, 520): below the ring and in front of it. RingFrame2 now stands
// a droideka at its slot as a soldier (and its depth as a soldier's). (2) PLAYER 1'S SPAWN POINT AND CLASS SCREENS IN
// PLAYERS 3 / 4'S LOOK (4-player): P1Look - his console screens still run (inputs, CyclePost, SelectClass, the spawn),
// but the spawn point screen's SideBar panels / "B BACK", the class screen's window (the orange class bar) / "B BACK",
// our screen texts and his ring are not shown, the game's own soldier stays hidden, the spawn screen's blue grid is
// hidden on all three screens; his map (layout 5) stands where players 3 / 4's stand in their quarters (138 x 148, anchor
// 0.28 / 0.439 - his root starts 0.08 lower than player 3's); split_spawn_menus.cpp draws the rest (ClassWheelP1Look; ring 3
// is his).
// 20g-r10: 4-player (MenusLikeP34=1) - player 1's side screen drawn by split_spawn_menus.cpp in players 3 / 4's look
// (ClassWheelSideLookAllow / ClassWheelSideLookUp): SideUpdate still picks (left / right, the mouse click) and joins
// (SelectTeam), but makes none of its soldiers / texts, and hides the spawn screen's own blue grid (+2210h) while it is
// up (shown again after). The look's group cannot be made: his own side screen as r9.
// 20g-r9: (1) ClassWheelFlashyLike - a FlashyTextElement (the console menus' text with its bar behind it: the pause
// menu's orange title bar and blue item bars) made like one of the game's: its no-argument constructor (GOG 1.3
// 0x001306E0, by pattern, its vtable's RTTI checked), its draw resource made by its own vfunc 30h (as the IF Lua's
// IFFlashyText does), then the copy's text fields and its own +D0h..+F7h (text colour +D4h, bar textures +E0h..+E8h,
// bar margins +ECh, flags +F4h) from the game's. (2) ClassWheelFindKind / ClassWheelDumpTree - an element tree searched
// / logged with every element's place. (3) ClassWheelPlaySound says whether it played. (4) 4-PLAYER RING like Battlefront
// II's 4-player wheel: player 1's ring's radius twice r7's (0.75 -> 1.5 of the split one), its picked soldier 0.40.
// 20g-r7: ClassWheelTextNewEx (a text with its own justify and colour) and ClassWheelTextColor - players 3 and 4's pause
// screens in the look of the game's (split_spawn_menus.cpp). 4-PLAYER: player 1's side soldiers and class ring at players 3 /
// 4's sizes (his screens are drawn in his quarter now - render_hooks.cpp - a box the size of his 2-player one, so the
// split look's soldiers stood about 60% of his quarter tall, players 3 / 4's about 40%): side soldiers 0.34 of the game's
// soldier (split 0.54), the ring's picked soldier 0.36 (0.508) and its radius 0.75 of the split one - the same steps
// player 2's screens take (split_spawn_menus.cpp). [Splitscreen] MenusLikeP34=1.
// 20f-r4: ClassWheelGroupNew makes a RedScreenGroupElement (0xB0 bytes: a group with a HUD root's anchor +A0h / +A4h and
// flag +A8h), not a plain RedGroupElement (0xA0) - the game's AddToRoot sets those three fields, and on r2 / r3's 0xA0
// group they went over the next heap block's header (the r2 / r3 crash). [wheel] 20f-r4 line.
// 20f-r2 (players 3 and 4's spawn screens, split_spawn_menus.cpp): ClassWheelGroupNew - an empty RedGroupElement of the
// game's (its default constructor found by pattern, its vtable's RTTI name and its deleting destructor's size checked at
// start; [wheel] 20f line); player 2's class ring is now one of three (ClassWheelRingFrame / ClassWheelRingClear: rings 1
// and 2 = players 3 and 4, the same code; ring 0 = player 2's, ClassWheelRing2Frame / Clear as before). Player 1's and
// player 2's screens are unchanged.
// 19i (19h-r2's log and video): (1) NO GLITCH FRAME on player 1's console screens: the switch the chooser
// queues is now made and settled in the hook before ClassWheelFrame (spawn_ui.cpp), so the new
// screen is set up before it is first drawn. The side screen's own elements are no longer deleted the
// frame A is pressed (that frame drew an empty side screen): they go the next frame, after the switch
// (SideClearFor at the top of ClassWheelFrame, as when B leaves it). The game's own soldier stays hidden
// on the side and class screens from the first frame (ChooserShowCurrent: it showed for one frame on
// the way to the class screen, before the ring was built). (2) SOUNDS: the tick list starts with the
// names 19h-r2's log found in the sound banks (shell_select_change, shell_select_unit - the old list
// had none, so left / right never ticked, for either player); the first plays are logged with the name.
// 19h-r2 (19h's log and video): (1) the flat layout never ran - it looked for the orange title bar in
// a panel's skin, but the skin is only the blue box (507 x 30 under COMMAND POST) and the bar is the
// title text's own (a FlashyTextElement, from the title's y down to the box). Each panel is now placed
// by its title: scale 0.6, the bar's top left at the Xbox's place, the box under the OBJECTIVES bar and
// the class window's stretched to the Xbox's bottom (the skin group's y rows). The bitmaps' corners go
// through their elements' matrices now, and the first panel's are listed in the log. (2) 19g's places
// (the group pinned) on the first frame the screen is up, the flat layout over them: the first build
// pinned nothing until its 30th try, so the game's own layout slid in for half a second. (3) Player 2's
// root has no x offset (0.02 was measured in his turned Info group).
// 19h: (1) FLAT PANELS AND WINDOW: 19g's log - the console screens' SideBar group (the spawn point
// screen's panels) and Info group (the class window) are turned 42 degrees about the vertical axis;
// the Xbox's are flat. ClassWheelXbPanels / ClassWheelXbWindow set the group straight and scale and
// place each panel / the window from its own size (the shown bitmaps under it, checked against its
// title) to the Xbox's place in the player's half; 19g's moves when a size does not check out.
// ClassWheelFlatPlace: a place in a straight group. (2) THE XBOX'S RINGS: in split screen both rings
// stand as the Xbox's - the picked soldier 32 degrees round to the right of the centre and biggest, the
// others smaller behind (XbRingRel), feet level, little depth (19g's depth moved the back ones right).
// (3) No "B BACK" on player 1's spawn point screen. (4) ClassWheelVehiclesAt: the vehicles at any post
// through the game's own PopupFill (VehTextHook keeps the string instead of setting it), for player 2's
// VEHICLES box. (5) The split maps 157 x 168 (19g 176), player 2's anchor 0.52; player 1's side
// soldiers' feet 3% lower.
// 19g: BOTH SPAWN SCREENS LIKE THE XBOX'S VERTICAL SPLIT. (1) THE MAPS: with 2 players local the map
// picks layout 5 when its flag (+1F289h) is set (player 1's on his spawn screens) and layout 4 in
// mode 1 with the flag clear (player 2's, set by split_spawn_menus.cpp) - GOG 1.3 0x001873C0; 18z
// changed layout 1, which split screen never uses, so nothing moved. Layout 5 now takes the Xbox
// place on player 1's spawn point screen and goes off the screen on his side / class screens (the
// game shows his map again every frame, so hiding it never held); layout 4 takes the Xbox place
// while player 2's spawn point screen is up (ClassWheelSplitMapBig). Both as the game had them
// otherwise. (2) THE BLUE GRID: the spawn screen's "shell_cube" model (SpawnDisplay +2210h, a
// Red3DModelElement the PC spawn screen makes for player 1 only) - ClassWheelCubeNew makes a copy
// for player 2's screens with the same constructor and SetModel (both found from the spawn
// screen's own code, byte-checked; the copy's type checked by name). (3) PLAYER 2'S RING:
// ClassWheelRing2Frame / ClassWheelRing2Clear - his class screen's soldiers on a ring that turns
// like player 1's. (4) Player 1's split-screen layout from 19f's video against the Xbox shots:
// side screen soldiers smaller and apart, names and title where the Xbox has them; the panels
// centred, the window under a smaller ring centred over it; "SELECT SPAWNPOINT" / "SELECT
// CHARACTER" on the "B BACK" row (they were off his half), no "SELECT SOLDIER"; "B BACK" moved in
// and up, hidden on the side screen. All of it split screen only - the 1-player layout is as it
// was before 18z (18z's panel / window moves had reached 1-player games too). (5)
// ClassWheelMoveElement logs the element's matrix (its rotation: the PC tilts the panels).
// 19c: ClassWheelMoveElement also logs the element's group and that group's anchor / position (diagnostic).
// 19b: the panels' and the window's parent groups pinned at 0, 0 before the children are placed (the
// script slides them in to different places). Nothing else changed.
// 19a: layout tune from 18z's video: the panels keep the script's own spacing (they overlapped at the
// Xbox title spacing) shifted down; player 1's half draws about a quarter further right than player
// 2's with the same numbers (his HUD root has an x offset), so his panels, window and ring get their
// own x. Nothing else changed.
// 18z: THE XBOX LAYOUT IN SPLIT SCREEN. The map's drawn element is the group at map +30h (18x hid the
// map object itself: nothing changed): on the side screen and soldier select it is hidden, on the
// spawn point screen it is shown above the panels (its anchor in the root moved up, layout 1 made
// smaller while either player's spawn point screen is up - ClassWheelSplitMapBig). The console
// screens' panels are moved where the Xbox has them (ifs_mapselect1's COMMAND POST / VEHICLES /
// OBJECTIVES centred in the lower half, ifs_charselect1's window centred under the soldiers);
// ClassWheelMoveElement does the same for player 2's screens (split_spawn_menus.cpp).
// 18x: SPLIT SCREEN LAYOUT. In split screen player 1's half is half as wide in the soldiers' units (the
// game's own soldier still stands at x 512, y 520 as in 1 player): the side soldiers and the ring now
// use half the width (the left side soldier was off the screen, the ring off to the right); the map
// element itself is hidden on the side screen and the soldier select (the split layouts are the game's)
// and shown on the spawn point screen. ClassWheelModelNew / ClassWheelModelDelete lend a class's
// soldier (a SoldierElement / DroidekaElement set up like the ring's) to player 2's screens.
// 18w: PLAYER 1'S CONSOLE FLOW IN SPLIT SCREEN. The flow (side -> spawn point -> class wheel) no longer
// needs a 1-player game: the chooser shows ifs_readyselect1 in split screen too (spawn_ui.cpp),
// and ConsoleActive() allows 2 players. Its soldiers, ring and texts already live in the spawn
// screen's own groups, which are in player 1's HUD root (18s), so the game draws them in his half. The
// mouse maps to the left half in split screen (the cursor's x over half the display). The map keeps
// the game's split-screen layout for now (the layouts the split map uses are not mapped yet).
// [ClassWheel] Split=1 (0 = the PC screen in split screen as before).
// 18u: the texts and sounds lent to player 2's spawn screens (split_spawn_menus.cpp): ClassWheelTextsReady,
// ClassWheelSetText, ClassWheelTextLike / TextNew / TextGrey, ClassWheelDeleteElement,
// ClassWheelShowElement, ClassWheelPlaySound (at the end), and ClassWheelSpawnScreenDeletes (the
// spawn screen's deleting destructor, already hooked, now also counts: one per match end). Nothing
// else changed.
// 18q: THE MOUSE AND THE SOUNDS, as the Classic Collection has them. (1) THE MOUSE: the game's own
// cursor position (what its interface hot-spot test reads: player 1's input object, +138h / +13Ch
// in display pixels, found through the Lua ScriptCB_IFObj_TestHotSpot; the display size from the
// same chain) is turned into the interface units the soldiers stand in, and the screens test it
// against their own things. Soldier select: a left click on a soldier of the ring picks that class
// (the ring turns to it); a click on the front (picked) one, or on "SELECT CHARACTER", spawns; a
// click on BACK (bottom left) goes back; a click anywhere else does nothing. Side screen: the side
// under the cursor is the picked one (its soldier lit, its name white), a click joins it. Spawn
// point screen: a click selects the post (BACK goes back). "SELECT CHARACTER" / "SELECT SPAWNPOINT"
// turn yellow under the cursor. The right button is still B, Enter / A and Backspace / B as before.
// Without the game's cursor (not found), the Windows cursor over the game's client area is used.
// (2) THE SOUNDS: each sound (select, back, tick) now has a list of names and the first one the
// loaded sound banks know is used (SetByName leaves the handle's sound pointer 0 for a name it
// does not have - that is the test); the log says which name each resolved to, so the back sound
// (whose name differed) plays. (3) The ring is wider (0.30 of the soldier's x, was 0.23): more room
// between the classes. [ ] still tune it.
// 18p: FINAL TOUCHES. (1) [ClassWheel] Enabled in BF1GameExt.ini: 1 = the console spawn flow comes
// up by itself (side screen first) in 1-player games, F6 still cycles; 0 = the PC spawn screen, F6
// off. (2) The mouse: left button = A (pick a side, a post, a class / spawn), right button = B
// (back). (3) Depth: the ring's soldiers stand at real depths (the element matrix's z, which the
// soldier draw turns into a 3D position like x and y) - back ones further away - so the depth
// buffer sorts weapons and droidekas against the soldier in front; 9 / 0 on the soldier select =
// less / more depth (logged). (4) Sounds: the game's own interface sounds through the same three
// functions its Lua ScriptCB_SndPlaySound uses (found from the Lua function table): A = select,
// B = back, left / right = the menu tick; a log-only scan lists the "shell_..." sound names the
// loaded sound banks have, once per session. (5) Picking a class starts its animation at once (a
// deselected soldier's idle timer is reset, as the game does with its own soldier). (6) Layout
// against the Classic Collection video: ring a little right and down, side soldiers at 40% / 60%
// and lower, names under them, title higher, prompts further right.
// 18o: THE TEXTS SHOW. The game's text element lays its string out when the string is set (SetText,
// which needs the font first), and SetJustify wipes that layout (it zeroes the per-font-page glyph
// counts) - our texts set the string first and the justify after, so they were empty. Now: font,
// justify, scale, colour, THEN the string. Texts go into the spawn screen's own text group (+300h,
// which the spawn screen anchors at the screen centre; children are placed in the interface's
// reference units, read from the game's own reference width / height, with y down):
//   side screen:   "CHOOSE YOUR SIDE" (top centre), each side's name under its soldier (white /
//                  grey); the soldiers stand at 40% and 60% of the width, the unpicked one dark grey
//   spawn point:   the panel titles stay the game's; inside the COMMAND POST box the post's name,
//                  inside VEHICLES the vehicles at that post, inside OBJECTIVES the team's objective
//                  (both strings taken as the game writes them into its PC popup / objective window:
//                  the two SetText calls go through BF1GameExt first and are copied); the HINTS
//                  panel is hidden; "SELECT SPAWNPOINT" at the bottom right
//   soldier select: "SELECT CHARACTER" at the bottom right, "SELECT SOLDIER" at the bottom centre
// Panel texts are the panel's own children, made like ifs_charselect1's BoxText (its font, scale,
// justify, colour, layer) and placed where BoxText sits under its title bar. Texts are deleted when
// their screen is left; when the spawn screen is deleted, texts in the console screens are deleted
// only if their panel is still in the element registry. Tuning on the side screen: [ ] = names
// apart, - = = names up / down, PageUp / PageDown = title up / down, ; ' = text size (logged).
// 18n: (1) MODELS SEEN THROUGH EACH OTHER: a group draws its children from the highest layer value
// down (SetLayer keeps them sorted that way), so the ring drew its front soldier first and the back
// ones over it. The front now gets the lowest value (drawn last), the back the highest. (2) THE
// DARK SIDE, for real: a group draws each child with its colour times the group's, and the soldier
// and droideka draws hand that colour to the model - so the unpicked side is coloured dark grey
// and the picked one white. (3) DROIDEKAS ROLL BACK UP: the droideka's own vfunc 3Ch(0) is its
// roll-up (3Ch(1) unrolls); a class that is no longer selected now gets 3Ch(0) and its frame time,
// so a droideka rolls back into its ball. Soldiers have no reverse in the game's code; they stop.
// (4) THE MAP: it is moved in whichever big layout it is in (1, or 2 when the map's other flag is
// set), not only layout 1. (5) SIDE SCREEN TEXTS, DIAGNOSTIC: the title is made three ways - A in
// the soldiers' group from its own position, B in the text group from the screen centre, C in the
// console screen ifs_readyselect1 itself - and the names A and B ways; the log says where each
// group sits. Whichever shows in the right place is the one that stays.
// 18m: (1) SIDE SCREEN SOLDIERS NO LONGER TURN: +14Ch is not a light, it is where the soldier aims
// (ShowClass sets it straight ahead); turning it round made the unpicked one twist to aim behind it.
// Both keep the game's straight-ahead aim now; the pick shows by the names (white / grey). (2) THE
// TEXTS SHOW: they went into the soldiers' group (+1A0h), which places its children for the 3D
// soldiers; they now go into the group the spawn screen keeps its own texts in (+300h), placed
// from that group's own position, and that group is kept shown while the side screen is up. (3)
// The side soldiers stand where the reference has them (40% and 60% across). (4) Droidekas on the
// ring are bigger (x1.4, as the reference shows them). (5) THE MAP: the spawn screen's map is the
// HUD map in its big 1-player layout (layout 1 of the game's layout table: size, size, x offset,
// y offset); on the side screen and the soldier select it is moved off the screen, on the spawn
// point screen it is moved left and made bigger, and anywhere else the layout is as the game had
// it - each change re-applied with the map's own SetMode. On the spawn point screen [ ] move it
// left / right, PageUp / PageDown up / down, - = smaller / bigger (all logged).
// 18l-r3: the side screen's two soldiers do not animate at all (no per-frame animation calls; they
// keep the pose they are made in). The class wheel is unchanged (only the selected class animates).
// 18l-r2: ONLY THE PICKED ONE ANIMATES - on the class wheel only the selected class's soldier gets
// the spawn screen's per-frame animation calls (vfunc 3Ch(1), vfunc 30h(frame time)); the others
// stop where they are. Changing class starts the new one and stops the old one. The side screen
// does the same: only the picked side's soldier animates.
// 18l: THE SIDE SCREEN, as the Classic Collection has it: "CHOOSE YOUR SIDE", each side's first
// class side by side (the game's own soldier/droideka elements), the side's name under each (the
// name the team itself carries, Team +0Ch), the picked side lit with its name white, the other
// dark (its light turned round) with its name grey; left / right pick, A joins with the spawn
// screen's own SelectTeam (GOG 1.3 rva 0x00078980, what the PC screen's team tabs call), then the
// spawn point screen. The texts are the game's own RedTextElement, made with its constructor
// (found where the spawn screen makes its own +E0h text), font (where the setup sets "gamefont_
// medium"), string (SetText, 18e) and justify (by pattern) functions. The game's soldier is
// hidden on the side screen. Everything is deleted when the side screen goes, or first when the
// spawn screen itself is deleted.
// 18k: THE CLASSIC COLLECTION LOOK, step 1 (measured from the reference video): (1) the ring is
// smaller (front soldier 0.78 of the game's size), its centre left of the screen centre (-0.18),
// raised (0.158 of the game's y), narrower (0.23) with a bigger lift (0.12) and bigger back soldiers
// (0.70); new tuning keys ; ' = front size, PageUp / PageDown = ring height (all logged). (2) The
// spawn point screen shows no soldier: the chooser's show-soldier call (18i-r2) keeps the game's
// soldier and droideka hidden there too. (3) Log-only scan for the side screen to come: the two
// teams' objects (their name fields) and the spawn screen's own two text elements (+20h, +E0h).
// 18j: (1) THE CONSOLE ORDER - side, then where to spawn, then class: ifs_readyselect1 (A) ->
// ifs_mapselect1 (A) -> ifs_charselect1 (A = spawn); B goes back the same way. After a spawn the
// next spawn screen starts at the spawn point screen; a new match starts at the side screen. F6
// cycles in the same order (PC -> side -> spawn point -> soldier select -> PC). (2) THE RING
// ANIMATES: every frame the spawn screen's own update calls its shown soldier's vfunc 3Ch(1)
// (idle animations) and vfunc 30h(frame time) (a droideka's animation); the ring now does the same
// for each of its soldiers, so they play their idle animations instead of standing frozen.
// 18i-r2: (1) THE CIS CRASH: ShowClass gives the element it shows a weapon (class vfunc 28h, when
// the class passes its type check) for droidekas too, and the droideka element's rebuild needs it
// (+A4h); the ring did that for soldiers only, so the droideka's rebuild read a null weapon. Now
// the ring does exactly what ShowClass does (same type check, same weapon), and a droideka with no
// weapon is left off the ring instead of being built. (2) THE DOUBLE MODEL: the chooser (every
// frame in mode 1, right after BF1GameExt's hook at its entry) shows the current soldier again,
// so the game's own soldier came back on top of the ring's front one. That one call in the chooser
// now goes through BF1GameExt and keeps the game's soldier hidden while the ring shows it; no
// redirect, no ring. (3) A console screen is only used if it is loaded in the match (walked in
// the game's live screen list each time the chooser asks); otherwise the PC screen stays.
// 18i: THE WHEEL TURNS - the ring now has one element per class of the team shown (the selected
// one included), made once and kept while the soldier select is up; the game's own soldier is
// hidden meanwhile (shown again when the ring goes). Changing class eases the ring round (shortest
// way, about a quarter of a second) so the selected class turns to the front. Droidekas are on
// the ring too: the game's own DroidekaElement type (constructor and set-class function found
// from the spawn screen's constructor and ShowClass, like the soldier's), placed by the same
// screen offset from where the game puts its droideka. New tuning keys: , and . move the ring's
// centre left / right ([ ] width and - = back size as before; all logged).
// 18h-r3: THE REAL CRASH (both times, ring on or off): the next match's spawn screen is made at
// the same address, and the console texts' elements (charselect title and box, mapselect post
// title) were only looked up again when that address changed - so the second match wrote text
// into the first match's deleted elements (both crashes are inside SetText). Now: the elements are
// looked up every frame (3 registry lookups), and nothing is touched until the spawn screen is set
// up (team shown 1 or 2, character made). The delete hook (r2) was refused because the vtable
// slot points at a jump thunk; the thunk is now followed before the check, so the ring is back.
// The hook also resets the console driver; and when the watcher sees the spawn screen gone, the
// console flow goes back to the side screen for the next match even without the hook.
// 18h-r2: (1) THE GCW CRASH: quitting a match from the pause menu while the ring was up left its
// soldiers in the old spawn screen, and the next match's spawn screen was made at the same
// address, so the ring used them as if they were still good. Now the spawn screen's own deleting
// destructor (its vtable slot 0, called by the game's destroy code) goes through BF1GameExt first:
// the ring's soldiers are deleted while the spawn screen is still whole, and the console flow
// starts at the side screen next match. No hook, no ring. The ring also waits until the spawn
// screen is set up (team shown 1 or 2). (2) SIZE: the draw code uses +84h to turn the screen
// position into a 3D position (depth included), and +80h is the model's size. The ring now keeps
// +84h exactly as the game's soldier has it and makes the back soldiers smaller with +80h; each
// ring soldier also copies the game's soldier's matrix (rotation and depth) and its +88h..+A3h
// (rotation and offset) before its own position is set.
// 18h: THE WHEEL, FIRST LOOK - on the soldier select the team's other classes stand around the
// selected one in a ring (the game's own soldier stays in front). Each is the game's own soldier
// element (class SoldierElement, the one the spawn screen shows its soldier with): made with the
// game's allocator and constructor, set up with the same calls the spawn screen's ShowClass and
// setup use (view, class, weapon, rebuild, idle, scale, position, layer, shown) and added to the
// same draw group (spawn screen +1A0h). Positions are worked out from the game's own soldier
// (its position, size and settings are read, never changed). Droidekas are left out of the ring
// for now. Leaving the soldier select deletes them (the game's own deleting destructor); if the
// match ends while they are up, they are left to the game. While on the soldier select: [ and ]
// make the ring narrower / wider, - and = make the soldiers at the back smaller / bigger (logged,
// for tuning).
// 18g: COMMAND POST CHOICE - on the spawn point screen left / up and right / down pick the
// previous / next command post your team can spawn at, through the game's own NextSpawnPoint (the
// PC "next spawn point" input's function: the team's usable posts in a fixed order, a cursor at
// SpawnManager +A8h) and SetSpawnPoint (the PC map click's setter; the current post is at spawn
// screen +23E0h). The post list is read by running NextSpawnPoint round once and putting its
// cursor back. Until the spawn point panel gets its own text (next step), its title shows the
// chosen post's name (the post's own name, the same text the PC popup window's title uses).
// 18f: CONSOLE FLOW - the console screens become a working flow for 1 player (F6 enters it from
// the PC screen, F6 still cycles): ifs_readyselect1 -> ifs_charselect1 -> ifs_mapselect1 -> spawn.
// Pad (the first connected XInput pad) or keyboard: left / right (D-pad, left stick, arrows) pick
// the previous / next class through the game's own SelectClass; A (Enter) goes on; B (Backspace)
// goes back; A on the spawn point screen spawns exactly as the PC Spawn button does (mode 2, then
// the game's own begin-spawn with this player's character - the same three calls). While a console
// screen is up the PC screen's own input function is skipped (its one call in the spawn screen's
// update now goes through BF1GameExt), so the mouse / pad no longer switch team or class behind
// the wheel. After a spawn the next spawn screen starts at the soldier select.
// 18e: CHARSELECT TEXT - the first piece of the console driver (game thread, through the chooser
// hook): while the F6 preview shows ifs_charselect1, its equipment window gets the selected
// class's name (the class's own display name, [class+30h] - the PC slot titles use it) and
// "EQUIPMENT:" + its weapons' names (the class's weapon list, the same calls the PC weapon lists
// use), set with the game's own text setter; PageUp / PageDown pick the previous / next class
// through the game's own SelectClass (so the soldier model follows). 18d's screenshots: the
// three console screens come up fine; the soldier and the map are the PC spawn screen's own
// parts and draw over any screen (moved / hidden in a later step).
// 18d: CONSOLE SCREEN PREVIEW - 18c found all three console screens built on PC (player 1's:
// ifs_readyselect1, ifs_charselect1, ifs_mapselect1 with their panels; no "2" copies; no 3D
// model elements in them). F6 while choosing on the spawn screen (1 player) cycles which screen
// the game's chooser shows: PC -> readyselect1 -> charselect1 -> mapselect1 -> PC, through the
// same name swap 13c used (spawn_ui.cpp). Nothing else changes; the scans stay.
// 18c: 18b found the registry (1620 elements) but no screen name in it: screens live in their own
// list (ScreenByName hashes the name and walks it), and an element is listed by its full path
// only. Now: the screen list is read (which screens exist), known PC paths are looked up as the
// check, the word search starts from each screen name without needing the name in the registry,
// and the game's memory is searched for every "ifs_..." text - each one, and each part of it up
// to a '.', is hashed and looked up, which names screens and elements the word list misses.
// 18b: CONSOLE SCREEN SCAN - the game's IF element registry (every interface element is listed
// there by the hash of its full path, e.g. "ifs_pc_SpawnSelect.Info.SideModel0") is read
// directly: are the console screens ifs_readyselect / ifs_charselect / ifs_mapselect built on PC,
// and which elements do they have? Names are found by trying "<path>.<word>" for a list of words
// (the hash can't be reversed); ifs_pc_SpawnSelect is named the same way as the check that it
// works. The 18a memory search is gone (answered: the PC's interface script loads the three
// console scripts by name); the frame timers +23A4h/+23A8h are no longer logged; the class lists
// are one line per class. Still log-only: no hooks, nothing written, no game code called.
// Phase 18a (2020 build: GOG 1.3 / Steam): CLASS WHEEL SCAN, log-only. Installs no hooks and
// writes nothing in the game - it only reads. [wheel] lines.
//
// Goal: the console spawn flow back for 1 player (the Classic Collection's three screens: side,
// soldier wheel, spawn point), driven through the game's own spawn code. The Classic Collection's
// Battlefront1.dll has that flow as C++: its SpawnDisplay switches between ifs_readyselect%i,
// ifs_charselect%i and ifs_mapselect%i (modes 4 / 3 / 2, one set per player). The PC exes (1.2 and
// 2020) only kept the PC screen (ifs_pc_SpawnSelect) and a push of ifs_readyselect1. This logs what
// the rebuild has to use:
//
//   1. The PC spawn screen object: class SpawnDisplay, 0x23F0 bytes, ONE object (made for player
//      index 0). Found through the code that makes it (GOG 1.3 rva 0x00077030):
//        push esi; push 23F0h; call new; add esp,4; test eax,eax; jz +0Dh; push 0; mov ecx,eax;
//        call SpawnDisplay::SpawnDisplay; mov esi,eax; jmp +2; xor esi,esi; mov [global],esi
//      The constructor's 'mov dword ptr [esi], vtable' gives the vtable: only an object with that
//      vtable is read. Fields (GOG 1.3 code): +18h mode (1 choosing, 2 Spawn pressed), +2370h
//      character, +238Ch player index, +23DCh team shown, +23ECh selected class index (written by
//      SetClassIndex, GOG 1.3 rva 0x00078C30, which is only 'mov [ecx+23ECh], arg'). The block
//      +2370h..+23F0h is read every 100 ms and every change is logged (its frame timers are not).
//   2. Each team's class list (SpawnManager +18h + k*4 = team k): +38h capacity, +3Ch count, +40h
//      extra classes (added after the AddUnitClass ones by a second path - the PC screen's class
//      cycle leaves them out), +44h class pointers, +54h each class's AddUnitClass number. Each
//      class: its C++ type, the texts it points to (looking for its ODF name), its first 60h bytes.
//   3. (18a only, removed in 18b) one read-only search of the game's memory for the console
//      screens' scripts - answer: the PC's interface script loads ifs_charselect, ifs_mapselect
//      and ifs_readyselect by name.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "class_wheel.hpp"
#include "menus.hpp"
#include "split_spawn_menus.hpp"
#include "players34.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

// the code that makes the one SpawnDisplay (GOG 1.3 rva 0x00077030)
const char* const kMakeDisplay = "56 68 F0 23 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0D 6A 00 8B C8 E8 ?? ?? ?? ?? "
                                 "8B F0 EB 02 33 F6 89 35 ?? ?? ?? ??";
constexpr uint32_t kMakeCtorCall = 22; // call SpawnDisplay::SpawnDisplay
constexpr uint32_t kMakeGlobal = 35;   // imm32 of 'mov [global], esi'
// SpawnDisplay::SpawnDisplay (GOG 1.3 rva 0x00076620): push ebp; mov ebp,esp; push esi;
// mov esi,ecx; call base; lea ecx,[esi+20h]; mov dword ptr [esi], vtable
const char* const kCtorBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 8D 4E 20 C7 06 ?? ?? ?? ??";
constexpr uint32_t kCtorVtable = 16;
// the accessors behind the field offsets read here (offset checks, nothing is called)
const char* const kSetClassIndex = "55 8B EC 8B 45 08 89 81 EC 23 00 00 5D C2 04 00";
const char* const kGetClassIndex = "8B 81 EC 23 00 00 C3";
const char* const kGetTeamShown = "8B 81 DC 23 00 00 C3";
// 18b: the IF element registry, reached from the PC spawn screen's setup function (GOG 1.3 rva
// 0x00077920), whose call at +15Dh is FindByPath(path) = FindByHash(hash of path):
const char* const kSetup = "55 8B EC 81 EC A0 00 00 00 56 57 8B F9 6A 00 89 7D EC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? "
                           "6A 01 83 EC 08 8B F0 8D 87 00 03 00 00";
constexpr uint32_t kSetupFindCall = 0x15D;
// FindByPath (GOG 1.3 rva 0x0029CEB0): hash the path (+0Ah), then call FindByHash (+17h)
const char* const kFindByPath =
    "55 8B EC 51 FF 75 08 8D 4D FC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 04 8B E5 5D C3";
constexpr uint32_t kFindHashCtorCall = 0x0A;
constexpr uint32_t kFindByHashCall = 0x17;
// the hash object's constructor (rva 0x002F9F10) calls the hash function at +09h; the function
// (rva 0x002FA0D0) is FNV-1a over (char | 20h), its loop at +10h:
const char* const kHashLoop = "BA C5 9D 1C 81 0F BE C0 8D 49 01 83 C8 20 33 C2 69 D0 93 01 00 01";
constexpr uint32_t kHashCtorCall = 0x09;
constexpr uint32_t kHashLoopAt = 0x10;
// FindByHash (rva 0x0029CE90): mov ecx, registry (+06h, imm32 at +07h); call Lookup (+0Bh)
const char* const kFindByHash = "55 8B EC FF 75 08 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 5D C3";
constexpr uint32_t kRegistryImm = 0x07;
constexpr uint32_t kLookupCall = 0x0B;
// Lookup (rva 0x0029CE60): probe(table = registry + 4, size 2000h, hash) - the probe (rva
// 0x002FA070) halves the size: 1000h key slots, then 1000h value slots; it starts at
// (hash & FFFh) and steps backwards (wrapping) until the key matches or is 0.
const char* const kLookup = "55 8B EC FF 75 08 8D 41 04 68 00 20 00 00 50 E8";
constexpr uint32_t kProbeCall = 0x0F;
const char* const kProbe = "55 8B EC 8B 55 0C D1 FA 56 8B 75 10 57 8B 7D 08 8D 42 FF 23 C6 8D 04 87";
constexpr uint32_t kRegSlots = 0x1000;
// IF element: +10h its path hash, +14h flags (100h = shown, 200h = in the registry)
constexpr uint32_t kIfFlags = 0x14;
// 18c: the screen list. ScreenByHash (GOG 1.3 rva 0x002B9E20) walks it: a circular list whose
// list object is its own end; [list] = first node, [node] = next node, [node+0Ch] = the screen's
// record, [record+4] = the hash of the screen's name.
const char* const kScreenByHash = "55 8B EC 83 EC 08 56 57 8D 45 F8 B9 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? B9 ?? ?? ?? ?? "
                                  "8B 00 89 45 FC 8D 45 F8 50 E8";
constexpr uint32_t kScreenListImm1 = 0x0C;
constexpr uint32_t kScreenListImm2 = 0x17;
constexpr uint32_t kScreenDerefCall = 0x2B; // node -> [node+0Ch]
constexpr uint32_t kScreenHashCall = 0x4B;  // record -> [record+4]
constexpr uint32_t kScreenNextCall = 0x57;  // node -> [node]
const char* const kDerefBody = "8B 01 8B 40 0C C3";
const char* const kRecordHashBody = "8B 41 04 C3";
const char* const kNextBody = "8B 01 8B 00 89 01 8B C1 C3";
constexpr int kMaxScreens = 256;
// 18e: the PC slot-title function (GOG 1.3 rva 0x00078EB0): name = [class+30h] ... then the text
// setter at +48h - a wrapper (push ebp; mov ebp,esp; pop ebp; jmp SetText) for SetText(wide)
// (rva 0x0029F0E0), which the PC screen uses on title bars and text boxes alike
const char* const kSlotTitle = "55 8B EC 83 EC 40 56 8B F1 8B 4D 08 57 8B 7D 0C 8B 41 30 85 C0 75 29";
constexpr uint32_t kSlotTitleSetText = 0x48;
const char* const kSetTextWrap = "55 8B EC 5D E9";
const char* const kSetTextBody = "55 8B EC 81 EC 3C 05 00 00";
// SelectClass(index) (GOG 1.3 rva 0x000788A0): SetClassIndex (+0Ah), then the class is shown
const char* const kSelectClass = "55 8B EC 56 57 FF 75 08 8B F9 E8 ?? ?? ?? ?? 8B CF E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? "
                                 "8B C8 E8 ?? ?? ?? ?? 8B CF 8B F0 E8 ?? ?? ?? ?? 50 8B CE E8 ?? ?? ?? ?? 50 8B CF "
                                 "E8 ?? ?? ?? ?? 6A 01 6A 00 6A 00 6A 00 8D 8F C0 23 00 00 E8";
constexpr uint32_t kSelectClassSetIndex = 0x0A;
constexpr uint32_t kClassName = 0x30;       // a class's (and a weapon class's) display name, wide text
constexpr uint32_t kWeaponCountSlot = 0x2C; // class vtable: number of weapons
constexpr uint32_t kWeaponGetSlot = 0x30;   // class vtable: weapon k
// 18f: the spawn screen's update calls its PC input function once (GOG 1.3 rva 0x00079B44:
// mov ecx,[edi+630h]; ...; call [eax+30h]; mov ecx,edi; call PcInput (+18h))
const char* const kInputCall = "8B 8F 30 06 00 00 F3 0F 10 45 08 51 F3 0F 11 04 24 8B 01 FF 50 30 8B CF E8";
constexpr uint32_t kInputCallAt = 24;
const char* const kPcInput = "55 8B EC 83 EC 1C 53 56 8B D9 57 89 5D EC E8"; // GOG 1.3 rva 0x00079DA0
// the PC Spawn button's sequence (3 copies in PcInput, all the same calls): push player;
// mov [ebx+18h],2; call CharIndex (+0Dh); ...; call GetSpawnManager (+16h); mov ecx,eax;
// call Character (+1Dh); push eax; mov ecx,ebx; call BeginSpawn (+25h)
const char* const kSpawnButton = "FF B3 8C 23 00 00 C7 43 18 02 00 00 00 E8 ?? ?? ?? ?? 83 C4 04 50 E8 ?? ?? ?? ?? 8B C8 "
                                 "E8 ?? ?? ?? ?? 50 8B CB E8";
const char* const kCharIndexWrap = "55 8B EC 5D E9";                 // rva 0x0019FC00 (cdecl, player -> index)
const char* const kGetSmBody = "A1 ?? ?? ?? ?? C3";                   // GetSpawnManager
const char* const kCharacterBody = "55 8B EC 56 8B 75 08 57 85 F6 78 21 3B 71 38 7D 1C 69 FE E8 00 00 00"; // 0x00219FB0
const char* const kBeginSpawnBody = "55 8B EC 81 EC 00 04 00 00 56 57 68 FF 00 00 00 6A 00 8B F9 E8"; // 0x00079250
// 18g: the PC next-spawn-point input's block in PcInput (GOG 1.3 rva 0x0007A066): push [ebx+23DCh];
// call GetSpawnManager (+06h); mov ecx,eax; call NextSpawnPoint (+0Dh); push eax; mov ecx,ebx;
// call SetSpawnPoint (+15h); then the repeat delay into [ebx+23ACh]
const char* const kNextPostBlock = "FF B3 DC 23 00 00 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 50 8B CB E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? "
                                   "D8 05 ?? ?? ?? ?? D9 9B AC 23 00 00";
const char* const kNextPostBody = "55 8B EC 81 EC 08 01 00 00 56 8D 45 FC 89 4D F8 50 33 F6 E8"; // 0x0021A060
const char* const kNextPostCursor = "8B 55 F8 33 C9 8B 82 A8 00 00 00 3B C6";                 // at +75h: reads +A8h
const char* const kNextPostCursorSet = "89 8A A8 00 00 00";                                   // at +90h: writes +A8h
constexpr uint32_t kNextPostCursorAt = 0x75;
constexpr uint32_t kNextPostCursorSetAt = 0x90;
const char* const kSetPostBody = "55 8B EC 56 8B F1 57 FF B6 DC 23 00 00 FF 75 08 E8";          // 0x00078910
// SetSpawnPoint's call at +31h stores the post: [esi+23E0h] = post (GOG 1.3 rva 0x00078C60)
const char* const kStorePostBody = "55 8B EC 56 8B F1 8B 4D 08 85 C9 75 1F";
constexpr uint32_t kSetPostStoreCall = 0x31;
// 21j-r19: the PC screen's click on its map (PcInput, GOG 1.3 rva 0x0007A357): after its team / class / Spawn buttons the
// cursor (the floats at [ebp-0Ch] / [ebp-8], display pixels as the input device keeps them) goes to PostAt; a post -> mov
// ecx,ebx; call SetSpawnPoint
const char* const kMapClickBlock = "F3 0F 10 45 F8 83 EC 08 8B CB F3 0F 11 44 24 04 F3 0F 10 45 F4 F3 0F 11 04 24 E8 ?? ?? ?? ?? "
                                   "85 C0 74 ?? 50 8B CB E8";
constexpr uint32_t kMapClickPostAt = 0x1A;  // call SpawnDisplay::PostAt(x, y)
constexpr uint32_t kMapClickSetPost = 0x26; // call SetSpawnPoint(post)
// SpawnDisplay::PostAt (GOG 1.3 rva 0x000771E0): the player's map (GetMap(+238Ch), +25h) is asked for the post whose icon is
// under (x, y) - the icons' hot-spots, set where each is drawn every frame - and only a post of the team shown is given back
const char* const kPostAtBody = "55 8B EC F3 0F 10 45 0C 53 57 83 EC 08 8B D9 F3 0F 11 44 24 04 F3 0F 10 45 08 F3 0F 11 04 24 "
                                "FF B3 8C 23 00 00 E8";
constexpr uint32_t kPostAtGetMap = 0x25;
constexpr uint32_t kSmPostCursor = 0xA8; // SpawnManager: NextSpawnPoint's cursor
constexpr uint32_t kSdPost = 0x23E0;     // spawn screen: the chosen post
constexpr uint32_t kPostName = 0x1C;     // post: its display name (wide text)
// 18h: the ring. All found from functions found above, at fixed offsets, each body checked:
const char* const kNewBody = "55 8B EC FF 75 08 FF 35 ?? ?? ?? ?? E8";                 // operator new (maker +06h)
const char* const kSoldierElemSite = "8D 8E 90 20 00 00 E8";                            // SpawnDisplay ctor +71h
const char* const kSoldierElemCtor = "56 57 8B F9 E8 ?? ?? ?? ?? 8D 8F C0 00 00 00 C7 07"; // GOG 1.3 rva 0x000C61D0
const char* const kShowClassBody = "55 8B EC 83 EC 0C 53 56 57 8B F9 8B 8F 30 06 00 00 85 C9 74 07"; // SelectClass +37h
const char* const kSetViewBody = "55 8B EC 8B 45 08 F3 0F 7E 00 66 0F D6 81 4C 01";      // ShowClass +67h
const char* const kSetSoldierClassBody = "55 8B EC 8B 45 08 3B 81 40 01 00 00 74 10 89 81 40 01 00 00"; // +6Fh
const char* const kSetWeaponBody = "55 8B EC 8B 45 08 89 81 A4 00 00 00 5D C2 04 00";     // ShowClass +C8h
const char* const kSetLayerBody = "55 8B EC 83 EC 0C 53 56 8B F1 57 89 75 F8 8D 46";      // ShowClass +E3h
const char* const kSetVisibleBody = "55 8B EC 8B 41 14 0F B6 55 08 25 FF FE FF FF C1 E2 08 0B D0 89 51 14 5D"; // +FDh
const char* const kSetScaleBody = "55 8B EC F3 0F 10 45 08 F3 0F 11 81 84 00 00 00 5D C2 04 00"; // setup +6EEh
const char* const kSetPositionBody = "55 8B EC 56 51 C7 04 24 00 00 80 3F 8D 71 30 6A 02 6A 03"; // setup +75Ah
const char* const kAddChildBody = "55 8B EC 83 EC 08 53 57 8B 7D 08 8B D9 8D 4F 1C E8";        // setup +7B4h
constexpr uint32_t kSdSoldierElem = 0x2090; // the spawn screen's own soldier element
constexpr uint32_t kSdGroup = 0x1A0;        // the draw group the spawn screen's soldier is in
constexpr uint32_t kElemSize = 0x170;       // sizeof(SoldierElement) (its deleting destructor frees 170h)
constexpr uint32_t kElemX = 0x60;           // position: the element's matrix (+30h) translation
constexpr uint32_t kElemY = 0x64;
constexpr uint32_t kElemScale = 0x84;       // SetScale
constexpr uint32_t kElemF80 = 0x80;         // vfunc 38h
constexpr uint32_t kElemView = 0x14C;       // SetView (3 floats)
constexpr int kRingMax = 8;               // classes on the ring at most
constexpr uint32_t kSdDroidekaElem = 0x640;  // the spawn screen's own droideka element
constexpr uint32_t kSdCurrentElem = 0x630;   // the element ShowClass shows (+2090h or +640h)
constexpr uint32_t kDroidekaSize = 0x1A50;   // sizeof(DroidekaElement) (its deleting destructor frees 1A50h)
const char* const kDroidekaElemSite = "8D 8E 40 06 00 00 E8";                    // SpawnDisplay ctor +66h
const char* const kDroidekaElemCtor = "56 8B F1 E8 ?? ?? ?? ?? 8D 8E B0 00 00 00 C7 06"; // GOG 1.3 rva 0x000C53E0
const char* const kSetDroidekaSite = "8D B7 40 06 00 00 53 8B CE E8";             // ShowClass +89h
const char* const kSetDroidekaClassBody = "55 8B EC 8B 45 08 89 81 B8 04 00 00 5D C2 04 00"; // GOG 0x00078BE0
// 18i-r2: ShowClass's weapon step (+A7h): type id getter, class vfunc 0Ch (is-a), vfunc 28h, SetWeapon
const char* const kShowWeaponSeq =
    "8B 33 E8 ?? ?? ?? ?? 50 8B 46 0C 8B CB FF D0 84 C0 74 13 8B 03 8B CB FF 50 28 8B 8F 30 06 00 00 50 E8";
const char* const kTypeFnBody = "B9 ?? ?? ?? ?? E9"; // the type id getter: mov ecx, type / jmp
// the chooser's mode-1 "show the current soldier" call (GOG 1.3 rva 0x00076E7B, chooser +13Bh)
const char* const kChooserShowSite = "5F 5E 8B E5 5D C2 04 00 8B 8E 30 06 00 00 6A 01 E8";
constexpr uint32_t kChooserShowCall = 16; // the E8 inside kChooserShowSite
// 18l: the side screen
const char* const kTextCtorSite = "8D 8E E0 00 00 00 E8";   // SpawnDisplay ctor +19h (its +E0h text)
const char* const kTextCtorBody =
    "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? 33 C0 C7 06 ?? ?? ?? ?? 89 86 A0 00 00 00 89 86 AC 00 00 00"; // 0x0029E910
const char* const kSetFontSite = "68 ?? ?? ?? ?? 8D 4F 20 E8";   // setup +5DEh (push "gamefont_medium")
const char* const kSetFontBody = "55 8B EC 8B 45 08 56 8B F1 85 C0 74 1B 80 38 00 74 16 50 E8"; // 0x0029E7B0
const char* const kSetJustifyBody =
    "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 8B 55 0C 8B 45 08 83 E2 03 83 E0 03 C1 E2 02 0B D0 8B 86 BC 00 00 00"; // 0x0029EE90
const char* const kSelectTeamBody = "55 8B EC 81 EC 00 04 00 00 56 8B 75 08 57 56 8B F9 E8"; // 0x00078980
const char* const kSetTeamShownBody = "55 8B EC 8B 45 08 89 81 DC 23 00 00 5D C2 04 00";        // 0x00079040
constexpr uint32_t kTextSize = 0xC0;   // sizeof(RedTextElement) (its deleting destructor frees C0h)
constexpr uint32_t kTextScaleX = 0xA4; // RedTextElement text scale (1.0 by default)
constexpr uint32_t kTextScaleY = 0xA8;
constexpr uint32_t kElemColor = 0x70;  // interface element colour: B, G, R, A bytes
constexpr uint32_t kTeamName = 0x0C;   // Team: its name (wide text pointer)
// 18o
const char* const kRefSizeSeq = "F3 0F 10 05 ?? ?? ?? ?? F3 0F 59 05 ?? ?? ?? ?? 68 ?? ?? ?? ?? F3 0F 11 05"; // 2 hits
constexpr uint32_t kRefSizeAt = 0x19;          // the reference width / height global (movss [g], xmm0)
const char* const kPopupFillHead = "55 8B EC 81 EC 94 03 00 00 56 57 8B 7D 08 8B F1"; // GOG 1.3 rva 0x0007A7B0
constexpr uint32_t kSetPostFillCall = 0x39;     // SetSpawnPoint +39h: call PopupFill(post)
const char* const kVehTextSite = "8B 5D F4 50 8B 8B 1C 06 00 00 E8";   // PopupFill +157h: SetText(PopupWindow.Text)
constexpr uint32_t kVehTextSiteAt = 0x157;
const char* const kObjTextSite = "8B 8F 28 06 00 00 8D 85 00 FC FF FF 50 E8"; // SelectTeam +92h: SetText(ObjectiveWindow.Text)
constexpr uint32_t kObjTextSiteAt = 0x92;
const char* const kSetFontObjBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? 8B 8E 80 00 00 00 85 C9 74 3D"; // 0x0029E800
constexpr uint32_t kSetFontObjCall = 0x1E;      // SetFont(name) +1Eh: call SetFontObj(font)
constexpr uint32_t kTextFont = 0x80;            // RedTextElement: its font object
constexpr uint32_t kTextFlags = 0xBC;           // RedTextElement: justify in bits 24..27
// 18p: the sounds - from the Lua ScriptCB_SndPlaySound (GOG 1.3 rva 0x001807B0), found through the
// Lua function table (the entry holds the name's address, then the function's)
const char* const kSndPlayHead = "55 8B EC 83 EC 10 8D 4D F0 56 8B 75 08 57 6A 01 E8";
constexpr uint32_t kSndSetCall = 0xCB;   // call SetByName(handle, name, 1)   (GOG 0x00143880)
constexpr uint32_t kSndPlayCall = 0xDC;  // call Play(handle, 0, 0, 1, 1)     (GOG 0x00143440)
const char* const kSndSetBody = "55 8B EC 8B 55 08 81 EC 00 02 00 00 56 8D B5 00 FE FF FF 57 8B F9 C7 07 00 00 00 00 C6 47 04 00";
const char* const kSndPlayBody = "55 8B EC 83 EC 2C 57 8B F9 E8 ?? ?? ?? ?? 84 C0 0F 84";
// 18q: each sound has a list of names; the first the loaded sound banks know is the one played
const char* const kSndSelectNames[] = { "shell_select", "shell_menu_select", "shell_accept", "shell_ok", "shell_menu_enter",
                                        "shell_enter" };
const char* const kSndBackNames[] = { "shell_abort", "shell_back", "shell_cancel", "shell_menu_back", "shell_menu_exit",
                                      "shell_exit", "shell_nav_back", "shell_menu_cancel", "shell_escape", "shell_menu_down" };
// 19i: the names 19h-r2's sound names line had (the banks' "shell_..." texts): shell_select_change first
const char* const kSndTickNames[] = { "shell_select_change", "shell_select_unit", "shell_menu_up", "shell_menu_down", "shell_nav_up",
                                      "shell_menu_left", "shell_scroll", "shell_select" };
// 18q: the mouse - the game's own cursor, read where its interface hot-spot test reads it: the Lua
// ScriptCB_IFObj_TestHotSpot (GOG 1.3 rva 0x00171C30, found through the Lua function table) calls
// the input manager getter (+36h: a "mov eax, imm32; ret"), its vfunc 20h(0) for player 1's input
// object, then +138h (x) and +13Ch (y) through two one-line getters (+48h, +5Bh), and the hit test
// (+7Ch) -> box test (+22h) -> point test (+26h) -> display getter (+0Ah: "mov eax, imm32; ret";
// the display's width is at +0, its height at +4) - x and y are in those pixels.
const char* const kHotSpotHead = "55 8B EC 83 EC 08 57 8B 7D 08 57 E8 ?? ?? ?? ?? 83 C4 04 83 F8 01 74 07 33 C0 5F 8B E5 5D C3 "
                                 "53 6A 01 57 C6 45 FC 00 E8 ?? ?? ?? ?? 8B D8 83 C4 08 85 DB 74 50 56 E8";
constexpr uint32_t kHotSpotMgrCall = 0x36;
const char* const kHotSpotMid = "6A 00 8B C8 8B 10 FF 52 20 8B F0 8B CE E8"; // +3Bh: vfunc 20h(0), then x getter
constexpr uint32_t kHotSpotMidAt = 0x3B;
constexpr uint32_t kHotSpotXCall = 0x48;
const char* const kHotSpotMid2 = "8B CE 66 0F 6E C0 0F 5B C0 F3 0F 11 45 F8 E8"; // +4Dh: then the y getter
constexpr uint32_t kHotSpotMid2At = 0x4D;
constexpr uint32_t kHotSpotYCall = 0x5B;
const char* const kHotSpotTail = "83 EC 08 8B CB 66 0F 6E C0 0F 5B C0 F3 0F 11 44 24 04 F3 0F 10 45 F8 F3 0F 11 04 24 E8"; // +60h
constexpr uint32_t kHotSpotTailAt = 0x60;
constexpr uint32_t kHotSpotTestCall = 0x7C;
const char* const kGetterImm = "B8 ?? ?? ?? ?? C3";              // mov eax, imm32; ret
const char* const kMouseXBody = "8B 81 38 01 00 00 C3";          // mov eax, [ecx+138h]; ret
const char* const kMouseYBody = "8B 81 3C 01 00 00 C3";
const char* const kHitTestHead = "55 8B EC 8B 49 18 85 C9 74 27 F3 0F 10 45 0C 83 EC 08 F3 0F 11 44 24 04 F3 0F 10 45 08 F3 0F 11 "
                                 "04 24 E8";                      // 0x00079770
constexpr uint32_t kHitBoxCall = 0x22;
const char* const kBoxTestHead = "55 8B EC F3 0F 10 45 0C 8D 45 FC 83 EC 08 50 8D 45 F8 50 83 EC 08 F3 0F 11 44 24 04 F3 0F 10 45 "
                                 "08 F3 0F 11 04 24 E8";          // 0x0029C730
constexpr uint32_t kBoxPointCall = 0x26;
const char* const kPointTestHead = "55 8B EC 83 EC 18 56 57 8B F9 E8"; // 0x0029C3F0
constexpr uint32_t kPointDisplayCall = 0x0A;
constexpr uint32_t kMouseX = 0x138;
constexpr uint32_t kMouseY = 0x13C;
static_assert(kMouseY == kMouseX + 4, "21j-r20: the cursor's x and y are read together");
// 18n
constexpr int kZFront = 0x90;         // the ring's front soldier's layer (drawn last of the soldiers)
constexpr int kZText = 0x10;          // our texts: drawn after everything else in their group
constexpr uint8_t kDarkGrey = 0x58;   // the unpicked side's colour
constexpr uint8_t kOutGrey = 0x2C;    // v6.9: a side out of the match (Mashup Conquest)
// 18m
constexpr uint32_t kSdTextGroup = 0x300; // the group the spawn screen keeps its own texts in
const char* const kMapSetModeBody =
    "55 8B EC 8B 45 08 53 8B D9 56 57 39 83 8C F2 01 00 74 06 FF 05 ?? ?? ?? ?? 89 83 8C F2 01 00 E8"; // 0x00188460
const char* const kMapTableSite = "F3 0F 10 04 BD";  // SetMode +79h: movss xmm0, [edi*4 + layout table]
constexpr uint32_t kMapTableAt = 0x7E;
const char* const kGetMapSite = "6A 00 E8";           // SelectTeam +81h: push 0; call GetMap
const char* const kGetMapBody = "55 8B EC 8B 45 08 8B 04 85 ?? ?? ?? ?? 5D C3"; // 0x000784E0
constexpr uint32_t kMapMode = 0x1F28C;  // MapDisplay: its mode (1 = the spawn screen's big map)
constexpr uint32_t kMapFlag = 0x1F289;  // MapDisplay: the other big-map flag (layout 2)
constexpr uint32_t kMapLayout1 = 28;    // layout 1 (1 player, mode 1): 7 floats each
// 19g: with more than one player local the map picks layout 5 with its flag set, else 4 in mode 1
// (GOG 1.3 0x001873C0); player 1's map has the flag on his spawn screens, player 2's is put in mode 1
constexpr int kSplitLayoutP1 = 5;
constexpr int kSplitLayoutP2 = 4;
constexpr uint32_t kLayoutFloats = 7;   // size x, size y, x offset, y offset, anchor x, anchor y, ?
// 19g: the spawn screen's blue grid: SpawnDisplay +2210h, a Red3DModelElement with the "shell_cube"
// model, made in the constructor (+7Ch: lea ecx,[esi+2210h]; call ctor) and set up in the setup
// (push [hash]; lea esi,[edi+2210h]; mov ecx,esi; call SetModel; ... SetScale (+A0h) ...)
constexpr uint32_t kSdCube = 0x2210;
constexpr uint32_t kCubeCtorSiteAt = 0x7C;                  // SpawnDisplay::SpawnDisplay +7Ch
const char* const kCubeCtorSite = "8D 8E 10 22 00 00 E8";
const char* const kCubeCtorBody = "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 6A FF 68 ?? ?? ?? ?? "
                                  "64 A1 00 00 00 00 50 53 83 EC 68 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 64 A3 00 00 00 00 "
                                  "8B F9 89 7D EC E8"; // GOG 1.3 rva 0x002A03A0
const char* const kCubeSetupSite = "FF 35 ?? ?? ?? ?? 8D B7 10 22 00 00 8B CE E8 ?? ?? ?? ?? 51 8B CE C7 04 24 ?? ?? ?? ?? E8 "
                                   "?? ?? ?? ?? E8 ?? ?? ?? ?? 51 8B CE"; // setup +86Ah
constexpr uint32_t kCubeSetModelCall = 0x0E;
const char* const kCubeSetModelBody =
    "55 8B EC 8B 45 08 56 8B F1 50 89 86 80 00 00 00 E8 ?? ?? ?? ?? 83 C4 04 89 86 50 01 00 00 5E 5D C2 04 00"; // 0x002A0C50
constexpr uint32_t kCubeScaleCall = 0x1D;
const char* const kCubeScaleBody = "55 8B EC F3 0F 10 45 08 F3 0F 11 81 A0 00 00 00 5D C2 04 00"; // +A0h (0x00078CE0)
// its deleting destructor frees 160h (the size made here)
const char* const kCubeDeleteBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? F6 45 08 01 74 0E 68 60 01 00 00 56 E8"; // 0x002A05C0
constexpr uint32_t kCubeSize = 0x160;
constexpr uint32_t kCubeHash = 0x80;    // SetModel: the model's name hash
constexpr uint32_t kCubeScale = 0xA0;   // 0.009 (the spawn screen's setup)
constexpr uint32_t kCubeDepth = 0x9C;   // display width / 800 * 1.25 (the setup)
constexpr int kCubeLayer = 255;         // the setup's (drawn first)
constexpr uint32_t kElemMatrix = 0x30;      // 4x4 floats; rows 0..2 + depth copied from the game's soldier
constexpr uint32_t kElemRotOfs = 0x88;      // rotation (4 floats) + offset (3 floats), 1Ch bytes
// the spawn screen's deleting destructor (its vtable slot 0), GOG 1.3 rva 0x00076A80
const char* const kSdDeleteBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? F6 45 08 01 74 0E 68 F0 23 00 00 56 E8";
// 20f-r2 / r4: an empty group for players 3 and 4's spawn screens (ClassWheelGroupNew). r4: a RedScreenGroupElement -
// the group class a HUD root's own top-level children are (the spawn screen's groups, GOG 1.3 0x0007664A ...): a
// RedGroupElement (0xA0 bytes) with the root anchor +A0h / +A4h and flag +A8h after it, 0xB0 bytes. r2 / r3 made a plain
// RedGroupElement, and the game's AddToRoot (which sets +A0h, +A4h, +A8h) wrote those 12 bytes past it - over the next
// heap block's header: in r3 the runtime heap's one free block (75 MB) became size 0 and the game's next allocation
// failed (the crash at GOG 1.3 0x00287BFF, r2 and r3). Its default constructor (GOG 1.3 rva 0x00291F50: RedGroupElement's
// constructor, its vtable, the three root fields cleared), made with the game's new; the vtable's RTTI name and its
// deleting destructor (slot 0, GOG 1.3 0x00291FF0: frees B0h, the size made here) checked at start
const char* const kGroupCtorBody = "55 8B EC 51 56 8B F1 89 75 FC E8 ?? ?? ?? ?? C7 06 ?? ?? ?? ?? 8B C6 C7 86 A4 00 00 00 00 00 00 00 "
                                   "C7 86 A0 00 00 00 00 00 00 00 C6 86 A8 00 00 00 00 5E 8B E5 5D C3";
constexpr uint32_t kGroupCtorVtable = 0x11; // imm32 of "mov dword ptr [esi], vtable"
const char* const kGroupDeleteBody = "55 8B EC 56 8B F1 E8 ?? ?? ?? ?? F6 45 08 01 74 0E 68 B0 00 00 00 56 E8";
constexpr uint32_t kGroupSize = 0xB0;
const char* const kGroupClass = "RedScreenGroupElement";

constexpr uint32_t kDisplaySize = 0x23F0;
constexpr uint32_t kMode = 0x18;
constexpr uint32_t kTailStart = 0x2370;
constexpr int kTailDwords = static_cast<int>((kDisplaySize - kTailStart) / 4); // 32
constexpr int kIdxCharacter = 0;   // +2370h
constexpr int kIdxPlayer = 7;      // +238Ch
constexpr int kIdxTeamShown = 27;  // +23DCh
constexpr int kIdxClassIndex = 31; // +23ECh
// character fields (spawn_manager.hpp)
constexpr uint32_t kCharTeam = 0xA0;
constexpr uint32_t kCharClass = 0xAC;
constexpr uint32_t kCharUnit = 0xB4;
// SpawnManager and team fields
constexpr uint32_t kSmTeams = 0x18;
constexpr uint32_t kSmUnitCounts = 0x44;
constexpr uint32_t kTeamCapacity = 0x38; // +38h .. +57h read in one go
constexpr uint32_t kTeamCount = 0x3C;
constexpr uint32_t kTeamClasses = 0x44;
constexpr int kMaxTeams = 8;
constexpr uint32_t kMaxClasses = 64;

constexpr DWORD kPollMs = 100;
constexpr DWORD kIfScanDelayMs = 2000;
constexpr int kMaxIfLines = 1500;
constexpr int kMaxLines = 700;
constexpr int kMuteAfter = 25;

uintptr_t g_global = 0; // address of the SpawnDisplay pointer
uintptr_t g_vtable = 0; // SpawnDisplay's vtable (address in memory)
int g_lines = 0;
uintptr_t g_registry = 0; // 18b: the IF element registry (0 = not found, no IF scan)
uintptr_t g_screenList = 0; // 18c: the screen list object (0 = not found)
// 18e: the game functions the driver calls (0 = not found: the driver stays off)
typedef void(__fastcall* SetTextW_t)(void* self, void* unusedEdx, const wchar_t* text);
typedef void(__fastcall* SelectClass_t)(void* self, void* unusedEdx, int index);
typedef int(__fastcall* WeaponCount_t)(void* self, void* unusedEdx);
typedef uintptr_t(__fastcall* WeaponGet_t)(void* self, void* unusedEdx, int k);
SetTextW_t g_setTextW = nullptr;
SelectClass_t g_selectClass = nullptr;
// 18f
typedef void(__fastcall* PcInput_t)(void* self, void* unusedEdx);
typedef int(__cdecl* CharIndex_t)(int player);
typedef uintptr_t(__fastcall* Character_t)(void* self, void* unusedEdx, int index);
typedef void(__fastcall* BeginSpawn_t)(void* self, void* unusedEdx, uintptr_t character);
PcInput_t g_pcInput = nullptr;
CharIndex_t g_charIndex = nullptr;
Character_t g_character = nullptr;
BeginSpawn_t g_beginSpawn = nullptr;
// 18g
typedef uintptr_t(__fastcall* NextPost_t)(void* sm, void* unusedEdx, int team);
typedef void(__fastcall* SetPost_t)(void* sd, void* unusedEdx, uintptr_t post);
NextPost_t g_nextPost = nullptr;
SetPost_t g_setPost = nullptr;
// 21j-r19: the post icon under the cursor on the spawn screen's map (what the PC screen's click on its map asks)
typedef uintptr_t(__fastcall* PostAt_t)(void* sd, void* unusedEdx, float x, float y);
PostAt_t g_postAt = nullptr;
uintptr_t g_postAtMaps = 0; // the maps of each player PostAt asks (its GetMap's array; a player with none is not asked)
// 18h
typedef void*(__cdecl* GameNew_t)(size_t size);
typedef void*(__fastcall* ElemCtor_t)(void* self, void* unusedEdx);
typedef void(__fastcall* ElemSetPtr_t)(void* self, void* unusedEdx, uintptr_t value);
typedef void(__fastcall* ElemSetInt_t)(void* self, void* unusedEdx, int value);
typedef void(__fastcall* ElemSetFloat_t)(void* self, void* unusedEdx, float value);
typedef void(__fastcall* ElemSetPos_t)(void* self, void* unusedEdx, float x, float y);
typedef bool(__fastcall* ElemAdd_t)(void* group, void* unusedEdx, void* elem);
typedef bool(__fastcall* ElemNoArg_t)(void* self, void* unusedEdx);
typedef void*(__fastcall* ElemDelete_t)(void* self, void* unusedEdx, int flags);
typedef uintptr_t(__fastcall* ClassWeapon_t)(void* cls, void* unusedEdx);
struct RingFunctions
{
    GameNew_t gameNew = nullptr;
    ElemCtor_t ctor = nullptr;
    ElemSetPtr_t setView = nullptr;
    ElemSetPtr_t setClass = nullptr;
    ElemSetPtr_t setWeapon = nullptr;
    ElemSetInt_t setLayer = nullptr;
    ElemSetInt_t setVisible = nullptr;
    ElemSetFloat_t setScale = nullptr;
    ElemSetPos_t setPos = nullptr;
    ElemAdd_t addChild = nullptr;
};
RingFunctions g_rf;
bool g_ringOk = false;
typedef uint32_t(__cdecl* TypeId_t)();
typedef bool(__fastcall* IsA_t)(void* cls, void* unusedEdx, uint32_t type);
TypeId_t g_weaponType = nullptr;      // ShowClass's weapon type id getter
bool g_showRedirected = false;        // the chooser's show-soldier call goes through BF1GameExt
ElemCtor_t g_rdCtor = nullptr;        // DroidekaElement::DroidekaElement
ElemSetPtr_t g_rdSetClass = nullptr;  // SetDroidekaClass (+4B8h)
bool g_rdOk = false;
typedef void(__fastcall* TextFont_t)(void* self, void* unusedEdx, const char* font);
typedef void(__fastcall* TextJustify_t)(void* self, void* unusedEdx, int h, int v);
typedef void(__fastcall* SelectTeam_t)(void* sd, void* unusedEdx, int team);
ElemCtor_t g_txCtor = nullptr;      // RedTextElement::RedTextElement
TextFont_t g_txFont = nullptr;      // RedTextElement::SetFont
TextJustify_t g_txJustify = nullptr; // RedTextElement::SetJustify (h: 0 left 1 right 2 centre; v: 2 centre)
SelectTeam_t g_selectTeam = nullptr; // SpawnDisplay::SelectTeam
bool g_sideOk = false;
typedef void(__fastcall* SetFontObj_t)(void* self, void* unusedEdx, uintptr_t font);
SetFontObj_t g_txFontObj = nullptr;  // RedTextElement::SetFont(font object)
uint32_t g_selectTeamRva = 0;
uint32_t g_setPostRva = 0;
uintptr_t g_refWAddr = 0;            // the interface's reference width (float global; 0 = not found)
uintptr_t g_refHAddr = 0;
wchar_t g_vehText[256] = {};         // the vehicles at the chosen post, as the game writes them
wchar_t g_objText[512] = {};         // the team's objective, as the game writes it
LONG g_vehSeq = 0;
LONG g_objSeq = 0;
bool g_popupHooked = false;
uint32_t g_popupFillRva = 0;         // 19h: PopupFill (byte-checked) - player 2's VEHICLES (ClassWheelVehiclesAt)
volatile LONG g_vehCapture = 0;      // 19h: 1 = VehTextHook keeps the string in g_vehCaptured (no SetText), 2 = drops it
wchar_t g_vehCaptured[256] = {};
int g_vehLines = 0;
typedef void(__fastcall* SndSet_t)(void* handle, void* unusedEdx, const char* name, int flag);
typedef void(__fastcall* SndPlay_t)(void* handle, void* unusedEdx, int a, int b, int c, int d);
SndSet_t g_sndSet = nullptr;
SndPlay_t g_sndPlay = nullptr;
bool g_sndOk = false;
bool g_wheelEnabled = true;   // [ClassWheel] Enabled (21j-r18: single player only - the GAME OPTIONS SPAWN SCREEN row)
bool g_wheelStarted = false;  // 21j-r18: the scan found the spawn screen on this build (the SPAWN SCREEN row shows)
char g_wheelIni[MAX_PATH + 32] = ""; // 21j-r18: BF1GameExt.ini (the row saves Enabled there)
bool g_wheelSplit = true;     // 18w: [ClassWheel] Split - the flow in split screen too
bool g_pauseLatch = true;     // 21h-r6: [ClassWheel] PauseLatch - buttons held when his flow comes back from a pause latched
bool g_sideCarousel = true;   // v7.4: [ClassWheel] SideCarousel - 1-player CHOOSE YOUR SIDE with more than two sides as a
                              // carousel (0: v7.0's rows, the sides out of the match greyed); split screen always has it
bool g_mouseClicks = true;    // 21j-r19: [ClassWheel] MouseClicks - the mouse as the Classic Collection's (click what you mean)
int g_clickLines = 0;         // 21j-r19: [wheel] mouse click lines
constexpr DWORD kFlowGapMs = 150; // 21h-r6: a gap this long between his flow's frames = it was stopped (a pause)
int g_gapLines = 0;
bool g_shellScanned = false;
// 18q: the sounds, each resolved to the first name the sound banks know
struct UiSound
{
    const char* what;
    const char* const* names;
    int count;
    int resolved;   // index into names; -1 = not looked up yet; -2 = none found (looked up)
    int tries;      // look-ups done (logged, at most a few)
};
UiSound g_sndSelect = { "select", kSndSelectNames, static_cast<int>(sizeof(kSndSelectNames) / sizeof(kSndSelectNames[0])), -1, 0 };
UiSound g_sndBack = { "back", kSndBackNames, static_cast<int>(sizeof(kSndBackNames) / sizeof(kSndBackNames[0])), -1, 0 };
UiSound g_sndTick = { "tick", kSndTickNames, static_cast<int>(sizeof(kSndTickNames) / sizeof(kSndTickNames[0])), -1, 0 };
// 18q: the mouse
typedef void*(__fastcall* InputDevice_t)(void* mgr, void* unusedEdx, int player);
uintptr_t g_inputMgr = 0;        // the input manager object (the address the getter returns)
uintptr_t g_displaySize = 0;     // the display's width (int) at +0, height at +4
bool g_mouseOk = false;          // the game's cursor can be read
int g_mouseLines = 0;
float g_ringDepth = 2.0f;     // the back of the ring, in ring half-widths behind the front (z)
bool g_keyDepthLess = false;
bool g_keyDepthMore = false;
typedef void(__fastcall* MapSetMode_t)(void* map, void* unusedEdx, int mode);
MapSetMode_t g_mapSetMode = nullptr; // MapDisplay::SetMode
uintptr_t g_mapTable = 0;            // the map's layout table (7 floats per layout)
uintptr_t g_mapArray = 0;            // the map of each player (player 0 first)
bool g_mapOk = false;
float g_dekaSize = 1.4f;             // droidekas on the ring, of their own size
uint32_t g_spawnCtorRva = 0;   // SpawnDisplay::SpawnDisplay
uint32_t g_setupRva = 0;       // the spawn screen's setup function
// 19g: the blue grid's element (Red3DModelElement): its constructor and SetModel (for player 2's copy)
typedef void(__fastcall* SetModel_t)(void* self, void* unusedEdx, uint32_t hash);
ElemCtor_t g_cubeCtor = nullptr;
SetModel_t g_cubeSetModel = nullptr;
bool g_cubeOk = false;
ElemCtor_t g_groupCtor = nullptr; // 20f-r2 / r4: RedScreenGroupElement's default constructor (ClassWheelGroupNew)
int g_cubeLines = 0;
uint32_t g_selectRva = 0;      // SelectClass
uint32_t g_makeRva = 0;        // the code that makes the spawn screen
typedef void*(__fastcall* SdDelete_t)(void* self, void* unusedEdx, int flags);
SdDelete_t g_origSdDelete = nullptr;
bool g_sdDeleteHooked = false;
struct XState
{
    DWORD packet;
    WORD buttons;
    BYTE lt;
    BYTE rt;
    SHORT lx;
    SHORT ly;
    SHORT rx;
    SHORT ry;
};
typedef DWORD(WINAPI* XGetState_t)(DWORD, XState*);
XGetState_t g_xGetState = nullptr;
// 18d: the preview - index into kPreview, changed by F6 on this thread, read on the game thread
const char* const kPreview[] = { nullptr, "ifs_readyselect1", "ifs_charselect1", "ifs_mapselect1" };
constexpr LONG kPreviewCount = 4;
volatile LONG g_previewIdx = 0;
bool g_f6Down = false;
int g_ifLines = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    if (++g_lines == kMaxLines)
        Log("[wheel] line limit reached, class wheel scan going quiet");
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

void ConfirmField(const char* what, const char* pattern)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(pattern, hits, 4);
    if (n >= 1)
        Log("[wheel] field check %s: %d hit(s), first at rva 0x%08X  OK", what, n, hits[0]);
    else
        Log("[wheel] field check %s: NOT FOUND - that offset may differ on this build", what);
}

// ---- 2. the class lists -----------------------------------------------------
void LogClass(int team, uint32_t i, uintptr_t cls, uint32_t numbers, bool inCycle)
{
    char type[64];
    if (!TypeName(cls, type, sizeof(type)))
        snprintf(type, sizeof(type), "type ?");
    const long number = numbers ? static_cast<long>(static_cast<int32_t>(ReadU32(numbers + 4 * i))) : -1;
    // +20h: a 16-character label inside the class (no 0 when all 16 are used), e.g. "Arc Trooper"
    char label[17] = {};
    if (ReadMem(cls + 0x20, label, 16))
    {
        for (int k = 0; k < 16 && label[k]; ++k)
            if (label[k] < 32 || label[k] > 126)
                label[k] = '?';
    }
    if (!Budget())
        return;
    Log("[wheel]   team %d class %u: 0x%08X %s, AddUnitClass number %ld, %s, label \"%s\"", team,
        static_cast<unsigned>(i), static_cast<unsigned>(cls), type, number,
        inCycle ? "in the PC screen's cycle" : "EXTRA (left out of the PC screen's cycle)", label);
}

void LogTeams(uintptr_t sm)
{
    if (Budget())
        Log("[wheel] %lu ms: the teams' class lists (SpawnManager 0x%08X):", ScanMsSinceAttach(),
            static_cast<unsigned>(sm));
    for (int k = 0; k < kMaxTeams; ++k)
    {
        const uintptr_t team = ReadU32(sm + kSmTeams + 4 * static_cast<uint32_t>(k));
        if (!team)
            continue;
        uint32_t hdr[8] = {}; // +38h capacity, +3Ch count, +40h extra, +44h classes, +48h, +4Ch, +50h, +54h numbers
        if (!ReadMem(team + kTeamCapacity, hdr, sizeof(hdr)))
        {
            if (Budget())
                Log("[wheel] team %d: 0x%08X unreadable", k, static_cast<unsigned>(team));
            continue;
        }
        char type[64];
        if (!TypeName(team, type, sizeof(type)))
            snprintf(type, sizeof(type), "type ?");
        const uint32_t count = hdr[1];
        const uint32_t extra = hdr[2];
        if (Budget())
            Log("[wheel] team %d: 0x%08X %s, %u classes (capacity %u), %u extra, classes at 0x%08X, numbers at 0x%08X, "
                "+48h %08X +4Ch %08X +50h %08X, SetUnitCount %ld",
                k, static_cast<unsigned>(team), type, static_cast<unsigned>(count), static_cast<unsigned>(hdr[0]),
                static_cast<unsigned>(extra), static_cast<unsigned>(hdr[3]), static_cast<unsigned>(hdr[7]),
                static_cast<unsigned>(hdr[4]), static_cast<unsigned>(hdr[5]), static_cast<unsigned>(hdr[6]),
                static_cast<long>(static_cast<int32_t>(ReadU32(sm + kSmUnitCounts + 4 * static_cast<uint32_t>(k)))));
        if (!hdr[3] || count > kMaxClasses || extra > count)
            continue;
        for (uint32_t i = 0; i < count; ++i)
            LogClass(k, i, ReadU32(hdr[3] + 4 * i), hdr[7], i + extra < count);
    }
}

// ---- 1. the spawn screen object -------------------------------------------
struct Watch
{
    uintptr_t display = 0;
    bool valid = false;
    bool have = false;
    uint32_t mode = 0;
    uint32_t tail[kTailDwords] = {};
    int changes[kTailDwords] = {};
    int modeChanges = 0;
};

// frame timers: +2380h, +2384h, +23A4h / +23A8h (18a: they tick while the screen is up),
// +23ACh (next refresh) and the five timer objects +23B0h..+23D7h
bool Noisy(int i)
{
    return i == 4 || i == 5 || (i >= 13 && i <= 25);
}

const char* FieldName(int i, char* buf, size_t n)
{
    switch (i)
    {
    case kIdxCharacter:
        return "+2370h character";
    case kIdxPlayer:
        return "+238Ch player";
    case kIdxTeamShown:
        return "+23DCh team shown";
    case kIdxClassIndex:
        return "+23ECh class index";
    case 9:
        return "+2394h (set at Spawn)";
    case 11:
        return "+239Ch (a class of the team shown)";
    case 26:
        return "+23D8h in the world";
    case 28:
        return "+23E0h spawn point object";
    case 29:
        return "+23E4h spawn point object 2";
    case 30:
        return "+23E8h spawn point index";
    default:
        snprintf(buf, n, "+%04Xh", static_cast<unsigned>(kTailStart + 4 * static_cast<uint32_t>(i)));
        return buf;
    }
}

const char* ModeName(uint32_t mode)
{
    return mode == 1 ? "choosing (PC screen)" : mode == 2 ? "Spawn pressed (ifs_readyselect1)" : "other";
}

void LogScreen(const Watch& w)
{
    const uint32_t ch = w.tail[kIdxCharacter];
    const uint32_t teamNo = w.tail[kIdxTeamShown];
    const uint32_t idx = w.tail[kIdxClassIndex];
    uint32_t cls = 0;
    uint32_t count = 0;
    const uintptr_t sm = SpawnScanManager();
    if (sm && teamNo < static_cast<uint32_t>(kMaxTeams))
    {
        const uintptr_t team = ReadU32(sm + kSmTeams + 4 * teamNo);
        if (team)
        {
            count = ReadU32(team + kTeamCount);
            const uint32_t arr = ReadU32(team + kTeamClasses);
            if (arr && idx < count && count <= kMaxClasses)
                cls = ReadU32(arr + 4 * idx);
        }
    }
    char type[64];
    if (!cls || !TypeName(cls, type, sizeof(type)))
        snprintf(type, sizeof(type), "-");
    const uint32_t chClass = ch ? ReadU32(ch + kCharClass) : 0;
    if (!Budget())
        return;
    Log("[wheel] %lu ms: SCREEN mode %u %s, team shown %u, class index %u of %u = 0x%08X %s | character 0x%08X: team "
        "%ld, class 0x%08X%s, unit 0x%08X",
        ScanMsSinceAttach(), static_cast<unsigned>(w.mode), ModeName(w.mode), static_cast<unsigned>(teamNo),
        static_cast<unsigned>(idx), static_cast<unsigned>(count), static_cast<unsigned>(cls), type,
        static_cast<unsigned>(ch), ch ? static_cast<long>(static_cast<int32_t>(ReadU32(ch + kCharTeam))) : 0L,
        static_cast<unsigned>(chClass), (cls && chClass == cls) ? " (= the selected class)" : "",
        static_cast<unsigned>(ch ? ReadU32(ch + kCharUnit) : 0));
}

// ---- 18b: the IF element registry ----------------------------------------
uint32_t HashMore(uint32_t h, const char* s)
{
    for (; *s; ++s)
        h = (h ^ (static_cast<uint32_t>(static_cast<unsigned char>(*s)) | 0x20u)) * 0x01000193u;
    return h;
}

uint32_t PathHash(const char* s)
{
    return HashMore(0x811C9DC5u, s);
}

uint32_t g_keys[kRegSlots];
uint32_t g_vals[kRegSlots];

uintptr_t RegLookup(uint32_t hash)
{
    if (!hash)
        return 0;
    uint32_t i = hash & (kRegSlots - 1);
    for (uint32_t n = 0; n < kRegSlots; ++n)
    {
        const uint32_t k = g_keys[i];
        if (k == hash)
            return g_vals[i];
        if (!k)
            return 0;
        i = i ? i - 1 : kRegSlots - 1;
    }
    return 0;
}

bool IfBudget()
{
    if (g_ifLines >= kMaxIfLines)
        return false;
    if (++g_ifLines == kMaxIfLines)
        Log("[wheel] IF scan line limit reached");
    return true;
}

// words tried after each found path ("<path>.<word>"); hashing ignores case
const char* const kWords[] = {
    "Info", "Window", "BoxText", "titleBarElement", "UnitNameText", "UnitName", "UnitText", "Helptext_Back",
    "Helptext_Accept", "Helptext_Select", "Helptext_Ok", "Helptext_Cancel", "Helptext_Prev", "Helptext_Next",
    "Helptext_Spawn", "Helptext_Map", "Helptext", "HelpText", "Help", "btn", "label", "text", "title", "buttons",
    "SideBar", "Hints", "Objectives", "Spawnpoint", "SpawnPoint", "Vehicle", "Vehicles", "VehicleList", "Skin",
    "ChunkC", "ChunkTL", "ChunkTR", "ChunkBL", "ChunkBR", "ChunkL", "ChunkR", "ChunkT", "ChunkB", "ChunkCL", "ChunkCR",
    "ChunkTC", "ChunkBC", "InfoText", "Ok", "PopupWindow", "ObjectiveWindow", "Model", "Models", "Icon", "Icons",
    "Name", "Map", "Background", "Cursor", "Arrow", "Arrows", "LeftArrow", "RightArrow", "UpArrow", "DownArrow",
    "ArrowL", "ArrowR", "Left", "Right", "Center", "Middle", "Accept", "Back", "Select", "Prompt", "Timer", "Unit",
    "Units", "Weapons", "Weapon", "Equipment", "Header", "List", "listbox", "Spawn", "Score", "Hilight", "Box", "Line",
    "border", "titlebar", "Image", "Logo", "Side", "team", "Republic", "CIS", "Rebel", "Rebels", "Empire", "Alliance",
    "Imperial", "Wheel", "Carousel", "Ring", "Platform", "Selection", "Choose", "Ready", "Respawn", "Countdown", "Time",
    "Panel", "Frame", "Bar", "bkg", "Hint", "Objective", "CommandPost", "Post", "Desc", "Description", "Stats",
    "Title", "Subtitle", "Caption", "Status", "Message", "Msg", "Teams", "Sides", "Classes", "Class", "Soldier",
    "Soldiers", "Character", "Characters", "Unitlist", "UnitList", "Cost", "Points", "Locked", "Lock", "Rotate",
    "Display", "Holo", "Hologram", "Shadow", "Glow", "Flash", "Pulse", "Container", "Group", "Main", "Top", "Bottom",
    "Upper", "Lower", "Value", "Values", "Count", "Number", "Ammo", "Health", "Primary", "Secondary", "Grenade",
    "Weapon1", "Weapon2", "Weapon3", "Weapon4", "Weapon5", "SideIcon", "SideModel", "SlotWindow", "chunk", "Overlay",
    "MapBox", "MapWindow", "MiniMap", "Minimap", "Radar", "Legend", "Key", "Blip", "Blips", "Marker", "Markers",
    nullptr
};
// numbered words: <word>0 .. <word>12
const char* const kNumbered[] = { "SlotWindow", "SideIcon", "SideModel", "Model", "Icon", "Text", "Line", "Box",
                                  "Window", "Unit", "team", "Side", "button", "btn", "label", "Slot", "Item", "Weapon",
                                  "Name", "Arrow", "Helptext", "Image", "Bar", "Hint", "Objective", "Vehicle",
                                  "Spawnpoint", "Post", "Seg", "Class", "Soldier", "Title", "Info", "Panel", "Model_",
                                  "Icon_", "Unit_", "Slot_", "Text_", nullptr };

constexpr int kMaxVocab = 900;
char g_vocab[kMaxVocab][24];
int g_vocabCount = 0;

void BuildVocab()
{
    g_vocabCount = 0;
    for (int i = 0; kWords[i] && g_vocabCount < kMaxVocab; ++i)
        snprintf(g_vocab[g_vocabCount++], sizeof(g_vocab[0]), "%s", kWords[i]);
    for (int i = 0; kNumbered[i]; ++i)
        for (int n = 0; n <= 12 && g_vocabCount < kMaxVocab; ++n)
            snprintf(g_vocab[g_vocabCount++], sizeof(g_vocab[0]), "%s%d", kNumbered[i], n);
}

struct Found
{
    uint32_t hash;
    uintptr_t obj;
    char path[160];
};
constexpr int kMaxFound = 2500;
Found g_found[kMaxFound];
int g_foundCount = 0;

struct ScreenEntry
{
    uint32_t hash;
    uintptr_t record;
    char name[64];
};
ScreenEntry g_screens[kMaxScreens];
int g_screenCount = 0;

int FindFound(uint32_t hash)
{
    for (int i = 0; i < g_foundCount; ++i)
        if (g_found[i].hash == hash)
            return i;
    return -1;
}

// adds a named element (once); false if it was not new or there is no room
bool AddFound(uint32_t hash, uintptr_t obj, const char* path)
{
    if (FindFound(hash) >= 0 || g_foundCount >= kMaxFound)
        return false;
    Found& f = g_found[g_foundCount++];
    f.hash = hash;
    f.obj = obj;
    snprintf(f.path, sizeof(f.path), "%s", path);
    return true;
}

int FindScreen(uint32_t hash)
{
    for (int i = 0; i < g_screenCount; ++i)
        if (g_screens[i].hash == hash)
            return i;
    return -1;
}

void NameScreen(const char* name)
{
    const int i = FindScreen(PathHash(name));
    if (i >= 0 && !g_screens[i].name[0])
        snprintf(g_screens[i].name, sizeof(g_screens[i].name), "%s", name);
}

int CompareFound(const void* a, const void* b)
{
    return _stricmp(static_cast<const Found*>(a)->path, static_cast<const Found*>(b)->path);
}

void LogElement(const char* indent, const Found& f)
{
    char type[64];
    if (!TypeName(f.obj, type, sizeof(type)))
        snprintf(type, sizeof(type), "type ?");
    const uint32_t flags = ReadU32(f.obj + kIfFlags);
    if (IfBudget())
        Log("[wheel] %s%s  (%s, %s, 0x%08X)", indent, f.path, type, (flags & 0x100) ? "shown" : "hidden",
            static_cast<unsigned>(f.obj));
}

// the word search: "<path>.<word>" for every word, starting from a screen name (which does not
// have to be in the registry itself), up to 6 levels down
void WordSearch(const char* root)
{
    struct Node
    {
        uint32_t hash;
        char path[160];
    };
    static Node level[2][600];
    int count[2] = { 1, 0 };
    level[0][0].hash = PathHash(root);
    snprintf(level[0][0].path, sizeof(level[0][0].path), "%s", root);
    for (int depth = 0; depth < 6; ++depth)
    {
        Node* cur = level[depth & 1];
        Node* nxt = level[(depth + 1) & 1];
        int n = 0;
        for (int q = 0; q < count[depth & 1]; ++q)
        {
            const uint32_t dot = HashMore(cur[q].hash, ".");
            for (int w = 0; w < g_vocabCount; ++w)
            {
                const uint32_t h = HashMore(dot, g_vocab[w]);
                const uintptr_t obj = RegLookup(h);
                if (!obj)
                    continue;
                char path[160];
                snprintf(path, sizeof(path), "%s.%s", cur[q].path, g_vocab[w]);
                if (!AddFound(h, obj, path) || n >= 600)
                    continue;
                nxt[n].hash = h;
                snprintf(nxt[n].path, sizeof(nxt[n].path), "%s", path);
                ++n;
            }
        }
        count[(depth + 1) & 1] = n;
        if (!n)
            break;
    }
}

bool IsPathChar(unsigned char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '.';
}

// every "ifs_..." text in the game's memory (not the exe, not this DLL): the whole text and each
// part of it up to a '.' are hashed; a match in the screen list or the registry names it
void TextSearch(int* textsSeen, int* named)
{
    *textsSeen = 0;
    *named = 0;
    HMODULE self = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCSTR>(&g_lines), &self);
    const BuildInfo* b = CurrentBuild();
    const uintptr_t exeLo = ExeBase();
    const uintptr_t exeHi = ExeBase() + (b ? b->sizeOfImage : 0);
    const size_t kChunk = 1 << 20;
    unsigned char* buf =
        static_cast<unsigned char*>(VirtualAlloc(nullptr, kChunk + 256, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!buf)
        return;
    const DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    uintptr_t addr = 0x10000;
    MEMORY_BASIC_INFORMATION mbi;
    while (addr < 0xFFFF0000u && VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)))
    {
        const uintptr_t regionLo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        const uintptr_t regionHi = regionLo + mbi.RegionSize;
        if (regionHi <= addr)
            break;
        const bool skip = mbi.State != MEM_COMMIT || !(mbi.Protect & readable) || (mbi.Protect & PAGE_GUARD) ||
                          (regionLo < exeHi && regionHi > exeLo) || (self && mbi.AllocationBase == self) ||
                          regionLo == reinterpret_cast<uintptr_t>(buf);
        for (uintptr_t p = regionLo; !skip && p < regionHi; p += kChunk)
        {
            const size_t want = (regionHi - p < kChunk + 200) ? static_cast<size_t>(regionHi - p) : kChunk + 200;
            SIZE_T got = 0;
            if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(p), buf, want, &got) || got < 8)
                continue;
            for (size_t i = 0; i + 4 <= got && i < kChunk; ++i)
            {
                if (buf[i] != 'i' || buf[i + 1] != 'f' || buf[i + 2] != 's' || buf[i + 3] != '_')
                    continue;
                if (i > 0 && IsPathChar(buf[i - 1]))
                    continue;
                size_t n = 4;
                while (i + n < got && n < 159 && IsPathChar(buf[i + n]))
                    ++n;
                if (i + n >= got || buf[i + n] != 0 || n < 6)
                    continue;
                ++*textsSeen;
                char text[160];
                memcpy(text, buf + i, n);
                text[n] = 0;
                // the screen name part, then every part up to a '.', then the whole text
                uint32_t h = 0x811C9DC5u;
                for (size_t k = 0; k <= n; ++k)
                {
                    if (k == n || text[k] == '.')
                    {
                        char part[160];
                        memcpy(part, text, k);
                        part[k] = 0;
                        if (!strchr(part, '.'))
                            NameScreen(part);
                        const uintptr_t obj = RegLookup(h);
                        if (obj && AddFound(h, obj, part))
                            ++*named;
                    }
                    if (k < n)
                        h = (h ^ (static_cast<uint32_t>(static_cast<unsigned char>(text[k])) | 0x20u)) * 0x01000193u;
                }
            }
        }
        addr = regionHi;
    }
    VirtualFree(buf, 0, MEM_RELEASE);
}

void ReadScreenList()
{
    g_screenCount = 0;
    if (!g_screenList)
        return;
    uintptr_t node = ReadU32(g_screenList);
    for (int guard = 0; node && node != g_screenList && guard < 1024 && g_screenCount < kMaxScreens; ++guard)
    {
        const uintptr_t record = ReadU32(node + 0x0C);
        if (record)
        {
            ScreenEntry& e = g_screens[g_screenCount++];
            e.hash = ReadU32(record + 4);
            e.record = record;
            e.name[0] = 0;
        }
        node = ReadU32(node);
    }
}

// the elements named under one screen (the list is sorted by then)
void LogScreenElements(const char* screen, bool listThem)
{
    char prefix[80];
    snprintf(prefix, sizeof(prefix), "%s.", screen);
    const size_t pl = strlen(prefix);
    int n = 0;
    for (int i = 0; i < g_foundCount; ++i)
        if (!_strnicmp(g_found[i].path, prefix, pl))
            ++n;
    const int si = FindScreen(PathHash(screen));
    if (IfBudget())
        Log("[wheel] %s: %s, %d element(s) named%s", screen, si >= 0 ? "IN the screen list" : "not in the screen list",
            n, (n && listThem) ? " (path, C++ type, shown/hidden, address):" : "");
    for (int i = 0; listThem && i < g_foundCount; ++i)
        if (!_strnicmp(g_found[i].path, prefix, pl))
            LogElement("    ", g_found[i]);
}

void IfScan()
{
    if (!g_registry)
        return;
    if (!ReadMem(g_registry + 4, g_keys, sizeof(g_keys)) ||
        !ReadMem(g_registry + 4 + sizeof(g_keys), g_vals, sizeof(g_vals)))
    {
        Log("[wheel] IF scan: the registry at 0x%08X is unreadable", static_cast<unsigned>(g_registry));
        return;
    }
    BuildVocab();
    g_foundCount = 0;
    ReadScreenList();
    int used = 0;
    for (uint32_t i = 0; i < kRegSlots; ++i)
        if (g_keys[i])
            ++used;
    if (IfBudget())
        Log("[wheel] %lu ms: IF scan - %d elements in the registry, %d screens in the screen list%s", ScanMsSinceAttach(),
            used, g_screenCount, g_screenList ? "" : " (screen list not found)");

    // 1. the check: paths the exe itself looks up on the PC screen (they must be there)
    const char* const kControl[] = { "ifs_pc_SpawnSelect.Info",
                                     "ifs_pc_SpawnSelect.Info.PopupWindow",
                                     "ifs_pc_SpawnSelect.Info.PopupWindow.Text",
                                     "ifs_pc_SpawnSelect.Info.PopupWindow.titleBarElement",
                                     "ifs_pc_SpawnSelect.Info.SideIcon0",
                                     "ifs_pc_SpawnSelect.Info.SideIcon0.ChunkC",
                                     "ifs_pc_SpawnSelect.Info.SideModel0",
                                     "ifs_pc_SpawnSelect.Info.SideModel1",
                                     "ifs_pc_SpawnSelect.Info.Ok.ChunkC",
                                     "ifs_pc_SpawnSelect.Info.SlotWindow0",
                                     "ifs_pc_SpawnSelect.Info.SlotWindow0.titleBarElement",
                                     "ifs_pc_SpawnSelect.Info.SlotWindow0.InfoText",
                                     "ifs_pc_SpawnSelect.Info.ObjectiveWindow.Text",
                                     "ifs_pc_SpawnSelect.Info.ObjectiveWindow.titleBarElement",
                                     "ifs_readyselect1.Helptext_Back.btn.label",
                                     "ifs_mapselect1.SideBar.Objectives.Skin.ChunkC",
                                     nullptr };
    int controlFound = 0;
    int controlCount = 0;
    for (int i = 0; kControl[i]; ++i)
    {
        ++controlCount;
        const uint32_t h = PathHash(kControl[i]);
        const uintptr_t obj = RegLookup(h);
        if (obj)
        {
            ++controlFound;
            AddFound(h, obj, kControl[i]);
        }
        char type[64] = "-";
        if (obj && !TypeName(obj, type, sizeof(type)))
            snprintf(type, sizeof(type), "type ?");
        if (IfBudget())
            Log("[wheel]   check: %s -> %s%s%s", kControl[i], obj ? "FOUND (" : "not found", obj ? type : "",
                obj ? ")" : "");
    }
    if (IfBudget())
        Log("[wheel] check: %d of %d known paths found in the registry", controlFound, controlCount);

    // 2. the word search under every candidate screen
    const char* const kScreens[] = { "ifs_readyselect1",  "ifs_readyselect2",  "ifs_readyselect",  "ifs_charselect1",
                                     "ifs_charselect2",   "ifs_charselect",    "ifs_mapselect1",   "ifs_mapselect2",
                                     "ifs_mapselect",     "ifs_sideselect1",   "ifs_sideselect2",  "ifs_sideselect",
                                     "ifs_pc_SpawnSelect", "ifs_pausemenu",    "ifs_pausemenu2",   "ifs_teamstats",
                                     nullptr };
    for (int i = 0; kScreens[i]; ++i)
    {
        NameScreen(kScreens[i]);
        WordSearch(kScreens[i]);
    }
    const int byWords = g_foundCount;

    // 3. every "ifs_..." text in memory
    int texts = 0;
    int byTexts = 0;
    const DWORD t0 = GetTickCount();
    TextSearch(&texts, &byTexts);
    if (IfBudget())
        Log("[wheel] named: %d by the check + word search, %d more from %d \"ifs_\" texts in memory (%lu ms); %d of %d "
            "registry elements named",
            byWords, byTexts, texts, static_cast<unsigned long>(GetTickCount() - t0), g_foundCount, used);

    // the screen list, named where possible
    char line[960];
    size_t pos = 0;
    line[0] = 0;
    int unnamed = 0;
    for (int i = 0; i < g_screenCount; ++i)
    {
        if (!g_screens[i].name[0])
        {
            ++unnamed;
            continue;
        }
        const int w = snprintf(line + pos, sizeof(line) - pos, "%s%s", pos ? ", " : "", g_screens[i].name);
        if (w < 0 || static_cast<size_t>(w) >= sizeof(line) - pos)
        {
            if (IfBudget())
                Log("[wheel] screens: %s", line);
            pos = 0;
            line[0] = 0;
            const int w2 = snprintf(line, sizeof(line), "%s", g_screens[i].name);
            pos = (w2 > 0) ? static_cast<size_t>(w2) : 0;
            continue;
        }
        pos += static_cast<size_t>(w);
    }
    if (pos && IfBudget())
        Log("[wheel] screens: %s", line);
    if (IfBudget())
        Log("[wheel] screen list: %d screens, %d named above, %d not named", g_screenCount, g_screenCount - unnamed,
            unnamed);

    // the console screens and the PC screen, element by element
    qsort(g_found, static_cast<size_t>(g_foundCount), sizeof(Found), CompareFound);
    for (int i = 0; kScreens[i]; ++i)
    {
        const bool consoleOrPc = strncmp(kScreens[i], "ifs_pausemenu", 13) != 0 && strcmp(kScreens[i], "ifs_teamstats") != 0;
        LogScreenElements(kScreens[i], consoleOrPc); // the pause menu and team stats: counts only
    }
    if (IfBudget())
        Log("[wheel] IF scan done");
}

// finds the registry through the PC spawn screen's setup -> FindByPath -> FindByHash
void FindRegistry(const BuildInfo* build)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kSetup, hits, 4);
    if (n != 1)
    {
        Log("[wheel] IF scan OFF: the spawn screen setup has %d pattern hits (need exactly 1)", n);
        return;
    }
    const uint32_t call = hits[0] + kSetupFindCall;
    if (*reinterpret_cast<const unsigned char*>(Rva(call)) != 0xE8)
    {
        Log("[wheel] IF scan OFF: no call at setup+15Dh (rva 0x%08X)", call);
        return;
    }
    const uint32_t findByPath = CallTarget(call);
    if (!MatchPatternAt(findByPath, kFindByPath))
    {
        Log("[wheel] IF scan OFF: FindByPath at rva 0x%08X does NOT match", findByPath);
        return;
    }
    const uint32_t hashFn = CallTarget(CallTarget(findByPath + kFindHashCtorCall) + kHashCtorCall);
    if (!MatchPatternAt(hashFn + kHashLoopAt, kHashLoop))
    {
        Log("[wheel] IF scan OFF: the path hash at rva 0x%08X is not FNV-1a (char | 20h)", hashFn);
        return;
    }
    const uint32_t findByHash = CallTarget(findByPath + kFindByHashCall);
    if (!MatchPatternAt(findByHash, kFindByHash))
    {
        Log("[wheel] IF scan OFF: FindByHash at rva 0x%08X does NOT match", findByHash);
        return;
    }
    const uint32_t lookup = CallTarget(findByHash + kLookupCall);
    if (!MatchPatternAt(lookup, kLookup))
    {
        Log("[wheel] IF scan OFF: the registry lookup at rva 0x%08X does NOT match", lookup);
        return;
    }
    const uint32_t probe = CallTarget(lookup + kProbeCall);
    if (!MatchPatternAt(probe, kProbe))
    {
        Log("[wheel] IF scan OFF: the registry probe at rva 0x%08X does NOT match", probe);
        return;
    }
    const uintptr_t reg = ReadU32(Rva(findByHash + kRegistryImm));
    if (reg < ExeBase() || reg + 4 + 2 * sizeof(g_keys) > ExeBase() + build->sizeOfImage)
    {
        Log("[wheel] IF scan OFF: the registry (0x%08X) is outside the exe", static_cast<unsigned>(reg));
        return;
    }
    g_registry = reg;
    g_setupRva = hits[0];
    Log("[wheel] IF registry: setup rva 0x%08X, FindByPath 0x%08X, hash 0x%08X (FNV-1a), FindByHash 0x%08X, lookup "
        "0x%08X, probe 0x%08X - all OK; registry at rva 0x%08X (%u slots)",
        hits[0], findByPath, hashFn, findByHash, lookup, probe, static_cast<unsigned>(reg - ExeBase()),
        static_cast<unsigned>(kRegSlots));
}

// 18c: the screen list, through ScreenByHash
void FindScreenList(const BuildInfo* build)
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kScreenByHash, hits, 4);
    if (n != 1)
    {
        Log("[wheel] screen list OFF: ScreenByHash has %d pattern hits (need exactly 1)", n);
        return;
    }
    const uint32_t f = hits[0];
    const uintptr_t list = ReadU32(Rva(f + kScreenListImm1));
    if (list != ReadU32(Rva(f + kScreenListImm2)) || list < ExeBase() || list >= ExeBase() + build->sizeOfImage)
    {
        Log("[wheel] screen list OFF: the two list addresses in ScreenByHash differ or leave the exe");
        return;
    }
    if (!MatchPatternAt(CallTarget(f + kScreenDerefCall), kDerefBody) ||
        !MatchPatternAt(CallTarget(f + kScreenHashCall), kRecordHashBody) ||
        !MatchPatternAt(CallTarget(f + kScreenNextCall), kNextBody))
    {
        Log("[wheel] screen list OFF: ScreenByHash's node / record / next helpers do NOT match");
        return;
    }
    g_screenList = list;
    Log("[wheel] screen list: ScreenByHash rva 0x%08X OK (node, record and next helpers OK), list at rva 0x%08X", f,
        static_cast<unsigned>(list - ExeBase()));
}

DWORD WINAPI WatchThread(LPVOID)
{
    Watch w;
    bool waitingLogged = false;
    uintptr_t teamsDumpedFor = 0;
    DWORD ifSince = 0;
    bool ifScanned = false;
    for (;;)
    {
        Sleep(kPollMs);
        const uintptr_t sd = ReadU32(g_global);
        if (!sd)
        {
            if (w.display && Budget())
                Log("[wheel] %lu ms: the spawn screen object 0x%08X is gone (match over?)", ScanMsSinceAttach(),
                    static_cast<unsigned>(w.display));
            if (w.display)
            {
                const LONG was = g_previewIdx;
                if ((was == 2 || was == 3) && InterlockedCompareExchange(&g_previewIdx, 1, was) == was && Budget())
                    Log("[wheel] console flow: the next match starts at the side screen (ifs_readyselect1)");
            }
            if (!waitingLogged && Budget())
            {
                Log("[wheel] %lu ms: no spawn screen object yet (the game makes it when a match loads)",
                    ScanMsSinceAttach());
                waitingLogged = true;
            }
            w = Watch();
            continue;
        }
        waitingLogged = false;
        if (sd != w.display)
        {
            w = Watch();
            w.display = sd;
            w.valid = ReadU32(sd) == g_vtable;
            if (Budget())
                Log("[wheel] %lu ms: spawn screen object 0x%08X, vtable %s", ScanMsSinceAttach(),
                    static_cast<unsigned>(sd), w.valid ? "OK (SpawnDisplay)" : "WRONG - this object is not read");
        }
        if (!w.valid)
            continue;

        // 18d: F6 cycles the preview (only while a match has the spawn screen)
        const bool f6 = g_wheelEnabled && (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
        if (f6 && !g_f6Down)
        {
            // 18j: the console order - PC (0) -> side (1) -> spawn point (3) -> soldier select (2) -> PC
            static const LONG kF6Next[4] = { 1, 3, 0, 2 };
            const LONG cur = g_previewIdx;
            const LONG next = (kPreviewCount == 4 && cur >= 0 && cur < 4) ? kF6Next[cur] : 0;
            InterlockedExchange(&g_previewIdx, next);
            Log("[wheel] %lu ms: preview (F6): the spawn screen now shows %s while you choose", ScanMsSinceAttach(),
                kPreview[next] ? kPreview[next] : "the PC screen (ifs_pc_SpawnSelect, as the game has it)");
        }
        g_f6Down = f6;

        uint32_t now[kTailDwords];
        const uint32_t mode = ReadU32(sd + kMode);
        if (!ReadMem(sd + kTailStart, now, sizeof(now)))
            continue;

        if (!w.have)
        {
            memcpy(w.tail, now, sizeof(now));
            w.mode = mode;
            w.have = true;
            if (Budget())
                Log("[wheel] %lu ms: spawn screen fields +2370h..+23EFh (mode +18h = %u):", ScanMsSinceAttach(),
                    static_cast<unsigned>(mode));
            for (int r = 0; r < kTailDwords; r += 4)
                if (Budget())
                    Log("[wheel]   +%04X: %08X %08X %08X %08X",
                        static_cast<unsigned>(kTailStart + 4 * static_cast<uint32_t>(r)), static_cast<unsigned>(now[r]),
                        static_cast<unsigned>(now[r + 1]), static_cast<unsigned>(now[r + 2]),
                        static_cast<unsigned>(now[r + 3]));
            LogScreen(w);
        }
        else
        {
            bool important = false;
            if (mode != w.mode)
            {
                ++w.modeChanges;
                if (w.modeChanges <= kMuteAfter && Budget())
                    Log("[wheel] %lu ms: +18h mode %u -> %u (%s)", ScanMsSinceAttach(), static_cast<unsigned>(w.mode),
                        static_cast<unsigned>(mode), ModeName(mode));
                w.mode = mode;
                important = true;
            }
            for (int i = 0; i < kTailDwords; ++i)
            {
                if (now[i] == w.tail[i] || Noisy(i))
                    continue;
                if (i == kIdxCharacter || i == kIdxTeamShown || i == kIdxClassIndex)
                    important = true;
                ++w.changes[i];
                if (w.changes[i] <= kMuteAfter && Budget())
                {
                    char nb[24];
                    float f = 0.0f;
                    memcpy(&f, &now[i], sizeof(f));
                    Log("[wheel] %lu ms: %s %08X -> %08X (as int %ld, as float %.3f)%s", ScanMsSinceAttach(),
                        FieldName(i, nb, sizeof(nb)), static_cast<unsigned>(w.tail[i]), static_cast<unsigned>(now[i]),
                        static_cast<long>(static_cast<int32_t>(now[i])), static_cast<double>(f),
                        w.changes[i] == kMuteAfter ? "  [changes a lot - muted from here]" : "");
                }
                w.tail[i] = now[i];
            }
            if (important)
                LogScreen(w);
        }

        // the class lists, once per match, when the screen first has a character
        const uintptr_t sm = SpawnScanManager();
        if (sm && sm != teamsDumpedFor && w.tail[kIdxCharacter])
        {
            teamsDumpedFor = sm;
            LogTeams(sm);
        }
        // 18b: the IF scan, once, 2 s into the first PC spawn screen (the match's screens exist by then)
        if (!ifScanned && g_registry && w.tail[kIdxCharacter] && w.mode == 1)
        {
            if (!ifSince)
                ifSince = GetTickCount();
            else if (GetTickCount() - ifSince >= kIfScanDelayMs)
            {
                ifScanned = true;
                IfScan();
            }
        }
    }
}

} // namespace

namespace {

// ---- 18e: the console driver, first piece (game thread) --------------------
struct Driver
{
    uintptr_t display = 0;   // the spawn screen object the lookups below belong to
    uintptr_t title = 0;     // ifs_charselect1.Info.Window.titleBarElement
    uintptr_t box = 0;       // ifs_charselect1.Info.Window.BoxText
    uintptr_t shownClass = 0;
    bool wasOn = false;
    bool leftDown = false;
    bool rightDown = false;
    bool aDown = false;
    bool bDown = false;
    bool clickDown = false;  // 18q
    bool upDown = false;
    bool downDown = false;
    DWORD upSince = 0;
    DWORD downSince = 0;
    DWORD upLast = 0;
    DWORD downLast = 0;
    uintptr_t postTitle = 0;  // 18g: ifs_mapselect1.SideBar.Spawnpoint.titleBarElement
    uintptr_t shownPost = 0;  // the post whose name the title shows
    DWORD leftSince = 0;
    DWORD rightSince = 0;
    DWORD leftLast = 0;
    DWORD rightLast = 0;
    int lines = 0;
    int skipLines = 0;
    bool panelsMoved = false; // 18z: ifs_mapselect1's panels moved (once per spawn screen)
    bool windowMoved = false; // 18z: ifs_charselect1's window moved
    int panelTries = 0;       // 19h: frames the flat layout was tried on (its elements can come a frame late)
    int windowTries = 0;
    bool backLogged = false;  // 19h: the spawn point screen's "B BACK" hidden (logged once per spawn screen)
    bool panelsPinned = false; // 19h-r2: the spawn point screen's groups pinned (19g's places)
    bool windowPinned = false; // 19h-r2: the class screen's
    bool sideBackHidden = false; // 19g: ifs_readyselect1's "B BACK" hidden (split screen)
    DWORD lastFrame = 0;         // 21h-r6: his flow's last frame (a gap: it was stopped - a pause)
};
Driver g_drv;
// 18z: where the Xbox has them, in the console screens' own units (player 1's "1" screens and player
// 2's "2" screens use the same scripts): the panels are children of a group anchored at the half's
// right edge (x from it), the window a child of one anchored top right
// 19a: player 1's half draws about 0.23 of its width further right than player 2's (his HUD root has
// an x offset of 0.1 that player 2's has not; 18z's video) - his own x for the same places
// 19g: from 19f's video against the Xbox shots (player 1's half: the panels' centre at 52% of the
// width - they stood at 40% - their top at 48% of the height; the panels move about 0.67 of a root
// unit per unit, the root's x offset 0.1 moves everything of his 0.1 of the half right of player 2's)
constexpr float kXbPanelX = -400.0f; // the panels centred (19a-19f: -495)
constexpr float kXbPostY = 305.0f;   // COMMAND POST (the script's own spacing 60 / 139 / 284, shifted down 245)
constexpr float kXbVehY = 384.0f;    // VEHICLES
constexpr float kXbObjY = 529.0f;    // OBJECTIVES
constexpr float kXbWindowX = -408.0f; // the class window centred under the ring (was -498)
constexpr float kXbWindowY = 375.0f;  // its title at about 47% (was 300: 41%)
// 19g: "B BACK" moved in and up to the Xbox's place (its group pinned; the script has it at 0, -15)
constexpr float kXbBackX = 37.0f;
constexpr float kXbBackY = -41.0f;

bool DriverBudget()
{
    return g_drv.lines++ < 200;
}

uint32_t LocalPlayersNow()
{
    const BuildInfo* b = CurrentBuild();
    return (b && b->localPlayerCountRva) ? ReadU32(Rva(b->localPlayerCountRva)) : 0;
}

// 20g-r12: horizontal 2-player ([Splitscreen] MenusHorizontal; Vertical as vertical.cpp reads it - logged only)
bool g_hzWanted = true;
bool g_verticalIni = false;
int g_hzLines = 0;

bool HzBudget()
{
    return g_hzLines++ < 80;
}

// 20g-r7: 4-player with player 1's screens at players 3 / 4's sizes ([Splitscreen] MenusLikeP34)
bool g_quarterLook = true;
bool QuarterLook()
{
    return g_quarterLook && LocalPlayersNow() >= 2 && PlayerNLocalPlayers() >= 3;
}
constexpr float kQuarterSideSize = 0.34f;   // players 3 / 4's side soldiers (split 0.54)
constexpr float kQuarterRingSize = 0.40f;   // their ring's picked soldier (split 0.508; 20g-r9: 0.36 -> 0.40, BF2's 4-player)
constexpr float kQuarterRingWidth = 1.5f;   // their ring's radius against the split one (20g-r9: 0.75 -> 1.5, BF2's 4-player
                                            // wheel spans about half the quarter)

// the console flow is on: a console screen picked (F6 / A / B) in a 1-player game (21j-r18: with Enabled - the SPAWN SCREEN
// row's CLASS WHEEL), or in split screen with Split
bool ConsoleActive()
{
    const LONG i = g_previewIdx;
    return i > 0 && i < kPreviewCount && (LocalPlayersNow() < 2 ? g_wheelEnabled : g_wheelSplit);
}

void SetFlow(LONG idx, const char* why)
{
    InterlockedExchange(&g_previewIdx, idx);
    if (DriverBudget())
        Log("[wheel] %lu ms: flow: %s -> %s", ScanMsSinceAttach(), why, kPreview[idx] ? kPreview[idx] : "the PC screen");
}

// press: true once per press. step: once per press, then repeating while held
bool Press(bool now, bool& down)
{
    const bool fire = now && !down;
    down = now;
    return fire;
}

bool Step(bool now, bool& down, DWORD& since, DWORD& last)
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

struct FlowInput
{
    bool left;
    bool right;
    bool up;
    bool down;
    bool a;
    bool b;
    bool click;   // 18q: the left mouse button (what it does depends on where the cursor is)
};

// the first connected XInput pad (player 1 in a 1-player game) plus the keyboard
FlowInput ReadFlowInput()
{
    FlowInput in = { false, false, false, false, false, false, false };
    if (g_xGetState)
    {
        for (DWORD i = 0; i < 4; ++i)
        {
            XState s = {};
            if (g_xGetState(i, &s) != 0)
                continue;
            in.left = (s.buttons & 0x0004) != 0 || s.lx < -16000;
            in.right = (s.buttons & 0x0008) != 0 || s.lx > 16000;
            in.up = (s.buttons & 0x0001) != 0 || s.ly > 16000;
            in.down = (s.buttons & 0x0002) != 0 || s.ly < -16000;
            in.a = (s.buttons & 0x1000) != 0;
            in.b = (s.buttons & 0x2000) != 0;
            break;
        }
    }
    in.left = in.left || (GetAsyncKeyState(VK_LEFT) & 0x8000) != 0;
    in.right = in.right || (GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0;
    in.up = in.up || (GetAsyncKeyState(VK_UP) & 0x8000) != 0;
    in.down = in.down || (GetAsyncKeyState(VK_DOWN) & 0x8000) != 0;
    in.a = in.a || (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;
    in.b = in.b || (GetAsyncKeyState(VK_BACK) & 0x8000) != 0;
    // 18p: the mouse - right button = B. 18q: the left button is its own input (where the cursor is decides)
    in.click = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    in.b = in.b || (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    return in;
}

// the PC screen's input function, skipped while a console screen is up
void __fastcall PcInputHook(void* sd, void* unusedEdx)
{
    if (ConsoleActive())
    {
        if (g_drv.skipLines < 6 && DriverBudget())
        {
            ++g_drv.skipLines;
            Log("[wheel] %lu ms: PC spawn screen input skipped (console screen up)", ScanMsSinceAttach());
        }
        return;
    }
    g_drv.skipLines = 0;
    g_pcInput(sd, unusedEdx);
}

bool ReadWide(uintptr_t va, wchar_t* out, size_t cap); // defined below

// 18g: the post's name on the spawn point panel's title (until the panel gets its own text)
void ShowPostName(uintptr_t post)
{
    // 18o: the panel's title bar keeps the game's own text; the post's name goes inside the box
    // (PostNameText, in the screen texts)
    (void)post;
}

// 18g: the previous / next post your team can use. The list: NextSpawnPoint round once from
// cursor 0 (the team's usable posts, in the game's fixed order), then the cursor goes back.
void CyclePost(uintptr_t sd, int step)
{
    const uintptr_t sm = SpawnScanManager();
    const int team = static_cast<int>(ReadU32(sd + kTailStart + 4 * kIdxTeamShown));
    if (!sm || !g_nextPost || !g_setPost)
        return;
    volatile int32_t* cursor = reinterpret_cast<volatile int32_t*>(sm + kSmPostCursor);
    const int32_t saved = *cursor;
    uintptr_t posts[64];
    int n = 0;
    *cursor = 0;
    while (n < 64)
    {
        const uintptr_t p = g_nextPost(reinterpret_cast<void*>(sm), nullptr, team);
        if (!p || (n > 0 && p == posts[0]))
            break;
        posts[n++] = p;
    }
    *cursor = saved;
    if (!n)
    {
        if (DriverBudget())
            Log("[wheel] %lu ms: map: team %d has no post to spawn at", ScanMsSinceAttach(), team);
        return;
    }
    const uintptr_t current = ReadU32(sd + kSdPost);
    int k = -1;
    for (int i = 0; i < n; ++i)
        if (posts[i] == current)
            k = i;
    const int to = (k < 0) ? (step > 0 ? 0 : n - 1) : (k + step + n) % n;
    *cursor = (to + 1) % n; // the PC's own next-post input carries on from here
    g_setPost(reinterpret_cast<void*>(sd), nullptr, posts[to]);
    wchar_t name[96];
    char a[200] = "?";
    if (ReadWide(ReadU32(posts[to] + kPostName), name, 96))
        WideCharToMultiByte(CP_UTF8, 0, name, -1, a, sizeof(a), nullptr, nullptr);
    if (DriverBudget())
        Log("[wheel] %lu ms: map: %s -> post %d of %d \"%s\" (0x%08X; was %d, now at +23E0h 0x%08X)",
            ScanMsSinceAttach(), step > 0 ? "next" : "previous", to + 1, n, a, static_cast<unsigned>(posts[to]), k + 1,
            static_cast<unsigned>(ReadU32(sd + kSdPost)));
    ShowPostName(ReadU32(sd + kSdPost));
}

// ---- 18h/18i: the ring (game thread) ----------------------------------------
// One element per class of the team shown (the game's own SoldierElement / DroidekaElement types,
// made with the game's allocator and constructors). The game's own soldier is hidden while the
// ring is up (the ring's own front soldier takes its place) and shown again when the ring goes.
// Turning: the ring's position (in classes) eases toward the selected class, so the selected one
// turns to the front instead of jumping.
struct Ring
{
    uintptr_t sd = 0;
    uintptr_t team = 0;            // the team the elements were made for
    int count = 0;                 // classes on the ring
    uintptr_t elem[kRingMax] = {};
    uintptr_t cls[kRingMax] = {};
    bool droideka[kRingMax] = {};
    int layer[kRingMax] = {};
    float box[kRingMax][4] = {};   // 18q: each soldier's box (x0, y0, x1, y1) in interface units
    float front[kRingMax] = {};    // 18q: 1 = front .. 0 = back
    int placed = 0;                // 18q: classes placed this frame (boxes good)
    float pickX = 0.0f;            // 21j-r19: where the picked soldier stands at rest (x, interface units): left / right of it
    float span[4] = {};            // 21j-r19: all the soldiers' boxes together (x0, y0, x1, y1) this frame
    bool spanOk = false;
    float pos = 0.0f;              // where the ring stands, in classes (the selected one at rest)
    bool posSet = false;
    LARGE_INTEGER last = {};
    bool hidGame = false;          // the game's own soldier is hidden by the ring
    bool baseLogged = false;
    bool skipLogged = false;
    bool hzLogged = false;         // 20g-r12: the horizontal ring's place logged
    int lines = 0;
};
Ring g_ring;
// 18k: defaults measured from the Classic Collection reference video
float g_ringWidth = 0.30f;  // of the game's soldier's x (about half the screen); 18q: wider (was 0.23)
float g_ringBack = 0.70f;   // size of the soldier furthest back (1 = as big as the front one)
float g_ringLift = 0.12f;   // how much higher (of the game's soldier's y) the one furthest back stands
float g_ringShift = -0.31f; // the ring's centre, left (-) / right (+), of the game's soldier's x (21j-r9: [ClassWheel] RingShift,
                            // -0.13 before - its right-hand soldiers stood under the class window's left edge)
constexpr float kRingShiftWas = -0.13f; // 21j-r9: where "SELECT SOLDIER" (centred) stood with it
float g_ringSize = 0.78f;   // the front soldier's size (1 = the game's own soldier's size)
float g_ringRaise = 0.11f;  // how much higher (of the game's soldier's y) the front soldier stands
constexpr float kTurnRate = 10.0f; // per second: about 90% of a turn in 0.23 s
// 19g: split screen (player 1's half), the Xbox's ring over the window: its front soldier's feet at
// about 47% of the height, centred at about 62% across (in the soldiers' units his half is the game's
// soldier's x wide, about 451 units per half with 0.07 of the half to the left of x 0), a soldier
// about 31% of the height tall (the 1-player ring's was 51% in his half)
// 19h: the Xbox's arrangement (its shots, the ring at rest): the picked soldier front right, the others
// smaller behind it, all feet level - x = centre + R sin(a + 32 deg), size = the picked one's x
// XbRingRel(nearness). 19g's video: x 311 (0.608 of the soldier's x) drew at 59% across with 451
// units per half, y 276 (raise 0.47) put the feet at 44% down, size 0.48 stood 29% of the height tall.
// The Xbox's: centre 54.5%, R 18% of the half, the picked one 31% tall with its feet at 46.6%.
constexpr float kSplitRingX = 0.568f;     // the ring's centre, of the game's soldier's x (19g 0.608)
constexpr float kSplitRingWidth = 0.317f; // R, of half the game's soldier's x (19g 0.35)
constexpr float kSplitRingSize = 0.508f;  // the picked soldier (19g 0.48 at the front)
constexpr float kSplitRingRaise = 0.44f;  // of the game's soldier's y (19g 0.47)
constexpr float kSplitRingLift = 0.0f;    // feet level (19g 0.03)
constexpr float kSplitRingDepth = 0.5f;   // depth for the depth buffer only (g_ringDepth's 2.0 moved the back ones right)
constexpr float kXbRingPhase = 0.5585f;   // 32 degrees: the picked one stands right of the centre
// 19h: a soldier's size on the Xbox's ring, of the picked one's: nearness 1 (front) = 1, 0 (back) = 0.366
// (fitted to its shots: the next ones 61-65%, the back ones 40%)
float XbRingRel(float nearness)
{
    const float m2 = nearness * nearness;
    const float m8 = m2 * m2 * m2 * m2;
    return 0.366f + 0.36f * nearness + 0.274f * m8;
}
// 20g-r12: the Classic Collection's horizontal ring (its video, the ring at rest with 5 classes: the picked one in front
// 0.515 of the half tall, its feet at 0.826; the two beside it 0.73 of its size, feet 0.749; the two behind 0.67, feet
// 0.719 - centred at 0.384 across, radius 0.074 of the half's width, the picked one in the middle): a soldier's size of
// the picked one's (n^4), and how far down from the ring's lift it stands (n^3: 1 front .. 0 back), by its nearness (1
// front .. 0 back)
constexpr float kClassicRingBack = 0.67f;
float ClassicRingRise(float nearness)
{
    return nearness * nearness * nearness;
}
float ClassicRingRel(float nearness)
{
    const float n2 = nearness * nearness;
    return kClassicRingBack + (1.0f - kClassicRingBack) * n2 * n2; // (n^4: 0.73 beside, 0.67 behind)
}
bool g_keyNarrow = false;
bool g_keyWide = false;
bool g_keySmaller = false;
bool g_keyBigger = false;
bool g_keyLeftShift = false;
bool g_keyRightShift = false;
bool g_keySizeDown = false;
bool g_keySizeUp = false;
bool g_keyRaiseUp = false;
bool g_keyRaiseDown = false;

bool RingBudget()
{
    return g_ring.lines++ < 150;
}

float ReadF32(uintptr_t va)
{
    float f = 0.0f;
    if (!ReadMem(va, &f, sizeof(f)))
        return 0.0f;
    return f;
}

bool InExeCodeVa(uintptr_t va); // defined below
uintptr_t FindModuleBytes(const void* needle, size_t n); // defined below (18p)
uintptr_t LiveLookup(const char* path); // defined below (18n)
void SoldierBox(float x, float y, float size80, bool droideka, float* x0, float* y0, float* x1, float* y1); // below (18q)
bool MouseIn(float x0, float y0, float x1, float y1); // defined below (18q)
bool ClickLines(); // 21j-r19: defined below
bool PlayUiSound(UiSound& s); // defined below (18p / 18q; 19i: true if played)

// 18q: the cursor this frame (read by ReadMouse, below)
struct Mouse
{
    bool ok = false;       // a position was read this frame
    bool game = false;     // from the game's own cursor (else the Windows cursor)
    float x = 0.0f;        // interface units
    float y = 0.0f;
    float w = 0.0f;        // the screen's width in those units (600 * aspect)
    float px = 0.0f;       // pixels, for the log
    float py = 0.0f;
    int dw = 0;
    int dh = 0;
};
Mouse g_mouse;

uintptr_t VirtualAt(uintptr_t obj, uint32_t slot)
{
    const uintptr_t vt = ReadU32(obj);
    const uintptr_t fn = vt ? ReadU32(vt + slot) : 0;
    return InExeCodeVa(fn) ? fn : 0;
}

// the game's own element of one type: where it stands and how it is set up (read only)
struct ElemBase
{
    float x = 0.0f;
    float y = 0.0f;
    float s = 0.0f;   // +84h: screen position -> 3D position
    float f80 = 0.0f; // +80h: size
    float matrix[16] = {};
    uint8_t rotOfs[0x1C] = {};
    float view[3] = {};
};

bool ReadElemBase(uintptr_t e, const char* type, ElemBase* b)
{
    char t[64];
    if (!TypeName(e, t, sizeof(t)) || strcmp(t, type) != 0)
        return false;
    b->x = ReadF32(e + kElemX);
    b->y = ReadF32(e + kElemY);
    b->s = ReadF32(e + kElemScale);
    b->f80 = ReadF32(e + kElemF80);
    ReadMem(e + kElemMatrix, b->matrix, sizeof(b->matrix));
    ReadMem(e + kElemRotOfs, b->rotOfs, sizeof(b->rotOfs));
    ReadMem(e + kElemView, b->view, sizeof(b->view));
    return b->s > 0.0f && b->f80 > 0.0f;
}

void ElemDelete(uintptr_t e)
{
    const uintptr_t del = e ? VirtualAt(e, 0) : 0;
    if (del)
        reinterpret_cast<ElemDelete_t>(del)(reinterpret_cast<void*>(e), nullptr, 1);
}

// what ShowClass gives SetWeapon for a class: class vfunc 28h, if the class passes the same type
// check (vfunc 0Ch with ShowClass's type id); 0 = no weapon. Without the type getter: vfunc 28h.
uintptr_t ClassWeapon(uintptr_t cls)
{
    const uintptr_t get = VirtualAt(cls, 0x28);
    if (!get)
        return 0;
    if (g_weaponType)
    {
        const uintptr_t isa = VirtualAt(cls, 0x0C);
        if (!isa || !reinterpret_cast<IsA_t>(isa)(reinterpret_cast<void*>(cls), nullptr, g_weaponType()))
            return 0;
    }
    return reinterpret_cast<ClassWeapon_t>(get)(reinterpret_cast<void*>(cls), nullptr);
}

// delete the ring; sdAlive = the spawn screen (still whole) to show the game's own soldier again,
// or 0 when the spawn screen itself is going
void RingDeleteAll(const char* why, uintptr_t sdAlive)
{
    int n = 0;
    for (int i = 0; i < kRingMax; ++i)
    {
        if (g_ring.elem[i])
        {
            ElemDelete(g_ring.elem[i]);
            ++n;
        }
        g_ring.elem[i] = 0;
        g_ring.cls[i] = 0;
        g_ring.droideka[i] = false;
        g_ring.layer[i] = -1;
    }
    g_ring.team = 0;
    g_ring.count = 0;
    g_ring.posSet = false;
    if (g_ring.hidGame && sdAlive)
    {
        const uintptr_t cur = ReadU32(sdAlive + kSdCurrentElem);
        if (cur == sdAlive + kSdSoldierElem || cur == sdAlive + kSdDroidekaElem)
            g_rf.setVisible(reinterpret_cast<void*>(cur), nullptr, 1);
    }
    g_ring.hidGame = false;
    if (n && RingBudget())
        Log("[wheel] %lu ms: ring: %d element(s) deleted (%s)", ScanMsSinceAttach(), n, why);
}

void RingClear(const char* why, uintptr_t sd)
{
    if (g_ring.sd == sd)
        RingDeleteAll(why, sd);
}

uintptr_t RingNew(bool droideka)
{
    if (droideka && !g_rdOk)
        return 0;
    void* mem = g_rf.gameNew(droideka ? kDroidekaSize : kElemSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(droideka ? g_rdCtor(mem, nullptr) : g_rf.ctor(mem, nullptr));
    const char* const want = droideka ? "DroidekaElement" : "SoldierElement";
    char t[64];
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, want) != 0)
    {
        if (RingBudget())
            Log("[wheel] ring: the new element is a \"%s\", not a %s - ring off", e ? t : "(null)", want);
        g_ringOk = false;
        return 0;
    }
    return e;
}

// 20g-r12: horizontal 2-player - player p's (0 / 1) Classic ring in `group`: its centre at the picked soldier's feet, its
// radius, the back ones' lift (the group's units) and the picked soldier's size (of the game's soldier's), from his HUD
// root (split_spawn_menus.cpp). False if the group is not in a HUD root.
bool HzRingPlace(int p, uintptr_t group, float* cx, float* cy, float* radius, float* lift, float* size)
{
    float w = 0.0f, h = 0.0f, ox = 0.0f, oy = 0.0f, kx = 0.0f, ky = 0.0f;
    if (!P2SpawnRootGeom(p, &w, &h, &ox, &oy, &kx, &ky) || !P2SpawnGroupPlace(group, kHzRingX, kHzRingFeetY, cx, cy))
        return false;
    *radius = kHzRingRadius * w;
    *lift = kHzRingLift * h;
    *size = kHzRingTall * h / kHzSoldierUnits;
    return true;
}

void RingUpdate(uintptr_t sd)
{
    if (!g_ringOk)
        return;
    if (g_ring.sd != sd)
    {
        // (the delete hook clears the ring before a spawn screen goes; this is only a fallback)
        const int lines = g_ring.lines;
        g_ring = Ring();
        g_ring.lines = lines;
        g_ring.sd = sd;
    }
    ElemBase sb;
    ElemBase db;
    if (!ReadElemBase(sd + kSdSoldierElem, "SoldierElement", &sb))
    {
        if (!g_ring.baseLogged && RingBudget())
            Log("[wheel] ring: the spawn screen's soldier at +2090h is not a set-up SoldierElement - no ring");
        g_ring.baseLogged = true;
        return;
    }
    const bool haveDroid = g_rdOk && ReadElemBase(sd + kSdDroidekaElem, "DroidekaElement", &db);
    if (!g_ring.baseLogged && RingBudget())
    {
        g_ring.baseLogged = true;
        Log("[wheel] ring: the game's soldier: x %.1f y %.1f depth %.3f, +84h %.5f, +80h (size) %.3f, view %.1f %.1f "
            "%.1f",
            sb.x, sb.y, sb.matrix[14], sb.s, sb.f80, sb.view[0], sb.view[1], sb.view[2]);
        if (haveDroid)
            Log("[wheel] ring: the game's droideka: x %.1f y %.1f depth %.3f, +84h %.5f, +80h (size) %.3f", db.x, db.y,
                db.matrix[14], db.s, db.f80);
        else
            Log("[wheel] ring: no droideka element to copy (droidekas stay off the ring)");
    }

    // tuning keys (while on the soldier select)
    const bool narrow = (GetAsyncKeyState(VK_OEM_4) & 0x8000) != 0; // [
    const bool wide = (GetAsyncKeyState(VK_OEM_6) & 0x8000) != 0;   // ]
    const bool smaller = (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) != 0;
    const bool bigger = (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) != 0;
    const bool toLeft = (GetAsyncKeyState(VK_OEM_COMMA) & 0x8000) != 0;
    const bool toRight = (GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000) != 0;
    bool tuned = false;
    if (narrow && !g_keyNarrow && g_ringWidth > 0.05f)
        g_ringWidth -= 0.02f, tuned = true;
    if (wide && !g_keyWide && g_ringWidth < 1.0f)
        g_ringWidth += 0.02f, tuned = true;
    if (smaller && !g_keySmaller && g_ringBack > 0.15f)
        g_ringBack -= 0.05f, tuned = true;
    if (bigger && !g_keyBigger && g_ringBack < 1.0f)
        g_ringBack += 0.05f, tuned = true;
    if (toLeft && !g_keyLeftShift && g_ringShift > -0.8f)
        g_ringShift -= 0.02f, tuned = true;
    if (toRight && !g_keyRightShift && g_ringShift < 0.8f)
        g_ringShift += 0.02f, tuned = true;
    g_keyNarrow = narrow;
    g_keyWide = wide;
    g_keySmaller = smaller;
    g_keyBigger = bigger;
    g_keyLeftShift = toLeft;
    g_keyRightShift = toRight;
    const bool depthLess = (GetAsyncKeyState('9') & 0x8000) != 0; // 18p
    const bool depthMore = (GetAsyncKeyState('0') & 0x8000) != 0;
    if (depthLess && !g_keyDepthLess && g_ringDepth > -6.0f)
        g_ringDepth -= 0.25f, tuned = true;
    if (depthMore && !g_keyDepthMore && g_ringDepth < 6.0f)
        g_ringDepth += 0.25f, tuned = true;
    g_keyDepthLess = depthLess;
    g_keyDepthMore = depthMore;
    const bool sizeDown = (GetAsyncKeyState(VK_OEM_1) & 0x8000) != 0; // ;
    const bool sizeUp = (GetAsyncKeyState(VK_OEM_7) & 0x8000) != 0;   // '
    const bool raiseUp = (GetAsyncKeyState(VK_PRIOR) & 0x8000) != 0;  // PageUp
    const bool raiseDown = (GetAsyncKeyState(VK_NEXT) & 0x8000) != 0; // PageDown
    if (sizeDown && !g_keySizeDown && g_ringSize > 0.2f)
        g_ringSize -= 0.02f, tuned = true;
    if (sizeUp && !g_keySizeUp && g_ringSize < 1.5f)
        g_ringSize += 0.02f, tuned = true;
    if (raiseUp && !g_keyRaiseUp && g_ringRaise < 0.6f)
        g_ringRaise += 0.01f, tuned = true;
    if (raiseDown && !g_keyRaiseDown && g_ringRaise > -0.3f)
        g_ringRaise -= 0.01f, tuned = true;
    g_keySizeDown = sizeDown;
    g_keySizeUp = sizeUp;
    g_keyRaiseUp = raiseUp;
    g_keyRaiseDown = raiseDown;
    if (tuned && RingBudget())
        Log("[wheel] ring tuning: width %.2f, back size %.2f, centre %+.2f, back lift %.2f, front size %.2f, height "
            "%+.3f, depth %+.2f",
            g_ringWidth, g_ringBack, g_ringShift, g_ringLift, g_ringSize, g_ringRaise, g_ringDepth);

    // the team's classes (the PC cycle) and the selected one
    const uintptr_t sm = SpawnScanManager();
    const uint32_t teamNo = ReadU32(sd + kTailStart + 4 * kIdxTeamShown);
    const uintptr_t team = (sm && teamNo < static_cast<uint32_t>(kMaxTeams)) ? ReadU32(sm + kSmTeams + 4 * teamNo) : 0;
    const uint32_t count = team ? ReadU32(team + kTeamCount) : 0;
    const uint32_t extra = team ? ReadU32(team + kTeamCount + 4) : 0;
    const uint32_t arr = team ? ReadU32(team + kTeamClasses) : 0;
    int n = (arr && count <= kMaxClasses && extra <= count) ? static_cast<int>(count - extra) : 0;
    if (n > kRingMax)
        n = kRingMax;
    const uint32_t selRaw = ReadU32(sd + kTailStart + 4 * kIdxClassIndex);
    if (n <= 0 || selRaw >= static_cast<uint32_t>(n))
        return;
    const int sel = static_cast<int>(selRaw);
    if (team != g_ring.team || n != g_ring.count)
    {
        RingDeleteAll("the team shown changed", sd);
        g_ring.team = team;
        g_ring.count = n;
    }

    // one element per class (made once, kept while the ring is up)
    for (int i = 0; i < n; ++i)
    {
        if (g_ring.elem[i])
            continue;
        const uintptr_t cls = ReadU32(arr + 4 * static_cast<uint32_t>(i));
        char ct[64];
        if (!cls || !TypeName(cls, ct, sizeof(ct)))
            continue;
        const bool isSoldier = strcmp(ct, "EntitySoldierClass") == 0;
        const bool isDroideka = strcmp(ct, "EntityDroidekaClass") == 0 && haveDroid;
        if (!isSoldier && !isDroideka)
        {
            if (!g_ring.skipLogged && RingBudget())
                Log("[wheel] ring: class %d is a %s - not on the ring", i, ct);
            g_ring.skipLogged = true;
            continue;
        }
        const uintptr_t weapon = ClassWeapon(cls); // what ShowClass would give it
        if (isDroideka && !weapon)
        {
            if (!g_ring.skipLogged && RingBudget())
                Log("[wheel] ring: class %d (droideka) has no weapon to show - not on the ring", i);
            g_ring.skipLogged = true;
            continue;
        }
        const uintptr_t e = RingNew(isDroideka);
        if (!e)
            return;
        if (isSoldier)
            g_rf.setView(reinterpret_cast<void*>(e), nullptr, reinterpret_cast<uintptr_t>(sb.view));
        if (!g_rf.addChild(reinterpret_cast<void*>(sd + kSdGroup), nullptr, reinterpret_cast<void*>(e)) && RingBudget())
            Log("[wheel] ring: AddChild refused element 0x%08X", static_cast<unsigned>(e));
        if (isSoldier)
            g_rf.setClass(reinterpret_cast<void*>(e), nullptr, cls);
        else
            g_rdSetClass(reinterpret_cast<void*>(e), nullptr, cls);
        if (weapon)
            g_rf.setWeapon(reinterpret_cast<void*>(e), nullptr, weapon);
        const uintptr_t rebuild = VirtualAt(e, 0x2C);
        const uintptr_t idle = VirtualAt(e, 0x3C);
        if (rebuild)
            reinterpret_cast<ElemNoArg_t>(rebuild)(reinterpret_cast<void*>(e), nullptr);
        if (idle)
            reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, 1);
        g_ring.elem[i] = e;
        g_ring.cls[i] = cls;
        g_ring.droideka[i] = isDroideka;
        g_ring.layer[i] = -1;
        if (RingBudget())
            Log("[wheel] %lu ms: ring: class %d (%s) element made at 0x%08X", ScanMsSinceAttach(), i,
                isDroideka ? "droideka" : "soldier", static_cast<unsigned>(e));
    }

    // turn: the ring eases toward the selected class (shortest way round)
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    float dt = 0.0f;
    if (g_ring.last.QuadPart && freq.QuadPart)
        dt = static_cast<float>(static_cast<double>(now.QuadPart - g_ring.last.QuadPart) / static_cast<double>(freq.QuadPart));
    g_ring.last = now;
    if (dt < 0.0f || dt > 0.1f)
        dt = 0.0f;
    const float nf = static_cast<float>(n);
    if (!g_ring.posSet)
    {
        g_ring.pos = static_cast<float>(sel);
        g_ring.posSet = true;
    }
    else
    {
        float d = static_cast<float>(sel) - g_ring.pos;
        while (d > nf * 0.5f)
            d -= nf;
        while (d <= -nf * 0.5f)
            d += nf;
        const float k = dt * kTurnRate;
        g_ring.pos += d * (k < 1.0f ? k : 1.0f);
        if (d < 0.002f && d > -0.002f)
            g_ring.pos = static_cast<float>(sel);
        while (g_ring.pos < 0.0f)
            g_ring.pos += nf;
        while (g_ring.pos >= nf)
            g_ring.pos -= nf;
    }

    // place: angle from the front, nearer = bigger, lower and drawn later. +84h stays exactly the
    // game's (it also sets depth); the size is +80h. A droideka element maps screen positions with
    // its own +84h, so it is moved by the same screen offset from where the game puts its droideka.
    const float kPi = 3.14159265f;
    const bool split = LocalPlayersNow() >= 2;
    const float wx = split ? 0.5f * sb.x : sb.x; // 18x: half the width in split screen
    // 19a: in split screen the ring stood at about a quarter across his half (18z's video): further right
    // 19g: split screen: the Xbox's ring, centred over the window, smaller and lower (kSplitRing*)
    // 19h: cx is the ring's centre; the picked soldier stands kXbRingPhase round from the front
    const float cx = split ? kSplitRingX * sb.x : wx + g_ringShift * wx;
    const float ringWidth = split ? kSplitRingWidth * (QuarterLook() ? kQuarterRingWidth : 1.0f) : g_ringWidth; // 20g-r7
    const float ringSize = split ? (QuarterLook() ? kQuarterRingSize : kSplitRingSize) : g_ringSize;
    const float ringRaise = split ? kSplitRingRaise : g_ringRaise;
    const float ringLift = split ? kSplitRingLift : g_ringLift;
    // 20g-r12: horizontal 2-player - the Classic Collection's ring (its places through his HUD root; the soldiers' group is
    // anchored at the root's top left, no safe area)
    float hzCx = 0.0f, hzCy = 0.0f, hzR = 0.0f, hzLift = 0.0f, hzSize = 0.0f;
    const bool hz = split && ClassWheelHorizontal() && HzRingPlace(0, sd + kSdGroup, &hzCx, &hzCy, &hzR, &hzLift, &hzSize);
    if (hz && !g_ring.hzLogged && RingBudget())
    {
        g_ring.hzLogged = true;
        Log("[wheel] %lu ms: ring: horizontal (Classic's) - centre %.0f, picked soldier's feet %.0f (%.3f / %.3f of his half), radius "
            "%.0f, back ones %.0f higher, picked size %.3f (%.3f of his half tall); the soldiers' group 0x%08X",
            ScanMsSinceAttach(), hzCx, hzCy, kHzRingX, kHzRingFeetY, hzR, hzLift, hzSize, kHzRingTall, static_cast<unsigned>(sd + kSdGroup));
    }
    g_ring.placed = n;
    // 21j-r19: where the picked soldier stands at rest (the front; split screen's ring is turned kXbRingPhase) - a click left of
    // it turns the wheel left, right of it right
    g_ring.pickX = hz ? hzCx : cx + ringWidth * wx * std::sin(split ? kXbRingPhase : 0.0f);
    g_ring.spanOk = false;
    for (int i = 0; i < n; ++i)
    {
        const uintptr_t e = g_ring.elem[i];
        g_ring.front[i] = -1.0f; // 18q: no box unless placed below
        if (!e)
            continue;
        float a = 2.0f * kPi * (static_cast<float>(i) - g_ring.pos) / nf;
        while (a > kPi)
            a -= 2.0f * kPi;
        while (a <= -kPi)
            a += 2.0f * kPi;
        const float nearness = (std::cos(a) + 1.0f) * 0.5f; // 1 front .. 0 back
        // 19h: split screen: the Xbox's arrangement (turned 32 degrees, its sizes, feet level)
        const float size = hz ? hzSize * ClassicRingRel(nearness) // 20g-r12
                              : (split ? ringSize * XbRingRel(nearness) : ringSize * (g_ringBack + (1.0f - g_ringBack) * nearness));
        const float x = hz ? hzCx + hzR * std::sin(a) : cx + ringWidth * wx * std::sin(split ? a + kXbRingPhase : a);
        const float y = hz ? hzCy - hzLift * (1.0f - ClassicRingRise(nearness))
                           : sb.y - ringRaise * sb.y - ringLift * sb.y * (1.0f - nearness);
        const float ringDepth = split ? kSplitRingDepth : g_ringDepth;
        const ElemBase& b = g_ring.droideka[i] ? db : sb;
        const float k = sb.s / b.s;
        float* const m = reinterpret_cast<float*>(e + kElemMatrix);
        for (int j = 0; j < 12; ++j)
            m[j] = b.matrix[j];
        // 18p: a real depth - the back of the ring further from the camera (the soldier draw scales
        // the matrix's z like x and y), so the depth buffer sorts the soldiers and their weapons
        m[14] = b.matrix[14] - (1.0f - nearness) * ringDepth * (hz ? hzR : ringWidth * wx) * k;
        m[15] = b.matrix[15];
        memcpy(reinterpret_cast<void*>(e + kElemRotOfs), b.rotOfs, sizeof(b.rotOfs));
        g_rf.setScale(reinterpret_cast<void*>(e), nullptr, b.s);
        g_rf.setPos(reinterpret_cast<void*>(e), nullptr, b.x + (x - sb.x) * k, b.y + (y - sb.y) * k);
        const uintptr_t set80 = VirtualAt(e, 0x38);
        const float size80 = b.f80 * size * (g_ring.droideka[i] ? g_dekaSize : 1.0f);
        if (set80)
            reinterpret_cast<ElemSetFloat_t>(set80)(reinterpret_cast<void*>(e), nullptr, size80);
        // 18q: where it stands on the screen, for the mouse (the soldier's x, y units are the
        // interface's: the ring's x, y here, not the droideka's own re-scaled position)
        SoldierBox(x, y, size80, g_ring.droideka[i], &g_ring.box[i][0], &g_ring.box[i][1], &g_ring.box[i][2],
                   &g_ring.box[i][3]);
        g_ring.front[i] = nearness;
        // 18n: a group draws from the highest layer value down - the front gets the lowest (drawn last)
        const int layer = kZFront + static_cast<int>((1.0f - nearness) * 30.0f);
        if (layer != g_ring.layer[i])
        {
            g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, layer);
            g_ring.layer[i] = layer;
        }
        g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
        // 18j: what the spawn screen's update does for its shown soldier every frame: vfunc 3Ch(1)
        // (idle animations), then vfunc 30h(frame time) (a droideka's animation; nothing for a soldier).
        // 18l-r2: only for the selected class - the others stop where they are
        // 18n: the classes that are not selected get vfunc 3Ch(0) and their frame time: a droideka's
        // 3Ch(0) is its roll-up (its own animation back into the ball); a soldier just stops
        // 18p: the others get 3Ch(0) too (it resets the idle timer, so a class plays an animation the
        // moment it is picked - what the game does with its own soldier when the class changes)
        {
            const uintptr_t idle = VirtualAt(e, 0x3C);
            const uintptr_t tick = VirtualAt(e, 0x30);
            if (idle)
                reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, i == sel ? 1 : 0);
            if (tick && (i == sel || g_ring.droideka[i]))
                reinterpret_cast<ElemSetFloat_t>(tick)(reinterpret_cast<void*>(e), nullptr, dt);
        }
    }

    // 21j-r19: the soldiers' boxes together (the wheel's area for the mouse)
    for (int i = 0; i < n && i < kRingMax; ++i)
    {
        if (!g_ring.elem[i] || g_ring.front[i] < 0.0f)
            continue;
        if (!g_ring.spanOk)
        {
            memcpy(g_ring.span, g_ring.box[i], sizeof(g_ring.span));
            g_ring.spanOk = true;
            continue;
        }
        for (int k = 0; k < 2; ++k)
            if (g_ring.box[i][k] < g_ring.span[k])
                g_ring.span[k] = g_ring.box[i][k];
        for (int k = 2; k < 4; ++k)
            if (g_ring.box[i][k] > g_ring.span[k])
                g_ring.span[k] = g_ring.box[i][k];
    }

    // the game's own soldier (and droideka) stay hidden while the ring shows the selected class
    if (g_ring.elem[sel])
    {
        g_rf.setVisible(reinterpret_cast<void*>(sd + kSdSoldierElem), nullptr, 0);
        if (haveDroid)
            g_rf.setVisible(reinterpret_cast<void*>(sd + kSdDroidekaElem), nullptr, 0);
        g_ring.hidGame = true;
    }
}

// 18q: the class of the ring under the cursor (the nearest to the front where boxes overlap), -1 = none
int RingHit(uintptr_t sd)
{
    if (g_ring.sd != sd || !g_ring.placed)
        return -1;
    int best = -1;
    for (int i = 0; i < g_ring.placed && i < kRingMax; ++i)
    {
        if (!g_ring.elem[i] || g_ring.front[i] < 0.0f)
            continue;
        if (!MouseIn(g_ring.box[i][0], g_ring.box[i][1], g_ring.box[i][2], g_ring.box[i][3]))
            continue;
        if (best < 0 || g_ring.front[i] > g_ring.front[best])
            best = i;
    }
    return best;
}

// 18q: the ring has finished turning (the picked class is at the front)
bool RingAtRest()
{
    if (!g_ring.posSet)
        return false;
    const float d = g_ring.pos - std::floor(g_ring.pos + 0.5f);
    return d < 0.01f && d > -0.01f;
}

// ---- 18l: the side screen ("CHOOSE YOUR SIDE"), game thread ----------------
// The Classic Collection's side screen: a title, each side's first class standing side by side
// (the game's own soldier/droideka elements, made like the ring's), the side's name under each,
// the picked side lit and its name white, the other dark (its light turned round) and its name
// grey. Left / right pick, A joins the picked side with the spawn screen's own SelectTeam (what
// the PC screen's team tabs call). Texts are the game's own RedTextElement (the type the spawn
// screen's own texts are), made with its constructor, font, string and justify functions, in the
// same draw group as the soldiers; everything is deleted when the side screen goes (or before
// the spawn screen itself is deleted).
// v6.5 (Kade, Oct 25 2026): up to 7 sides - Mashup Conquest (dinput8_BF1Modes.dll) offers every team of the match
// (REP / CIS / ALL / IMP and the map's locals); any other match the two stock ones, laid out exactly as before
constexpr int kMaxSides = 7;
struct Side
{
    uintptr_t sd = 0;
    uintptr_t model[kMaxSides] = {};
    bool droideka[kMaxSides] = {};
    uintptr_t title = 0;      // our texts, in the spawn screen's text group (+300h)
    uintptr_t label[kMaxSides] = {};
    wchar_t name[kMaxSides][48] = {};
    int team[kMaxSides] = {}; // v6.5: the team each side joins (1, 2 stock; Mashup: 1 - 7)
    int count = 2;            // v6.5: how many sides (2 stock)
    int pick = 0;          // the index of the picked side (stock: 0 = team 1, 1 = team 2)
    bool made = false;
    bool hidGame = false;  // the game's own soldier is hidden by the side screen
    bool failLogged = false;
    bool groupShown = false;  // the text group was shown by the side screen (it was hidden)
    bool look = false;        // 20g-r10: 4-player - drawn by split_spawn_menus.cpp in players 3 / 4's look (none of ours made)
    bool hz = false;          // 20g-r12: made in the horizontal layout (made again if that changes)
    DWORD lookAt = 0;         //            its last frame (ClassWheelP1Look)
    int nOutLogged = 0;       // v6.9: the out sides last logged
    float pos = 0.0f;         // v7.1: the carousel's turn (side index at the front, eased toward the picked one)
    bool posSet = false;
    LARGE_INTEGER last = {};  // v7.1: its last frame (the turn's time step)
    int total = 2;            // v7.2: the match's sides (more than two: a carousel, the sides out of the match left off it)
    int keepTeam = 0;         // v7.2: the side picked when the carousel was made again (a side went out), kept picked
    bool car = false;         // v7.4: made as a carousel (made again when [ClassWheel] SideCarousel / the MODS row changes)
};
Side g_side;

// v6.5: the sides of this match: BF1Modes.dll's BF1Modes_SideTeams (Mashup Conquest: every playable team, 2 - 7), else
// teams 1 and 2. A Mashup side screen is the 1-player one only (split screen keeps its two-side layouts).
typedef int(__cdecl* SideTeams_t)(int* teams, int max);
int SideTeamsNow(int* teams)
{
    static SideTeams_t fn = nullptr;
    static bool looked = false;
    if (!looked)
    {
        looked = true;
        const HMODULE m = GetModuleHandleA("dinput8_BF1Modes.dll");
        if (m)
            fn = reinterpret_cast<SideTeams_t>(GetProcAddress(m, "BF1Modes_SideTeams"));
        Log("[wheel] side screen: dinput8_BF1Modes.dll %s", fn ? "found - Mashup Conquest can offer up to 7 sides" :
                                                              (m ? "loaded but has no BF1Modes_SideTeams" : "not loaded - two sides"));
    }
    if (fn)
    {
        const int n = fn(teams, kMaxSides);
        if (n >= 3 && n <= kMaxSides)
            return n;
    }
    teams[0] = 1;
    teams[1] = 2;
    return 2;
}
// v6.9: a side that is out of this match (BF1Modes.dll's BF1Modes_SideOut: Mashup Conquest, its reinforcements gone) -
// greyed out on the side screen and not pickable. No BF1Modes (or an older one): no side is out.
typedef int(__cdecl* SideOut_t)(int team);
bool SideOutNow(int team)
{
    static SideOut_t fn = nullptr;
    static bool looked = false;
    if (!looked)
    {
        const HMODULE m = GetModuleHandleA("dinput8_BF1Modes.dll");
        if (m)
        {
            looked = true;
            fn = reinterpret_cast<SideOut_t>(GetProcAddress(m, "BF1Modes_SideOut"));
            Log("[wheel] side screen: dinput8_BF1Modes.dll %s", fn ? "has BF1Modes_SideOut - defeated sides greyed out" :
                                                                   "has no BF1Modes_SideOut - no side is greyed out");
        }
    }
    return fn && fn(team) != 0;
}
// v6.9: the next side not out from 'from' going 'dir' (+1 / -1), else -1
int SideNextIn(const bool* out, int n, int from, int dir)
{
    for (int i = from + dir; i >= 0 && i < n; i += dir)
        if (!out[i])
            return i;
    return -1;
}
// v7.4: the carousel is used (split screen always; 1 player as [ClassWheel] SideCarousel / the MODS page say)
bool SideCarouselOn()
{
    return g_sideCarousel || LocalPlayersNow() >= 2;
}
// v7.3: this match's sides for player 1's side screen: every side (Mashup: up to 7; more than two - the ones out of the
// match left off, Ben's v7.2), count / total / team set, the pick put on `want` (else the first)
void SideListMake(uint32_t want)
{
    int teams[kMaxSides] = {};
    const int n = SideTeamsNow(teams);
    int k = 0;
    for (int i = 0; i < n; ++i)
        if (n <= 2 || !SideCarouselOn() || !SideOutNow(teams[i])) // (v7.4: rows keep the out sides, greyed)
            g_side.team[k++] = teams[i];
    if (k == 0) // (every side out - the round is over: all of them, greyed)
        for (int i = 0; i < n; ++i)
            g_side.team[k++] = teams[i];
    g_side.count = k;
    g_side.total = n;
    g_side.car = n > 2 && SideCarouselOn(); // v7.4
    g_side.pick = 0;
    for (int i = 0; i < g_side.count; ++i)
        if (static_cast<uint32_t>(g_side.team[i]) == want)
            g_side.pick = i;
}
// v7.1: the carousel goes round - the next side not out from 'from' going 'dir', wrapping, else -1
int SideNextWrap(const bool* out, int n, int from, int dir)
{
    for (int k = 1; k < n; ++k)
    {
        const int j = ((from + dir * k) % n + n) % n;
        if (!out[j])
            return j;
    }
    return -1;
}
// v7.1: CHOOSE YOUR SIDE with more than two sides is a carousel (as the class wheel): the picked side in front, the others
// round it, smaller, darker and higher the further back; only the picked side's name shows
constexpr float kCarBack = 0.55f;   // the size of the side furthest back, of the front one's
constexpr float kCarLift = 0.07f;   // how much higher (of the screen height) the one furthest back stands
constexpr float kCarRadius = 1.5f;  // the ring's radius, of the stock pair's distance from their centre (look.x)
constexpr float kCarRadiusHz = 0.6f; // horizontal: of the distance between Classic's two side soldiers
bool g_sideLookAllowed = false; // 20g-r10: split_spawn_menus.cpp can draw player 1's side screen in players 3 / 4's look
int g_sideLookLines = 0;
float g_sideSize = 0.75f;   // the side soldiers' size (1 = the game's own soldier's size)
float g_sideX = 0.104f;     // how far left / right of their centre each soldier stands, of the width
float g_sideCenter = 0.016f; // the soldiers' centre, right of the screen centre (so they show at 40% / 60%)
float g_sideLabelX = 0.133f; // the names: left / right of the screen centre, of the reference width
float g_sideFeet = 0.843f;  // where their feet stand (the soldier's y, of the screen height)
float g_sideTitleY = 0.14f; // the title, of the screen height
float g_sideLabelY = 0.79f; // the names, of the screen height
float g_sideTextScale = 1.0f;
bool g_keyT1 = false, g_keyT2 = false, g_keyT3 = false, g_keyT4 = false, g_keyT5 = false, g_keyT6 = false, g_keyT7 = false,
     g_keyT8 = false;

// the interface's reference size (the units 2D elements are placed in); 800 x 600 if not found
float RefW()
{
    const float w = g_refWAddr ? ReadF32(g_refWAddr) : 0.0f;
    return (w > 100.0f && w < 10000.0f) ? w : 800.0f;
}
float RefH()
{
    const float h = g_refHAddr ? ReadF32(g_refHAddr) : 0.0f;
    return (h > 100.0f && h < 10000.0f) ? h : 600.0f;
}

// 19g: the side screen's layout: the 1-player one (tunable), or in split screen the Xbox's in player
// 1's half (19f's video: the soldiers 44% of the height tall at size 0.75, centred at 40% / 64%; the
// texts' group is at 54% across - his root's x offset - and 1 reference unit is about 1/512 of the
// half's width, 1/624 of its height; the Xbox has the soldiers at 34% / 70%, 32% tall, their feet at
// 62% - 64%, the names at 35% / 70% and 66% down, the title 11% down, all centred at 52%)
struct SideLook
{
    float size, x, center, feet;           // the soldiers (of the game soldier's size; of the half's width / height)
    float labelX, labelY, titleY, textX;   // the texts (of the reference width / height; textX: all texts moved)
};
SideLook SideLookNow()
{
    if (LocalPlayersNow() >= 2) // 20g-r7: 4-player - players 3 / 4's soldier size
        return { QuarterLook() ? kQuarterSideSize : 0.54f, 0.158f, 0.0195f, 0.677f, 0.1125f, 0.653f, 0.0975f, -0.01f }; // 19h: feet 0.647 -> 0.677 (19g's video: 3% high)
    return { g_sideSize, g_sideX, g_sideCenter, g_sideFeet, g_sideLabelX, g_sideLabelY, g_sideTitleY, 0.0f };
}

float SideStep(int n, const SideLook& look)
{
    const float a = 2.0f * look.x, b = 0.80f / static_cast<float>(n - 1);
    return a < b ? a : b;
}
// v6.5: the place of side i of n: its soldier (of the screen width, from the soldiers' centre) and its name (of the
// reference width, from the screen centre); n = 2 gives the stock places
// v6.7: more than 4 sides - two rows: 4 on the top row, the rest below (each row centred, smaller soldiers)
constexpr float kRowTopFeet = 0.49f, kRowTopLabel = 0.475f;  // of the screen height (v6.8: the names right under the feet)
constexpr float kRowLowFeet = 0.85f, kRowLowLabel = 0.835f;
constexpr float kTwoRowSize = 0.58f;                         // the soldiers' size against the 4-side row
bool SideTwoRows(int n)
{
    return n > 4;
}
// v7.0: one layout per side count - 2: the stock pair; 3 / 4: one row; 5: 3 + 2; 6: 3 + 3; 7: 4 + 3 (the top row's count)
int SideTopCount(int n)
{
    if (!SideTwoRows(n))
        return n;
    return n >= 7 ? 4 : 3;
}
void SideCell(int i, int n, int* col, int* cols, int* row)
{
    if (SideTwoRows(n))
    {
        const int top = SideTopCount(n);
        *row = i < top ? 0 : 1;
        *col = i < top ? i : i - top;
        *cols = i < top ? top : n - top;
        return;
    }
    *row = 0;
    *col = i;
    *cols = n;
}
float SideFeetY(int i, int n, const SideLook& look)
{
    if (!SideTwoRows(n))
        return look.feet;
    return i < SideTopCount(n) ? kRowTopFeet : kRowLowFeet;
}
float SideLabelY(int i, int n, const SideLook& look)
{
    if (!SideTwoRows(n))
        return look.labelY;
    return i < SideTopCount(n) ? kRowTopLabel : kRowLowLabel;
}
float SideOffset(int i, int n, const SideLook& look)
{
    if (n <= 2)
        return i == 0 ? -look.x : look.x;
    int col = 0, cols = n, row = 0;
    SideCell(i, n, &col, &cols, &row);
    const float step = SideStep(SideTopCount(n), look); // v7.0: the top row's spacing for both rows
    return (static_cast<float>(col) - 0.5f * static_cast<float>(cols - 1)) * step;
}
float SideLabelOffset(int i, int n, const SideLook& look)
{
    if (n <= 2)
        return i == 0 ? -look.labelX : look.labelX;
    return SideOffset(i, n, look) * (look.labelX / look.x);
}
float SideSizeK(int n, const SideLook& look)
{
    if (n <= 2)
        return 1.0f;
    if (SideTwoRows(n))
        return SideSizeK(SideTopCount(n), look) * kTwoRowSize; // v7.0: 3 per row (5 / 6 sides) a little bigger than 4
    const float step = SideStep(n, look);
    return (step / (2.0f * look.x) > 0.45f ? step / (2.0f * look.x) : 0.45f);
}
float SideTextK(int n)
{
    // v7.0: by the widest row (5 / 6 sides have rows of 3)
    const int top = SideTopCount(n);
    return top <= 3 && n <= 3 ? 1.0f : (top <= 3 ? 0.8f : (n <= 4 ? 0.8f : 0.65f));
}
bool TextOk()
{
    return g_txCtor && g_txFont && g_txJustify && g_setTextW;
}

// one of our texts: the game's RedTextElement, white, centred on (x, y)
uintptr_t TextNew(uintptr_t group, const char* font, const wchar_t* s, float x, float y, float scale, int z)
{
    void* mem = g_rf.gameNew(kTextSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_txCtor(mem, nullptr));
    char t[64];
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, "RedTextElement") != 0)
    {
        Log("[wheel] side screen: the new text is a \"%s\", not a RedTextElement - side screen off", e ? t : "(null)");
        g_sideOk = false;
        return 0;
    }
    // 18o: font, justify, scale, colour first - the string last (SetText lays the text out with the
    // font, and SetJustify wipes that layout)
    g_txFont(reinterpret_cast<void*>(e), nullptr, font);
    g_txJustify(reinterpret_cast<void*>(e), nullptr, 2, 2); // hcenter, vcenter
    *reinterpret_cast<float*>(e + kTextScaleX) = scale;
    *reinterpret_cast<float*>(e + kTextScaleY) = scale;
    uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
    c[0] = c[1] = c[2] = c[3] = 0xFF;
    g_setTextW(reinterpret_cast<void*>(e), nullptr, s);
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y); // the caller's local position
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, z);
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    return e;
}

// 18o: a text made like an existing one (its font object, scale, justify, colour, layer), at (x, y)
uintptr_t TextLike(uintptr_t group, uintptr_t like, const wchar_t* s, float x, float y)
{
    void* mem = g_rf.gameNew(kTextSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_txCtor(mem, nullptr));
    char t[64];
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, "RedTextElement") != 0)
        return 0;
    const uintptr_t font = like ? ReadU32(like + kTextFont) : 0;
    if (font && g_txFontObj)
        g_txFontObj(reinterpret_cast<void*>(e), nullptr, font);
    else
        g_txFont(reinterpret_cast<void*>(e), nullptr, "gamefont_medium");
    const uint32_t flags = like ? ReadU32(like + kTextFlags) : 0;
    g_txJustify(reinterpret_cast<void*>(e), nullptr, like ? static_cast<int>((flags >> 24) & 3) : 0,
                like ? static_cast<int>((flags >> 26) & 3) : 0);
    *reinterpret_cast<float*>(e + kTextScaleX) = like ? ReadF32(like + kTextScaleX) : 1.0f;
    *reinterpret_cast<float*>(e + kTextScaleY) = like ? ReadF32(like + kTextScaleY) : 1.0f;
    *reinterpret_cast<uint32_t*>(e + kElemColor) = like ? ReadU32(like + kElemColor) : 0xFFFFFFFFu;
    g_setTextW(reinterpret_cast<void*>(e), nullptr, s);
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, like ? static_cast<int>(ReadU32(like + 0x14) & 0xFF) : kZText);
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    return e;
}

void TextGrey(uintptr_t e, bool grey)
{
    if (!e)
        return;
    uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
    const uint8_t v = grey ? 0x80 : 0xFF;
    c[0] = c[1] = c[2] = v;
    c[3] = 0xFF;
}

// a class's soldier (or droideka) element, made and set up exactly like the ring's
uintptr_t ClassElementNew(uintptr_t sd, uintptr_t cls, const ElemBase& sb, bool haveDroid, bool* droidekaOut)
{
    char ct[64];
    if (!cls || !TypeName(cls, ct, sizeof(ct)))
        return 0;
    const bool isSoldier = strcmp(ct, "EntitySoldierClass") == 0;
    const bool isDroideka = strcmp(ct, "EntityDroidekaClass") == 0 && haveDroid;
    if (!isSoldier && !isDroideka)
        return 0;
    const uintptr_t weapon = ClassWeapon(cls);
    if (isDroideka && !weapon)
        return 0;
    const uintptr_t e = RingNew(isDroideka);
    if (!e)
        return 0;
    if (isSoldier)
        g_rf.setView(reinterpret_cast<void*>(e), nullptr, reinterpret_cast<uintptr_t>(sb.view));
    g_rf.addChild(reinterpret_cast<void*>(sd + kSdGroup), nullptr, reinterpret_cast<void*>(e));
    if (isSoldier)
        g_rf.setClass(reinterpret_cast<void*>(e), nullptr, cls);
    else
        g_rdSetClass(reinterpret_cast<void*>(e), nullptr, cls);
    if (weapon)
        g_rf.setWeapon(reinterpret_cast<void*>(e), nullptr, weapon);
    const uintptr_t rebuild = VirtualAt(e, 0x2C);
    const uintptr_t idle = VirtualAt(e, 0x3C);
    if (rebuild)
        reinterpret_cast<ElemNoArg_t>(rebuild)(reinterpret_cast<void*>(e), nullptr);
    if (idle)
        reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, 1);
    *droidekaOut = isDroideka;
    return e;
}

// delete the side screen; sdAlive = the spawn screen (still whole) to show its soldier again
void SideClear(const char* why, uintptr_t sdAlive)
{
    int n = 0;
    for (int i = 0; i < kMaxSides; ++i)
    {
        if (g_side.model[i])
        {
            ElemDelete(g_side.model[i]);
            ++n;
        }
        g_side.model[i] = 0;
    }
    if (g_side.title)
    {
        ElemDelete(g_side.title);
        ++n;
    }
    g_side.title = 0;
    for (int t = 0; t < kMaxSides; ++t)
    {
        if (g_side.label[t])
        {
            ElemDelete(g_side.label[t]);
            ++n;
        }
        g_side.label[t] = 0;
    }
    g_side.made = false;
    g_side.posSet = false; // v7.1: the carousel starts at the picked side next time
    g_side.look = false; // 20g-r10
    g_side.hz = false;   // 20g-r12
    if (g_side.groupShown && sdAlive)
        g_rf.setVisible(reinterpret_cast<void*>(sdAlive + kSdTextGroup), nullptr, 0);
    g_side.groupShown = false;
    if (g_side.hidGame && sdAlive)
    {
        const uintptr_t cur = ReadU32(sdAlive + kSdCurrentElem);
        if (cur == sdAlive + kSdSoldierElem || cur == sdAlive + kSdDroidekaElem)
            g_rf.setVisible(reinterpret_cast<void*>(cur), nullptr, 1);
    }
    g_side.hidGame = false;
    if (n && RingBudget())
        Log("[wheel] %lu ms: side screen: %d element(s) deleted (%s)", ScanMsSinceAttach(), n, why);
}

void SideClearFor(const char* why, uintptr_t sd)
{
    if (g_side.sd == sd)
        SideClear(why, sd);
}

// the side screen, every frame while it is up: -1 = side screen off (the old A applies),
// 0 = still choosing, 1 = A joined the picked side
bool MouseOnPrompt(); // v7.2: defined below
int SideUpdate(uintptr_t sd, bool left, bool right, bool a, bool mouseA)
{
    if (!g_sideOk || !g_ringOk || !TextOk() || !g_selectTeam)
        return -1;
    if (g_side.sd != sd)
    {
        g_side = Side();
        g_side.sd = sd;
    }
    ElemBase sb;
    ElemBase db;
    if (!ReadElemBase(sd + kSdSoldierElem, "SoldierElement", &sb))
        return -1;
    const bool haveDroid = g_rdOk && ReadElemBase(sd + kSdDroidekaElem, "DroidekaElement", &db);
    const uintptr_t sm = SpawnScanManager();
    if (!sm)
        return -1;
    const bool split = LocalPlayersNow() >= 2;
    const float W = split ? sb.x : 2.0f * sb.x; // the screen (18x: the half), in the units the game's soldier is placed in
    const float H = sb.y / 0.9f;     // (its feet stand at about 90% of the height)
    const SideLook look = SideLookNow(); // 19g
    // 20g-r10: 4-player (MenusLikeP34=1) - drawn by split_spawn_menus.cpp as players 3 / 4's side screens (their group, root
    // and places); this side screen still picks and joins
    const bool look34 = QuarterLook() && g_sideLookAllowed;
    if (g_side.made && g_side.look != look34)
        SideClear(look34 ? "players 3 / 4's look now" : "players 3 / 4's look not drawn - ours", sd);
    // v7.2 / v7.3: the carousel holds only the sides still in the match - one going out while the screen is up: made again
    // without it (this frame, the picked side kept); players 3 / 4's look too (v7.3)
    // v7.4: the MODS page's SIDE CAROUSEL row changed while it is up - made again the other way
    if (g_side.made && !g_side.look && g_side.total > 2 && g_side.car != SideCarouselOn())
    {
        const int keep = (g_side.pick >= 0 && g_side.pick < g_side.count) ? g_side.team[g_side.pick] : 0;
        SideClear("the side carousel switched on / off", sd);
        g_side.keepTeam = keep;
    }
    if (g_side.made && g_side.total > 2 && SideCarouselOn())
    {
        bool gone = false;
        for (int i = 0; i < g_side.count && !gone; ++i)
            gone = SideOutNow(g_side.team[i]);
        if (gone)
        {
            const int keep = (g_side.pick >= 0 && g_side.pick < g_side.count) ? g_side.team[g_side.pick] : 0;
            SideClear("a side is out of the match - the carousel made again without it", sd);
            g_side.keepTeam = keep;
        }
    }
    if (!g_side.made && look34)
    {
        const uint32_t shown = ReadU32(sd + kTailStart + 4 * kIdxTeamShown);
        // v7.3: every side of the match in 4-player too (split_spawn_menus.cpp draws them - its carousel for more than two)
        SideListMake(g_side.keepTeam ? static_cast<uint32_t>(g_side.keepTeam) : shown);
        g_side.keepTeam = 0;
        g_side.made = true;
        g_side.look = true;
        if (g_sideLookLines++ < 6)
            Log("[wheel] %lu ms: side screen: 4-player - drawn in players 3 / 4's look (split_spawn_menus.cpp's group in his HUD root); "
                "its own soldiers and texts not made", ScanMsSinceAttach());
    }
    // 20g-r12: horizontal 2-player - the Classic Collection's side screen (its places through his HUD root): the soldiers in
    // their group (the root's top left, no safe area), the texts in the text group (+300h) - each found in the root or not
    const bool hz = split && !look34 && ClassWheelHorizontal();
    if (g_side.made && !g_side.look && g_side.hz != hz) // (its texts are placed once: all of it made again in the other layout)
        SideClear(hz ? "the horizontal layout now" : "the horizontal layout off", sd);
    float hzModelX[2] = {}, hzModelY = 0.0f, hzSize = 0.0f;
    bool hzModels = false;
    if (hz)
    {
        float w = 0.0f, h = 0.0f, ox = 0.0f, oy = 0.0f, kx = 0.0f, ky = 0.0f;
        float yOther = 0.0f;
        hzModels = P2SpawnRootGeom(0, &w, &h, &ox, &oy, &kx, &ky) &&
                   P2SpawnGroupPlace(sd + kSdGroup, kHzSideX0, kHzSideFeetY, &hzModelX[0], &hzModelY) &&
                   P2SpawnGroupPlace(sd + kSdGroup, kHzSideX1, kHzSideFeetY, &hzModelX[1], &yOther);
        if (hzModels)
            hzSize = kHzSideTall * h / kHzSoldierUnits;
    }
    if (!g_side.made)
    {
        const uint32_t shown = ReadU32(sd + kTailStart + 4 * kIdxTeamShown);
        // v6.5: the sides (Mashup: up to 7, the 1-player side screen only)
        // v7.1 / v7.2 / v7.3: every side of the match (split screen and 4-player too; more than two - the ones out left off)
        SideListMake(g_side.keepTeam ? static_cast<uint32_t>(g_side.keepTeam) : shown);
        g_side.keepTeam = 0;
        // v6.9: not a side that is out (his own side defeated: the first one still in)
        if (SideOutNow(g_side.team[g_side.pick]))
            for (int i = 0; i < g_side.count; ++i)
                if (!SideOutNow(g_side.team[i]))
                {
                    g_side.pick = i;
                    break;
                }
        if (g_side.count > 2 && RingBudget())
            Log("[wheel] %lu ms: side screen: Mashup - %d sides (teams %d %d %d %d %d %d %d)", ScanMsSinceAttach(), g_side.count,
                g_side.team[0], g_side.team[1], g_side.team[2], g_side.team[3], g_side.team[4], g_side.team[5], g_side.team[6]);
        // 20g-r12: horizontal - the texts' places in the text group (Classic's), else the split screen's
        float hzNameX[2] = {}, hzNameY = 0.0f, hzTitleX = 0.0f, hzTitleY = 0.0f;
        float yOther = 0.0f;
        const bool hzTexts = hz && P2SpawnGroupPlace(sd + kSdTextGroup, kHzNameX0, kHzNameY, &hzNameX[0], &hzNameY) &&
                             P2SpawnGroupPlace(sd + kSdTextGroup, kHzNameX1, kHzNameY, &hzNameX[1], &yOther) &&
                             P2SpawnGroupPlace(sd + kSdTextGroup, kHzTitleX, kHzTitleY, &hzTitleX, &hzTitleY);
        if (hz && HzBudget())
            Log("[wheel] %lu ms: side screen: horizontal (Classic's) - soldiers %s (feet at %.0f / %.0f, %.0f, size %.3f), texts %s "
                "(names at %.0f / %.0f, %.0f, title at %.0f, %.0f, scale %.2f)",
                ScanMsSinceAttach(), hzModels ? "placed" : "NOT in a HUD root - the split screen's places", hzModelX[0], hzModelX[1],
                hzModelY, hzSize, hzTexts ? "placed" : "NOT in a HUD root - the split screen's places", hzNameX[0], hzNameX[1], hzNameY,
                hzTitleX, hzTitleY, kHzTextScale);
        for (int i = 0; i < g_side.count; ++i)
        {
            const uintptr_t team = ReadU32(sm + kSmTeams + 4 * static_cast<uint32_t>(g_side.team[i]));
            const uintptr_t arr = team ? ReadU32(team + kTeamClasses) : 0;
            const uintptr_t cls0 = arr ? ReadU32(arr) : 0;
            g_side.model[i] = ClassElementNew(sd, cls0, sb, haveDroid, &g_side.droideka[i]);
            wchar_t* const name = g_side.name[i];
            if (!team || !ReadWide(ReadU32(team + kTeamName), name, 48))
                swprintf(name, 48, L"SIDE %d", g_side.team[i]);
            // the name under the soldier, in the text group (centre-anchored, reference units, y down)
            // v7.1: a carousel - every name where the front side's goes (only the picked one is shown)
            const bool carName = g_side.car; // v7.2 / v7.4: a carousel (the match's sides; some may be off it)
            const float lx = carName ? (hzTexts ? 0.5f * (hzNameX[0] + hzNameX[1]) : look.textX * RefW())
                                     : (hzTexts ? hzNameX[i] : (look.textX + SideLabelOffset(i, g_side.count, look)) * RefW());
            const float ly = carName ? (hzTexts ? hzNameY : (look.labelY - 0.5f) * RefH())
                                     : (hzTexts ? hzNameY : (SideLabelY(i, g_side.count, look) - 0.5f) * RefH()); // v6.7: its row
            g_side.label[i] = TextNew(sd + kSdTextGroup, "gamefont_large", name, lx, ly,
                                      hzTexts ? kHzTextScale : g_sideTextScale * (carName ? 1.0f : SideTextK(g_side.count)), kZText);
            char a8[48] = {};
            WideCharToMultiByte(CP_UTF8, 0, name, -1, a8, sizeof(a8), nullptr, nullptr);
            if (RingBudget())
                Log("[wheel] %lu ms: side screen: side %d \"%s\" - class 0 0x%08X, model 0x%08X%s, name text 0x%08X at "
                    "%.0f %.0f",
                    ScanMsSinceAttach(), i + 1, a8, static_cast<unsigned>(cls0), static_cast<unsigned>(g_side.model[i]),
                    g_side.droideka[i] ? " (droideka)" : "", static_cast<unsigned>(g_side.label[i]), lx, ly);
        }
        g_side.title = TextNew(sd + kSdTextGroup, "gamefont_large", L"CHOOSE YOUR SIDE", hzTexts ? hzTitleX : look.textX * RefW(),
                               hzTexts ? hzTitleY : (look.titleY - 0.5f) * RefH(), hzTexts ? kHzTextScale : g_sideTextScale, kZText);
        if (RingBudget())
            Log("[wheel] side screen: title text 0x%08X at %.0f %.0f (reference size %.0f x %.0f)%s",
                static_cast<unsigned>(g_side.title), hzTexts ? hzTitleX : look.textX * RefW(),
                hzTexts ? hzTitleY : (look.titleY - 0.5f) * RefH(), RefW(), RefH(),
                hzTexts ? " - horizontal (Classic's layout)" : (split ? " - split screen (the Xbox's layout)" : ""));
        g_side.made = true;
        g_side.hz = hz; // 20g-r12
        // the text group shown while the side screen is up (and hidden again after, if it was hidden)
        const uintptr_t grp = sd + kSdTextGroup;
        if (!(ReadU32(grp + 0x14) & 0x100))
        {
            g_side.groupShown = true;
            g_rf.setVisible(reinterpret_cast<void*>(grp), nullptr, 1);
        }
        if (RingBudget())
            Log("[wheel] side screen: the text group +300h was %s", g_side.groupShown ? "hidden - shown now" : "shown");
        if (!g_sideOk)
        {
            SideClear("a text could not be made", sd);
            return -1;
        }
    }

    // pick: left = side 1, right = side 2; 18q: the side under the cursor (its soldier, or its name)
    // v6.5: more than two sides - left / right step through them
    const int was = g_side.pick;
    const int nSides = g_side.count;
    const bool car = g_side.car && !g_side.look; // v7.1 / v7.2 / v7.4: the carousel (more than two sides, [ClassWheel] SideCarousel)
    // v6.9: the sides out of the match (greyed out, skipped, not joined)
    bool out[kMaxSides] = {};
    int nOut = 0;
    for (int i = 0; i < nSides; ++i)
        if ((out[i] = SideOutNow(g_side.team[i])))
            ++nOut;
    if (nOut != g_side.nOutLogged && RingBudget())
    {
        g_side.nOutLogged = nOut;
        Log("[wheel] %lu ms: side screen: %d side(s) out of the match - greyed out, not pickable", ScanMsSinceAttach(), nOut);
    }
    if (!car && nSides <= 2)
    {
        if (left && !out[0])
            g_side.pick = 0;
        if (right && !out[1])
            g_side.pick = 1;
    }
    else
    {
        if (left) // v7.1: round the carousel
        {
            const int j = car ? SideNextWrap(out, nSides, g_side.pick, -1) : SideNextIn(out, nSides, g_side.pick, -1); // v7.4: rows stop at the ends
            if (j >= 0)
                g_side.pick = j;
        }
        if (right)
        {
            const int j = car ? SideNextWrap(out, nSides, g_side.pick, +1) : SideNextIn(out, nSides, g_side.pick, +1);
            if (j >= 0)
                g_side.pick = j;
        }
    }
    if (g_side.pick >= 0 && g_side.pick < nSides && out[g_side.pick]) // (it went out while the screen was up)
    {
        int j = SideNextIn(out, nSides, g_side.pick, +1);
        if (j < 0)
            j = SideNextIn(out, nSides, g_side.pick, -1);
        if (j >= 0)
            g_side.pick = j;
    }
    const float sizeK = SideSizeK(nSides, look);
    const float nameHalfW = nSides <= 3 ? 90.0f : (SideTopCount(nSides) <= 3 || nSides == 4 ? 64.0f : 48.0f); // v7.0
    // v7.1: the carousel (more than two sides): its turn eased toward the picked side, the shortest way round (as the ring),
    // then each side's place, size and nearness (1 front .. 0 back)
    float carX[kMaxSides] = {}, carY[kMaxSides] = {}, carSize[kMaxSides] = {}, carNear[kMaxSides] = {};
    float carR = 0.0f;
    if (car)
    {
        static LARGE_INTEGER freq = {};
        if (!freq.QuadPart)
            QueryPerformanceFrequency(&freq);
        LARGE_INTEGER now = {};
        QueryPerformanceCounter(&now);
        float dt = 0.0f;
        if (g_side.last.QuadPart && freq.QuadPart)
            dt = static_cast<float>(static_cast<double>(now.QuadPart - g_side.last.QuadPart) / static_cast<double>(freq.QuadPart));
        g_side.last = now;
        if (dt < 0.0f || dt > 0.1f)
            dt = 0.0f;
        const float nf = static_cast<float>(nSides);
        if (!g_side.posSet)
        {
            g_side.pos = static_cast<float>(g_side.pick);
            g_side.posSet = true;
        }
        else
        {
            float d = static_cast<float>(g_side.pick) - g_side.pos;
            while (d > nf * 0.5f)
                d -= nf;
            while (d <= -nf * 0.5f)
                d += nf;
            const float k = dt * kTurnRate;
            g_side.pos += d * (k < 1.0f ? k : 1.0f);
            if (d < 0.002f && d > -0.002f)
                g_side.pos = static_cast<float>(g_side.pick);
            while (g_side.pos < 0.0f)
                g_side.pos += nf;
            while (g_side.pos >= nf)
                g_side.pos -= nf;
        }
        const float cx = hzModels ? 0.5f * (hzModelX[0] + hzModelX[1]) : 0.5f * W + look.center * W;
        carR = hzModels ? kCarRadiusHz * (hzModelX[1] - hzModelX[0]) : kCarRadius * look.x * W;
        const float feet = hzModels ? hzModelY : look.feet * H;
        const float front = hzModels ? hzSize : look.size;
        const float kPi = 3.14159265f;
        for (int i = 0; i < nSides; ++i)
        {
            float ang = 2.0f * kPi * (static_cast<float>(i) - g_side.pos) / nf;
            while (ang > kPi)
                ang -= 2.0f * kPi;
            while (ang <= -kPi)
                ang += 2.0f * kPi;
            const float nearness = (std::cos(ang) + 1.0f) * 0.5f;
            carNear[i] = nearness;
            carX[i] = cx + carR * std::sin(ang);
            carY[i] = feet - kCarLift * H * (1.0f - nearness);
            carSize[i] = front * (kCarBack + (1.0f - kCarBack) * nearness);
        }
    }
    int hover = -1;
    float hoverNear = -1.0f; // v7.1: the nearest side under the cursor wins (the front one stands over the others)
    float boxes[kMaxSides][8] = {}; // 21j-r20: each side's soldier box and name box (units), for the click lines
    for (int i = 0; i < nSides && !g_side.look && !hz; ++i) // (20g-r10: not in players 3 / 4's look - no boxes of ours; 20g-r12: nor horizontal)
    {
        const float x = car ? carX[i] : 0.5f * W + look.center * W + SideOffset(i, nSides, look) * W;
        const float y = car ? carY[i] : SideFeetY(i, nSides, look) * H; // v6.7: its row
        const ElemBase& b = g_side.droideka[i] ? db : sb;
        float x0, y0, x1, y1;
        SoldierBox(x, y, b.f80 * (car ? carSize[i] : look.size * sizeK) * (g_side.droideka[i] ? g_dekaSize : 1.0f), g_side.droideka[i],
                   &x0, &y0, &x1, &y1);
        // the name under the soldier (centre-anchored text group: x from the centre, y from the top); v7.1: the carousel
        // shows the picked side's name only, under the front
        const float nx = g_mouse.w * 0.5f + (look.textX + (car ? 0.0f : SideLabelOffset(i, nSides, look))) * RefW();
        const float ny = (car ? look.labelY : SideLabelY(i, nSides, look)) * RefH();
        const bool onName = (!car || i == g_side.pick) && MouseIn(nx - nameHalfW, ny - 22.0f, nx + nameHalfW, ny + 22.0f);
        const float nearness = car ? carNear[i] : 1.0f;
        if ((MouseIn(x0, y0, x1, y1) || onName) && (onName ? 2.0f : nearness) > hoverNear)
        {
            hover = i;
            hoverNear = onName ? 2.0f : nearness;
        }
        const float b8[8] = { x0, y0, x1, y1, nx - nameHalfW, ny - 22.0f, nx + nameHalfW, ny + 22.0f };
        memcpy(boxes[i], b8, sizeof(b8));
    }
    // v7.2: a click on "SELECT SIDE" joins the picked side
    if (mouseA && !g_side.look && !hz && MouseOnPrompt())
    {
        if (ClickLines())
            Log("[wheel] %lu ms: mouse: click on SELECT SIDE - side %d joined", ScanMsSinceAttach(), g_side.pick + 1);
        a = true;
        mouseA = false;
        hover = -1;
    }
    const bool hoverOut = hover >= 0 && out[hover]; // v6.9: under the cursor but out - not picked, a click does nothing
    if (hover >= 0 && !hoverOut && !car) // v7.1: the carousel turns on a click, not under the cursor (it would turn away)
        g_side.pick = hover;
    // v7.1: the carousel and the mouse: a click on another side turns it to the front; a click on the front side (or its
    // name) joins it; elsewhere nothing ([ClassWheel] MouseClicks=1) - horizontal / players 3 / 4's look: a click joins
    if (car && mouseA && !hz)
    {
        if (hover >= 0 && !hoverOut && hover != g_side.pick)
        {
            g_side.pick = hover;
            PlayUiSound(g_sndTick);
            if (ClickLines())
                Log("[wheel] %lu ms: mouse: click on side %d - the carousel turns to it", ScanMsSinceAttach(), hover + 1);
        }
        else if (hover >= 0 && hover == g_side.pick && !hoverOut)
            a = true;
        else if (!g_mouseClicks && !hoverOut)
            a = true;
        mouseA = false;
    }
    if (g_side.pick != was && RingBudget())
        Log("[wheel] %lu ms: side screen: side %d picked%s", ScanMsSinceAttach(), g_side.pick + 1,
            hover >= 0 ? " (under the cursor)" : "");
    if (g_side.pick != was && hover >= 0 && !left && !right)
        PlayUiSound(g_sndTick); // the cursor moved to the other side
    // a left click joins the picked side (the one under the cursor, or the last picked). 21j-r19 ([ClassWheel] MouseClicks=1, the
    // Classic Collection's): only a click on a side (its soldier or its name) joins it - anywhere else nothing (players 3 / 4's
    // look has no boxes of ours: a click joins the picked side there, as before)
    if (mouseA && hoverOut)
    {
        if (ClickLines())
            Log("[wheel] %lu ms: mouse: click on side %d - it is out of the match, nothing done", ScanMsSinceAttach(), hover + 1);
    }
    else if (mouseA && g_mouseClicks && !g_side.look && !hz && hover < 0)
    {
        if (ClickLines())
            Log("[wheel] %lu ms: mouse: click on the side screen at %.0f, %.0f units (%s), on neither side - nothing done (side 1: "
                "soldier %.0f - %.0f across, %.0f - %.0f down, name %.0f - %.0f, %.0f - %.0f; side 2: soldier %.0f - %.0f, %.0f - %.0f, "
                "name %.0f - %.0f, %.0f - %.0f)",
                ScanMsSinceAttach(), g_mouse.x, g_mouse.y, g_mouse.game ? "the game's cursor" : "the Windows cursor",
                boxes[0][0], boxes[0][2], boxes[0][1], boxes[0][3], boxes[0][4], boxes[0][6], boxes[0][5], boxes[0][7], boxes[1][0],
                boxes[1][2], boxes[1][1], boxes[1][3], boxes[1][4], boxes[1][6], boxes[1][5], boxes[1][7]);
    }
    else if (mouseA)
        a = true;

    // 18o: text tuning keys ([ ] names apart, - = names up / down, PageUp / PageDown title, ; ' size)
    // 19g: 1 player only (split screen has the Xbox's layout); v6.5: two sides only
    if (!split && nSides <= 2)
    {
        const bool k1 = (GetAsyncKeyState(VK_OEM_4) & 0x8000) != 0, k2 = (GetAsyncKeyState(VK_OEM_6) & 0x8000) != 0;
        const bool k3 = (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) != 0, k4 = (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) != 0;
        const bool k5 = (GetAsyncKeyState(VK_PRIOR) & 0x8000) != 0, k6 = (GetAsyncKeyState(VK_NEXT) & 0x8000) != 0;
        const bool k7 = (GetAsyncKeyState(VK_OEM_1) & 0x8000) != 0, k8 = (GetAsyncKeyState(VK_OEM_7) & 0x8000) != 0;
        bool tuned = false;
        if (k1 && !g_keyT1 && g_sideLabelX > 0.02f)
            g_sideLabelX -= 0.01f, tuned = true;
        if (k2 && !g_keyT2 && g_sideLabelX < 0.45f)
            g_sideLabelX += 0.01f, tuned = true;
        if (k3 && !g_keyT3 && g_sideLabelY > 0.05f)
            g_sideLabelY -= 0.01f, tuned = true;
        if (k4 && !g_keyT4 && g_sideLabelY < 0.98f)
            g_sideLabelY += 0.01f, tuned = true;
        if (k5 && !g_keyT5 && g_sideTitleY > 0.02f)
            g_sideTitleY -= 0.01f, tuned = true;
        if (k6 && !g_keyT6 && g_sideTitleY < 0.95f)
            g_sideTitleY += 0.01f, tuned = true;
        if (k7 && !g_keyT7 && g_sideTextScale > 0.3f)
            g_sideTextScale -= 0.05f, tuned = true;
        if (k8 && !g_keyT8 && g_sideTextScale < 3.0f)
            g_sideTextScale += 0.05f, tuned = true;
        g_keyT1 = k1, g_keyT2 = k2, g_keyT3 = k3, g_keyT4 = k4, g_keyT5 = k5, g_keyT6 = k6, g_keyT7 = k7, g_keyT8 = k8;
        if (tuned)
        {
            if (RingBudget())
                Log("[wheel] text tuning: names x %.2f y %.2f, title y %.2f, size %.2f", g_sideLabelX, g_sideLabelY,
                    g_sideTitleY, g_sideTextScale);
            for (int i = 0; i < 2; ++i)
                if (g_side.label[i])
                {
                    *reinterpret_cast<float*>(g_side.label[i] + kTextScaleX) = g_sideTextScale;
                    *reinterpret_cast<float*>(g_side.label[i] + kTextScaleY) = g_sideTextScale;
                    g_setTextW(reinterpret_cast<void*>(g_side.label[i]), nullptr, g_side.name[i]); // laid out again
                    g_rf.setPos(reinterpret_cast<void*>(g_side.label[i]), nullptr, (i == 0 ? -g_sideLabelX : g_sideLabelX) * RefW(),
                                (g_sideLabelY - 0.5f) * RefH());
                }
            if (g_side.title)
            {
                *reinterpret_cast<float*>(g_side.title + kTextScaleX) = g_sideTextScale;
                *reinterpret_cast<float*>(g_side.title + kTextScaleY) = g_sideTextScale;
                g_setTextW(reinterpret_cast<void*>(g_side.title), nullptr, L"CHOOSE YOUR SIDE");
                g_rf.setPos(reinterpret_cast<void*>(g_side.title), nullptr, 0.0f, (g_sideTitleY - 0.5f) * RefH());
            }
        }
    }

    // place the soldiers, light the picked one, the others dark (their light turned round)
    for (int i = 0; i < nSides; ++i)
    {
        const uintptr_t e = g_side.model[i];
        if (!e)
            continue;
        const ElemBase& b = g_side.droideka[i] ? db : sb;
        const float k = sb.s / b.s;
        const float x = car ? carX[i]
                            : ((hzModels && i < 2) ? hzModelX[i] : 0.5f * W + look.center * W + SideOffset(i, nSides, look) * W); // 20g-r12
        const float y = car ? carY[i] : (hzModels ? hzModelY : SideFeetY(i, nSides, look) * H); // v6.7: its row
        float* const m = reinterpret_cast<float*>(e + kElemMatrix);
        for (int j = 0; j < 12; ++j)
            m[j] = b.matrix[j];
        m[14] = b.matrix[14] - (car ? (1.0f - carNear[i]) * kSplitRingDepth * carR * k : 0.0f); // v7.1: the back ones further back
        m[15] = b.matrix[15];
        memcpy(reinterpret_cast<void*>(e + kElemRotOfs), b.rotOfs, sizeof(b.rotOfs));
        g_rf.setScale(reinterpret_cast<void*>(e), nullptr, b.s);
        g_rf.setPos(reinterpret_cast<void*>(e), nullptr, b.x + (x - sb.x) * k, b.y + (y - sb.y) * k);
        const uintptr_t set80 = VirtualAt(e, 0x38);
        if (set80)
            reinterpret_cast<ElemSetFloat_t>(set80)(reinterpret_cast<void*>(e), nullptr,
                                                     b.f80 * (car ? carSize[i] : (hzModels ? hzSize : look.size * sizeK)) *
                                                         (g_side.droideka[i] ? g_dekaSize : 1.0f));
        // 18m: +14Ch is where the soldier aims, not a light - both keep the game's straight-ahead aim
        // 18n: the unpicked side dark (the group hands each child's colour to the model's draw)
        uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
        const uint8_t v = out[i] ? kOutGrey : (i == g_side.pick ? 0xFF : kDarkGrey); // v6.9: out - darker still
        c[0] = c[1] = c[2] = v;
        c[3] = 0xFF;
        g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, car ? kZFront + static_cast<int>((1.0f - carNear[i]) * 30.0f) : kZFront);
        g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
        // 18l-r3: no animation on the side screen (no per-frame animation calls)
        TextGrey(g_side.label[i], i != g_side.pick || out[i]);
        if (car && g_side.label[i]) // v7.1: only the picked side's name
            g_rf.setVisible(reinterpret_cast<void*>(g_side.label[i]), nullptr, i == g_side.pick ? 1 : 0);
        if (out[i] && g_side.label[i]) // v6.9: an out side's name darker than an unpicked one's
        {
            uint8_t* const t = reinterpret_cast<uint8_t*>(g_side.label[i] + kElemColor);
            t[0] = t[1] = t[2] = kOutGrey;
        }
    }

    // the game's own soldier stays hidden while the side screen is up
    g_rf.setVisible(reinterpret_cast<void*>(sd + kSdSoldierElem), nullptr, 0);
    if (haveDroid)
        g_rf.setVisible(reinterpret_cast<void*>(sd + kSdDroidekaElem), nullptr, 0);
    g_side.hidGame = true;
    // (20g-r11: in players 3 / 4's look the spawn screen's own blue grid is hidden by P1LookCube, every screen of the flow)
    g_side.lookAt = GetTickCount();

    if (!a)
        return 0;
    if (g_side.pick >= 0 && g_side.pick < nSides && out[g_side.pick]) // v6.9: every side out - nothing to join
    {
        if (RingBudget())
            Log("[wheel] %lu ms: side screen: A on side %d - out of the match, not joined", ScanMsSinceAttach(), g_side.pick + 1);
        return 0;
    }
    const int team = (g_side.pick >= 0 && g_side.pick < nSides) ? g_side.team[g_side.pick] : g_side.pick + 1; // v6.5
    if (RingBudget())
        Log("[wheel] %lu ms: side screen: A - SelectTeam(%d) (the spawn screen's own team select)", ScanMsSinceAttach(),
            team);
    g_selectTeam(reinterpret_cast<void*>(sd), nullptr, team);
    // 19i: its elements go the next frame (SideClearFor, the flow having left the side screen), after
    // the switch to the spawn point screen - deleting them here drew one empty side screen frame
    return 1;
}



// ---- 18p: the game's interface sounds -------------------------------------------------------
// What the Lua ScriptCB_SndPlaySound(name) does: a sound handle (8 bytes, zeroed), SetByName(handle,
// name, 1), Play(handle, 0, 0, 1, 1). A name no loaded sound bank has plays nothing.
// 18q: does a loaded sound bank know this name? SetByName zeroes the handle, then sets its sound
// pointer only for a name it finds (a sound, a stream or a sound property) - the pointer is the test.
bool SoundKnown(const char* name)
{
    uint8_t handle[8] = {};
    g_sndSet(handle, nullptr, name, 1);
    uint32_t p = 0;
    memcpy(&p, handle, 4);
    return p != 0;
}

// the first name of the list the sound banks know, looked up on the first play (and again, once
// more, if the name it had stops resolving - a sound bank came or went); logged
const char* ResolveSound(UiSound& s)
{
    if (s.resolved >= 0)
        return s.names[s.resolved];
    if (s.resolved == -2)
        return nullptr;
    if (s.tries >= 3)
    {
        s.resolved = -2;
        return nullptr;
    }
    ++s.tries;
    char line[400] = "";
    for (int i = 0; i < s.count; ++i)
    {
        const bool known = SoundKnown(s.names[i]);
        if (strlen(line) < sizeof(line) - 60)
        {
            strcat_s(line, s.names[i]);
            strcat_s(line, known ? " YES" : " no");
            strcat_s(line, i + 1 < s.count ? ", " : "");
        }
        if (known && s.resolved < 0)
            s.resolved = i;
    }
    Log("[wheel] %lu ms: sound \"%s\": %s%s", ScanMsSinceAttach(), s.what, line,
        s.resolved >= 0 ? "" : " - NONE of these is in the loaded sound banks (see the sound names line)");
    return s.resolved >= 0 ? s.names[s.resolved] : nullptr;
}

// 19i: true if a sound was played (its name logged for the first few plays)
bool PlayUiSound(UiSound& s)
{
    static int plays = 0;
    if (!g_sndOk)
        return false;
    const char* name = ResolveSound(s);
    if (!name)
        return false;
    uint8_t handle[8] = {};
    g_sndSet(handle, nullptr, name, 1);
    uint32_t p = 0;
    memcpy(&p, handle, 4);
    if (!p)
    {
        // the name it had is gone (another sound bank set): look again next time
        s.resolved = -1;
        return false;
    }
    g_sndPlay(handle, nullptr, 0, 0, 1, 1);
    if (plays++ < 16)
        Log("[wheel] %lu ms: sound \"%s\" played: %s (handle 0x%08X)", ScanMsSinceAttach(), s.what, name, static_cast<unsigned>(p));
    return true;
}

// ---- 18q: the mouse (game thread) -----------------------------------------------------------
// The cursor in the interface units the soldiers stand in: x from the left edge of the screen, y
// from the top, 600 (the reference height) for the whole height - the units the soldiers' group
// (+1A0h, anchored top left) places its children in (the game's soldier at 512, 520 stands at 48%
// across and 87% down a 16:9 screen). From the game's own cursor (what its hot-spot test reads),
// or the Windows cursor over the game's client area when that is not there.

bool MouseLines()
{
    return g_mouseLines++ < 60;
}

void ReadMouse()
{
    Mouse m;
    int dw = 0;
    int dh = 0;
    float px = 0.0f;
    float py = 0.0f;
    bool got = false;
    if (g_mouseOk)
    {
        // 21j-r20: every logged run so far used the Windows cursor, which is not the game's: the game moves its own cursor by
        // the mouse's raw counts (DirectInput, GOG 1.3 0x006A1170) and leaves the Windows one where it was (512, 312 at every
        // flow start) - the clicks landed where the Windows cursor was, not where the game drew its arrow. Why: the input
        // manager's vfunc 20h (GetBufB, whose +138h / +13Ch are the cursor) is BF1GameExt's own since 6c (player2_input.cpp's
        // Hook_GetBufB - index 0 still the game's), outside the exe's code, so VirtualAt refused it. Now an executable vfunc
        // outside .text is taken too; the window's size stands in for the display's if that is not usable; the cursor is kept to
        // the screen; and how it was read (or why not) is logged ([wheel] "mouse: the game's cursor")
        const int ddw = static_cast<int>(ReadU32(g_displaySize));
        const int ddh = static_cast<int>(ReadU32(g_displaySize + 4));
        const uintptr_t vt = ReadU32(g_inputMgr);
        uintptr_t vf = VirtualAt(g_inputMgr, 0x20);
        const bool vfInCode = vf != 0;
        if (!vf && vt)
        {
            const uintptr_t raw = ReadU32(vt + 0x20);
            MEMORY_BASIC_INFORMATION mbi = {};
            if (raw && VirtualQuery(reinterpret_cast<LPCVOID>(raw), &mbi, sizeof(mbi)) && mbi.State == MEM_COMMIT &&
                (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0)
                vf = raw;
        }
        const uintptr_t dev = vf ? reinterpret_cast<uintptr_t>(
                                       reinterpret_cast<InputDevice_t>(vf)(reinterpret_cast<void*>(g_inputMgr), nullptr, 0))
                                 : 0;
        int32_t xy[2] = {};
        const bool read = dev && ReadMem(dev + kMouseX, xy, sizeof(xy)); // x, y side by side (kMouseY = kMouseX + 4)
        int cw = 0, ch = 0;
        {
            const HWND fg = GetForegroundWindow();
            DWORD pid = 0;
            if (fg)
                GetWindowThreadProcessId(fg, &pid);
            RECT r = {};
            if (fg && pid == GetCurrentProcessId() && GetClientRect(fg, &r))
            {
                cw = r.right;
                ch = r.bottom;
            }
        }
        const bool dispOk = ddw > 100 && ddw < 20000 && ddh > 100 && ddh < 20000;
        const bool clientOk = cw > 100 && cw < 20000 && ch > 100 && ch < 20000;
        dw = dispOk ? ddw : (clientOk ? cw : 0);
        dh = dispOk ? ddh : (clientOk ? ch : 0);
        int why = 0; // 0 = the game's cursor, else why not
        if (!vf)
            why = 1;
        else if (!dev)
            why = 2;
        else if (!read)
            why = 3;
        else if (!dw || !dh)
            why = 4;
        else if (xy[0] < -dw || xy[0] > 2 * dw || xy[1] < -dh || xy[1] > 2 * dh)
            why = 5;
        if (!why)
        {
            px = static_cast<float>(xy[0] < 0 ? 0 : (xy[0] > dw - 1 ? dw - 1 : xy[0]));
            py = static_cast<float>(xy[1] < 0 ? 0 : (xy[1] > dh - 1 ? dh - 1 : xy[1]));
            got = true;
            m.game = true;
        }
        static int lastWhy = -1;
        static int whyLines = 0;
        static bool sizeLogged = false;
        if ((why != lastWhy || (!why && !dispOk && !sizeLogged)) && whyLines < 24)
        {
            ++whyLines;
            sizeLogged = sizeLogged || (!why && !dispOk);
            static const char* const kWhy[] = { "used", "NOT used: no input vfunc 20h", "NOT used: no input object",
                                                "NOT used: its cursor could not be read", "NOT used: no screen size",
                                                "NOT used: its cursor is off the screen" };
            Log("[wheel] %lu ms: mouse: the game's cursor %s - manager 0x%08X vtable 0x%08X, vfunc 20h 0x%08X (%s), input 0x%08X, "
                "+138h / +13Ch = %d, %d; display %d x %d (%s), window %d x %d",
                ScanMsSinceAttach(), kWhy[why], static_cast<unsigned>(g_inputMgr), static_cast<unsigned>(vt),
                static_cast<unsigned>(vf), vf ? (vfInCode ? "in the exe's code" : "outside the exe's code - BF1GameExt's player 2 redirect") : "none",
                static_cast<unsigned>(dev), xy[0], xy[1], ddw, ddh, dispOk ? "used" : "not usable - the window's size used", cw, ch);
        }
        lastWhy = why;
    }
    if (!got)
    {
        const HWND fg = GetForegroundWindow();
        DWORD pid = 0;
        if (fg)
            GetWindowThreadProcessId(fg, &pid);
        POINT p = {};
        RECT r = {};
        if (fg && pid == GetCurrentProcessId() && GetCursorPos(&p) && ScreenToClient(fg, &p) && GetClientRect(fg, &r) &&
            r.right > 100 && r.bottom > 100)
        {
            dw = r.right;
            dh = r.bottom;
            px = static_cast<float>(p.x);
            py = static_cast<float>(p.y);
            got = true;
        }
    }
    if (!got)
    {
        g_mouse = m;
        return;
    }
    // 18w: in split screen player 1 has the left half: the cursor over half the width
    const bool split = LocalPlayersNow() >= 2;
    if (split)
        dw /= 2;
    if (split && px > static_cast<float>(dw) + 64.0f)
    {
        g_mouse = m; // over player 2's half: no cursor for the flow
        return;
    }
    const float unitsPerPx = RefH() / static_cast<float>(dh);
    m.ok = true;
    m.px = px;
    m.py = py;
    m.dw = dw;
    m.dh = dh;
    m.x = px * unitsPerPx;
    m.y = py * unitsPerPx;
    m.w = static_cast<float>(dw) * unitsPerPx;
    static int firstReads = 0;
    if (firstReads < 3 && MouseLines())
    {
        ++firstReads;
        Log("[wheel] %lu ms: mouse: %s at %.0f, %.0f px of %d x %d = %.0f, %.0f units (screen %.0f x %.0f units)",
            ScanMsSinceAttach(), m.game ? "the game's cursor" : "the Windows cursor", px, py, dw, dh, m.x, m.y, m.w, RefH());
    }
    g_mouse = m;
}

bool MouseIn(float x0, float y0, float x1, float y1)
{
    return g_mouse.ok && g_mouse.x >= x0 && g_mouse.x <= x1 && g_mouse.y >= y0 && g_mouse.y <= y1;
}

// the BACK text of the console screens: the bottom left corner
bool MouseOnBack()
{
    return g_mouse.ok && g_mouse.x < g_mouse.w * 0.11f && g_mouse.y > RefH() * 0.87f;
}

// 21j-r19: the soldier select's wheel under the cursor, as the Classic Collection's: -1 = left of the picked soldier, +1 = right
// of it, 0 = off the wheel. The wheel's area: from the left edge of the screen (his half in split screen) to a little past the
// ring's right-hand soldier, from a little over the soldiers' heads down to above the BACK / SELECT CHARACTER row - the class
// window to the right of it is not the wheel
constexpr float kWheelMargin = 40.0f; // interface units (the screen is 600 high)
int RingSideAt(uintptr_t sd)
{
    if (g_ring.sd != sd || !g_ring.spanOk || !g_mouse.ok)
        return 0;
    const float bottom = (g_ring.span[3] + kWheelMargin < RefH() * 0.85f) ? g_ring.span[3] + kWheelMargin : RefH() * 0.85f;
    if (!MouseIn(0.0f, g_ring.span[1] - kWheelMargin, g_ring.span[2] + kWheelMargin, bottom))
        return 0;
    return g_mouse.x < g_ring.pickX ? -1 : 1;
}

// 21j-r19: a click on the spawn point screen's map: the game's own test (what its PC screen does with a click on its map) - the
// post whose icon is under the cursor, if it is his team's - picked as the left / right keys pick one (SetSpawnPoint). 0 = no
// post there (or no test on this build)
uintptr_t PostUnderMouse(uintptr_t sd)
{
    if (!g_postAt || !g_postAtMaps || !g_mouse.ok)
        return 0;
    const uint32_t player = ReadU32(sd + kTailStart + 4 * kIdxPlayer);
    if (player > 3 || !ReadU32(g_postAtMaps + 4 * player)) // PostAt asks his map without looking
        return 0;
    return g_postAt(reinterpret_cast<void*>(sd), nullptr, g_mouse.px, g_mouse.py);
}

bool ClickLines()
{
    return g_clickLines++ < 200;
}

// a soldier's box in interface units: its feet at (x, y), its height from its size (+80h: the
// game's soldier, size 0.936, stands about 400 units tall), a droideka wider and lower
void SoldierBox(float x, float y, float size80, bool droideka, float* x0, float* y0, float* x1, float* y1)
{
    const float h = (droideka ? 340.0f : 430.0f) * size80;
    const float hw = (droideka ? 0.34f : 0.17f) * h;
    *x0 = x - hw;
    *x1 = x + hw;
    *y0 = y - h;
    *y1 = y + 0.03f * h;
}

// log-only, once: every "shell_..." text in the game's memory (the sound banks' names), so the
// names above can be checked against what the game has
void ShellSoundScan()
{
    if (g_shellScanned)
        return;
    g_shellScanned = true;
    HMODULE self = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCSTR>(&g_shellScanned), &self);
    const BuildInfo* b = CurrentBuild();
    const uintptr_t exeLo = ExeBase();
    const uintptr_t exeHi = ExeBase() + (b ? b->sizeOfImage : 0);
    const size_t kChunk = 1 << 20;
    unsigned char* buf =
        static_cast<unsigned char*>(VirtualAlloc(nullptr, kChunk + 256, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!buf)
        return;
    const DWORD readable = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ |
                           PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    char names[32][40];
    int count = 0;
    int seen = 0;
    uintptr_t addr = 0x10000;
    MEMORY_BASIC_INFORMATION mbi;
    const DWORD t0 = GetTickCount();
    while (addr < 0xFFFF0000u && VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)) &&
           GetTickCount() - t0 < 4000)
    {
        const uintptr_t regionLo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        const uintptr_t regionHi = regionLo + mbi.RegionSize;
        if (regionHi <= addr)
            break;
        const bool skip = mbi.State != MEM_COMMIT || !(mbi.Protect & readable) || (mbi.Protect & PAGE_GUARD) ||
                          (regionLo < exeHi && regionHi > exeLo) || (self && mbi.AllocationBase == self) ||
                          regionLo == reinterpret_cast<uintptr_t>(buf);
        for (uintptr_t p = regionLo; !skip && p < regionHi; p += kChunk)
        {
            const size_t want = (regionHi - p < kChunk + 200) ? static_cast<size_t>(regionHi - p) : kChunk + 200;
            SIZE_T got = 0;
            if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(p), buf, want, &got) || got < 8)
                continue;
            for (size_t i = 0; i + 6 <= got && i < kChunk; ++i)
            {
                if (memcmp(buf + i, "shell_", 6) != 0)
                    continue;
                if (i > 0 && (isalnum(buf[i - 1]) || buf[i - 1] == '_'))
                    continue;
                size_t n = 6;
                while (i + n < got && n < 39 && (islower(buf[i + n]) || isdigit(buf[i + n]) || buf[i + n] == '_'))
                    ++n;
                if (i + n >= got || buf[i + n] != 0 || n < 8)
                    continue;
                ++seen;
                char text[40];
                memcpy(text, buf + i, n);
                text[n] = 0;
                bool dup = false;
                for (int k = 0; k < count && !dup; ++k)
                    dup = strcmp(names[k], text) == 0;
                if (!dup && count < 32)
                    strcpy_s(names[count++], text);
            }
        }
        addr = regionHi;
    }
    VirtualFree(buf, 0, MEM_RELEASE);
    char line[1400] = {};
    for (int k = 0; k < count; ++k)
    {
        strcat_s(line, names[k]);
        strcat_s(line, " ");
    }
    Log("[wheel] sound names: %d \"shell_...\" text(s) in memory, %d distinct%s: %s", seen, count,
        count >= 32 ? " (first 32)" : "", count ? line : "(none - the sound names used may be wrong)");
}

// ---- 18o: the spawn point screen's and the soldier select's texts, game thread ----------------
// Texts of the screen the console flow is on: made when the screen comes up, deleted when it is
// left. Panel texts are children of the console screen's own panels (kept only while the panel is
// still in the element registry); prompts are in the spawn screen's text group (+300h, centre).
struct ScreenText
{
    uintptr_t e = 0;
    const char* panel = nullptr; // registry path of the panel it is in (null = the spawn screen's text group)
    uintptr_t group = 0;
};
struct ScreenTexts
{
    uintptr_t sd = 0;
    LONG flow = 0;
    ScreenText t[8];
    int n = 0;
    uintptr_t postText = 0;   // the COMMAND POST box text
    uintptr_t vehText = 0;    // the VEHICLES box text
    uintptr_t objText = 0;    // the OBJECTIVES box text
    uintptr_t shownPost = 0;
    LONG vehSeq = -1;
    LONG objSeq = -1;
    uintptr_t hints = 0;      // ifs_mapselect1.SideBar.Hints, hidden while on the spawn point screen
    uintptr_t prompt = 0;     // 18q: "SELECT SPAWNPOINT" / "SELECT CHARACTER" (yellow under the cursor)
    bool promptLit = false;
    int lines = 0;
};
ScreenTexts g_stx;
float g_promptX = 0.46f;    // "SELECT ..." prompts: right of the centre, of the reference width
float g_promptY = 0.43f;    // below the centre, of the height
float g_selSoldierY = 0.34f; // "SELECT SOLDIER": below the centre, of the height
// 21j-r9: "SELECT SOLDIER" across (of the screen's width, from the centre): it moves with the ring - half the ring's shift from
// -0.13 (the ring's units are the game's soldier's x, half the screen)
float SelSoldierX()
{
    return (g_ringShift - kRingShiftWas) * 0.5f;
}
float g_promptScale = 1.0f;
// 19g: split screen (player 1's half): the prompts on the Xbox's "B BACK" row, centred at about 70%
// across and 89% down (at 0.46 / 0.43 they stood off his half); no "SELECT SOLDIER" (the Xbox has none)
constexpr float kSplitPromptX = 0.105f;
constexpr float kSplitPromptY = 0.405f;
float PromptX()
{
    return LocalPlayersNow() >= 2 ? kSplitPromptX : g_promptX;
}
float PromptY()
{
    return LocalPlayersNow() >= 2 ? kSplitPromptY : g_promptY;
}

bool StxBudget()
{
    return g_stx.lines++ < 120;
}

// 18q: the cursor on the "SELECT ..." prompt (centre-anchored text group: x from the screen's
// centre, y from the top; the text is about 240 units wide)
bool MouseOnPrompt()
{
    if (!g_mouse.ok)
        return false;
    const float px = g_mouse.w * 0.5f + PromptX() * RefW();
    const float py = (0.5f + PromptY()) * RefH();
    return MouseIn(px - 130.0f, py - 22.0f, px + 130.0f, py + 22.0f);
}

// the two SetText calls the game makes for its PC popup (the vehicles at the post) and objective
// window (the team's objective) go through here first: the strings are kept for our panels
void __fastcall VehTextHook(void* elem, void* unusedEdx, const wchar_t* s)
{
    // 19h: ClassWheelVehiclesAt's own PopupFill calls (player 2's post, then player 1's back): the
    // string is kept for player 2 (or dropped), the PC popup's text and player 1's panel left as they were
    if (g_vehCapture)
    {
        if (g_vehCapture == 1)
        {
            if (s)
                wcsncpy_s(g_vehCaptured, s, _TRUNCATE);
            else
                g_vehCaptured[0] = 0;
        }
        return;
    }
    if (s)
    {
        wcsncpy_s(g_vehText, s, _TRUNCATE);
        InterlockedIncrement(&g_vehSeq);
    }
    g_setTextW(elem, unusedEdx, s);
}

void __fastcall ObjTextHook(void* elem, void* unusedEdx, const wchar_t* s)
{
    if (s)
    {
        wcsncpy_s(g_objText, s, _TRUNCATE);
        InterlockedIncrement(&g_objSeq);
    }
    g_setTextW(elem, unusedEdx, s);
}

void ScreenTextsClear(const char* why, bool sdAlive)
{
    int n = 0;
    int skipped = 0;
    for (int i = 0; i < g_stx.n; ++i)
    {
        ScreenText& st = g_stx.t[i];
        if (!st.e)
            continue;
        // a panel text is deleted only while its panel is still in the registry (the console
        // screen may already be gone when the spawn screen itself is deleted)
        const bool ok = st.panel ? (LiveLookup(st.panel) == st.group) : true;
        if (ok)
        {
            ElemDelete(st.e);
            ++n;
        }
        else
            ++skipped;
        st.e = 0;
    }
    if (g_stx.hints && sdAlive && LiveLookup("ifs_mapselect1.SideBar.Hints") == g_stx.hints)
        g_rf.setVisible(reinterpret_cast<void*>(g_stx.hints), nullptr, 1);
    g_stx.hints = 0;
    g_stx.prompt = 0;
    g_stx.promptLit = false;
    g_stx.n = 0;
    g_stx.postText = g_stx.vehText = g_stx.objText = 0;
    g_stx.shownPost = 0;
    g_stx.vehSeq = g_stx.objSeq = -1;
    g_stx.flow = 0;
    if ((n || skipped) && StxBudget())
        Log("[wheel] %lu ms: screen texts: %d deleted%s (%s)", ScanMsSinceAttach(), n,
            skipped ? " (some left to the game: their panel is gone)" : "", why);
}

uintptr_t StxAdd(uintptr_t e, const char* panel, uintptr_t group)
{
    if (e && g_stx.n < 8)
    {
        g_stx.t[g_stx.n].e = e;
        g_stx.t[g_stx.n].panel = panel;
        g_stx.t[g_stx.n].group = group;
        ++g_stx.n;
    }
    return e;
}

// a text inside a console panel, where ifs_charselect1's BoxText sits under its title bar
uintptr_t PanelText(const char* panel, const char* titlePath, const wchar_t* s)
{
    const uintptr_t group = LiveLookup(panel);
    const uintptr_t title = LiveLookup(titlePath);
    const uintptr_t like = g_drv.box;        // ifs_charselect1.Info.Window.BoxText
    const uintptr_t likeTitle = g_drv.title; // ifs_charselect1.Info.Window.titleBarElement
    if (!group || !title || !like || !likeTitle)
    {
        if (StxBudget())
            Log("[wheel] screen texts: %s - panel 0x%08X, title 0x%08X, BoxText 0x%08X / its title 0x%08X: MISSING", panel,
                static_cast<unsigned>(group), static_cast<unsigned>(title), static_cast<unsigned>(like),
                static_cast<unsigned>(likeTitle));
        return 0;
    }
    const float x = ReadF32(title + kElemX) + (ReadF32(like + kElemX) - ReadF32(likeTitle + kElemX));
    const float y = ReadF32(title + kElemY) + (ReadF32(like + kElemY) - ReadF32(likeTitle + kElemY));
    const uintptr_t e = TextLike(group, like, s, x, y);
    if (StxBudget())
        Log("[wheel] %lu ms: screen texts: %s text 0x%08X at %.1f %.1f (title at %.1f %.1f, BoxText offset %.1f %.1f)",
            ScanMsSinceAttach(), panel, static_cast<unsigned>(e), x, y, ReadF32(title + kElemX), ReadF32(title + kElemY),
            ReadF32(like + kElemX) - ReadF32(likeTitle + kElemX), ReadF32(like + kElemY) - ReadF32(likeTitle + kElemY));
    return StxAdd(e, panel, group);
}

uintptr_t PromptText(uintptr_t sd, const wchar_t* s, float fx, float fy)
{
    const uintptr_t e = TextNew(sd + kSdTextGroup, "gamefont_large", s, fx * RefW(), fy * RefH(), g_promptScale, kZText);
    return StxAdd(e, nullptr, sd + kSdTextGroup);
}

// 20g-r12: horizontal 2-player - a prompt where the Classic Collection has it (centred at (hx, kHzPromptY) of his half,
// through his HUD root); the split screen's place if the text group is not in a HUD root
uintptr_t PromptTextHz(uintptr_t sd, const wchar_t* s, float hx)
{
    float x = 0.0f;
    float y = 0.0f;
    if (!P2SpawnGroupPlace(sd + kSdTextGroup, hx, kHzPromptY, &x, &y))
    {
        if (HzBudget())
            Log("[wheel] %lu ms: screen texts: horizontal - the text group 0x%08X is not in a HUD root: the prompt at the split "
                "screen's place", ScanMsSinceAttach(), static_cast<unsigned>(sd + kSdTextGroup));
        return PromptText(sd, s, PromptX(), PromptY());
    }
    const uintptr_t e = TextNew(sd + kSdTextGroup, "gamefont_large", s, x, y, kHzPromptScale, kZText);
    if (HzBudget())
        Log("[wheel] %lu ms: screen texts: horizontal - prompt 0x%08X at %.0f %.0f (%.3f %.3f of his half, Classic's), scale %.2f",
            ScanMsSinceAttach(), static_cast<unsigned>(e), x, y, hx, kHzPromptY, kHzPromptScale);
    return StxAdd(e, nullptr, sd + kSdTextGroup);
}

void PostNameText(uintptr_t post)
{
    if (!g_stx.postText || !post || post == g_stx.shownPost)
        return;
    g_stx.shownPost = post;
    wchar_t name[96];
    if (!ReadWide(ReadU32(post + kPostName), name, 96))
        name[0] = 0;
    g_setTextW(reinterpret_cast<void*>(g_stx.postText), nullptr, name);
}

// every frame the console flow is on: the texts of the screen it is on
void ScreenTextsUpdate(uintptr_t sd, LONG flow)
{
    if (!TextOk())
        return;
    if (g_stx.sd != sd)
    {
        g_stx = ScreenTexts();
        g_stx.sd = sd;
    }
    if (g_stx.flow != flow)
    {
        ScreenTextsClear("screen changed", true);
        g_stx.flow = flow;
        if (flow == 3)
        {
            g_stx.postText = PanelText("ifs_mapselect1.SideBar.Spawnpoint",
                                       "ifs_mapselect1.SideBar.Spawnpoint.titleBarElement", L"");
            g_stx.vehText = PanelText("ifs_mapselect1.SideBar.Vehicle", "ifs_mapselect1.SideBar.Vehicle.titleBarElement",
                                      g_vehText);
            g_stx.objText = PanelText("ifs_mapselect1.SideBar.Objectives",
                                      "ifs_mapselect1.SideBar.Objectives.titleBarElement", g_objText);
            g_stx.vehSeq = g_vehSeq;
            g_stx.objSeq = g_objSeq;
            g_stx.hints = LiveLookup("ifs_mapselect1.SideBar.Hints");
            if (g_stx.hints)
                g_rf.setVisible(reinterpret_cast<void*>(g_stx.hints), nullptr, 0);
            g_stx.prompt = ClassWheelHorizontal() ? PromptTextHz(sd, L"SELECT SPAWNPOINT", kHzPromptPostX) // 20g-r12
                                                  : PromptText(sd, L"SELECT SPAWNPOINT", PromptX(), PromptY());
        }
        else if (flow == 1) // v7.2 (Ben): the side screen's "SELECT SIDE", across from BACK (as the other screens' prompts)
        {
            g_stx.prompt = ClassWheelHorizontal() ? PromptTextHz(sd, L"SELECT SIDE", kHzPromptPostX)
                                                  : PromptText(sd, L"SELECT SIDE", PromptX(), PromptY());
        }
        else if (flow == 2)
        {
            g_stx.prompt = ClassWheelHorizontal() ? PromptTextHz(sd, L"SELECT CHARACTER", kHzPromptClassX) // 20g-r12
                                                  : PromptText(sd, L"SELECT CHARACTER", PromptX(), PromptY());
            if (LocalPlayersNow() < 2)
                PromptText(sd, L"SELECT SOLDIER", SelSoldierX(), g_selSoldierY); // 21j-r9: under the ring (moved left with it)
        }
    }
    // 18q: the prompt yellow while the cursor is on it
    if (g_stx.prompt)
    {
        const bool lit = !ClassWheelHorizontal() && MouseOnPrompt(); // (20g-r12: not in horizontal - the box is the vertical half's)
        if (lit != g_stx.promptLit)
        {
            uint8_t* const c = reinterpret_cast<uint8_t*>(g_stx.prompt + kElemColor); // B G R A
            c[0] = lit ? 0x20 : 0xFF;
            c[1] = lit ? 0xD8 : 0xFF;
            c[2] = 0xFF;
            c[3] = 0xFF;
            g_stx.promptLit = lit;
        }
    }
    if (flow == 3)
    {
        PostNameText(ReadU32(sd + kSdPost));
        if (g_stx.vehText && g_stx.vehSeq != g_vehSeq)
        {
            g_stx.vehSeq = g_vehSeq;
            g_setTextW(reinterpret_cast<void*>(g_stx.vehText), nullptr, g_vehText);
        }
        if (g_stx.objText && g_stx.objSeq != g_objSeq)
        {
            g_stx.objSeq = g_objSeq;
            g_setTextW(reinterpret_cast<void*>(g_stx.objText), nullptr, g_objText);
        }
    }
}

// ---- 18m: the spawn screen's map, game thread --------------------------------
// The spawn screen's map is the HUD map (MapDisplay) in its big 1-player layout: layout 1 of the
// game's layout table (size, size, x offset, y offset, ...). BF1GameExt changes layout 1 and has
// the map re-apply it with its own SetMode. want: 1 = off the screen (side screen, soldier
// select), 2 = moved left and bigger (spawn point screen), 0 = as the game had it.
struct MapLook
{
    bool saved = false;
    float orig[2][4] = {}; // layouts 1 and 2 as the game had them
    int lines = 0;
};
MapLook g_mapLook;
float g_mapX = -384.0f;  // the spawn point screen: x offset added (the layout's 800-wide units)
float g_mapY = 0.0f;     // y offset added
float g_mapSize = 1.13f; // size, of the game's
bool g_keyMapLeft = false;
bool g_keyMapRight = false;
bool g_keyMapUp = false;
bool g_keyMapDown = false;
bool g_keyMapSmaller = false;
bool g_keyMapBigger = false;
constexpr float kMapHideX = 5000.0f;

// 18n: the big layouts are 1 (1 player, spawn screen mode) and 2 (1 player, the map's other flag);
// the one the map is in gets the change, the other stays as the game had it
// 19g: in split screen the spawn map is layout 5 (player 1's: his map's flag is set on his spawn
// screens) or layout 4 (player 2's in mode 1, split_spawn_menus.cpp) - the game's pick with more than one
// player local (GOG 1.3 0x001873C0); 18z changed layout 1, which split screen never uses. Each is set
// to the Xbox's map while that player's spawn point screen is up, off the screen on his side / class
// screens (player 1's: the game shows his map again every frame, so hiding its element never held),
// and as the game had it otherwise. Each player's root starts at 0.1 (player 1) / 0 (player 2) of
// his half and is 0.88 of it wide; its y runs from 0.05 down 0.9 of the height (19f's video: layout
// 5's anchor 0.3 / 0.5 and y offset -16 drew player 1's map centred at 36% / 47%, 25% of the height
// tall at size 128). The Xbox's: centred at 52% (player 1) / 47% (player 2) across, 31% down, 34% of
// the height tall.
// 19h: 19g's video: size 176 stood 43% of the half wide and 34% tall; the Xbox's is 38% wide, 33% tall
// (157 x 168); player 2's: his root draws 0.88 of the anchor across (no offset): 0.542 for the Xbox's
// 47.7% (not seen yet: his map never drew before 19h-r2)
constexpr float kXbMapW = 157.0f;
constexpr float kXbMapH = 168.0f;
constexpr float kXbMapAnchorY = 0.29f;
constexpr float kXbMapAnchorX[2] = { 0.477f, 0.542f };
constexpr float kMapOffAnchorX = -5.0f;  // off the screen (to the left)
// 20g-r11: 4-player, players 3 / 4's look - player 1's map where theirs stands in their quarters (split_spawn_menus.cpp's layout 1:
// 138 x 148 at 0.28 / 0.53 of the root). A root draws a map at its offset + 0.88 x the anchor (r10's video: player 1's at
// 0.1 + 0.88 x 0.477 / 0.1 + 0.88 x 0.29 of his quarter, as drawn); player 1's root starts 0.1 down, player 3's 0.02 - so
// his anchor y is 0.53 - 0.08 / 0.88 for the same place in his quarter as player 3's (x: both start 0.1 across)
constexpr float kQMapW = 138.0f;
constexpr float kQMapH = 148.0f;
constexpr float kQMapAnchorX = 0.28f;
constexpr float kQMapAnchorYP1 = 0.53f - 0.08f / 0.88f;
int g_splitMapWant[2] = { 0, 0 };        // per player: 0 = as the game had it, 1 = off the screen, 2 = the Xbox's
bool g_splitMapLook = false;             // 20g-r11: player 1's map (layout 5) in players 3 / 4's place (ClassWheelFrame)
float g_splitMapOrig[2][kLayoutFloats] = {};
bool g_splitMapSaved = false;

int SplitLayoutOf(int player)
{
    return player == 0 ? kSplitLayoutP1 : kSplitLayoutP2;
}

// the layout a map is in with more than one player local (the game's pick: flag -> 5, else by mode)
int MapLayoutSplit(uintptr_t map)
{
    uint8_t flag = 0;
    ReadMem(map + kMapFlag, &flag, 1);
    const uint32_t mode = ReadU32(map + kMapMode);
    return flag ? 5 : (mode == 1 ? 4 : (mode == 0 ? 3 : 6));
}

// both split layouts as the players' screens want them (written, and re-applied by the maps in them,
// only when they change). (Player 2's map's Update runs with the flag set and layout 4's values lent
// to layout 5 for that call only - split_spawn_menus.cpp.)
void SplitMapLayout()
{
    if (!g_mapOk || !g_mapTable || !g_mapArray)
        return;
    if (!g_splitMapSaved)
    {
        for (int p = 0; p < 2; ++p)
            memcpy(g_splitMapOrig[p], reinterpret_cast<const void*>(g_mapTable + kMapLayout1 * static_cast<uint32_t>(SplitLayoutOf(p))),
                   sizeof(g_splitMapOrig[p]));
        g_splitMapSaved = true;
        if (g_mapLook.lines++ < 80)
            Log("[wheel] map: split screen layouts as the game has them: 5 (player 1's) %.0f x %.0f %+.0f %+.0f anchor %.3f %.3f, "
                "4 (player 2's) %.0f x %.0f %+.0f %+.0f anchor %.3f %.3f",
                g_splitMapOrig[0][0], g_splitMapOrig[0][1], g_splitMapOrig[0][2], g_splitMapOrig[0][3], g_splitMapOrig[0][4],
                g_splitMapOrig[0][5], g_splitMapOrig[1][0], g_splitMapOrig[1][1], g_splitMapOrig[1][2], g_splitMapOrig[1][3],
                g_splitMapOrig[1][4], g_splitMapOrig[1][5]);
    }
    const bool hzOn = ClassWheelHorizontal(); // 20g-r12
    for (int p = 0; p < 2; ++p)
    {
        const int want = g_splitMapWant[p];
        float d[kLayoutFloats];
        memcpy(d, g_splitMapOrig[p], sizeof(d));
        const bool look = p == 0 && want == 2 && g_splitMapLook; // 20g-r11: players 3 / 4's place
        // 20g-r12: horizontal 2-player - the spawn point screen's map where the Classic Collection has it in each half: its
        // anchor through the player's HUD root's safe area (a root draws a map at offset + scale x anchor)
        float w = 0.0f, h = 0.0f, ox = 0.0f, oy = 0.0f, kx = 0.0f, ky = 0.0f;
        const bool hz = hzOn && want == 2 && !look && P2SpawnRootGeom(p, &w, &h, &ox, &oy, &kx, &ky);
        if (want != 0)
        {
            d[0] = look ? kQMapW : (hz ? kHzMapW : kXbMapW); // 19h: its own width and height (19g: 176 both)
            d[1] = look ? kQMapH : (hz ? kHzMapH : kXbMapH);
            d[2] = d[3] = 0.0f;
            d[4] = look ? kQMapAnchorX : (hz ? (kHzMapX - ox) / kx : (want == 2 ? kXbMapAnchorX[p] : kMapOffAnchorX));
            d[5] = look ? kQMapAnchorYP1 : (hz ? (kHzMapY - oy) / ky : kXbMapAnchorY);
        }
        const int layout = SplitLayoutOf(p);
        float* const ent = reinterpret_cast<float*>(g_mapTable + kMapLayout1 * static_cast<uint32_t>(layout));
        if (memcmp(ent, d, sizeof(d)) == 0)
            continue;
        memcpy(ent, d, sizeof(d));
        // re-applied by each map in that layout now (its own SetMode with its own mode)
        int applied = 0;
        for (int q = 0; q < 2; ++q)
        {
            const uintptr_t map = ReadU32(g_mapArray + 4 * static_cast<uint32_t>(q));
            if (map && g_mapSetMode && MapLayoutSplit(map) == layout)
            {
                g_mapSetMode(reinterpret_cast<void*>(map), nullptr, static_cast<int>(ReadU32(map + kMapMode)));
                ++applied;
            }
        }
        if (g_mapLook.lines++ < 80)
            Log("[wheel] %lu ms: map: split screen layout %d (player %d's) %s: %.0f x %.0f %+.0f %+.0f, anchor %.3f %.3f - "
                "%d map(s) in it re-applied",
                ScanMsSinceAttach(), layout, p + 1,
                look ? "players 3 / 4's place (4-player)"
                     : (hz ? "the Classic Collection's place (horizontal)"
                           : (want == 2 ? "the Xbox's place" : (want == 1 ? "off the screen" : "as the game had it"))),
                d[0], d[1], d[2], d[3], d[4],
                d[5], applied);
    }
}

void MapUpdate(int want)
{
    if (!g_mapOk)
        return;
    if (LocalPlayersNow() >= 2)
    {
        // 19g: player 1's map through layout 5 (off the screen on his side screen and soldier select)
        g_splitMapWant[0] = want == 2 ? 2 : (want == 1 ? 1 : 0);
        SplitMapLayout();
        return;
    }
    float* const ent[2] = { reinterpret_cast<float*>(g_mapTable + kMapLayout1),
                            reinterpret_cast<float*>(g_mapTable + 2 * kMapLayout1) };
    if (!g_mapLook.saved)
    {
        if (want == 0)
            return; // nothing has been changed
        memcpy(g_mapLook.orig[0], ent[0], sizeof(g_mapLook.orig[0]));
        memcpy(g_mapLook.orig[1], ent[1], sizeof(g_mapLook.orig[1]));
        g_mapLook.saved = true;
    }
    if (want == 2)
    {
        // tuning keys (spawn point screen)
        const bool l = (GetAsyncKeyState(VK_OEM_4) & 0x8000) != 0;      // [
        const bool r = (GetAsyncKeyState(VK_OEM_6) & 0x8000) != 0;      // ]
        const bool u = (GetAsyncKeyState(VK_PRIOR) & 0x8000) != 0;      // PageUp
        const bool d = (GetAsyncKeyState(VK_NEXT) & 0x8000) != 0;       // PageDown
        const bool sm = (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) != 0; // -
        const bool bg = (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) != 0;  // =
        bool tuned = false;
        if (l && !g_keyMapLeft)
            g_mapX -= 8.0f, tuned = true;
        if (r && !g_keyMapRight)
            g_mapX += 8.0f, tuned = true;
        if (u && !g_keyMapUp)
            g_mapY -= 8.0f, tuned = true;
        if (d && !g_keyMapDown)
            g_mapY += 8.0f, tuned = true;
        if (sm && !g_keyMapSmaller && g_mapSize > 0.3f)
            g_mapSize -= 0.02f, tuned = true;
        if (bg && !g_keyMapBigger && g_mapSize < 3.0f)
            g_mapSize += 0.02f, tuned = true;
        g_keyMapLeft = l;
        g_keyMapRight = r;
        g_keyMapUp = u;
        g_keyMapDown = d;
        g_keyMapSmaller = sm;
        g_keyMapBigger = bg;
        if (tuned && g_mapLook.lines++ < 80)
            Log("[wheel] map tuning: x offset %+.0f, y offset %+.0f, size %.2f", g_mapX, g_mapY, g_mapSize);
    }
    const uintptr_t map = ReadU32(g_mapArray); // player 1's map
    uint8_t flag = 0;
    if (map)
        ReadMem(map + kMapFlag, &flag, 1);
    const uint32_t mode = map ? ReadU32(map + kMapMode) : 0;
    // the layout the map is in (the game's own choice for 1 player): 2 with the flag, 1 in mode 1
    const int layout = (!map || LocalPlayersNow() >= 2) ? 0 : (flag ? 2 : (mode == 1 ? 1 : 0));
    bool changed = false;
    for (int k = 0; k < 2; ++k)
    {
        float desired[4];
        memcpy(desired, g_mapLook.orig[k], sizeof(desired));
        if (layout == k + 1 && want == 1)
            desired[2] = g_mapLook.orig[k][2] + kMapHideX;
        else if (layout == k + 1 && want == 2)
        {
            desired[0] = g_mapLook.orig[k][0] * g_mapSize;
            desired[1] = g_mapLook.orig[k][1] * g_mapSize;
            desired[2] = g_mapLook.orig[k][2] + g_mapX;
            desired[3] = g_mapLook.orig[k][3] + g_mapY;
        }
        if (memcmp(desired, ent[k], sizeof(desired)) != 0)
        {
            memcpy(ent[k], desired, sizeof(desired));
            changed = true;
        }
    }
    if (!changed)
        return;
    if (layout)
        g_mapSetMode(reinterpret_cast<void*>(map), nullptr, static_cast<int>(mode)); // re-applied by the map itself
    if (g_mapLook.lines++ < 80)
        Log("[wheel] %lu ms: map: %s - map 0x%08X mode %u flag %u = layout %d%s; layout 1 %.0f x %.0f %+.0f %+.0f, "
            "layout 2 %.0f x %.0f %+.0f %+.0f",
            ScanMsSinceAttach(), want == 1 ? "off the screen" : (want == 2 ? "moved left" : "as the game had it"),
            static_cast<unsigned>(map), mode, flag, layout, layout ? " (re-applied with SetMode)" : " (not a big layout)",
            ent[0][0], ent[0][1], ent[0][2], ent[0][3], ent[1][0], ent[1][1], ent[1][2], ent[1][3]);
}

// the layouts as the game had them, without SetMode (the spawn screen is going)
void MapRestoreTable()
{
    if (!g_mapOk)
        return;
    // 19g: the split screen layouts too (both players' spawn screens are going)
    if (g_splitMapSaved)
    {
        for (int p = 0; p < 2; ++p)
        {
            memcpy(reinterpret_cast<void*>(g_mapTable + kMapLayout1 * static_cast<uint32_t>(SplitLayoutOf(p))), g_splitMapOrig[p],
                   sizeof(g_splitMapOrig[p]));
            g_splitMapWant[p] = 0;
        }
    }
    if (!g_mapLook.saved)
        return;
    memcpy(reinterpret_cast<void*>(g_mapTable + kMapLayout1), g_mapLook.orig[0], sizeof(g_mapLook.orig[0]));
    memcpy(reinterpret_cast<void*>(g_mapTable + 2 * kMapLayout1), g_mapLook.orig[1], sizeof(g_mapLook.orig[1]));
}


void P1LookForget(uintptr_t sd); // 20g-r11 (below): player 1's look forgotten with its spawn screen

// the spawn screen's deleting destructor, through BF1GameExt first (game thread, the spawn screen
// still whole): the ring's soldiers are deleted, and the console flow starts at the side screen
volatile LONG g_sdDeletes = 0; // 18u: spawn screens deleted (one per match end) - player 2's flow tells matches apart by it

void* __fastcall SdDeleteHook(void* self, void* unusedEdx, int flags)
{
    const uintptr_t sd = reinterpret_cast<uintptr_t>(self);
    InterlockedIncrement(&g_sdDeletes);
    if (g_ring.sd == sd)
    {
        if (RingBudget())
            Log("[wheel] %lu ms: the spawn screen 0x%08X is being deleted - the ring goes first", ScanMsSinceAttach(),
                static_cast<unsigned>(sd));
        RingDeleteAll("the spawn screen is being deleted", 0);
        const int lines = g_ring.lines;
        g_ring = Ring();
        g_ring.lines = lines;
    }
    if (g_side.sd == sd)
    {
        SideClear("the spawn screen is being deleted", 0);
        g_side = Side();
    }
    P1LookForget(sd); // 20g-r11
    if (g_stx.sd == sd)
    {
        ScreenTextsClear("the spawn screen is being deleted", false);
        g_stx = ScreenTexts();
    }
    MapRestoreTable(); // 18m: the map's layout as the game had it
    if (g_drv.display == sd)
    {
        g_drv.display = 0;
        g_drv.title = 0;
        g_drv.box = 0;
        g_drv.postTitle = 0;
        g_drv.shownClass = 0;
        g_drv.shownPost = 0;
        g_drv.wasOn = false;
    }
    const LONG was = g_previewIdx;
    if ((was == 2 || was == 3) && InterlockedCompareExchange(&g_previewIdx, 1, was) == was && RingBudget())
        Log("[wheel] console flow: the next match starts at the side screen (ifs_readyselect1)");
    return g_origSdDelete(self, unusedEdx, flags);
}

// 18k: log-only scan for the side screen to come - the two teams' objects (looking for their
// names) and the spawn screen's own two text elements (+20h, +E0h). Once per spawn screen.
uintptr_t g_sideScanned = 0;

void LogTextAt(const char* what, uintptr_t va)
{
    char s[64] = {};
    wchar_t w[48] = {};
    if (va < 0x10000)
        return;
    if (ReadMem(va, s, sizeof(s) - 1))
    {
        int n = 0;
        while (n < 60 && s[n] >= 0x20 && s[n] < 0x7F)
            ++n;
        if (n >= 3 && s[n] == 0)
            Log("[wheel] side scan:   %s -> text \"%s\"", what, s);
    }
    if (ReadMem(va, w, sizeof(w) - sizeof(wchar_t)))
    {
        int n = 0;
        while (n < 46 && w[n] >= 0x20 && w[n] < 0x7F)
            ++n;
        if (n >= 3 && w[n] == 0)
        {
            char a[64] = {};
            for (int i = 0; i < n; ++i)
                a[i] = static_cast<char>(w[i]);
            Log("[wheel] side scan:   %s -> wide text \"%s\"", what, a);
        }
    }
}

void SideScreenScan(uintptr_t sd)
{
    if (g_sideScanned == sd)
        return;
    g_sideScanned = sd;
    const uintptr_t sm = SpawnScanManager();
    for (uint32_t t = 1; t <= 2 && sm; ++t)
    {
        const uintptr_t team = ReadU32(sm + kSmTeams + 4 * t);
        char type[64] = {};
        TypeName(team, type, sizeof(type));
        uint32_t f[16] = {};
        ReadMem(team, f, sizeof(f));
        Log("[wheel] side scan: team %u object 0x%08X (%s), first 40h bytes:", t, static_cast<unsigned>(team),
            type[0] ? type : "no type name");
        for (int r = 0; r < 4; ++r)
            Log("[wheel] side scan:   +%02X: %08X %08X %08X %08X", r * 16, f[r * 4], f[r * 4 + 1], f[r * 4 + 2],
                f[r * 4 + 3]);
        for (int i = 0; i < 16; ++i)
        {
            char what[32];
            snprintf(what, sizeof(what), "team %u +%02X", t, i * 4);
            LogTextAt(what, f[i]);                      // a pointer to a name
            LogTextAt(what, team + static_cast<uintptr_t>(i * 4)); // a name kept inside the object
        }
        // the team's class 0 (the side screen's model) and its name
        const uintptr_t arr = ReadU32(team + kTeamClasses);
        const uintptr_t cls0 = arr ? ReadU32(arr) : 0;
        char ct[64] = {};
        TypeName(cls0, ct, sizeof(ct));
        Log("[wheel] side scan: team %u class 0 = 0x%08X (%s)", t, static_cast<unsigned>(cls0), ct[0] ? ct : "?");
        LogTextAt("class 0 name (+30h)", ReadU32(cls0 + kClassName));
    }
    static const uint32_t kSdTexts[2] = { 0x20u, 0xE0u };
    for (const uint32_t off : kSdTexts)
    {
        const uintptr_t e = sd + off;
        char type[64] = {};
        TypeName(e, type, sizeof(type));
        uint32_t f[16] = {};
        ReadMem(e + 0x80, f, sizeof(f));
        Log("[wheel] side scan: spawn screen +%02Xh = %s, shown %d, x %.1f y %.1f; +80h..+BFh:", off,
            type[0] ? type : "?", (ReadU32(e + 0x14) & 0x100) ? 1 : 0, ReadF32(e + kElemX), ReadF32(e + kElemY));
        for (int r = 0; r < 4; ++r)
            Log("[wheel] side scan:   +%02X: %08X %08X %08X %08X", 0x80 + r * 16, f[r * 4], f[r * 4 + 1], f[r * 4 + 2],
                f[r * 4 + 3]);
        for (int i = 0; i < 16; ++i)
        {
            char what[40];
            snprintf(what, sizeof(what), "+%02Xh element +%02X", off, 0x80 + i * 4);
            LogTextAt(what, f[i]);
        }
    }
}

// the chooser's mode-1 "show the current soldier" call (every frame, after the hook at its entry):
// while the ring shows the selected class, the game's own soldier and droideka stay hidden
void __fastcall ChooserShowCurrent(void* elem, void* unusedEdx, int on)
{
    const uintptr_t e = reinterpret_cast<uintptr_t>(elem);
    const uintptr_t sd = g_ring.sd;
    if (on && g_ring.hidGame && sd && (e == sd + kSdSoldierElem || e == sd + kSdDroidekaElem))
        on = 0;
    // 18l: the side screen shows its own two soldiers
    if (on && g_side.hidGame && g_side.sd && (e == g_side.sd + kSdSoldierElem || e == g_side.sd + kSdDroidekaElem))
        on = 0;
    // 18k: the spawn point screen shows no soldier (Classic Collection)
    // 19i: nor the side screen, nor the class screen while the ring is on (it showed for the one frame
    // between the spawn point screen's A and the ring's first frame)
    const uintptr_t shown = g_drv.display;
    const LONG flow = g_previewIdx;
    const bool noSoldier = flow == 3 || (flow == 1 && g_sideOk) || (flow == 2 && g_ringOk);
    if (on && shown && noSoldier && g_drv.wasOn && (e == shown + kSdSoldierElem || e == shown + kSdDroidekaElem))
        on = 0;
    g_rf.setVisible(elem, unusedEdx, on);
}

// exactly what the PC Spawn button does: mode 2, then begin the spawn of this player's character
void DoSpawn(uintptr_t sd)
{
    const uintptr_t sm = SpawnScanManager();
    const int player = static_cast<int>(ReadU32(sd + kTailStart + 4 * kIdxPlayer));
    if (!sm || !g_charIndex || !g_character || !g_beginSpawn)
        return;
    const int index = g_charIndex(player);
    const uintptr_t ch = g_character(reinterpret_cast<void*>(sm), nullptr, index);
    if (DriverBudget())
        Log("[wheel] %lu ms: flow: A on the soldier select -> SPAWN (player %d, character index %d = 0x%08X)",
            ScanMsSinceAttach(), player, index, static_cast<unsigned>(ch));
    if (!ch)
        return;
    *reinterpret_cast<volatile uint32_t*>(sd + kMode) = 2;
    g_beginSpawn(reinterpret_cast<void*>(sd), nullptr, ch);
}

// the registry, live (a few reads per lookup)
uintptr_t LiveLookup(const char* path)
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

// wide text (with its 0) at va, at most cap - 1 characters
bool ReadWide(uintptr_t va, wchar_t* out, size_t cap)
{
    out[0] = 0;
    if (va < 0x10000 || cap < 2)
        return false;
    wchar_t buf[128] = {};
    SIZE_T got = 0;
    if (!ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(va), buf, sizeof(buf) - sizeof(wchar_t),
                           &got) &&
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

// 18p: the first place in the exe module (any readable section) holding these bytes (0 = none)
uintptr_t FindModuleBytes(const void* needle, size_t n)
{
    const BuildInfo* b = CurrentBuild();
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = lo + (b ? b->sizeOfImage : 0);
    const unsigned char* const want = static_cast<const unsigned char*>(needle);
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t addr = lo;
    while (addr < hi && VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)))
    {
        const uintptr_t regionLo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        uintptr_t regionHi = regionLo + mbi.RegionSize;
        if (regionHi > hi)
            regionHi = hi;
        const bool ok = mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) && !(mbi.Protect & PAGE_NOACCESS);
        if (ok && regionHi > addr + n)
        {
            const unsigned char* p = reinterpret_cast<const unsigned char*>(addr);
            const size_t len = regionHi - addr;
            for (size_t i = 0; i + n <= len; ++i)
                if (p[i] == want[0] && memcmp(p + i, want, n) == 0)
                    return addr + i;
        }
        addr = regionHi;
    }
    return 0;
}

bool InExeCodeVa(uintptr_t va)
{
    uint32_t rva = 0;
    uint32_t size = 0;
    return va >= ExeBase() && CodeSection(&rva, &size) && va - ExeBase() >= rva && va - ExeBase() < rva + size;
}

bool InExeCode(uintptr_t va)
{
    uint32_t rva = 0;
    uint32_t size = 0;
    return va >= ExeBase() && CodeSection(&rva, &size) && va - ExeBase() >= rva && va - ExeBase() < rva + size;
}

// the selected class of the team the spawn screen shows, and how many classes the PC cycle has
uintptr_t SelectedClass(uintptr_t sd, int* index, int* cycle)
{
    *index = 0;
    *cycle = 0;
    const uintptr_t sm = SpawnScanManager();
    const uint32_t teamNo = ReadU32(sd + kTailStart + 4 * kIdxTeamShown);
    if (!sm || teamNo >= static_cast<uint32_t>(kMaxTeams))
        return 0;
    const uintptr_t team = ReadU32(sm + kSmTeams + 4 * teamNo);
    if (!team)
        return 0;
    const uint32_t count = ReadU32(team + kTeamCount);
    const uint32_t extra = ReadU32(team + kTeamCount + 4);
    const uint32_t arr = ReadU32(team + kTeamClasses);
    const uint32_t idx = ReadU32(sd + kTailStart + 4 * kIdxClassIndex);
    if (!arr || count > kMaxClasses || extra > count || idx >= count)
        return 0;
    *index = static_cast<int>(idx);
    *cycle = static_cast<int>(count - extra);
    return ReadU32(arr + 4 * idx);
}

void ShowClassTexts(uintptr_t cls)
{
    char type[64];
    if (!TypeName(cls, type, sizeof(type)) ||
        (strcmp(type, "EntitySoldierClass") != 0 && strcmp(type, "EntityDroidekaClass") != 0))
    {
        if (DriverBudget())
            Log("[wheel] charselect: class 0x%08X is a %s - texts left as they are", static_cast<unsigned>(cls),
                type[0] ? type : "?");
        return;
    }
    wchar_t name[96];
    if (!ReadWide(ReadU32(cls + kClassName), name, 96))
        wcscpy_s(name, L"?");
    wchar_t box[600];
    wcscpy_s(box, L"EQUIPMENT:");
    int weapons = 0;
    const uintptr_t vt = ReadU32(cls);
    const uintptr_t fnCount = vt ? ReadU32(vt + kWeaponCountSlot) : 0;
    const uintptr_t fnGet = vt ? ReadU32(vt + kWeaponGetSlot) : 0;
    if (InExeCode(fnCount) && InExeCode(fnGet))
    {
        int n = reinterpret_cast<WeaponCount_t>(fnCount)(reinterpret_cast<void*>(cls), nullptr);
        if (n < 0 || n > 16)
            n = 0;
        for (int k = 0; k < n; ++k)
        {
            const uintptr_t w = reinterpret_cast<WeaponGet_t>(fnGet)(reinterpret_cast<void*>(cls), nullptr, k);
            wchar_t wn[96];
            if (!w || !ReadWide(ReadU32(w + kClassName), wn, 96))
                continue;
            if (wcslen(box) + wcslen(wn) + 2 >= 600)
                break;
            wcscat_s(box, L"\n");
            wcscat_s(box, wn);
            ++weapons;
        }
    }
    g_setTextW(reinterpret_cast<void*>(g_drv.title), nullptr, name);
    g_setTextW(reinterpret_cast<void*>(g_drv.box), nullptr, box);
    if (DriverBudget())
    {
        char a[200] = "";
        WideCharToMultiByte(CP_UTF8, 0, name, -1, a, sizeof(a), nullptr, nullptr);
        Log("[wheel] %lu ms: charselect: class 0x%08X (%s) - title \"%s\", EQUIPMENT: %d weapon(s)", ScanMsSinceAttach(),
            static_cast<unsigned>(cls), type, a, weapons);
    }
}

// ---- 20g-r11: PLAYER 1'S SPAWN SCREENS IN PLAYERS 3 / 4'S LOOK (4-player, MenusLikeP34=1) -----------------------------
// r10 drew his side screen as theirs; now his spawn point and class screens too (r10's video: his map was the Xbox's
// half-screen place, big and over his score bar, the panels off his quarter's bottom; his class ring right of centre
// with the class window's orange bar under it). split_spawn_menus.cpp draws all three in its group in his HUD root, exactly as
// players 3 / 4's (ClassWheelP1Look gives it the screen, side, post and class). Here, while that look is on: his console
// screens still run (left / right / A / B, CyclePost, SelectClass, the spawn), but the spawn point screen's SideBar
// panels and "B BACK", the class screen's window and "B BACK", our own screen texts and his ring are not shown; the game's
// own soldier stays hidden on the class screen; the spawn screen's blue grid is hidden (the look has its own at his
// quarter's centre); and his map (layout 5) stands where players 3 / 4's stand in their quarters.
struct P1Look
{
    LONG screen = 0;          // the console flow's screen (1 side, 3 spawn point, 2 class) while the look is on, else 0
    DWORD at = 0;             // its last frame
    int team = 0;
    uintptr_t post = 0;
    int cls = 0;
    uintptr_t sd = 0;
    bool hidCube = false;     // the spawn screen's blue grid hidden by the look (shown again after, while still choosing)
    unsigned hidMask = 0;     // the console screens' groups the look hid (shown again when it ends): kP1Hide* bits
    int lines = 0;
};
P1Look g_p1Look;
uintptr_t g_cubePendingSd = 0; // the look ended while that spawn screen was not choosing: its grid shown again next time it is
const char* const kP1HidePaths[4] = { "ifs_mapselect1.SideBar", "ifs_mapselect1.Helptext_Back", "ifs_charselect1.Info",
                                      "ifs_charselect1.Helptext_Back" };

bool P1LookOn()
{
    return QuarterLook() && g_sideLookAllowed;
}

void P1LookHide(int i)
{
    const uintptr_t e = LiveLookup(kP1HidePaths[i]);
    if (e && g_rf.setVisible)
    {
        g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 0);
        g_p1Look.hidMask |= 1u << i;
    }
}

void P1LookCube(uintptr_t sd)
{
    if (!g_cubeOk || !sd || !g_rf.setVisible)
        return;
    const uintptr_t cube = sd + kSdCube;
    if (!(ReadU32(cube + 0x14) & 0x100))
        return;
    if (!g_p1Look.hidCube && g_p1Look.lines++ < 20)
        Log("[wheel] %lu ms: players 3 / 4's look: the spawn screen's blue grid 0x%08X hidden (the look has its own)", ScanMsSinceAttach(),
            static_cast<unsigned>(cube));
    g_p1Look.hidCube = true;
    g_p1Look.sd = sd;
    if (g_cubePendingSd == sd)
        g_cubePendingSd = 0; // (hidden by the look again: its own restore at its end)
    g_rf.setVisible(reinterpret_cast<void*>(cube), nullptr, 0);
}

// the look ends: the console screens' groups it hid shown again; the grid shown again while the spawn screen is choosing
// (else the next time it is, outside the look - g_cubePendingSd)
void P1LookEnd(uintptr_t sd, const char* why)
{
    if (g_cubePendingSd && g_cubePendingSd == sd && ReadU32(sd + kMode) == 1 && g_rf.setVisible)
    {
        g_rf.setVisible(reinterpret_cast<void*>(sd + kSdCube), nullptr, 1);
        g_cubePendingSd = 0;
        if (g_p1Look.lines++ < 20)
            Log("[wheel] %lu ms: the spawn screen's blue grid shown again (players 3 / 4's look ended before)", ScanMsSinceAttach());
    }
    if (!g_p1Look.screen && !g_p1Look.hidCube && !g_p1Look.hidMask)
        return;
    const bool alive = sd && g_p1Look.sd == sd && g_rf.setVisible;
    const bool restore = g_p1Look.hidCube && alive && ReadU32(sd + kMode) == 1;
    if (restore)
        g_rf.setVisible(reinterpret_cast<void*>(sd + kSdCube), nullptr, 1);
    else if (g_p1Look.hidCube && alive)
        g_cubePendingSd = sd;
    int shown = 0;
    for (int i = 0; i < 4 && g_rf.setVisible; ++i)
    {
        if (!(g_p1Look.hidMask & (1u << i)))
            continue;
        const uintptr_t e = LiveLookup(kP1HidePaths[i]);
        if (e)
        {
            g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
            ++shown;
        }
    }
    if (g_p1Look.lines++ < 20)
        Log("[wheel] %lu ms: players 3 / 4's look for player 1's spawn screens off (%s)%s; %d console group(s) shown again",
            ScanMsSinceAttach(), why,
            restore ? " - the spawn screen's blue grid shown again" : (g_cubePendingSd ? " - its grid shown when it chooses again" : ""),
            shown);
    const int lines = g_p1Look.lines;
    g_p1Look = P1Look();
    g_p1Look.lines = lines;
}

// the spawn screen is being deleted (the match ends): the look forgotten, nothing touched
void P1LookForget(uintptr_t sd)
{
    if (g_cubePendingSd == sd)
        g_cubePendingSd = 0;
    if (g_p1Look.sd == sd || g_p1Look.screen)
    {
        const int lines = g_p1Look.lines;
        g_p1Look = P1Look();
        g_p1Look.lines = lines;
    }
}

void P1LookRecord(uintptr_t sd, LONG flow)
{
    if (g_p1Look.screen != flow && g_p1Look.lines++ < 20)
        Log("[wheel] %lu ms: players 3 / 4's look: player 1's %s screen (drawn by split_spawn_menus.cpp)", ScanMsSinceAttach(),
            flow == 1 ? "side" : (flow == 3 ? "spawn point" : "class"));
    g_p1Look.screen = flow;
    g_p1Look.at = GetTickCount();
    g_p1Look.team = static_cast<int>(ReadU32(sd + kTailStart + 4 * kIdxTeamShown));
    g_p1Look.post = ReadU32(sd + kSdPost);
    int index = 0;
    int cycle = 0;
    SelectedClass(sd, &index, &cycle);
    g_p1Look.cls = index;
}

// his class screen in the look: no ring of ours in the spawn screen, and the game's own soldier kept hidden (as the ring does)
void RingLookHide(uintptr_t sd)
{
    if (g_ring.sd != sd)
    {
        const int lines = g_ring.lines;
        g_ring = Ring();
        g_ring.lines = lines;
        g_ring.sd = sd;
    }
    bool any = false;
    for (int i = 0; i < kRingMax; ++i)
        any = any || g_ring.elem[i] != 0;
    if (any)
        RingDeleteAll("players 3 / 4's look (split_spawn_menus.cpp draws his ring)", sd);
    g_rf.setVisible(reinterpret_cast<void*>(sd + kSdSoldierElem), nullptr, 0);
    if (g_rdOk)
        g_rf.setVisible(reinterpret_cast<void*>(sd + kSdDroidekaElem), nullptr, 0);
    g_ring.hidGame = true;
    g_ring.placed = 0;
}

} // namespace

bool ClassWheelMoveElement(uintptr_t e, float x, float y, const char* what); // 18z, defined below
bool ClassWheelLayoutTryNow(int tries);                                                                         // 19h
constexpr int kLayoutTries = 30;                                                                                // 19h
bool ClassWheelXbPanels(int player, uintptr_t sideBar, const uintptr_t* panel, const uintptr_t* title, const uintptr_t* skin,
                        const char* who, bool lastTry);                                                                     // 19h
bool ClassWheelXbWindow(int player, uintptr_t info, uintptr_t window, uintptr_t title, uintptr_t skin, const char* who,
                        bool lastTry); // 19h

void ClassWheelFrame(uintptr_t sd, const char* screen)
{
    const bool on = screen && ConsoleActive() && g_setTextW && g_selectClass && g_registry &&
                    ReadU32(sd + kMode) == 1;
    if (!on)
    {
        P1LookEnd(sd, "the console flow is not on"); // 20g-r11
        RingClear("the soldier select is not up", sd);
        SideClearFor("the side screen is not up", sd);
        MapUpdate(0); // 18m: the map as the game had it
        if (g_stx.sd == sd && g_stx.n)
            ScreenTextsClear("the console flow is not on", true);
        if (g_drv.wasOn && DriverBudget())
            Log("[wheel] %lu ms: console flow idle (screen: %s)", ScanMsSinceAttach(), screen ? screen : "PC");
        g_drv.wasOn = false;
        return;
    }
    // nothing is touched until the spawn screen is set up (a new match's first frames)
    const uint32_t teamShown = ReadU32(sd + kTailStart + 4 * kIdxTeamShown);
    // v6.8: teams 1 - 7 (Mashup Conquest's sides 3 - 7 too - the flow stopped dead on the spawn point screen for them)
    if (teamShown < 1 || teamShown > 7 || !ReadU32(sd + kTailStart + 4 * kIdxCharacter))
        return;
    const LONG flow = g_previewIdx;
    // 20g-r12: horizontal - his half's HUD root gets Classic's safe area before anything of his is placed this frame (the
    // menu layer sets it too, but after this frame's spawn screen update); the mouse is not his there (its boxes are the
    // vertical halves')
    const bool hzNow = ClassWheelHorizontal();
    if (hzNow)
        P2SpawnHorizontalRoots();
    // 20g-r11: 4-player - his three screens drawn by split_spawn_menus.cpp in players 3 / 4's look (P1Look above)
    const bool look = P1LookOn() && g_ringOk && (flow == 1 || flow == 2 || flow == 3);
    if (!look && (g_p1Look.screen || g_p1Look.hidCube || g_p1Look.hidMask || g_cubePendingSd == sd))
        P1LookEnd(sd, "players 3 / 4's look is off");
    if (flow != 2)
        RingClear("left the soldier select", sd);
    if (flow != 1)
        SideClearFor("left the side screen", sd);
    g_splitMapLook = look; // 20g-r11: his map in players 3 / 4's place (SplitMapLayout)
    MapUpdate(flow == 3 ? 2 : 1); // 18m: spawn point screen: moved left; side screen / soldier select: off
    // the console screens are rebuilt every match (often at the same addresses as everything else):
    // their elements are looked up every frame, never kept from an earlier one
    const uintptr_t title = LiveLookup("ifs_charselect1.Info.Window.titleBarElement");
    const uintptr_t box = LiveLookup("ifs_charselect1.Info.Window.BoxText");
    const uintptr_t postTitle = LiveLookup("ifs_mapselect1.SideBar.Spawnpoint.titleBarElement");
    if (sd != g_drv.display || title != g_drv.title || box != g_drv.box || postTitle != g_drv.postTitle)
    {
        g_drv.display = sd;
        g_drv.title = title;
        g_drv.box = box;
        g_drv.postTitle = postTitle;
        g_drv.shownPost = 0;
        g_drv.shownClass = 0;
        g_drv.panelsMoved = false; // 18z: a new spawn screen - its screens' panels again
        g_drv.windowMoved = false;
        g_drv.panelTries = 0;
        g_drv.windowTries = 0;
        g_drv.backLogged = false;
        g_drv.panelsPinned = false;
        g_drv.windowPinned = false;
        g_drv.sideBackHidden = false;
        if (DriverBudget())
            Log("[wheel] %lu ms: console elements: charselect titleBarElement 0x%08X, BoxText 0x%08X, mapselect post "
                "title 0x%08X%s",
                ScanMsSinceAttach(), static_cast<unsigned>(title), static_cast<unsigned>(box),
                static_cast<unsigned>(postTitle), (title && box) ? "" : " - MISSING, texts off");
    }
    if (!g_drv.wasOn)
    {
        g_drv.lastFrame = 0; // 21h-r6: (the latch below covers the flow's start)
        SideScreenScan(sd); // 18k: log-only, once per spawn screen
        ShellSoundScan();   // 18p: log-only, once
        {
            // 18p: a button (or mouse button) already held when the flow comes up does not count as a press
            const FlowInput held = ReadFlowInput();
            g_drv.aDown = held.a;
            g_drv.bDown = held.b;
            g_drv.clickDown = held.click;
        }
        g_drv.shownClass = 0; // set the texts again each time the flow comes up
        g_drv.shownPost = 0;
        if (DriverBudget())
            Log("[wheel] %lu ms: console flow ON at %s (pad or keyboard: left / right = class, A / Enter = on, "
                "B / Backspace = back; mouse: %s, BACK to go back, right button = back)",
                ScanMsSinceAttach(), screen,
                g_mouseClicks ? "21j-r19 - click a side, a post on the map, the wheel left / right of the picked soldier, "
                                "SELECT SPAWNPOINT / SELECT CHARACTER to go on"
                              : "click a soldier to pick it, the front one / the prompt to go on");
    }
    g_drv.wasOn = true;
    {
        // 21h-r6: back after a gap (the game's pause stops his flow): the buttons held now are not presses (the A / B that
        // closed the pause menu)
        const DWORD nowTick = GetTickCount();
        const DWORD gap = g_drv.lastFrame ? nowTick - g_drv.lastFrame : 0;
        g_drv.lastFrame = nowTick;
        if (g_pauseLatch && gap >= kFlowGapMs)
        {
            const FlowInput held = ReadFlowInput();
            g_drv.aDown = held.a;
            g_drv.bDown = held.b;
            g_drv.clickDown = held.click;
            if (g_gapLines < 40)
            {
                ++g_gapLines;
                Log("[wheel] %lu ms: his flow is back after %lu ms (a pause) at %s - buttons latched (A %s, B %s): only a new press "
                    "counts",
                    ScanMsSinceAttach(), static_cast<unsigned long>(gap), screen, held.a ? "held" : "up", held.b ? "held" : "up");
            }
        }
    }

    ReadMouse();                 // 18q: the cursor, in the soldiers' units (before the texts: the prompt lights up)
    if (!look)
        ScreenTextsUpdate(sd, flow); // 18o: the spawn point screen's and the soldier select's texts
    else
    {
        // 20g-r11: none of ours in players 3 / 4's look (the look has its own)
        if (g_stx.sd == sd && g_stx.flow)
            ScreenTextsClear("players 3 / 4's look", true);
        P1LookCube(sd);
        P1LookRecord(sd, flow);
    }
    const FlowInput in = ReadFlowInput();
    const bool left = Step(in.left, g_drv.leftDown, g_drv.leftSince, g_drv.leftLast);
    const bool right = Step(in.right, g_drv.rightDown, g_drv.rightSince, g_drv.rightLast);
    const bool a = Press(in.a, g_drv.aDown);
    bool b = Press(in.b, g_drv.bDown);
    const bool up = Step(in.up, g_drv.upDown, g_drv.upSince, g_drv.upLast);
    const bool down = Step(in.down, g_drv.downDown, g_drv.downSince, g_drv.downLast);
    bool click = Press(in.click, g_drv.clickDown);
    if (hzNow)
        click = false; // 20g-r12
    if (click && MouseLines())
        Log("[wheel] %lu ms: mouse: click at %.0f, %.0f px of %d x %d (%s) = %.0f, %.0f units of %.0f x %.0f%s%s",
            ScanMsSinceAttach(), g_mouse.px, g_mouse.py, g_mouse.dw, g_mouse.dh,
            g_mouse.ok ? (g_mouse.game ? "the game's cursor" : "the Windows cursor") : "NO CURSOR READ", g_mouse.x,
            g_mouse.y, g_mouse.w, RefH(), MouseOnBack() ? " - on BACK" : "", MouseOnPrompt() ? " - on the prompt" : "");
    if (click && MouseOnBack()) // 18q: BACK (bottom left) = B
    {
        b = true;
        click = false;
    }
    // v6.6 (p25b): a click on the score bar at the top of the spawn point / soldier screens = CHOOSE YOUR SIDE (the PC spawn
    // screen switched sides with its side icons up there; the console flow only had B / BACK on the spawn point screen)
    bool sideClick = false;
    if (click && (flow == 2 || flow == 3) && g_mouse.ok && g_mouse.y < RefH() * 0.17f &&
        std::fabs(g_mouse.x - g_mouse.w * 0.5f) < g_mouse.w * 0.25f)
    {
        sideClick = true;
        click = false;
    }

    // 21f: sides locked: no side screen (ClassWheelPreviewScreenImpl moves the flow on before it is shown; this is the
    // frame it may already be up)
    if (flow == 1 && SidesLockedNow() == 1)
    {
        SetFlow(3, "sides are locked (campaign / Galactic Conquest: the game's side) - no side screen");
        return;
    }
    if (sideClick)
    {
        if (SidesLockedNow() == 1)
        {
            if (DriverBudget())
                Log("[wheel] %lu ms: flow: click on the score bar - sides are locked, no side screen", ScanMsSinceAttach());
        }
        else
        {
            PlayUiSound(g_sndBack);
            SetFlow(1, "click on the score bar (change side)");
        }
        return;
    }
    // 18j: the console order - side (1) -> spawn point (3) -> soldier select (2) -> spawn
    if (flow == 1) // ifs_readyselect1: the side screen (18l: its title, the two sides, their names)
    {
        if (!g_drv.sideBackHidden && LocalPlayersNow() >= 2) // 19g: the Xbox's side screen has no "B BACK"
        {
            g_drv.sideBackHidden = true;
            const uintptr_t back = LiveLookup("ifs_readyselect1.Helptext_Back");
            if (back && g_rf.setVisible)
                g_rf.setVisible(reinterpret_cast<void*>(back), nullptr, 0);
            if (DriverBudget())
                Log("[wheel] %lu ms: layout: ifs_readyselect1.Helptext_Back 0x%08X %s (split screen)", ScanMsSinceAttach(),
                    static_cast<unsigned>(back), back ? "hidden" : "MISSING");
        }
        const int side = SideUpdate(sd, left, right, a, click);
        if (left || right)
            PlayUiSound(g_sndTick);
        if (side > 0 || (side < 0 && (a || click)))
        {
            PlayUiSound(g_sndSelect);
            SetFlow(3, side > 0 ? "A on the side screen (side joined)" : "A on the side screen");
        }
        return;
    }
    if (flow == 3) // ifs_mapselect1: the spawn point screen
    {
        if (look) // 20g-r11: players 3 / 4's look - its panels and "B BACK" not shown (every frame: its script shows them)
        {
            P1LookHide(0); // ifs_mapselect1.SideBar
            P1LookHide(1); // ifs_mapselect1.Helptext_Back
        }
        else if (!g_drv.panelsMoved) // 18z: the panels where the Xbox has them (19g: split screen only)
        {
            ++g_drv.panelTries;
            if (LocalPlayersNow() < 2)
                g_drv.panelsMoved = true;
            else if (ClassWheelHorizontal()) // 20g-r12: Classic's horizontal split has the game's own (turned) panels
            {
                g_drv.panelsMoved = true;
                if (HzBudget())
                    Log("[wheel] %lu ms: layout: horizontal - ifs_mapselect1's panels left as the game lays them out (Classic's)",
                        ScanMsSinceAttach());
            }
            else
            {
                const uintptr_t panels[3] = { LiveLookup("ifs_mapselect1.SideBar.Spawnpoint"), LiveLookup("ifs_mapselect1.SideBar.Vehicle"),
                                              LiveLookup("ifs_mapselect1.SideBar.Objectives") };
                if (!g_drv.panelsPinned)
                {
                    // 19h-r2: 19g's places first (the group pinned; the game's own layout slides in otherwise), on
                    // every frame until they are all there; the flat layout over them below
                    const uintptr_t bar = LiveLookup("ifs_mapselect1.SideBar");
                    g_drv.panelsPinned = bar && panels[0] && panels[1] && panels[2];
                    ClassWheelMoveElement(bar, 0.0f, 0.0f, "SideBar group");
                    ClassWheelMoveElement(panels[0], kXbPanelX, kXbPostY, "COMMAND POST panel");
                    ClassWheelMoveElement(panels[1], kXbPanelX, kXbVehY, "VEHICLES panel");
                    ClassWheelMoveElement(panels[2], kXbPanelX, kXbObjY, "OBJECTIVES panel");
                }
                if (ClassWheelLayoutTryNow(g_drv.panelTries))
                {
                    // 19h: flat, from the title bars - tried on a few frames (its elements can come a frame late)
                    const uintptr_t titles[3] = { LiveLookup("ifs_mapselect1.SideBar.Spawnpoint.titleBarElement"),
                                                  LiveLookup("ifs_mapselect1.SideBar.Vehicle.titleBarElement"),
                                                  LiveLookup("ifs_mapselect1.SideBar.Objectives.titleBarElement") };
                    const uintptr_t skins[3] = { LiveLookup("ifs_mapselect1.SideBar.Spawnpoint.Skin"), LiveLookup("ifs_mapselect1.SideBar.Vehicle.Skin"),
                                                 LiveLookup("ifs_mapselect1.SideBar.Objectives.Skin") };
                    const bool flat = ClassWheelXbPanels(0, LiveLookup("ifs_mapselect1.SideBar"), panels, titles, skins, "player 1's",
                                                         g_drv.panelTries >= kLayoutTries);
                    if (flat || g_drv.panelTries >= kLayoutTries)
                        g_drv.panelsMoved = true;
                    if (!flat && g_drv.panelsMoved && DriverBudget())
                        Log("[wheel] %lu ms: flat layout: ifs_mapselect1's panels stay as 19g had them (no title found in %d tries)",
                            ScanMsSinceAttach(), g_drv.panelTries);
                }
            }
        }
        if (LocalPlayersNow() >= 2)
        {
            // 19h: the Xbox's spawn point screen has no "B BACK": its icon and label hidden every frame the
            // screen is up (its script may show them each time it comes up; B still goes back)
            const uintptr_t backIcon = LiveLookup("ifs_mapselect1.Helptext_Back.Icon");
            const uintptr_t backBtn = LiveLookup("ifs_mapselect1.Helptext_Back.btn");
            if (g_rf.setVisible)
            {
                if (backIcon)
                    g_rf.setVisible(reinterpret_cast<void*>(backIcon), nullptr, 0);
                if (backBtn)
                    g_rf.setVisible(reinterpret_cast<void*>(backBtn), nullptr, 0);
            }
            if (!g_drv.backLogged && DriverBudget())
                Log("[wheel] %lu ms: layout: ifs_mapselect1's \"B BACK\" (icon 0x%08X, label 0x%08X) hidden (split screen)",
                    ScanMsSinceAttach(), static_cast<unsigned>(backIcon), static_cast<unsigned>(backBtn));
            g_drv.backLogged = true;
        }
        ShowPostName(ReadU32(sd + kSdPost)); // the current post's name, once per post
        if (left || up)
            CyclePost(sd, -1);
        else if (right || down)
            CyclePost(sd, +1);
        if (left || up || right || down)
            PlayUiSound(g_sndTick);
        if (look)
            P1LookRecord(sd, flow); // (the post just picked)
        // 21j-r19 ([ClassWheel] MouseClicks=1, the Classic Collection's): a click on one of his team's posts on the map picks it,
        // a click on "SELECT SPAWNPOINT" goes on to the soldier select, anywhere else nothing. (MouseClicks=0, and players 3 / 4's
        // look - no prompt of ours there: a click anywhere but BACK goes on, as before.)
        bool go = a;
        const char* goWhy = "A on the spawn point screen";
        if (click && (!g_mouseClicks || look))
        {
            go = true;
            goWhy = "click on the spawn point screen";
        }
        else if (click)
        {
            const uintptr_t post = MouseOnPrompt() ? 0 : PostUnderMouse(sd);
            if (MouseOnPrompt())
            {
                go = true;
                goWhy = "click on SELECT SPAWNPOINT";
            }
            else if (post)
            {
                const uintptr_t was = ReadU32(sd + kSdPost);
                if (post != was)
                {
                    g_setPost(reinterpret_cast<void*>(sd), nullptr, post);
                    PlayUiSound(g_sndTick);
                    ShowPostName(ReadU32(sd + kSdPost));
                }
                wchar_t name[96];
                char an[200] = "?";
                if (ReadWide(ReadU32(post + kPostName), name, 96))
                    WideCharToMultiByte(CP_UTF8, 0, name, -1, an, sizeof(an), nullptr, nullptr);
                if (ClickLines())
                    Log("[wheel] %lu ms: mouse: click on the map's post \"%s\" (0x%08X) at %.0f, %.0f px - %s (now at +23E0h 0x%08X)",
                        ScanMsSinceAttach(), an, static_cast<unsigned>(post), g_mouse.px, g_mouse.py,
                        post != was ? "picked" : "already the picked one", static_cast<unsigned>(ReadU32(sd + kSdPost)));
            }
            else if (ClickLines())
                Log("[wheel] %lu ms: mouse: click on the spawn point screen on nothing (no post of his team there%s) - nothing done",
                    ScanMsSinceAttach(), g_postAt ? "" : "; the map's post test is OFF on this build");
        }
        if (go)
        {
            PlayUiSound(g_sndSelect);
            SetFlow(2, goWhy);
        }
        else if (b)
        {
            if (SidesLockedNow() == 1) // 21f: no side screen to go back to
            {
                if (DriverBudget())
                    Log("[wheel] %lu ms: flow: B on the spawn point screen - sides are locked, no side screen to go back to",
                        ScanMsSinceAttach());
            }
            else
            {
                PlayUiSound(g_sndBack);
                SetFlow(1, "B on the spawn point screen");
            }
        }
        return;
    }

    // flow == 2, ifs_charselect1: the soldier select
    if (look) // 20g-r11: players 3 / 4's look - its window (the orange class bar) and "B BACK" not shown
    {
        P1LookHide(2); // ifs_charselect1.Info
        P1LookHide(3); // ifs_charselect1.Helptext_Back
    }
    else if (!g_drv.windowMoved) // 18z: the class window centred under the soldiers (19g: split screen only)
    {
        ++g_drv.windowTries;
        if (LocalPlayersNow() < 2)
            g_drv.windowMoved = true;
        else if (ClassWheelHorizontal()) // 20g-r12: Classic's horizontal split has the game's own (turned) window and B BACK
        {
            g_drv.windowMoved = true;
            if (HzBudget())
                Log("[wheel] %lu ms: layout: horizontal - ifs_charselect1's class window and \"B BACK\" left as the game lays them "
                    "out (Classic's)", ScanMsSinceAttach());
        }
        else
        {
            if (!g_drv.windowPinned)
            {
                // 19h-r2: 19g's places first (the group pinned), on every frame until they are all there; the
                // flat layout over them below
                const uintptr_t back = LiveLookup("ifs_charselect1.Helptext_Back");
                const uintptr_t info = LiveLookup("ifs_charselect1.Info");
                const uintptr_t window = LiveLookup("ifs_charselect1.Info.Window");
                g_drv.windowPinned = back && info && window;
                ClassWheelMoveElement(back, kXbBackX, kXbBackY, "B BACK (class screen)");
                ClassWheelMoveElement(info, 0.0f, 0.0f, "Info group");
                ClassWheelMoveElement(window, kXbWindowX, kXbWindowY, "class window");
            }
            // 19h: flat, from the title bar - tried on a few frames
            if (ClassWheelLayoutTryNow(g_drv.windowTries))
            {
                const bool flat = ClassWheelXbWindow(0, LiveLookup("ifs_charselect1.Info"), LiveLookup("ifs_charselect1.Info.Window"),
                                                     LiveLookup("ifs_charselect1.Info.Window.titleBarElement"),
                                                     LiveLookup("ifs_charselect1.Info.Window.Skin"), "player 1's",
                                                     g_drv.windowTries >= kLayoutTries);
                if (flat || g_drv.windowTries >= kLayoutTries)
                    g_drv.windowMoved = true;
                if (!flat && g_drv.windowMoved && DriverBudget())
                    Log("[wheel] %lu ms: flat layout: ifs_charselect1's window stays as 19g had it (no title found in %d tries)",
                        ScanMsSinceAttach(), g_drv.windowTries);
            }
        }
    }
    int index = 0;
    int cycle = 0;
    const uintptr_t cls = SelectedClass(sd, &index, &cycle);
    // 18q: a click on a soldier of the ring picks that class; on the front (picked) one, or on
    // "SELECT CHARACTER", it spawns; anywhere else it does nothing
    bool spawnClick = false;
    bool wheelLeft = false;
    bool wheelRight = false;
    if (click && g_mouseClicks && !look)
    {
        // 21j-r19 ([ClassWheel] MouseClicks=1, the Classic Collection's): a click on the wheel left of the picked soldier turns it
        // left, right of it right (one class, as the left / right keys); "SELECT CHARACTER" spawns; anywhere else nothing
        const int side = MouseOnPrompt() ? 0 : RingSideAt(sd);
        if (MouseOnPrompt())
            spawnClick = true;
        else if (side < 0)
            wheelLeft = true;
        else if (side > 0)
            wheelRight = true;
        if (ClickLines())
            Log("[wheel] %lu ms: mouse: click on the soldier select at %.0f, %.0f units (the picked soldier at %.0f, the wheel %.0f "
                "- %.0f across, %.0f - %.0f down) - %s",
                ScanMsSinceAttach(), g_mouse.x, g_mouse.y, g_ring.pickX, g_ring.span[0], g_ring.span[2], g_ring.span[1],
                g_ring.span[3], spawnClick ? "SELECT CHARACTER: spawn" : (wheelLeft ? "the wheel's left: turn left"
                                                                    : (wheelRight ? "the wheel's right: turn right" : "nothing")));
    }
    else if (click)
    {
        const int hit = RingHit(sd);
        if (hit >= 0 && hit != index && cycle > 0 && hit < cycle)
        {
            if (DriverBudget())
                Log("[wheel] %lu ms: charselect: click on class %d of the ring -> SelectClass(%d) (was %d of %d)",
                    ScanMsSinceAttach(), hit, hit, index, cycle);
            g_selectClass(reinterpret_cast<void*>(sd), nullptr, hit);
            PlayUiSound(g_sndTick);
        }
        else if ((hit >= 0 && hit == index && RingAtRest()) || (!look && MouseOnPrompt())) // (20g-r11: no prompt of ours in the look)
            spawnClick = true;
        else if (DriverBudget())
            Log("[wheel] %lu ms: charselect: click on nothing (ring: %d class(es) placed)", ScanMsSinceAttach(),
                g_ring.placed);
    }
    if (cycle > 0 && (left || right || wheelLeft || wheelRight))
    {
        const bool toLeft = left || wheelLeft;
        const int to = toLeft ? (index + cycle - 1) % cycle : (index + 1) % cycle;
        if (DriverBudget())
            Log("[wheel] %lu ms: charselect: %s -> SelectClass(%d) (was %d of %d)", ScanMsSinceAttach(),
                toLeft ? (wheelLeft ? "click left" : "left") : (wheelRight ? "click right" : "right"), to, index, cycle);
        g_selectClass(reinterpret_cast<void*>(sd), nullptr, to);
        PlayUiSound(g_sndTick);
    }
    if (look)
    {
        RingLookHide(sd); // 20g-r11: (after SelectClass) his ring is the look's; the game's own soldier hidden again
        P1LookRecord(sd, flow);
    }
    else
        RingUpdate(sd); // after SelectClass, so the game's own soldier ShowClass just showed is hidden again
    if (cls && cls != g_drv.shownClass && g_drv.title && g_drv.box)
    {
        g_drv.shownClass = cls;
        ShowClassTexts(cls);
    }
    if (a || spawnClick)
    {
        PlayUiSound(g_sndSelect);
        DoSpawn(sd);
        SetFlow(3, spawnClick ? "click on the picked soldier / SELECT CHARACTER - spawned; the next spawn screen starts "
                                "at the spawn point screen"
                              : "spawned - the next spawn screen starts at the spawn point screen");
    }
    else if (b)
    {
        PlayUiSound(g_sndBack);
        SetFlow(3, "B on the soldier select");
    }
}

namespace {

// 18i-r2: is this screen loaded right now? (walks the live screen list, read only; game thread)
bool ScreenLoaded(const char* name)
{
    if (!g_screenList || !name)
        return false;
    const uint32_t hash = PathHash(name);
    uintptr_t node = ReadU32(g_screenList);
    for (int guard = 0; node && node != g_screenList && guard < 1024; ++guard)
    {
        const uintptr_t record = ReadU32(node + 0x0C);
        if (record && ReadU32(record + 4) == hash)
            return true;
        node = ReadU32(node);
    }
    return false;
}

int g_missingScreenLogs = 0;

const char* ClassWheelPreviewScreenImpl()
{
    LONG i = g_previewIdx;
    if (i <= 0 || i >= kPreviewCount)
        return nullptr;
    // 21j-r18: PC MENU chosen (Enabled=0): a 1-player game gets the game's own PC spawn screen
    if (LocalPlayersNow() < 2 && !g_wheelEnabled)
        return nullptr;
    // 21f: sides locked (campaign / Galactic Conquest): no side screen - the spawn point screen, his side the game's
    if (i == 1 && SidesLockedNow() == 1)
    {
        SetFlow(3, "sides are locked (campaign / Galactic Conquest: the game's side) - no side screen");
        i = 3;
    }
    // 18i-r2: only a console screen that is loaded in this match (a mod that replaces the interface
    // files may not have them) - otherwise the game keeps its PC screen
    if (!ScreenLoaded(kPreview[i]))
    {
        if (g_missingScreenLogs < 5)
        {
            ++g_missingScreenLogs;
            Log("[wheel] %lu ms: %s is not loaded in this match (a mod's interface without it?) - the PC spawn screen "
                "is used instead",
                ScanMsSinceAttach(), kPreview[i]);
        }
        return nullptr;
    }
    return kPreview[i];
}

// 18e: the driver's game functions, found by pattern and checked
void FindDriverFunctions()
{
    uint32_t hits[4] = {};
    int n = FindPattern(kSlotTitle, hits, 4);
    if (n != 1)
    {
        Log("[wheel] charselect driver OFF: the slot-title function has %d pattern hits (need exactly 1)", n);
        return;
    }
    const uint32_t slot = hits[0];
    if (*reinterpret_cast<const unsigned char*>(Rva(slot + kSlotTitleSetText)) != 0xE8)
    {
        Log("[wheel] charselect driver OFF: no call at slot-title+48h");
        return;
    }
    const uint32_t wrap = CallTarget(slot + kSlotTitleSetText);
    if (!MatchPatternAt(wrap, kSetTextWrap))
    {
        Log("[wheel] charselect driver OFF: the text setter wrapper at rva 0x%08X does NOT match", wrap);
        return;
    }
    uint32_t body = static_cast<uint32_t>(static_cast<int64_t>(wrap) + 4 + 5 + ReadI32At(wrap + 5));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(body)) == 0xE9; ++hop)
        body = static_cast<uint32_t>(static_cast<int64_t>(body) + 5 + ReadI32At(body + 1));
    if (!MatchPatternAt(body, kSetTextBody))
    {
        Log("[wheel] charselect driver OFF: SetText at rva 0x%08X does NOT match", body);
        return;
    }
    n = FindPattern(kSelectClass, hits, 4);
    if (n != 1)
    {
        Log("[wheel] charselect driver OFF: SelectClass has %d pattern hits (need exactly 1)", n);
        return;
    }
    const uint32_t select = hits[0];
    if (!MatchPatternAt(CallTarget(select + kSelectClassSetIndex), kSetClassIndex))
    {
        Log("[wheel] charselect driver OFF: SelectClass's first call is not SetClassIndex");
        return;
    }
    g_setTextW = reinterpret_cast<SetTextW_t>(Rva(wrap));
    g_selectClass = reinterpret_cast<SelectClass_t>(Rva(select));
    g_selectRva = select;
    Log("[wheel] charselect driver: slot title rva 0x%08X, SetText wrapper 0x%08X -> SetText 0x%08X, SelectClass "
        "0x%08X - all OK",
        slot, wrap, body, select);
}

// 18f: the PC Spawn button's calls (all copies must agree) and the PC input call site
void FindFlowFunctions(bool onGameThread)
{
    uint32_t hits[8] = {};
    const int n = FindPattern(kSpawnButton, hits, 8);
    if (n < 1 || n > 8)
    {
        Log("[wheel] console spawn OFF: the PC Spawn button sequence has %d pattern hits", n);
    }
    else
    {
        uint32_t t[4] = { CallTarget(hits[0] + 0x0D), CallTarget(hits[0] + 0x16), CallTarget(hits[0] + 0x1D),
                          CallTarget(hits[0] + 0x25) };
        bool same = true;
        for (int i = 1; i < n; ++i)
            same = same && CallTarget(hits[i] + 0x0D) == t[0] && CallTarget(hits[i] + 0x16) == t[1] &&
                   CallTarget(hits[i] + 0x1D) == t[2] && CallTarget(hits[i] + 0x25) == t[3];
        if (!same || !MatchPatternAt(t[0], kCharIndexWrap) || !MatchPatternAt(t[1], kGetSmBody) ||
            !MatchPatternAt(t[2], kCharacterBody) || !MatchPatternAt(t[3], kBeginSpawnBody))
        {
            Log("[wheel] console spawn OFF: the %d Spawn button sequences do not agree or their calls do NOT match", n);
        }
        else
        {
            g_charIndex = reinterpret_cast<CharIndex_t>(Rva(t[0]));
            g_character = reinterpret_cast<Character_t>(Rva(t[2]));
            g_beginSpawn = reinterpret_cast<BeginSpawn_t>(Rva(t[3]));
            Log("[wheel] console spawn: %d PC Spawn button sequences agree - CharIndex 0x%08X, Character 0x%08X, "
                "BeginSpawn 0x%08X - all OK",
                n, t[0], t[2], t[3]);
        }
    }

    // the pad / keyboard: XInput (whichever version the system has)
    const char* const dlls[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" };
    for (const char* name : dlls)
    {
        HMODULE h = LoadLibraryA(name);
        if (!h)
            continue;
        g_xGetState = reinterpret_cast<XGetState_t>(reinterpret_cast<void*>(GetProcAddress(h, "XInputGetState")));
        if (g_xGetState)
        {
            Log("[wheel] console flow pad: %s XInputGetState", name);
            break;
        }
    }

    // 18h: the ring's functions (each found at a fixed offset in a function found above, body checked)
    {
        const char* fail = nullptr;
        uint32_t f[10] = {};
        if (!g_makeRva || !g_spawnCtorRva || !g_setupRva || !g_selectRva)
            fail = "a function it starts from was not found";
        else
        {
            f[0] = CallTarget(g_makeRva + 6);
            const bool siteOk = MatchPatternAt(g_spawnCtorRva + 0x71, kSoldierElemSite);
            f[1] = siteOk ? CallTarget(g_spawnCtorRva + 0x77) : 0;
            const uint32_t show = CallTarget(g_selectRva + 0x37);
            const bool showOk = MatchPatternAt(show, kShowClassBody);
            f[2] = showOk ? CallTarget(show + 0x67) : 0;
            f[3] = showOk ? CallTarget(show + 0x6F) : 0;
            f[4] = showOk ? CallTarget(show + 0xC8) : 0;
            f[5] = showOk ? CallTarget(show + 0xE3) : 0;
            f[6] = showOk ? CallTarget(show + 0xFD) : 0;
            f[7] = CallTarget(g_setupRva + 0x6EE);
            f[8] = CallTarget(g_setupRva + 0x75A);
            f[9] = CallTarget(g_setupRva + 0x7B4);
            const char* const pats[10] = { kNewBody,       kSoldierElemCtor, kSetViewBody,    kSetSoldierClassBody,
                                           kSetWeaponBody, kSetLayerBody,    kSetVisibleBody, kSetScaleBody,
                                           kSetPositionBody, kAddChildBody };
            static const char* const names[10] = { "operator new", "SoldierElement ctor", "SetView", "SetSoldierClass",
                                                   "SetWeapon",    "SetLayer",            "SetVisible", "SetScale",
                                                   "SetPosition",  "AddChild" };
            for (int i = 0; i < 10 && !fail; ++i)
                if (!f[i] || !MatchPatternAt(f[i], pats[i]))
                    fail = names[i];
            if (!fail && !showOk)
                fail = "ShowClass";
        }
        if (fail)
            Log("[wheel] ring OFF: %s does NOT match", fail);
        else
        {
            g_rf.gameNew = reinterpret_cast<GameNew_t>(Rva(f[0]));
            g_rf.ctor = reinterpret_cast<ElemCtor_t>(Rva(f[1]));
            g_rf.setView = reinterpret_cast<ElemSetPtr_t>(Rva(f[2]));
            g_rf.setClass = reinterpret_cast<ElemSetPtr_t>(Rva(f[3]));
            g_rf.setWeapon = reinterpret_cast<ElemSetPtr_t>(Rva(f[4]));
            g_rf.setLayer = reinterpret_cast<ElemSetInt_t>(Rva(f[5]));
            g_rf.setVisible = reinterpret_cast<ElemSetInt_t>(Rva(f[6]));
            g_rf.setScale = reinterpret_cast<ElemSetFloat_t>(Rva(f[7]));
            g_rf.setPos = reinterpret_cast<ElemSetPos_t>(Rva(f[8]));
            g_rf.addChild = reinterpret_cast<ElemAdd_t>(Rva(f[9]));
            // the spawn screen's deleting destructor (vtable slot 0) through BF1GameExt first
            const uintptr_t slot = g_vtable;
            const uintptr_t orig = slot ? ReadU32(slot) : 0;
            uintptr_t real = orig; // the slot may point at a jump thunk (E9 rel32): follow it for the check
            for (int j = 0; j < 3 && real && InExeCodeVa(real); ++j)
            {
                uint8_t op = 0;
                int32_t rel = 0;
                if (!ReadMem(real, &op, 1) || op != 0xE9 || !ReadMem(real + 1, &rel, 4))
                    break;
                real = real + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(rel));
            }
            if (!g_sdDeleteHooked && orig && InExeCodeVa(orig) && InExeCodeVa(real) &&
                MatchPatternAt(real - ExeBase(), kSdDeleteBody))
            {
                DWORD old = 0;
                if (VirtualProtect(reinterpret_cast<void*>(slot), 4, PAGE_READWRITE, &old))
                {
                    g_origSdDelete = reinterpret_cast<SdDelete_t>(orig);
                    *reinterpret_cast<volatile uint32_t*>(slot) =
                        static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&SdDeleteHook));
                    DWORD tmp = 0;
                    VirtualProtect(reinterpret_cast<void*>(slot), 4, old, &tmp);
                    g_sdDeleteHooked = ReadU32(slot) == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&SdDeleteHook));
                }
            }
            if (g_sdDeleteHooked)
                Log("[wheel] ring: the spawn screen's deleting destructor (vtable slot 0 at 0x%08X -> rva 0x%08X -> rva "
                    "0x%08X) now goes through BF1GameExt first - OK",
                    static_cast<unsigned>(slot), static_cast<unsigned>(orig - ExeBase()),
                    static_cast<unsigned>(real - ExeBase()));
            else
                Log("[wheel] ring OFF: the spawn screen's deleting destructor could not be hooked (slot 0x%08X -> 0x%08X "
                    "-> 0x%08X)",
                    static_cast<unsigned>(slot), static_cast<unsigned>(orig), static_cast<unsigned>(real));
            g_ringOk = g_sdDeleteHooked;
            // 18i-r2: ShowClass's weapon step (type id getter + SetWeapon call), mirrored by the ring
            {
                const uint32_t show1 = CallTarget(g_selectRva + 0x37);
                const uint32_t typeFn = MatchPatternAt(show1 + 0xA7, kShowWeaponSeq) ? CallTarget(show1 + 0xA9) : 0;
                if (typeFn && MatchPatternAt(typeFn, kTypeFnBody) && CallTarget(show1 + 0xC8) == f[4])
                {
                    g_weaponType = reinterpret_cast<TypeId_t>(Rva(typeFn));
                    Log("[wheel] ring: ShowClass's weapon step OK (type id getter rva 0x%08X, SetWeapon 0x%08X)", typeFn,
                        f[4]);
                }
                else
                    Log("[wheel] ring: ShowClass's weapon step does NOT match - soldiers get vfunc 28h's weapon directly, "
                        "droidekas stay off the ring");
            }
            // 18i: the droideka element's constructor and set-class function
            const bool dSite = MatchPatternAt(g_spawnCtorRva + 0x66, kDroidekaElemSite);
            const uint32_t dCtor = dSite ? CallTarget(g_spawnCtorRva + 0x6C) : 0;
            const uint32_t show2 = CallTarget(g_selectRva + 0x37);
            const bool sSite = MatchPatternAt(show2 + 0x89, kSetDroidekaSite);
            const uint32_t dSet = sSite ? CallTarget(show2 + 0x92) : 0;
            if (g_weaponType && dCtor && dSet && MatchPatternAt(dCtor, kDroidekaElemCtor) &&
                MatchPatternAt(dSet, kSetDroidekaClassBody))
            {
                g_rdCtor = reinterpret_cast<ElemCtor_t>(Rva(dCtor));
                g_rdSetClass = reinterpret_cast<ElemSetPtr_t>(Rva(dSet));
                g_rdOk = true;
                Log("[wheel] ring: DroidekaElement ctor 0x%08X, SetDroidekaClass 0x%08X - OK (droidekas on the ring)",
                    dCtor, dSet);
            }
            else
                Log("[wheel] ring: droideka functions do NOT match (site %d/%d, ctor 0x%08X, set 0x%08X) - droidekas "
                    "stay off the ring",
                    dSite ? 1 : 0, sSite ? 1 : 0, dCtor, dSet);
            // 19g: the blue grid's constructor (SpawnDisplay ctor +7Ch) and SetModel (setup +86Ah), for player 2's copy
            {
                const bool cSite = MatchPatternAt(g_spawnCtorRva + kCubeCtorSiteAt, kCubeCtorSite);
                const uint32_t cCtor = cSite ? CallTarget(g_spawnCtorRva + kCubeCtorSiteAt + 6) : 0;
                const uint32_t setupSite = g_setupRva + 0x86A;
                const bool mSite = MatchPatternAt(setupSite, kCubeSetupSite);
                const uint32_t cModel = mSite ? CallTarget(setupSite + kCubeSetModelCall) : 0;
                const uint32_t cScale = mSite ? CallTarget(setupSite + kCubeScaleCall) : 0;
                if (cCtor && cModel && cScale && MatchPatternAt(cCtor, kCubeCtorBody) && MatchPatternAt(cModel, kCubeSetModelBody) &&
                    MatchPatternAt(cScale, kCubeScaleBody))
                {
                    g_cubeCtor = reinterpret_cast<ElemCtor_t>(Rva(cCtor));
                    g_cubeSetModel = reinterpret_cast<SetModel_t>(Rva(cModel));
                    g_cubeOk = true;
                    Log("[wheel] blue grid (SpawnDisplay +2210h): Red3DModelElement ctor 0x%08X, SetModel 0x%08X, its scale at "
                        "+A0h (0x%08X) - OK (player 2's screens get a copy)",
                        cCtor, cModel, cScale);
                }
                else
                    Log("[wheel] blue grid: functions do NOT match (ctor site %d, setup site %d, ctor 0x%08X, SetModel 0x%08X, "
                        "scale 0x%08X) - player 2's screens get no grid",
                        cSite ? 1 : 0, mSite ? 1 : 0, cCtor, cModel, cScale);
            }
            // 18l: the side screen's functions
            {
                const uint32_t tCtor =
                    MatchPatternAt(g_spawnCtorRva + 0x19, kTextCtorSite) ? CallTarget(g_spawnCtorRva + 0x1F) : 0;
                const uint32_t tFont =
                    MatchPatternAt(g_setupRva + 0x5DE, kSetFontSite) ? CallTarget(g_setupRva + 0x5E6) : 0;
                uint32_t jh[4] = {};
                uint32_t th[4] = {};
                const int nj = FindPattern(kSetJustifyBody, jh, 4);
                const int nt = FindPattern(kSelectTeamBody, th, 4);
                const bool teamOk = nt == 1 && MatchPatternAt(CallTarget(th[0] + 0x11), kSetTeamShownBody);
                if (tCtor && tFont && nj == 1 && teamOk && MatchPatternAt(tCtor, kTextCtorBody) &&
                    MatchPatternAt(tFont, kSetFontBody))
                {
                    g_txCtor = reinterpret_cast<ElemCtor_t>(Rva(tCtor));
                    g_txFont = reinterpret_cast<TextFont_t>(Rva(tFont));
                    g_txJustify = reinterpret_cast<TextJustify_t>(Rva(jh[0]));
                    g_selectTeam = reinterpret_cast<SelectTeam_t>(Rva(th[0]));
                    g_selectTeamRva = th[0];
                    const uint32_t fontObj = CallTarget(tFont + kSetFontObjCall);
                    if (MatchPatternAt(fontObj, kSetFontObjBody))
                        g_txFontObj = reinterpret_cast<SetFontObj_t>(Rva(fontObj));
                    uint32_t rh[4] = {};
                    const int nr = FindPattern(kRefSizeSeq, rh, 4);
                    if (nr == 2)
                    {
                        g_refWAddr = ReadU32(Rva(rh[0] + kRefSizeAt));
                        g_refHAddr = ReadU32(Rva(rh[1] + kRefSizeAt));
                    }
                    Log("[wheel] texts: SetFontObj 0x%08X%s, reference size globals %d hit(s) (width at 0x%08X, height at "
                        "0x%08X)",
                        fontObj, g_txFontObj ? "" : " (does NOT match - panel texts use gamefont_medium)", nr,
                        static_cast<unsigned>(g_refWAddr), static_cast<unsigned>(g_refHAddr));
                    g_sideOk = true;
                    Log("[wheel] side screen: RedTextElement ctor 0x%08X, SetFont 0x%08X, SetJustify 0x%08X, SelectTeam "
                        "0x%08X (SetTeamShown OK) - all OK",
                        tCtor, tFont, jh[0], th[0]);
                }
                else
                    Log("[wheel] side screen OFF: text ctor 0x%08X, SetFont 0x%08X, SetJustify %d hit(s), SelectTeam %d "
                        "hit(s) (SetTeamShown %s) - the side screen stays as it was",
                        tCtor, tFont, nj, nt, teamOk ? "OK" : "NOT OK");
                // 18m: the map - SetMode by pattern, its layout table from SetMode's code, the per-player
                // map array from GetMap (called in SelectTeam)
                uint32_t mh[4] = {};
                const int nm = FindPattern(kMapSetModeBody, mh, 4);
                const bool tableSite = nm == 1 && MatchPatternAt(mh[0] + 0x79, kMapTableSite);
                const uintptr_t table = tableSite ? ReadU32(Rva(mh[0] + kMapTableAt)) : 0;
                const uint32_t getMap =
                    (teamOk && MatchPatternAt(th[0] + 0x81, kGetMapSite)) ? CallTarget(th[0] + 0x83) : 0;
                const uintptr_t arr = (getMap && MatchPatternAt(getMap, kGetMapBody)) ? ReadU32(Rva(getMap + 9)) : 0;
                float l1[4] = {};
                if (table)
                    ReadMem(table + kMapLayout1, l1, sizeof(l1));
                if (nm == 1 && table && arr && l1[0] > 50.0f && l1[0] < 2000.0f && l1[1] > 50.0f && l1[1] < 2000.0f)
                {
                    g_mapSetMode = reinterpret_cast<MapSetMode_t>(Rva(mh[0]));
                    g_mapTable = table;
                    g_mapArray = arr;
                    g_mapOk = true;
                    Log("[wheel] map: SetMode 0x%08X, layout table at rva 0x%08X (layout 1: %.0f x %.0f, offset %+.0f "
                        "%+.0f), GetMap 0x%08X (maps at rva 0x%08X) - OK",
                        mh[0], static_cast<unsigned>(table - ExeBase()), l1[0], l1[1], l1[2], l1[3], getMap,
                        static_cast<unsigned>(arr - ExeBase()));
                }
                else
                    Log("[wheel] map OFF: SetMode %d hit(s), table site %d, table 0x%08X (layout 1 %.0f x %.0f), GetMap "
                        "0x%08X, maps 0x%08X - the map stays as the game has it",
                        nm, tableSite ? 1 : 0, static_cast<unsigned>(table), l1[0], l1[1], getMap,
                        static_cast<unsigned>(arr));
            }
            Log("[wheel] ring: new 0x%08X, SoldierElement ctor 0x%08X, SetView 0x%08X, SetSoldierClass 0x%08X, SetWeapon "
                "0x%08X, SetLayer 0x%08X, SetVisible 0x%08X, SetScale 0x%08X, SetPosition 0x%08X, AddChild 0x%08X - all OK",
                f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9]);
        }
    }

    // 18g: the PC next-spawn-point input's calls
    {
        uint32_t ph[4] = {};
        const int pn = FindPattern(kNextPostBlock, ph, 4);
        const uint32_t next = pn == 1 ? CallTarget(ph[0] + 0x0D) : 0;
        const uint32_t set = pn == 1 ? CallTarget(ph[0] + 0x15) : 0;
        if (pn != 1)
            Log("[wheel] command post choice OFF: the next-spawn-point block has %d pattern hits (need exactly 1)", pn);
        else if (!MatchPatternAt(CallTarget(ph[0] + 0x06), kGetSmBody) || !MatchPatternAt(next, kNextPostBody) ||
                 !MatchPatternAt(next + kNextPostCursorAt, kNextPostCursor) ||
                 !MatchPatternAt(next + kNextPostCursorSetAt, kNextPostCursorSet) || !MatchPatternAt(set, kSetPostBody) ||
                 *reinterpret_cast<const unsigned char*>(Rva(set + kSetPostStoreCall)) != 0xE8 ||
                 !MatchPatternAt(CallTarget(set + kSetPostStoreCall), kStorePostBody))
            Log("[wheel] command post choice OFF: NextSpawnPoint / SetSpawnPoint (or the cursor +A8h / post +23E0h) do "
                "NOT match");
        else
        {
            g_nextPost = reinterpret_cast<NextPost_t>(Rva(next));
            g_setPost = reinterpret_cast<SetPost_t>(Rva(set));
            g_setPostRva = set;
            Log("[wheel] command post choice: next-spawn-point block rva 0x%08X, NextSpawnPoint 0x%08X (cursor +A8h), "
                "SetSpawnPoint 0x%08X (post at +23E0h) - all OK",
                ph[0], next, set);
        }
    }

    // 21j-r19: the PC screen's click on its map - the post under the cursor (the mouse picks a post on the spawn point screen)
    if (g_setPost)
    {
        uint32_t mh[4] = {};
        const int mn = FindPattern(kMapClickBlock, mh, 4);
        const uint32_t at = mn == 1 ? CallTarget(mh[0] + kMapClickPostAt) : 0;
        const uint32_t set = mn == 1 ? CallTarget(mh[0] + kMapClickSetPost) : 0;
        const bool body = at && MatchPatternAt(at, kPostAtBody);
        const uint32_t getMap = body ? CallTarget(at + kPostAtGetMap) : 0;
        const uintptr_t maps = (getMap && MatchPatternAt(getMap, kGetMapBody)) ? ReadU32(Rva(getMap + 9)) : 0;
        const uintptr_t lo = ExeBase();
        const uintptr_t hi = CurrentBuild() ? ExeBase() + CurrentBuild()->sizeOfImage : 0;
        if (mn == 1 && set == g_setPostRva && body && maps >= lo && maps < hi && (!g_mapArray || maps == g_mapArray))
        {
            g_postAt = reinterpret_cast<PostAt_t>(Rva(at));
            g_postAtMaps = maps;
            Log("[wheel] 21j-r19: the map's post click: the PC screen's map click rva 0x%08X, PostAt 0x%08X (the post icon under the "
                "cursor, his team's), its SetSpawnPoint the same (0x%08X), the maps at rva 0x%08X - OK: a click on a post picks it",
                mh[0], at, set, static_cast<unsigned>(maps - lo));
        }
        else
            Log("[wheel] 21j-r19: the map's post click OFF (map click block %d hit(s), PostAt 0x%08X body %s, its SetSpawnPoint 0x%08X "
                "vs 0x%08X, maps 0x%08X) - posts are picked with the keys only; SELECT SPAWNPOINT still goes on",
                mn, at, body ? "OK" : "NOT matching", set, g_setPostRva, static_cast<unsigned>(maps));
    }

    // 18i-r2: the chooser's show-soldier call through BF1GameExt (game thread; the call is rewritten).
    // Without it the game's soldier shows through the ring every frame, so no redirect = no ring.
    if (g_ringOk)
    {
        uint32_t sh[4] = {};
        const int ns = onGameThread ? FindPattern(kChooserShowSite, sh, 4) : -1;
        const uint32_t site = ns == 1 ? sh[0] + kChooserShowCall : 0;
        const uintptr_t at = site ? Rva(site) : 0;
        const uint32_t tgt = site ? CallTarget(site) : 0;
        DWORD oldProt = 0;
        if (site && *reinterpret_cast<const unsigned char*>(at) == 0xE8 && MatchPatternAt(tgt, kSetVisibleBody) &&
            Rva(tgt) == reinterpret_cast<uintptr_t>(g_rf.setVisible) &&
            VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
        {
            const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&ChooserShowCurrent) - (at + 5));
            memcpy(reinterpret_cast<void*>(at + 1), &rel, 4);
            DWORD ignored = 0;
            VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, oldProt, &ignored);
            FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), 5);
            g_showRedirected = true;
            Log("[wheel] ring: the chooser's show-soldier call at rva 0x%08X (SetVisible rva 0x%08X) now goes through "
                "BF1GameExt - the game's soldier stays hidden while the ring shows it",
                site, tgt);
        }
        else
        {
            g_ringOk = false;
            Log("[wheel] ring OFF: the chooser's show-soldier call was not redirected (%s, %d hit(s), call rva 0x%08X, "
                "target rva 0x%08X)",
                onGameThread ? "pattern" : "not on the game thread", ns, site, tgt);
        }
    }

    // 18p: the interface sounds, through the Lua ScriptCB_SndPlaySound's own calls
    {
        const uintptr_t nameVa = FindModuleBytes("ScriptCB_SndPlaySound", 22);
        uintptr_t fnVa = 0;
        if (nameVa)
        {
            const uint32_t v = static_cast<uint32_t>(nameVa);
            const uintptr_t entry = FindModuleBytes(&v, 4);
            fnVa = entry ? ReadU32(entry + 4) : 0;
        }
        const uint32_t fn = (fnVa && InExeCodeVa(fnVa)) ? static_cast<uint32_t>(fnVa - ExeBase()) : 0;
        const bool headOk = fn && MatchPatternAt(fn, kSndPlayHead);
        const uint32_t setFn = headOk ? CallTarget(fn + kSndSetCall) : 0;
        const uint32_t playFn = headOk ? CallTarget(fn + kSndPlayCall) : 0;
        if (headOk && MatchPatternAt(setFn, kSndSetBody) && MatchPatternAt(playFn, kSndPlayBody))
        {
            g_sndSet = reinterpret_cast<SndSet_t>(Rva(setFn));
            g_sndPlay = reinterpret_cast<SndPlay_t>(Rva(playFn));
            g_sndOk = true;
            Log("[wheel] sounds: ScriptCB_SndPlaySound rva 0x%08X, SetByName 0x%08X, Play 0x%08X - OK (each sound's name "
                "is resolved from its list on first play, and logged)",
                fn, setFn, playFn);
        }
        else
            Log("[wheel] sounds OFF: ScriptCB_SndPlaySound name at 0x%08X, function rva 0x%08X (%s), SetByName 0x%08X, "
                "Play 0x%08X",
                static_cast<unsigned>(nameVa), fn, headOk ? "head OK" : "head does NOT match", setFn, playFn);
    }

    // 18q: the mouse - the game's own cursor, through the Lua ScriptCB_IFObj_TestHotSpot's own calls
    {
        const uintptr_t nameVa = FindModuleBytes("ScriptCB_IFObj_TestHotSpot", 27);
        uintptr_t fnVa = 0;
        if (nameVa)
        {
            const uint32_t v = static_cast<uint32_t>(nameVa);
            const uintptr_t entry = FindModuleBytes(&v, 4);
            fnVa = entry ? ReadU32(entry + 4) : 0;
        }
        const uint32_t fn = (fnVa && InExeCodeVa(fnVa)) ? static_cast<uint32_t>(fnVa - ExeBase()) : 0;
        const bool shapeOk = fn && MatchPatternAt(fn, kHotSpotHead) && MatchPatternAt(fn + kHotSpotMidAt, kHotSpotMid) &&
                             MatchPatternAt(fn + kHotSpotMid2At, kHotSpotMid2) && MatchPatternAt(fn + kHotSpotTailAt, kHotSpotTail);
        const uint32_t mgrFn = shapeOk ? CallTarget(fn + kHotSpotMgrCall) : 0;
        const uint32_t xFn = shapeOk ? CallTarget(fn + kHotSpotXCall) : 0;
        const uint32_t yFn = shapeOk ? CallTarget(fn + kHotSpotYCall) : 0;
        const uint32_t testFn = shapeOk ? CallTarget(fn + kHotSpotTestCall) : 0;
        const bool mgrOk = mgrFn && MatchPatternAt(mgrFn, kGetterImm);
        const bool xyOk = xFn && yFn && MatchPatternAt(xFn, kMouseXBody) && MatchPatternAt(yFn, kMouseYBody);
        const bool testOk = testFn && MatchPatternAt(testFn, kHitTestHead);
        const uint32_t boxFn = testOk ? CallTarget(testFn + kHitBoxCall) : 0;
        const bool boxOk = boxFn && MatchPatternAt(boxFn, kBoxTestHead);
        const uint32_t pointFn = boxOk ? CallTarget(boxFn + kBoxPointCall) : 0;
        const bool pointOk = pointFn && MatchPatternAt(pointFn, kPointTestHead);
        const uint32_t dispFn = pointOk ? CallTarget(pointFn + kPointDisplayCall) : 0;
        const bool dispOk = dispFn && MatchPatternAt(dispFn, kGetterImm);
        const uintptr_t mgr = mgrOk ? ReadU32(Rva(mgrFn + 1)) : 0;
        const uintptr_t disp = dispOk ? ReadU32(Rva(dispFn + 1)) : 0;
        const uintptr_t lo = ExeBase();
        const uintptr_t hi = lo + (CurrentBuild() ? CurrentBuild()->sizeOfImage : 0);
        if (shapeOk && mgrOk && xyOk && testOk && boxOk && pointOk && dispOk && mgr >= lo && mgr < hi && disp >= lo &&
            disp < hi)
        {
            g_inputMgr = mgr;
            g_displaySize = disp;
            g_mouseOk = true;
            Log("[wheel] mouse: ScriptCB_IFObj_TestHotSpot rva 0x%08X, input manager at rva 0x%08X (vfunc 20h(0) = player "
                "1's input, cursor at +138h / +13Ch), hit test 0x%08X -> box test 0x%08X -> point test 0x%08X, display "
                "size at rva 0x%08X - OK (the game's own cursor is used)",
                fn, static_cast<unsigned>(mgr - lo), testFn, boxFn, pointFn, static_cast<unsigned>(disp - lo));
        }
        else
            Log("[wheel] mouse: the game's cursor was NOT found (TestHotSpot name at 0x%08X, function rva 0x%08X, shape %s, "
                "manager getter 0x%08X %s, x / y getters 0x%08X 0x%08X %s, hit test 0x%08X %s, box test 0x%08X %s, point "
                "test 0x%08X %s, display getter 0x%08X %s) - the Windows cursor over the game's client area is used",
                static_cast<unsigned>(nameVa), fn, shapeOk ? "OK" : "does NOT match", mgrFn, mgrOk ? "OK" : "no", xFn, yFn,
                xyOk ? "OK" : "no", testFn, testOk ? "OK" : "no", boxFn, boxOk ? "OK" : "no", pointFn,
                pointOk ? "OK" : "no", dispFn, dispOk ? "OK" : "no");
    }

    // 18o: the game's two SetText calls for the vehicles at the post (PopupFill, from SetSpawnPoint) and
    // the team's objective (SelectTeam) through BF1GameExt first (game thread; the calls are rewritten)
    if (onGameThread && g_setTextW && g_setPostRva && g_selectTeamRva)
    {
        const uint32_t fill = *reinterpret_cast<const unsigned char*>(Rva(g_setPostRva + kSetPostFillCall)) == 0xE8
                                  ? CallTarget(g_setPostRva + kSetPostFillCall)
                                  : 0;
        const bool fillOk = fill && MatchPatternAt(fill, kPopupFillHead);
        const uint32_t vehSite = fillOk && MatchPatternAt(fill + kVehTextSiteAt, kVehTextSite) ? fill + kVehTextSiteAt + 10 : 0;
        const uint32_t objSite =
            MatchPatternAt(g_selectTeamRva + kObjTextSiteAt, kObjTextSite) ? g_selectTeamRva + kObjTextSiteAt + 13 : 0;
        const uint32_t setText = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_setTextW) - ExeBase());
        int done = 0;
        const uint32_t sites[2] = { vehSite, objSite };
        void* const hooks[2] = { reinterpret_cast<void*>(&VehTextHook), reinterpret_cast<void*>(&ObjTextHook) };
        for (int i = 0; i < 2; ++i)
        {
            const uint32_t site = sites[i];
            if (!site || CallTarget(site) != setText)
                continue;
            const uintptr_t at = Rva(site);
            DWORD oldProt = 0;
            if (!VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
                continue;
            const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(hooks[i]) - (at + 5));
            memcpy(reinterpret_cast<void*>(at + 1), &rel, 4);
            DWORD ignored = 0;
            VirtualProtect(reinterpret_cast<LPVOID>(at + 1), 4, oldProt, &ignored);
            FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), 5);
            ++done;
        }
        g_popupHooked = done == 2;
        g_popupFillRva = (done == 2 && vehSite) ? fill : 0; // 19h
        Log("[wheel] panel texts: PopupFill rva 0x%08X %s, vehicles SetText call rva 0x%08X, objective SetText call rva "
            "0x%08X - %d of 2 now go through BF1GameExt first%s",
            fill, fillOk ? "OK" : "does NOT match", vehSite, objSite, done,
            done == 2 ? "" : " (the VEHICLES / OBJECTIVES boxes stay empty)");
    }

    // the PC input call: only on the game thread (the call is rewritten)
    if (!onGameThread)
    {
        Log("[wheel] PC input skip NOT installed: the scan did not run on the game thread");
        return;
    }
    const int m = FindPattern(kInputCall, hits, 4);
    if (m != 1)
    {
        Log("[wheel] PC input skip NOT installed: %d pattern hits for its call (need exactly 1)", m);
        return;
    }
    const uintptr_t call = Rva(hits[0] + kInputCallAt);
    const uint32_t target = CallTarget(hits[0] + kInputCallAt);
    if (*reinterpret_cast<const unsigned char*>(call) != 0xE8 || !MatchPatternAt(target, kPcInput))
    {
        Log("[wheel] PC input skip NOT installed: the call at rva 0x%08X is not the PC input function",
            hits[0] + kInputCallAt);
        return;
    }
    g_pcInput = reinterpret_cast<PcInput_t>(Rva(target));
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&PcInputHook) - (call + 5));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(call + 1), 4, PAGE_EXECUTE_READWRITE, &oldProt))
    {
        Log("[wheel] PC input skip NOT installed: VirtualProtect failed (%lu)", GetLastError());
        return;
    }
    memcpy(reinterpret_cast<void*>(call + 1), &rel, 4);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(call + 1), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(call), 5);
    Log("[wheel] PC input skip: the spawn screen's call at rva 0x%08X (to the PC input function rva 0x%08X) now goes "
        "through BF1GameExt - skipped while a console screen is up",
        hits[0] + kInputCallAt, target);
}

} // namespace

const char* ClassWheelPreviewScreen()
{
    return ClassWheelPreviewScreenImpl();
}

// ---- 18u: lent to player 2's spawn screens (split_spawn_menus.cpp; game thread) ------------------------
bool ClassWheelTextsReady()
{
    return TextOk() && g_rf.gameNew && g_rf.setPos && g_rf.addChild && g_rf.setLayer && g_rf.setVisible;
}

bool ClassWheelSetText(uintptr_t text, const wchar_t* s)
{
    if (!text || !g_setTextW)
        return false;
    g_setTextW(reinterpret_cast<void*>(text), nullptr, s ? s : L"");
    return true;
}

uintptr_t ClassWheelTextLike(uintptr_t group, uintptr_t like, const wchar_t* s, float x, float y)
{
    if (!group || !ClassWheelTextsReady())
        return 0;
    return TextLike(group, like, s ? s : L"", x, y);
}

uintptr_t ClassWheelTextNew(uintptr_t group, const wchar_t* s, float x, float y, float scale)
{
    if (!group || !ClassWheelTextsReady())
        return 0;
    // TextNew turns player 1's side screen off when the game's text element is not what it expects;
    // player 2's texts leave player 1's flow as it was
    const bool sideOk = g_sideOk;
    const uintptr_t e = TextNew(group, "gamefont_large", s ? s : L"", x, y, scale, kZText);
    g_sideOk = sideOk;
    return e;
}

void ClassWheelTextGrey(uintptr_t text, bool grey)
{
    TextGrey(text, grey);
}

// 20g-r7: as ClassWheelTextNew, with its own horizontal justify (0 left, 1 right, 2 centre) and colour (0xAARRGGBB -
// the element's B, G, R, A bytes); the string goes in last, as TextNew's (SetJustify wipes the layout)
uintptr_t ClassWheelTextNewEx(uintptr_t group, const wchar_t* s, float x, float y, float scale, int hJustify, uint32_t argb)
{
    if (!group || !ClassWheelTextsReady())
        return 0;
    const bool sideOk = g_sideOk;
    const uintptr_t e = TextNew(group, "gamefont_large", L"", x, y, scale, kZText);
    g_sideOk = sideOk;
    if (!e)
        return 0;
    if (hJustify != 2)
        g_txJustify(reinterpret_cast<void*>(e), nullptr, hJustify < 0 || hJustify > 2 ? 0 : hJustify, 2);
    *reinterpret_cast<uint32_t*>(e + kElemColor) = argb;
    g_setTextW(reinterpret_cast<void*>(e), nullptr, s ? s : L"");
    return e;
}

void ClassWheelTextColor(uintptr_t text, uint32_t argb)
{
    if (text)
        *reinterpret_cast<uint32_t*>(text + kElemColor) = argb;
}

void ClassWheelDeleteElement(uintptr_t e)
{
    ElemDelete(e);
}

void ClassWheelShowElement(uintptr_t e, bool on)
{
    if (e && g_rf.setVisible)
        g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, on ? 1 : 0);
}

bool ClassWheelPlaySound(int which)
{
    // 19i: player 2's plays logged for a while (19h-r2: no UI sound heard for him)
    static int lines = 0;
    const bool played = PlayUiSound(which == 1 ? g_sndSelect : (which == 2 ? g_sndBack : g_sndTick));
    if (lines++ < 12)
        Log("[wheel] %lu ms: sound for player 2's screens: %s - %s", ScanMsSinceAttach(),
            which == 1 ? "select" : (which == 2 ? "back" : "tick"), played ? "played" : "NOT played (no name the banks know)");
    return played;
}

// 18x: a class's soldier for player 2's screens: made like the ring's, in `group`, at (x, y) of that
// group (root units), the game's own soldier's view / matrix / scale, size = the game's x sizeMul
uintptr_t ClassWheelModelNew(uintptr_t group, uintptr_t cls, float x, float y, float sizeMul, bool* droideka)
{
    *droideka = false;
    const uintptr_t sd = g_global ? ReadU32(g_global) : 0;
    ElemBase sb;
    ElemBase db;
    if (!sd || !g_ringOk || !group || !ReadElemBase(sd + kSdSoldierElem, "SoldierElement", &sb))
        return 0;
    const bool haveDroid = g_rdOk && ReadElemBase(sd + kSdDroidekaElem, "DroidekaElement", &db);
    char ct[64];
    if (!cls || !TypeName(cls, ct, sizeof(ct)))
        return 0;
    const bool isSoldier = strcmp(ct, "EntitySoldierClass") == 0;
    const bool isDroideka = strcmp(ct, "EntityDroidekaClass") == 0 && haveDroid;
    if (!isSoldier && !isDroideka)
        return 0;
    const uintptr_t weapon = ClassWeapon(cls);
    if (isDroideka && !weapon)
        return 0;
    const bool ringOk = g_ringOk;
    const uintptr_t e = RingNew(isDroideka);
    g_ringOk = ringOk; // player 2's try never turns player 1's ring off
    if (!e)
        return 0;
    if (isSoldier)
        g_rf.setView(reinterpret_cast<void*>(e), nullptr, reinterpret_cast<uintptr_t>(sb.view));
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    if (isSoldier)
        g_rf.setClass(reinterpret_cast<void*>(e), nullptr, cls);
    else
        g_rdSetClass(reinterpret_cast<void*>(e), nullptr, cls);
    if (weapon)
        g_rf.setWeapon(reinterpret_cast<void*>(e), nullptr, weapon);
    const uintptr_t rebuild = VirtualAt(e, 0x2C);
    const uintptr_t idle = VirtualAt(e, 0x3C);
    if (rebuild)
        reinterpret_cast<ElemNoArg_t>(rebuild)(reinterpret_cast<void*>(e), nullptr);
    if (idle)
        reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, 1);
    const ElemBase& b = isDroideka ? db : sb;
    float* const m = reinterpret_cast<float*>(e + kElemMatrix);
    for (int j = 0; j < 12; ++j)
        m[j] = b.matrix[j];
    m[14] = b.matrix[14];
    m[15] = b.matrix[15];
    memcpy(reinterpret_cast<void*>(e + kElemRotOfs), b.rotOfs, sizeof(b.rotOfs));
    g_rf.setScale(reinterpret_cast<void*>(e), nullptr, b.s);
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    const uintptr_t set80 = VirtualAt(e, 0x38);
    if (set80)
        reinterpret_cast<ElemSetFloat_t>(set80)(reinterpret_cast<void*>(e), nullptr, b.f80 * sizeMul * (isDroideka ? g_dekaSize : 1.0f));
    uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
    c[0] = c[1] = c[2] = c[3] = 0xFF;
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, kZFront);
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    *droideka = isDroideka;
    return e;
}

void ClassWheelModelGrey(uintptr_t e, bool grey)
{
    if (!e)
        return;
    uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
    c[0] = c[1] = c[2] = grey ? kDarkGrey : 0xFF;
    c[3] = 0xFF;
}

// ---- 19g: lent to player 2's screens: the blue grid, a layer, his class ring ------------------------
// The spawn screen's blue grid ("shell_cube", SpawnDisplay +2210h) copied into `group` at (x, y) of it
// (the root's centre: where player 1's stands in his root): the same class (checked by name; its
// deleting destructor checked to free the size made here), player 1's model (SetModel with his hash),
// scale (+A0h), depth (+9Ch) and colour (+70h), alpha 255 and layer 255 (what the spawn screen gives
// its own when it shows it). Deleted with ClassWheelDeleteElement.
uintptr_t ClassWheelCubeNew(uintptr_t group, float x, float y)
{
    const uintptr_t sd = g_global ? ReadU32(g_global) : 0;
    if (!g_cubeOk || !sd || !group || !g_rf.gameNew || !g_rf.addChild || !g_rf.setPos || !g_rf.setLayer || !g_rf.setVisible)
        return 0;
    const uintptr_t src = sd + kSdCube;
    char t[64] = "";
    const uintptr_t del = VirtualAt(src, 0);
    uint32_t delRva = del ? static_cast<uint32_t>(del - ExeBase()) : 0;
    for (int hop = 0; hop < 8 && delRva && *reinterpret_cast<const unsigned char*>(Rva(delRva)) == 0xE9; ++hop)
        delRva = static_cast<uint32_t>(static_cast<int64_t>(delRva) + 5 + ReadI32At(delRva + 1));
    const uint32_t hash = ReadU32(src + kCubeHash);
    const bool typeOk = TypeName(src, t, sizeof(t)) && strcmp(t, "Red3DModelElement") == 0;
    if (!typeOk || !delRva || !MatchPatternAt(delRva, kCubeDeleteBody) || !hash)
    {
        if (g_cubeLines++ < 20)
            Log("[wheel] blue grid: player 1's (spawn screen +2210h) is not a set-up Red3DModelElement (type \"%s\", model hash "
                "0x%08X, deleting destructor rva 0x%08X %s) - no copy",
                typeOk ? t : "?", hash, delRva, (delRva && MatchPatternAt(delRva, kCubeDeleteBody)) ? "OK" : "does NOT match");
        return 0;
    }
    void* mem = g_rf.gameNew(kCubeSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_cubeCtor(mem, nullptr));
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, "Red3DModelElement") != 0)
    {
        Log("[wheel] blue grid: the new element is a \"%s\", not a Red3DModelElement - no grid for player 2 (left alone)",
            e ? t : "(null)");
        g_cubeOk = false;
        return 0;
    }
    g_cubeSetModel(reinterpret_cast<void*>(e), nullptr, hash);
    *reinterpret_cast<float*>(e + kCubeScale) = ReadF32(src + kCubeScale);
    *reinterpret_cast<float*>(e + kCubeDepth) = ReadF32(src + kCubeDepth);
    *reinterpret_cast<uint32_t*>(e + kElemColor) = ReadU32(src + kElemColor) | 0xFF000000u; // alpha (+73h) 255
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, kCubeLayer);
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    if (g_cubeLines++ < 20)
        Log("[wheel] %lu ms: blue grid: copy 0x%08X made in group 0x%08X at %.0f %.0f (model hash 0x%08X, scale %.4f, depth %.3f, "
            "colour 0x%08X) - player 1's is 0x%08X (%s, at %.0f %.0f in the list at 0x%08X; his soldiers' group 0x%08X)",
            ScanMsSinceAttach(), static_cast<unsigned>(e), static_cast<unsigned>(group), x, y, hash, ReadF32(e + kCubeScale),
            ReadF32(e + kCubeDepth), ReadU32(e + kElemColor), static_cast<unsigned>(src),
            (ReadU32(src + 0x14) & 0x100) ? "shown" : "hidden", ReadF32(src + kElemX), ReadF32(src + kElemY),
            static_cast<unsigned>(ReadU32(src + 0x24)), static_cast<unsigned>(sd + kSdGroup)); // 20g-r12: where his is
    return e;
}

void ClassWheelSetLayer(uintptr_t e, int layer)
{
    if (e && g_rf.setLayer)
        g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, layer);
}

namespace {
// player 2's class ring: the same soldiers as player 1's ring, in his class screen's group. 20f-r2: three of them -
// ring 0 = player 2's, rings 1 / 2 = players 3 / 4's (split_spawn_menus.cpp's own screens in their HUD roots). 20g-r11: ring 3 =
// player 1's in players 3 / 4's look (4-player)
constexpr int kRings = 12;    // v7.1: 4 - 7 = players 1 - 4's side carousels (ClassWheelSideRingFrame); v7.3: 8 - 11 = theirs in
                              // players 3 / 4's look (slots 5 - 8)
constexpr int kSideRing0 = 4;

int Ring2Player(int r)
{
    return r >= kSideRing0 ? (r - kSideRing0) % 4 + 1 : (r == 3 ? 1 : r + 2);
}
struct Ring2
{
    uintptr_t group = 0;
    int n = 0;
    uintptr_t cls[kRingMax] = {};
    uintptr_t elem[kRingMax] = {};
    bool droideka[kRingMax] = {};
    int layer[kRingMax] = {};
    float pos = 0.0f;
    bool posSet = false;
    LARGE_INTEGER last = {};
    int lines = 0;
};
Ring2 g_ring2[kRings];

bool Ring2Budget(int r)
{
    return g_ring2[r].lines++ < 60;
}

// del: delete them (their group is alive), else forget them (a match being torn down)
void Ring2Drop(int r, bool del, const char* why)
{
    Ring2& R = g_ring2[r];
    int n = 0;
    for (int i = 0; i < kRingMax; ++i)
    {
        if (R.elem[i])
        {
            if (del)
                ElemDelete(R.elem[i]);
            ++n;
        }
        R.elem[i] = 0;
        R.cls[i] = 0;
        R.droideka[i] = false;
        R.layer[i] = -1;
    }
    R.group = 0;
    R.n = 0;
    R.posSet = false;
    if (n && Ring2Budget(r))
        Log("[wheel] %lu ms: player %d's ring: %d soldier(s) %s (%s)", ScanMsSinceAttach(), Ring2Player(r), n,
            del ? "deleted" : "left to the game", why);
}

int RingFrame2(int r, uintptr_t group, const uintptr_t* classes, int n, int sel, float cx, float cy, float width, float size,
               float lift, float fixX, float fixY, int style)
{
    const bool xbox = style == kRingStyleXbox;
    const bool classic = style == kRingStyleClassic; // 20g-r12
    (void)fixX; // 20g-r11: no longer used - a droideka stands at its slot as a soldier does (kept for the callers' logs)
    (void)fixY;
    if (n > kRingMax)
        n = kRingMax;
    if (r < 0 || r >= kRings || !g_ringOk || !group || !classes || n <= 0 || sel < 0 || sel >= n)
        return 0;
    Ring2& R = g_ring2[r];
    const uintptr_t sd = g_global ? ReadU32(g_global) : 0;
    ElemBase sb;
    ElemBase db;
    if (!sd || !ReadElemBase(sd + kSdSoldierElem, "SoldierElement", &sb))
        return 0;
    const bool haveDroid = g_rdOk && ReadElemBase(sd + kSdDroidekaElem, "DroidekaElement", &db);
    if (R.n && R.group != group)
        Ring2Drop(r, false, "another group"); // (split_spawn_menus.cpp clears it first; never touched here)
    bool same = R.n == n;
    for (int i = 0; same && i < n; ++i)
        same = R.cls[i] == classes[i];
    if (!same && R.n)
        Ring2Drop(r, true, "the side's classes changed");
    R.group = group;
    R.n = n;
    for (int i = 0; i < n; ++i)
    {
        if (R.elem[i] || R.cls[i] == classes[i])
            continue; // made (or tried) already
        const uintptr_t cls = classes[i];
        R.cls[i] = cls;
        char ct[64];
        if (!cls || !TypeName(cls, ct, sizeof(ct)))
            continue;
        const bool isSoldier = strcmp(ct, "EntitySoldierClass") == 0;
        const bool isDroideka = strcmp(ct, "EntityDroidekaClass") == 0 && haveDroid;
        const uintptr_t weapon = (isSoldier || isDroideka) ? ClassWeapon(cls) : 0;
        if ((!isSoldier && !isDroideka) || (isDroideka && !weapon))
        {
            if (Ring2Budget(r))
                Log("[wheel] player %d's ring: class %d is a %s%s - not on the ring", Ring2Player(r), i, ct,
                    isDroideka ? " without a weapon" : "");
            continue;
        }
        const bool ringOk = g_ringOk;
        const uintptr_t e = RingNew(isDroideka);
        g_ringOk = ringOk; // player 2's try never turns player 1's ring off
        if (!e)
            continue;
        if (isSoldier)
            g_rf.setView(reinterpret_cast<void*>(e), nullptr, reinterpret_cast<uintptr_t>(sb.view));
        g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
        if (isSoldier)
            g_rf.setClass(reinterpret_cast<void*>(e), nullptr, cls);
        else
            g_rdSetClass(reinterpret_cast<void*>(e), nullptr, cls);
        if (weapon)
            g_rf.setWeapon(reinterpret_cast<void*>(e), nullptr, weapon);
        const uintptr_t rebuild = VirtualAt(e, 0x2C);
        const uintptr_t idle = VirtualAt(e, 0x3C);
        if (rebuild)
            reinterpret_cast<ElemNoArg_t>(rebuild)(reinterpret_cast<void*>(e), nullptr);
        if (idle)
            reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, 1);
        uint8_t* const c = reinterpret_cast<uint8_t*>(e + kElemColor);
        c[0] = c[1] = c[2] = c[3] = 0xFF;
        R.elem[i] = e;
        R.droideka[i] = isDroideka;
        R.layer[i] = -1;
        if (Ring2Budget(r))
            Log("[wheel] %lu ms: player %d's ring: class %d (%s) element made at 0x%08X in group 0x%08X", ScanMsSinceAttach(),
                Ring2Player(r), i, isDroideka ? "droideka" : "soldier", static_cast<unsigned>(e), static_cast<unsigned>(group));
    }

    // turn: eases toward the selected class (shortest way round), as player 1's ring
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    float dt = 0.0f;
    if (R.last.QuadPart && freq.QuadPart)
        dt = static_cast<float>(static_cast<double>(now.QuadPart - R.last.QuadPart) / static_cast<double>(freq.QuadPart));
    R.last = now;
    if (dt < 0.0f || dt > 0.1f)
        dt = 0.0f;
    const float nf = static_cast<float>(n);
    if (!R.posSet)
    {
        R.pos = static_cast<float>(sel);
        R.posSet = true;
    }
    else
    {
        float d = static_cast<float>(sel) - R.pos;
        while (d > nf * 0.5f)
            d -= nf;
        while (d <= -nf * 0.5f)
            d += nf;
        const float k = dt * kTurnRate;
        R.pos += d * (k < 1.0f ? k : 1.0f);
        if (d < 0.002f && d > -0.002f)
            R.pos = static_cast<float>(sel);
        while (R.pos < 0.0f)
            R.pos += nf;
        while (R.pos >= nf)
            R.pos -= nf;
    }

    // place: as player 1's ring (nearer = bigger, lower, drawn later, nearer the camera)
    const float kPi = 3.14159265f;
    int placed = 0;
    for (int i = 0; i < n; ++i)
    {
        const uintptr_t e = R.elem[i];
        if (!e)
            continue;
        float a = 2.0f * kPi * (static_cast<float>(i) - R.pos) / nf;
        while (a > kPi)
            a -= 2.0f * kPi;
        while (a <= -kPi)
            a += 2.0f * kPi;
        const float nearness = (std::cos(a) + 1.0f) * 0.5f; // 1 front .. 0 back
        // 19h: xbox: the Xbox's arrangement as player 1's split ring - (cx, cy) the ring's centre at the
        // feet, the picked one kXbRingPhase round from the front, sizes XbRingRel, a small depth; else
        // 19g's ring ((cx, cy) the front soldier's feet)
        // 20g-r12: classic: the Classic Collection's horizontal ring - the picked one in front in the middle, the others
        // round it, smaller and higher the further back (ClassicRingRel / ClassicRingRise)
        const float sizeMul = classic ? size * ClassicRingRel(nearness)
                                      : (xbox ? size * XbRingRel(nearness) : size * (g_ringBack + (1.0f - g_ringBack) * nearness));
        const float x = cx + width * std::sin(xbox ? a + kXbRingPhase : a);
        const float y = classic ? cy - lift * (1.0f - ClassicRingRise(nearness)) : cy - lift * (1.0f - nearness);
        const ElemBase& b = R.droideka[i] ? db : sb;
        float* const m = reinterpret_cast<float*>(e + kElemMatrix);
        for (int j = 0; j < 12; ++j)
            m[j] = b.matrix[j];
        m[14] = b.matrix[14] - (1.0f - nearness) * ((xbox || classic) ? kSplitRingDepth : g_ringDepth) * width;
        m[15] = b.matrix[15];
        memcpy(reinterpret_cast<void*>(e + kElemRotOfs), b.rotOfs, sizeof(b.rotOfs));
        g_rf.setScale(reinterpret_cast<void*>(e), nullptr, b.s);
        // 20g-r11: a droideka at its slot exactly as a soldier (r10's video: the droideka drew exactly where a soldier at
        // fix + (x - fix) * (soldier +84h / droideka +84h) would - its own +84h does not move it in these groups; r9's
        // mapping pulled it 17% toward (fixX, fixY), off the ring, down and in front)
        g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
        const uintptr_t set80 = VirtualAt(e, 0x38);
        if (set80)
            reinterpret_cast<ElemSetFloat_t>(set80)(reinterpret_cast<void*>(e), nullptr,
                                                     b.f80 * sizeMul * (R.droideka[i] ? g_dekaSize : 1.0f));
        const int layer = kZFront + static_cast<int>((1.0f - nearness) * 30.0f);
        if (layer != R.layer[i])
        {
            g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, layer);
            R.layer[i] = layer;
        }
        g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
        // the selected class plays its idle animations, the others stop (as player 1's ring)
        const uintptr_t idle = VirtualAt(e, 0x3C);
        const uintptr_t tick = VirtualAt(e, 0x30);
        if (idle)
            reinterpret_cast<ElemSetInt_t>(idle)(reinterpret_cast<void*>(e), nullptr, i == sel ? 1 : 0);
        if (tick && (i == sel || R.droideka[i]))
            reinterpret_cast<ElemSetFloat_t>(tick)(reinterpret_cast<void*>(e), nullptr, dt);
        ++placed;
    }
    return placed;
}
} // namespace

// player 2's class ring in `group` (his class screen's Info): one soldier per class (made like
// ClassWheelModelNew's), the ring turning to `sel` as player 1's does; (cx, cy) = the ring's centre
// at the soldiers' feet in the group's units (19h; was the front soldier's), width = the ring's radius
// (units), size = the picked soldier's size (of the game's soldier), lift = how much higher the back
// of the ring stands (units; 19h: 0, feet level), xbox = the Xbox's arrangement (false: 19g's, with
// (cx, cy) the front soldier's feet), (fixX, fixY) = where the game's own soldier stands, in the group's units
// (20g-r11: not used any more - a droideka stands at its slot as a soldier does). Returns the soldiers placed
// (0 = none: the ring's functions or the game's soldier are not there).
int ClassWheelRing2Frame(uintptr_t group, const uintptr_t* classes, int n, int sel, float cx, float cy, float width, float size,
                         float lift, float fixX, float fixY, int style)
{
    return RingFrame2(0, group, classes, n, sel, cx, cy, width, size, lift, fixX, fixY, style);
}

void ClassWheelRing2Clear(bool del, const char* why)
{
    Ring2Drop(0, del, why ? why : "player 2's class screen is not up");
}

// 20f-r2: players 3 and 4's rings (ring 1 / 2), the same as player 2's
int ClassWheelRingFrame(int ring, uintptr_t group, const uintptr_t* classes, int n, int sel, float cx, float cy, float width,
                        float size, float lift, float fixX, float fixY, int style)
{
    if (ring < 1 || ring >= kRings)
        return 0;
    return RingFrame2(ring, group, classes, n, sel, cx, cy, width, size, lift, fixX, fixY, style);
}

void ClassWheelRingClear(int ring, bool del, const char* why)
{
    if (ring >= 1 && ring < kSideRing0)
        Ring2Drop(ring, del, why ? why : "his class screen is not up");
}

// v7.1: the sides of this match for the side screens (Mashup Conquest: every playable team, else teams 1 and 2) and
// whether a side is out of the match
int ClassWheelSideTeams(int* teams, int max)
{
    int t[kMaxSides] = {};
    const int n = SideTeamsNow(t);
    int k = 0;
    for (int i = 0; i < n && k < max; ++i)
        teams[k++] = t[i];
    return k;
}
bool ClassWheelSideOut(int team)
{
    return SideOutNow(team);
}

// v7.1: player p's (1 - 4) side carousel - each side's first class on a ring in `group` (as the class rings), turned to
// `sel`, the others darker, a side out of the match darker still
int ClassWheelSideRingFrame(int player, uintptr_t group, const uintptr_t* classes, const bool* out, int n, int sel, float cx,
                            float cy, float width, float size, float lift, int style)
{
    if (player < 1 || player > 8) // v7.3: 5 - 8 = players 1 - 4 in players 3 / 4's look
        return 0;
    const int r = kSideRing0 + player - 1;
    const int placed = RingFrame2(r, group, classes, n, sel, cx, cy, width, size, lift, cx, cy, style);
    Ring2& R = g_ring2[r];
    for (int i = 0; i < R.n && i < kRingMax; ++i)
    {
        if (!R.elem[i])
            continue;
        uint8_t* const c = reinterpret_cast<uint8_t*>(R.elem[i] + kElemColor);
        const uint8_t v = (out && out[i]) ? kOutGrey : (i == sel ? 0xFF : kDarkGrey);
        c[0] = c[1] = c[2] = v;
        c[3] = 0xFF;
    }
    return placed;
}
void ClassWheelSideRingClear(int player, bool del, const char* why)
{
    if (player >= 1 && player <= 8)
        Ring2Drop(kSideRing0 + player - 1, del, why ? why : "his side screen is not up");
}

// 20f-r2 / r4: an empty RedScreenGroupElement of the game's (its own constructor, the game's new) - players 3 and 4's
// spawn screens are built in one (split_spawn_menus.cpp puts it in their HUD roots). 0 if its constructor was not found at
// start (or the vtable it writes is not RedScreenGroupElement's, or its deleting destructor does not free the size made).
uintptr_t ClassWheelGroupNew()
{
    if (!g_groupCtor || !g_rf.gameNew)
        return 0;
    void* mem = g_rf.gameNew(kGroupSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_groupCtor(mem, nullptr));
    char t[64] = "";
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, kGroupClass) != 0)
    {
        Log("[wheel] 20f: the new group is a \"%s\", not a RedScreenGroupElement - no groups made (left as it is)", e ? t : "(null)");
        g_groupCtor = nullptr;
        return 0;
    }
    return e;
}

// ---- 19h: the Xbox's flat panels and class window ----------------------------------------------------
// 19g's log: the console screens' SideBar group (the spawn point screen's panels) and Info group (the
// class window) are turned 42 degrees about the vertical axis (matrix 0.743 0 0.669 / 0 1 0 / -0.669 0
// 0.743); the panels and the window themselves are not turned. The Xbox's are flat. The group is set
// straight (rows = identity, pinned at 0, 0) and each panel / the window is scaled (its own rows = s)
// and placed so that its orange title bar's top left lands where the Xbox has it. 19h-r2 (19h's log):
// a panel is its title (a FlashyTextElement: the text with the orange bar behind it, the bar from the
// title's y down to the box) over its Skin (9 bitmaps: the blue box only - 507 x 30 under COMMAND POST,
// the title 28 above it); the first build looked for the bar in the skin and never laid anything out.
// A flat element at x, y of a group anchored at (ax, ay) in a root draws at offX + 0.88 ax + x / 512
// across and 0.05 + 0.9 ay + y / 624 down (19c-19g's videos; offX 0.1 for player 1, 0 for player
// 2). The box is stretched (its Skin group's y rows) to the Xbox's box bottom where the Xbox's box is
// taller than the PC's (the OBJECTIVES panel, the class window).
namespace {
constexpr uint32_t kElemKids = 0x80;        // a group's children (circular list: [list] first node, [node] next, [node+0Ch] child)
constexpr uint32_t kBmpCorners = 0x88;      // a bitmap's 4 corners (x, y first), 1Ch apart (its SetRect, GOG 1.3 0x002A92A0)
constexpr uint32_t kBmpCornerStep = 0x1C;
constexpr uint32_t kElemFlagsAt = 0x14;     // 100h = shown
constexpr uint32_t kElemAnchorXAt = 0xA0;   // a root's child: its place in the root
constexpr uint32_t kElemAnchorYAt = 0xA4;
constexpr float kFlatUnitsX = 512.0f;
constexpr float kFlatUnitsY = 624.0f;
constexpr float kFlatAnchorX = 0.88f;
constexpr float kFlatAnchorY = 0.9f;
constexpr float kFlatOffY = 0.05f;
constexpr float kFlatScale = 0.6f;          // the panels and the window (their texts with them): bars 59% of the half wide
constexpr float kFlatBarH = 28.0f;          // the title bar, from the title's y down (the box starts there)
constexpr float kFlatBarInset = 18.0f;      // the title's x is this far in from the box's left edge
constexpr float kFlatBoxW = 506.9f;         // the box (and the bar) when the skin cannot be read
// the Xbox's shots (fractions of the half): the COMMAND POST bar's left edge at 26% (player 1) / 22%
// (player 2); the bars' tops at 49%, 56%, 67.5% down, the OBJECTIVES box to 85%; the class window's
// bar at the same left, its top at 46.8%, its box to 85%
constexpr float kXbPanelLeft[2] = { 0.26f, 0.22f };
constexpr float kXbPanelTop[3] = { 0.490f, 0.561f, 0.675f };
constexpr float kXbPanelGap = 0.006f;
constexpr float kXbObjBottom = 0.85f;
constexpr float kXbWinLeft[2] = { 0.26f, 0.22f };
constexpr float kXbWinTop = 0.468f;
constexpr float kXbWinBottom = 0.85f;
constexpr float kStretchMax = 8.0f;
int g_flatLines = 0;

bool FlatBudget()
{
    return g_flatLines++ < 400;
}

struct Bounds
{
    float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
    int corners = 0;
    int bitmaps = 0;
};

// a 2D affine transform (row vectors: x' = x a + y c + tx, y' = x b + y d + ty)
struct Affine
{
    float a = 1.0f, b = 0.0f, c = 0.0f, d = 1.0f, tx = 0.0f, ty = 0.0f;
};

Affine ElemAffine(uintptr_t e)
{
    float m[16] = {};
    ReadMem(e + kElemMatrix, m, sizeof(m));
    Affine t;
    t.a = m[0];
    t.b = m[1];
    t.c = m[4];
    t.d = m[5];
    t.tx = m[12];
    t.ty = m[13];
    return t;
}

// local (child) then parent
Affine Compose(const Affine& local, const Affine& parent)
{
    Affine r;
    r.a = local.a * parent.a + local.b * parent.c;
    r.b = local.a * parent.b + local.b * parent.d;
    r.c = local.c * parent.a + local.d * parent.c;
    r.d = local.c * parent.b + local.d * parent.d;
    r.tx = local.tx * parent.a + local.ty * parent.c + parent.tx;
    r.ty = local.tx * parent.b + local.ty * parent.d + parent.ty;
    return r;
}

void BoundsAdd(Bounds& b, float x, float y)
{
    if (!(x > -5000.0f && x < 5000.0f && y > -5000.0f && y < 5000.0f)) // (NaN fails too)
        return;
    if (!b.corners)
    {
        b.x0 = b.x1 = x;
        b.y0 = b.y1 = y;
    }
    else
    {
        b.x0 = x < b.x0 ? x : b.x0;
        b.x1 = x > b.x1 ? x : b.x1;
        b.y0 = y < b.y0 ? y : b.y0;
        b.y1 = y > b.y1 ? y : b.y1;
    }
    ++b.corners;
}

// the shown bitmaps under e, each corner through its element's matrices up to (not including) the
// root element's own; `t` = e's own transform in that element's units
void BoundsWalk(uintptr_t e, const Affine& t, Bounds& b, int depth, bool logEach)
{
    char t64[64];
    if (!e || depth > 6 || !TypeName(e, t64, sizeof(t64)) || !(ReadU32(e + kElemFlagsAt) & 0x100))
        return;
    if (strstr(t64, "Bitmap"))
    {
        ++b.bitmaps;
        float cx[4];
        float cy[4];
        for (uint32_t v = 0; v < 4; ++v)
        {
            const float x = ReadF32(e + kBmpCorners + v * kBmpCornerStep);
            const float y = ReadF32(e + kBmpCorners + v * kBmpCornerStep + 4);
            cx[v] = x * t.a + y * t.c + t.tx;
            cy[v] = x * t.b + y * t.d + t.ty;
            BoundsAdd(b, cx[v], cy[v]);
        }
        if (logEach && FlatBudget())
            Log("[wheel]   flat layout:   bitmap 0x%08X at %.1f %.1f, rows %.3f %.3f / %.3f %.3f: corners %.1f %.1f, %.1f %.1f, %.1f %.1f, "
                "%.1f %.1f -> %.1f %.1f .. %.1f %.1f",
                static_cast<unsigned>(e), ReadF32(e + kElemX), ReadF32(e + kElemY), ReadF32(e + kElemMatrix), ReadF32(e + kElemMatrix + 4),
                ReadF32(e + kElemMatrix + 16), ReadF32(e + kElemMatrix + 20), ReadF32(e + kBmpCorners), ReadF32(e + kBmpCorners + 4),
                ReadF32(e + kBmpCorners + kBmpCornerStep), ReadF32(e + kBmpCorners + kBmpCornerStep + 4),
                ReadF32(e + kBmpCorners + 2 * kBmpCornerStep), ReadF32(e + kBmpCorners + 2 * kBmpCornerStep + 4),
                ReadF32(e + kBmpCorners + 3 * kBmpCornerStep), ReadF32(e + kBmpCorners + 3 * kBmpCornerStep + 4), cx[0], cy[0], cx[3],
                cy[3]);
        return;
    }
    if (!strstr(t64, "Group"))
        return;
    if (logEach && depth > 0 && FlatBudget())
        Log("[wheel]   flat layout:   group 0x%08X at %.1f %.1f, rows %.3f %.3f / %.3f %.3f", static_cast<unsigned>(e), ReadF32(e + kElemX),
            ReadF32(e + kElemY), ReadF32(e + kElemMatrix), ReadF32(e + kElemMatrix + 4), ReadF32(e + kElemMatrix + 16),
            ReadF32(e + kElemMatrix + 20));
    const uintptr_t head = e + kElemKids;
    uintptr_t node = ReadU32(head);
    for (int guard = 0; node && node != head && guard < 256; ++guard)
    {
        const uintptr_t c = ReadU32(node + 0x0C);
        if (c)
            BoundsWalk(c, Compose(ElemAffine(c), t), b, depth + 1, logEach);
        node = ReadU32(node);
    }
}

// a panel's box (its skin's bitmaps) in the panel's own units (its origin at 0, 0; its own matrix not
// applied), and its title's place. True when the title is there and the box is plausible; on the last
// try `b` is kFlatBoxW x 30 under the title when the box could not be read (logged), else false then
bool PanelGeometry(uintptr_t e, uintptr_t title, const char* what, Bounds* b, float* tx, float* ty, bool logEach, bool lastTry)
{
    if (!e || !title)
        return false;
    *tx = ReadF32(title + kElemX);
    *ty = ReadF32(title + kElemY);
    if (!(*tx > -2000.0f && *tx < 2000.0f && *ty > -2000.0f && *ty < 2000.0f))
        return false;
    Bounds r;
    const uintptr_t head = e + kElemKids;
    uintptr_t node = ReadU32(head);
    for (int guard = 0; node && node != head && guard < 256; ++guard)
    {
        const uintptr_t c = ReadU32(node + 0x0C);
        if (c && c != title)
            BoundsWalk(c, Compose(ElemAffine(c), Affine()), r, 1, logEach);
        node = ReadU32(node);
    }
    const float w = r.x1 - r.x0;
    const float h = r.y1 - r.y0;
    const bool ok = r.corners >= 4 && w >= 100.0f && w <= 1500.0f && h >= 4.0f && h <= 1500.0f && *tx >= r.x0 - 60.0f &&
                    *tx <= r.x1 + 60.0f && *ty >= r.y0 - 400.0f && *ty <= r.y1 + 400.0f;
    if (FlatBudget())
        Log("[wheel] %lu ms: flat layout: %s 0x%08X: box of %d bitmap(s), x %.1f .. %.1f, y %.1f .. %.1f (%.1f x %.1f); its title at "
            "%.1f %.1f%s",
            ScanMsSinceAttach(), what, static_cast<unsigned>(e), r.bitmaps, r.x0, r.x1, r.y0, r.y1, w, h, *tx, *ty,
            ok ? "" : (lastTry ? " - box NOT read: 507 x 30 under the title assumed" : " - box NOT read: tried again later"));
    if (ok)
        *b = r;
    else if (!lastTry)
        return false;
    else
    {
        b->x0 = *tx - kFlatBarInset;
        b->x1 = b->x0 + kFlatBoxW;
        b->y0 = *ty + kFlatBarH;
        b->y1 = b->y0 + 30.0f;
        b->corners = 0;
        b->bitmaps = 0;
    }
    return true;
}

// a matrix's rotation rows set to (sx, sy, 1); the translation and depth kept
void SetRows(uintptr_t e, float sx, float sy)
{
    float* const m = reinterpret_cast<float*>(e + kElemMatrix);
    m[0] = sx;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[4] = 0.0f;
    m[5] = sy;
    m[6] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
}

// a group straight and pinned at 0, 0 (its rows logged first)
void Straighten(uintptr_t group, const char* what)
{
    float m[16] = {};
    ReadMem(group + kElemMatrix, m, sizeof(m));
    SetRows(group, 1.0f, 1.0f);
    g_rf.setPos(reinterpret_cast<void*>(group), nullptr, 0.0f, 0.0f);
    if (FlatBudget())
        Log("[wheel] %lu ms: flat layout: %s 0x%08X set straight (was %.3f %.3f %.3f / %.3f %.3f %.3f / %.3f %.3f %.3f) at 0 0, "
            "anchor %.2f %.2f",
            ScanMsSinceAttach(), what, static_cast<unsigned>(group), m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10],
            ReadF32(group + kElemAnchorXAt), ReadF32(group + kElemAnchorYAt));
}

// the box (the skin group, measured on its own in the panel's units) stretched down to `bottom` (a
// fraction of the half) when it ends above it: its y rows x ky (whatever they were), its y moved so its
// top stays; nothing when the box is not under the title or cannot be read
void StretchBox(uintptr_t skin, float ty, float s, float top, float bottom, const char* what)
{
    if (!skin)
        return;
    Bounds b;
    BoundsWalk(skin, Compose(ElemAffine(skin), Affine()), b, 1, false);
    const float h = b.y1 - b.y0;
    if (b.corners < 4 || h < 4.0f || h > 1500.0f || b.y0 < ty - 5.0f)
        return;
    const float boxTop = top + s * (b.y0 - ty) / kFlatUnitsY; // (of the half)
    const float have = s * h / kFlatUnitsY;
    const float want = bottom - boxTop;
    if (have <= 0.0f || want <= have * 1.02f)
        return;
    float ky = want / have;
    if (ky > kStretchMax)
        ky = kStretchMax;
    // the skin's rows scale its children about its own origin: the box top (y0, with the skin's y and
    // rows in it) goes to sy0 + (y0 - sy0) ky, so the skin moves up by the difference
    float* const m = reinterpret_cast<float*>(skin + kElemMatrix);
    const float sy0 = m[13];
    const float newY = sy0 + (b.y0 - sy0) * (1.0f - ky);
    m[4] *= ky;
    m[5] *= ky;
    m[6] *= ky;
    g_rf.setPos(reinterpret_cast<void*>(skin), nullptr, m[12], newY);
    if (FlatBudget())
        Log("[wheel] %lu ms: flat layout: %s box (skin 0x%08X, %.1f tall) stretched x %.2f (its y %.1f -> %.1f): %.3f .. %.3f down (of "
            "the half)",
            ScanMsSinceAttach(), what, static_cast<unsigned>(skin), h, ky, sy0, newY, boxTop, boxTop + have * ky);
}

// the root's x offset as drawn: player 1's 0.1 (his map's anchor 0.477 drew at 52%), player 2's 0 (his
// root's own offset; 19h-r1's 0.02 came from texts in his turned Info group, not from a flat one)
float FlatOffX(int player)
{
    return player == 0 ? 0.1f : 0.0f;
}
} // namespace

// 19h: the frames a screen's flat layout is tried on (its elements can join a frame or two late): 1, 2,
// 3, 5, 10, 20 and 30 (kLayoutTries, the last)
bool ClassWheelLayoutTryNow(int tries)
{
    return tries == 1 || tries == 2 || tries == 3 || tries == 5 || tries == 10 || tries == 20 || tries >= kLayoutTries;
}

// 19h: the spawn point screen's three panels (COMMAND POST, VEHICLES, OBJECTIVES) flat where the
// Xbox has them: each scaled kFlatScale, its bar's top left at the Xbox's place; the OBJECTIVES box
// stretched to the Xbox's bottom. skin[3] = the panels' Skin groups (the boxes; 0 = no stretch).
// False (nothing changed) when a title is missing.
bool ClassWheelXbPanels(int player, uintptr_t sideBar, const uintptr_t* panel, const uintptr_t* title, const uintptr_t* skin, const char* who,
                        bool lastTry)
{
    if (player < 0 || player > 1 || !sideBar || !panel || !title || !g_rf.setPos)
        return false;
    Bounds b[3];
    float tx[3] = {};
    float ty[3] = {};
    static const char* const kNames[3] = { "COMMAND POST panel", "VEHICLES panel", "OBJECTIVES panel" };
    static int s_detail = 0; // the first panel's bitmaps listed, the first few times
    for (int i = 0; i < 3; ++i)
    {
        char what[96];
        snprintf(what, sizeof(what), "%s %s", who, kNames[i]);
        if (!PanelGeometry(panel[i], title[i], what, &b[i], &tx[i], &ty[i], i == 0 && s_detail < 2, lastTry))
        {
            if (FlatBudget())
                Log("[wheel] %lu ms: flat layout: %s %s - no title / panel / box yet: left as it is", ScanMsSinceAttach(), who, kNames[i]);
            return false;
        }
    }
    if (s_detail < 2)
        ++s_detail;
    const float s = kFlatScale;
    Straighten(sideBar, who);
    const float ax = ReadF32(sideBar + kElemAnchorXAt);
    const float ay = ReadF32(sideBar + kElemAnchorYAt);
    float bottom = 0.0f;
    for (int i = 0; i < 3; ++i)
    {
        // the bar's top left (the title's y, the box's left edge) at the Xbox's place; a panel never
        // over the one above it
        float top = kXbPanelTop[i];
        if (i > 0 && top < bottom + kXbPanelGap)
            top = bottom + kXbPanelGap;
        const float left = kXbPanelLeft[player];
        const float x = (left - FlatOffX(player) - kFlatAnchorX * ax) * kFlatUnitsX - s * b[i].x0;
        const float y = (top - kFlatOffY - kFlatAnchorY * ay) * kFlatUnitsY - s * ty[i];
        g_rf.setPos(reinterpret_cast<void*>(panel[i]), nullptr, x, y);
        SetRows(panel[i], s, s);
        bottom = top + s * (b[i].y1 - ty[i]) / kFlatUnitsY;
        if (FlatBudget())
            Log("[wheel] %lu ms: flat layout: %s %s 0x%08X at %.1f %.1f, scale %.2f: %.3f .. %.3f across, %.3f .. %.3f down (of the half)",
                ScanMsSinceAttach(), who, kNames[i], static_cast<unsigned>(panel[i]), x, y, s, left,
                left + s * (b[i].x1 - b[i].x0) / kFlatUnitsX, top, bottom);
        if (i == 2 && skin)
            StretchBox(skin[2], ty[2], s, top, kXbObjBottom, "the OBJECTIVES");
    }
    return true;
}

// 19h: the class window flat where the Xbox has it (under the ring), its box stretched to the Xbox's
// bottom; false = nothing changed (its title is missing)
bool ClassWheelXbWindow(int player, uintptr_t info, uintptr_t window, uintptr_t title, uintptr_t skin, const char* who, bool lastTry)
{
    if (player < 0 || player > 1 || !info || !window || !title || !g_rf.setPos)
        return false;
    Bounds b;
    float tx = 0.0f;
    float ty = 0.0f;
    char what[96];
    snprintf(what, sizeof(what), "%s class window", who);
    static int s_detail = 0;
    if (!PanelGeometry(window, title, what, &b, &tx, &ty, s_detail < 2, lastTry))
    {
        if (FlatBudget())
            Log("[wheel] %lu ms: flat layout: %s - no title / window / box yet: left as it is", ScanMsSinceAttach(), what);
        return false;
    }
    if (s_detail < 2)
        ++s_detail;
    const float s = kFlatScale;
    Straighten(info, who);
    const float ax = ReadF32(info + kElemAnchorXAt);
    const float ay = ReadF32(info + kElemAnchorYAt);
    const float left = kXbWinLeft[player];
    const float x = (left - FlatOffX(player) - kFlatAnchorX * ax) * kFlatUnitsX - s * b.x0;
    const float y = (kXbWinTop - kFlatOffY - kFlatAnchorY * ay) * kFlatUnitsY - s * ty;
    g_rf.setPos(reinterpret_cast<void*>(window), nullptr, x, y);
    SetRows(window, s, s);
    if (FlatBudget())
        Log("[wheel] %lu ms: flat layout: %s 0x%08X at %.1f %.1f, scale %.2f: %.3f .. %.3f across, %.3f .. %.3f down (of the half)",
            ScanMsSinceAttach(), what, static_cast<unsigned>(window), x, y, s, left, left + s * (b.x1 - b.x0) / kFlatUnitsX, kXbWinTop,
            kXbWinTop + s * (b.y1 - ty) / kFlatUnitsY);
    StretchBox(skin, ty, s, kXbWinTop, kXbWinBottom, "the class window's");
    return true;
}

// 19h: a flat group's (x, y) for a place in fractions of player 2's half (the same mapping; false when
// the group is not straight - not one ClassWheelXbWindow / Panels straightened)
bool ClassWheelFlatPlace(int player, uintptr_t group, float fx, float fy, float* x, float* y)
{
    if (player < 0 || player > 1 || !group)
        return false;
    float m[16] = {};
    if (!ReadMem(group + kElemMatrix, m, sizeof(m)))
        return false;
    const bool straight = m[0] > 0.999f && m[0] < 1.001f && m[2] > -0.001f && m[2] < 0.001f && m[8] > -0.001f && m[8] < 0.001f;
    if (!straight)
        return false;
    *x = (fx - FlatOffX(player) - kFlatAnchorX * ReadF32(group + kElemAnchorXAt)) * kFlatUnitsX - ReadF32(group + kElemX);
    *y = (fy - kFlatOffY - kFlatAnchorY * ReadF32(group + kElemAnchorYAt)) * kFlatUnitsY - ReadF32(group + kElemY);
    return true;
}

// 19h: the vehicles at a post as the game writes them for its PC popup (its own PopupFill, through
// VehTextHook, which keeps the string instead of setting it); player 1's post filled again after it
// (the popup's title back to his; his panel's text untouched). Game thread only.
bool ClassWheelVehiclesAt(uintptr_t post, wchar_t* out, size_t cap)
{
    if (!out || cap == 0)
        return false;
    out[0] = 0;
    const uintptr_t sd = g_global ? ReadU32(g_global) : 0;
    if (!post || !sd || !g_popupHooked || !g_popupFillRva)
        return false;
    typedef void(__fastcall * PopupFill_t)(void* sd, void* unusedEdx, uintptr_t post);
    const PopupFill_t fill = reinterpret_cast<PopupFill_t>(Rva(g_popupFillRva));
    g_vehCaptured[0] = 0;
    g_vehCapture = 1;
    fill(reinterpret_cast<void*>(sd), nullptr, post);
    g_vehCapture = 2;
    const uintptr_t own = ReadU32(sd + kSdPost);
    if (own)
        fill(reinterpret_cast<void*>(sd), nullptr, own);
    g_vehCapture = 0;
    wcsncpy_s(out, cap, g_vehCaptured, _TRUNCATE);
    if (g_vehLines++ < 100)
    {
        char a[300] = "";
        WideCharToMultiByte(CP_UTF8, 0, out, -1, a, static_cast<int>(sizeof(a)) - 1, nullptr, nullptr);
        a[sizeof(a) - 1] = 0;
        Log("[wheel] %lu ms: vehicles at post 0x%08X (for player 2): \"%s\" (PopupFill rva 0x%08X; player 1's post 0x%08X filled "
            "again)",
            ScanMsSinceAttach(), static_cast<unsigned>(post), a, g_popupFillRva, static_cast<unsigned>(own));
    }
    return true;
}

bool ClassWheelMoveElement(uintptr_t e, float x, float y, const char* what)
{
    if (!e || !g_rf.setPos)
    {
        if (DriverBudget())
            Log("[wheel] %lu ms: layout: %s - element MISSING, not moved", ScanMsSinceAttach(), what);
        return false;
    }
    const float wasX = ReadF32(e + kElemX);
    const float wasY = ReadF32(e + kElemY);
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    // 19c: the element's own anchor in its root (+A0h / +A4h: set for a root's child) and its list owner
    // 19g: and its matrix's rotation / scale rows and depth (the PC tilts the panels; the Xbox does not);
    // its own line budget (player 2's screens move theirs each time they come up)
    static int s_moveLines = 0;
    if (s_moveLines++ < 160)
    {
        float m[16] = {};
        ReadMem(e + kElemMatrix, m, sizeof(m));
        Log("[wheel] %lu ms: layout: %s 0x%08X moved %.0f %.0f -> %.0f %.0f (anchor %.2f %.2f, in the list at 0x%08X); "
            "matrix %.3f %.3f %.3f / %.3f %.3f %.3f / %.3f %.3f %.3f, z %.1f",
            ScanMsSinceAttach(), what, static_cast<unsigned>(e), wasX, wasY, x, y, ReadF32(e + 0xA0), ReadF32(e + 0xA4),
            static_cast<unsigned>(ReadU32(e + 0x24)), m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10], m[14]);
    }
    return true;
}

// 19g: a player's split layout as the game had it, table only (nothing re-applied: a match being torn down)
void ClassWheelSplitMapReset(int player)
{
    if (player < 0 || player > 1 || !g_mapOk || !g_mapTable || !g_splitMapSaved)
        return;
    g_splitMapWant[player] = 0;
    memcpy(reinterpret_cast<void*>(g_mapTable + kMapLayout1 * static_cast<uint32_t>(SplitLayoutOf(player))), g_splitMapOrig[player],
           sizeof(g_splitMapOrig[player]));
}

void ClassWheelSplitMapBig(int player, bool on)
{
    if (player < 0 || player > 1)
        return;
    g_splitMapWant[player] = on ? 2 : 0; // 19g: layout 4 (player 2) / 5 (player 1)
    SplitMapLayout();
}

bool ClassWheelSplitWanted()
{
    return g_wheelSplit; // 21j-r18: Enabled is single player's only (it needed both before)
}

// 21j-r18: the main menu's GAME OPTIONS SPAWN SCREEN row
int ClassWheelMenuState()
{
    if (!g_wheelStarted)
        return -1;
    return g_wheelEnabled ? 1 : 0;
}

int ClassWheelMenuSet(bool on)
{
    if (!g_wheelStarted)
        return -1;
    const bool was = g_wheelEnabled;
    g_wheelEnabled = on;
    // the flow state back at its start (the side screen) when the class wheel is chosen and it stood on the PC screen (F6, or
    // both flows off at game start); PC MENU leaves it for split screen's flow
    if (on)
        InterlockedCompareExchange(&g_previewIdx, 1, 0);
    const BOOL saved = g_wheelIni[0] ? WritePrivateProfileStringA("ClassWheel", "Enabled", on ? "1" : "0", g_wheelIni) : FALSE;
    Log("[wheel] %lu ms: 21j-r18: GAME OPTIONS' SPAWN SCREEN: %s (was %s) - 1-player games get %s from the next spawn screen on; "
        "split screen as [ClassWheel] Split=%d says; saved in BF1GameExt.ini [ClassWheel] Enabled=%d%s",
        ScanMsSinceAttach(), on ? "CLASS WHEEL" : "PC MENU", was ? "CLASS WHEEL" : "PC MENU",
        on ? "the console flow (side, spawn point, class wheel)" : "the game's own PC spawn screen", g_wheelSplit ? 1 : 0,
        on ? 1 : 0, saved ? "" : " - NOT SAVED (the write failed)");
    return g_wheelEnabled ? 1 : 0;
}

// v7.4: the MODS page's SIDE CAROUSEL row (1 player; split screen always has it): -1 = no spawn screen this build
int ClassWheelSideCarouselState()
{
    if (!g_wheelStarted)
        return -1;
    return g_sideCarousel ? 1 : 0;
}
int ClassWheelSideCarouselSet(bool on)
{
    if (!g_wheelStarted)
        return -1;
    g_sideCarousel = on;
    const BOOL saved = g_wheelIni[0] ? WritePrivateProfileStringA("ClassWheel", "SideCarousel", on ? "1" : "0", g_wheelIni) : FALSE;
    Log("[wheel] %lu ms: v7.4: MODS page SIDE CAROUSEL: %s - 1-player CHOOSE YOUR SIDE with more than two sides %s; saved in "
        "BF1GameExt.ini [ClassWheel] SideCarousel=%d%s",
        ScanMsSinceAttach(), on ? "ON" : "OFF", on ? "is a carousel" : "in rows", on ? 1 : 0, saved ? "" : " - NOT SAVED (the write failed)");
    return g_sideCarousel ? 1 : 0;
}

// 20g-r12: horizontal 2-player, the Classic Collection's layout (the HUD roots are split_spawn_menus.cpp's to read)
bool ClassWheelHorizontal()
{
    // (the HUD roots' shape decides, not the INI's Vertical: vertical halves are 512 x 624; if the vertical split could not be
    // installed the halves are horizontal and get the horizontal layout)
    // (asked many times a frame: the answer kept for 50 ms - the roots' safe reads are system calls)
    static DWORD s_at = 0;
    static bool s_known = false;
    static bool s_hz = false;
    const DWORD now = GetTickCount();
    if (!s_known || now - s_at >= 50)
    {
        s_hz = g_hzWanted && LocalPlayersNow() == 2 && PlayerNLocalPlayers() < 3 && P2SpawnRootsHorizontal();
        s_at = now;
        s_known = true;
    }
    return s_hz;
}

// ---- 20g-r9: FlashyTextElements made like the game's; element trees searched and logged -------------------------
namespace {
const char* const kFlashyCtorBody = "56 8B F1 E8 ?? ?? ?? ?? 8D 8E C0 00 00 00 C7 06 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 8B CE "
                                    "E8 ?? ?? ?? ?? 8B C6 5E C3";
constexpr uint32_t kFlashyCtorVtable = 16;  // C7 06 <vtable>
constexpr uint32_t kFlashySize = 0x100;     // the IF Lua's IFFlashyText: new(100h) (GOG 1.3 0x0017065C)
constexpr uint32_t kFlashyFirst = 0xD0;     // its own fields: +D0h .. +F7h (+F8h is its draw resource)
constexpr uint32_t kFlashyEnd = 0xF8;
constexpr uint32_t kFlashyInitSlot = 0x30;  // vfunc 30h: makes the draw resource (the IF Lua calls it after the constructor)
constexpr uint32_t kFlashyBarWidth = 0xF0;  // 21j-r9: the bar's width (a word; IFFlashyText's Setup, GOG 1.3 0x00177F20)
// 21j-r15: its animation (GOG 1.3: the update 0x00131430 adds dt to +DCh and steps the state - +F4h's low 3 bits - through a
// table of times until 6; the constructor's init 0x00130A30 sets 6 and 0; the draw 0x00130C10: the bar in states 1 - 7, growing
// with the timer in 1; the text from state 4)
constexpr uint32_t kFlashyTimer = 0xDC;
constexpr uint32_t kFlashyFlags = 0xF4;
constexpr uint32_t kFlashyStateMask = 7;
int g_flashyStateLines = 0;
constexpr uint32_t kTextBoxW = 0xA0;        // 21j-r9: RedTextElement's text box (words; SetTextBox, GOG 1.3 0x0029EEE0)
constexpr uint32_t kTextBoxH = 0xA2;
const char* const kFlashyClass = "FlashyTextElement";
ElemCtor_t g_flashyCtor = nullptr;
int g_treeLines = 0;

void FindFlashyCtor()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kFlashyCtorBody, hits, 4);
    const uintptr_t vt = n == 1 ? ReadU32(Rva(hits[0] + kFlashyCtorVtable)) : 0;
    uintptr_t fake = vt;
    char t[64] = "";
    const bool typeOk = vt && TypeName(reinterpret_cast<uintptr_t>(&fake), t, sizeof(t)) && strcmp(t, kFlashyClass) == 0;
    if (typeOk && g_rf.gameNew && g_rf.addChild && TextOk())
        g_flashyCtor = reinterpret_cast<ElemCtor_t>(Rva(hits[0]));
    Log("[wheel] 20g-r9: FlashyTextElement constructor: %d pattern hit(s), vtable rva 0x%08X (%s) - %s", n,
        vt ? static_cast<unsigned>(vt - ExeBase()) : 0u, typeOk ? t : "NOT FlashyTextElement",
        g_flashyCtor ? "players 3 and 4's pause screens can copy the game's menu bars" : "NOT found (their pause screens stay texts)");
}

void TreeWalk(uintptr_t e, const char* type, const Affine& parent, int depth, uintptr_t* out, float* xs, float* ys, int max, int* n,
              bool log)
{
    if (!e || depth > 12 || (type && *n >= max)) // a dump (no type) walks the whole tree
        return;
    const Affine a = Compose(ElemAffine(e), parent);
    char t[64] = "";
    TypeName(e, t, sizeof(t));
    if (log && g_treeLines < 400)
    {
        ++g_treeLines;
        char extra[200] = "";
        if (strcmp(t, kFlashyClass) == 0)
            snprintf(extra, sizeof(extra), " | text scale %.2f, text colour %08X, bar %08X %08X %08X, margins %08X, +F0h %04X, +F2h %u, "
                     "flags +F4h %08X", static_cast<double>(ReadF32(e + kTextScaleX)), ReadU32(e + 0xD4), ReadU32(e + 0xE0),
                     ReadU32(e + 0xE4), ReadU32(e + 0xE8), ReadU32(e + 0xEC), ReadU32(e + 0xF0) & 0xFFFF,
                     static_cast<unsigned>(ReadU32(e + 0xF0) >> 16 & 0xFF), ReadU32(e + 0xF4));
        else if (strcmp(t, "RedTextElement") == 0)
            snprintf(extra, sizeof(extra), " | text scale %.2f, flags %08X", static_cast<double>(ReadF32(e + kTextScaleX)),
                     ReadU32(e + kTextFlags));
        Log("[wheel] tree: %*s%s 0x%08X at %.1f %.1f (rows %.2f %.2f / %.2f %.2f), %s, colour %08X%s", depth * 2, "", t[0] ? t : "?",
            static_cast<unsigned>(e), static_cast<double>(a.tx), static_cast<double>(a.ty), static_cast<double>(a.a),
            static_cast<double>(a.b), static_cast<double>(a.c), static_cast<double>(a.d),
            (ReadU32(e + kElemFlagsAt) & 0x100) ? "shown" : "hidden", ReadU32(e + kElemColor), extra);
    }
    if (type && strcmp(t, type) == 0 && *n < max)
    {
        out[*n] = e;
        xs[*n] = a.tx;
        ys[*n] = a.ty;
        ++*n;
    }
    if (!strstr(t, "Group") && !strstr(t, "Container"))
        return;
    const uintptr_t head = e + kElemKids;
    uintptr_t node = ReadU32(head);
    for (int guard = 0; node && node != head && guard < 256; ++guard)
    {
        TreeWalk(ReadU32(node + 0x0C), type, a, depth + 1, out, xs, ys, max, n, log);
        node = ReadU32(node);
    }
}
} // namespace

int ClassWheelFindKind(uintptr_t e, const char* type, float ax, float ay, uintptr_t* out, float* xs, float* ys, int max)
{
    Affine at;
    at.tx = ax;
    at.ty = ay;
    int n = 0;
    TreeWalk(e, type, at, 0, out, xs, ys, max, &n, false);
    return n;
}

void ClassWheelDumpTree(uintptr_t e, const char* what, float ax, float ay)
{
    if (!e || g_treeLines >= 400)
        return;
    Log("[wheel] tree: %s (its root anchor %.1f %.1f):", what, static_cast<double>(ax), static_cast<double>(ay));
    Affine at;
    at.tx = ax;
    at.ty = ay;
    int n = 0;
    uintptr_t dummy = 0;
    float dx = 0.0f;
    float dy = 0.0f;
    TreeWalk(e, nullptr, at, 0, &dummy, &dx, &dy, 0, &n, true);
}

bool ClassWheelIsFlashy(uintptr_t e)
{
    char t[64] = "";
    return e && TypeName(e, t, sizeof(t)) && strcmp(t, kFlashyClass) == 0;
}

uintptr_t ClassWheelFlashyLike(uintptr_t group, uintptr_t like, const wchar_t* s, float x, float y)
{
    if (!group || !g_flashyCtor || !ClassWheelIsFlashy(like))
        return 0;
    void* mem = g_rf.gameNew(kFlashySize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_flashyCtor(mem, nullptr));
    if (!ClassWheelIsFlashy(e))
    {
        ElemDelete(e); // not left behind (its own deleting destructor frees the block)
        return 0;
    }
    const uintptr_t init = VirtualAt(e, kFlashyInitSlot);
    if (init)
        reinterpret_cast<ElemNoArg_t>(init)(reinterpret_cast<void*>(e), nullptr);
    // the text as the game's (as TextLike: font object, justify, scale, colour), then its own fields - not its resource
    const uintptr_t font = ReadU32(like + kTextFont);
    if (font && g_txFontObj)
        g_txFontObj(reinterpret_cast<void*>(e), nullptr, font);
    else
        g_txFont(reinterpret_cast<void*>(e), nullptr, "gamefont_medium");
    const uint32_t flags = ReadU32(like + kTextFlags);
    g_txJustify(reinterpret_cast<void*>(e), nullptr, static_cast<int>((flags >> 24) & 3), static_cast<int>((flags >> 26) & 3));
    *reinterpret_cast<float*>(e + kTextScaleX) = ReadF32(like + kTextScaleX);
    *reinterpret_cast<float*>(e + kTextScaleY) = ReadF32(like + kTextScaleY);
    *reinterpret_cast<uint32_t*>(e + kElemColor) = ReadU32(like + kElemColor);
    // 21j-r15: its animation as the constructor left it - done (state 6: the whole bar and the text), not the game's bar's: a
    // bar copied while the game's screen flashes it in (the game's own Enter) came out stuck at its start - a stub of orange,
    // no title (the 4-player screenshot after r14: player 1's windows, made in the frame his spawn point screen comes up).
    // Nothing of ours runs a copy's animation (the game animates only the texts its Flash put in its list).
    const uint32_t state = ReadU32(e + kFlashyFlags) & kFlashyStateMask;
    const float timer = ReadF32(e + kFlashyTimer);
    const uint32_t likeState = ReadU32(like + kFlashyFlags) & kFlashyStateMask;
    ReadMem(like + kFlashyFirst, reinterpret_cast<void*>(e + kFlashyFirst), kFlashyEnd - kFlashyFirst);
    *reinterpret_cast<uint32_t*>(e + kFlashyFlags) = (ReadU32(e + kFlashyFlags) & ~kFlashyStateMask) | state;
    *reinterpret_cast<float*>(e + kFlashyTimer) = timer;
    if (likeState != state && g_flashyStateLines < 12)
    {
        ++g_flashyStateLines;
        Log("[wheel] %lu ms: 21j-r15: a bar copied from 0x%08X while it was in its animation (state %u, %.2f s) - the copy keeps the "
            "constructor's state %u (the whole bar and its text)",
            ScanMsSinceAttach(), static_cast<unsigned>(like), likeState, static_cast<double>(ReadF32(like + kFlashyTimer)), state);
    }
    g_setTextW(reinterpret_cast<void*>(e), nullptr, s ? s : L"");
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, static_cast<int>(ReadU32(like + 0x14) & 0xFF));
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    return e;
}

// 21j-r9: e in group (the game's AddChild: false if e is in a group already)
bool ClassWheelAddChild(uintptr_t group, uintptr_t e)
{
    return group && e && g_rf.addChild && g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
}

// 21j-r9: e at (x, y) of its group with its rows (sx, sy, 1) - what it holds drawn that much bigger / smaller (19h's flat panels)
bool ClassWheelPlaceScaled(uintptr_t e, float x, float y, float sx, float sy)
{
    if (!e || !g_rf.setPos)
        return false;
    float* const m = reinterpret_cast<float*>(e + kElemMatrix);
    m[0] = sx;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[4] = 0.0f;
    m[5] = sy;
    m[6] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    return true;
}

// 21j-r9: a FlashyTextElement's bar width (+F0h, a word: IFFlashyText's Setup writes its width there, GOG 1.3 0x00177F20)
bool ClassWheelFlashyBarWidth(uintptr_t e, int w)
{
    if (!ClassWheelIsFlashy(e) || w < 0 || w > 4000)
        return false;
    *reinterpret_cast<int16_t*>(e + kFlashyBarWidth) = static_cast<int16_t>(w);
    return true;
}

// 21j-r9: a text in one of split_spawn_menus.cpp's console windows, as the game's box texts (ifs_charselect's BoxText): left / top,
// its own font and scale, white, wrapped in a box of boxW x boxH units (RedTextElement +A0h / +A2h, the words the IF Lua's
// SetTextBox writes - GOG 1.3 0x0029EEE0 - set before the string, which is laid out in it), at `layer`
uintptr_t ClassWheelBoxTextNew(uintptr_t group, const char* font, const wchar_t* s, float x, float y, float scale, int boxW, int boxH,
                               int layer)
{
    if (!group || !ClassWheelTextsReady())
        return 0;
    void* mem = g_rf.gameNew(kTextSize);
    if (!mem)
        return 0;
    const uintptr_t e = reinterpret_cast<uintptr_t>(g_txCtor(mem, nullptr));
    char t[64] = "";
    if (!e || !TypeName(e, t, sizeof(t)) || strcmp(t, "RedTextElement") != 0)
    {
        ElemDelete(e); // (not left behind: its own deleting destructor frees the block)
        return 0;
    }
    g_txFont(reinterpret_cast<void*>(e), nullptr, font ? font : "gamefont_small");
    g_txJustify(reinterpret_cast<void*>(e), nullptr, 0, 0); // the IF Lua's "left", "top"
    *reinterpret_cast<float*>(e + kTextScaleX) = scale;
    *reinterpret_cast<float*>(e + kTextScaleY) = scale;
    *reinterpret_cast<uint32_t*>(e + kElemColor) = 0xFFFFFFFFu;
    *reinterpret_cast<int16_t*>(e + kTextBoxW) = static_cast<int16_t>(boxW < 0 ? 0 : (boxW > 8000 ? 8000 : boxW));
    *reinterpret_cast<int16_t*>(e + kTextBoxH) = static_cast<int16_t>(boxH < 0 ? 0 : (boxH > 8000 ? 8000 : boxH));
    g_setTextW(reinterpret_cast<void*>(e), nullptr, s ? s : L"");
    g_rf.setPos(reinterpret_cast<void*>(e), nullptr, x, y);
    g_rf.setLayer(reinterpret_cast<void*>(e), nullptr, layer);
    g_rf.addChild(reinterpret_cast<void*>(group), nullptr, reinterpret_cast<void*>(e));
    g_rf.setVisible(reinterpret_cast<void*>(e), nullptr, 1);
    return e;
}

long ClassWheelSpawnScreenDeletes(bool* known)
{
    *known = g_sdDeleteHooked;
    return g_sdDeletes;
}

// 20g-r10: player 1's side screen in players 3 / 4's look (4-player) - split_spawn_menus.cpp allows it (its group can be
// made). 20g-r11: his spawn point and class screens too (ClassWheelP1Look: what split_spawn_menus.cpp draws them from)
void ClassWheelP1LookAllow(bool on)
{
    if (on != g_sideLookAllowed && g_sideLookLines++ < 12)
        Log("[wheel] 20g-r11: player 1's spawn screens in players 3 / 4's look %s", on ? "allowed (4-player)" : "NOT drawn - his own as r9");
    g_sideLookAllowed = on;
}

// v7.3: player 1's side screen's sides (teams, how many; total = the match's sides) - for his side screen in players 3 / 4's look
int ClassWheelP1Sides(int* teams, int max, int* total)
{
    int k = 0;
    for (int i = 0; i < g_side.count && k < max; ++i)
        teams[k++] = g_side.team[i];
    if (total)
        *total = g_side.total;
    return k;
}

bool ClassWheelP1Look(int* screen, int* team, uintptr_t* post, int* cls, int* pick)
{
    // (the chooser does not run while the world is paused - players 3 / 4's pause screens: the look stays, frozen; it ends
    // with the console flow, the spawn screen's deletion, and split_spawn_menus.cpp's top-screen test - 5 s only as a last resort)
    if (!g_p1Look.screen || GetTickCount() - g_p1Look.at > 5000)
        return false;
    if (g_p1Look.screen == 1 && (!g_side.made || !g_side.look || GetTickCount() - g_side.lookAt > 5000))
        return false; // (his side screen's own state not ready yet)
    *screen = static_cast<int>(g_p1Look.screen);
    *team = g_p1Look.team;
    *post = g_p1Look.post;
    *cls = g_p1Look.cls;
    *pick = g_side.pick;
    return true;
}

namespace {
// 20f-r2 / r4: RedScreenGroupElement's default constructor by pattern; its vtable (RTTI) and deleting destructor checked
void FindGroupCtor()
{
    uint32_t hits[4] = {};
    const int n = FindPattern(kGroupCtorBody, hits, 4);
    const uintptr_t vt = n == 1 ? ReadU32(Rva(hits[0] + kGroupCtorVtable)) : 0;
    uintptr_t fake = vt; // an "object" whose vtable is vt, for TypeName
    char t[64] = "";
    const bool typeOk = vt && TypeName(reinterpret_cast<uintptr_t>(&fake), t, sizeof(t)) && strcmp(t, kGroupClass) == 0;
    const uintptr_t del = typeOk ? ReadU32(vt) : 0;
    uint32_t delRva = (del && InExeCodeVa(del)) ? static_cast<uint32_t>(del - ExeBase()) : 0;
    for (int hop = 0; hop < 8 && delRva && *reinterpret_cast<const unsigned char*>(Rva(delRva)) == 0xE9; ++hop)
        delRva = static_cast<uint32_t>(static_cast<int64_t>(delRva) + 5 + ReadI32At(delRva + 1));
    const bool delOk = delRva && MatchPatternAt(delRva, kGroupDeleteBody);
    if (n == 1 && typeOk && delOk && g_rf.gameNew && g_rf.addChild)
        g_groupCtor = reinterpret_cast<ElemCtor_t>(Rva(hits[0]));
    Log("[wheel] 20f-r4: RedScreenGroupElement constructor: %d pattern hit(s)%s, vtable rva 0x%08X (%s), deleting destructor rva "
        "0x%08X %s, the game's new %s - %s",
        n, n == 1 ? "" : " (need exactly 1)", vt ? static_cast<unsigned>(vt - ExeBase()) : 0u, typeOk ? t : "NOT RedScreenGroupElement",
        delRva, delOk ? "OK (frees B0h)" : "does NOT match", g_rf.gameNew ? "OK" : "NOT found",
        g_groupCtor ? "players 3 and 4's spawn screens can have groups of their own" : "NO groups (players 3 and 4 keep the auto-spawn)");
}
} // namespace

void ClassWheelScanStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[wheel] not used on this build");
        return;
    }
    Log("[wheel] class_wheel.cpp file build %s, spawn_manager.cpp file build %s - CLASS WHEEL SCAN (log-only: no "
        "hooks, nothing written)",
        kFileBuild, SpawnScanFileBuild());
    // 18p: [ClassWheel] Enabled in BF1GameExt.ini
    {
        char exe[MAX_PATH] = {};
        char ini[MAX_PATH + 32] = {};
        if (GetModuleFileNameA(nullptr, exe, MAX_PATH))
        {
            char* slash = strrchr(exe, '\\');
            if (slash)
                *slash = 0;
            snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
            snprintf(g_wheelIni, sizeof(g_wheelIni), "%s", ini); // 21j-r18
            g_wheelEnabled = GetPrivateProfileIntA("ClassWheel", "Enabled", 1, ini) != 0;
            g_wheelSplit = GetPrivateProfileIntA("ClassWheel", "Split", 1, ini) != 0;
            g_pauseLatch = GetPrivateProfileIntA("ClassWheel", "PauseLatch", 1, ini) != 0; // 21h-r6
            g_mouseClicks = GetPrivateProfileIntA("ClassWheel", "MouseClicks", 1, ini) != 0; // 21j-r19
            g_sideCarousel = GetPrivateProfileIntA("ClassWheel", "SideCarousel", 1, ini) != 0; // v7.4
            // 21j-r9: the 1-player ring's place, percent of the game's soldier's x (-80 .. 80)
            {
                int shift = static_cast<int>(GetPrivateProfileIntA("ClassWheel", "RingShift", -31, ini));
                shift = shift < -80 ? -80 : (shift > 80 ? 80 : shift);
                g_ringShift = static_cast<float>(shift) / 100.0f;
            }
            g_quarterLook = GetPrivateProfileIntA("Splitscreen", "MenusLikeP34", 1, ini) != 0; // 20g-r7
            g_hzWanted = GetPrivateProfileIntA("Splitscreen", "MenusHorizontal", 1, ini) != 0;  // 20g-r12
            g_verticalIni = GetPrivateProfileIntA("Splitscreen", "Vertical", 0, ini) != 0;     // 20g-r12
        }
        Log("[wheel] 20g-r12: [Splitscreen] MenusHorizontal=%d (Vertical=%d): %s", g_hzWanted ? 1 : 0, g_verticalIni ? 1 : 0,
            !g_hzWanted ? "the horizontal split keeps the 2-player (vertical) layout"
                        : "a horizontal split's spawn screens laid out as the Classic Collection's (both halves alike) - decided by "
                          "the HUD roots' shape each match (vertical halves keep their own layout)");
        Log("[wheel] 20g-r7: [Splitscreen] MenusLikeP34=%d (4-player: player 1's side soldiers and class ring at players 3 / 4's "
            "sizes)", g_quarterLook ? 1 : 0);
        Log("[wheel] 21j-r9: [ClassWheel] RingShift=%d: the 1-player class ring's centre %.2f of the game's soldier's x from it (-0.13 "
            "before: its right-hand soldiers under the class window), \"SELECT SOLDIER\" %+.3f of the screen across", static_cast<int>(
            g_ringShift * 100.0f + (g_ringShift < 0.0f ? -0.5f : 0.5f)), g_ringShift, SelSoldierX());
        Log("[wheel] v7.4: [ClassWheel] SideCarousel=%d: 1-player CHOOSE YOUR SIDE with more than two sides %s", g_sideCarousel ? 1 : 0,
            g_sideCarousel ? "is a carousel (the sides out of the match left off)" : "in rows (v7.0's; the sides out greyed) - split screen keeps the carousel");
        Log("[wheel] 21h-r6: [ClassWheel] PauseLatch=%d: %s", g_pauseLatch ? 1 : 0,
            g_pauseLatch ? "back from a pause, his spawn screens take only a new press (the A / B that closed the pause is latched)"
                         : "as before (the button that closed the pause counts on his spawn screen)");
        Log("[wheel] 21j-r19: [ClassWheel] MouseClicks=%d: %s", g_mouseClicks ? 1 : 0,
            g_mouseClicks ? "the mouse as the Classic Collection's - side screen: click a side to join it; spawn point screen: click a "
                            "post on the map to pick it, SELECT SPAWNPOINT to go on; soldier select: click the wheel left / right of "
                            "the picked soldier to turn it, SELECT CHARACTER to spawn; BACK (or the right button) goes back"
                          : "as r18 (a click on a soldier picks that class, the front one spawns; a click anywhere goes on from the "
                            "side and spawn point screens)");
        Log("[wheel] [ClassWheel] Split=%d: %s", g_wheelSplit ? 1 : 0,
            g_wheelSplit ? "player 1's console flow in split screen too (his half)" : "the PC screen in split screen");
        // 21j-r18: the flow state starts at the side screen when either the 1-player or the split screen flow is on (the 1-player
        // PC MENU is the game's PC screen whatever the state says)
        InterlockedExchange(&g_previewIdx, (g_wheelEnabled || g_wheelSplit) ? 1 : 0);
        Log("[wheel] [ClassWheel] Enabled=%d (21j-r18: single player only - the main menu's GAME OPTIONS SPAWN SCREEN row): %s",
            g_wheelEnabled ? 1 : 0,
            g_wheelEnabled ? "CLASS WHEEL - the console spawn flow comes up by itself in 1-player games (side screen first); F6 "
                             "cycles PC -> side -> spawn point -> soldier select"
                           : "PC MENU - the game's own PC spawn screen in 1-player games (F6 off); split screen as Split says");
    }

    uint32_t hits[4] = {};
    const int n = FindPattern(kMakeDisplay, hits, 4);
    if (n != 1)
    {
        Log("[wheel] the code that makes the spawn screen: %d pattern hits (need exactly 1) - class wheel scan OFF", n);
        return;
    }
    const uint32_t make = hits[0];
    const uint32_t ctor = CallTarget(make + kMakeCtorCall);
    g_makeRva = make;
    g_spawnCtorRva = ctor;
    if (!MatchPatternAt(ctor, kCtorBody))
    {
        Log("[wheel] SpawnDisplay::SpawnDisplay at rva 0x%08X does NOT match - class wheel scan OFF", ctor);
        return;
    }
    const uintptr_t vtable = ReadU32(Rva(ctor + kCtorVtable));
    const uintptr_t global = ReadU32(Rva(make + kMakeGlobal));
    const uintptr_t lo = ExeBase();
    const uintptr_t hi = ExeBase() + build->sizeOfImage;
    if (vtable < lo || vtable >= hi || global < lo || global >= hi)
    {
        Log("[wheel] the vtable (0x%08X) or the spawn screen pointer (0x%08X) is outside the exe - class wheel scan OFF",
            static_cast<unsigned>(vtable), static_cast<unsigned>(global));
        return;
    }
    g_vtable = vtable;
    g_global = global;
    g_wheelStarted = true; // 21j-r18: the spawn screen found - GAME OPTIONS' SPAWN SCREEN row shows (not when the scan is OFF)
    Log("[wheel] spawn screen maker rva 0x%08X OK, SpawnDisplay::SpawnDisplay rva 0x%08X OK, vtable rva 0x%08X, "
        "spawn screen pointer at rva 0x%08X",
        make, ctor, static_cast<unsigned>(vtable - lo), static_cast<unsigned>(global - lo));
    ConfirmField("SetClassIndex (+23ECh = arg)", kSetClassIndex);
    ConfirmField("GetClassIndex (reads +23ECh)", kGetClassIndex);
    ConfirmField("GetTeamShown (reads +23DCh)", kGetTeamShown);
    FindRegistry(build);
    FindScreenList(build);
    FindDriverFunctions();
    FindFlowFunctions(onGameThread);
    FindGroupCtor(); // 20f-r2
    FindFlashyCtor(); // 20g-r9

    const HANDLE t = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[wheel] watching the spawn screen every %lu ms (the class lists once per match, the IF scan 2 s into the "
            "first spawn screen); PREVIEW: F6 while choosing (1 player) cycles PC -> ifs_readyselect1 -> "
            "ifs_charselect1 -> ifs_mapselect1",
            static_cast<unsigned long>(kPollMs));
    }
    else
    {
        Log("[wheel] thread failed to start (error %lu)", GetLastError());
    }
}

} // namespace bf1
