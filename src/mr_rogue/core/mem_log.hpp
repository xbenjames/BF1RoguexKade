// BF1GameExt - mem_log.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 22c: the memory log ([mem] lines, BF1GameExt.ini [Limits] MemoryLog). Log only: it changes nothing the game uses
// except one pointer the game itself never sets (its heap allocator's "allocation failed" callback, unused in both exes).
// See mem_log.cpp.
#pragma once

#include <cstddef>

namespace bf1 {

// At the game's DirectInput call (after the limits and EGM's patches): finds the game's heap allocator and heap table by
// pattern, puts BF1GameExt's callback in the allocator's failure slot, logs the first [mem] lines and starts the watcher
// (a thread: every second it samples the address space and the game's heaps; a [mem] line every MemoryLog seconds, and
// whenever the map changes). [Limits] MemoryLog=0: nothing.
void MemLogStart(bool onGameThread);

// DLL_PROCESS_DETACH: the game's failure slot back to 0 (if it is still BF1GameExt's).
void MemLogStop();

// The crash logger (crashlog.cpp): the address space, the game's heaps and the failure counts as [exception] lines.
// No allocation.
void MemLogCrash();

// render_hooks.cpp's Direct3D create wrappers (any thread). kind: 0 texture, 1 volume texture, 2 cube texture,
// 3 vertex buffer, 4 index buffer; pool: D3DPOOL (0 default, 1 managed, 2 system memory, 3 scratch).
void MemLogD3dMade(int kind, unsigned pool, unsigned long long bytes);
void MemLogD3dFailed(int kind, long hr, unsigned w, unsigned h, unsigned depthOrLevels, unsigned usage, unsigned format,
                     unsigned pool, const void* caller);

// "address space: 312 MB free, the largest free block 45 MB" (for other modules' lines; a fresh look each call).
const char* MemLogVaShort(char* out, size_t n);

} // namespace bf1
