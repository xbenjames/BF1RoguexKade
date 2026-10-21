// BF1GameExt - split_shadows.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 12d: High (stencil) shadows act as Low while split screen is on
// (2020 build: GOG 1.3 / Steam).
//
// With Shadows on High, split screen shows flat coloured wedges on player 1's
// side; on Low or Off they are gone. Every part of the game asks one gate
// (GOG 1.3 0x006AA4E0, 17 callers) whether stencil shadows are active: the
// WorldShadowMapEffect switch is on AND the device can do them. The Classic
// Collection (Battlefront1.dll, 2024) runs the same gate and the same shadow
// code - only its rendering back end differs - so this answers the gate with
// "no stencil shadows" while two local players are active: exactly what the
// Low setting does, everywhere at once. Single player keeps High.
// [splitshadows] lines.
//
// Phase 12e: the per-player motion blur draw (GOG 1.3 0x00470320) is skipped
// while split screen is on. It copies each view into ONE texture, stretched to
// fill it, then samples it at the view's half - so each player's blur shows the
// other player's picture, stretched 2x. [splitblur] lines.
//
// Phase 12g-r2: the screen blur's final blend-back (end of BlurEffect::Render,
// GOG 1.3 0x0045D110) is skipped while split screen is on - each view's blur
// overlay was half its picture stretched 2x. The rest of the pass still runs:
// it refreshes the effects layer that holograms / refraction sample (skipping
// the whole pass in r1 froze that layer and left trails). [splitblur] lines.
//
// Phase 12h: the REAL stencil fix. The shadow-volume setup's column 2 (GOG 1.3
// 0x006D6010) ends with colour writes ON and blending off, so the volumes are
// painted onto the screen (the coloured wedges). In split screen its final
// COLORWRITE call now gets 0 instead of 7: the volumes only mark the stencil
// and High stencil shadows work. [splitstencil] lines.
//
// BF1GameExt.ini [Display] SplitShadowsLow, SplitMotionBlurOff,
// SplitScreenBlurOff, SplitStencilFix (all default 1; 0 = leave that alone).
// With SplitStencilFix=1, set SplitShadowsLow=0 to keep High stencil shadows.
#pragma once

namespace bf1 {

// Call after the code is unlocked, on the game thread. Installs both fixes.
void SplitShadowsStart(bool onGameThread);

} // namespace bf1
