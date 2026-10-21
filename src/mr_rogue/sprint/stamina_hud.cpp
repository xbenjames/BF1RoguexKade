// BF1GameExt - stamina_hud.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/sprint/; file names in the comments and log lines follow the new names. Nothing else changed.
// 20g-r5: players 3 and 4's stamina bar - their own StatusDisplays (players34.cpp's 20g HUD) go through the same
// update hook (it is the class's), recognised by PlayerNHudStatusPlayer; the per-player state is 4 wide.
// Phase 14c (2020 build): HUD scan for the stamina bar, log-only. [hud] lines.
//
// GOG 1.3 analysis (the same code as Steam):
//   StatusDisplay array: 0x0061C900 allocates one StatusDisplay (0x34A0 bytes)
//     per slot of a 2-pointer global array:
//       push esi; mov esi, <array>; push 34A0h; call new; ...; mov [esi], eax;
//       add esi, 4; cmp esi, <array + 8>; jl ...
//   StatusDisplay::Update (vtable 0x007EF790 slot 1, 0x0061D100) updates its
//     embedded panels: +0x130 (0x00552C50: health + secondary bar), +0x16C0
//     (0x005510C0: weapon panel, sub-panels +0xD0 and +0x920), +0x2840.
//   The secondary bar (0x00552C50) is one slot shared by vehicle health,
//     repair, JET PACK FUEL and shield: icon element +0x3D0 (hud_jetpack_icon,
//     24 x 24 at -35, 20), bar elements +0x4D0 / +0x5D0. The jet fuel bar
//     (0x00553180) is a bitmap of the "hud1" atlas (48 x 12, UV 0.30,0.76 ..
//     0.68,0.85) whose width and UV are scaled by the fuel left, coloured by
//     a blend of two colours (low fuel flashes).
//   Interface element fields: +0x14 flags (bit 8 = visible), +0x30 matrix
//     (position = +0x60, +0x64), +0x70 colour (ARGB); RedBitmapElement: quad
//     corners +0x88/+0x8C .. +0xDC/+0xE0, UVs +0x94/+0x98 .. +0xE8/+0xEC,
//     texture +0xF8.
// Element classes are found by their RTTI names (type descriptor in .data,
// complete object locator and vtable in .rdata), not by address.
// r2: memory checks walk every region of a range (r1 accepted only ranges
// inside ONE region, and on Steam .data / .rdata are split into several, so
// the scan switched itself off: ".data / .rdata not readable").
//
// Phase 14d - THE STAMINA BAR. The 14c-r2 dump (Steam, both players) showed
// that each weapon sub-panel (same class, primary at +0x1790, grenade at
// +0x1FE0) owns a bar bitmap at sub-panel +0x740 that is hidden unless its
// weapon charges or heats up. The grenade panel's one (StatusDisplay +0x2720)
// is the JET FUEL BAR ART: "hud1" atlas, 48 x 12, UV 0.680,0.758 .. 0.305,0.852
// (the status panel's jet bar mirrored for the left side), at 32, 2.
// The weapon sub-panel update (0x00551220) sets that bar's visibility itself
// every frame (shown only for a charging / heating weapon) and reads it back.
// So the game never sees our changes:
//   StatusDisplay::Update (vtable slot 1, 0x0061D100) goes through BF1GameExt:
//     1. put back the bar exactly as the game left it last frame (if we drew)
//     2. the game's own update
//     3. if the game left the bar hidden and the player is on foot: save it,
//        then show it as the stamina bar - moved under the grenade icon
//        (INI StaminaBarX / Y), width x stamina left, its UV cut to match
//        (drains like the jet pack bar), green; below the minimum to sprint it
//        is red (14e; 14d flashed it green / dim green).
//   The frame is drawn after the update, with our bar in it. If the game is
//   using the bar for its own weapon, it wins and the stamina bar waits.
// The vtable slot is data (.rdata), checked before it is written: the slot
// must lead to the update's own bytes.
// 14e: after running out, the bar is RED while it refills (StaminaBarLowColour,
// default the HUD's own red FFDF2020) and turns green again once there is
// enough to sprint (StaminaMinSprint). No flashing.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "players34.hpp"
#include "game_check.hpp"
#include "sprint.hpp"
#include "stamina_hud.hpp"

namespace bf1 {
namespace {

const char* const kFileBuild = "p23a-r1";

const char* const kStatusArrayAlloc =
    "56 BE ?? ?? ?? ?? 68 A0 34 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 09 8B C8 E8 ?? ?? ?? ?? "
    "EB 02 33 C0 89 06 83 C6 04 81 FE ?? ?? ?? ?? 7C D7";
constexpr uint32_t kOffArrayStart = 2;
constexpr uint32_t kOffArrayEnd = 41;
constexpr uint32_t kStatusSize = 0x34A0;

const char* const kClassNames[] = {
    "RedBitmapElement",  "RedMaskedBitmapElement", "RedTextElement",   "FlashyTextElement",
    "RedGroupElement",   "RedScreenGroupElement",  "RedVectorElement", "Red3DModelElement",
    "RedInterfaceElement", "SoldierElement",       "EntityElement",    "DroidekaElement",
    "RedGlyphCacheElement",
};
constexpr int kClassCount = static_cast<int>(sizeof(kClassNames) / sizeof(kClassNames[0]));
constexpr int kBitmapClasses = 2; // the first two have the bitmap quad

constexpr uint32_t kFlags = 0x14;
constexpr uint32_t kPosX = 0x60;
constexpr uint32_t kPosY = 0x64;
constexpr uint32_t kColour = 0x70;
constexpr uint32_t kQuad0X = 0x88;
constexpr uint32_t kQuad0Y = 0x8C;
constexpr uint32_t kUv0U = 0x94;
constexpr uint32_t kUv0V = 0x98;
constexpr uint32_t kQuad3X = 0xDC;
constexpr uint32_t kQuad3Y = 0xE0;
constexpr uint32_t kUv3U = 0xE8;
constexpr uint32_t kUv3V = 0xEC;
constexpr uint32_t kTexture = 0xF8;
constexpr uint32_t kElementSpan = 0x100;

constexpr DWORD kDumpDelayMs = 3000;
constexpr int kMaxDumpsPerPlayer = 3;
constexpr int kMaxLines = 900;

// 14d: StatusDisplay::Update (vtable slot 1) and the grenade panel's bar.
const char* const kStatusUpdateBody =
    "55 8B EC 53 56 8B F1 57 FF 76 18 E8 ?? ?? ?? ?? 83 C4 04 50 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? "
    "8B F8 85 FF 0F 84";
constexpr int kUpdateSlot = 1;
constexpr uint32_t kGrenadeBar = 0x2720;
constexpr uint32_t kVert[4] = { 0x88, 0xA4, 0xC0, 0xDC }; // x, y, then u, v at +0xC / +0x10
// The art the bar must have before it is borrowed (the jet fuel bar, mirrored).
constexpr float kArtU0 = 0.680f;
constexpr float kArtV0 = 0.758f;
constexpr float kArtU1 = 0.305f;
constexpr float kArtV1 = 0.852f;
constexpr float kArtW = 48.0f;
constexpr float kArtH = 12.0f;

typedef bool(__fastcall* StatusUpdate_t)(void* self, void* edx, float dt);
StatusUpdate_t g_origUpdate = nullptr;

bool g_barOn = true;
float g_barX = 6.0f;
float g_barY = 36.0f;
float g_barW = 48.0f;
uint32_t g_barColour = 0xFF2DDF2D;
uint32_t g_barLowColour = 0xFFDF2020;

struct BarSave
{
    bool drawn;          // we changed the bar this frame: put it back next frame
    uint32_t flags;
    float x;
    float y;
    uint32_t colour;
    float vert[4][4];    // x, y, u, v of each corner
};
BarSave g_save[4] = {};
bool g_artChecked[4] = {};
bool g_artOk[4] = {};
bool g_loggedDraw[4] = {};
bool g_gameUsing[4] = {};
long g_drawn[4] = {};

uintptr_t g_array = 0;
uintptr_t g_vtables[kClassCount] = {};
bool g_ready = false;
int g_lines = 0;
uintptr_t g_lastSoldier[2] = {};
DWORD g_due[2] = {};
int g_dumps[2] = {};

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[hud] line limit reached, going quiet");
    return true;
}

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

// True if every page of [start, start + len) is committed and readable (walks
// all the regions the range crosses).
bool Readable(uintptr_t start, size_t len)
{
    if (!start)
        return false;
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
        const uintptr_t next = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        if (next <= p)
            return false;
        p = next;
    }
    return true;
}

// Start and length of an exe section's initialised part (false if missing).
bool Section(const char* name, uintptr_t* start, uint32_t* len)
{
    const uintptr_t base = ExeBase();
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return false;
    const auto* sec = reinterpret_cast<const IMAGE_SECTION_HEADER*>(
        reinterpret_cast<uintptr_t>(&nt->OptionalHeader) + nt->FileHeader.SizeOfOptionalHeader);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        char nm[9] = {};
        memcpy(nm, sec[i].Name, 8);
        if (strcmp(nm, name) != 0)
            continue;
        uint32_t size = sec[i].Misc.VirtualSize;
        if (sec[i].SizeOfRawData && sec[i].SizeOfRawData < size)
            size = sec[i].SizeOfRawData;
        *start = base + sec[i].VirtualAddress;
        *len = size;
        return Readable(*start, size);
    }
    return false;
}

// Aligned dword search; returns the address or 0. 'from' continues a search.
uintptr_t FindDword(uintptr_t start, uint32_t len, uint32_t value, uintptr_t from)
{
    uintptr_t p = from ? from + 4 : start;
    for (; p + 4 <= start + len; p += 4)
        if (ReadU32(p) == value)
            return p;
    return 0;
}

// vtable of a class by its RTTI name (complete object, offset 0), or 0.
uintptr_t VtableByRtti(const char* cls, uintptr_t data, uint32_t dataLen, uintptr_t rdata,
                       uint32_t rdataLen)
{
    char mangled[96];
    const int n = snprintf(mangled, sizeof(mangled), ".?AV%s@@", cls);
    if (n <= 0 || n >= static_cast<int>(sizeof(mangled)))
        return 0;
    const size_t want = static_cast<size_t>(n) + 1; // with the terminating 0
    const unsigned char* d = reinterpret_cast<const unsigned char*>(data);
    for (uint32_t i = 8; i + want <= dataLen; ++i)
    {
        if (d[i] != '.' || memcmp(d + i, mangled, want) != 0)
            continue;
        const uint32_t td = static_cast<uint32_t>(data + i - 8);
        uintptr_t ref = 0;
        while ((ref = FindDword(rdata, rdataLen, td, ref)) != 0)
        {
            const uintptr_t col = ref - 12;
            if (col < rdata || ReadU32(col) != 0 || ReadU32(col + 4) != 0)
                continue;
            const uintptr_t vref = FindDword(rdata, rdataLen, static_cast<uint32_t>(col), 0);
            if (vref)
                return vref + 4;
        }
    }
    return 0;
}

int ClassOf(uint32_t vtable)
{
    for (int i = 0; i < kClassCount; ++i)
        if (g_vtables[i] && vtable == g_vtables[i])
            return i;
    return -1;
}

const char* Region(uint32_t off)
{
    if (off < 0x130)
        return "display";
    if (off < 0x16C0)
        return "status";
    if (off < 0x1790)
        return "weapon";
    if (off < 0x1FE0)
        return "weapon/primary";
    if (off < 0x2840)
        return "weapon/grenade";
    return "panel3";
}

const char* Known(uint32_t off)
{
    switch (off)
    {
    case 0x0130: return "  <- status panel";
    case 0x0500: return "  <- secondary-bar ICON (jet pack / vehicle / repair / shield)";
    case 0x0600: return "  <- secondary BAR 1 (jet pack fuel)";
    case 0x0700: return "  <- secondary BAR 2";
    case 0x16C0: return "  <- weapon panel";
    case 0x1790: return "  <- primary weapon sub-panel";
    case 0x1FE0: return "  <- grenade sub-panel";
    case 0x2840: return "  <- third panel";
    default: return "";
    }
}

void Dump(int p, uintptr_t sd)
{
    if (!Readable(sd, kStatusSize))
    {
        if (Budget())
            Log("[hud] %lu ms: player %d's StatusDisplay 0x%08X is not readable", ScanMsSinceAttach(),
                p + 1, static_cast<unsigned>(sd));
        return;
    }
    if (Budget())
        Log("[hud] %lu ms: player %d's StatusDisplay 0x%08X (dump %d of %d) - every element inside it:",
            ScanMsSinceAttach(), p + 1, static_cast<unsigned>(sd), g_dumps[p], kMaxDumpsPerPlayer);
    int count = 0;
    for (uint32_t off = 0; off + 4 <= kStatusSize; off += 4)
    {
        const int c = ClassOf(ReadU32(sd + off));
        if (c < 0)
            continue;
        const uintptr_t e = sd + off;
        if (off + kElementSpan > kStatusSize && !Readable(e, kElementSpan))
            continue;
        ++count;
        if (!Budget())
            return;
        const uint32_t flags = ReadU32(e + kFlags);
        char extra[200] = "";
        if (c < kBitmapClasses)
        {
            const uintptr_t tex = ReadU32(e + kTexture);
            uint32_t t0 = 0;
            uint32_t t1 = 0;
            if (tex && Readable(tex, 8))
            {
                t0 = ReadU32(tex);
                t1 = ReadU32(tex + 4);
            }
            snprintf(extra, sizeof(extra),
                     " | rect %.1f %.1f .. %.1f %.1f | uv %.3f %.3f .. %.3f %.3f | texture 0x%08X "
                     "(%08X %08X)",
                     static_cast<double>(ReadF32(e + kQuad0X)), static_cast<double>(ReadF32(e + kQuad0Y)),
                     static_cast<double>(ReadF32(e + kQuad3X)), static_cast<double>(ReadF32(e + kQuad3Y)),
                     static_cast<double>(ReadF32(e + kUv0U)), static_cast<double>(ReadF32(e + kUv0V)),
                     static_cast<double>(ReadF32(e + kUv3U)), static_cast<double>(ReadF32(e + kUv3V)),
                     static_cast<unsigned>(tex), static_cast<unsigned>(t0), static_cast<unsigned>(t1));
        }
        Log("[hud]   +0x%04X %-14s %-22s %s pos %7.1f %7.1f colour %08X%s%s", static_cast<unsigned>(off),
            Region(off), kClassNames[c], (flags >> 8) & 1 ? "shown " : "hidden",
            static_cast<double>(ReadF32(e + kPosX)), static_cast<double>(ReadF32(e + kPosY)),
            static_cast<unsigned>(ReadU32(e + kColour)), extra, Known(off));
    }
    if (Budget())
        Log("[hud] %lu ms: player %d: %d elements listed", ScanMsSinceAttach(), p + 1, count);
}


// ---------------------------------------------------------------------------
// 14d: the stamina bar
// ---------------------------------------------------------------------------
void WriteF32(uintptr_t va, float v)
{
    memcpy(reinterpret_cast<void*>(va), &v, sizeof(v));
}

void WriteU32(uintptr_t va, uint32_t v)
{
    memcpy(reinterpret_cast<void*>(va), &v, sizeof(v));
}

bool Near(float a, float b)
{
    const float d = a - b;
    return d > -0.01f && d < 0.01f;
}

uint32_t IniHex(const char* ini, const char* key, uint32_t def)
{
    char defText[16];
    snprintf(defText, sizeof(defText), "%08X", static_cast<unsigned>(def));
    char buf[32] = {};
    GetPrivateProfileStringA("Sprint", key, defText, buf, sizeof(buf), ini);
    char* end = nullptr;
    const unsigned long v = strtoul(buf, &end, 16);
    return end == buf ? def : static_cast<uint32_t>(v);
}

float IniF(const char* ini, const char* key, float def, float lo, float hi)
{
    char defText[32];
    snprintf(defText, sizeof(defText), "%g", static_cast<double>(def));
    char buf[32] = {};
    GetPrivateProfileStringA("Sprint", key, defText, buf, sizeof(buf), ini);
    char* end = nullptr;
    double v = strtod(buf, &end);
    if (end == buf)
        v = def;
    if (v < lo)
        v = lo;
    if (v > hi)
        v = hi;
    return static_cast<float>(v);
}

void ReadBarIni()
{
    char exe[MAX_PATH] = {};
    if (!GetModuleFileNameA(nullptr, exe, MAX_PATH))
        return;
    char* slash = strrchr(exe, '\\');
    if (!slash)
        return;
    *slash = 0;
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", exe);
    g_barOn = GetPrivateProfileIntA("Sprint", "StaminaBar", 0, ini) != 0; // v6.4: default off
    g_barX = IniF(ini, "StaminaBarX", 6.0f, -500.0f, 500.0f);
    g_barY = IniF(ini, "StaminaBarY", 36.0f, -500.0f, 500.0f);
    g_barW = IniF(ini, "StaminaBarWidth", kArtW, 4.0f, 400.0f);
    g_barColour = IniHex(ini, "StaminaBarColour", 0xFF2DDF2D);
    g_barLowColour = IniHex(ini, "StaminaBarLowColour", 0xFFDF2020);
}

int PlayerOf(uintptr_t sd)
{
    if (!g_array || !sd)
        return -1;
    if (ReadU32(g_array) == sd)
        return 0;
    if (ReadU32(g_array + 4) == sd)
        return 1;
    return PlayerNHudStatusPlayer(sd); // 20g-r5: players 3 and 4's own StatusDisplays (players34.cpp), else -1
}

bool BarVisible(uintptr_t bar)
{
    return ((ReadU32(bar + kFlags) >> 8) & 1) != 0;
}

void SaveBar(BarSave& s, uintptr_t bar)
{
    s.flags = ReadU32(bar + kFlags);
    s.x = ReadF32(bar + kPosX);
    s.y = ReadF32(bar + kPosY);
    s.colour = ReadU32(bar + kColour);
    for (int i = 0; i < 4; ++i)
        for (int k = 0; k < 4; ++k)
            s.vert[i][k] = ReadF32(bar + kVert[i] + (k < 2 ? k * 4 : 0xC + (k - 2) * 4));
}

void RestoreBar(const BarSave& s, uintptr_t bar)
{
    WriteU32(bar + kFlags, s.flags);
    WriteF32(bar + kPosX, s.x);
    WriteF32(bar + kPosY, s.y);
    WriteU32(bar + kColour, s.colour);
    for (int i = 0; i < 4; ++i)
        for (int k = 0; k < 4; ++k)
            WriteF32(bar + kVert[i] + (k < 2 ? k * 4 : 0xC + (k - 2) * 4), s.vert[i][k]);
}

// The bar must be a RedBitmapElement with the jet fuel art before it is used.
bool CheckArt(int p, uintptr_t bar)
{
    if (g_artChecked[p])
        return g_artOk[p];
    g_artChecked[p] = true;
    const bool cls = ReadU32(bar) == static_cast<uint32_t>(g_vtables[0]);
    const float u0 = ReadF32(bar + kUv0U);
    const float v0 = ReadF32(bar + kUv0V);
    const float u1 = ReadF32(bar + kUv3U);
    const float v1 = ReadF32(bar + kUv3V);
    const float w = ReadF32(bar + kQuad3X) - ReadF32(bar + kQuad0X);
    const float h = ReadF32(bar + kQuad3Y) - ReadF32(bar + kQuad0Y);
    g_artOk[p] = cls && Near(u0, kArtU0) && Near(v0, kArtV0) && Near(u1, kArtU1) && Near(v1, kArtV1) &&
                 Near(w, kArtW) && Near(h, kArtH);
    if (Budget())
        Log("[hud] %lu ms: player %d's grenade-panel bar (StatusDisplay +0x%04X): %s, uv %.3f %.3f .. "
            "%.3f %.3f, %.0f x %.0f - %s",
            ScanMsSinceAttach(), p + 1, static_cast<unsigned>(kGrenadeBar),
            cls ? "RedBitmapElement" : "NOT a RedBitmapElement", static_cast<double>(u0),
            static_cast<double>(v0), static_cast<double>(u1), static_cast<double>(v1),
            static_cast<double>(w), static_cast<double>(h),
            g_artOk[p] ? "the jet fuel bar art: used as the stamina bar"
                       : "NOT the expected jet fuel bar art: stamina bar disabled for this player");
    return g_artOk[p];
}

void DrawBar(int p, uintptr_t bar, float f, bool low)
{
    BarSave& s = g_save[p];
    SaveBar(s, bar);
    s.drawn = true;

    const float w = g_barW * f;
    const float u0 = kArtU0;
    const float uf = kArtU0 + (kArtU1 - kArtU0) * f;
    // corners: 0 = top-left, 1 = bottom-left, 2 = top-right, 3 = bottom-right
    const float corner[4][4] = {
        { 0.0f, 0.0f, u0, kArtV0 },
        { 0.0f, kArtH, u0, kArtV1 },
        { w, 0.0f, uf, kArtV0 },
        { w, kArtH, uf, kArtV1 },
    };
    for (int i = 0; i < 4; ++i)
        for (int k = 0; k < 4; ++k)
            WriteF32(bar + kVert[i] + (k < 2 ? k * 4 : 0xC + (k - 2) * 4), corner[i][k]);
    WriteF32(bar + kPosX, g_barX);
    WriteF32(bar + kPosY, g_barY);
    WriteU32(bar + kColour, low ? g_barLowColour : g_barColour);
    WriteU32(bar + kFlags, ReadU32(bar + kFlags) | 0x100u);
    ++g_drawn[p];
    if (!g_loggedDraw[p])
    {
        g_loggedDraw[p] = true;
        if (Budget())
            Log("[hud] %lu ms: player %d's stamina bar drawn for the first time: %.0f%% at %.1f, %.1f in the "
                "grenade panel, %.0f x %.0f, colour %08X",
                ScanMsSinceAttach(), p + 1, static_cast<double>(f * 100.0f), static_cast<double>(g_barX),
                static_cast<double>(g_barY), static_cast<double>(g_barW), static_cast<double>(kArtH),
                static_cast<unsigned>(g_barColour));
    }
}

bool __fastcall Hook_StatusUpdate(void* self, void* edx, float dt)
{
    const uintptr_t sd = reinterpret_cast<uintptr_t>(self);
    const int p = PlayerOf(sd);
    const uintptr_t bar = sd + kGrenadeBar;

    // 1. the game gets its bar back exactly as it left it
    if (p >= 0 && g_save[p].drawn)
    {
        RestoreBar(g_save[p], bar);
        g_save[p].drawn = false;
    }

    // 2. the game's own update
    const bool r = g_origUpdate(self, edx, dt);

    // 3. the stamina bar, if the game left the bar hidden
    if (p < 0 || !g_barOn)
        return r;
    float f = 0.0f;
    const int state = SprintHudState(p, &f);
    if (!state || !CheckArt(p, bar))
        return r;
    if (BarVisible(bar))
    {
        if (!g_gameUsing[p])
        {
            g_gameUsing[p] = true;
            if (Budget())
                Log("[hud] %lu ms: player %d: the game is using the grenade-panel bar for its own weapon - "
                    "the stamina bar waits until it is hidden again",
                    ScanMsSinceAttach(), p + 1);
        }
        return r;
    }
    g_gameUsing[p] = false;
    DrawBar(p, bar, f, state == 2);
    return r;
}

void InstallBar(uintptr_t rdata, uint32_t rdataLen, uintptr_t data, uint32_t dataLen)
{
    ReadBarIni();
    if (!g_barOn)
    {
        Log("[hud] BF1GameExt.ini [Sprint] StaminaBar=0: no stamina bar");
        return;
    }
    const uintptr_t vt = VtableByRtti("StatusDisplay", data, dataLen, rdata, rdataLen);
    if (!vt)
    {
        Log("[hud] StatusDisplay vtable not found - no stamina bar");
        return;
    }
    const uintptr_t slot = vt + kUpdateSlot * 4;
    uint32_t target = ReadU32(slot);
    for (int hop = 0; hop < 8 && target > ExeBase() &&
                      *reinterpret_cast<const unsigned char*>(target) == 0xE9;
         ++hop)
    {
        int32_t rel = 0;
        memcpy(&rel, reinterpret_cast<const void*>(target + 1), sizeof(rel));
        target = static_cast<uint32_t>(static_cast<int64_t>(target) + 5 + rel);
    }
    const BuildInfo* b = CurrentBuild();
    if (!b || target < ExeBase() || target >= ExeBase() + b->sizeOfImage ||
        !MatchPatternAt(static_cast<uint32_t>(target - ExeBase()), kStatusUpdateBody))
    {
        Log("[hud] StatusDisplay vtable rva 0x%08X: slot 1 does not lead to the update's code (0x%08X) - "
            "no stamina bar",
            static_cast<unsigned>(vt - ExeBase()), static_cast<unsigned>(target));
        return;
    }
    g_origUpdate = reinterpret_cast<StatusUpdate_t>(ReadU32(slot));
    DWORD oldProt = 0;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, PAGE_READWRITE, &oldProt))
    {
        Log("[hud] VirtualProtect on the StatusDisplay vtable failed (error %lu) - no stamina bar",
            GetLastError());
        return;
    }
    WriteU32(slot, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&Hook_StatusUpdate)));
    DWORD ignored = 0;
    VirtualProtect(reinterpret_cast<LPVOID>(slot), 4, oldProt, &ignored);
    Log("[hud] stamina bar ON: StatusDisplay::Update (vtable rva 0x%08X slot 1 -> rva 0x%08X, code "
        "checked) goes through BF1GameExt; bar = the grenade panel's jet-fuel-art bar at %.1f, %.1f, "
        "width %.0f, colour %08X (low %08X)",
        static_cast<unsigned>(vt - ExeBase()), static_cast<unsigned>(target - ExeBase()),
        static_cast<double>(g_barX), static_cast<double>(g_barY), static_cast<double>(g_barW),
        static_cast<unsigned>(g_barColour), static_cast<unsigned>(g_barLowColour));
}

} // namespace

void StaminaHudStart()
{
    Log("[hud] stamina_hud.cpp file build %s", kFileBuild);
    uint32_t hits[4] = {};
    const int n = FindPattern(kStatusArrayAlloc, hits, 4);
    if (n != 1)
    {
        Log("[hud] StatusDisplay allocation: %d pattern hits (need 1) - HUD scan disabled", n);
        return;
    }
    const uintptr_t start = ReadU32(Rva(hits[0] + kOffArrayStart));
    const uintptr_t end = ReadU32(Rva(hits[0] + kOffArrayEnd));
    const BuildInfo* b = CurrentBuild();
    if (end != start + 8 || !b || start < ExeBase() || end > ExeBase() + b->sizeOfImage)
    {
        Log("[hud] StatusDisplay array 0x%08X .. 0x%08X is not 2 pointers inside the exe - HUD scan "
            "disabled",
            static_cast<unsigned>(start), static_cast<unsigned>(end));
        return;
    }
    g_array = start;
    Log("[hud] StatusDisplay array at rva 0x%08X (allocation at rva 0x%08X)",
        static_cast<unsigned>(start - ExeBase()), hits[0]);

    uintptr_t data = 0;
    uintptr_t rdata = 0;
    uint32_t dataLen = 0;
    uint32_t rdataLen = 0;
    if (!Section(".data", &data, &dataLen) || !Section(".rdata", &rdata, &rdataLen))
    {
        Log("[hud] .data / .rdata not readable - HUD scan disabled");
        return;
    }
    Log("[hud] .data rva 0x%08X (0x%X bytes searched), .rdata rva 0x%08X (0x%X bytes)",
        static_cast<unsigned>(data - ExeBase()), static_cast<unsigned>(dataLen),
        static_cast<unsigned>(rdata - ExeBase()), static_cast<unsigned>(rdataLen));
    int found = 0;
    for (int i = 0; i < kClassCount; ++i)
    {
        g_vtables[i] = VtableByRtti(kClassNames[i], data, dataLen, rdata, rdataLen);
        if (g_vtables[i])
            ++found;
        Log("[hud]   %-24s vtable %s0x%08X", kClassNames[i], g_vtables[i] ? "rva " : "NOT FOUND ",
            static_cast<unsigned>(g_vtables[i] ? g_vtables[i] - ExeBase() : 0));
    }
    if (!g_vtables[0])
    {
        Log("[hud] RedBitmapElement not found - HUD scan disabled");
        return;
    }
    g_ready = true;
    Log("[hud] HUD scan ON: %d of %d element classes found; each player's StatusDisplay is listed "
        "3 s after a spawn (at most %d times per player)",
        found, kClassCount, kMaxDumpsPerPlayer);
    InstallBar(rdata, rdataLen, data, dataLen);
}

void StaminaHudFrame(int p, uintptr_t soldier)
{
    if (!g_ready || p < 0 || p > 1)
        return;
    const DWORD now = GetTickCount();
    if (soldier != g_lastSoldier[p])
    {
        g_lastSoldier[p] = soldier;
        if (g_dumps[p] < kMaxDumpsPerPlayer)
            g_due[p] = now + kDumpDelayMs;
    }
    if (!g_due[p] || static_cast<LONG>(now - g_due[p]) < 0)
        return;
    g_due[p] = 0;
    const uintptr_t sd = ReadU32(g_array + static_cast<uint32_t>(p) * 4);
    ++g_dumps[p];
    if (!sd)
    {
        if (Budget())
            Log("[hud] %lu ms: player %d has no StatusDisplay (slot %d is empty)", ScanMsSinceAttach(),
                p + 1, p);
        return;
    }
    Dump(p, sd);
}

} // namespace bf1
