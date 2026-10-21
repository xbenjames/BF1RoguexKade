// BF1GameExt - players34.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was playern_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 21j-r33: PlayerNSignName / PlayerNSignNamesClear - the names players 3 / 4 signed in with (their characters get them).
// Phase 21h-r3: PlayerNMenuInput - players 3 / 4's real pause menu up / gone (their soldiers get a calm packet; held until
// their buttons are up after it goes).
// Phase 21g-r2: PlayerNSignIn - the player count of a match follows the sign-in screen (2 / 3 / 4).
// Phase 20g-r8: PlayerNControlsGet / Set / Save / Live - players 3 and 4's controller options (their pause screen's
// OPTIONS page in split_spawn_menus.cpp): invert look, invert flying, look sensitivity.
// Phase 20g-r6: PlayerNHudMap / PlayerNHudMapShow (players 3 and 4's own minimaps, for their spawn point screens in
// split_spawn_menus.cpp) and PlayerNSetPaused (their pause screens: their soldiers stand still while one is up).
// Phase 20g-r5: PlayerNHudStatusPlayer (players 3 and 4's own StatusDisplays, for their stamina bar in stamina_hud.cpp).
// Phase 20f-r2: PlayerNSpecView (the renderer view of player 3's / 4's quarter, for their spawn screens in
// split_spawn_menus.cpp); the frame hook runs their screens (P2SpawnP34Frame) and turns that view's HUD flag on while one is
// up; the auto-spawn stands aside while they look after him.
// Phase 20e: pads 3 and 4 drive players 3 and 4 ([Splitscreen] PadsP34=1; [pad34] lines); r2: views 3 and 4
// through the game's own chase camera ([chase34] lines), sprint / third person for them (sprint.cpp).
// Phase 20d (r2: the camera manager guarded for players 3 and 4 - their spawn crashed it; r3: their HUD displays
// lent from players 1 / 2 - a hit on them crashed; r4: their weapon fire kept off the
// first-person models they don't have - shooting crashed; r5: the input manager's rumble
// kept off them, [p34] trace lines; r6: their damage tint wrote past the screen filter - the real shooting crash): players 3 and 4 in the world - first-person / music / pause calls guarded for them, views 3 and 4
// follow their soldiers, [Splitscreen] AutoSpawnP34=1 spawns them (test; no pads yet). [players] lines.
// Phase 20c: [Splitscreen] Players=3 / 4 (with Views=4) - players 3 and 4 made to exist (the game told 3 / 4 local
// players, except the call sites that walk 2-slot tables; the network's local slots 4 of ours). [players] lines.
// Phase 20b: [Splitscreen] Views=4 - the two players' views as the top quarters, views 3 and 4 (spectators until
// players 3 and 4 exist) as the bottom quarters; Views=2 = nothing of 20b installed. [views4] lines.
// Phase 20a: 4-player step 1, the map (2020 build: GOG 1.3 / Steam). Log only - nothing changes in the game.
// [playerN] lines. The static map (every 2-player limit found in GOG 1.3) is at the top of players34.cpp.
//
// (1) At start: every site of the map found by pattern and checked (each one exactly once, its operands
//     where they should be), and the address of every per-player table read from the code.
// (2) A census of the calls into the game's per-player entry points, by caller (return address) and by
//     the player asked for (or answered): GetNumLocalPlayers, GetIFManager, the HUD display getters, the
//     pause calls, the control state getter, the network's local slots, the local slot searches. Each one
//     through its jump thunk (the thunk's target moved to a counting wrapper that calls the game's function).
//     A caller that reaches player 2 already loops over local players; one that only ever asks for player 1
//     is a system to fix for players 3 and 4.
// (3) Every per-player table, live, 45 s and 3 min into a split screen match, with what lies right after
//     each 2-slot table (what a player 3 / 4 index would read today); the census 45 s in and at match end.
// [Splitscreen] PlayerNScan=1 (0 = none of this is installed).
#pragma once

namespace bf1 {

// Call last in the scan sequence (after every other part has put its hooks in: a thunk already moved by
// another part is chained, not replaced). Does nothing on GOG 1.2, or with PlayerNScan=0, Views=2 and Players=2.
void PlayerNScanStart(bool onGameThread);

// 20e: called by player2_input.cpp each frame right after player 2's input update (game thread, inside the input
// manager's update): pads 3 and 4 and players 3 and 4's input objects. Does nothing unless Players=3 / 4 and
// PadsP34=1 are on.
void PlayerNInputFrame(float dt);

// 20e: player 3 / 4's input object (which 0), buffer A (1) or buffer B (2) for the input manager's getters (index 2
// / 3), or nullptr while 20e is off (the game's own getter then).
void* PlayerNInputObject(int index, int which);

// 20e-r2 (sprint.cpp): how many players the pads drive (4 / 3 while pads 3 and 4 are on, else 2), and pad 3 / 4's
// address (p = 2 / 3; 0 if off or not connected) - the game's pad class, the same layout as pad 2.
int PlayerNPadPlayers();
uintptr_t PlayerNPadAddress(int p);

// 20f-r2 (split_spawn_menus.cpp): the renderer view (= HUD root index) of player k + 3's quarter (k = 0 / 1), or -1 while
// there is none (no 4 views, or the slot is not our camera's).
int PlayerNSpecView(int k);

// 20g-r5 (stamina_hud.cpp): 2 / 3 when `display` is player 3's / 4's own StatusDisplay (set up this match), else -1.
int PlayerNHudStatusPlayer(uintptr_t display);

// 20g-r6 (ragdoll.cpp): how many local players there are with players 3 / 4 installed (Players=3 / 4: 3 / 4), else 0.
// The game's own count stays 2 in split screen (only its getter answers 3 / 4).
int PlayerNLocalPlayers();

// 21g-r2 (menus.cpp, the sign-in screen's LAUNCH): the player count of the next matches (2 - Players; 2 = the game's
// own 2-player split, 3 = the 3-player layout, 4 = quarters) and the XInput pads that signed in as players 3 / 4 (-1: the
// first free pad). Answers the count set, 0 if players 3 / 4 are not on. Game thread.
int PlayerNSignIn(int count, int padP3, int padP4);
// 21j-r33 (menus.cpp, the sign-in's LAUNCH: "signname N NAME" / "signname clear"): the name player 3 / 4 signed in with
// (Latin-1, at most 63 letters; 1 = kept, 0 = not player 3 / 4 or no name) - from then on his character gets it in each
// match he plays; Clear forgets both (the next LAUNCH sends them again). Game thread.
int PlayerNSignName(int player, const char* name);
void PlayerNSignNamesClear();
// 21g-r2: the most players a match can have (Players, installed: 3 / 4), 0 while players 3 / 4 are not on. (The sign-in
// screen is built for this many; PlayerNLocalPlayers is this match's count.)
int PlayerNPlayersMax();

// 20g-r6 (split_spawn_menus.cpp): player k + 3's own MapDisplay (k = 0 / 1; set up this match, else 0), and showing / hiding
// it alone through the class's own ForPlayer (false if there is none). Game thread.
uintptr_t PlayerNHudMap(int k);
bool PlayerNHudMapShow(int k, bool on);

// 20g-r6 (split_spawn_menus.cpp): player k + 3's pause screen is up / down - while up his PlayerInput updates without its pad
// (his soldier stands still); the pad itself still updates (the pause screen reads it).
void PlayerNSetPaused(int k, bool on);

// 21h-r3 (split_spawn_menus.cpp): player k + 3's real pause menu is up (true: his pad drives the menu, his soldier's control state
// gets a calm packet) / gone (false: the calm packet held until every button of his pad is up, 3 s at most)
void PlayerNMenuInput(int k, bool up);

// 20g-r8 (split_spawn_menus.cpp): player k + 3's controller options (k = 0 / 1). invert: the look pitch inverted on foot and
// in ground vehicles; invertFly: flying (on = the game's default); sens: look sensitivity 1-10 (5 = the game's default,
// the game's own scale 2 ^ ((sens - 5) / 5)). Set takes effect from his next input frame; Save writes them to
// BF1GameExt.ini [Splitscreen] InvertLookP3 / P4, InvertFlyingP3 / P4, SensitivityP3 / P4 (false if it could not).
// Live: they are being written into his PlayerInput (pads 3 / 4 on and the game's reads checked out). Game thread.
struct PlayerNControls
{
    int invert = 0;
    int invertFly = 1;
    int sens = 5;
};
PlayerNControls PlayerNControlsGet(int k);
void PlayerNControlsSet(int k, const PlayerNControls& c);
bool PlayerNControlsSave(int k);
bool PlayerNControlsLive();

// This file's build marker.
const char* PlayerNScanFileBuild();

} // namespace bf1
