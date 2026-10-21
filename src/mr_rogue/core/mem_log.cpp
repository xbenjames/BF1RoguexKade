// BF1GameExt - mem_log.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// 22d: the [mem] lines and the crash report add the texture cap's totals (tex_cap.cpp). Nothing else changed.
//
// Phase 22c: THE MEMORY LOG ([mem] lines; BF1GameExt.ini [Limits] MemoryLog = seconds between lines, 0 = off). Log only.
//
// Why: 22b (EGM's patches) on EGM's own maps (abes1c, abeach1a) drew the world untextured and crashed twice, both times
// writing through address 0 after an allocation came back empty:
//   - mid-match, the moment player 1 died (the session's first ragdoll): Bullet asked Windows for 3.6 MB (its contact pool,
//     4096 x 880 bytes + 19 - exactly the crash's ECX 0x370013), malloc gave 0, Bullet's default allocator cleared it anyway
//     (DINPUT8.dll+0x10D67C, called from Bullet's btAllocDefault). A 3.6 MB malloc only fails when the game's 2 GB address
//     space (the exe is not large-address aware) is full or chopped up;
//   - loading abeach1a: the game's async reader (its loader thread) copied map data into a buffer that was never allocated.
// EGM's launcher runs the game with the 4 GB flag set (it refuses an exe without it) and a 512 MB main heap; BF1GameExt has
// the 2 GB the exe gives and [Limits]' 128 MB. This log shows which memory runs out, when, and who asked:
//   - the address space (VirtualQuery over all of it): free, the largest free block (what the biggest allocation can get),
//     what is in use (exe and DLLs, private, mapped, reserved) - sampled every second on a thread of its own, the lowest
//     values kept between lines;
//   - the game's heaps (its own allocator's table, found by pattern: Application, Gamespy, Debug, Lua, Runtime, Temp):
//     free, largest free block, the lowest free between lines (free lists read with every block checked inside its heap,
//     so a list changing under the reader cannot make it read outside);
//   - every failed game allocation, as it happens: the game's allocator (Steam rva 0x00284400, GOG 1.3 0x0027DA30) calls a
//     function pointer when a heap has no block big enough (Steam rva 0x006E55D4, GOG 1.3 0x006A3584) - neither exe ever
//     sets it (its setter has no callers), so BF1GameExt's goes there: the heap, the size asked, who asked (the allocator's
//     caller and that one's caller), the heap's state, the address space;
//   - Direct3D resources made since the last line (render_hooks.cpp's create wrappers: textures by pool - a managed texture
//     keeps a copy in the game's address space - and buffers) and every failed create (its size, format, pool, caller);
//   - at a crash (crashlog.cpp): the address space, the heaps and the failure counts.
// Checked: the patterns on both exes (each address operand relocated; the table the allocator reads = the one heap create
// fills), and the failure path in Unicorn on both exes (22c sim): the callback sees the allocator's frame exactly as read
// here - its return into the allocator at +8Ch, then the allocator's saved edi / esi / ebx, its local, saved ebp, its
// caller, the heap and the size.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "log.hpp"
#include "mem_log.hpp"
#include "split_spawn_menus.hpp"
#include "game_check.hpp"
#include "tex_cap.hpp"

#ifdef _MSC_VER
extern "C" void* _AddressOfReturnAddress(void);
#pragma intrinsic(_AddressOfReturnAddress)
#define BF1_RETURN_SLOT() _AddressOfReturnAddress()
#else
#define BF1_RETURN_SLOT() (static_cast<void*>(static_cast<char*>(__builtin_frame_address(0)) + sizeof(void*)))
#endif

namespace bf1 {
namespace {

// ---- the game's heap allocator and heap table (patterns checked on GOG 1.3 and the decrypted Steam exe) -----------------
// alloc(heap, size): Steam rva 0x00284400, GOG 1.3 0x0027DA30
const char* const kAllocBody =
    "55 8B EC 51 80 3D ?? ?? ?? ?? 00 75 05 E8 ?? ?? ?? ?? 8B 45 08 53 56 57 8D 0C C5 00 00 00 00 2B C8 A1 ?? ?? ?? ??";
constexpr uint32_t kAllocTable = 0x22; // mov eax, [heap table]
// its failure path: heap start < 70000000h -> (unless the game expects it to fail) the record, then call [callback]
constexpr uint32_t kAllocFailAt = 0x62;
const char* const kAllocFail = "81 3B 00 00 00 70 73 22 83 3D ?? ?? ?? ?? 00 7F 0E 8B 45 08 A3 ?? ?? ?? ?? 89 3D ?? ?? ?? ?? A1 "
                               "?? ?? ?? ?? 85 C0 74 02 FF D0 5F 5E 33 C0";
constexpr uint32_t kAllocSuppress = 0x6C; // cmp [the "may fail" counter], 0
constexpr uint32_t kAllocRecord = 0x77;   // mov [failed heap], eax
constexpr uint32_t kAllocRecord4 = 0x7D;  // mov [failed size], edi
constexpr uint32_t kAllocCallback = 0x82; // mov eax, [callback]
constexpr uint32_t kAllocCallRet = 0x8C;  // right after call eax
// create(start, end, align, name): Steam rva 0x00284550, GOG 1.3 0x0027DB80 - the heap count, the table, the entry layout
const char* const kCreateBody =
    "55 8B EC 53 8B 5D 10 56 57 8D 73 FF F7 D6 8D 7B FF 03 7D 08 23 FE 23 75 0C 2B F7 74 0C 56 6A 00 57 E8 ?? ?? ?? ?? 83 C4 0C "
    "8B 15 ?? ?? ?? ?? 33 C0 85 D2 7E 18 8B 0D ?? ?? ?? ?? 83 C1 0C";
constexpr uint32_t kCreateCount = 0x2B;
constexpr uint32_t kCreateTable = 0x37;
constexpr uint32_t kCreateSetAt = 0x55;
const char* const kCreateSet = "8B 15 ?? ?? ?? ?? 8D 0C C5 00 00 00 00 89 37 2B C8 8D 14 8A C7 47 08 00 00 00 00 8B 4D 14 C7 47 04 "
                               "00 00 00 00 89 47 0C 89 3A 89 7A 04 5F 89 72 0C 5E 89 5A 14 C7 42 08 00 00 00 00 89 4A 10 C6 42 18 00";
// a heap: +00h its start, +04h the first free block, +0Ch its size, +10h its name, +14h alignment; 1Ch bytes
constexpr uint32_t kEntry = 0x1C;
constexpr uint32_t kEntStart = 0x00;
constexpr uint32_t kEntFree = 0x04;
constexpr uint32_t kEntSize = 0x0C;
constexpr uint32_t kEntName = 0x10;
// a block: +00h its size, +04h previous, +08h next, +0Ch its heap
constexpr uint32_t kBlkSize = 0x00;
constexpr uint32_t kBlkNext = 0x08;
constexpr int kMaxHeaps = 16;
constexpr int kWalkMax = 8192;

constexpr LONG kFailLines = 40;   // failed game allocations logged one by one
constexpr LONG kFailVaLines = 12; // ... with the address space
constexpr LONG kD3dFailLines = 30;
constexpr uint32_t kMB = 1024u * 1024u;

int g_intervalS = 30;
bool g_on = false;
uint32_t g_allocRva = 0;
uintptr_t g_callRet = 0;   // the allocator's address right after its call [callback]
uintptr_t g_tableVa = 0;   // -> the heap table
uintptr_t g_countVa = 0;   // the heap count
uintptr_t g_recordVa = 0;  // the game's last failed allocation: heap, size
uintptr_t g_callbackVa = 0;
uintptr_t g_suppressVa = 0;
bool g_callbackOurs = false;

volatile LONG g_fails = 0;
volatile LONG g_failsExpected = 0;
volatile LONG g_failHeap[kMaxHeaps] = {};
volatile LONG g_failLargest = 0;
volatile LONG g_failLastMs = 0;

// Direct3D: made by kind, KB by pool (all kinds), failures
const char* const kD3dKind[5] = { "CreateTexture", "CreateVolumeTexture", "CreateCubeTexture", "CreateVertexBuffer",
                                  "CreateIndexBuffer" };
volatile LONG g_d3dMade[5] = {};
volatile LONG g_d3dKb[4] = {};      // default, managed, system memory, scratch
volatile LONG g_d3dKbTex = 0;       // textures only (all pools)
volatile LONG g_d3dFails = 0;

uint32_t Rd(uintptr_t va)
{
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

bool InExe(uintptr_t va)
{
    const BuildInfo* b = CurrentBuild();
    return b && va >= ExeBase() && va < ExeBase() + b->sizeOfImage;
}

bool IsSteam()
{
    const BuildInfo* b = CurrentBuild();
    return b && !strncmp(b->name, "Steam", 5);
}

bool IsGog13()
{
    const BuildInfo* b = CurrentBuild();
    return b && !strncmp(b->name, "GOG 1.3", 7);
}

// committed and readable; *allocBase = the allocation it is part of
bool Committed(uintptr_t a, uintptr_t* allocBase)
{
    MEMORY_BASIC_INFORMATION m;
    if (!VirtualQuery(reinterpret_cast<LPCVOID>(a), &m, sizeof(m)))
        return false;
    if (m.State != MEM_COMMIT || (m.Protect & (PAGE_GUARD | PAGE_NOACCESS)) || !(m.Protect & 0xFF) || m.Protect == PAGE_EXECUTE)
        return false;
    if (allocBase)
        *allocBase = reinterpret_cast<uintptr_t>(m.AllocationBase);
    return true;
}

// [a, a + n) inside this thread's stack
bool OnStack(uintptr_t a, size_t n)
{
    const NT_TIB* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    const uintptr_t lo = reinterpret_cast<uintptr_t>(tib->StackLimit);
    const uintptr_t hi = reinterpret_cast<uintptr_t>(tib->StackBase);
    return a >= lo && a + n <= hi && a + n > a;
}

// "exe+0x001D50CB", "BF1GameExt.dll+0x1094", "0x12345678"
const char* CodeText(uintptr_t a, char* out, size_t n)
{
    if (!a)
        snprintf(out, n, "?");
    else if (InExe(a))
        snprintf(out, n, "exe+0x%06X", static_cast<unsigned>(a - ExeBase()));
    else
    {
        HMODULE hm = nullptr;
        char path[MAX_PATH] = {};
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCSTR>(a), &hm) &&
            hm && GetModuleFileNameA(hm, path, MAX_PATH))
        {
            const char* name = strrchr(path, '\\');
            snprintf(out, n, "%s+0x%X", name ? name + 1 : path, static_cast<unsigned>(a - reinterpret_cast<uintptr_t>(hm)));
        }
        else
            snprintf(out, n, "0x%08X", static_cast<unsigned>(a));
    }
    return out;
}

// ---- the address space ---------------------------------------------------------------------------------------------------
struct Va
{
    uint64_t top;      // the highest address + 1 (2 GB without the large-address flag)
    uint64_t freeB;    // free, in whole 64 KB allocation units
    uint64_t largest;  // the largest free block (usable part)
    uint64_t image;    // committed: exe and DLLs
    uint64_t priv;     // committed: private (heaps, VirtualAlloc, Direct3D's system copies)
    uint64_t mapped;   // committed: mapped files / sections
    uint64_t reserved; // reserved, not committed
    int regions;
};

void VaSnap(Va* v)
{
    memset(v, 0, sizeof(*v));
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    uintptr_t a = reinterpret_cast<uintptr_t>(si.lpMinimumApplicationAddress);
    const uintptr_t end = reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress);
    v->top = static_cast<uint64_t>(end) + 1;
    MEMORY_BASIC_INFORMATION m;
    while (a < end && v->regions < 400000)
    {
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(a), &m, sizeof(m)))
            break;
        const uintptr_t rb = reinterpret_cast<uintptr_t>(m.BaseAddress);
        const uintptr_t rs = m.RegionSize;
        ++v->regions;
        if (m.State == MEM_FREE)
        {
            const uintptr_t lo = (rb + 0xFFFF) & ~static_cast<uintptr_t>(0xFFFF);
            const uintptr_t hi = (rb + rs) & ~static_cast<uintptr_t>(0xFFFF);
            if (hi > lo)
            {
                v->freeB += hi - lo;
                if (hi - lo > v->largest)
                    v->largest = hi - lo;
            }
        }
        else if (m.State == MEM_RESERVE)
            v->reserved += rs;
        else if (m.Type == MEM_IMAGE)
            v->image += rs;
        else if (m.Type == MEM_MAPPED)
            v->mapped += rs;
        else
            v->priv += rs;
        if (rb + rs <= a)
            break;
        a = rb + rs;
    }
}

unsigned long Mb(uint64_t b)
{
    return static_cast<unsigned long>(b / kMB);
}

const char* VaText(const Va& v, char* out, size_t n)
{
    snprintf(out, n,
             "address space %lu MB: %lu MB free, the largest free block %lu MB; in use: exe and DLLs %lu MB, private %lu MB, "
             "mapped %lu MB, reserved %lu MB (%d regions)",
             Mb(v.top), Mb(v.freeB), Mb(v.largest), Mb(v.image), Mb(v.priv), Mb(v.mapped), Mb(v.reserved), v.regions);
    return out;
}

// ---- the game's heaps ----------------------------------------------------------------------------------------------------
struct HeapInfo
{
    char name[16];
    uint32_t size;
    uint32_t freeB;
    uint32_t largest;
    int blocks; // -1: its range or its list was not readable (the list moved, or the heap was being made again)
};

void NameAt(uintptr_t p, char* out, size_t n)
{
    size_t i = 0;
    if (p && InExe(p))
    {
        for (; i + 1 < n; ++i)
        {
            const char c = *reinterpret_cast<const char*>(p + i);
            if (!c)
                break;
            out[i] = (c > 32 && c < 127) ? c : '_';
        }
    }
    if (!i)
        out[i++] = '?';
    out[i] = 0;
}

// heaps 0 .. (returned - 1) into out[] by their index (size 0: an empty slot)
int ReadHeaps(HeapInfo* out, int max)
{
    if (!g_tableVa || !g_countVa)
        return 0;
    const uintptr_t table = Rd(g_tableVa);
    int count = static_cast<int>(Rd(g_countVa));
    if (!table || count <= 0)
        return 0;
    if (count > max)
        count = max;
    uintptr_t tb = 0;
    uintptr_t te = 0;
    if (!Committed(table, &tb) || !Committed(table + static_cast<uint32_t>(count) * kEntry - 1, &te) || tb != te)
        return 0;
    int last = 0;
    for (int i = 0; i < count; ++i)
    {
        HeapInfo& h = out[i];
        memset(&h, 0, sizeof(h));
        const uintptr_t e = table + static_cast<uint32_t>(i) * kEntry;
        h.size = Rd(e + kEntSize);
        if (!h.size)
            continue;
        last = i + 1;
        NameAt(Rd(e + kEntName), h.name, sizeof(h.name));
        const uintptr_t start = Rd(e + kEntStart);
        const uintptr_t end = start + h.size;
        uintptr_t sb = 0;
        uintptr_t eb = 0;
        if (end <= start || !Committed(start, &sb) || !Committed(end - 1, &eb) || sb != eb)
        {
            h.blocks = -1;
            continue;
        }
        uintptr_t b = Rd(e + kEntFree);
        int k = 0;
        while (b && k < kWalkMax)
        {
            if (b < start || b + 0x10 > end || (b & 3))
            {
                k = -1; // a block outside its heap: the list changed while it was read
                break;
            }
            const uint32_t sz = Rd(b + kBlkSize);
            h.freeB += sz;
            if (sz > h.largest)
                h.largest = sz;
            ++k;
            b = Rd(b + kBlkNext);
        }
        h.blocks = k;
    }
    return last;
}

const char* HeapsText(const HeapInfo* h, int n, const uint32_t* lowFree, char* out, size_t outSize)
{
    int pos = 0;
    out[0] = 0;
    for (int i = 0; i < n && pos >= 0 && pos < static_cast<int>(outSize) - 80; ++i)
    {
        if (!h[i].size)
            continue;
        if (h[i].blocks < 0)
            pos += snprintf(out + pos, outSize - static_cast<size_t>(pos), "%s%d %s (%lu KB): not readable just then",
                            pos ? ", " : "", i, h[i].name, static_cast<unsigned long>(h[i].size / 1024));
        else if (lowFree && lowFree[i] != 0xFFFFFFFFu)
            pos += snprintf(out + pos, outSize - static_cast<size_t>(pos), "%s%d %s %lu of %lu (%lu; %lu)", pos ? ", " : "", i,
                            h[i].name, static_cast<unsigned long>(h[i].freeB / 1024), static_cast<unsigned long>(h[i].size / 1024),
                            static_cast<unsigned long>(h[i].largest / 1024), static_cast<unsigned long>(lowFree[i] / 1024));
        else
            pos += snprintf(out + pos, outSize - static_cast<size_t>(pos), "%s%d %s %lu of %lu (%lu)", pos ? ", " : "", i,
                            h[i].name, static_cast<unsigned long>(h[i].freeB / 1024), static_cast<unsigned long>(h[i].size / 1024),
                            static_cast<unsigned long>(h[i].largest / 1024));
    }
    if (!out[0])
        snprintf(out, outSize, "not readable");
    return out;
}

const char* FailsText(char* out, size_t n)
{
    snprintf(out, n, "the game's failed allocations so far: %ld (%ld it expected might fail)%s", static_cast<long>(g_fails),
             static_cast<long>(g_failsExpected), g_callbackOurs ? "" : " - NOT watched (the callback is not in)");
    if (g_fails)
    {
        const size_t used = strlen(out);
        int pos = static_cast<int>(used);
        pos += snprintf(out + pos, n - used, ", the largest asked %lu bytes, the last at %lu ms; by heap:",
                        static_cast<unsigned long>(g_failLargest), static_cast<unsigned long>(g_failLastMs));
        for (int i = 0; i < kMaxHeaps && pos > 0 && pos < static_cast<int>(n) - 16; ++i)
            if (g_failHeap[i])
                pos += snprintf(out + pos, n - static_cast<size_t>(pos), " %d: %ld", i, static_cast<long>(g_failHeap[i]));
    }
    return out;
}

// ---- the game's allocation failure callback (the failing thread, inside the game's allocator) ---------------------------
// The allocator's frame (22c sim, both exes): p[0] = its return into the allocator (+8Ch), p[1..3] its saved edi / esi /
// ebx, p[4] its local, p[5] its saved ebp, p[6] its caller, p[7] the heap, p[8] the size asked.
void GameAllocFailed()
{
    const uint32_t* p = static_cast<const uint32_t*>(BF1_RETURN_SLOT());
    int heap = -1;
    uint32_t size = 0;
    uintptr_t from = 0;
    uintptr_t from2 = 0;
    if (p && OnStack(reinterpret_cast<uintptr_t>(p), 9 * 4) && p[0] == g_callRet)
    {
        heap = static_cast<int32_t>(p[7]);
        size = p[8];
        from = p[6];
        const uintptr_t up = p[5];
        if (OnStack(up, 8))
            from2 = Rd(up + 4);
    }
    const bool expected = g_suppressVa && static_cast<int32_t>(Rd(g_suppressVa)) > 0;
    const LONG n = InterlockedIncrement(&g_fails);
    if (expected)
        InterlockedIncrement(&g_failsExpected);
    if (heap >= 0 && heap < kMaxHeaps)
        InterlockedIncrement(&g_failHeap[heap]);
    for (;;)
    {
        const LONG cur = g_failLargest;
        if (static_cast<uint32_t>(cur) >= size || InterlockedCompareExchange(&g_failLargest, static_cast<LONG>(size), cur) == cur)
            break;
    }
    InterlockedExchange(&g_failLastMs, static_cast<LONG>(ScanMsSinceAttach()));
    if (n > kFailLines)
        return;
    HeapInfo h[kMaxHeaps];
    const int hn = ReadHeaps(h, kMaxHeaps);
    char a[48];
    char b[48];
    char heapText[160];
    if (heap >= 0 && heap < hn && h[heap].size)
        snprintf(heapText, sizeof(heapText), "heap %d \"%s\" (%lu KB) had no free block that big - %lu KB free, its largest free "
                 "block %lu KB (%d free block(s))", heap, h[heap].name, static_cast<unsigned long>(h[heap].size / 1024),
                 static_cast<unsigned long>(h[heap].freeB / 1024), static_cast<unsigned long>(h[heap].largest / 1024), h[heap].blocks);
    else
        snprintf(heapText, sizeof(heapText), "heap %d (not readable)", heap);
    Log("[mem] %lu ms: THE GAME'S ALLOCATION FAILED (#%ld%s): %lu bytes asked; %s; asked by %s (called from %s), thread %lu",
        ScanMsSinceAttach(), static_cast<long>(n), expected ? ", one the game expected might fail" : "", static_cast<unsigned long>(size),
        heapText, CodeText(from, a, sizeof(a)), CodeText(from2, b, sizeof(b)), static_cast<unsigned long>(GetCurrentThreadId()));
    if (n <= kFailVaLines)
    {
        Va v;
        VaSnap(&v);
        char t[300];
        Log("[mem]   at that failure: %s", VaText(v, t, sizeof(t)));
    }
    if (n == kFailLines)
        Log("[mem] %ld failed game allocations logged - from now on they are counted in the [mem] lines only", static_cast<long>(n));
}

// ---- the watcher ---------------------------------------------------------------------------------------------------------
struct Low
{
    uint64_t largest;
    uint64_t freeB;
    uint32_t heapFree[kMaxHeaps];
};

void LowReset(Low* l)
{
    l->largest = ~0ull;
    l->freeB = ~0ull;
    for (int i = 0; i < kMaxHeaps; ++i)
        l->heapFree[i] = 0xFFFFFFFFu;
}

void LowTake(Low* l, const Va& v, const HeapInfo* h, int n)
{
    if (v.largest < l->largest)
        l->largest = v.largest;
    if (v.freeB < l->freeB)
        l->freeB = v.freeB;
    for (int i = 0; i < n; ++i)
        if (h[i].size && h[i].blocks >= 0 && h[i].freeB < l->heapFree[i])
            l->heapFree[i] = h[i].freeB;
}

LONG g_d3dMadeAt[5] = {};
LONG g_d3dKbAt[4] = {};
LONG g_d3dKbTexAt = 0;

void LogLines(const char* why, const char* map, const Va& v, const HeapInfo* h, int hn, const Low* low, unsigned long spanS)
{
    char t[300];
    char lowText[120] = "";
    if (low && low->largest != ~0ull)
        snprintf(lowText, sizeof(lowText), " (the lowest in the last %lu s: the largest free block %lu MB, free %lu MB)", spanS,
                 Mb(low->largest), Mb(low->freeB));
    char mapText[64] = "";
    if (map && map[0])
        snprintf(mapText, sizeof(mapText), ", map \"%s\"", map);
    Log("[mem] %lu ms%s%s%s: %s%s", ScanMsSinceAttach(), why ? " - " : "", why ? why : "", mapText, VaText(v, t, sizeof(t)), lowText);
    char ht[700];
    Log("[mem]   the game's heaps, KB free of KB (its largest free block%s): %s", low ? "; the lowest free in that time" : "",
        HeapsText(h, hn, low ? low->heapFree : nullptr, ht, sizeof(ht)));
    char ft[300];
    Log("[mem]   %s", FailsText(ft, sizeof(ft)));
    LONG made[5];
    LONG kb[4];
    LONG madeAll = 0;
    for (int i = 0; i < 5; ++i)
    {
        made[i] = g_d3dMade[i] - g_d3dMadeAt[i];
        g_d3dMadeAt[i] += made[i];
        madeAll += made[i];
    }
    for (int i = 0; i < 4; ++i)
    {
        kb[i] = g_d3dKb[i] - g_d3dKbAt[i];
        g_d3dKbAt[i] += kb[i];
    }
    const LONG tex = g_d3dKbTex - g_d3dKbTexAt;
    g_d3dKbTexAt += tex;
    if (madeAll || g_d3dFails)
        Log("[mem]   Direct3D since the last line: %ld textures (%ld volume, %ld cube) %ld MB, %ld vertex / %ld index buffers "
            "%ld MB; by pool: managed %ld MB (each keeps a copy in the game's address space; %ld MB since the start), system "
            "memory %ld MB, video memory %ld MB, scratch %ld MB; failed creates so far %ld",
            static_cast<long>(made[0] + made[1] + made[2]), static_cast<long>(made[1]), static_cast<long>(made[2]),
            static_cast<long>(tex / 1024), static_cast<long>(made[3]), static_cast<long>(made[4]),
            static_cast<long>((kb[0] + kb[1] + kb[2] + kb[3] - tex) / 1024), static_cast<long>(kb[1] / 1024),
            static_cast<long>(g_d3dKb[1] / 1024), static_cast<long>(kb[2] / 1024), static_cast<long>(kb[0] / 1024),
            static_cast<long>(kb[3] / 1024), static_cast<long>(g_d3dFails));
    char tt[300];
    if (TexCapText(tt, sizeof(tt))[0])
        Log("[mem]   %s", tt); // 22d
}

DWORD WINAPI Watcher(LPVOID)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    DWORD lastLine = GetTickCount();
    char lastMap[48] = "";
    P2SpawnMissionName(lastMap, sizeof(lastMap));
    Low low;
    LowReset(&low);
    bool vaLow = false;
    int vaLowLines = 0;
    bool heapLow[kMaxHeaps] = {};
    int heapLowLines = 0;
    const DWORD intervalMs = static_cast<DWORD>(g_intervalS) * 1000u;
    for (;;)
    {
        Sleep(1000);
        Va v;
        VaSnap(&v);
        HeapInfo h[kMaxHeaps];
        const int hn = ReadHeaps(h, kMaxHeaps);
        LowTake(&low, v, h, hn);
        // a block under 32 MB: the next big allocation (a level chunk, a texture's copy, Bullet's pools) may not fit
        if (!vaLow && v.largest < 32ull * kMB && vaLowLines < 10)
        {
            vaLow = true;
            ++vaLowLines;
            char t[300];
            Log("[mem] %lu ms: LOW - the address space's largest free block is under 32 MB: %s", ScanMsSinceAttach(), VaText(v, t, sizeof(t)));
        }
        else if (vaLow && v.largest > 64ull * kMB)
            vaLow = false;
        for (int i = 0; i < hn; ++i)
        {
            if (!h[i].size || h[i].blocks < 0 || h[i].size < 4 * kMB)
                continue;
            if (!heapLow[i] && h[i].freeB < h[i].size / 20 && heapLowLines < 20)
            {
                heapLow[i] = true;
                ++heapLowLines;
                Log("[mem] %lu ms: LOW - heap %d \"%s\" has %lu KB free of %lu KB (its largest free block %lu KB)", ScanMsSinceAttach(), i,
                    h[i].name, static_cast<unsigned long>(h[i].freeB / 1024), static_cast<unsigned long>(h[i].size / 1024),
                    static_cast<unsigned long>(h[i].largest / 1024));
            }
            else if (heapLow[i] && h[i].freeB > h[i].size / 10)
                heapLow[i] = false;
        }
        char map[48] = "";
        P2SpawnMissionName(map, sizeof(map));
        const bool mapChanged = strcmp(map, lastMap) != 0;
        const DWORD now = GetTickCount();
        if (mapChanged || now - lastLine >= intervalMs)
        {
            char why[80] = "";
            if (mapChanged)
                snprintf(why, sizeof(why), "the map changed (was \"%s\")", lastMap);
            LogLines(why[0] ? why : nullptr, map, v, h, hn, &low, static_cast<unsigned long>((now - lastLine + 500) / 1000));
            memcpy(lastMap, map, sizeof(lastMap));
            lastLine = now;
            LowReset(&low);
        }
    }
}

int ReadIni()
{
    char path[MAX_PATH];
    const DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (!n || n >= MAX_PATH)
        return 30;
    char* slash = strrchr(path, '\\');
    if (!slash)
        return 30;
    snprintf(slash + 1, static_cast<size_t>(MAX_PATH - (slash + 1 - path)), "BF1GameExt.ini");
    return static_cast<int>(GetPrivateProfileIntA("Limits", "MemoryLog", 30, path));
}

bool ExeLargeAddressAware()
{
    const uintptr_t base = ExeBase();
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (!base || dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;
    const IMAGE_NT_HEADERS* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + static_cast<uintptr_t>(dos->e_lfanew));
    return nt->Signature == IMAGE_NT_SIGNATURE && (nt->FileHeader.Characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE);
}

bool Find(char* why, size_t whyLen)
{
    uint32_t hits[4] = {};
    if (FindPattern(kAllocBody, hits, 4) != 1)
    {
        snprintf(why, whyLen, "the heap allocator: not exactly 1 pattern hit");
        return false;
    }
    const uint32_t a = hits[0];
    if (!MatchPatternAt(a + kAllocFailAt, kAllocFail))
    {
        snprintf(why, whyLen, "the heap allocator rva 0x%06X: its failure path does not match", static_cast<unsigned>(a));
        return false;
    }
    if (FindPattern(kCreateBody, hits, 4) != 1)
    {
        snprintf(why, whyLen, "heap create: not exactly 1 pattern hit");
        return false;
    }
    const uint32_t c = hits[0];
    if (!MatchPatternAt(c + kCreateSetAt, kCreateSet))
    {
        snprintf(why, whyLen, "heap create rva 0x%06X: its entry stores do not match", static_cast<unsigned>(c));
        return false;
    }
    const uintptr_t table = Rd(Rva(a + kAllocTable));
    const uintptr_t rec = Rd(Rva(a + kAllocRecord));
    const uintptr_t cb = Rd(Rva(a + kAllocCallback));
    const uintptr_t sup = Rd(Rva(a + kAllocSuppress));
    const uintptr_t count = Rd(Rva(c + kCreateCount));
    if (Rd(Rva(a + kAllocRecord4)) != rec + 4 || cb != rec + 0xC || Rd(Rva(c + kCreateTable)) != table ||
        Rd(Rva(c + kCreateSetAt + 2)) != table || !InExe(table) || !InExe(rec) || !InExe(cb) || !InExe(sup) || !InExe(count))
    {
        snprintf(why, whyLen, "the allocator's and heap create's globals do not agree");
        return false;
    }
    g_allocRva = a;
    g_callRet = Rva(a + kAllocCallRet);
    g_tableVa = table;
    g_countVa = count;
    g_recordVa = rec;
    g_callbackVa = cb;
    g_suppressVa = sup;
    return true;
}

} // namespace

void MemLogStart(bool onGameThread)
{
    static bool started = false;
    if (started)
        return;
    started = true;
    const int iv = ReadIni();
    if (iv <= 0)
    {
        Log("[mem] 22c: the memory log is off ([Limits] MemoryLog=0)");
        return;
    }
    g_intervalS = iv < 5 ? 5 : (iv > 3600 ? 3600 : iv);
    if (!IsSteam() && !IsGog13())
    {
        Log("[mem] 22c: the memory log: not GOG 1.3 or Steam - not used");
        return;
    }
    char why[160] = "";
    const bool found = Find(why, sizeof(why));
    if (found)
    {
        const uint32_t old = Rd(g_callbackVa);
        if (!old)
        {
            void (*fn)() = &GameAllocFailed;
            InterlockedExchange(reinterpret_cast<volatile LONG*>(g_callbackVa), static_cast<LONG>(reinterpret_cast<uintptr_t>(fn)));
            g_callbackOurs = true;
        }
        Log("[mem] 22c: THE MEMORY LOG ([Limits] MemoryLog=%d, %s): the game's heap allocator rva 0x%06X (its heap table at rva "
            "0x%06X, %lu slots; the failed-allocation record rva 0x%06X); its failure callback slot rva 0x%06X %s",
            g_intervalS, onGameThread ? "the game's DirectInput call" : "the timer", static_cast<unsigned>(g_allocRva),
            static_cast<unsigned>(g_tableVa - ExeBase()), static_cast<unsigned long>(Rd(g_countVa)),
            static_cast<unsigned>(g_recordVa - ExeBase()), static_cast<unsigned>(g_callbackVa - ExeBase()),
            g_callbackOurs ? "was empty (as in both exes) - BF1GameExt's now: every failed game allocation is logged with its heap, "
                             "size and caller"
                           : "is NOT empty - left alone (failed allocations not watched)");
    }
    else
        Log("[mem] 22c: the game's heaps NOT found (%s) - the address space only", why);
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    const bool msOk = GlobalMemoryStatusEx(&ms) != 0;
    Log("[mem] 22c: the exe is %s; this PC: %lu MB of RAM (%lu MB free)%s; a [mem] line every %d s and when the map changes, the "
        "address space and the heaps sampled every second (the lowest between lines), LOW lines when the largest free block is "
        "under 32 MB or a heap over 4 MB is under 5%% free",
        ExeLargeAddressAware() ? "LARGE-ADDRESS AWARE (up to 4 GB on 64-bit Windows)"
                               : "not large-address aware: the game has 2 GB of address space",
        msOk ? Mb(ms.ullTotalPhys) : 0ul, msOk ? Mb(ms.ullAvailPhys) : 0ul, msOk ? "" : " (not read)", g_intervalS);
    Va v;
    VaSnap(&v);
    HeapInfo h[kMaxHeaps];
    const int hn = found ? ReadHeaps(h, kMaxHeaps) : 0;
    char map[48] = "";
    P2SpawnMissionName(map, sizeof(map));
    LogLines("at the game's DirectInput call", map, v, h, hn, nullptr, 0);
    g_on = true;
    const HANDLE t = CreateThread(nullptr, 0, Watcher, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
    else
        Log("[mem] the watcher thread did not start (error %lu) - failures and crashes are still logged", GetLastError());
}

void MemLogStop()
{
    if (g_callbackOurs && g_callbackVa)
    {
        void (*fn)() = &GameAllocFailed;
        InterlockedCompareExchange(reinterpret_cast<volatile LONG*>(g_callbackVa), 0, static_cast<LONG>(reinterpret_cast<uintptr_t>(fn)));
    }
}

void MemLogCrash()
{
    Va v;
    VaSnap(&v);
    char t[300];
    Log("[exception]   memory: %s", VaText(v, t, sizeof(t)));
    if (g_tableVa)
    {
        HeapInfo h[kMaxHeaps];
        const int hn = ReadHeaps(h, kMaxHeaps);
        char ht[700];
        Log("[exception]   the game's heaps, KB free of KB (its largest free block): %s", HeapsText(h, hn, nullptr, ht, sizeof(ht)));
        char ft[300];
        Log("[exception]   %s", FailsText(ft, sizeof(ft)));
    }
    if (g_d3dFails)
        Log("[exception]   Direct3D creates that failed so far: %ld", static_cast<long>(g_d3dFails));
    char tt[300];
    if (TexCapText(tt, sizeof(tt))[0])
        Log("[exception]   %s", tt); // 22d
}

void MemLogD3dMade(int kind, unsigned pool, unsigned long long bytes)
{
    if (kind < 0 || kind > 4)
        return;
    InterlockedIncrement(&g_d3dMade[kind]);
    const LONG kb = static_cast<LONG>((bytes + 1023) / 1024);
    InterlockedExchangeAdd(&g_d3dKb[pool < 4 ? pool : 0], kb);
    if (kind <= 2)
        InterlockedExchangeAdd(&g_d3dKbTex, kb);
}

void MemLogD3dFailed(int kind, long hr, unsigned w, unsigned h, unsigned depthOrLevels, unsigned usage, unsigned format,
                     unsigned pool, const void* caller)
{
    const LONG n = InterlockedIncrement(&g_d3dFails);
    if (n > kD3dFailLines)
        return;
    const char* hrText = hr == static_cast<long>(0x8007000E) ? "E_OUTOFMEMORY"
                         : hr == static_cast<long>(0x8876017C) ? "D3DERR_OUTOFVIDEOMEMORY"
                         : hr == static_cast<long>(0x8876086C) ? "D3DERR_INVALIDCALL"
                         : hr == static_cast<long>(0x88760868) ? "D3DERR_DEVICELOST"
                         : hr == static_cast<long>(0x8876086A) ? "D3DERR_NOTAVAILABLE" : "?";
    const char* poolText = pool == 0 ? "video memory (default)" : pool == 1 ? "managed" : pool == 2 ? "system memory" : pool == 3 ? "scratch" : "?";
    char fmt[16];
    if (format >= 0x20202020u && format <= 0x7A7A7A7Au)
        snprintf(fmt, sizeof(fmt), "%c%c%c%c", static_cast<char>(format & 0xFF), static_cast<char>((format >> 8) & 0xFF),
                 static_cast<char>((format >> 16) & 0xFF), static_cast<char>((format >> 24) & 0xFF));
    else
        snprintf(fmt, sizeof(fmt), "%u", format);
    char who[48];
    char t[300];
    Va v;
    VaSnap(&v);
    if (kind >= 3)
        Log("[mem] %lu ms: DIRECT3D %s FAILED (#%ld): 0x%08lX %s - %u bytes, usage 0x%X, %s 0x%X, pool %s; asked by %s; %s",
            ScanMsSinceAttach(), kD3dKind[kind], static_cast<long>(n), static_cast<unsigned long>(hr), hrText, w, usage,
            kind == 3 ? "FVF" : "format", format, poolText, CodeText(reinterpret_cast<uintptr_t>(caller), who, sizeof(who)),
            VaText(v, t, sizeof(t)));
    else
        Log("[mem] %lu ms: DIRECT3D %s FAILED (#%ld): 0x%08lX %s - %u x %u, %u %s, format %s, usage 0x%X, pool %s; asked by %s; %s",
            ScanMsSinceAttach(), kD3dKind[kind < 0 || kind > 4 ? 0 : kind], static_cast<long>(n), static_cast<unsigned long>(hr),
            hrText, w, h, depthOrLevels, kind == 1 ? "deep" : "levels", fmt, usage, poolText,
            CodeText(reinterpret_cast<uintptr_t>(caller), who, sizeof(who)), VaText(v, t, sizeof(t)));
    if (n == kD3dFailLines)
        Log("[mem] %ld failed Direct3D creates logged - from now on only counted", static_cast<long>(n));
}

const char* MemLogVaShort(char* out, size_t n)
{
    Va v;
    VaSnap(&v);
    snprintf(out, n, "address space: %lu MB free, the largest free block %lu MB", Mb(v.freeB), Mb(v.largest));
    return out;
}

} // namespace bf1
