// BF1GameExt - spawn_manager.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/spawn/ (was spawn_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// 18a: SpawnScanManager (the SpawnManager itself, for class_wheel.cpp) and
// SpawnScanFileBuild. Nothing else changed.
// 17d: SpawnScanCharacters - the whole character list in one call (the death
// tracker read it one character at a time: two slow memory checks each).
// Phase 7a (2020 build). Read-only: watches the characters of player 1 and
// player 2 and logs what changes, as [chars] lines.
//
// Found by pattern (proven on GOG 1.3, the same code as Steam):
//   Team::AddCharacter(name, index):
//     push ebp; mov ebp, esp; push edi; mov edi, ecx;
//     mov eax, [edi+2Ch]; cmp eax, [edi+28h]; jge ...; push esi;
//     call GetSpawnManager          (offset 15)  "mov eax, [<global>]; ret"
//     mov ecx, eax; call Allocate   (offset 22)  count at +38h, array at +3Ch,
//                                                0E8h bytes per character
//     ...; call Character::Init     (offset 40)  [+0A0h] = team,
//                                                [+9Ch] = index, name at +1Ch

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"
#include "spawn_manager.hpp"

namespace bf1 {
namespace {

const char* const kAddCharacter =
    "55 8B EC 57 8B F9 8B 47 2C 3B 47 28 7D 2C 56 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? FF 75 0C 8B "
    "F0 FF 75 08 8B CE FF 77 08 E8 ?? ?? ?? ?? 8B 4F 2C 8B 47 34 89 34 88 FF 47 2C";
const char* const kGetSpawnManagerBody = "A1 ?? ?? ?? ?? C3";
const char* const kAllocateBody = "8B 51 38 8D 42 01 89 41 38 69 C2 E8 00 00 00 03 41 3C C3";
const char* const kCharacterInitBody =
    "55 8B EC 8B 45 08 56 8B F1 68 80 00 00 00 6A 00 89 86 A0 00 00 00 8B 45 10 89 86 9C 00 00 00";

constexpr uint32_t kCountOffset = 0x38;
constexpr uint32_t kArrayOffset = 0x3C;
constexpr uint32_t kCharSize = 0xE8;
constexpr uint32_t kNameOffset = 0x1C;
constexpr uint32_t kNameBytes = 0x80;
constexpr uint32_t kIndexOffset = 0x9C;
constexpr uint32_t kTeamOffset = 0xA0;
constexpr int kWatched = 2;              // characters 0 and 1
constexpr int kDwords = kCharSize / 4;
constexpr int kMuteAfter = 12;           // changes per field before it is muted
constexpr int kMaxLines = 700;
constexpr DWORD kPollMs = 100;

uintptr_t g_smGlobal = 0;  // address of the SpawnManager pointer

int g_lines = 0;

bool Budget()
{
    if (g_lines >= kMaxLines)
        return false;
    ++g_lines;
    if (g_lines == kMaxLines)
        Log("[chars] line limit reached, spawn scan going quiet");
    return true;
}

uint32_t ReadU32(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

int32_t ReadI32At(uint32_t rva)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(Rva(rva)), sizeof(v));
    return v;
}

uint32_t CallTarget(uint32_t rva)
{
    uint32_t t = static_cast<uint32_t>(static_cast<int64_t>(rva) + 5 + ReadI32At(rva + 1));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(Rva(t)) == 0xE9; ++hop)
        t = static_cast<uint32_t>(static_cast<int64_t>(t) + 5 + ReadI32At(t + 1));
    return t;
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

bool Check(const char* what, uint32_t rva, const char* body)
{
    if (rva && MatchPatternAt(rva, body))
    {
        Log("[chars] %-22s rva 0x%08X  OK", what, rva);
        return true;
    }
    Log("[chars] %-22s rva 0x%08X  code does NOT match - spawn scan disabled", what, rva);
    return false;
}

// Names are stored as UTF-16 (the code compares them a WORD at a time).
void ReadName(uintptr_t ch, char* out, size_t outSize)
{
    wchar_t w[kNameBytes / 2 + 1] = {};
    memcpy(w, reinterpret_cast<const void*>(ch + kNameOffset), kNameBytes);
    w[kNameBytes / 2] = 0;
    out[0] = 0;
    if (WideCharToMultiByte(CP_UTF8, 0, w, -1, out, static_cast<int>(outSize), nullptr, nullptr) <= 0)
        out[0] = 0;
}

struct Watch
{
    bool have;
    uint32_t prev[kDwords];
    int changes[kDwords];
};

Watch g_watch[kWatched];
uintptr_t g_lastArray = 0;
uint32_t g_lastCount = 0;

void LogRoster(uintptr_t array, uint32_t count)
{
    const uint32_t shown = count < 8 ? count : 8;
    for (uint32_t k = 0; k < shown; ++k)
    {
        const uintptr_t ch = array + k * kCharSize;
        if (!Readable(ch, kCharSize) || !Budget())
            continue;
        char name[96];
        ReadName(ch, name, sizeof(name));
        Log("[chars]   character %u: index %ld, team %ld, name \"%s\"", static_cast<unsigned>(k),
            static_cast<long>(static_cast<int32_t>(ReadU32(ch + kIndexOffset))),
            static_cast<long>(static_cast<int32_t>(ReadU32(ch + kTeamOffset))), name);
    }
}

void DumpCharacter(int k, uintptr_t ch)
{
    if (!Budget())
        return;
    Log("[chars] %lu ms: character %d (%s) full dump, 0x%X bytes:", ScanMsSinceAttach(), k,
        k == 0 ? "player 1" : "player 2", static_cast<unsigned>(kCharSize));
    for (uint32_t off = 0; off < kCharSize; off += 16)
    {
        if (!Budget())
            return;
        char line[160];
        int pos = snprintf(line, sizeof(line), "[chars]     +%03X:", static_cast<unsigned>(off));
        for (uint32_t i = 0; i < 16 && off + i < kCharSize; i += 4)
            pos += snprintf(line + pos, sizeof(line) - static_cast<size_t>(pos), " %08X",
                            static_cast<unsigned>(ReadU32(ch + off + i)));
        Log("%s", line);
    }
}

void WatchCharacter(int k, uintptr_t ch)
{
    Watch& w = g_watch[k];
    uint32_t now[kDwords];
    memcpy(now, reinterpret_cast<const void*>(ch), sizeof(now));
    if (!w.have)
    {
        DumpCharacter(k, ch);
        memcpy(w.prev, now, sizeof(now));
        memset(w.changes, 0, sizeof(w.changes));
        w.have = true;
        return;
    }
    for (int i = 0; i < kDwords; ++i)
    {
        if (now[i] == w.prev[i])
            continue;
        ++w.changes[i];
        if (w.changes[i] <= kMuteAfter && Budget())
        {
            float f = 0.0f;
            memcpy(&f, &now[i], sizeof(f));
            Log("[chars] %lu ms: %s +%03X: %08X -> %08X (as float %.3f)%s", ScanMsSinceAttach(),
                k == 0 ? "player 1" : "player 2", static_cast<unsigned>(i * 4),
                static_cast<unsigned>(w.prev[i]), static_cast<unsigned>(now[i]),
                static_cast<double>(f),
                w.changes[i] == kMuteAfter ? "  [changes a lot - muted from here]" : "");
        }
    }
    memcpy(w.prev, now, sizeof(now));
}

DWORD WINAPI SpawnScanThread(LPVOID)
{
    bool waitingLogged = false;
    for (;;)
    {
        Sleep(kPollMs);
        const uintptr_t sm = ReadU32(g_smGlobal);
        if (!sm || !Readable(sm, kArrayOffset + 4))
        {
            if (!waitingLogged && Budget())
            {
                Log("[chars] %lu ms: no SpawnManager yet (it exists during a match)",
                    ScanMsSinceAttach());
                waitingLogged = true;
            }
            g_lastArray = 0;
            continue;
        }
        waitingLogged = false;

        const uint32_t count = ReadU32(sm + kCountOffset);
        const uintptr_t array = ReadU32(sm + kArrayOffset);
        if (array != g_lastArray || count != g_lastCount)
        {
            if (Budget())
                Log("[chars] %lu ms: SpawnManager 0x%08X: %lu characters, array 0x%08X",
                    ScanMsSinceAttach(), static_cast<unsigned>(sm), static_cast<unsigned long>(count),
                    static_cast<unsigned>(array));
            if (array != g_lastArray)
                memset(g_watch, 0, sizeof(g_watch));
            if (array && count)
                LogRoster(array, count);
            g_lastArray = array;
            g_lastCount = count;
        }
        if (!array)
            continue;
        for (int k = 0; k < kWatched && static_cast<uint32_t>(k) < count; ++k)
        {
            const uintptr_t ch = array + static_cast<uint32_t>(k) * kCharSize;
            if (Readable(ch, kCharSize))
                WatchCharacter(k, ch);
        }
    }
}

} // namespace

uintptr_t SpawnScanCharacter(int k)
{
    if (!g_smGlobal || k < 0)
        return 0;
    const uintptr_t sm = ReadU32(g_smGlobal);
    if (!sm || !Readable(sm, kArrayOffset + 4))
        return 0;
    const uint32_t count = ReadU32(sm + kCountOffset);
    const uintptr_t array = ReadU32(sm + kArrayOffset);
    if (!array || static_cast<uint32_t>(k) >= count)
        return 0;
    const uintptr_t ch = array + static_cast<uint32_t>(k) * kCharSize;
    return Readable(ch, kCharSize) ? ch : 0;
}

// 18a: the SpawnManager itself (0 outside a match)
uintptr_t SpawnScanManager()
{
    if (!g_smGlobal)
        return 0;
    const uintptr_t sm = ReadU32(g_smGlobal);
    if (!sm || !Readable(sm, kArrayOffset + 4))
        return 0;
    return sm;
}

const char* SpawnScanFileBuild()
{
    return "p23a-r1";
}

// 17d: the character list (start, count) in one call
bool SpawnScanCharacters(uintptr_t* array, uint32_t* count)
{
    *array = 0;
    *count = 0;
    if (!g_smGlobal)
        return false;
    const uintptr_t sm = ReadU32(g_smGlobal);
    if (!sm || !Readable(sm, kArrayOffset + 4))
        return false;
    const uint32_t n = ReadU32(sm + kCountOffset);
    const uintptr_t a = ReadU32(sm + kArrayOffset);
    if (!a || !n || n > 1024)
        return false;
    *array = a;
    *count = n;
    return true;
}

void SpawnScanStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[chars] not used on this build");
        return;
    }

    uint32_t hits[4] = {};
    const int n = FindPattern(kAddCharacter, hits, 4);
    if (n != 1)
    {
        Log("[chars] Team::AddCharacter: %d pattern hits (need exactly 1) - spawn scan disabled", n);
        return;
    }
    const uint32_t add = hits[0];
    Log("[chars] Team::AddCharacter       rva 0x%08X  OK", add);
    const uint32_t getSm = CallTarget(add + 15);
    if (!Check("GetSpawnManager", getSm, kGetSpawnManagerBody) ||
        !Check("character allocator", CallTarget(add + 22), kAllocateBody) ||
        !Check("Character::Init", CallTarget(add + 40), kCharacterInitBody))
        return;

    const uintptr_t global = ReadU32(Rva(getSm + 1));
    if (global < ExeBase() || global >= ExeBase() + build->sizeOfImage)
    {
        Log("[chars] the SpawnManager pointer is outside the exe - spawn scan disabled");
        return;
    }
    g_smGlobal = global;
    Log("[chars] SpawnManager pointer at rva 0x%08X", static_cast<unsigned>(global - ExeBase()));

    const HANDLE t = CreateThread(nullptr, 0, SpawnScanThread, nullptr, 0, nullptr);
    if (t)
    {
        CloseHandle(t);
        Log("[chars] spawn scan started, polling every %lu ms", static_cast<unsigned long>(kPollMs));
    }
    else
    {
        Log("[chars] thread failed to start (error %lu)", GetLastError());
    }
}

} // namespace bf1
