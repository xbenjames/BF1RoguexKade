// BF1GameExt - prone_fix.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 10b: double-tap only counts within a time window (2020 build).
//
// The game's ButtonState update (GOG 1.3 0x004BFEA0) marks a button DOUBLE-TAP
// whenever it is pressed while its last event is still TAP - with no time
// limit: the TAP is never expired. So crouch, get up (e.g. jump), crouch again
// any number of seconds later = double-tap = prone. The 10a scan showed it:
// crouch (bit3, +0x40) pressed 100+ frames after its last release -> DOUBLE-TAP.
// This makes a press count as a double-tap only if the button was released at
// most N frames earlier (N from DoubleTapMs at 60 fps); a press after a stale
// TAP is a fresh press. Every button of every soldier goes through this one
// function, so it fixes player 1 and player 2 alike. [pronefix] lines.
//
// BF1GameExt.ini [Controls] DoubleTapMs=600 (0 = the game's original behaviour).
#pragma once

namespace bf1 {

// Call after the code is unlocked, on the game thread.
void ProneFixStart(bool onGameThread);

} // namespace bf1
