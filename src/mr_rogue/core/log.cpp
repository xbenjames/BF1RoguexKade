// BF1GameExt - log.cpp   (file build p25e-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// 21j-r11: Log's time on the game's thread is the frame-rate scan's kPerfLog part (render_hooks.cpp, [perf] lines; the wait
// for the lock and the write). Nothing else changed.
// 25e (v6.9): the writes are off the game's thread. Ben's log of Oct 26 showed freezes of 1.5 - 4.8 s at the start of a
// match, all in "the log" (4840 ms over 26 lines in one frame) with the game's thread on the CPU 7% of the time: WriteFile
// itself was waiting on the disk (or on whatever scans the file). Log now only copies the line into a memory buffer (4 MB;
// a full buffer drops lines and says how many); a writer thread writes the buffer every 100 ms (sooner once 64 KB wait).
// LogFlush writes what waits right away (the crash handler calls it, so a crash's report is on disk before the game goes);
// LogNoLock (process exit) writes what waits and its line directly.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "render_hooks.hpp"
#include "log.hpp"

namespace bf1 {
namespace {

constexpr DWORD kBufSize = 4u << 20;
constexpr DWORD kWakeAt = 64u << 10;
constexpr DWORD kPeriodMs = 100;

HANDLE g_file = INVALID_HANDLE_VALUE;
CRITICAL_SECTION g_lock;      // the buffer
CRITICAL_SECTION g_writeLock; // the file (keeps the order of the writes)
bool g_lockReady = false;
char* g_buf = nullptr;        // lines waiting
DWORD g_len = 0;
char* g_spare = nullptr;      // the writer's buffer
volatile LONG g_dropped = 0;
HANDLE g_wake = nullptr;
bool g_threaded = false;

int FormatLine(char* buf, size_t size, const char* fmt, va_list args)
{
    int n = vsnprintf(buf, size - 2, fmt, args);
    if (n < 0)
        return 0;
    if (n > static_cast<int>(size) - 3)
        n = static_cast<int>(size) - 3;
    buf[n++] = '\r';
    buf[n++] = '\n';
    return n;
}

void WriteRaw(const char* p, DWORD n)
{
    while (n)
    {
        DWORD written = 0;
        if (!WriteFile(g_file, p, n, &written, nullptr) || !written)
            return;
        p += written;
        n -= written;
    }
}

// Takes what waits and writes it (the caller does not hold g_lock).
void Drain()
{
    EnterCriticalSection(&g_writeLock);
    EnterCriticalSection(&g_lock);
    char* out = g_buf;
    const DWORD n = g_len;
    g_buf = g_spare;
    g_spare = out;
    g_len = 0;
    const LONG dropped = InterlockedExchange(&g_dropped, 0);
    LeaveCriticalSection(&g_lock);
    if (n)
        WriteRaw(out, n);
    if (dropped)
    {
        char line[96];
        const int k = snprintf(line, sizeof(line), "[log] %ld line(s) dropped (the buffer was full)\r\n", static_cast<long>(dropped));
        if (k > 0)
            WriteRaw(line, static_cast<DWORD>(k));
    }
    LeaveCriticalSection(&g_writeLock);
}

DWORD WINAPI WriterThread(void*)
{
    for (;;)
    {
        WaitForSingleObject(g_wake, kPeriodMs);
        Drain();
    }
}

void AddLine(const char* fmt, va_list args)
{
    char line[1024];
    const int n = FormatLine(line, sizeof(line), fmt, args);
    if (n <= 0)
        return;
    if (!g_threaded)
    {
        EnterCriticalSection(&g_writeLock);
        WriteRaw(line, static_cast<DWORD>(n));
        LeaveCriticalSection(&g_writeLock);
        return;
    }
    bool wake = false;
    EnterCriticalSection(&g_lock);
    if (g_len + static_cast<DWORD>(n) <= kBufSize)
    {
        memcpy(g_buf + g_len, line, static_cast<size_t>(n));
        g_len += static_cast<DWORD>(n);
        wake = g_len >= kWakeAt;
    }
    else
    {
        InterlockedIncrement(&g_dropped);
    }
    LeaveCriticalSection(&g_lock);
    if (wake)
        SetEvent(g_wake);
}

} // namespace

bool LogOpen(const char* path)
{
    if (!g_lockReady)
    {
        InitializeCriticalSection(&g_lock);
        InitializeCriticalSection(&g_writeLock);
        g_lockReady = true;
    }

    g_file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (g_file == INVALID_HANDLE_VALUE)
        return false;

    g_buf = static_cast<char*>(VirtualAlloc(nullptr, kBufSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    g_spare = static_cast<char*>(VirtualAlloc(nullptr, kBufSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    g_wake = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    if (g_buf && g_spare && g_wake)
    {
        // Made in DllMain: the thread runs once the loader lock is let go; until then the lines wait in the buffer.
        const HANDLE t = CreateThread(nullptr, 0, WriterThread, nullptr, 0, nullptr);
        if (t)
        {
            CloseHandle(t);
            g_threaded = true;
        }
    }
    return true;
}

void Log(const char* fmt, ...)
{
    if (!g_lockReady || g_file == INVALID_HANDLE_VALUE)
        return;

    const bool perf = FramePerfEnter(kPerfLog); // 21j-r11: the frame-rate scan (nothing unless this is the game's thread)
    va_list args;
    va_start(args, fmt);
    AddLine(fmt, args);
    va_end(args);
    FramePerfLeave(perf);
}

void LogFlush()
{
    if (!g_lockReady || g_file == INVALID_HANDLE_VALUE || !g_threaded)
        return;
    Drain();
}

void LogNoLock(const char* fmt, ...)
{
    if (g_file == INVALID_HANDLE_VALUE)
        return;
    // Process exit: the other threads are gone (maybe inside a lock), so no locks - what waits, then this line.
    if (g_threaded && g_buf && g_len)
    {
        WriteRaw(g_buf, g_len);
        g_len = 0;
    }
    char line[1024];
    va_list args;
    va_start(args, fmt);
    const int n = FormatLine(line, sizeof(line), fmt, args);
    va_end(args);
    if (n > 0)
        WriteRaw(line, static_cast<DWORD>(n));
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
