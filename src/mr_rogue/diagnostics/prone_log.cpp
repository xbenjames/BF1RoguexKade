// BF1GameExt - prone_log.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was prone_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 10a (2020 build): read-only, every frame on the game thread.
//
// Player 1's character (SpawnManager character 0) holds its unit at +0xB4;
// that is the controllable PlayerController drives (checked: its +0xD4 is the
// character index, 0). PlayerController::Update copies the control-state bits
// into the controllable's buttons at +0x38 .. +0x4F:
//   +0x38 bit0   +0x3A bit1   +0x3C bit6   +0x3E bit2   +0x40 bit3   +0x42 bit7
//   +0x44 bit4   +0x46 bit5   +0x48 bit10  +0x4A bit11  +0x4C bit9   +0x4E bit8
// Each is { signed counter, event } as the game's ButtonState update keeps it.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "prone_log.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

namespace bf1 {
namespace {

constexpr uint32_t kUnit = 0xB4;            // Character -> unit (controllable)
constexpr uint32_t kCharacterIndex = 0xD4;  // controllable -> character index
constexpr uint32_t kButtons = 0x38;
constexpr int kButtonCount = 12;
const char* const kBitName[kButtonCount] = {
    "bit0 (+0x38)", "bit1 (+0x3A)", "bit6 (+0x3C)",  "bit2 (+0x3E)",
    "bit3 (+0x40)", "bit7 (+0x42)", "bit4 (+0x44)",  "bit5 (+0x46)",
    "bit10 (+0x48)", "bit11 (+0x4A)", "bit9 (+0x4C)", "bit8 (+0x4E)",
};
constexpr int kMaxLines = 700;

int g_lines = 0;
uintptr_t g_lastUnit = 0;
bool g_badUnitLogged = false;
int8_t g_prevCounter[kButtonCount] = {};
uint8_t g_prevEvent[kButtonCount] = {};
bool g_havePrev = false;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[prone] line limit reached, going quiet");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

bool Readable(uintptr_t start, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (!start || !VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    const DWORD prot = mbi.Protect & 0xFF;
    if (prot == 0 || prot == PAGE_EXECUTE)
        return false;
    return start + len <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

const char* EventName(uint8_t e)
{
    switch (e)
    {
    case 0: return "none";
    case 1: return "TAP";
    case 2: return "DOUBLE-TAP";
    case 3: return "long hold";
    default: return "?";
    }
}

} // namespace

void ProneScanFrame()
{
    const uintptr_t ch = SpawnScanCharacter(0);
    const uintptr_t unit = ch ? ReadU32(ch + kUnit) : 0;
    if (!unit || !Readable(unit + kButtons, kButtonCount * 2) ||
        !Readable(unit + kCharacterIndex, 4))
    {
        g_lastUnit = 0;
        g_havePrev = false;
        return;
    }
    if (unit != g_lastUnit)
    {
        const int32_t idx = static_cast<int32_t>(ReadU32(unit + kCharacterIndex));
        if (idx != 0)
        {
            if (!g_badUnitLogged && Budget())
            {
                Log("[prone] %lu ms: player 1's unit 0x%08X has character index %ld at +0xD4, not "
                    "0 - not the controllable, scan waits",
                    ScanMsSinceAttach(), static_cast<unsigned>(unit), static_cast<long>(idx));
                g_badUnitLogged = true;
            }
            return;
        }
        if (Budget())
            Log("[prone] %lu ms: watching player 1's buttons (controllable 0x%08X, character index "
                "0 at +0xD4 OK)",
                ScanMsSinceAttach(), static_cast<unsigned>(unit));
        g_lastUnit = unit;
        g_havePrev = false;
    }

    int8_t counter[kButtonCount];
    uint8_t event[kButtonCount];
    for (int i = 0; i < kButtonCount; ++i)
    {
        const unsigned char* b = reinterpret_cast<const unsigned char*>(unit + kButtons + i * 2);
        counter[i] = static_cast<int8_t>(b[0]);
        event[i] = b[1];
    }

    if (g_havePrev)
    {
        for (int i = 0; i < kButtonCount; ++i)
        {
            const int8_t c0 = g_prevCounter[i];
            const int8_t c1 = counter[i];
            const uint8_t e0 = g_prevEvent[i];
            const uint8_t e1 = event[i];
            const bool pressed = c0 <= 0 && c1 > 0;
            const bool released = c0 > 0 && c1 <= 0;
            if (pressed)
            {
                if (Budget())
                    Log("[prone] %lu ms: %s PRESSED (%d frames since its last release%s) -> event %s",
                        ScanMsSinceAttach(), kBitName[i], c0 < 0 ? -c0 : 0,
                        c0 <= -100 ? ", the counter's floor: it never resets" : "",
                        EventName(e1));
            }
            else if (released)
            {
                if (Budget())
                    Log("[prone] %lu ms: %s released after %d frames -> event %s",
                        ScanMsSinceAttach(), kBitName[i], c0, EventName(e1));
            }
            else if (e1 != e0)
            {
                if (Budget())
                    Log("[prone] %lu ms: %s event changed by the game: %s -> %s (counter %d)",
                        ScanMsSinceAttach(), kBitName[i], EventName(e0), EventName(e1), c1);
            }
        }
    }
    memcpy(g_prevCounter, counter, sizeof(counter));
    memcpy(g_prevEvent, event, sizeof(event));
    g_havePrev = true;
}

} // namespace bf1
