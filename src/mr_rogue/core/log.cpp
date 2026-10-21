// BF1GameExt - log.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r11: Log's time on the game's thread is the frame-rate scan's kPerfLog part (render_hooks.cpp, [perf] lines; the wait
// for the lock and the write). Nothing else changed.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include "render_hooks.hpp"
#include "log.hpp"

namespace bf1 {
namespace {

HANDLE g_file = INVALID_HANDLE_VALUE;
CRITICAL_SECTION g_lock;
bool g_lockReady = false;

void WriteLineV(const char* fmt, va_list args)
{
    if (g_file == INVALID_HANDLE_VALUE)
        return;

    char buf[1024];
    int n = vsnprintf(buf, sizeof(buf) - 2, fmt, args);
    if (n < 0)
        return;
    if (n > static_cast<int>(sizeof(buf)) - 3)
        n = static_cast<int>(sizeof(buf)) - 3;

    buf[n++] = '\r';
    buf[n++] = '\n';

    DWORD written = 0;
    WriteFile(g_file, buf, static_cast<DWORD>(n), &written, nullptr);
}

} // namespace

bool LogOpen(const char* path)
{
    if (!g_lockReady)
    {
        InitializeCriticalSection(&g_lock);
        g_lockReady = true;
    }

    g_file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    return g_file != INVALID_HANDLE_VALUE;
}

void Log(const char* fmt, ...)
{
    if (!g_lockReady)
        return;

    const bool perf = FramePerfEnter(kPerfLog); // 21j-r11: the frame-rate scan (nothing unless this is the game's thread)
    EnterCriticalSection(&g_lock);
    va_list args;
    va_start(args, fmt);
    WriteLineV(fmt, args);
    va_end(args);
    LeaveCriticalSection(&g_lock);
    FramePerfLeave(perf);
}

void LogNoLock(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    WriteLineV(fmt, args);
    va_end(args);
}

void LogBytes(const char* indent, uint32_t rva, const void* data, size_t len)
{
    const unsigned char* p = static_cast<const unsigned char*>(data);

    for (size_t off = 0; off < len; off += 16)
    {
        char line[160];
        int pos = snprintf(line, sizeof(line), "%s%08X:", indent,
                           static_cast<unsigned>(rva + off));
        if (pos < 0)
            return;

        for (size_t i = off; i < off + 16 && i < len; ++i)
        {
            if (pos > static_cast<int>(sizeof(line)) - 4)
                break;
            pos += snprintf(line + pos, sizeof(line) - pos, " %02X", p[i]);
        }
        Log("%s", line);
    }
}

} // namespace bf1
