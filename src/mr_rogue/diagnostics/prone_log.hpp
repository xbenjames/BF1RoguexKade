// BF1GameExt - prone_log.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was prone_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 10a: read-only scan of player 1's buttons for the prone bug (2020 build).
//
// Each button of a soldier's control block (controllable + 0x38 .. + 0x4F, 2
// bytes per button) is kept by the game's ButtonState update (GOG 1.3
// 0x004BFEA0): byte 0 a counter (2 / up to 100 while held, -1 down to -100
// after release, never back to 0), byte 1 an event (1 = tap: released after a
// short press, 2 = DOUBLE-TAP: pressed while the event still says tap, 3 = long
// hold). The tap is never expired, so any later press of the same button can
// read as a double-tap - the suspected prone bug. This logs every press,
// release, double-tap and game-made event change of player 1's buttons, with
// how long since the last release. [prone] lines.
#pragma once

namespace bf1 {

// Called every frame from the game thread (player 2's frame hook).
void ProneScanFrame();

} // namespace bf1
