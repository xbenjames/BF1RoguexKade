// BF1GameExt - ragdoll.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/ragdoll/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r17: RagdollMenuState / RagdollMenuSet (the MODS page's RAGDOLL EFFECTS row).
// 19b: RagdollOrigDeathFn().
// Phase 15a: Bullet Physics inside BF1GameExt - integration test (log-only).
// Bullet 3.25 (zlib license) is compiled into the DLL from bullet3\src's three
// single-file builds. The test drops a 1 m box onto a ground plane in a
// private Bullet world and logs that it lands and comes to rest at y = 0.5.
// Phase 15b: ragdoll scan (log-only, 2020 builds: GOG 1.3 / Steam). Three hooks
// read, never change, the game: the soldier render's two calls to the
// animation controller update (the bone matrices the renderer draws), the
// soldier's death function and its delete (both EntitySoldier vtable slots).
// Logged: the skeletons, a model-space / terrain check on player 1's soldier,
// every death, the body's pose after death near the players, and when bodies
// are removed. [ragdoll] lines. From 15c its logging is off unless INI
// [Ragdoll] Scan=1.
// Phase 15c: first ragdoll. Soldiers who die on terrain near another local
// player fall as Bullet ragdolls (ragdoll_body.cpp) instead of playing the
// death animation: their bone matrices are replaced after each controller
// update until the body is gone (INI [Ragdoll] Enabled / Range / MaxBodies /
// Debug).
// Phase 15d: self-collision and fading muscle tone (ragdoll_body.cpp); the
// hit scan, log-only (INI [Ragdoll] HitScan, [hit] lines): pushes, damage and
// thrown soldiers, for thrown ragdolls, rocket push and headshot punch next.
// Phase 15e: thrown ragdolls (players too), explosions pushing ragdolls,
// survivors blending back into their animation, live tuning (F7 / F8).
// Phase 15f: muscles follow the game's animation and the body blends in from
// it; headshot punch, hit push, blast tumble; seat scan (log-only).
#pragma once

namespace bf1 {

// Call once, after the scan. Runs the Bullet test (INI [Ragdoll] BulletTest,
// default 1), then sets up the hooks for the ragdolls (INI [Ragdoll] Enabled,
// default 1) and the 15b scan (INI [Ragdoll] Scan, default 0). The hooks are
// only installed when onGameThread is true.
void RagdollStart(bool onGameThread);

// 19b: the game's own soldier death function (the +8 sub-object's vtable slot 1) while the ragdoll
// hook sits in that slot, 0 when the slot is the game's own (or ragdolls are off).
uintptr_t RagdollOrigDeathFn();

// 21j-r17: the MODS page's RAGDOLL EFFECTS row: 1 on, 0 off ([Ragdoll] Enabled as used now), -1 = the ragdolls are not set up
// this session (no hooks / not on this build).
int RagdollMenuState();

// 21j-r17: RAGDOLL EFFECTS ON / OFF from the MODS page - at once (off ends the running ragdolls), saved in BF1GameExt.ini
// [Ragdoll] Enabled (and ragdoll_live.ini when it has the key). The new state (-1 = not set up, nothing changed).
int RagdollMenuSet(bool on);

} // namespace bf1
