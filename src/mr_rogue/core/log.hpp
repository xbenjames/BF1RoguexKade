// BF1GameExt - log.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Simple line logger. One file, truncated on every launch.
#pragma once
#include <cstddef>
#include <cstdint>

namespace bf1 {

bool LogOpen(const char* path);

// Thread-safe. Adds CRLF.
void Log(const char* fmt, ...);

// For DLL_PROCESS_DETACH only (other threads may be dead and holding the lock).
void LogNoLock(const char* fmt, ...);

// Hex dump, 16 bytes per line. 'rva' is printed as the address of data[0].
void LogBytes(const char* indent, uint32_t rva, const void* data, size_t len);

} // namespace bf1
