// BF1GameExt - bf1memext_loader.cpp   (file build p23a-r1)
// Credit: BF1MemExt (the extended limits DLL this loads on GOG 1.2), and Dark_Phantom. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/bf1memext/ (was memext_loader.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Loads BF1MemExt.dll at the game's entry point, without patching the exe file.
// GOG 1.2 only: BF1MemExt's patches target the 1.2 code, and on any other exe
// it shows "Failed to apply patches!" and closes the game, so it is never
// loaded there.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "bf1memext_loader.hpp"

namespace bf1 {
namespace {

// GOG 1.2 entry point (VS2003 CRT startup), rva 0x0026A1CC:
//   6A 60              push 60h
//   68 F0 8D 6B 00     push 006B8DF0h
//   E8 90 35 00 00     call __SEH_prolog
// The hook overwrites the first 5 bytes with a JMP. Nothing branches into
// bytes 1..4, and the bytes are restored before the entry ever runs.
// The entry RVA and bytes are byte-checked from the build table first.
constexpr size_t kHookSize = 5;
constexpr int kMaxCopiedLogLines = 150;

uintptr_t g_entry = 0;
unsigned char g_savedEntry[kHookSize] = {};
char g_logBuf[32 * 1024];

typedef int(__cdecl* CrtEntry_t)(void);

bool WriteCode(uintptr_t addr, const void* bytes, size_t n)
{
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(addr), n, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(reinterpret_cast<void*>(addr), bytes, n);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(addr), n, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(addr), n);
    return true;
}

void GameDir(char* out, size_t outSize)
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        exe[0] = 0;
    const char* slash = strrchr(exe, '\\');
    const int dirLen = slash ? static_cast<int>(slash - exe) : 0;
    snprintf(out, outSize, "%.*s", dirLen, exe);
}

// BF1MemExt writes BF1MemExt.log in the working directory. Copy it into our
// log so one file covers both DLLs.
void CopyMemExtLog()
{
    char cwd[MAX_PATH] = {};
    const DWORD n = GetCurrentDirectoryA(MAX_PATH, cwd);
    if (!n || n >= MAX_PATH)
        cwd[0] = 0;

    char path[MAX_PATH + 32];
    snprintf(path, sizeof(path), "%s\\BF1MemExt.log", cwd);

    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
    {
        Log("  BF1MemExt.log not found at %s", path);
        return;
    }

    DWORD got = 0;
    if (!ReadFile(f, g_logBuf, sizeof(g_logBuf) - 1, &got, nullptr))
        got = 0;
    CloseHandle(f);
    g_logBuf[got] = 0;

    Log("  --- BF1MemExt.log (%s) ---", path);
    int lines = 0;
    char* line = g_logBuf;
    while (*line && lines < kMaxCopiedLogLines)
    {
        char* nl = strpbrk(line, "\r\n");
        if (nl)
            *nl = 0;
        if (*line)
        {
            Log("  [BF1MemExt] %s", line);
            ++lines;
        }
        if (!nl)
            break;
        line = nl + 1;
    }
    Log("  --- end of BF1MemExt.log (%d lines) ---", lines);
}

void LoadMemExt()
{
    char dir[MAX_PATH] = {};
    GameDir(dir, sizeof(dir));

    char path[MAX_PATH + 32];
    snprintf(path, sizeof(path), "%s\\BF1MemExt.dll", dir);

    if (GetModuleHandleA("BF1MemExt.dll"))
    {
        Log("BF1MemExt.dll is ALREADY loaded - something else loaded it (patched exe?)");
        return;
    }
    if (GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES)
    {
        Log("BF1MemExt.dll not found at %s - game runs without the limit patches", path);
        return;
    }

    Log("loading %s", path);
    const DWORD t0 = GetTickCount();
    const HMODULE h = LoadLibraryA(path);
    if (!h)
    {
        Log("ERROR: LoadLibrary failed for BF1MemExt.dll (error %lu)", GetLastError());
        return;
    }
    Log("BF1MemExt.dll loaded at 0x%08X in %lu ms - its patches are applied",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(h)),
        static_cast<unsigned long>(GetTickCount() - t0));
    CopyMemExtLog();
}

// The exe's entry point jumps here. Same stack as the real entry would get:
// [esp] = return into kernel32, [esp+4] = PEB pointer.
DWORD WINAPI EntryHook(LPVOID)
{
    if (!WriteCode(g_entry, g_savedEntry, kHookSize))
    {
        // Can't happen if the hook went in, but never run a half-restored entry.
        Log("FATAL: could not restore the entry point bytes (error %lu)", GetLastError());
        FatalAppExitA(0, "BF1GameExt: could not restore Battlefront's entry point. "
                         "Check BF1GameExt_split.log.");
    }

    Log("");
    Log("=== ENTRY POINT === original entry bytes restored, loading BF1MemExt");
    LoadMemExt();
    Log("=== ENTRY POINT === starting the game");

    // The VS2003 CRT entry takes no arguments and never returns (it exits the
    // process). If it ever did return, this function returns its value.
    return static_cast<DWORD>(reinterpret_cast<CrtEntry_t>(g_entry)());
}

} // namespace

bool MemExtInstallEntryHook()
{
    const BuildInfo* build = CurrentBuild();
    if (!build)
    {
        Log("entry hook: SKIPPED - unknown exe");
        return false;
    }

    if (!build->loadsMemExt)
    {
        char dll[MAX_PATH] = {};
        GameDir(dll, sizeof(dll));
        const size_t len = strlen(dll);
        snprintf(dll + len, sizeof(dll) - len, "\\BF1MemExt.dll");
        const bool present = GetFileAttributesA(dll) != INVALID_FILE_ATTRIBUTES;
        Log("entry hook: not needed - BF1MemExt only supports the 1.2 exe, so it is not loaded "
            "on %s%s",
            build->name,
            present ? " (BF1MemExt.dll is in the game folder; it is simply not used on this exe)"
                    : "");
        return false;
    }

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ExeBase());
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(ExeBase() + dos->e_lfanew);
    const uint32_t entryRva = nt->OptionalHeader.AddressOfEntryPoint;
    if (entryRva != build->entryRva)
    {
        Log("entry hook: SKIPPED - entry point is rva 0x%08X, clean exe has 0x%08X "
            "(patched or different exe)",
            entryRva, build->entryRva);
        return false;
    }

    const ByteSite entrySite = { "entry point", build->entryRva, build->entryHex };
    if (!CheckSite(entrySite, true))
    {
        Log("entry hook: SKIPPED - entry point bytes don't match the clean exe");
        return false;
    }
    g_entry = Rva(build->entryRva);

    memcpy(g_savedEntry, reinterpret_cast<const void*>(g_entry), kHookSize);

    unsigned char jmp[kHookSize] = { 0xE9, 0, 0, 0, 0 };
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&EntryHook) -
                                             (g_entry + kHookSize));
    memcpy(jmp + 1, &rel, sizeof(rel));

    if (!WriteCode(g_entry, jmp, kHookSize))
    {
        Log("entry hook: FAILED - VirtualProtect error %lu", GetLastError());
        return false;
    }

    Log("entry hook: installed at rva 0x%08X (byte-check OK)", build->entryRva);
    return true;
}

} // namespace bf1
