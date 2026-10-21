// BF1GameExt - loadscreen_log.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was load_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 9a: read-only scan of the loading screen's layout (2020 build).
//
// The loading screen is a Lua interface script laid out in 4:3 (the game's
// ScriptCB_GetTargetAspect returns 0.75), so on a wide screen it is boxed with
// black bars and the planet name lands in the left bar. This wraps the
// interface Lua calls the script uses (log-only) and, while armed, logs every
// number they receive or return, so the fix can widen exactly the loading
// picture. Starting a map (ScriptCB_EnterMission / RestartMission) arms it for
// 60 s by itself; Insert arms it by hand. [loadscan] lines.
#pragma once

namespace bf1 {

// Finds the game's Lua number helpers and wraps the interface Lua functions.
// Call during the scan (before the game registers its Lua functions).
void LoadScanStart();

// Insert: log the next 60 seconds.
void LoadScanArm();

} // namespace bf1
