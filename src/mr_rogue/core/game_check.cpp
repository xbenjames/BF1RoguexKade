// BF1GameExt - game_check.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/ (was scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 22e: the exe's SHA-256 line also knows an exe with the 4 GB flag (BF1GameExt_4GB.bat: the flag and the header checksum,
// nothing else): it hashes it once more with the two fields as the original has them. Nothing else changed.
// Diagnostics for Battlefront.exe: GOG 1.2, GOG 1.3 or Steam (addresses per
// build come from game_build.cpp; on Steam they are found in memory by
// runtime_resolve.cpp). Read-only: no hooks, no patches, no writes.
//
// What it checks:
//   [1] exe identity (file hash)
//   [2] byte-check of every split screen site found by static analysis
//   [3] the split screen Lua callbacks, found by name
//   [4] loaded modules
//   [5] DLL files in the game folder
//   [6] controllers (DirectInput devices with their D-pad/POV count, XInput pads)
//   [7] split screen values right now, then a watcher that logs every change
//   [8] menu scripts (1.2 only): do the game's .lvl menu files call Lua
//       functions that only exist in the 2020 build? (background, [menus])

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <bcrypt.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "runtime_resolve.hpp"
#include "game_check.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "bcrypt.lib")
#endif

namespace bf1 {
namespace {

constexpr int kMaxWatchLines = 300;

// ---------------------------------------------------------------------------
// Image layout
// ---------------------------------------------------------------------------
struct Section
{
    bool found = false;
    char name[9] = {};
    uint32_t rva = 0;
    uint32_t vsize = 0;
    uint32_t rawsize = 0;
    uintptr_t start = 0;
};

struct Image
{
    uintptr_t base = 0;
    const IMAGE_NT_HEADERS32* nt = nullptr;
    Section text;
    Section rdata;
    Section data;
    bool hasBf1ext = false;      // BF1MemExt.exe's startup section
    char sectionList[128] = {};  // e.g. ".text .rdata .data .tls .rsrc"
};

Image g_img;
bool g_identityOk = false;
bool g_splitSitesOk = false;
DWORD g_attachTick = 0;

bool InSection(const Section& s, uintptr_t va)
{
    return s.found && va >= s.start && va < s.start + s.vsize;
}

bool ParseImage()
{
    const HMODULE exe = GetModuleHandleA(nullptr);
    if (!exe)
        return false;

    g_img.base = reinterpret_cast<uintptr_t>(exe);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(g_img.base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    g_img.nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(g_img.base + dos->e_lfanew);
    if (g_img.nt->Signature != IMAGE_NT_SIGNATURE)
        return false;

    const auto* sec = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
        reinterpret_cast<uintptr_t>(&g_img.nt->OptionalHeader) +
        g_img.nt->FileHeader.SizeOfOptionalHeader);

    for (WORD i = 0; i < g_img.nt->FileHeader.NumberOfSections; ++i)
    {
        char nm[9] = {};
        memcpy(nm, sec[i].Name, 8);

        const size_t used = strlen(g_img.sectionList);
        if (used + strlen(nm) + 2 < sizeof(g_img.sectionList))
            snprintf(g_img.sectionList + used, sizeof(g_img.sectionList) - used, "%s%s",
                     used ? " " : "", nm);
        if (strcmp(nm, ".bf1ext") == 0)
            g_img.hasBf1ext = true;

        Section* dst = nullptr;
        if (strcmp(nm, ".text") == 0)       dst = &g_img.text;
        else if (strcmp(nm, ".rdata") == 0) dst = &g_img.rdata;
        else if (strcmp(nm, ".data") == 0)  dst = &g_img.data;
        if (!dst)
            continue;

        dst->found = true;
        memcpy(dst->name, nm, sizeof(nm));
        dst->rva = sec[i].VirtualAddress;
        dst->vsize = sec[i].Misc.VirtualSize;
        dst->rawsize = sec[i].SizeOfRawData;
        dst->start = g_img.base + sec[i].VirtualAddress;
    }
    return g_img.text.found;
}

// ---------------------------------------------------------------------------
// Memory helpers
// ---------------------------------------------------------------------------
bool RangeReadable(uintptr_t start, size_t len)
{
    uintptr_t p = start;
    const uintptr_t end = start + len;
    while (p < end)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(p), &mbi, sizeof(mbi)))
            return false;
        if (mbi.State != MEM_COMMIT)
            return false;
        if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
            return false;
        const DWORD prot = mbi.Protect & 0xFF;
        if (prot == 0 || prot == PAGE_EXECUTE)
            return false;
        p = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    }
    return true;
}

size_t InitializedSize(const Section& s)
{
    return s.rawsize < s.vsize ? s.rawsize : s.vsize;
}

// Exact NUL-terminated string that is not the tail of a longer string.
uintptr_t FindCString(const Section& s, const char* str)
{
    if (!s.found || !RangeReadable(s.start, s.vsize))
        return 0;

    const size_t n = strlen(str) + 1;
    const auto* p = reinterpret_cast<const unsigned char*>(s.start);
    for (size_t i = 0; i + n <= s.vsize; ++i)
    {
        if (p[i] != static_cast<unsigned char>(str[0]))
            continue;
        if (memcmp(p + i, str, n) == 0 && (i == 0 || p[i - 1] == 0))
            return s.start + i;
    }
    return 0;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uint8_t ReadU8(uintptr_t va)
{
    return *reinterpret_cast<const volatile uint8_t*>(va);
}

// ---------------------------------------------------------------------------
// Small Windows helpers
// ---------------------------------------------------------------------------
void WideToUtf8(const wchar_t* w, char* out, int outSize)
{
    out[0] = 0;
    if (WideCharToMultiByte(CP_UTF8, 0, w, -1, out, outSize, nullptr, nullptr) <= 0)
        out[0] = 0;
}

void GameDirFromExe(const char* exePath, char* dirOut, size_t dirSize)
{
    const char* slash = strrchr(exePath, '\\');
    const int dirLen = slash ? static_cast<int>(slash - exePath) : 0;
    snprintf(dirOut, dirSize, "%.*s", dirLen, exePath);
}

// 22e: two header fields hashed as they were before BF1GameExt_4GB.bat changed them (file offsets; 0 = none)
struct HeaderUndo
{
    uint32_t charOfs;
    uint16_t charValue;
    uint32_t sumOfs;
    uint32_t sumValue;
};

bool Sha256File(const char* path, char hexOut[65], const HeaderUndo* undo = nullptr)
{
    hexOut[0] = 0;
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
        return false;

    bool ok = false;
    BCRYPT_ALG_HANDLE alg = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    const DWORD chunk = 1 << 20;
    unsigned char* buf = static_cast<unsigned char*>(HeapAlloc(GetProcessHeap(), 0, chunk));

    if (buf &&
        BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) >= 0 &&
        BCryptCreateHash(alg, &hash, nullptr, 0, nullptr, 0, 0) >= 0)
    {
        ok = true;
        bool undoDone = false;
        for (;;)
        {
            DWORD got = 0;
            if (!ReadFile(f, buf, chunk, &got, nullptr))
            {
                ok = false;
                break;
            }
            if (got == 0)
                break;
            if (undo && !undoDone) // 22e: the header is in the first chunk
            {
                undoDone = true;
                if (undo->charOfs && undo->charOfs + 2 <= got)
                    memcpy(buf + undo->charOfs, &undo->charValue, 2);
                if (undo->sumOfs && undo->sumOfs + 4 <= got)
                    memcpy(buf + undo->sumOfs, &undo->sumValue, 4);
            }
            if (BCryptHashData(hash, buf, got, 0) < 0)
            {
                ok = false;
                break;
            }
        }

        unsigned char digest[32] = {};
        if (ok && BCryptFinishHash(hash, digest, sizeof(digest), 0) >= 0)
        {
            for (int i = 0; i < 32; ++i)
                snprintf(hexOut + i * 2, 3, "%02x", digest[i]);
        }
        else
        {
            ok = false;
        }
    }

    if (hash)
        BCryptDestroyHash(hash);
    if (alg)
        BCryptCloseAlgorithmProvider(alg, 0);
    if (buf)
        HeapFree(GetProcessHeap(), 0, buf);
    CloseHandle(f);
    return ok;
}

// ---------------------------------------------------------------------------
// [2] Byte-checks
// ---------------------------------------------------------------------------
void RunByteChecks()
{
    const BuildInfo* build = CurrentBuild();
    int ok = 0;
    int bad = 0;
    for (size_t i = 0; i < build->splitSiteCount; ++i)
    {
        const ByteSite& site = build->splitSites[i];
        if (CheckSite(site, true))
        {
            ++ok;
            Log("  %-46s rva 0x%08X  OK", site.name, site.rva);
        }
        else
        {
            ++bad;
        }
    }
    Log("  byte-check: %d OK, %d mismatch", ok, bad);
    g_splitSitesOk = (bad == 0 && ok > 0 && ok == static_cast<int>(build->splitSiteCount) &&
                      build->localPlayerCountRva != 0 && !build->resolveAtRuntime);
}

// ---------------------------------------------------------------------------
// [3] Lua callbacks: { const char* name, lua_CFunction func } pairs in .data
// ---------------------------------------------------------------------------
// Address of the function-pointer half of a { name, func } slot, or 0.
uintptr_t FindLuaSlot(const char* name)
{
    const uintptr_t str = FindCString(g_img.rdata, name);
    if (!str)
        return 0;

    const Section& d = g_img.data;
    const size_t limit = InitializedSize(d);
    if (!d.found || !RangeReadable(d.start, limit))
        return 0;

    for (size_t i = 0; i + 8 <= limit; i += 4)
    {
        if (ReadU32(d.start + i) == static_cast<uint32_t>(str))
            return d.start + i + 4;
    }
    return 0;
}

void ScanLua()
{
    const BuildInfo* build = CurrentBuild();
    int ok = 0;
    int moved = 0;
    int missing = 0;
    for (size_t i = 0; i < build->luaSiteCount; ++i)
    {
        const LuaSite& w = build->luaSites[i];
        const uintptr_t slot = FindLuaSlot(w.name);
        const uintptr_t fn = slot ? ReadU32(slot) : 0;
        if (!fn)
        {
            ++missing;
            Log("  %-34s NOT FOUND", w.name);
        }
        else if (!InSection(g_img.text, fn))
        {
            ++ok;
            Log("  %-34s redirected to BF1GameExt (0x%08X) for logging - see [lua] lines",
                w.name, static_cast<unsigned>(fn));
        }
        else if (fn == Rva(w.funcRva))
        {
            ++ok;
            Log("  %-34s rva 0x%08X  OK", w.name, w.funcRva);
        }
        else
        {
            ++moved;
            Log("  %-34s rva 0x%08X  MOVED (static analysis said rva 0x%08X)", w.name,
                static_cast<unsigned>(fn - ExeBase()), w.funcRva);
        }
    }
    Log("  Lua callbacks: %d OK, %d moved, %d not found", ok, moved, missing);
}

// ---------------------------------------------------------------------------
// [4] [5] Modules and game folder
// ---------------------------------------------------------------------------
void ScanModules(const char* gameDir)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                           GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE)
    {
        Log("  module snapshot failed (error %lu)", GetLastError());
        return;
    }

    MODULEENTRY32W me;
    me.dwSize = sizeof(me);
    int n = 0;
    const size_t dirLen = strlen(gameDir);
    for (BOOL ok = Module32FirstW(snap, &me); ok; ok = Module32NextW(snap, &me))
    {
        char path[MAX_PATH * 3];
        WideToUtf8(me.szExePath, path, sizeof(path));
        const bool inGameDir = dirLen && _strnicmp(path, gameDir, dirLen) == 0;
        Log("  %08X %8lX  %s%s",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(me.modBaseAddr)),
            static_cast<unsigned long>(me.modBaseSize), path,
            inGameDir ? "   <-- game folder" : "");
        ++n;
    }
    CloseHandle(snap);
    Log("  %d modules loaded", n);
}

void ScanGameDirDlls(const char* gameDir)
{
    char pattern[MAX_PATH + 8];
    snprintf(pattern, sizeof(pattern), "%s\\*.dll", gameDir);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
    {
        Log("  no .dll files found in %s", gameDir);
        return;
    }
    do
    {
        Log("  %-28s %10lu bytes", fd.cFileName, static_cast<unsigned long>(fd.nFileSizeLow));
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

// ---------------------------------------------------------------------------
// [6] Controllers
// ---------------------------------------------------------------------------
struct XPadLite
{
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
};

struct XStateLite
{
    DWORD dwPacketNumber;
    XPadLite Gamepad;
};

typedef DWORD(WINAPI* XInputGetState_t)(DWORD, XStateLite*);
typedef HRESULT(WINAPI* DI8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

// IID_IDirectInput8A, defined here so no dxguid.lib is needed.
const GUID kIID_IDirectInput8A = {
    0xBF798030, 0x483A, 0x4DA2, { 0xAA, 0x99, 0x5D, 0x64, 0xED, 0x36, 0x97, 0x00 }
};

struct EnumCtx
{
    IDirectInput8A* di;
    int count;
};

BOOL CALLBACK EnumGameControllers(LPCDIDEVICEINSTANCEA inst, LPVOID p)
{
    EnumCtx* ctx = static_cast<EnumCtx*>(p);
    ++ctx->count;
    Log("  DirectInput device %d: \"%s\" (product \"%s\") VID_%04X PID_%04X", ctx->count,
        inst->tszInstanceName, inst->tszProductName,
        static_cast<unsigned>(LOWORD(inst->guidProduct.Data1)),
        static_cast<unsigned>(HIWORD(inst->guidProduct.Data1)));

    IDirectInputDevice8A* dev = nullptr;
    if (SUCCEEDED(ctx->di->CreateDevice(inst->guidInstance, &dev, nullptr)) && dev)
    {
        DIDEVCAPS caps;
        memset(&caps, 0, sizeof(caps));
        caps.dwSize = sizeof(caps);
        if (SUCCEEDED(dev->GetCapabilities(&caps)))
            Log("    axes %lu, buttons %lu, POV hats (D-pad) %lu",
                static_cast<unsigned long>(caps.dwAxes),
                static_cast<unsigned long>(caps.dwButtons),
                static_cast<unsigned long>(caps.dwPOVs));
        dev->Release();
    }
    return DIENUM_CONTINUE;
}

void ScanControllers(void* realDI8Create)
{
    // DirectInput: what the 1.2 game itself can see.
    if (!realDI8Create)
    {
        Log("  DirectInput enumeration skipped (real dinput8 not available)");
    }
    else
    {
        const auto create = reinterpret_cast<DI8Create_t>(realDI8Create);
        IDirectInput8A* di = nullptr;
        const HRESULT hr = create(GetModuleHandleA(nullptr), DIRECTINPUT_VERSION,
                                  kIID_IDirectInput8A, reinterpret_cast<LPVOID*>(&di), nullptr);
        if (FAILED(hr) || !di)
        {
            Log("  DirectInput8Create for enumeration failed: 0x%08lX",
                static_cast<unsigned long>(hr));
        }
        else
        {
            EnumCtx ctx = { di, 0 };
            di->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllers, &ctx, DIEDFL_ATTACHEDONLY);
            Log("  DirectInput game controllers attached: %d", ctx.count);
            di->Release();
        }
    }

    // XInput: GOG 1.3 reads pads through it; GOG 1.2 only uses DirectInput.
    const char* const xinputDlls[] = { "xinput1_4.dll", "xinput9_1_0.dll", "xinput1_3.dll" };
    HMODULE xi = nullptr;
    const char* xiName = nullptr;
    for (const char* name : xinputDlls)
    {
        xi = LoadLibraryA(name);
        if (xi)
        {
            xiName = name;
            break;
        }
    }
    if (!xi)
    {
        Log("  no XInput DLL found on this system");
        return;
    }

    const auto getState = reinterpret_cast<XInputGetState_t>(
        reinterpret_cast<void*>(GetProcAddress(xi, "XInputGetState")));
    if (!getState)
    {
        Log("  %s has no XInputGetState", xiName);
        return;
    }

    int connected = 0;
    for (DWORD pad = 0; pad < 4; ++pad)
    {
        XStateLite st = {};
        if (getState(pad, &st) == ERROR_SUCCESS)
        {
            ++connected;
            Log("  XInput pad %lu: CONNECTED", static_cast<unsigned long>(pad));
        }
        else
        {
            Log("  XInput pad %lu: not connected", static_cast<unsigned long>(pad));
        }
    }
    Log("  XInput pads connected: %d (via %s)", connected, xiName);
}

// ---------------------------------------------------------------------------
// [7] Split screen values + watcher
// ---------------------------------------------------------------------------
struct SplitValues
{
    uint32_t players;
    uint8_t latch;
    uint8_t wasSplit;
};

SplitValues ReadSplitValues()
{
    SplitValues v;
    const BuildInfo* build = CurrentBuild();
    v.players = ReadU32(Rva(build->localPlayerCountRva));
    v.latch = ReadU8(Rva(build->bootSplitFlagRva));
    v.wasSplit = ReadU8(Rva(build->wasSplitRva));
    return v;
}

void LogSplitValues(const char* prefix, const SplitValues& v)
{
    Log("%s local players %lu | boot split flag %u | was split screen %u", prefix,
        static_cast<unsigned long>(v.players), static_cast<unsigned>(v.latch),
        static_cast<unsigned>(v.wasSplit));
}

DWORD WINAPI WatchThread(LPVOID)
{
    SplitValues last = ReadSplitValues();
    int lines = 0;
    for (;;)
    {
        Sleep(100);
        const SplitValues now = ReadSplitValues();
        if (now.players == last.players && now.latch == last.latch &&
            now.wasSplit == last.wasSplit)
            continue;

        last = now;
        if (lines < kMaxWatchLines)
        {
            char prefix[64];
            snprintf(prefix, sizeof(prefix), "[watch] %lu ms:",
                     static_cast<unsigned long>(GetTickCount() - g_attachTick));
            LogSplitValues(prefix, now);
        }
        else if (lines == kMaxWatchLines)
        {
            Log("[watch] line limit reached, watcher going quiet");
        }
        ++lines;
    }
}

// ---------------------------------------------------------------------------
// [8] Menu scripts
// These Lua functions exist in the 2020 Steam/GOG exe but not in the 1.2 exe.
// A menu .lvl that calls them came from the 2020 build, and the screens that
// call them will stop working on the 1.2 exe.
// ---------------------------------------------------------------------------
const char* const kNewerLuaFunctions[] = {
    "ScriptCB_GetCurrentAspect",  "ScriptCB_GetLoginName",
    "ScriptCB_GetStringWidth",    "ScriptCB_GetTargetAspect",
    "ScriptCB_IsLastInputFromJoystick", "ScriptCB_IsLoggedIn",
    "ScriptCB_LastSignInError",   "ScriptCB_SaveCalibration",
    "ScriptCB_SearchStatsRank",   "ScriptCB_UpdateCalibration",
};
constexpr size_t kNewerCount = sizeof(kNewerLuaFunctions) / sizeof(kNewerLuaFunctions[0]);
constexpr LONGLONG kMaxMenuFileSize = 512LL * 1024 * 1024;
constexpr int kMaxMenuDepth = 4;

char g_menuRoot[MAX_PATH + 32] = {};
int g_menuFilesChecked = 0;
int g_menuFilesNewer = 0;

bool IsIdentChar(unsigned char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
           c == '_';
}

bool IsMenuLvl(const char* name)
{
    if (_strnicmp(name, "shell", 5) != 0 && _strnicmp(name, "common", 6) != 0)
        return false;
    const size_t n = strlen(name);
    return n > 4 && _stricmp(name + n - 4, ".lvl") == 0;
}

void FindNewerFunctions(const unsigned char* data, size_t size, bool (&found)[kNewerCount])
{
    static const char kPrefix[] = "ScriptCB_";
    const size_t prefixLen = sizeof(kPrefix) - 1;
    const unsigned char* p = data;
    const unsigned char* const end = data + size;

    while (p < end)
    {
        p = static_cast<const unsigned char*>(memchr(p, 'S', static_cast<size_t>(end - p)));
        if (!p)
            break;
        const size_t left = static_cast<size_t>(end - p);
        if (left >= prefixLen && memcmp(p, kPrefix, prefixLen) == 0)
        {
            for (size_t k = 0; k < kNewerCount; ++k)
            {
                const size_t n = strlen(kNewerLuaFunctions[k]);
                if (left >= n && memcmp(p, kNewerLuaFunctions[k], n) == 0 &&
                    (left == n || !IsIdentChar(p[n])))
                    found[k] = true;
            }
            p += prefixLen;
        }
        else
        {
            ++p;
        }
    }
}

void CheckMenuFile(const char* path, const char* shownName)
{
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE)
    {
        Log("[menus] %s: could not open (error %lu)", shownName, GetLastError());
        return;
    }

    LARGE_INTEGER size;
    size.QuadPart = 0;
    GetFileSizeEx(f, &size);

    FILETIME ft = {};
    SYSTEMTIME st = {};
    if (GetFileTime(f, nullptr, nullptr, &ft))
        FileTimeToSystemTime(&ft, &st);

    if (size.QuadPart <= 0 || size.QuadPart > kMaxMenuFileSize)
    {
        Log("[menus] %s: skipped (size %lld bytes)", shownName,
            static_cast<long long>(size.QuadPart));
        CloseHandle(f);
        return;
    }

    bool found[kNewerCount] = {};
    HANDLE map = CreateFileMappingA(f, nullptr, PAGE_READONLY, 0, 0, nullptr);
    const void* view = map ? MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0) : nullptr;
    if (view)
    {
        FindNewerFunctions(static_cast<const unsigned char*>(view),
                           static_cast<size_t>(size.QuadPart), found);
        UnmapViewOfFile(view);
    }
    if (map)
        CloseHandle(map);
    CloseHandle(f);

    if (!view)
    {
        Log("[menus] %s: could not read (error %lu)", shownName, GetLastError());
        return;
    }

    ++g_menuFilesChecked;
    char list[512] = {};
    int count = 0;
    for (size_t k = 0; k < kNewerCount; ++k)
    {
        if (!found[k])
            continue;
        const size_t used = strlen(list);
        // Print names without the "ScriptCB_" prefix to keep the line short.
        snprintf(list + used, sizeof(list) - used, "%s%s", count ? ", " : "",
                 kNewerLuaFunctions[k] + 9);
        ++count;
    }
    if (count)
        ++g_menuFilesNewer;

    Log("[menus] %s (%lld bytes, modified %04u-%02u-%02u): %s%s", shownName,
        static_cast<long long>(size.QuadPart), static_cast<unsigned>(st.wYear),
        static_cast<unsigned>(st.wMonth), static_cast<unsigned>(st.wDay),
        count ? "calls 2020-only functions: " : "no 2020-only functions", list);
}

void WalkMenuDir(const char* dir, const char* rel, int depth)
{
    if (depth > kMaxMenuDepth)
        return;

    char pattern[MAX_PATH * 2];
    snprintf(pattern, sizeof(pattern), "%s\\*", dir);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
        return;
    do
    {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;

        char full[MAX_PATH * 2];
        snprintf(full, sizeof(full), "%s\\%s", dir, fd.cFileName);
        char sub[MAX_PATH * 2];
        snprintf(sub, sizeof(sub), "%s%s%s", rel, *rel ? "\\" : "", fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            WalkMenuDir(full, sub, depth + 1);
        else if (IsMenuLvl(fd.cFileName))
            CheckMenuFile(full, sub);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

DWORD WINAPI MenuScriptThread(LPVOID)
{
    if (GetFileAttributesA(g_menuRoot) == INVALID_FILE_ATTRIBUTES)
    {
        Log("[menus] %s not found - menu script check skipped", g_menuRoot);
        return 0;
    }

    Log("[menus] checking shell*.lvl and common*.lvl under %s", g_menuRoot);
    WalkMenuDir(g_menuRoot, "", 0);

    Log("[menus] checked %d file(s); %d call functions the 1.2 exe doesn't have",
        g_menuFilesChecked, g_menuFilesNewer);
    if (g_menuFilesNewer)
        Log("[menus] RESULT: these menu files are from the 2020 build. On the 1.2 exe, "
            "screens that call those functions (controls, profile/login) will break.");
    else if (g_menuFilesChecked)
        Log("[menus] RESULT: menu files match the 1.2 exe");
    return 0;
}

} // namespace

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------
void ScanOnAttach()
{
    g_attachTick = GetTickCount();

    char exePath[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH))
        exePath[0] = 0;
    Log("exe: %s", exePath);

    if (!ParseImage())
    {
        Log("ERROR: could not read the exe's PE headers in memory");
        return;
    }

    const IMAGE_FILE_HEADER& fh = g_img.nt->FileHeader;
    const IMAGE_OPTIONAL_HEADER32& oh = g_img.nt->OptionalHeader;
    Log("module base 0x%08X, PE timestamp 0x%08lX, SizeOfImage 0x%08lX, linker %u.%u",
        static_cast<unsigned>(g_img.base), static_cast<unsigned long>(fh.TimeDateStamp),
        static_cast<unsigned long>(oh.SizeOfImage), static_cast<unsigned>(oh.MajorLinkerVersion),
        static_cast<unsigned>(oh.MinorLinkerVersion));

    const BuildInfo* build = CurrentBuild();
    g_identityOk = build != nullptr;
    if (g_identityOk)
        Log("IDENTITY: %s (load address 0x%08X, %s)", build->name,
            static_cast<unsigned>(g_img.base),
            LoadDelta() == 0 ? "preferred address" : "moved by ASLR - addresses use base + RVA");
    else
        Log("IDENTITY: UNKNOWN exe - not GOG 1.2 or GOG 1.3. Everything that touches the game "
            "stays off. Send this log anyway.");

    Log("sections: %s", g_img.sectionList);
    if (build && build->codeLockedOnDisk)
        Log("Steam DRM wrapper present: the game code stays locked until Steam unlocks it in "
            "memory. BF1GameExt waits for that and never removes or bypasses it.");
    if (g_img.hasBf1ext)
        Log("WARNING: this exe was patched by BF1MemExt.exe (.bf1ext section). "
            "Restore a clean exe - BF1GameExt loads BF1MemExt itself on 1.2.");
}

bool ScanExePatchedByMemExt()
{
    return g_img.hasBf1ext;
}

bool ScanSplitSitesOk()
{
    return g_identityOk && g_splitSitesOk;
}

bool ScanIdentityOk()
{
    return g_identityOk;
}

unsigned long ScanMsSinceAttach()
{
    return static_cast<unsigned long>(GetTickCount() - g_attachTick);
}

uintptr_t ScanLuaFuncSlot(const char* name)
{
    return g_img.text.found ? FindLuaSlot(name) : 0;
}

void ScanRun(const char* trigger, void* realDI8Create)
{
    Log("");
    Log("=== SCAN START === trigger: %s | %lu ms after DLL attach", trigger,
        static_cast<unsigned long>(GetTickCount() - g_attachTick));

    if (!g_img.text.found)
    {
        Log("ERROR: no .text section info - scan aborted");
        return;
    }

    char exePath[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exePath, MAX_PATH))
        exePath[0] = 0;
    char gameDir[MAX_PATH] = {};
    GameDirFromExe(exePath, gameDir, sizeof(gameDir));

    Log("[1] Exe file hash");
    char sha[65];
    const BuildInfo* build = CurrentBuild();
    if (Sha256File(exePath, sha))
    {
        bool match = build && strcmp(sha, build->sha256) == 0;
        bool flagOnly = false;
        // 22e: an exe with the 4 GB flag (BF1GameExt_4GB.bat): the same file apart from the flag and the header checksum?
        const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ExeBase());
        const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(ExeBase() + static_cast<uintptr_t>(dos->e_lfanew));
        if (!match && build && (nt->FileHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE))
        {
            // the originals' header checksums (GOG 1.3, Steam 1.3)
            const uint32_t stamp = nt->FileHeader.TimeDateStamp;
            const uint32_t sum = stamp == 0x5F24321Du ? 0x004A21B4u : (stamp == 0x5F243336u ? 0x00509958u : 0u);
            HeaderUndo undo;
            undo.charOfs = static_cast<uint32_t>(dos->e_lfanew) + 4 + 18;
            undo.charValue = static_cast<uint16_t>(nt->FileHeader.Characteristics & ~IMAGE_FILE_LARGE_ADDRESS_AWARE);
            undo.sumOfs = sum ? static_cast<uint32_t>(dos->e_lfanew) + 4 + 20 + 64 : 0;
            undo.sumValue = sum;
            char sha2[65];
            flagOnly = sum && Sha256File(exePath, sha2, &undo) && strcmp(sha2, build->sha256) == 0;
        }
        Log("  SHA-256 %s  %s", sha,
            match ? "MATCH (same file Claude analyzed)"
                  : (flagOnly ? "MATCH apart from the 4 GB flag (BF1GameExt_4GB.bat: the flag and the header checksum, nothing else)"
                              : "DIFFERENT from the file Claude analyzed"));
    }
    else
        Log("  SHA-256 failed");

    if (build && build->resolveAtRuntime)
    {
        Log("[R] Finding the split screen and controls sites in memory (code is locked on disk)");
        ResolveRuntimeBuild();
        build = CurrentBuild();
    }

    Log("[2] Byte-check of split screen sites");
    if (build && build->splitSiteCount)
        RunByteChecks();
    else
        Log("  skipped (%s)", build ? "split screen sites not found" : "unknown exe");

    Log("[3] Split screen Lua callbacks (found by name)");
    if (build)
        ScanLua();
    else
        Log("  skipped (unknown exe)");

    Log("[4] Loaded modules");
    ScanModules(gameDir);

    Log("[5] DLL files in %s", gameDir);
    ScanGameDirDlls(gameDir);

    Log("[6] Controllers");
    ScanControllers(realDI8Create);

    Log("[7] Split screen values");
    if (g_identityOk && build && build->localPlayerCountRva)
    {
        LogSplitValues("  now:", ReadSplitValues());
        const HANDLE t = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
        if (t)
        {
            CloseHandle(t);
            Log("  watcher started: every change from here on is logged as [watch]");
        }
        else
        {
            Log("  watcher thread failed to start (error %lu)", GetLastError());
        }
    }
    else
    {
        Log("  skipped (%s)", g_identityOk ? "split screen values not found" : "unknown exe");
    }

    if (build && build->checkMenuScripts)
    {
        Log("[8] Menu scripts (runs in the background, lines start with [menus])");
        snprintf(g_menuRoot, sizeof(g_menuRoot), "%s\\Data\\_LVL_PC", gameDir);
        const HANDLE mt = CreateThread(nullptr, 0, MenuScriptThread, nullptr, 0, nullptr);
        if (mt)
            CloseHandle(mt);
        else
            Log("  menu script thread failed to start (error %lu)", GetLastError());
    }
    else
    {
        Log("[8] Menu scripts: not needed on this build");
    }

    Log("=== SCAN DONE === the scan itself installs no hooks and patches nothing");
}

} // namespace bf1
