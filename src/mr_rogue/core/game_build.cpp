// BF1GameExt - game_build.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Per-build addresses for Battlefront.exe, all as RVAs, from static analysis.
//
//   GOG 1.2 (2005-01-12): fixed base 0x00400000, no relocations, loads BF1MemExt
//   GOG 1.3 (2020-07-31): ASLR, relocations, built-in XInput; BF1MemExt does
//                         not support it (it would close the game), so it is
//                         never loaded there
//   Steam  (2020-07-31): same source as GOG 1.3, built 8 minutes later with the
//                         Steam SDK and Steam's DRM wrapper. Lua table offsets
//                         are read from the (unlocked) data section; everything
//                         in code is found in memory after Steam unlocks it.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <cstdint>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"

namespace bf1 {
namespace {

constexpr uintptr_t kPreferredBase = 0x00400000;

// ---------------------------------------------------------------------------
// GOG 1.2 (2005)
// ---------------------------------------------------------------------------
const ByteSite kSplit12[] = {
    { "ScriptCB_SetSplitscreen", 0x000A3350,
      "8B 44 24 04 6A 01 50 E8 F4 60 01 00 33 C9 83 C4 08 84 C0 0F 95 C1 33 C0 41 89 0D 68 16 71 00 C3" },
    { "ScriptCB_IsSplitscreen", 0x000A3370,
      "8B 15 68 16 71 00 8B 4C 24 04 83 FA 01 0F 9F C0 50 51 E8 99 60 01 00 83 C4 08 B8 01 00 00 00 C3" },
    { "GetLocalPlayerCount", 0x001BC980, "A1 68 16 71 00 C3" },
    { "IsLocalPlayerIndex", 0x001BC990, "8B 44 24 04 3B 05 68 16 71 00" },
    { "startup: set boot-split flag", 0x001C401C, "39 1D 68 16 71 00 0F 9F C0 A2 DC 34 76 00" },
    { "force 2 if boot-split (helper)", 0x001D7D00,
      "A0 DC 34 76 00 84 C0 74 0A C7 05 68 16 71 00 02 00 00 00" },
    { "force 2 if boot-split (launch)", 0x001DB305,
      "38 1D DC 34 76 00 74 0C C7 05 68 16 71 00 02 00 00 00 EB 09" },
    { "back to menus: save was-split", 0x001ED3C0, "8B 0D 68 16 71 00" },
    { "back to menus: reset to 1 player", 0x001ED3FD, "C7 05 68 16 71 00 01 00 00 00" },
};

const LuaSite kLua12[] = {
    { "ScriptCB_SetSplitscreen", 0x000A3350 },
    { "ScriptCB_IsSplitscreen", 0x000A3370 },
    { "ScriptCB_WasSplitscreen", 0x000A3390 },
    { "ScriptCB_SetHorizontalSplitScreen", 0x000A3BB0 },
    { "ScriptCB_IsHorizontalSplitScreen", 0x000B5D60 },
    { "ScriptCB_GetPausingViewport", 0x000A3750 },
    { "ScriptCB_GetNumControllers", 0x000A31F0 },
    { "ScriptCB_SetJoystickEnabled", 0x000A43E0 },
};

const ByteSite kControls12[] = {
    { "GetCurrentProfile (index, array, 0x458 stride)", 0x001F6C40,
      "A1 D4 C6 7F 00 85 C0 7C 11 83 F8 02 7D 0C 69 C0 58 04 00 00 05 D8 C6 7F 00 C3 33 C0 C3" },
    { "binding tables (3 x 43, +0x0C/+0xB8/+0x164)", 0x001F6474,
      "8D 73 0C 8D B8 48 06 00 00 B9 2B 00 00 00 F3 A5 8D B3 B8 00 00 00 8D B8 F4 06 00 00 B9 "
      "2B 00 00 00 F3 A5 8D B3 64 01 00 00 8D B8 A0 07 00 00 B9 2B 00 00 00 F3 A5" },
    { "SetCurrentProfile (local slots)", 0x001F7107,
      "85 ED 89 2D D4 C6 7F 00 7D 04 B0 01 5D C3 56 8B F5 69 F6 58 04 00 00 57 81 C6 D8 C6 7F "
      "00 BF 08 34 6F 00" },
    { "SetJoystickEnabled (profile + 0x370)", 0x000A4419,
      "38 9E 70 03 00 00 74 14 8B CE 88 9E 70 03 00 00" },
};

// ---------------------------------------------------------------------------
// GOG 1.3 (2020)
// ---------------------------------------------------------------------------
const ByteSite kSplit13[] = {
    { "ScriptCB_SetSplitscreen", 0x00168B00,
      "55 8B EC 6A 01 FF 75 08 E8 CD 54 EA FF 33 C9 84 C0 0F 95 C1 41 51 E8 EC 26 EB FF 83 C4 0C "
      "33 C0 5D C3" },
    { "ScriptCB_IsSplitscreen", 0x00168B30,
      "55 8B EC E8 11 13 EA FF 83 F8 01 0F 9F C0 0F B6 C0 50 FF 75 08 E8 5A 4E EA FF 83 C4 08 B8 "
      "01 00 00 00 5D C3" },
    { "GetNumLocalPlayers", 0x0012BBB0, "A1 FC 82 83 00 C3" },
    { "SetNumLocalPlayers", 0x0016FEC0, "55 8B EC 8B 45 08 A3 FC 82 83 00 5D C3" },
    { "WasSplitscreen getter", 0x0016F860, "A0 43 BE 9E 00 C3" },
    { "boot-split: force 2 players", 0x001BA9A0, "80 3D 73 4B 8C 00 00 74 0A 6A 02 E8 57 08 E6 FF" },
    { "back to menus: save was-split, set 1 player", 0x00208CB9,
      "E8 8B 11 E0 FF 83 F8 01 0F 9F 05 43 BE 9E 00 E8 B3 70 E0 FF 6A 01 E8 33 25 E1 FF" },
};

const LuaSite kLua13[] = {
    { "ScriptCB_SetSplitscreen", 0x00168B00 },
    { "ScriptCB_IsSplitscreen", 0x00168B30 },
    { "ScriptCB_WasSplitscreen", 0x00168B60 },
    { "ScriptCB_SetHorizontalSplitScreen", 0x0016CAA0 },
    { "ScriptCB_IsHorizontalSplitScreen", 0x0016CA80 },
    { "ScriptCB_GetPausingViewport", 0x0016C270 },
    { "ScriptCB_GetNumControllers", 0x00168920 },
    { "ScriptCB_SetJoystickEnabled", 0x0016D6E0 },
};

// ---------------------------------------------------------------------------
// Steam (2020) - Lua function offsets from the data section, checked against
// the Steam exe on disk. Code sites are found at runtime.
// ---------------------------------------------------------------------------
const LuaSite kLuaSteam[] = {
    { "ScriptCB_SetSplitscreen", 0x0016E870 },
    { "ScriptCB_IsSplitscreen", 0x0016E8A0 },
    { "ScriptCB_WasSplitscreen", 0x0016E8D0 },
    { "ScriptCB_SetHorizontalSplitScreen", 0x00172810 },
    { "ScriptCB_IsHorizontalSplitScreen", 0x001727F0 },
    { "ScriptCB_GetPausingViewport", 0x00171FE0 },
    { "ScriptCB_GetNumControllers", 0x0016E690 },
    { "ScriptCB_SetJoystickEnabled", 0x00173450 },
};

const ByteSite kControls13[] = {
    { "GetCurrentProfile (index, array, 0x458 stride)", 0x001F3F10,
      "A1 58 99 96 00 83 F8 01 77 0C 69 C0 58 04 00 00 05 A0 90 96 00 C3" },
    { "local player slots loop (0x30 stride, +0x10 = profile)", 0x001F3BE7,
      "BE E8 8F 96 00 0F 1F 40 00 39 7E 10 75 0A FF 36 8D 4F 28 E8 C2 B2 E1 FF 83 C6 30 81 FE 48 "
      "90 96 00 7C E6" },
    { "binding tables (3 x 0xAC from profile + 0x34)", 0x001F3A1D,
      "8D 7B 0C 89 45 F8 33 F6 8B D8 56 57 8B CB E8 0F 7C E2 FF 46 81 C7 AC 00 00 00 83 FE 03 7C "
      "EB" },
    { "SetJoystickEnabled (profile + 0x370)", 0x0016D71B,
      "38 9E 70 03 00 00 74 16 8B CE 88 9E 70 03 00 00" },
};

template <typename T, size_t N>
constexpr size_t Count(const T (&)[N])
{
    return N;
}

const BuildInfo kBuilds[] = {
    {
        "GOG 1.2 (2005-01-12)",
        0x41E48CFC,
        0x01B42000,
        "d32e19d4ab6c47d9f235a02703d2bf1324690b961771eaa9595ea3cb223a147e",
        0x0026A1CC,
        "6A 60 68 F0 8D 6B 00 E8 90 35 00 00 BF 94 00 00",
        true,
        0x00311668,
        0x003C3B60,
        0x003634DC,
        kSplit12,
        Count(kSplit12),
        kLua12,
        Count(kLua12),
        0x003FC6D4,
        0x003FC6D8,
        0x002F3408,
        kControls12,
        Count(kControls12),
        true,
        false,
        false,
    },
    {
        "GOG 1.3 (2020-07-31)",
        0x5F24321D,
        0x021BB000,
        "f5d7b3c2fd4ae6602180d20630869a89c415f3100228ee250cf8493f80826596",
        0x00011784,
        "E9 7F D5 32 00 E9 A2 94 20 00 E9 2D DA 0E 00",
        false,
        0x004382FC,
        0x005EBE43,
        0x004C4B73,
        kSplit13,
        Count(kSplit13),
        kLua13,
        Count(kLua13),
        0x00569958,
        0x005690A0,
        0x00568FE8,
        kControls13,
        Count(kControls13),
        false,
        false,
        false,
    },
    {
        "Steam 1.3 (2020-07-31)",
        0x5F243336,
        0x02224000,
        "deca98fb348310e5e3b6dd74cbffb026a612b0410442c7787d4589b9d845b772",
        0,       // entry point is Steam's unlock stub: never hooked
        "",
        false,   // BF1MemExt: 1.2 only
        0,       // local player count: found at runtime
        0,       // was split screen: found at runtime
        0,       // boot-split flag: found at runtime
        nullptr, // split screen sites: found at runtime
        0,
        kLuaSteam,
        Count(kLuaSteam),
        0,       // profile index: found at runtime
        0,       // profile array: found at runtime
        0,       // local player slots: found at runtime
        nullptr, // controls sites: found at runtime
        0,
        false,
        true,    // code locked on disk
        true,    // resolve at runtime
    },
};

const BuildInfo* g_build = nullptr;
uintptr_t g_base = 0;
constexpr int kMaxPatternBytes = 128;
constexpr size_t kEntropySample = 256 * 1024;
constexpr double kLockedEntropy = 7.6; // locked code ~8.0, normal x86 code ~6

const IMAGE_NT_HEADERS32* NtHeaders()
{
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(g_base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return nullptr;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(g_base + dos->e_lfanew);
    return nt->Signature == IMAGE_NT_SIGNATURE ? nt : nullptr;
}

bool Readable(uintptr_t start, size_t len)
{
    uintptr_t p = start;
    const uintptr_t end = start + len;
    while (p < end)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(p), &mbi, sizeof(mbi)))
            return false;
        if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
            return false;
        const DWORD prot = mbi.Protect & 0xFF;
        if (prot == 0 || prot == PAGE_EXECUTE)
            return false;
        p = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    }
    return true;
}

int HexNibble(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// "8B 44 ?? 04" -> bytes + mask (mask 0 = any byte). Returns the length or -1.
int ParseHex(const char* hex, unsigned char* out, unsigned char* mask, int max)
{
    int n = 0;
    while (*hex)
    {
        if (*hex == ' ')
        {
            ++hex;
            continue;
        }
        if (n >= max)
            return -1;
        if (hex[0] == '?' && hex[1] == '?')
        {
            out[n] = 0;
            mask[n++] = 0;
            hex += 2;
            continue;
        }
        const int hi = HexNibble(hex[0]);
        const int lo = hex[1] ? HexNibble(hex[1]) : -1;
        if (hi < 0 || lo < 0)
            return -1;
        out[n] = static_cast<unsigned char>((hi << 4) | lo);
        mask[n++] = 1;
        hex += 2;
    }
    return n;
}

bool MaskedEqual(const unsigned char* mem, const unsigned char* want, const unsigned char* mask,
                 int n)
{
    for (int i = 0; i < n; ++i)
        if (mask[i] && mem[i] != want[i])
            return false;
    return true;
}

const IMAGE_SECTION_HEADER* FindSection(const char* name)
{
    const IMAGE_NT_HEADERS32* nt = NtHeaders();
    if (!nt)
        return nullptr;
    const auto* sec = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
        reinterpret_cast<uintptr_t>(&nt->OptionalHeader) + nt->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        char nm[9] = {};
        memcpy(nm, sec[i].Name, 8);
        if (strcmp(nm, name) == 0)
            return &sec[i];
    }
    return nullptr;
}

// Shifts every relocated DWORD inside [rva, rva + n) of 'buf' by LoadDelta().
// Returns false if a relocated DWORD only partly overlaps the range.
bool ApplyRelocations(uint32_t rva, unsigned char* buf, int n)
{
    const intptr_t delta = LoadDelta();
    const IMAGE_NT_HEADERS32* nt = NtHeaders();
    if (!nt)
        return false;
    const IMAGE_DATA_DIRECTORY& dir =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
    if (!dir.VirtualAddress || !dir.Size)
        return true; // no relocations (GOG 1.2)

    const uintptr_t start = g_base + dir.VirtualAddress;
    const uintptr_t end = start + dir.Size;
    const uint32_t lo = rva;
    const uint32_t hi = rva + static_cast<uint32_t>(n);

    uintptr_t p = start;
    while (p + sizeof(IMAGE_BASE_RELOCATION) <= end)
    {
        const auto* block = reinterpret_cast<const IMAGE_BASE_RELOCATION*>(p);
        if (block->SizeOfBlock < sizeof(IMAGE_BASE_RELOCATION))
            break;
        const uint32_t page = block->VirtualAddress;
        // A page's relocations can only touch [page, page + 0x1003).
        if (page + 0x1003 > lo && page < hi)
        {
            const auto* entries = reinterpret_cast<const WORD*>(p + sizeof(IMAGE_BASE_RELOCATION));
            const size_t count = (block->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            for (size_t i = 0; i < count; ++i)
            {
                if ((entries[i] >> 12) != IMAGE_REL_BASED_HIGHLOW)
                    continue;
                const uint32_t r = page + (entries[i] & 0x0FFF);
                if (r + 4 <= lo || r >= hi)
                    continue;
                if (r < lo || r + 4 > hi)
                    return false; // pattern cuts a relocated address in half
                uint32_t v = 0;
                memcpy(&v, buf + (r - lo), sizeof(v));
                v = static_cast<uint32_t>(v + static_cast<uint32_t>(delta));
                memcpy(buf + (r - lo), &v, sizeof(v));
            }
        }
        p += block->SizeOfBlock;
    }
    return true;
}

} // namespace

const BuildInfo* DetectBuild()
{
    g_base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
    g_build = nullptr;
    const IMAGE_NT_HEADERS32* nt = NtHeaders();
    if (!nt)
        return nullptr;
    for (const BuildInfo& b : kBuilds)
    {
        if (nt->FileHeader.TimeDateStamp == b.timeDateStamp &&
            nt->OptionalHeader.SizeOfImage == b.sizeOfImage)
        {
            g_build = &b;
            break;
        }
    }
    return g_build;
}

const BuildInfo* CurrentBuild()
{
    return g_build;
}

uintptr_t ExeBase()
{
    return g_base;
}

intptr_t LoadDelta()
{
    return static_cast<intptr_t>(g_base) - static_cast<intptr_t>(kPreferredBase);
}

uintptr_t Rva(uint32_t rva)
{
    return g_base + rva;
}

bool CheckSite(const ByteSite& site, bool logMismatch)
{
    unsigned char want[kMaxPatternBytes];
    unsigned char mask[kMaxPatternBytes];
    const int n = ParseHex(site.hex, want, mask, kMaxPatternBytes);
    if (n <= 0)
    {
        if (logMismatch)
            Log("  %-46s rva 0x%08X  BAD PATTERN IN DLL", site.name, site.rva);
        return false;
    }
    if (!ApplyRelocations(site.rva, want, n))
    {
        if (logMismatch)
            Log("  %-46s rva 0x%08X  PATTERN CUTS A RELOCATED ADDRESS", site.name, site.rva);
        return false;
    }
    const uintptr_t at = Rva(site.rva);
    if (!Readable(at, static_cast<size_t>(n)))
    {
        if (logMismatch)
            Log("  %-46s rva 0x%08X  NOT READABLE", site.name, site.rva);
        return false;
    }
    if (MaskedEqual(reinterpret_cast<const unsigned char*>(at), want, mask, n))
        return true;

    if (logMismatch)
    {
        Log("  %-46s rva 0x%08X  MISMATCH - expected / actual:", site.name, site.rva);
        LogBytes("    ", site.rva, want, static_cast<size_t>(n));
        LogBytes("    ", site.rva, reinterpret_cast<const void*>(at), static_cast<size_t>(n));
    }
    return false;
}

uint32_t LuaFuncRva(const char* name)
{
    if (!g_build)
        return 0;
    for (size_t i = 0; i < g_build->luaSiteCount; ++i)
        if (strcmp(g_build->luaSites[i].name, name) == 0)
            return g_build->luaSites[i].funcRva;
    return 0;
}

bool CodeSection(uint32_t* rva, uint32_t* size)
{
    const IMAGE_SECTION_HEADER* text = FindSection(".text");
    if (!text)
        return false;
    *rva = text->VirtualAddress;
    *size = text->Misc.VirtualSize;
    return true;
}

bool MatchPatternAt(uint32_t rva, const char* pattern)
{
    unsigned char want[kMaxPatternBytes];
    unsigned char mask[kMaxPatternBytes];
    const int n = ParseHex(pattern, want, mask, kMaxPatternBytes);
    if (n <= 0)
        return false;
    const uintptr_t at = Rva(rva);
    if (!Readable(at, static_cast<size_t>(n)))
        return false;
    return MaskedEqual(reinterpret_cast<const unsigned char*>(at), want, mask, n);
}

int FindPattern(const char* pattern, uint32_t* hits, int maxHits)
{
    unsigned char want[kMaxPatternBytes];
    unsigned char mask[kMaxPatternBytes];
    const int n = ParseHex(pattern, want, mask, kMaxPatternBytes);
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (n <= 0 || !mask[0] || !CodeSection(&textRva, &textSize))
        return 0;
    const uintptr_t start = Rva(textRva);
    if (!Readable(start, textSize))
        return 0;

    const auto* p = reinterpret_cast<const unsigned char*>(start);
    int found = 0;
    for (uint32_t i = 0; i + static_cast<uint32_t>(n) <= textSize; ++i)
    {
        if (p[i] != want[0] || !MaskedEqual(p + i, want, mask, n))
            continue;
        if (found < maxHits)
            hits[found] = textRva + i;
        ++found;
    }
    return found;
}

bool CodeLooksUnlocked()
{
    if (!g_build || !g_build->codeLockedOnDisk)
        return true;
    uint32_t textRva = 0;
    uint32_t textSize = 0;
    if (!CodeSection(&textRva, &textSize))
        return false;
    const size_t n = textSize < kEntropySample ? textSize : kEntropySample;
    const uintptr_t start = Rva(textRva);
    if (n == 0 || !Readable(start, n))
        return false;

    uint32_t hist[256] = {};
    const auto* p = reinterpret_cast<const unsigned char*>(start);
    for (size_t i = 0; i < n; ++i)
        ++hist[p[i]];
    double e = 0.0;
    for (uint32_t c : hist)
    {
        if (!c)
            continue;
        const double q = static_cast<double>(c) / static_cast<double>(n);
        e -= q * std::log2(q);
    }
    return e < kLockedEntropy;
}

void PublishResolvedBuild(const BuildInfo* resolved)
{
    g_build = resolved;
}

} // namespace bf1
