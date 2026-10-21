// BF1GameExt - vertical.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/splitscreen/; file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 8a (2020 build): side-by-side split screen through five byte-checked
// edits inside CameraManager::SetNumCameras' 2-player block. [vertical] lines.
// 21i-r8: switchable - with [Splitscreen] SplitMenu=1 the five edits go in whatever
// Vertical says (the operands point at BF1GameExt's values; with Vertical=0 those
// values are the game's own: top / bottom, ratio x0.5, 48 deg - nothing looks
// different), and the GAME OPTIONS page's SPLIT SCREEN row switches them
// (VerticalSet): from the main menu at once (the next match's SetNumCameras(2) takes it);
// from a match's pause menu it waits (pending) until the main menu is made again
// (VerticalApplyPending, menus.cpp at ifs_main) - the match keeps its layout,
// its HUD and spawn screens were laid out for it. The choice is written back to
// BF1GameExt.ini (Vertical=0 / 1) either way.
//
// Found by ANCHORS rather than one long pattern (Steam's build of this function
// differs from GOG 1.3's in small ways; r2's 143-byte pattern missed it):
//   1. the view rect table in the exe's data: top half (0, 0, 1, 0.5), with the
//      bottom half (0, 0.5, 1, 0.5) 0x20 later and full screen 0x40 later
//   2. the two code loads of top / bottom: movaps xmmN, [rect]
//   3. in the 2-player block (from the top load up to the full-screen rect's
//      load, which starts the 1-camera branch): the multiply by the game's 0.5
//      (ratio * 0.5) and the two 48-degree immediates (one per camera)
// Then: the rect operands point at our left / right halves, the 0.5 operand at
// our 2.0, and both FOV immediates become the Xbox build's side-by-side 36 deg.
// If anything does not match, the area's bytes go into the log and nothing
// (or, if only the FOV is missing, everything but the FOV) is changed.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"
#include "vertical.hpp"

namespace bf1 {
namespace {

// The PC build's 48 deg (0.8378 rad), pushed for each camera of the 2-player
// block, and the Xbox build's side-by-side angle (0x3F20D97C, pushed by its
// camera manager's split-direction setter for both halves): 0.6283 rad = 36 deg.
constexpr uint32_t kPcFovBits = 0x3F567750u;
constexpr uint32_t kXboxSideBySideFovBits = 0x3F20D97Cu;

const float kTop[4] = { 0.0f, 0.0f, 1.0f, 0.5f };
const float kBottom[4] = { 0.0f, 0.5f, 1.0f, 0.5f };
const float kFull[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

// What BF1GameExt gives the block instead. movaps needs 16-byte alignment.
// (21i-r8: player 1's / player 2's rect and the ratio multiplier as the layout in force - side by side below, or the
// game's own top / bottom with VerticalSet(false))
alignas(16) float g_leftRect[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
alignas(16) float g_rightRect[4] = { 0.5f, 0.0f, 0.5f, 1.0f };
alignas(16) float g_ratioMul = 2.0f;
// 21i-r8: the switchable layout
bool g_installed = false;   // the rects / ratio operands point at ours
bool g_vertical = false;    // the layout in force (the next SetNumCameras(2) uses it)
uintptr_t g_fovImm[2] = {}; // the two 48-degree immediates (0 if not found)
int g_switches = 0;
int g_pending = -1;         // 21i-r8: chosen in a match (0 / 1), put in when the main menu is made again; -1 none

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

float ReadF32(uintptr_t va)
{
    float v = 0.0f;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

bool InExe(uintptr_t va, size_t len)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va + len <= ExeBase() + b->sizeOfImage;
}

bool IniPath(char* ini, size_t n)
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return false;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return false;
    *slash = 0;
    snprintf(ini, n, "%s\\BF1GameExt.ini", exe);
    return true;
}

bool ReadIniVertical()
{
    char ini[MAX_PATH + 32];
    if (!IniPath(ini, sizeof(ini)))
        return false;
    return GetPrivateProfileIntA("Splitscreen", "Vertical", 0, ini) != 0;
}

// 21i-r8: [Splitscreen] SplitMenu (the GAME OPTIONS page's SPLIT SCREEN row; default 1)
bool ReadIniSplitMenu()
{
    char ini[MAX_PATH + 32];
    if (!IniPath(ini, sizeof(ini)))
        return false;
    return GetPrivateProfileIntA("Splitscreen", "SplitMenu", 1, ini) != 0;
}

// 1. The rect table: top, bottom at +0x20, full at +0x40, in a readable,
//    non-code section of the exe (initialized part only).
int FindRectTables(uintptr_t* out, int maxOut)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    CodeSection(&codeRva, &codeSize);

    const uintptr_t base = ExeBase();
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return 0;
    const IMAGE_NT_HEADERS32* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + static_cast<uintptr_t>(dos->e_lfanew));
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return 0;
    const IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);

    int n = 0;
    for (unsigned s = 0; s < nt->FileHeader.NumberOfSections; ++s)
    {
        if (!(sec[s].Characteristics & IMAGE_SCN_MEM_READ) || sec[s].VirtualAddress == codeRva)
            continue;
        uint32_t len = sec[s].Misc.VirtualSize;
        if (sec[s].SizeOfRawData && sec[s].SizeOfRawData < len)
            len = sec[s].SizeOfRawData;
        if (len < 0x50)
            continue;
        const uintptr_t start = base + sec[s].VirtualAddress;
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)) ||
            mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
            continue;
        for (uint32_t off = 0; off + 0x50 <= len; off += 4)
        {
            const uintptr_t p = start + off;
            if (memcmp(reinterpret_cast<const void*>(p), kTop, 16) == 0 &&
                memcmp(reinterpret_cast<const void*>(p + 0x20), kBottom, 16) == 0 &&
                memcmp(reinterpret_cast<const void*>(p + 0x40), kFull, 16) == 0)
            {
                if (n < maxOut)
                    out[n] = p;
                ++n;
            }
        }
    }
    return n;
}

// 2. Code loads of a 16-byte constant: movaps xmmN, [disp32] = 0F 28 (05..3D) <va>
int FindMovapsLoads(uintptr_t va, uintptr_t* out, int maxOut)
{
    uint32_t codeRva = 0;
    uint32_t codeSize = 0;
    if (!CodeSection(&codeRva, &codeSize) || codeSize < 8)
        return 0;
    const unsigned char* code = reinterpret_cast<const unsigned char*>(Rva(codeRva));
    const uint32_t want = static_cast<uint32_t>(va);
    int n = 0;
    for (uint32_t i = 0; i + 7 <= codeSize; ++i)
    {
        if (code[i] != 0x0F || code[i + 1] != 0x28 || (code[i + 2] & 0xC7) != 0x05)
            continue;
        uint32_t disp = 0;
        memcpy(&disp, code + i + 3, sizeof(disp));
        if (disp != want)
            continue;
        if (n < maxOut)
            out[n] = reinterpret_cast<uintptr_t>(code + i);
        ++n;
    }
    return n;
}

void DumpBytes(const char* why, uintptr_t from, uint32_t len)
{
    Log("[vertical] %s - bytes from rva 0x%08X so it can be matched next time:", why,
        static_cast<unsigned>(from - ExeBase()));
    for (uint32_t off = 0; off < len; off += 16)
    {
        char line[96];
        int pos = snprintf(line, sizeof(line), "[vertical]   +%03X:", static_cast<unsigned>(off));
        for (uint32_t i = 0; i < 16 && off + i < len && pos > 0 && pos < 90; ++i)
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %02X",
                            static_cast<unsigned>(
                                *reinterpret_cast<const unsigned char*>(from + off + i)));
        Log("%s", line);
    }
}

bool Patch32(uintptr_t at, uint32_t value)
{
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(at), 4, PAGE_EXECUTE_READWRITE, &oldProt))
        return false;
    memcpy(reinterpret_cast<void*>(at), &value, sizeof(value));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(at), 4, oldProt, &ignored);
    FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<LPCVOID>(at), 4);
    return true;
}

uint32_t RvaOf(uintptr_t va)
{
    return static_cast<uint32_t>(va - ExeBase());
}

// 21i-r8: the values for a layout: side by side (the Xbox's) or top / bottom (the game's own values, as its table and its
// 0.5 have them); the FOV immediates 36 / 48 deg
void ApplyLayout(bool vertical)
{
    const float a[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
    const float b[4] = { 0.5f, 0.0f, 0.5f, 1.0f };
    memcpy(g_leftRect, vertical ? a : kTop, sizeof(g_leftRect));
    memcpy(g_rightRect, vertical ? b : kBottom, sizeof(g_rightRect));
    g_ratioMul = vertical ? 2.0f : 0.5f;
    for (uintptr_t imm : g_fovImm)
        if (imm)
            Patch32(imm, vertical ? kXboxSideBySideFovBits : kPcFovBits);
    g_vertical = vertical;
}

} // namespace

bool VerticalSwitchable()
{
    return g_installed;
}

bool VerticalOn()
{
    return g_installed && g_vertical;
}

// 21i-r8: what the row shows: the pending choice if there is one, else the layout in force
bool VerticalWanted()
{
    return g_pending >= 0 ? g_pending != 0 : g_vertical;
}

bool VerticalPendingDiffers()
{
    return g_installed && g_pending >= 0 && (g_pending != 0) != g_vertical;
}

// 21i-r8: the GAME OPTIONS page's SPLIT SCREEN row (the game thread - Lua): now = from the main menu (the next
// SetNumCameras(2) takes it), else from a match (pending until the main menu is made again); BF1GameExt.ini's Vertical
bool VerticalSet(bool vertical, bool now)
{
    if (!g_installed)
        return false;
    const bool was = VerticalWanted();
    if (now)
    {
        g_pending = -1;
        ApplyLayout(vertical);
    }
    else
        g_pending = (vertical == g_vertical) ? -1 : (vertical ? 1 : 0);
    char ini[MAX_PATH + 32];
    const bool wrote = IniPath(ini, sizeof(ini)) && WritePrivateProfileStringA("Splitscreen", "Vertical", vertical ? "1" : "0", ini) != 0;
    if (++g_switches <= 100)
        Log("[vertical] 21i-r8: GAME OPTIONS' SPLIT SCREEN: %s -> %s (%s; FOV %s); BF1GameExt.ini Vertical=%d %s",
            was ? "VERTICAL" : "HORIZONTAL", vertical ? "VERTICAL" : "HORIZONTAL",
            now ? "the next match's 2-player split" : g_pending >= 0 ? "in a match - this match keeps its layout, the next one after the main menu gets it"
                                                                  : "in a match - back to the layout this match has",
            (g_fovImm[0] && g_fovImm[1]) ? (vertical ? "36 deg" : "48 deg") : "not found - 48 deg", vertical ? 1 : 0,
            wrote ? "written" : "NOT written");
    return true;
}

// 21i-r8: the main menu made again (menus.cpp, ScriptCB_AddIFScreen("ifs_main")): a choice made in a match goes in
void VerticalApplyPending()
{
    if (!g_installed || g_pending < 0)
        return;
    const bool v = g_pending != 0;
    g_pending = -1;
    ApplyLayout(v);
    if (++g_switches <= 100)
        Log("[vertical] 21i-r8: the main menu - the SPLIT SCREEN chosen in the match goes in: %s for the next match", v ? "VERTICAL" : "HORIZONTAL");
}

void VerticalStart(bool onGameThread)
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[vertical] not used on this build");
        return;
    }
    const bool wantVertical = ReadIniVertical();
    const bool menu = ReadIniSplitMenu(); // 21i-r8
    if (!wantVertical && !menu)
    {
        Log("[vertical] BF1GameExt.ini [Splitscreen] Vertical=0, SplitMenu=0: the game's top / bottom split (nothing installed)");
        return;
    }
    if (!onGameThread)
    {
        Log("[vertical] the scan did not run on the game thread - NOT installed (not safe)");
        return;
    }

    // 1. The rect table.
    uintptr_t tables[4] = {};
    const int nt = FindRectTables(tables, 4);
    if (nt != 1)
    {
        Log("[vertical] view rect table (top, bottom +0x20, full +0x40): %d found (need exactly 1) "
            "- NOT installed",
            nt);
        return;
    }
    const uintptr_t topVa = tables[0];
    Log("[vertical] view rect table at rva 0x%08X (top, bottom, full screen)  OK", RvaOf(topVa));

    // 2. The loads of top and bottom, and of full screen (the 1-camera branch).
    uintptr_t topLoads[4] = {};
    uintptr_t bottomLoads[4] = {};
    uintptr_t fullLoads[4] = {};
    const int ntl = FindMovapsLoads(topVa, topLoads, 4);
    const int nbl = FindMovapsLoads(topVa + 0x20, bottomLoads, 4);
    const int nfl = FindMovapsLoads(topVa + 0x40, fullLoads, 4);
    if (ntl != 1 || nbl != 1)
    {
        Log("[vertical] loads of the top / bottom rects: %d / %d (need 1 / 1) - NOT installed", ntl,
            nbl);
        return;
    }
    const uintptr_t topLoad = topLoads[0];
    const uintptr_t bottomLoad = bottomLoads[0];
    if (bottomLoad <= topLoad || bottomLoad - topLoad > 0x40)
    {
        Log("[vertical] the bottom rect's load is not just after the top's - NOT installed");
        DumpBytes("top / bottom loads apart", topLoad - 0x20, 0x80);
        return;
    }
    // The 2-player block ends where the 1-camera branch loads the full rect.
    uintptr_t blockEnd = topLoad + 0x100;
    for (int i = 0; i < nfl && i < 4; ++i)
        if (fullLoads[i] > bottomLoad && fullLoads[i] < blockEnd)
            blockEnd = fullLoads[i];
    Log("[vertical] loads: top at rva 0x%08X, bottom at rva 0x%08X, 2-player block ends at rva "
        "0x%08X  OK",
        RvaOf(topLoad), RvaOf(bottomLoad), RvaOf(blockEnd));

    // 3a. ratio * 0.5: mulss xmmN, [disp32] whose target is the game's 0.5.
    uintptr_t ratioOperand = 0;
    for (uintptr_t p = bottomLoad + 7; p + 8 <= blockEnd; ++p)
    {
        const unsigned char* c = reinterpret_cast<const unsigned char*>(p);
        if (c[0] != 0xF3 || c[1] != 0x0F || c[2] != 0x59 || (c[3] & 0xC7) != 0x05)
            continue;
        const uintptr_t target = ReadU32(p + 4);
        if (InExe(target, 4) && ReadF32(target) == 0.5f)
        {
            ratioOperand = p + 4;
            break;
        }
    }
    if (!ratioOperand)
    {
        Log("[vertical] no 'ratio * 0.5' in the 2-player block - NOT installed");
        DumpBytes("2-player block", topLoad - 0x20, static_cast<uint32_t>(blockEnd - topLoad) + 0x40);
        return;
    }

    // 3b. The two 48-degree immediates (mov dword ptr [...], imm32 = C7 ...).
    uintptr_t fovImm[4] = {};
    int nfov = 0;
    for (uintptr_t p = ratioOperand; p + 4 <= blockEnd; ++p)
    {
        if (ReadU32(p) != kPcFovBits)
            continue;
        const unsigned char* c = reinterpret_cast<const unsigned char*>(p);
        if (c[-3] != 0xC7 && c[-4] != 0xC7)
            continue;
        if (nfov < 4)
            fovImm[nfov] = p;
        ++nfov;
    }

    // Change the views and the ratio (these alone already give correctly
    // proportioned halves); the FOV only if both immediates were found.
    // (21i-r8: our values first - the layout Vertical asks for - then the operands; the FOV values once those went in)
    ApplyLayout(wantVertical);
    if (!Patch32(topLoad + 3, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_leftRect))) ||
        !Patch32(bottomLoad + 3, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_rightRect))) ||
        !Patch32(ratioOperand, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_ratioMul))))
    {
        Log("[vertical] VirtualProtect failed (error %lu) - NOT installed", GetLastError());
        return;
    }
    g_installed = true;
    if (nfov == 2)
    {
        g_fovImm[0] = fovImm[0];
        g_fovImm[1] = fovImm[1];
        ApplyLayout(wantVertical);
    }
    Log("[vertical] views and ratio operands at BF1GameExt's values (ratio operand at rva 0x%08X)  installed - %s; 21i-r8: "
        "[Splitscreen] SplitMenu=%d (%s)",
        RvaOf(ratioOperand),
        wantVertical ? "VERTICAL: player 1 left (0, 0, 0.5, 1), player 2 right (0.5, 0, 0.5, 1), ratio x2"
                     : "HORIZONTAL (Vertical=0): the game's own top / bottom and ratio x0.5",
        menu ? 1 : 0, menu ? "GAME OPTIONS' SPLIT SCREEN row switches it for the next match" : "no row - Vertical only");

    if (nfov == 2)
    {
        Log("[vertical] cameras: the two 48-degree values at rva 0x%08X and 0x%08X  found - %s",
            RvaOf(fovImm[0]), RvaOf(fovImm[1]),
            wantVertical ? "36.0 deg (the Xbox build's side-by-side angle)" : "48 deg (the game's own, horizontal)");
    }
    else
    {
        Log("[vertical] found %d 48-degree values in the 2-player block (need 2) - the halves keep "
            "48 deg across (still correctly proportioned)",
            nfov);
        DumpBytes("2-player block", topLoad - 0x20, static_cast<uint32_t>(blockEnd - topLoad) + 0x40);
    }
}

} // namespace bf1
