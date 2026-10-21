// BF1GameExt - autospawn.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 7b: spawns player 2 automatically (2020 build: GOG 1.3 / Steam).
//
// BF1 creates player 2's character when a match starts, with a team, a default
// class and a spawn point. What the PC spawn screen does when you press Spawn
// is SetReady(1) on its player's character; the next Character::Update then
// spawns the soldier. This presses that button for player 2, through the
// game's own SetReady, every 3 seconds while player 1 is in the world and
// player 2 is not (so player 2 also respawns after dying). [spawn] lines.
//
// BF1GameExt.ini [Splitscreen] AutoSpawnP2=1 turns it on (default 1).
#pragma once

namespace bf1 {

// Finds SetReady through the spawn screen's "Spawn pressed" code and reads the
// INI. Call after the code is unlocked.
void AutoSpawnStart();

// Called every frame from the game thread (player 2's frame hook).
void AutoSpawnFrame();

} // namespace bf1
