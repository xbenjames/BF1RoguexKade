// BF1GameExt - crashlog.cpp   (file build p25e-r1)
// 25e (v6.9): LogFlush at the end of each report (log.cpp buffers now).
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Vectored exception handler that only logs. Exceptions are reported when they
// are first raised, so a logged one may still be handled by the game; the last
// [exception] entry before "session end" is normally the crash.
// 22c: two more parts in each report - the calls on the stack (the return
// addresses found in the faulting thread's stack whose bytes before them are a
// call instruction, nearest first, as exe+rva / module+offset: who called what
// led to the fault), and the memory at that moment (mem_log.cpp: the address
// space, the game's heaps, its failed allocations so far).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "crashlog.hpp"
#include "log.hpp"
#include "mem_log.hpp"

namespace bf1 {
namespace {

constexpr LONG kMaxLogged = 8;
volatile LONG g_logged = 0;

bool Readable(uintptr_t addr, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    const DWORD prot = mbi.Protect & 0xFF;
    if (prot == 0 || prot == PAGE_EXECUTE)
        return false;
    return addr + len <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

void ModuleOf(uintptr_t addr, char* out, size_t outSize)
{
    HMODULE hm = nullptr;
    char path[MAX_PATH] = {};
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(addr), &hm) &&
        hm && GetModuleFileNameA(hm, path, MAX_PATH))
    {
        const char* name = strrchr(path, '\\');
        snprintf(out, outSize, "%s+0x%X", name ? name + 1 : path,
                 static_cast<unsigned>(addr - reinterpret_cast<uintptr_t>(hm)));
    }
    else
    {
        snprintf(out, outSize, "no module");
    }
}

// 22c: v is a return address - executable code whose bytes before it are a call (rel32, [abs32], reg, [reg], [reg+disp])
bool LooksLikeReturn(uintptr_t v, uintptr_t& codeLo, uintptr_t& codeHi)
{
    if (v < 0x10000 || v > 0xFFFF0000u)
        return false;
    if (!(v - 7 >= codeLo && v < codeHi))
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(v - 7), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT)
            return false;
        const DWORD prot = mbi.Protect & 0xFF;
        if (prot != PAGE_EXECUTE_READ && prot != PAGE_EXECUTE_READWRITE && prot != PAGE_EXECUTE_WRITECOPY)
            return false;
        codeLo = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        codeHi = codeLo + mbi.RegionSize;
        if (!(v - 7 >= codeLo && v < codeHi))
            return false;
    }
    const unsigned char* b = reinterpret_cast<const unsigned char*>(v);
    if (b[-5] == 0xE8)
        return true;
    if (b[-6] == 0xFF && (b[-5] == 0x15 || (b[-5] & 0xF8) == 0x90))
        return true;
    if (b[-2] == 0xFF && ((b[-1] & 0xF8) == 0xD0 || ((b[-1] & 0xF8) == 0x10 && b[-1] != 0x14 && b[-1] != 0x15)))
        return true;
    if (b[-3] == 0xFF && (((b[-2] & 0xF8) == 0x50 && b[-2] != 0x54) || b[-2] == 0x14))
        return true;
    if (b[-4] == 0xFF && b[-3] == 0x54)
        return true;
    return b[-7] == 0xFF && b[-6] == 0x94;
}

// 22c: the calls on the faulting thread's stack, nearest first
void LogStackCalls(uintptr_t sp)
{
    const NT_TIB* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    const uintptr_t top = reinterpret_cast<uintptr_t>(tib->StackBase);
    const uintptr_t low = reinterpret_cast<uintptr_t>(tib->StackLimit);
    if (sp < low || sp >= top)
    {
        Log("[exception]   calls on the stack: esp is not in this thread's stack");
        return;
    }
    char line[700];
    int pos = snprintf(line, sizeof(line), "[exception]   calls on the stack (nearest first):");
    int found = 0;
    uintptr_t codeLo = 0; // the last code region looked up (this report's own: two threads may fault at once)
    uintptr_t codeHi = 0;
    for (uintptr_t a = sp & ~static_cast<uintptr_t>(3); a + 4 <= top && a < sp + 4096 && found < 16; a += 4)
    {
        uint32_t v = 0;
        memcpy(&v, reinterpret_cast<const void*>(a), sizeof(v));
        if (!LooksLikeReturn(v, codeLo, codeHi))
            continue;
        char where[MAX_PATH + 32];
        ModuleOf(v, where, sizeof(where));
        const char* name = strrchr(where, '+') ? where : "?";
        if (pos > 0 && pos < static_cast<int>(sizeof(line)) - 48)
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %s [esp+%X]", name,
                            static_cast<unsigned>(a - sp));
        ++found;
    }
    if (!found && pos > 0 && pos < static_cast<int>(sizeof(line)) - 8)
        snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " none found");
    Log("%s", line);
}

const char* ExceptionName(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_ACCESS_VIOLATION:      return "ACCESS VIOLATION";
    case EXCEPTION_ILLEGAL_INSTRUCTION:   return "ILLEGAL INSTRUCTION";
    case EXCEPTION_PRIV_INSTRUCTION:      return "PRIVILEGED INSTRUCTION";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:    return "INTEGER DIVIDE BY ZERO";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "ARRAY BOUNDS EXCEEDED";
    case EXCEPTION_IN_PAGE_ERROR:         return "IN PAGE ERROR";
    default:                              return nullptr;
    }
}

LONG CALLBACK CrashVeh(PEXCEPTION_POINTERS ep)
{
    const EXCEPTION_RECORD* er = ep->ExceptionRecord;
    const char* name = ExceptionName(er->ExceptionCode);
    if (!name)
        return EXCEPTION_CONTINUE_SEARCH;

    const LONG n = InterlockedIncrement(&g_logged);
    if (n > kMaxLogged)
        return EXCEPTION_CONTINUE_SEARCH;

    const uintptr_t at = reinterpret_cast<uintptr_t>(er->ExceptionAddress);
    char where[MAX_PATH + 32];
    ModuleOf(at, where, sizeof(where));

    Log("[exception] #%ld %s (0x%08lX) at 0x%08X (%s), thread %lu", static_cast<long>(n), name,
        static_cast<unsigned long>(er->ExceptionCode), static_cast<unsigned>(at), where,
        static_cast<unsigned long>(GetCurrentThreadId()));

    if (er->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && er->NumberParameters >= 2)
    {
        const ULONG_PTR kind = er->ExceptionInformation[0];
        Log("[exception]   tried to %s address 0x%08X",
            kind == 0 ? "READ" : (kind == 1 ? "WRITE" : "EXECUTE"),
            static_cast<unsigned>(er->ExceptionInformation[1]));
    }

    const CONTEXT* c = ep->ContextRecord;
    Log("[exception]   eax=%08lX ebx=%08lX ecx=%08lX edx=%08lX esi=%08lX edi=%08lX",
        static_cast<unsigned long>(c->Eax), static_cast<unsigned long>(c->Ebx),
        static_cast<unsigned long>(c->Ecx), static_cast<unsigned long>(c->Edx),
        static_cast<unsigned long>(c->Esi), static_cast<unsigned long>(c->Edi));
    Log("[exception]   ebp=%08lX esp=%08lX eip=%08lX", static_cast<unsigned long>(c->Ebp),
        static_cast<unsigned long>(c->Esp), static_cast<unsigned long>(c->Eip));

    if (Readable(at, 16))
        LogBytes("[exception]   code ", static_cast<uint32_t>(at), reinterpret_cast<const void*>(at),
                 16);

    const uintptr_t sp = static_cast<uintptr_t>(c->Esp);
    if (Readable(sp, 32))
    {
        char line[160];
        int pos = snprintf(line, sizeof(line), "[exception]   stack:");
        for (int i = 0; i < 8 && pos > 0 && pos < static_cast<int>(sizeof(line)) - 12; ++i)
        {
            uint32_t v = 0;
            memcpy(&v, reinterpret_cast<const void*>(sp + static_cast<uintptr_t>(i) * 4), sizeof(v));
            pos += snprintf(line + pos, sizeof(line) - pos, " %08X", static_cast<unsigned>(v));
        }
        Log("%s", line);
    }
    LogStackCalls(sp); // 22c
    MemLogCrash();     // 22c

    if (n == kMaxLogged)
        Log("[exception] limit of %ld reached, no more exceptions will be logged",
            static_cast<long>(kMaxLogged));
    LogFlush(); // 25e (v6.9): the log is buffered now - the report goes to disk before the game may go
    return EXCEPTION_CONTINUE_SEARCH;
}

} // namespace

void CrashLogInstall()
{
    if (AddVectoredExceptionHandler(0, CrashVeh))
        Log("crash logger: installed (logs faults, never handles them)");
    else
        Log("crash logger: FAILED to install (error %lu)", GetLastError());
}

} // namespace bf1
