// BF1GameExt - effects_fix.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 11b: the split screen glow / effects layer draws in the right half
// (2020 build: GOG 1.3 / Steam).
//
// Two effect passes (GOG 1.3 0x006EC770 and 0x006E45A0) save the player's
// viewport, switch to the full-screen effects target, draw into it, and only
// restore the viewport at the end. The switch resets the viewport to the whole
// screen, so in split screen their draws come out twice as wide - the red
// wedges / smears on player 1's side (player 2's half is drawn last and
// repainted). This redirects exactly those two "switch target" calls to a DLL
// function that keeps the player's viewport across the switch, using the
// game's own GetViewport / SetRenderTarget / SetViewport. In single player the
// viewport is the whole screen either way. [effectsfix] lines.
//
// BF1GameExt.ini [Display] SplitEffectsFix=1 turns it on (default 1).
#pragma once

namespace bf1 {

// Call after the code is unlocked, on the game thread.
void EffectsFixStart(bool onGameThread);

} // namespace bf1
