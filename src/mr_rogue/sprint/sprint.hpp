// BF1GameExt - sprint.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/sprint/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r17: SprintMenuState / SprintMenuSet (the MODS page's SPRINT row).
// Phase 14a: sprint scan + test boost (2020 build: GOG 1.3 / Steam).
//
// BF1 has no sprint. A soldier's run speed lives in its class (MaxSpeed,
// shared by every soldier of that class, AI included) and is used in exactly
// one place: EntitySoldier::Move, which every soldier calls once per frame
// with its inputs (dt, forward, strafe, turn, brake). Move aims the soldier's
// velocity at MaxSpeed * forward input, so scaling ONE soldier's forward
// input on the way in makes that soldier alone run faster.
//
// This phase hooks Move's entry (log-only, plus a test boost):
//   - recognises player 1's and player 2's soldiers (soldier + 0x250 is the
//     controllable their character drives)
//   - logs their class speeds, movement state, inputs and measured speed
//   - watches the sprint button (L3 on the player's pad; Left Shift for
//     player 1 on the keyboard) and whether the game also reacts to it
//   - test boost: BF1GameExt.ini [Sprint] TestBoost=1 - while the button is
//     held, standing, and moving, the forward input x SpeedMult (default 1.5)
// [sprint] lines.
// Phase 14b: the boost is forward only; [Sprint] L3SprintOnly=1 takes the
// game's "view" toggle off L3 (both players); [Splitscreen] P2ThirdPerson=1
// keeps player 2 in third person. [view] lines.
// Phase 14c: stamina (BF2's defaults, INI [Sprint] Stamina*), and the HUD scan
// for the stamina bar (stamina_hud.cpp).
// Phase 14e: log-only animation scan ([anim] lines): soldier fields that a
// sprint changes compared with normal running.
// Phase 14f: the run animation at sprint speed - the move animation selector's
// 8 m/s cap (above it: standing pose) is raised to INI [Sprint] AnimSpeedLimit.
#pragma once

namespace bf1 {

// Finds Move by pattern, checks it, installs the entry hook (14b: also the
// L3 change and the player 2 view code). Call after the
// code is unlocked, on the game thread (the hook is skipped otherwise).
void SprintStart(bool onGameThread);

// Player p's stamina, 0..1 (1 when stamina is off). For the HUD bar.
float SprintStamina01(int p);

// 14d, for the stamina bar: 0 = no bar (stamina off, or not on foot in the last
// 250 ms), 1 = normal, 2 = low (emptied, or below the minimum to start a
// sprint). *fraction = stamina 0..1 when non-zero is returned.
int SprintHudState(int p, float* fraction);

// 21j-r17: the MODS page's SPRINT row: 1 on, 0 off ([Sprint] TestBoost as used now), -1 = Move not hooked this session.
int SprintMenuState();

// 21j-r17: SPRINT ON / OFF from the MODS page - at once (off: no boost, no stamina bar), saved in BF1GameExt.ini [Sprint]
// TestBoost. The new state (-1 = not hooked, nothing changed).
int SprintMenuSet(bool on);

} // namespace bf1
