// BF1GameExt - stamina_hud.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/sprint/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 14c: HUD scan for the stamina bar (2020 build: GOG 1.3 / Steam), log-only.
//
// Each local player has a StatusDisplay (0x34A0 bytes; pointers in a 2-slot
// global array, GOG 1.3 0x009ECBA0) that holds the HUD panels as embedded
// interface elements:
//   +0x0130  status panel (health, and the secondary bar: vehicle health /
//            repair / JET PACK FUEL / shield, with its icon at +0x0500 and its
//            bars at +0x0600 and +0x0700)
//   +0x16C0  weapon panel (primary weapon panel at +0x1790, secondary =
//            grenade panel at +0x1FE0)
//   +0x2840  a third panel
// This finds the array and the interface element classes (by their RTTI
// names), then - 3 s after each spawn, at most 3 times per player - lists
// every element inside the player's StatusDisplay: offset, class, visible,
// position, colour and, for bitmaps, the rectangle, UVs and texture.
// [hud] lines.
// Phase 14d: the stamina bar. Each weapon sub-panel has a bar the game only
// shows for a charging / heating weapon; the grenade panel's one is the jet
// fuel bar art. StatusDisplay::Update goes through BF1GameExt (vtable slot):
// after the game's update, if it left that bar hidden, the bar is shown under
// the grenade icon in green, cut to the stamina left (and put back exactly as
// it was before the next update). INI [Sprint] StaminaBar*.
// Phase 14e: red while it refills after running out, green again once there
// is enough to sprint.
#pragma once

#include <cstdint>

namespace bf1 {

// Finds the StatusDisplay array and the element classes. Call after the code
// is unlocked (from SprintStart).
void StaminaHudStart();

// Player p's soldier, every frame, on the game thread (from the Move hook).
void StaminaHudFrame(int p, uintptr_t soldier);

} // namespace bf1
