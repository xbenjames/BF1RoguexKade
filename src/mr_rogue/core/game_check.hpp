// BF1GameExt - game_check.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/ (was scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Read-only diagnostics (GOG 1.2 or GOG 1.3). Installs no hooks, patches nothing.
#pragma once

#include <cstdint>

namespace bf1 {

// Called from DllMain. Cheap: checks the exe identity and section list.
void ScanOnAttach();

// True if the exe still has BF1MemExt.exe's ".bf1ext" startup section.
bool ScanExePatchedByMemExt();

// True once ScanRun has confirmed the 1.2 exe and all split screen sites
// byte-check OK. Nothing may write to the split screen values otherwise.
bool ScanSplitSitesOk();

// True if the exe is a known build (see game_build.cpp).
bool ScanIdentityOk();

// Milliseconds since DllMain, the same clock the [watch] lines use.
unsigned long ScanMsSinceAttach();

// Address of the function pointer in the game's Lua registration slot for
// 'name' (e.g. "ScriptCB_SetSplitscreen"), or 0 if not found.
uintptr_t ScanLuaFuncSlot(const char* name);

// Full scan. realDI8Create = the real dinput8 DirectInput8Create (may be null).
// Starts the value watcher (1.2 exe only) and the menu-script check.
void ScanRun(const char* trigger, void* realDI8Create);

} // namespace bf1
