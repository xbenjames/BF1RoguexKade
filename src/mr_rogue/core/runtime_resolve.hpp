// BF1GameExt - runtime_resolve.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// For builds whose code is locked on disk (Steam): finds the split screen and
// controls sites in memory, after Steam has unlocked the game, using the GOG
// 1.3 code patterns (same source code), then publishes the addresses it found.
#pragma once

namespace bf1 {

// Call once, after CodeLooksUnlocked() is true. Logs every step as [resolve].
// Returns true if the split screen part was found; the controls part is
// published separately and can fail on its own.
bool ResolveRuntimeBuild();

} // namespace bf1
