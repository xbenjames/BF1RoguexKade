// BF1GameExt - class_wheel.hpp   (file build p23a-r1)
// v7.1: ClassWheelSideTeams / ClassWheelSideOut / ClassWheelSideRingFrame / ClassWheelSideRingClear (the side carousel).
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was classwheel_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r18: ClassWheelMenuState / ClassWheelMenuSet (the main menu's GAME OPTIONS SPAWN SCREEN row); ClassWheelSplitWanted is
// [ClassWheel] Split alone ([ClassWheel] Enabled is single player only).
// 20g-r12: ClassWheelHorizontal (horizontal 2-player, the Classic Collection's layout); the rings' arrangement is a style
// (kRingStyle*: 0 = 19g's, 1 = the Xbox's, 2 = Classic's horizontal) - true / false still mean the Xbox's / 19g's.
// 20g-r11: ClassWheelP1LookAllow / ClassWheelP1Look (player 1's three spawn screens in players 3 / 4's look) replace
// r10's ClassWheelSideLookAllow / ClassWheelSideLookUp.
// 20g-r10: ClassWheelSideLookAllow / ClassWheelSideLookUp (player 1's side screen in players 3 / 4's look).
// 20g-r9: ClassWheelFlashyLike / ClassWheelIsFlashy (copies of the game's menu bars), ClassWheelFindKind /
// ClassWheelDumpTree (element trees searched / logged); ClassWheelPlaySound says whether it played.
// 20g-r7: ClassWheelTextNewEx / ClassWheelTextColor.
// 20f-r4: ClassWheelGroupNew makes a RedScreenGroupElement (a HUD root's own group class, 0xB0 bytes).
// 20f-r2: ClassWheelGroupNew (an empty RedGroupElement for players 3 and 4's spawn screens), ClassWheelRingFrame /
// ClassWheelRingClear (players 3 and 4's class rings, rings 1 and 2; player 2's is ring 0).
// 19h-r2: ClassWheelXbPanels / ClassWheelXbWindow take the panels' Skin groups (the boxes, stretched).
// 19h: ClassWheelXbPanels / ClassWheelXbWindow (flat panels and class window placed from their own
// sizes), ClassWheelFlatPlace, ClassWheelVehiclesAt; ClassWheelRing2Frame's (cx, cy) is now the
// ring's centre (the Xbox's arrangement).
// 19g: ClassWheelCubeNew (the spawn screen's blue grid for player 2), ClassWheelSetLayer,
// ClassWheelRing2Frame / ClassWheelRing2Clear (player 2's class ring); ClassWheelSplitMapBig now sets
// the split screen's own map layouts (5 = player 1's, 4 = player 2's).
// 18z: ClassWheelMoveElement, ClassWheelSplitMapBig (the Xbox layout for player 2's screens too).
// 18x: ClassWheelModelNew / ClassWheelModelGrey (a class's soldier for player 2's screens).
// 18w: ClassWheelSplitWanted() - the console flow in split screen ([ClassWheel] Split).
// 18u: the texts and sounds are lent to player 2's spawn screens (split_spawn_menus.cpp): the ClassWheel...
// functions at the end, and the count of spawn screens deleted (one per match end). Player 1's console
// flow is unchanged.
// Phase 18a: CLASS WHEEL SCAN (2020 build: GOG 1.3 / Steam), log-only - no hooks, nothing in the
// game is written. Maps what the console spawn flow (the Classic Collection's three screens: side,
// soldier wheel, spawn point) needs from the PC exe: the PC spawn screen object (SpawnDisplay) and
// its fields, each team's class list, and whether the console screens' layouts (Lua) are loaded on
// PC at all. [wheel] lines.
#pragma once

#include <cstdint>

namespace bf1 {

// Finds the spawn screen object by pattern and starts the watcher thread. Call after
// SpawnScanStart (it needs the SpawnManager). Does nothing on GOG 1.2. 18f: on the game thread it
// also routes the spawn screen's PC input call through BF1GameExt (skipped while a console screen
// is up).
void ClassWheelScanStart(bool onGameThread);

// 18d: the console screen preview. F6 while you choose on the spawn screen (1 player) cycles the
// screen the game's spawn-screen chooser shows: the PC screen -> ifs_readyselect1 ->
// ifs_charselect1 -> ifs_mapselect1 -> the PC screen. Returns the screen to show, or null for the
// game's own PC screen. Called by spawn_ui.cpp's chooser hook (game thread).
const char* ClassWheelPreviewScreen();

// 18e/18f: the console driver, run on the game thread every frame the spawn screen is up
// (spawn_ui.cpp's chooser hook). screen = the screen the chooser shows (null = the PC
// screen). 18f: the console flow (1 player, entered with F6): side -> soldier -> spawn point,
// pad / keyboard: left / right pick the class, A (Enter) goes on, B (Backspace) goes back, A on the
// spawn point screen spawns through the game's own Spawn button code. 18g: on the spawn point
// screen left / up and right / down pick the previous / next command post through the game's own
// spawn point functions. 18h: the soldier select shows the team's other classes around the
// selected one as a ring (the wheel's first look: [ ] = ring width, - = = back size while on it).
// 18i: the ring turns (eases round to the selected class), droidekas are on it, and , . move its
// centre left / right. 18j: the console order is side -> spawn point -> class (A on the soldier
// select spawns), and the ring's soldiers play their idle animations. 18k: the ring as the Classic
// Collection has it (; ' = front size, PageUp / PageDown = height), no soldier on the spawn point
// screen, and a log-only scan for the side screen. 18l: the side screen ("CHOOSE YOUR SIDE", the two
// sides' soldiers and names; left / right pick, A joins with the game's own SelectTeam). 18l-r2: only
// the selected class animates. 18l-r3: the side screen's soldiers do not animate at all. 18m: side
// screen texts show and its soldiers no longer turn, bigger droidekas, the map moved left on the
// spawn point screen and off the screen on the side screen and soldier select. 18n: front soldier
// drawn last, unpicked side dark, droidekas roll back up, map in either big layout, text diagnostic.
// 18o: the texts show (string set last); side screen title and names, spawn point panel texts
// (post name, vehicles, objective), HINTS hidden, "SELECT ..." prompts. 18p: [ClassWheel] Enabled
// in the INI, mouse buttons = A / B, ring depth (no more weapons through the front soldier), the
// game's interface sounds, picking a class plays its animation at once, layout as the reference.
void ClassWheelFrame(uintptr_t spawnDisplay, const char* screen);

// 20g-r12: horizontal 2-player - [Splitscreen] MenusHorizontal=1 with Vertical=0, 2 local players (not 3 / 4) and both
// HUD roots horizontal halves: both players' side, spawn point and class screens laid out as the Classic Collection's
// horizontal split (split_spawn_menus.cpp gives both halves its safe area). Game thread.
bool ClassWheelHorizontal();
// 20g-r12: the class rings' arrangements (ClassWheelRing2Frame / ClassWheelRingFrame's style)
constexpr int kRingStyle19g = 0;     // (cx, cy) the front soldier's feet
constexpr int kRingStyleXbox = 1;    // the picked soldier 32 degrees round to the right, the others smaller behind
constexpr int kRingStyleClassic = 2; // the Classic Collection's horizontal: the picked one in front in the middle
// 20g-r12: the Classic Collection's horizontal split, measured in its video (fractions of a half: across, down; both
// halves alike). Both players' screens use these (class_wheel.cpp: player 1's, split_spawn_menus.cpp: player 2's).
constexpr float kHzTitleX = 0.50f, kHzTitleY = 0.155f;          // CHOOSE YOUR SIDE (centred)
constexpr float kHzSideX0 = 0.424f, kHzSideX1 = 0.574f;          // the two sides' soldiers (their feet)
constexpr float kHzSideFeetY = 0.707f;
constexpr float kHzSideTall = 0.43f;                             // their height, of the half's
constexpr float kHzNameX0 = 0.4275f, kHzNameX1 = 0.5725f, kHzNameY = 0.764f; // the sides' names (centred)
constexpr float kHzTextScale = 0.82f;                            // the side screen's texts (ours were 1.22 times Classic's)
constexpr float kHzPromptPostX = 0.845f, kHzPromptClassX = 0.854f, kHzPromptY = 0.858f; // SELECT SPAWNPOINT / CHARACTER
constexpr float kHzPromptScale = 0.81f;
constexpr float kHzRingX = 0.384f, kHzRingFeetY = 0.826f;        // the ring's centre / the picked soldier's feet
constexpr float kHzRingRadius = 0.0737f;                         // of the half's width
constexpr float kHzRingLift = 0.107f;                            // the back ones' feet that much higher (of the half's height)
constexpr float kHzRingTall = 0.515f;                            // the picked soldier's height, of the half's
constexpr float kHzSoldierUnits = 376.0f; // a soldier of size 1 (the game's own) stands this many root units tall
constexpr float kHzMapW = 128.0f, kHzMapH = 140.0f;              // the spawn point screen's map (its layout's size)
constexpr float kHzMapX = 0.320f, kHzMapY = 0.432f;              // its centre

// 18u: lent to player 2's spawn screens (split_spawn_menus.cpp). Game thread only. All return false / 0
// (and do nothing) if the functions they need were not found at start.
// The game's text functions were found (RedTextElement, SetFont, SetJustify, SetText, the element calls).
bool ClassWheelTextsReady();
// SetText(wide) on any of the game's text elements (a console screen's title bar or BoxText too).
bool ClassWheelSetText(uintptr_t text, const wchar_t* s);
// A new RedTextElement in group at (x, y) of the group. TextLike: made like `like` (its font, scale,
// justify, colour, layer). TextNew: white "gamefont_large" at `scale`, centred on (x, y).
uintptr_t ClassWheelTextLike(uintptr_t group, uintptr_t like, const wchar_t* s, float x, float y);
uintptr_t ClassWheelTextNew(uintptr_t group, const wchar_t* s, float x, float y, float scale);
void ClassWheelTextGrey(uintptr_t text, bool grey);
// 20g-r7: a text with its own horizontal justify (0 left, 1 right, 2 centre) and colour (0xAARRGGBB); and a colour change.
uintptr_t ClassWheelTextNewEx(uintptr_t group, const wchar_t* s, float x, float y, float scale, int hJustify, uint32_t argb);
void ClassWheelTextColor(uintptr_t text, uint32_t argb);
// Deletes an element made with the two above (its own deleting destructor; it leaves its group).
void ClassWheelDeleteElement(uintptr_t e);
// The element's SetVisible.
void ClassWheelShowElement(uintptr_t e, bool on);
// The interface sounds player 1's flow plays: 0 = tick (left / right), 1 = select (A), 2 = back (B).
bool ClassWheelPlaySound(int which); // 20g-r9: true if it played

// 20g-r10 / r11: 4-player - player 1's side, spawn point and class screens drawn by split_spawn_menus.cpp in players 3 / 4's
// look. Allow: they can be drawn (player 1's own soldiers, texts, panels, window and ring are then not shown); off again
// if its group cannot be made. ClassWheelP1Look: true while one of them is up in that look - screen 1 side, 3 spawn
// point, 2 class (the console flow's numbers), his side shown, his post, his class index (of the side's classes), the
// side picked on the side screen (0 / 1). Game thread.
void ClassWheelP1LookAllow(bool on);
bool ClassWheelP1Look(int* screen, int* team, uintptr_t* post, int* cls, int* pick);
// How many spawn screens the game has deleted (one at each match end), through the spawn screen's
// deleting destructor hook; *known = false if that hook is not installed (the count stays 0).
long ClassWheelSpawnScreenDeletes(bool* known);
// 18w: [ClassWheel] Enabled=1 and Split=1: player 1's console flow runs in split screen too (the chooser
// shows the screen ClassWheelPreviewScreen names there as well).
bool ClassWheelSplitWanted();

// 21j-r18: the main menu's GAME OPTIONS SPAWN SCREEN row - 1-player games' spawn screen: 1 = CLASS WHEEL (the console flow),
// 0 = PC MENU (the game's own), -1 = the class wheel scan is off (not on this build, or it did not find the spawn screen - no
// row then). Set: from the next spawn screen on, saved in
// BF1GameExt.ini [ClassWheel] Enabled; the new state.
int ClassWheelMenuState();
int ClassWheelMenuSet(bool on);
// 18x: a class's 3D soldier (or droideka) made like the ring's, as a child of `group` at (x, y) of it
// (root units), the game's own soldier's view and pose, size = the game's soldier's x sizeMul. 0 if
// the ring's functions were not found, the class is not a soldier class, or the spawn screen is gone.
// Delete it with ClassWheelDeleteElement. Grey = the unpicked side's colour.
uintptr_t ClassWheelModelNew(uintptr_t group, uintptr_t cls, float x, float y, float sizeMul, bool* droideka);
void ClassWheelModelGrey(uintptr_t e, bool grey);
// 18z: an interface element moved to (x, y) of its group (SetPosition; logged with where it was).
bool ClassWheelMoveElement(uintptr_t e, float x, float y, const char* what);
// 18z: a player's spawn point screen is up in split screen. 19g: that player's own split map layout
// (5 = player 1's, 4 = player 2's in mode 1) takes the Xbox's place above the panels; as the game had
// it when off. Maps in that layout re-apply it (their own SetMode).
void ClassWheelSplitMapBig(int player, bool on);
// 19g: that player's split layout as the game had it, the table only (no map touched: a match ending).
void ClassWheelSplitMapReset(int player);
// 19g: a copy of the spawn screen's blue grid ("shell_cube", player 1's spawn screen +2210h) in `group`
// at (x, y) of it (put it at the root's centre), drawn first in its group (layer 255). 0 if its
// functions or player 1's grid are not there. Delete it with ClassWheelDeleteElement.
uintptr_t ClassWheelCubeNew(uintptr_t group, float x, float y);
// 19g: an element's layer (its group draws from the highest value down; re-sorted in its group).
void ClassWheelSetLayer(uintptr_t e, int layer);
// 19g: player 2's class ring in `group`: one soldier per class of `classes` (n of them), turning to
// `sel`; (cx, cy) = the ring's centre at the soldiers' feet (19h; the group's units), width = its
// radius, size = the picked soldier's size (of the game's soldier), lift = the back of the ring stands
// that much higher, (fixX, fixY) = the root's x 512, y 520 (where the game's own soldier stands) in the
// group's units (droidekas are placed around it). 19h: xbox = the Xbox's arrangement (the picked
// soldier 32 degrees round to the right, the others smaller behind); false = 19g's ring, (cx, cy) the
// front soldier's feet. Every frame his class screen is up (game thread). Returns the soldiers placed.
// 20g-r12: style kRingStyleClassic - the Classic Collection's horizontal ring: the picked soldier in front in the middle,
// the others round it (smaller, behind), the back ones standing `lift` higher (n^3 of the nearness).
int ClassWheelRing2Frame(uintptr_t group, const uintptr_t* classes, int n, int sel, float cx, float cy, float width, float size,
                         float lift, float fixX, float fixY, int style);
// 19g: his ring's soldiers deleted (del: their group is alive) or forgotten (a match being torn down).
void ClassWheelRing2Clear(bool del, const char* why);
// 19h: the Xbox's flat spawn point screen panels for `player` (0 / 1): sideBar = the screen's SideBar
// group, panel[3] = COMMAND POST, VEHICLES, OBJECTIVES, title[3] = their titleBarElements (the orange
// bars: each panel is placed by its bar's top left), skin[3] = their Skin groups (the boxes; the
// OBJECTIVES one is stretched to the Xbox's bottom). The group is set straight and the panels scaled.
// False (nothing changed) when a title is missing, or a box cannot be read yet (lastTry: a 507 x 30 box
// under the title is assumed then).
bool ClassWheelXbPanels(int player, uintptr_t sideBar, const uintptr_t* panel, const uintptr_t* title, const uintptr_t* skin,
                        const char* who, bool lastTry);
// 19h: the frames a flat layout is tried on (1, 2, 3, 5, 10, 20, then every one from 30: the last).
bool ClassWheelLayoutTryNow(int tries);
// 19h: the same for the class screen's window (info = its Info group, skin = the window's Skin group).
bool ClassWheelXbWindow(int player, uintptr_t info, uintptr_t window, uintptr_t title, uintptr_t skin, const char* who, bool lastTry);
// 19h: (x, y) in a straightened group for a place in fractions of the player's half (false: the group
// is not straight).
bool ClassWheelFlatPlace(int player, uintptr_t group, float fx, float fy, float* x, float* y);
// 19h: the vehicles at `post` as the game writes them for its PC popup (its own PopupFill, the string
// kept instead of set). Game thread only. False if PopupFill was not found or hooked.
bool ClassWheelVehiclesAt(uintptr_t post, wchar_t* out, size_t cap);
// 20f-r2: players 3 and 4's class rings - ring 1 = player 3, 2 = player 4 - the same as ClassWheelRing2Frame / Clear
// (player 2's, ring 0). Game thread only.
int ClassWheelRingFrame(int ring, uintptr_t group, const uintptr_t* classes, int n, int sel, float cx, float cy, float width,
                        float size, float lift, float fixX, float fixY, int style);
void ClassWheelRingClear(int ring, bool del, const char* why);
// v7.1: the sides of this match for the side screens (Mashup Conquest: every playable team 1 - 7; else teams 1 and 2) -
// how many put in `teams` (at most `max`) - and whether a side is out of the match (greyed out, not pickable).
int ClassWheelSideTeams(int* teams, int max);
bool ClassWheelSideOut(int team);
// v7.1: player p's (1 - 4) side carousel (CHOOSE YOUR SIDE with more than two sides): one soldier per side (`classes` = each
// side's first class) on a ring in `group`, turned to `sel`, the others darker (`out` sides darker still); the arguments
// as ClassWheelRing2Frame's. Every frame his side screen is up (game thread). Returns the soldiers placed.
int ClassWheelSideRingFrame(int player, uintptr_t group, const uintptr_t* classes, const bool* out, int n, int sel, float cx,
                            float cy, float width, float size, float lift, int style);
void ClassWheelSideRingClear(int player, bool del, const char* why);
// v7.3: slots 5 - 8 of ClassWheelSideRingFrame / Clear = players 1 - 4's side carousels in players 3 / 4's look (4-player).
// Player 1's side screen's sides (teams, how many; total = the match's sides, more than two: the ones out are left off).
int ClassWheelP1Sides(int* teams, int max, int* total);
// 20f-r2 / r4: a new, empty RedScreenGroupElement (the game's new and its own constructor; not in any group or root
// yet - its root anchor / flag are the fields AddToRoot sets), or 0 if
// its constructor was not found at start. Delete it with ClassWheelDeleteElement (its children are only taken out of
// it, not deleted).
uintptr_t ClassWheelGroupNew();

// 20g-r9: elements of class `type` (RTTI name) under e, e included, depth first, up to max, with each one's origin in
// e's parent's units - (ax, ay) is e's parent's origin there (a root child: its anchor in the root); returns how many.
int ClassWheelFindKind(uintptr_t e, const char* type, float ax, float ay, uintptr_t* out, float* xs, float* ys, int max);
// 20g-r9: e's tree in the log ([wheel] tree lines, 400 in all): class, place, rows, shown, colour, text / bar fields.
void ClassWheelDumpTree(uintptr_t e, const char* what, float ax, float ay);
// 20g-r9: e is a FlashyTextElement (the console menus' text with its bar)
bool ClassWheelIsFlashy(uintptr_t e);
// 20g-r9: a FlashyTextElement made like `like` (one of the game's: its font, justify, scale, colour, bar textures,
// margins and flags) with string s at (x, y) of group; 0 if `like` is not one or it could not be made. Delete it with
// ClassWheelDeleteElement.
uintptr_t ClassWheelFlashyLike(uintptr_t group, uintptr_t like, const wchar_t* s, float x, float y);
// 21j-r9: e put in group (the game's AddChild; false if it is in a group already, or not found)
bool ClassWheelAddChild(uintptr_t group, uintptr_t e);
// 21j-r9: e at (x, y) of its group, its rows (sx, sy, 1): what it holds drawn sx / sy as big
bool ClassWheelPlaceScaled(uintptr_t e, float x, float y, float sx, float sy);
// 21j-r9: a FlashyTextElement's bar width (game units; false if e is not one)
bool ClassWheelFlashyBarWidth(uintptr_t e, int w);
// 21j-r9: a text in a console window's box, as the game's BoxText: left / top, its font and scale, white, wrapped in boxW x boxH
// units, at `layer` (0 if the texts were not found or no memory). Delete it with ClassWheelDeleteElement.
uintptr_t ClassWheelBoxTextNew(uintptr_t group, const char* font, const wchar_t* s, float x, float y, float scale, int boxW, int boxH,
                               int layer);

} // namespace bf1
