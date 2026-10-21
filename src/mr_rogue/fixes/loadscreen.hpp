// BF1GameExt - loadscreen.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 9b: loading screens fill a widescreen display (2020 build).
//
// The loading screen is native (LoadDisplay). On a screen wider than 4:3 it
// sets "widescreen" flags, and its draw helper then fits every layer into a
// centred 4:3 box (black bars left and right, the planet name out in the left
// bar). This rewrites that one 75-byte branch of the draw helper to FILL
// instead: the full screen width, the height width * 3/4 centred, so the top
// and bottom run off-screen (12.5% each at 16:9). Every layer scales together
// and keeps its exact proportions. [loadscreen] lines.
//
// BF1GameExt.ini [Display] LoadScreenFill=1 turns it on (default 1).
#pragma once

namespace bf1 {

// Call after the code is unlocked, on the game thread.
void LoadScreenFillStart(bool onGameThread);

} // namespace bf1
