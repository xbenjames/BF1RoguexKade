// BF1GameExt - addons.hpp   (v6.4, Ben James)
// The add-on loader (GameData\dinput8_*.dll) and the MODS page's registry of mods (add-ons' and BF1GameExt's own rows).
// See bfx_addon.h for the interface an add-on sees.
#pragma once

#include <cstddef>
#include "bfx_addon.h"

namespace bf1 {

// DllMain: load every GameData\dinput8_*.dll in name order ([Addons] LoadAddons=1 in BF1GameExt.ini; default on).
// gameDir = the exe's folder. Logs each one as [addon] lines.
void AddonsLoad(const char* gameDir, const char* hostBuild);

// The game's DirectInput8Create call, the code unlocked: every add-on's BFX_Init, once. Call BEFORE BF1GameExt's own hooks.
void AddonsInit();

// After the real DirectInput8Create made the object: every add-on's BFX_DirectInput, once.
void AddonsDirectInput(void* directInput8);

// How many add-ons were loaded.
int AddonsCount();

// --- the MODS registry ---
// BF1GameExt's own features register here too (sprint, ragdolls ...), so the page treats them like any add-on.
void ModsRegister(const BfxModOptions* mod);

// Everything registered (add-ons first, in load order, then BF1GameExt's own), skipping mods with no rows.
int ModsCount();
const BfxModOptions* ModsAt(int k);

// The Lua chunk that sets gBFXM_R (the MODS page's rows): one entry per mod { name, on (its MASTER row's value or nil),
// k }. Writes into buf; returns the length (0 = nothing / too small).
size_t ModsRowsLua(char* buf, size_t size);

// The Lua chunk that sets gBFXM_P for mod k: { name, rows = { { label, text, avail, kind, restart } ... } }.
size_t ModsPageLua(int k, char* buf, size_t size);

// Turn option i of mod k by dir (+1 / -1: ON <-> OFF, or the next / previous choice). Returns the value now (-1 = not
// available / bad k, i).
int ModsTurn(int k, int i, int dir);

} // namespace bf1
