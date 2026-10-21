// BF1GameExt - input_log.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was input_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 5 scan: how the 2020 build (GOG 1.3 / Steam) hands pads to players.
//
// The 2020 exe has exactly one XInput pad object (GetNumPads() returns 1, the
// pad array is built with one slot), so player 2 never gets a pad. This scan
// finds the pad array, the per-player active-pad table and the player-input
// object in memory, then logs what they hold - and what every physical XInput
// pad is pressing - while you play. Read-only.
#pragma once

namespace bf1 {

// Finds the structures by pattern (logs [input] lines), then starts the
// watcher. Call after the code is unlocked. Does nothing on GOG 1.2, whose
// input works differently (DirectInput only).
void InputScanStart();

} // namespace bf1
