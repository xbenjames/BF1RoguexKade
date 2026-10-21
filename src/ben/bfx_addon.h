/* bfx_addon.h - the interface between BF1GameExt (GameData\dinput8.dll) and its add-on DLLs (GameData\dinput8_*.dll).
 * Plain C so a C add-on and the C++ host share it unchanged.   (BF1GameExt v6.4, Ben James, Oct 2026)
 *
 * HOW AN ADD-ON IS LOADED
 *   BF1GameExt's DllMain loads every GameData\dinput8_*.dll in name order (LoadLibraryA), the same moment Windows loads
 *   dinput8.dll itself. The add-on's own DllMain must do nothing but remember its module handle (the game's code is
 *   still locked on Steam at that point).
 *
 * WHAT AN ADD-ON MAY EXPORT (every one optional, all __cdecl, found by GetProcAddress)
 *   void BFX_Init(const BfxHost* host)
 *       Called once, on the game thread, at the game's DirectInput8Create call - the game's code is unlocked and
 *       relocated by then, so this is where an add-on checks its byte patterns and installs its hooks. BF1GameExt
 *       calls every add-on's BFX_Init BEFORE it installs its own hooks, so an add-on that patches the same place as a
 *       BF1GameExt feature wins (BF1GameExt's byte-check then fails and that feature stays off, logged).
 *   void BFX_DirectInput(void* directInput8)
 *       Called with the IDirectInput8 object the game got, right after it was made (once). For add-ons that wrap the
 *       keyboard / mouse device (e.g. to hide a key from the game).
 *   const BfxModOptions* BFX_ModOptions(void)
 *       The rows this add-on wants on BF1GameExt's MODS page (OPTIONS > MODS). Return a table that stays valid for
 *       the life of the DLL (static). NULL / not exported = no rows.
 *
 * STAND-ALONE USE
 *   An add-on may also export DirectInput8Create and forward it to the system's dinput8.dll, so the same file works
 *   renamed to dinput8.dll without BF1GameExt (nothing calls BFX_Init then - the add-on starts itself from its
 *   DirectInput8Create instead).
 */
#ifndef BFX_ADDON_H
#define BFX_ADDON_H

#ifdef __cplusplus
extern "C" {
#endif

#define BFX_API_VERSION 1

typedef struct BfxHost
{
    int version;              /* BFX_API_VERSION */
    const char* gameDir;      /* GameData folder (where Battlefront.exe is), no trailing backslash */
    const char* hostBuild;    /* BF1GameExt's build string, e.g. "p24a-r1" */
    void (*log)(const char* line);  /* a line into BF1GameExt_split.log, tagged [addon] */
} BfxHost;

/* kinds of row */
#define BFX_OPT_ONOFF   0   /* get() returns 0 / 1; set(v) with 0 / 1 */
#define BFX_OPT_CHOICE  1   /* get() returns an index into values[]; set(index) */

/* flags */
#define BFX_OPTF_RESTART 1  /* takes effect at the next game start - the page says so */
#define BFX_OPTF_MASTER  2  /* this row is the mod's own ON / OFF; shown on the MODS page's row for the mod too */

typedef struct BfxOption
{
    const char* label;          /* "CP CLASS CHANGE" (upper case, the menus' font has capitals only) */
    int kind;                   /* BFX_OPT_* */
    const char* const* values;  /* BFX_OPT_CHOICE: the texts, upper case */
    int valueCount;
    int (*get)(void);           /* the current value; -1 = not available this session (the row says so, cannot change) */
    int (*set)(int value);      /* apply at once where possible, save to the mod's own ini; returns the value now */
    int flags;                  /* BFX_OPTF_* */
} BfxOption;

typedef struct BfxModOptions
{
    int version;                /* BFX_API_VERSION */
    const char* name;           /* the mod's name on the MODS page, upper case: "SPRINT" */
    int count;
    const BfxOption* options;
} BfxModOptions;

#ifdef __cplusplus
}
#endif
#endif
