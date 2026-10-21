// BF1GameExt - tex_cap.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/fixes/; file names in the comments and log lines follow the new names. Nothing else changed.
//
// 22e: TWO CAPS - [Limits] MaxTextureSize (1024) when the game has 2 GB of address space, MaxTextureSize4GB (default 0 = no
// cap: EGM's textures at full size) when its exe has the 4 GB flag (BF1GameExt_4GB.bat, optional) and Windows gives it more
// than 2 GB. The safety net works in both. Nothing else changed.
//
// Phase 22d: THE TEXTURE CAP (BF1GameExt.ini [Limits] MaxTextureSize, default 1024; TextureSafetyNet, default 1).
//
// Why: 22c's memory log on EGM's maps (BES1r, abeach1a): the game's own heaps were fine (the Runtime heap 70 - 85 MB free,
// no failed game allocation); the 2 GB address space was not. Direct3D keeps a copy of every managed texture in the game's
// address space, and EGM's textures are big - many 2048 x 2048 uncompressed 32-bit (A8R8G8B8, 21 MB each with their mip
// levels). BES1r's textures took 1.1 GB in one load (the address space from 1141 MB free to 0 in a second, 38 textures
// failed: the untextured world); abeach1a went from 933 MB free to 7 in 0.6 s, 60 failed, and the game's loader thread then
// copied a level's data into a texture that was never made (battlefront.exe+0x348B8E, 22b's crash: the async read at
// +0x30A758).
//
// The game can already load a texture smaller: its loader (Steam rva 0x00283810, GOG 1.3 0x0027CE40 - a texture's FACE /
// INFO chunk) leaves out the top mip levels when the texture detail is Medium or Low (each LVL_ chunk below the skip is not
// read; the rest move up a level: Steam 0x00283B90). At High it skips none: "cmp eax, 2 / je" (Steam rva 0x002838F8, GOG 1.3
// 0x0027CF28) goes straight to "xor ebx, ebx" (no levels skipped). 22d sends that je to BF1GameExt's stub instead (pushad,
// TexSkip(mips, the texture's record), the result into ebx, popad, back to where the game applies the skip): the game then
// shifts the size, lowers the level count and reads only the levels kept, exactly as at Medium.
//   - MaxTextureSize: a texture wider or taller than this loads at this size (or under); 1024 = a quarter of a 2048
//     texture's memory. Smaller textures (the game's own, 256 - 1024) are untouched. 0 = no cap.
//   - TextureSafetyNet: before the game makes a texture, BF1GameExt asks Windows for an address range twice its size (reserved
//     and given straight back); if there is none, the texture loads one level smaller, and again, while it can - the texture
//     is made, smaller, instead of failing.
// The record (the loader's esi): +04h height, +08h width, +0Ch depth, +14h format, +18h type (1 2D, 2 cube, 3 volume); edx =
// its mip levels (the first je already sends 1-level textures past). [tex] lines: the first textures capped, every safety-net
// skip; the totals in the [mem] lines. Checked: the branch, its target and the join by pattern on both exes, and the patched
// set-up in Unicorn on both exes (22d sim: 2D / cube / volume at High through the stub, Medium unchanged).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "mem_log.hpp"
#include "game_check.hpp"
#include "tex_cap.hpp"

namespace bf1 {
namespace {

// cmp edx,1 / je noskip / cmp eax,2 / je noskip (High) / mov edi,[esi+4] / imul edi,[esi+0Ch] / mov eax,[esi+14h] /
// imul edi,[esi+8] / cmp eax,'DXT1'
const char* const kBranch = "83 FA 01 0F 84 ?? ?? ?? ?? 83 F8 02 0F 84 ?? ?? ?? ?? 8B 7E 04 0F AF 7E 0C 8B 46 14 0F AF 7E 08 3D 44 "
                            "58 54 31";
constexpr uint32_t kBranchHighJe = 0x0C; // 0F 84 rel32 (High: no levels skipped)
// mov edi,[ebp-14h] / lea eax,[edx-1] / cmp ebx,eax / cmovbe eax,ebx / mov ebx,eax / sub edx,ebx / mov ecx,ebx / shr [esi+8],cl
const char* const kJoin = "8B 7D EC 8D 42 FF 3B D8 0F 46 C3 8B D8 2B D3 8B CB D3 6E 08";
constexpr uint32_t kJoinSkip = 0x03; // where the skip (ebx) is applied; the no-skip path jumps here
const char* const kNoSkip = "33 DB EB"; // xor ebx, ebx / jmp short join+3

constexpr uint32_t kRecHeight = 0x04;
constexpr uint32_t kRecWidth = 0x08;
constexpr uint32_t kRecDepth = 0x0C;
constexpr uint32_t kRecFormat = 0x14;
constexpr uint32_t kRecType = 0x18;
constexpr uint64_t kProbeMinBytes = 1024u * 1024u; // the safety net looks at textures of 1 MB and more
constexpr LONG kCapLines = 12;
constexpr LONG kSafetyLines = 30;

unsigned g_cap = 1024;    // the one in force (22e: MaxTextureSize at 2 GB, MaxTextureSize4GB with the 4 GB flag)
unsigned g_cap2gb = 1024;
unsigned g_cap4gb = 0;
bool g_big = false;       // 22e: Windows gives the game more than 2 GB (the exe's 4 GB flag)
bool g_safety = true;
bool g_in = false;
unsigned char* g_stub = nullptr;
volatile LONG g_seen = 0;       // textures through the High branch (more than 1 level)
volatile LONG g_capped = 0;     // made smaller by the cap
volatile LONG g_safetySkips = 0; // textures made smaller by the safety net
volatile LONG g_kbAsked = 0;    // their chains' size as the files have them (KB)
volatile LONG g_kbMade = 0;     // ... as made

uint32_t Rd(const unsigned char* p)
{
    uint32_t v = 0;
    memcpy(&v, p, sizeof(v));
    return v;
}

const char* FormatName(uint32_t f, char* out, size_t n)
{
    switch (f)
    {
    case 21: snprintf(out, n, "A8R8G8B8"); break;
    case 22: snprintf(out, n, "X8R8G8B8"); break;
    case 23: snprintf(out, n, "R5G6B5"); break;
    case 25: snprintf(out, n, "A1R5G5B5"); break;
    case 26: snprintf(out, n, "A4R4G4B4"); break;
    case 28: snprintf(out, n, "A8"); break;
    case 50: snprintf(out, n, "L8"); break;
    case 51: snprintf(out, n, "A8L8"); break;
    default:
        if (f >= 0x20202020u && f <= 0x7A7A7A7Au)
            snprintf(out, n, "%c%c%c%c", static_cast<char>(f & 0xFF), static_cast<char>((f >> 8) & 0xFF),
                     static_cast<char>((f >> 16) & 0xFF), static_cast<char>((f >> 24) & 0xFF));
        else
            snprintf(out, n, "format %u", f);
    }
    return out;
}

// a mip chain's bytes from (w, h, d), levels of them (render_hooks.cpp's estimate: DXTn blocks, the common formats)
uint64_t ChainBytes(uint32_t w, uint32_t h, uint32_t d, int levels, uint32_t fmt)
{
    unsigned block = 0;
    unsigned bpp = 4;
    switch (fmt)
    {
    case 0x31545844u: block = 8; break;
    case 0x32545844u: case 0x33545844u: case 0x34545844u: case 0x35545844u: block = 16; break;
    case 20: bpp = 3; break;
    case 23: case 24: case 25: case 26: case 29: case 30: case 40: case 51: case 60: case 80: case 111: bpp = 2; break;
    case 27: case 28: case 41: case 50: case 52: bpp = 1; break;
    case 113: bpp = 8; break;
    case 116: bpp = 16; break;
    default: break;
    }
    uint64_t total = 0;
    uint32_t lw = w ? w : 1;
    uint32_t lh = h ? h : 1;
    uint32_t ld = d ? d : 1;
    for (int l = 0; l < levels && l < 16; ++l)
    {
        total += block ? static_cast<uint64_t>((lw + 3) / 4) * ((lh + 3) / 4) * block * ld : static_cast<uint64_t>(lw) * lh * ld * bpp;
        lw = lw > 1 ? lw / 2 : 1;
        lh = lh > 1 ? lh / 2 : 1;
        ld = ld > 1 ? ld / 2 : 1;
    }
    return total;
}

uint64_t RecBytes(uint32_t w, uint32_t h, uint32_t d, uint32_t type, int skip, int mips, uint32_t fmt)
{
    const uint32_t sw = w >> skip ? w >> skip : 1;
    const uint32_t sh = h >> skip ? h >> skip : 1;
    const uint32_t sd = type == 3 ? (d >> skip ? d >> skip : 1) : 1;
    return ChainBytes(sw, type == 2 ? sw : sh, sd, mips - skip, fmt) * (type == 2 ? 6u : 1u);
}

// is there still an address range of this size? (reserved and given straight back)
bool Fits(uint64_t bytes)
{
    const uint64_t want = bytes * 2 + 1024u * 1024u;
    if (want >= 0x7FFF0000ull)
        return false;
    void* p = VirtualAlloc(nullptr, static_cast<SIZE_T>(want), MEM_RESERVE, PAGE_NOACCESS);
    if (!p)
        return false;
    VirtualFree(p, 0, MEM_RELEASE);
    return true;
}

// the stub's call (any thread the game loads textures on): how many top levels to leave out
int __cdecl TexSkip(int mips, const unsigned char* rec)
{
    if (!rec || mips <= 1)
        return 0;
    const uint32_t h = Rd(rec + kRecHeight);
    const uint32_t w = Rd(rec + kRecWidth);
    const uint32_t d = Rd(rec + kRecDepth);
    const uint32_t fmt = Rd(rec + kRecFormat);
    const uint32_t type = Rd(rec + kRecType);
    InterlockedIncrement(&g_seen);
    int skip = 0;
    if (g_cap)
        while (skip + 1 < mips && ((w >> skip) > g_cap || (type != 2 && (h >> skip) > g_cap)))
            ++skip;
    const int capSkip = skip;
    if (g_safety)
    {
        while (skip + 1 < mips)
        {
            const uint64_t b = RecBytes(w, h, d, type, skip, mips, fmt);
            if (b < kProbeMinBytes || Fits(b))
                break;
            ++skip;
        }
    }
    const uint64_t asked = RecBytes(w, h, d, type, 0, mips, fmt);
    const uint64_t made = RecBytes(w, h, d, type, skip, mips, fmt);
    InterlockedExchangeAdd(&g_kbAsked, static_cast<LONG>((asked + 1023) / 1024));
    InterlockedExchangeAdd(&g_kbMade, static_cast<LONG>((made + 1023) / 1024));
    char f[16];
    if (capSkip)
    {
        const LONG n = InterlockedIncrement(&g_capped);
        if (n <= kCapLines)
            Log("[tex] %lu ms: %s %u x %u, %d levels, %s: loads at %u x %u (%d level(s) left out by the cap, %lu KB -> %lu KB)%s",
                ScanMsSinceAttach(), type == 2 ? "cube" : (type == 3 ? "volume" : "texture"), w, h, mips, FormatName(fmt, f, sizeof(f)),
                w >> capSkip ? w >> capSkip : 1u, h >> capSkip ? h >> capSkip : 1u, capSkip, static_cast<unsigned long>(asked / 1024),
                static_cast<unsigned long>(RecBytes(w, h, d, type, capSkip, mips, fmt) / 1024),
                n == kCapLines ? " - the rest are counted in the [mem] lines" : "");
    }
    if (skip > capSkip)
    {
        const LONG n = InterlockedIncrement(&g_safetySkips);
        if (n <= kSafetyLines)
        {
            char va[120];
            Log("[tex] %lu ms: SAFETY NET - %s %u x %u, %d levels, %s would not fit in the address space at %u x %u: loads at %u x %u "
                "(%lu KB); %s",
                ScanMsSinceAttach(), type == 2 ? "cube" : (type == 3 ? "volume" : "texture"), w, h, mips, FormatName(fmt, f, sizeof(f)),
                w >> capSkip ? w >> capSkip : 1u, h >> capSkip ? h >> capSkip : 1u, w >> skip ? w >> skip : 1u, h >> skip ? h >> skip : 1u,
                static_cast<unsigned long>(made / 1024), MemLogVaShort(va, sizeof(va)));
        }
    }
    return skip;
}

bool WriteCode(uintptr_t at, const void* bytes, size_t n)
{
    DWORD old = 0;
    if (!VirtualProtect(reinterpret_cast<void*>(at), n, PAGE_EXECUTE_READWRITE, &old))
        return false;
    memcpy(reinterpret_cast<void*>(at), bytes, n);
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<void*>(at), n, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(at), n);
    return true;
}

void ReadIni()
{
    char path[MAX_PATH];
    const DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (!n || n >= MAX_PATH)
        return;
    char* slash = strrchr(path, '\\');
    if (!slash)
        return;
    snprintf(slash + 1, static_cast<size_t>(MAX_PATH - (slash + 1 - path)), "BF1GameExt.ini");
    const int cap = static_cast<int>(GetPrivateProfileIntA("Limits", "MaxTextureSize", 1024, path));
    g_cap2gb = cap <= 0 ? 0u : (cap < 128 ? 128u : (cap > 8192 ? 8192u : static_cast<unsigned>(cap)));
    const int cap4 = static_cast<int>(GetPrivateProfileIntA("Limits", "MaxTextureSize4GB", 0, path)); // 22e
    g_cap4gb = cap4 <= 0 ? 0u : (cap4 < 128 ? 128u : (cap4 > 8192 ? 8192u : static_cast<unsigned>(cap4)));
    g_safety = GetPrivateProfileIntA("Limits", "TextureSafetyNet", 1, path) != 0;
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    g_big = reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress) > 0x80000000u;
    g_cap = g_big ? g_cap4gb : g_cap2gb;
}

} // namespace

void TexCapStart(bool onGameThread)
{
    static bool started = false;
    if (started)
        return;
    started = true;
    ReadIni();
    if (!g_cap && !g_safety)
    {
        Log("[tex] 22d: the texture cap is off ([Limits] %s=0, TextureSafetyNet=0)", g_big ? "MaxTextureSize4GB" : "MaxTextureSize");
        return;
    }
    const BuildInfo* b = CurrentBuild();
    if (!b || (strncmp(b->name, "Steam", 5) && strncmp(b->name, "GOG 1.3", 7)))
    {
        Log("[tex] 22d: the texture cap: not GOG 1.3 or Steam - not used");
        return;
    }
    uint32_t hits[4] = {};
    const int nb = FindPattern(kBranch, hits, 4);
    const uint32_t br = hits[0];
    const int nj = FindPattern(kJoin, hits, 4);
    const uint32_t jo = hits[0];
    if (nb != 1 || nj != 1)
    {
        Log("[tex] 22d: the texture loader's branch / join: %d / %d pattern hits (need 1 / 1) - the texture cap is NOT in", nb, nj);
        return;
    }
    const uintptr_t je = Rva(br + kBranchHighJe);
    int32_t rel = 0;
    memcpy(&rel, reinterpret_cast<const void*>(je + 2), sizeof(rel));
    const uintptr_t noSkip = je + 6 + static_cast<intptr_t>(rel);
    const uintptr_t join = Rva(jo + kJoinSkip);
    const int8_t short8 = *reinterpret_cast<const int8_t*>(noSkip + 3);
    const bool ok = *reinterpret_cast<const unsigned char*>(je) == 0x0F && *reinterpret_cast<const unsigned char*>(je + 1) == 0x84 &&
                    MatchPatternAt(static_cast<uint32_t>(noSkip - ExeBase()), kNoSkip) && noSkip + 4 + short8 == join;
    if (!ok)
    {
        Log("[tex] 22d: the High branch at rva 0x%08X does not lead to the no-skip path and the join (rva 0x%08X) as expected - the "
            "texture cap is NOT in", br + kBranchHighJe, jo + kJoinSkip);
        return;
    }
    g_stub = static_cast<unsigned char*>(VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!g_stub)
    {
        Log("[tex] 22d: no memory for the stub - the texture cap is NOT in");
        return;
    }
    // pushad / push esi / push edx / call TexSkip / add esp, 8 / mov [esp+10h], eax (ebx after popad) / popad / jmp join
    unsigned char* q = g_stub;
    *q++ = 0x60;
    *q++ = 0x56;
    *q++ = 0x52;
    *q++ = 0xE8;
    int (__cdecl* fn)(int, const unsigned char*) = &TexSkip;
    const int32_t toFn = static_cast<int32_t>(reinterpret_cast<intptr_t>(fn) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toFn, 4);
    q += 4;
    *q++ = 0x83; *q++ = 0xC4; *q++ = 0x08;
    *q++ = 0x89; *q++ = 0x44; *q++ = 0x24; *q++ = 0x10;
    *q++ = 0x61;
    *q++ = 0xE9;
    const int32_t toJoin = static_cast<int32_t>(static_cast<intptr_t>(join) - reinterpret_cast<intptr_t>(q + 4));
    memcpy(q, &toJoin, 4);
    q += 4;
    FlushInstructionCache(GetCurrentProcess(), g_stub, static_cast<SIZE_T>(q - g_stub));
    const int32_t toStub = static_cast<int32_t>(reinterpret_cast<intptr_t>(g_stub) - static_cast<intptr_t>(je + 6));
    if (!WriteCode(je + 2, &toStub, 4))
    {
        Log("[tex] 22d: the High branch could not be written (error %lu) - the texture cap is NOT in", GetLastError());
        return;
    }
    g_in = true;
    char capText[48];
    if (g_cap)
        snprintf(capText, sizeof(capText), "%u", g_cap);
    else
        snprintf(capText, sizeof(capText), "0 (no cap)");
    Log("[tex] 22d: THE TEXTURE CAP IN (%s): the texture loader's High-detail branch (its je at rva 0x%08X) goes through BF1GameExt "
        "(the no-skip path rva 0x%08X, the join rva 0x%08X) - the game's own mip skip, as at Medium: %s=%s (22e: %s; a "
        "texture wider or taller loads at that size), TextureSafetyNet=%d (a texture that would not fit in the address space "
        "loads a level smaller); [tex] lines, totals in the [mem] lines. Only at High texture detail (Medium / Low skip by "
        "themselves).",
        onGameThread ? "the game's DirectInput call" : "the timer", br + kBranchHighJe, static_cast<unsigned>(noSkip - ExeBase()),
        static_cast<unsigned>(join - ExeBase()), g_big ? "MaxTextureSize4GB" : "MaxTextureSize", capText,
        g_big ? "the game has more than 2 GB - the exe's 4 GB flag" : "the game has 2 GB - no 4 GB flag on the exe", g_safety ? 1 : 0);
}

const char* TexCapText(char* out, size_t n)
{
    if (!g_in)
    {
        out[0] = 0;
        return out;
    }
    snprintf(out, n, "textures at High detail so far: %ld loaded with mip levels, %ld made smaller by the cap (%s), %ld by the "
             "safety net; %ld MB as the files have them -> %ld MB made",
             static_cast<long>(g_seen), static_cast<long>(g_capped), g_cap ? (g_big ? "MaxTextureSize4GB" : "MaxTextureSize") : "off", static_cast<long>(g_safetySkips),
             static_cast<long>(g_kbAsked / 1024), static_cast<long>(g_kbMade / 1024));
    return out;
}

} // namespace bf1
