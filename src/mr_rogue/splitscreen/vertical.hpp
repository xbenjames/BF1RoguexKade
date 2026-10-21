// BF1GameExt - vertical.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 8a: vertical (side-by-side) split screen (2020 build: GOG 1.3 / Steam).
//
// CameraManager::SetNumCameras(2) gives the two views the rects (x, y, w, h)
// (0, 0, 1, 0.5) and (0, 0.5, 1, 0.5) and each camera SetPerspective(near 0.7,
// far 120, fov 48 deg, ratio = screen h/w * 0.5): the same 48 deg across, the
// height halved. Side by side is what the Xbox build does (its camera manager's
// split-direction setter, 0x00048950 in the XBE): rects (0, 0, 0.5, 1) and
// (0.5, 0, 0.5, 1), ratio * 2, and a fixed 0.6283 rad = 36 deg across each half
// (the Xbox's top / bottom uses 0.6283 / screen ratio = 48 deg at 4:3, the same
// as PC, so the two builds agree on everything but this).
//
// Five byte-checked edits inside SetNumCameras' 2-player block, made at start
// (on the game thread, before any match): the two rect operands and the ratio
// operand point at values in BF1GameExt, and the two FOV immediates change.
// The game's shared constants are not touched. The block is found by anchors
// (the rect table and its loads), not one long pattern, because Steam's build
// of it differs slightly from GOG 1.3's. [vertical] lines.
//
// BF1GameExt.ini [Splitscreen] Vertical=1 turns it on (default 0). 21i-r8: SplitMenu=1 installs the edits either way
// (with the game's own values while Vertical=0) so GAME OPTIONS can switch it.
#pragma once

namespace bf1 {

// Call after the code is unlocked. onGameThread must be true (patching code
// the game thread could be running is not safe).
void VerticalStart(bool onGameThread);

// 21i-r8: switchable with [Splitscreen] SplitMenu=1 (the GAME OPTIONS page's SPLIT SCREEN row): installed = the five
// edits went in; on = side by side for the next SetNumCameras(2); set: the layout for the next match (and the INI's Vertical).
bool VerticalSwitchable();
bool VerticalOn();
bool VerticalWanted();         // the pending choice (made in a match) if any, else the layout in force
bool VerticalPendingDiffers(); // a choice made in a match waits for the main menu
bool VerticalSet(bool vertical, bool now); // now: from the main menu; else pending until it is made again
void VerticalApplyPending();   // the main menu made again (ifs_main)

} // namespace bf1
