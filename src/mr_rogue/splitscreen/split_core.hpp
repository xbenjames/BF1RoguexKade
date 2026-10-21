// BF1GameExt - split_core.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/ (was split_test.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 2 split screen test.
//
//   F9  or  LB+RB+Back  ->  split ON:  2 local players, and it stays on
//                           (menu scripts can't switch it back off)
//   F10 or  LB+RB+Start ->  split OFF: 1 local player
//   F11                 ->  player 2 input redirect on/off (phase 6c)
//   Insert / Home / LB+RB+Y -> capture one whole rendered frame (phase 11a)
//   Num 1-9 / Num 0         -> draw-group switches: one off/on each, Num 0 all on (11e)
//   LB+RB+X                 -> draw-group switches: cycle one off at a time (11e)
//   Num *                   -> every draw that samples a screen copy off/on (11f)
//   Num /                   -> keep the game's render-state copy in sync (12a shadows)
//   Num -                   -> blur and motion blur off/on (12a motion)
//
// Keys only react while the game window is in front. Use them in the menus,
// before starting a match.
//
// BF1GameExt.ini [Splitscreen] Enabled=1 turns the sticky split mode on from
// the start, so F9 is not needed.
//
// The game's split screen Lua functions are redirected through logging
// wrappers, so every call the menu scripts make shows up as a [lua] line.
#pragma once

namespace bf1 {

// Called from DllMain, before the game registers its Lua functions.
// Checks each registration slot, then points it at a logging wrapper.
void SplitTestInstallLuaHooks();

// Starts the hotkey thread. Call only after ScanSplitSitesOk() is true.
void SplitTestStart();

} // namespace bf1
