// BF1GameExt - spawn_manager.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was spawn_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 18a: SpawnScanManager (the SpawnManager itself, for the class wheel scan) and
// SpawnScanFileBuild (this file's build, logged by class_wheel.cpp).
// Phase 7a: read-only scan of BF1's characters (2020 build: GOG 1.3 / Steam).
//
// The SpawnManager creates one Character per local player when a match starts
// (CreateLocalCharacter(team, i) for i < GetNumLocalPlayers()), so with split
// screen on, player 2 already has a character. Characters live in an array on
// the SpawnManager (+0x3C, 0xE8 bytes each, count at +0x38); a character holds
// its name at +0x1C, its index at +0x9C and its team at +0xA0.
//
// This logs the characters when a match starts, dumps characters 0 and 1
// (player 1 and player 2) once, then logs every 4-byte field of theirs that
// changes - so spawning player 1 shows exactly which fields BF1 uses to request
// and track a spawn. Fields that change constantly get muted. [chars] lines.
#pragma once

#include <cstdint>

namespace bf1 {

// Finds the SpawnManager by pattern, then starts the watcher. Call after the
// code is unlocked. Does nothing on GOG 1.2.
void SpawnScanStart();

// Address of character k (0 = player 1, 1 = player 2) while a match is running,
// or 0. Field offsets: +0x9C index, +0xA0 team, +0xAC class, +0xB0 spawn point,
// +0xB4 unit (non-zero = in the world), +0xC8 command post index, +0xCC ready.
uintptr_t SpawnScanCharacter(int k);

// 17d: the whole character list (start, count; 0xE8 bytes each) - false if there is none
bool SpawnScanCharacters(uintptr_t* array, uint32_t* count);

// 18a: the SpawnManager while a match is running, or 0. Team k is at +0x18 + k*4
// (k = the team number AddUnitClass uses), the AddUnitClass/SetUnitCount numbers at +0x44 + k*4.
uintptr_t SpawnScanManager();

// 18a: this file's build marker ("p18a-r1"), so the log shows which spawn_manager.cpp was compiled.
const char* SpawnScanFileBuild();

} // namespace bf1
