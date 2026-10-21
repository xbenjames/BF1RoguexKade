// BF1GameExt - loadscreen_log.cpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/diagnostics/ (was load_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 9a (2020 build): read-only scan of the loading screen's layout.
//
// The 2020 loading screen is a Lua interface script. It creates its picture
// with ScriptCB_AddIFImage, sizes and places things with ScriptCB_IFImage_SetRect
// and ScriptCB_IFObj_SetPos / SetPosition / SetScreenPosition / SetWidthHeight,
// and lays out in 4:3 because ScriptCB_GetTargetAspect returns 0.75. This wraps
// those Lua functions (log-only: every wrapper calls the game's original) and,
// while armed, logs the numbers each call receives or returns. It arms itself
// for 60 s when the shell starts a map (ScriptCB_EnterMission or
// ScriptCB_RestartMission, also wrapped log-only); Insert arms it by hand.
// (r1 used F12, which Steam's screenshot key swallows.) [loadscan] lines.
//
// Arguments are read with the game's own Lua helpers, found inside
// ScriptCB_IFImage_SetRect (proven on GOG 1.3):
//   +0x40  push edi ; push esi ; call IsNumber(L, i)     (body checked)
//   +0x89  push edi ; push esi ; call ToNumber(L, i)     (wrapper -> checked
//          luaL-style read; ONLY called after IsNumber says yes, because on a
//          non-number it raises a Lua error)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "loadscreen_log.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

typedef int(__cdecl* LuaFn)(void* L);
typedef int(__cdecl* IsNumber_t)(void* L, int index);
typedef float(__cdecl* ToNumber_t)(void* L, int index);

const char* const kIsNumberBody =
    "55 8B EC FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 0F 95 C0 5D C3";
const char* const kToNumberWrapperBody = "55 8B EC 5D E9";
const char* const kCheckNumberBody =
    "55 8B EC 51 56 8B 75 0C 57 8B 7D 08 56 57 E8 ?? ?? ?? ?? D9 5D FC F3 0F 10 45 FC 83 C4 08 0F 2E";

IsNumber_t g_isNumber = nullptr;
ToNumber_t g_toNumber = nullptr;

constexpr DWORD kArmMs = 60000;
constexpr int kMaxLines = 900;
volatile LONG g_armUntil = 0;
volatile LONG g_lines = 0;

struct Wrapped
{
    const char* name;
    bool getter; // log what it returns instead of what it receives
    bool arms;   // a map is starting: arm the scan
    LuaFn orig;
};

Wrapped g_w[] = {
    { "ScriptCB_GetTargetAspect", true, false, nullptr },
    { "ScriptCB_GetCurrentAspect", true, false, nullptr },
    { "ScriptCB_GetScreenInfo", true, false, nullptr },
    { "ScriptCB_GetSafeScreenInfo", true, false, nullptr },
    { "ScriptCB_AddIFImage", false, false, nullptr },
    { "ScriptCB_AddIFMaskImage", false, false, nullptr },
    { "ScriptCB_IFImage_SetRect", false, false, nullptr },
    { "ScriptCB_IFImage_SetTexCoords", false, false, nullptr },
    { "ScriptCB_IFImage_SetTexture", false, false, nullptr },
    { "ScriptCB_IFObj_SetPos", false, false, nullptr },
    { "ScriptCB_IFObj_SetPosition", false, false, nullptr },
    { "ScriptCB_IFObj_SetScreenPosition", false, false, nullptr },
    { "ScriptCB_IFObj_SetWidthHeight", false, false, nullptr },
    { "ScriptCB_IFObj_SetUseSafezone", false, false, nullptr },
    { "ScriptCB_IFText_SetTextBox", false, false, nullptr },
    { "ScriptCB_IFText_SetTextPosition", false, false, nullptr },
    { "ScriptCB_EnterMission", false, true, nullptr },
    { "ScriptCB_RestartMission", false, true, nullptr },
};
constexpr int kWrapped = static_cast<int>(sizeof(g_w) / sizeof(g_w[0]));

bool Armed()
{
    const LONG until = g_armUntil;
    return until != 0 && static_cast<LONG>(GetTickCount() - static_cast<DWORD>(until)) < 0;
}

bool Budget()
{
    const LONG n = InterlockedIncrement(&g_lines);
    if (n == kMaxLines)
        Log("[loadscan] line limit reached, going quiet");
    return n < kMaxLines;
}

// "2:0.500 3:1.000 ..." for the numeric stack slots in [first, last].
void NumbersAt(void* L, int first, int last, char* out, size_t outSize)
{
    out[0] = 0;
    size_t used = 0;
    for (int i = first; i <= last; ++i)
    {
        if (i == 0 || !g_isNumber(L, i))
            continue;
        const float v = g_toNumber(L, i);
        const int w = snprintf(out + used, outSize - used, "%s%d:%.4f", used ? " " : "", i,
                               static_cast<double>(v));
        if (w < 0 || static_cast<size_t>(w) >= outSize - used)
            break;
        used += static_cast<size_t>(w);
    }
    if (!used)
        snprintf(out, outSize, "(no numbers)");
}

void Arm(const char* why)
{
    InterlockedExchange(&g_armUntil, static_cast<LONG>(GetTickCount() + kArmMs));
    Log("[loadscan] %lu ms: %s: ARMED for %lu s", ScanMsSinceAttach(), why,
        static_cast<unsigned long>(kArmMs / 1000));
}

int Handle(int n, void* L)
{
    const Wrapped& w = g_w[n];
    if (w.arms && g_isNumber)
        Arm(w.name);
    if (!Armed() || !g_isNumber || !g_toNumber)
        return w.orig(L);

    if (!w.getter)
    {
        char args[256];
        NumbersAt(L, 1, 8, args, sizeof(args));
        if (Budget())
            Log("[loadscan] %lu ms: %s %s", ScanMsSinceAttach(), w.name, args);
        return w.orig(L);
    }

    const int results = w.orig(L);
    if (results > 0 && results <= 8)
    {
        char vals[256];
        NumbersAt(L, -results, -1, vals, sizeof(vals));
        if (Budget())
            Log("[loadscan] %lu ms: %s -> %s", ScanMsSinceAttach(), w.name, vals);
    }
    return results;
}

template <int N> int __cdecl Wrap(void* L)
{
    return Handle(N, L);
}

const LuaFn kWrappers[] = {
    &Wrap<0>,  &Wrap<1>,  &Wrap<2>,  &Wrap<3>,  &Wrap<4>,  &Wrap<5>,
    &Wrap<6>,  &Wrap<7>,  &Wrap<8>,  &Wrap<9>,  &Wrap<10>, &Wrap<11>,
    &Wrap<12>, &Wrap<13>, &Wrap<14>, &Wrap<15>, &Wrap<16>, &Wrap<17>,
};
static_assert(sizeof(kWrappers) / sizeof(kWrappers[0]) == sizeof(g_w) / sizeof(g_w[0]),
              "one wrapper per wrapped Lua function");

int32_t ReadI32(uintptr_t va)
{
    int32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uintptr_t CallTargetVa(uintptr_t callAt)
{
    uintptr_t t = static_cast<uintptr_t>(static_cast<intptr_t>(callAt) + 5 + ReadI32(callAt + 1));
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(t) == 0xE9; ++hop)
        t = static_cast<uintptr_t>(static_cast<intptr_t>(t) + 5 + ReadI32(t + 1));
    return t;
}

bool InCode(uintptr_t va)
{
    uint32_t rva = 0;
    uint32_t size = 0;
    return CodeSection(&rva, &size) && va >= Rva(rva) && va < Rva(rva) + size;
}

uint32_t RvaOf(uintptr_t va)
{
    return static_cast<uint32_t>(va - ExeBase());
}

// The two Lua helpers, from inside ScriptCB_IFImage_SetRect.
bool FindLuaHelpers()
{
    const uintptr_t slot = ScanLuaFuncSlot("ScriptCB_IFImage_SetRect");
    if (!slot)
    {
        Log("[loadscan] ScriptCB_IFImage_SetRect: registration slot NOT FOUND - scan disabled");
        return false;
    }
    uint32_t fnRaw = 0;
    memcpy(&fnRaw, reinterpret_cast<const void*>(slot), sizeof(fnRaw));
    uintptr_t fn = fnRaw;
    if (!InCode(fn))
    {
        Log("[loadscan] ScriptCB_IFImage_SetRect does not point into the exe's code - scan disabled");
        return false;
    }
    for (int hop = 0; hop < 8 && *reinterpret_cast<const unsigned char*>(fn) == 0xE9; ++hop)
        fn = static_cast<uintptr_t>(static_cast<intptr_t>(fn) + 5 + ReadI32(fn + 1));

    // The first two "push edi ; push esi ; call" in its first 0x100 bytes.
    uintptr_t calls[2] = {};
    int found = 0;
    for (uintptr_t p = fn; p < fn + 0x100 && found < 2; ++p)
    {
        const unsigned char* c = reinterpret_cast<const unsigned char*>(p);
        if (c[0] == 0x57 && c[1] == 0x56 && c[2] == 0xE8)
            calls[found++] = p + 2;
    }
    if (found != 2)
    {
        Log("[loadscan] ScriptCB_IFImage_SetRect at rva 0x%08X: helper calls not found - scan "
            "disabled",
            RvaOf(fn));
        return false;
    }
    const uintptr_t isNum = CallTargetVa(calls[0]);
    const uintptr_t toNumWrapper = CallTargetVa(calls[1]); // through the link thunk
    if (!InCode(isNum) || !InCode(toNumWrapper) || !MatchPatternAt(RvaOf(isNum), kIsNumberBody) ||
        !MatchPatternAt(RvaOf(toNumWrapper), kToNumberWrapperBody))
    {
        Log("[loadscan] Lua helpers do not match (IsNumber rva 0x%08X, ToNumber rva 0x%08X) - scan "
            "disabled",
            RvaOf(isNum), RvaOf(toNumWrapper));
        return false;
    }
    const uintptr_t checked = CallTargetVa(toNumWrapper + 4);
    if (!InCode(checked) || !MatchPatternAt(RvaOf(checked), kCheckNumberBody))
    {
        Log("[loadscan] ToNumber's target at rva 0x%08X does not match - scan disabled",
            RvaOf(checked));
        return false;
    }
    g_isNumber = reinterpret_cast<IsNumber_t>(isNum);
    g_toNumber = reinterpret_cast<ToNumber_t>(toNumWrapper);
    Log("[loadscan] Lua helpers OK: IsNumber rva 0x%08X, ToNumber rva 0x%08X (-> rva 0x%08X)",
        RvaOf(isNum), RvaOf(toNumWrapper), RvaOf(checked));
    return true;
}

} // namespace

void LoadScanStart()
{
    const BuildInfo* build = CurrentBuild();
    if (!build || build->loadsMemExt)
    {
        Log("[loadscan] not used on this build");
        return;
    }
    if (!FindLuaHelpers())
        return;

    int installed = 0;
    for (int i = 0; i < kWrapped; ++i)
    {
        const uintptr_t slot = ScanLuaFuncSlot(g_w[i].name);
        if (!slot)
        {
            Log("[loadscan] %s: registration slot NOT FOUND - not wrapped", g_w[i].name);
            continue;
        }
        uint32_t current = 0;
        memcpy(&current, reinterpret_cast<const void*>(slot), sizeof(current));
        if (!InCode(current))
        {
            Log("[loadscan] %s: slot does not point into the exe's code - not wrapped",
                g_w[i].name);
            continue;
        }
        g_w[i].orig = reinterpret_cast<LuaFn>(static_cast<uintptr_t>(current));
        const uint32_t wrapper = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(kWrappers[i]));
        memcpy(reinterpret_cast<void*>(slot), &wrapper, sizeof(wrapper));
        ++installed;
    }
    Log("[loadscan] %d of %d Lua functions wrapped (log-only). Starting a map arms the scan for "
        "60 s by itself (Insert arms it by hand).",
        installed, kWrapped);
}

void LoadScanArm()
{
    if (!g_isNumber)
    {
        Log("[loadscan] Insert: the scan is not installed on this run");
        return;
    }
    Arm("Insert key");
}

} // namespace bf1
