// BF1GameExt - addons.cpp   (v6.4, Ben James, Oct 2026)
// The add-on loader and the MODS registry. [addon] lines.
//
// Why: BF1GameExt grew its own sprint, ragdolls, MODS page and so on, and Ben's BF1 mods (sprint with BF2's animation,
// lightsaber combos, cloth, class change at a command post, team colours) are separate DLLs. BF2's GameExt lets other
// dinput8_*.dll files ride along; this gives BF1GameExt the same, plus one MODS page every add-on can put its rows on,
// so the player switches everything in one place and each mod keeps its own .ini.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "addons.hpp"
#include "log.hpp"

namespace bf1 {
namespace {

constexpr int kMaxAddons = 24;
constexpr int kMaxMods = 32;

typedef void (__cdecl* BfxInit_t)(const BfxHost*);
typedef void (__cdecl* BfxDirectInput_t)(void*);
typedef const BfxModOptions* (__cdecl* BfxModOptions_t)(void);

struct Addon
{
    char name[64];
    HMODULE module;
    BfxInit_t init;
    BfxDirectInput_t directInput;
    BfxModOptions_t modOptions;
};

Addon g_addons[kMaxAddons];
int g_addonCount = 0;
bool g_inited = false;
bool g_diGiven = false;
char g_gameDir[MAX_PATH] = "";
char g_hostBuild[32] = "";

const BfxModOptions* g_mods[kMaxMods];
int g_modCount = 0;

void __cdecl HostLog(const char* line)
{
    Log("[addon] %s", line ? line : "");
}

int CompareNames(const void* a, const void* b)
{
    return _stricmp(static_cast<const Addon*>(a)->name, static_cast<const Addon*>(b)->name);
}

// Lua string literal (the names are plain upper-case ASCII; anything odd is dropped)
void LuaStr(char* out, size_t size, const char* s)
{
    size_t at = 0;
    if (size < 3)
    {
        if (size)
            out[0] = 0;
        return;
    }
    out[at++] = '"';
    for (const char* p = s ? s : ""; *p && at + 2 < size; ++p)
    {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (c == '"' || c == '\\' || c < 32 || c > 126)
            continue;
        out[at++] = static_cast<char>(c);
    }
    out[at++] = '"';
    out[at] = 0;
}

void Append(char* buf, size_t size, size_t* at, const char* fmt, ...)
{
    if (*at >= size)
        return;
    va_list ap;
    va_start(ap, fmt);
    const int n = vsnprintf(buf + *at, size - *at, fmt, ap);
    va_end(ap);
    if (n < 0)
        return;
    *at += static_cast<size_t>(n);
    if (*at > size - 1)
        *at = size - 1;
}

// The text a row shows for its value
void OptionText(const BfxOption& o, int v, char* out, size_t size)
{
    if (v < 0)
    {
        snprintf(out, size, "NOT AVAILABLE");
        return;
    }
    if (o.kind == BFX_OPT_CHOICE)
    {
        if (o.values && v < o.valueCount && o.values[v])
            snprintf(out, size, "%s", o.values[v]);
        else
            snprintf(out, size, "%d", v);
        return;
    }
    snprintf(out, size, "%s", v ? "ON" : "OFF");
}

int SafeGet(const BfxOption& o)
{
    if (!o.get)
        return -1;
    __try
    {
        return o.get();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }
}

int SafeSet(const BfxOption& o, int v)
{
    if (!o.set)
        return -1;
    __try
    {
        return o.set(v);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }
}

const BfxOption* MasterOf(const BfxModOptions* m)
{
    for (int i = 0; i < m->count; ++i)
        if (m->options[i].flags & BFX_OPTF_MASTER)
            return &m->options[i];
    return nullptr;
}

} // namespace

// ---------------------------------------------------------------------------------------------------------------------------
// loading
// ---------------------------------------------------------------------------------------------------------------------------
void AddonsLoad(const char* gameDir, const char* hostBuild)
{
    snprintf(g_gameDir, sizeof(g_gameDir), "%s", gameDir ? gameDir : "");
    snprintf(g_hostBuild, sizeof(g_hostBuild), "%s", hostBuild ? hostBuild : "");

    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", g_gameDir);
    if (GetPrivateProfileIntA("Addons", "LoadAddons", 1, ini) == 0)
    {
        Log("[addon] [Addons] LoadAddons=0 - no dinput8_*.dll loaded");
        return;
    }

    char pattern[MAX_PATH + 32];
    snprintf(pattern, sizeof(pattern), "%s\\dinput8_*.dll", g_gameDir);
    WIN32_FIND_DATAA fd;
    const HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE)
    {
        Log("[addon] no dinput8_*.dll next to the exe");
        return;
    }
    int found = 0;
    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        const size_t n = strlen(fd.cFileName);
        if (n < 5 || _stricmp(fd.cFileName + n - 4, ".dll") != 0) // "dinput8_*.dll" also matches "dinput8_x.dllx"
            continue;
        if (found < kMaxAddons)
        {
            snprintf(g_addons[found].name, sizeof(g_addons[found].name), "%s", fd.cFileName);
            ++found;
        }
        else
            Log("[addon] %s: more than %d add-ons - NOT loaded", fd.cFileName, kMaxAddons);
    } while (FindNextFileA(h, &fd));
    FindClose(h);

    qsort(g_addons, static_cast<size_t>(found), sizeof(Addon), CompareNames);

    for (int i = 0; i < found; ++i)
    {
        Addon& a = g_addons[i];
        char path[MAX_PATH + 80];
        snprintf(path, sizeof(path), "%s\\%s", g_gameDir, a.name);
        a.module = LoadLibraryA(path);
        if (!a.module)
        {
            Log("[addon] %s: LoadLibrary FAILED (error %lu) - skipped", a.name, GetLastError());
            continue;
        }
        a.init = reinterpret_cast<BfxInit_t>(reinterpret_cast<void*>(GetProcAddress(a.module, "BFX_Init")));
        a.directInput = reinterpret_cast<BfxDirectInput_t>(reinterpret_cast<void*>(GetProcAddress(a.module, "BFX_DirectInput")));
        a.modOptions = reinterpret_cast<BfxModOptions_t>(reinterpret_cast<void*>(GetProcAddress(a.module, "BFX_ModOptions")));
        Log("[addon] loaded %s at 0x%08lX: BFX_Init %s, BFX_DirectInput %s, BFX_ModOptions %s", a.name,
            reinterpret_cast<unsigned long>(a.module), a.init ? "yes" : "no", a.directInput ? "yes" : "no",
            a.modOptions ? "yes" : "no");
        g_addons[g_addonCount++] = a; // compact over failed loads
    }
    Log("[addon] %d add-on(s) loaded", g_addonCount);
}

void AddonsInit()
{
    if (g_inited)
        return;
    g_inited = true;
    BfxHost host;
    host.version = BFX_API_VERSION;
    host.gameDir = g_gameDir;
    host.hostBuild = g_hostBuild;
    host.log = HostLog;
    for (int i = 0; i < g_addonCount; ++i)
    {
        Addon& a = g_addons[i];
        if (a.init)
        {
            Log("[addon] %s: BFX_Init", a.name);
            __try
            {
                a.init(&host);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                Log("[addon] %s: BFX_Init CRASHED (exception 0x%08lX) - the add-on is left as it is", a.name,
                    static_cast<unsigned long>(GetExceptionCode()));
            }
        }
        if (a.modOptions)
        {
            const BfxModOptions* m = nullptr;
            __try
            {
                m = a.modOptions();
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                m = nullptr;
            }
            if (m && m->version == BFX_API_VERSION && m->count > 0 && m->options && m->name)
            {
                ModsRegister(m);
                Log("[addon] %s: MODS page rows: %s (%d)", a.name, m->name, m->count);
            }
            else if (m)
                Log("[addon] %s: BFX_ModOptions returned a table BF1GameExt cannot use (version %d, %d rows) - no rows",
                    a.name, m->version, m->count);
        }
    }
}

void AddonsDirectInput(void* di)
{
    if (g_diGiven || !di)
        return;
    g_diGiven = true;
    for (int i = 0; i < g_addonCount; ++i)
    {
        Addon& a = g_addons[i];
        if (!a.directInput)
            continue;
        __try
        {
            a.directInput(di);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            Log("[addon] %s: BFX_DirectInput CRASHED (exception 0x%08lX)", a.name,
                static_cast<unsigned long>(GetExceptionCode()));
        }
    }
}

int AddonsCount()
{
    return g_addonCount;
}

// ---------------------------------------------------------------------------------------------------------------------------
// the MODS registry
// ---------------------------------------------------------------------------------------------------------------------------
void ModsRegister(const BfxModOptions* mod)
{
    if (!mod || !mod->name || mod->count <= 0 || !mod->options)
        return;
    for (int i = 0; i < g_modCount; ++i)
        if (g_mods[i] == mod)
            return;
    if (g_modCount >= kMaxMods)
    {
        Log("[addon] MODS: more than %d mods - %s not listed", kMaxMods, mod->name);
        return;
    }
    g_mods[g_modCount++] = mod;
}

int ModsCount()
{
    return g_modCount;
}

const BfxModOptions* ModsAt(int k)
{
    return (k >= 0 && k < g_modCount) ? g_mods[k] : nullptr;
}

size_t ModsRowsLua(char* buf, size_t size)
{
    size_t at = 0;
    Append(buf, size, &at, "gBFXM_R = {\n");
    for (int k = 0; k < g_modCount; ++k)
    {
        const BfxModOptions* m = g_mods[k];
        const BfxOption* master = MasterOf(m);
        char name[128];
        LuaStr(name, sizeof(name), m->name);
        if (master)
        {
            const int v = SafeGet(*master);
            char text[48];
            OptionText(*master, v, text, sizeof(text));
            char tx[64];
            LuaStr(tx, sizeof(tx), text);
            Append(buf, size, &at, "  { name = %s, k = %d, on = %d, mi = %d, text = %s },\n", name, k, v,
                   static_cast<int>(master - m->options), tx);
        }
        else
            Append(buf, size, &at, "  { name = %s, k = %d },\n", name, k);
    }
    Append(buf, size, &at, "}\n");
    return at;
}

size_t ModsPageLua(int k, char* buf, size_t size)
{
    const BfxModOptions* m = ModsAt(k);
    size_t at = 0;
    if (!m)
    {
        Append(buf, size, &at, "gBFXM_P = nil\n");
        return at;
    }
    char name[128];
    LuaStr(name, sizeof(name), m->name);
    Append(buf, size, &at, "gBFXM_P = { name = %s, k = %d, rows = {\n", name, k);
    for (int i = 0; i < m->count; ++i)
    {
        const BfxOption& o = m->options[i];
        const int v = SafeGet(o);
        char text[48];
        OptionText(o, v, text, sizeof(text));
        char label[128];
        char tx[64];
        LuaStr(label, sizeof(label), o.label ? o.label : "?");
        LuaStr(tx, sizeof(tx), text);
        Append(buf, size, &at, "  { label = %s, text = %s, avail = %d, kind = %d, restart = %d },\n", label, tx,
               v >= 0 ? 1 : 0, o.kind, (o.flags & BFX_OPTF_RESTART) ? 1 : 0);
    }
    Append(buf, size, &at, "} }\n");
    return at;
}

int ModsTurn(int k, int i, int dir)
{
    const BfxModOptions* m = ModsAt(k);
    if (!m || i < 0 || i >= m->count)
        return -1;
    const BfxOption& o = m->options[i];
    const int now = SafeGet(o);
    if (now < 0)
        return -1;
    int next;
    if (o.kind == BFX_OPT_CHOICE)
    {
        const int n = o.valueCount > 0 ? o.valueCount : 1;
        next = (now + (dir < 0 ? n - 1 : 1)) % n;
    }
    else
        next = now ? 0 : 1;
    const int got = SafeSet(o, next);
    Log("[addon] MODS: %s / %s: %d -> %d (answered %d)", m->name, o.label ? o.label : "?", now, next, got);
    return got;
}

} // namespace bf1
