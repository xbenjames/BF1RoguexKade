// BF1GameExt - egm.hpp   (file build p23a-r1)
// Credit: EGM's patches - the SWBF Launcher by Antstafer and abkarino (CC BY-NC 4.0: credit them, no commercial use);
//         brought into BF1GameExt by Mr. Rogue. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/antstafer_abkarino_egm/ (was egm_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 22: EGM's patches - 22a the scan, 22b the patches in (BF1GameExt.ini [EGM], one switch per group). EGM's patches
// come from the SWBF Launcher by Antstafer and abkarino (EGM's memory launcher, CC BY-NC 4.0: credited, kept free).
// See egm.cpp.
#pragma once

namespace bf1 {

// At load (DllMain, main thread): puts BF1GameExt's start-up hook in the C++ start-up table's first slot. When the game's
// start-up calls it (Steam: after its unlock), it finds every EGM pattern, applies the early groups (RenderPools, Foliage,
// FixEffects' scene queue), puts the slot back and runs the game's own function. Steam: also times the unlock.
// [EGM] InitProbe=0: no hook (the early groups are then left alone).
void EgmScanEarly();

// With the other scans (the game's DirectInput call, after [Limits]): checks every EGM site and applies the other groups
// (only from the game's own thread). [EGM] Scan: 0 quiet, 1 a line per group, 2 every site.
void EgmScanRun(bool onGameThread);

} // namespace bf1
