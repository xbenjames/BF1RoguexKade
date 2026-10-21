// BF1GameExt - limits.cpp   (file build p23a-r1)
// Credit: the extended limits - BF1MemExt's limit patches, and Dark_Phantom (his limits were the first used with
//         BF1GameExt's split screen); brought into BF1GameExt by Mr. Rogue. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/bf1memext/ (was limits_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
//
// 22c: [Limits] MainHeapMB (128 - 512, default 128 = BF1MemExt's, as before): the main heap's size. The game cuts its
// heaps out of it (Application, Gamespy, Lua 16 MB, then Runtime = the rest - the map's data); EGM's launcher made it
// 512 MB, on an exe with the 4 GB flag. Without that flag the game has 2 GB of address space and every MB here is one MB
// less for everything else (Direct3D's texture copies, the sound, BF1GameExt): the memory log ([mem] lines) shows which runs
// out. The two main heap sites take this value; everything else as before.
// 21j-r17: LimitsDlcView / LimitsDlcEntry (end of file) - the addon mission list read for the MODS page's ADDON FOLDER
// (menus.cpp): the count, the cap and the list from dlc_add's own sites, each entry's mission name (+08h) and the
// addon folder it came from (+3Ah). GOG 1.3 0x004C6CA0 is AddDownloadableContent(map, mission, memory): entry +00h / +04h the
// two names' hashes, +08h the mission name (strncpy of its own length), +3Ah the path the addon scan (0x004C6E60: every
// AddOn\<folder> with an addme.script, its full path in the buffer right after the list) left, +140h (memory + 20h) << 16.
// Nothing written; the limits as before.
//
// 18r: STEAM EARLY LIMITS, NO LONGER TIED TO THE UNLOCK GAP. The 16b misses ("Steam had already
// fixed up the addresses", load address 0x00760000 both times) were a wrong reading, not a late
// one: the test took the heap code's address operand as "fixed up" when it fell inside the exe's
// loaded range - and at a low load address the exe file's own address (0x00981F18) is inside it
// (0x00760000 .. 0x02984000). At 0x00CB0000 (the 16b test) it was not, so that one worked. Now:
//   - heaps (Steam): the early thread pauses the main thread, checks the main heap pointer (the
//     only store to it in the whole exe is heap_main's own, +0Ah; GOG 1.3 rva 0x53FEE8, Steam
//     0x581F18 = GOG + 0x42030, the data shift every other Steam global shows) is still 0 and
//     that the thread is not inside heap_main (its EIP, or a return address into it on its stack:
//     heap_main+0Ah while its 64 MB allocation runs), writes, resumes; retried until written or
//     the heap exists. The heap is made by the game's first heap allocation (the allocator calls
//     heap_main when its "heap made" flag is 0). Whether Steam had fixed up its addresses yet is
//     only logged now (the operand compared with the pointer's known rva both ways).
//   - hi-res units (Steam): at the game's DirectInput call (the scans' trigger, on the game's
//     main thread, before any match), while the animator object does not exist. The animator pool
//     lives in that object (+0, set by its set-up; the constructor leaves +0 alone, so the pool
//     pointer itself cannot be tested): the object is made when a match loads its first soldier
//     class (class factory -> "if (!object) make it"; GOG 1.3 rva 0x2133D0, 0x1920 bytes, pointer
//     in a global), set up after the mission load (the match state's enter), and deleted when the
//     match state exits (0x213410: the clean-up loop, then the global back to 0). Global 0 = no
//     object, no pool, no clean-up to come for one. The global is read from that create/delete
//     pair (5 places that must agree, new group anim_object). The render pools are rebuilt on every
//     mission load (their set-up resets the table first), so they take the new sizes from the next
//     load on. Not written while the thread is inside the render pool set-up, the pool create
//     function, the animator set-up or its clean-up (EIP / return addresses, ranges measured on
//     GOG 1.3 around the groups - the same code, shifted, on Steam).
//   - every early value is read back at the DirectInput call and logged (raised / stock), with
//     how and when each set was written, or why not.
//   GOG 1.3 is unchanged (both sets at load, in DllMain).
//
// Phase 16b: BF1MemExt LIMITS ON (GOG 1.3 and Steam). 16a found and checked
// every site; 16b raises the limits there, each set behind an INI switch
// ([Limits]), every value byte-checked before it is written:
//   - heaps: main 64 -> 128 MB, debug 2 -> 4 MB, application 15.25 -> 20 MB
//     (the application heap is carved out of the start of the main heap -
//     its set-up is called from the main heap's - so it is only raised with
//     the main heap)
//   - hi-res units: soldier animators 10 -> 100 (pool, count, the init, link
//     and clean-up loops), the 4 render pools (BF1MemExt's sizes) and their
//     minimum count 80 -> 124. Three pool values are one-byte pushes in the
//     2020 code (1.2 had them in registers): those three calls go through a
//     wrapper here that passes 1000 instead of 100.
//   - matrix pool: 3062 -> 50000 matrices, in this DLL (3.2 MB, 16-byte
//     aligned); both allocators and the range check move together, the count
//     matches the storage (BF1MemExt 1.1 on 1.2: half the places moved, and the
//     limit set in bytes)
//   - DLC missions: 512 -> 4096 (the 2020 build had raised 1.2's 50 to 512);
//     the list moves into this DLL (the missions already in it are copied),
//     its count and the DLC path buffer after it stay where they are
//     (BF1MemExt 1.1 on 1.2 moved that buffer into mission 51's place)
//
// When: the heaps and the animator pool are made once, at the game's start
// (1.2's animator set-up is called from the function that loads the fonts),
// so they must be raised before the game runs:
//   - GOG 1.3: at load (DllMain, on the main thread, before the game starts)
//   - Steam: Steam unlocks the code, then fixes up its addresses, then starts
//     the game (16a r2: the heap code was readable while its addresses were
//     still the exe file's). Once every early site is readable and the main
//     heap code's address is still unfixed - proof the game has not started -
//     the main thread (running Steam's unlocker) is paused for the writes
//     (no logging while it is: it could hold a lock), the sites checked again,
//     written, and the thread resumed. If the addresses are already fixed, the
//     early sets are left alone and the log says so.
//   Their values are plain numbers (no addresses), so Steam's address
//   fix-ups never touch them.
// The matrix pool and the DLC list are addresses: they are changed with the
// other scans (the game's DirectInput call, on the game's own thread, after
// the fix-ups) - only from that thread.
//
// 16a (log only) found every site: see its notes below.
// 16a result (Steam): all 15 groups found once (DLC find pairs twice), every
// cross-check agreed, all 4 sets READY - Steam's code at other rvas (main heap
// 0x1ED780, GOG 0x1E8160). The early check missed: "code readable 203 ms after
// load, but the main heap code matched 0 times" - Steam unlocks the code in
// stages. 16a r2 result (Steam): the heap code's address operand read
// 0x00981F18 = the exe file's own (rva 0x581F18 at 0x400000; the game was at
// 0x00CB0000): Steam fixes up the addresses after unlocking - the window 16b
// uses.
//
// 16a - how the sites were found: the 2020 exe is a Visual Studio 2019
// rebuild of the 2005 (Visual Studio 2003) code, so BF1MemExt's 1.2 addresses
// do not carry over. Each site was matched on GOG 1.3 by what it does (the
// same sizes, strings and data), then turned into a pattern with every
// relocated address and every call into another function as "??" (16b: and
// every number 16b changes, so the patterns match before and after - each
// site's value is checked on its own); each pattern matches exactly once in
// GOG 1.3 (the DLC find functions come in
// look-alike pairs: 2 each). Nothing in the game's data points into the DLC
// list or the matrix pool (GOG 1.3's relocation table). Steam runs the same
// code at other addresses; its rvas are in the table (16a's log).

#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "game_build.hpp"
#include "limits.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

enum Kind
{
    kConst,  // a size or count: exactly the value (or the raised value, once 16b wrote it)
    kGlobal, // an address in the exe's data (checked against the other sites)
    kList    // an address in the DLC list (list start + a field offset)
};

enum Set
{
    kSetHeaps,
    kSetHiRes,
    kSetMatrix,
    kSetDlc,
    kSetCount
};

const char* const kSetNames[kSetCount] = { "heaps", "hi-res units", "matrix pool", "DLC missions" };
const char* const kSetKeys[kSetCount] = { "Heaps", "HiResUnits", "MatrixPool", "DLCMissions" };

struct Site
{
    uint8_t off;
    uint8_t size; // 1 or 4
    Kind kind;
    uint32_t value;  // GOG 1.3 value (kGlobal / kList: its address there, for the log)
    uint32_t raised; // 16b's value (0: not changed directly)
    const char* what;
};

struct Chunk
{
    uint16_t off;
    const char* pattern;
};

struct Group
{
    Set set;
    const char* name;
    bool early;        // raised before the game starts (plain numbers only)
    uint32_t gogRva;   // GOG 1.3
    uint32_t steamRva; // Steam (16a's log)
    int hits;          // matches expected
    Chunk chunk[2];
    Site site[14];
};

const Group kGroups[] = {
    { kSetHeaps, "heap_main", true, 0x1E8160, 0x1ED780, 1,
      {
          { 0x00, "68 ?? ?? ?? ?? E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? 8D 88 ?? ?? ?? ?? 51 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 04 8D 88 ?? ?? ?? ?? 51 50 E8 ?? ?? ?? ?? 50 A3 ?? ?? ?? ?? E8 ?? ?? ?? ??" },
      },
      {
          { 0x01, 4, kConst, 0x04000000, 0x08000000, "main heap size (push)" },
          { 0x0B, 4, kGlobal, 0x0093FEE8, 0x00000000, "main heap pointer" },
          { 0x11, 4, kConst, 0x04000000, 0x08000000, "main heap end (lea)" },
          { 0x1D, 4, kConst, 0x00200000, 0x00400000, "debug heap size (push)" },
          { 0x2F, 4, kConst, 0x00200000, 0x00400000, "debug heap end (lea)" },
          { 0x3C, 4, kGlobal, 0x0093FEE4, 0x00000000, "debug heap pointer" },
      } },
    { kSetHeaps, "heap_app", true, 0x140C05, 0x144FD5, 1,
      {
          { 0x00, "A1 ?? ?? ?? ?? 83 C4 04 C7 05 ?? ?? ?? ?? 00 00 00 00 56 68 ?? ?? ?? ?? 8D B0 ?? ?? ?? ?? 6A 40 56 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? A3 ?? ?? ?? ??" },
      },
      {
          { 0x1A, 4, kConst, 0x00F40000, 0x01400000, "application heap size (lea)" },
          { 0x2D, 4, kGlobal, 0x008A7130, 0x00000000, "application heap pointer" },
      } },
    { kSetHiRes, "hires_alloc", true, 0x214FB0, 0x21A5D0, 1,
      {
          { 0x00, "68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 1D 68 ?? ?? ?? ?? 6A ?? 8D 70 10 C7 00 ?? ?? ?? ?? 68 20 20 00 00 56 E8 ?? ?? ?? ?? EB 02 33 F6 89 37 33 F6 8B 0F FF 77 18 03 CE E8 ?? ?? ?? ?? 81 C6 20 20 00 00 81 FE ?? ?? ?? ?? 7C E6 8D 4F 04 33 F6 8B 07 03 C6 50 05 A0 00 00 00 50 E8 ?? ?? ?? ?? 81 C6 20 20 00 00 8D 4F 04 81 FE ?? ?? ?? ?? 7C DF" },
      },
      {
          { 0x01, 4, kConst, 0x00014150, 0x000C8C90, "animator pool bytes (10 x 0x2020 + 0x10)" },
          { 0x17, 1, kConst, 0x0000000A, 0x00000064, "animator count (push)" },
          { 0x1D, 4, kConst, 0x0000000A, 0x00000064, "animator count (header)" },
          { 0x48, 4, kConst, 0x00014140, 0x000C8C80, "init loop end" },
          { 0x6E, 4, kConst, 0x00014140, 0x000C8C80, "link loop end" },
      } },
    { kSetHiRes, "hires_cleanup", true, 0x2131E0, 0x218800, 1,
      {
          { 0x00, "8B 3B 8D 87 A0 00 00 00 03 C6 8B C8 89 45 FC E8 ?? ?? ?? ?? 84 C0 74 0D FF 75 FC 8D 4B 04 E8 ?? ?? ?? ?? 8B 3B 81 C6 20 20 00 00 81 FE ?? ?? ?? ?? 7C CD" },
      },
      {
          { 0x2D, 4, kConst, 0x00014140, 0x000C8C80, "cleanup loop end" },
      } },
    { kSetHiRes, "pools_init", true, 0x28E073, 0x294853, 1,
      {
          { 0x00, "68 ?? ?? ?? ?? 6A 00 68 ?? ?? ?? ?? 6A ?? 6A 00 E8 ?? ?? ?? ?? 83 C4 14 C7 04 24 00 00 20 41 6A 00 E8 ?? ?? ?? ?? 6A 00 6A 00 6A ?? 6A 00 E8 ?? ?? ?? ?? 68 F0 55 00 00 68 F0 55 00 00 68 ?? ?? ?? ?? 68 F4 01 00 00 6A 01 E8 ?? ?? ?? ?? 83 C4 28 C7 04 24 00 00 A0 40 6A 01 E8 ?? ?? ?? ?? 68 E8 03 00 00 68 D0 07 00 00 68 E0 2E 00 00 6A 01 E8 ?? ?? ?? ?? 68 E8 03 00 00 68 ?? ?? ?? ?? 68" },
          { 0x80, "?? ?? ?? ?? 6A ?? 6A 02 E8 ?? ?? ?? ?? 83 C4 28 C7 04 24 00 00 20 41 6A 02 E8 ?? ?? ?? ?? 68 B8 0B 00 00 68 B8 0B 00 00 68 ?? ?? ?? ?? 68 C8 00 00 00 6A 03 E8 ?? ?? ?? ??" },
      },
      {
          { 0x01, 4, kConst, 0x000003E8, 0x00002710, "pool 0 a5" },
          { 0x08, 4, kConst, 0x00002710, 0x000130B0, "pool 0 a3" },
          { 0x0D, 1, kConst, 0x00000064, 0x00000000, "pool 0 a2 (imm8: wrapper)" },
          { 0x2B, 1, kConst, 0x00000064, 0x00000000, "pool 0 params a2 (imm8: wrapper)" },
          { 0x3E, 4, kConst, 0x000186A0, 0x000F4240, "pool 1 a3" },
          { 0x7B, 4, kConst, 0x000003E8, 0x00002710, "pool 2 a4" },
          { 0x80, 4, kConst, 0x000003E8, 0x00002710, "pool 2 a3" },
          { 0x85, 1, kConst, 0x00000064, 0x00000000, "pool 2 a2 (imm8: wrapper)" },
          { 0xA9, 4, kConst, 0x0000C350, 0x00030D40, "pool 3 a3" },
      } },
    { kSetHiRes, "pools_min", true, 0x2EB6B6, 0x2F1E96, 1,
      {
          { 0x00, "85 FF 74 3A 6A 00 51 C7 04 24 00 24 74 49 6A ?? FF 75 0C E8 ?? ?? ?? ??" },
      },
      {
          { 0x0F, 1, kConst, 0x00000050, 0x0000007C, "pool minimum count (push)" },
      } },
    // 18r: the animator object's create (if the global is 0: new 0x1920 bytes, construct, store) and,
    // 40h on, its delete (clean-up, then the global back to 0) - read only: the global tells whether
    // the object (and with it the animator pool) exists. Steam rva = GOG + 0x5620 (as hires_alloc /
    // hires_cleanup next to it); found by pattern either way.
    { kSetHiRes, "anim_object", false, 0x2133D0, 0x2189F0, 1,
      {
          { 0x00, "83 3D ?? ?? ?? ?? 00 75 25 68 20 19 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0D 8B C8 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? C3 33 C0 A3 ?? ?? ?? ?? C3" },
          { 0x40, "8B 0D ?? ?? ?? ?? 85 C9 74 11 6A 01 E8 ?? ?? ?? ?? C7 05 ?? ?? ?? ?? 00 00 00 00 C3" },
      },
      {
          { 0x02, 4, kGlobal, 0x009EC350, 0x00000000, "animator object (create: cmp)" },
          { 0x0A, 4, kConst, 0x00001920, 0x00000000, "animator object size" },
          { 0x22, 4, kGlobal, 0x009EC350, 0x00000000, "animator object (create: store)" },
          { 0x2A, 4, kGlobal, 0x009EC350, 0x00000000, "animator object (create: store 0)" },
          { 0x42, 4, kGlobal, 0x009EC350, 0x00000000, "animator object (delete: load)" },
          { 0x53, 4, kGlobal, 0x009EC350, 0x00000000, "animator object (delete: store 0)" },
      } },
    { kSetMatrix, "matrix_alloc_n", false, 0x26E9C7, 0x275397, 1,
      {
          { 0x00, "8B 35 ?? ?? ?? ?? 8B C6 C1 E0 06 05 ?? ?? ?? ?? 8D 0C 16 81 F9 ?? ?? ?? ?? 1B C9 23 CE" },
      },
      {
          { 0x02, 4, kGlobal, 0x009F7DBC, 0x00000000, "matrix index" },
          { 0x0C, 4, kGlobal, 0x00A15BA0, 0x00000000, "matrix pool start" },
          { 0x15, 4, kConst, 0x00000BF6, 0x0000C350, "matrix count" },
      } },
    { kSetMatrix, "matrix_alloc_1", false, 0x26EA43, 0x275413, 1,
      {
          { 0x00, "8B 0D ?? ?? ?? ?? 56 8B F1 C1 E6 06 81 C6 ?? ?? ?? ?? 8D 41 01 3D ?? ?? ?? ?? 8B C6 1B D2" },
      },
      {
          { 0x02, 4, kGlobal, 0x009F7DBC, 0x00000000, "matrix index" },
          { 0x0E, 4, kGlobal, 0x00A15BA0, 0x00000000, "matrix pool start" },
          { 0x16, 4, kConst, 0x00000BF6, 0x0000C350, "matrix count" },
      } },
    { kSetMatrix, "matrix_range", false, 0x2724A3, 0x278E73, 1,
      {
          { 0x00, "8B 45 08 3D ?? ?? ?? ?? 72 0B 3D ?? ?? ?? ?? 73 04" },
      },
      {
          { 0x04, 4, kGlobal, 0x00A15BA0, 0x00000000, "matrix pool start" },
          { 0x0B, 4, kGlobal, 0x00A45920, 0x00000000, "matrix pool end" },
      } },
    { kSetDlc, "dlc_add", false, 0x0C6CCF, 0x0CB09F, 1,
      {
          { 0x00, "81 3D ?? ?? ?? ?? ?? ?? ?? ?? 0F 8D B7 00 00 00 53 8D 4D FC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 69 0D ?? ?? ?? ?? 44 01 00 00 69 1D ?? ?? ?? ?? 44 01 00 00 57 89 81 ?? ?? ?? ?? 8D 4D FC E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 8B CF 89 83 ?? ?? ?? ?? 8D 51 01 8A 01 41 84 C0 75 F9 2B CA 8D 83 ?? ?? ?? ?? 51 57 50 E8 ?? ?? ?? ?? 69 05 ?? ?? ?? ?? 44 01 00 00 68 04 01 00 00 68 ?? ?? ?? ??" },
          { 0x80, "05 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 83 C4 18 69 C1 44 01 00 00 41 89 0D ?? ?? ?? ?? 8D 4D FC 57 C6 80 ?? ?? ?? ?? 00 89 B0 ?? ?? ?? ?? E8 ?? ??" },
      },
      {
          { 0x02, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x06, 4, kConst, 0x00000200, 0x00001000, "DLC cap (cmp)" },
          { 0x22, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x2C, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x37, 4, kList, 0x0086B6F8, 0x00000000, "list +0x00" },
          { 0x4E, 4, kList, 0x0086B6FC, 0x00000000, "list +0x04" },
          { 0x60, 4, kList, 0x0086B700, 0x00000000, "list +0x08" },
          { 0x6E, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x81, 4, kList, 0x0086B732, 0x00000000, "list +0x3A" },
          { 0x7C, 4, kGlobal, 0x00893EF8, 0x00000000, "path buffer (right after the list)" },
          { 0x8D, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x9D, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0xA7, 4, kList, 0x0086B835, 0x00000000, "list +0x13D" },
          { 0xAE, 4, kList, 0x0086B838, 0x00000000, "list +0x140" },
      } },
    { kSetDlc, "dlc_reset", false, 0x0C6E60, 0x0CB230, 1,
      {
          { 0x00, "55 8B EC 81 EC 30 03 00 00 C7 05 ?? ?? ?? ?? 00 00 00 00 56 57 8B 7D 08 8B CF" },
      },
      {
          { 0x0B, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
      } },
    { kSetDlc, "dlc_get_path", false, 0x0C70F0, 0x0CB4C0, 1,
      {
          { 0x00, "E8 ?? ?? ?? ?? 84 C0 74 10 69 05 ?? ?? ?? ?? 44 01 00 00 05 ?? ?? ?? ?? C3" },
      },
      {
          { 0x0B, 4, kGlobal, 0x00837C68, 0x00000000, "current DLC index" },
          { 0x14, 4, kList, 0x0086B732, 0x00000000, "list +0x3A" },
      } },
    { kSetDlc, "dlc_get_field", false, 0x0C7150, 0x0CB520, 1,
      {
          { 0x00, "E8 ?? ?? ?? ?? 84 C0 74 11 69 05 ?? ?? ?? ?? 44 01 00 00 8B 80 ?? ?? ?? ?? C3" },
      },
      {
          { 0x0B, 4, kGlobal, 0x00837C68, 0x00000000, "current DLC index" },
          { 0x15, 4, kList, 0x0086B838, 0x00000000, "list +0x140" },
      } },
    { kSetDlc, "dlc_find_a", false, 0x0C71C0, 0x0CB590, 2,
      {
          { 0x00, "55 8B EC 8B 15 ?? ?? ?? ?? 33 C9 56 85 D2 7E 16 8B 75 08 B8 ?? ?? ?? ?? 39 30 74 0F 41 05 44 01 00 00 3B CA 7C F2" },
      },
      {
          { 0x05, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x14, 4, kList, 0x0086B6FC, 0x00000000, "list +0x04" },
      } },
    { kSetDlc, "dlc_find_b", false, 0x0C72C0, 0x0CB690, 2,
      {
          { 0x00, "55 8B EC 8B 15 ?? ?? ?? ?? 33 C0 C7 05 ?? ?? ?? ?? FF FF FF FF 85 D2 7E 21 56 8B 75 08 B9 ?? ?? ?? ?? 39 31 74 0E 40 81 C1 44 01 00 00 3B C2 7C F1" },
      },
      {
          { 0x05, 4, kGlobal, 0x0086B6F0, 0x00000000, "DLC count" },
          { 0x0D, 4, kGlobal, 0x00837C68, 0x00000000, "current DLC index" },
          { 0x1E, 4, kList, 0x0086B6F8, 0x00000000, "list +0x00 or +0x04" },
      } },
};

constexpr int kGroupCount = static_cast<int>(sizeof(kGroups) / sizeof(kGroups[0]));
constexpr int kMaxHits = 4;
constexpr uint32_t kDlcEntry = 0x144;       // 2020 build's DLC entry size
constexpr uint32_t kDlcCap = 0x200;         // 512
constexpr uint32_t kDlcNewCap = 0x1000;     // 4096 (BF1MemExt's)
constexpr uint32_t kMatrixCount = 0xBF6;    // 3062
constexpr uint32_t kMatrixNewCount = 50000; // BF1MemExt's storage, counted in matrices
constexpr uint32_t kMatrixBytes = 0x40;
constexpr uint32_t kPreferredBase = 0x00400000;

// the render pool calls that pass a one-byte 100 (pools_init offsets): create(type 0), params(type 0),
// create(type 2) - each goes through a wrapper
constexpr uint8_t kPoolCreate0Call = 0x10;
constexpr uint8_t kPoolParams0Call = 0x2E;
constexpr uint8_t kPoolCreate2Call = 0x88;

// 18r: the main heap pointer (heap_main +0Bh stores it; nothing else in the exe touches it)
constexpr uint32_t kHeapPtrRvaGog = 0x0053FEE8;
constexpr uint32_t kHeapPtrRvaSteam = 0x00581F18; // 16a r2's log (the exe file's 0x00981F18); GOG + 0x42030
constexpr uint32_t kHeapPtrSite = 0x0B;
constexpr uint32_t kHeapCodeLen = 0x49;          // heap_main +00h .. +48h (ret)

// 18r: code the hi-res units are not written from inside (offsets from the groups, measured on GOG 1.3)
struct CodeSpan
{
    const char* group;
    int32_t from;   // the function's start, from the group
    uint32_t len;
    const char* what;
};
const CodeSpan kHiResSpans[] = {
    { "pools_init", -0x33, 0x3A4, "the render pool set-up" },
    { "pools_min", -0x66, 0xC2, "the pool create function" },
    { "hires_alloc", -0x950, 0xB58, "the animator set-up" },
    { "hires_cleanup", -0x10, 0x57, "the animator clean-up" },
};
constexpr int kHiResSpanCount = static_cast<int>(sizeof(kHiResSpans) / sizeof(kHiResSpans[0]));

// the matrix pool and the DLC list, in this DLL
alignas(16) unsigned char g_matrixStorage[kMatrixNewCount * kMatrixBytes];
alignas(16) unsigned char g_dlcStorage[kDlcNewCap * kDlcEntry];

typedef int(__cdecl* PoolCreate_t)(int type, int count, int a3, int a4, int a5);
typedef void(__cdecl* PoolParams_t)(int type, int p1, int p2, int p3);
PoolCreate_t g_poolCreate = nullptr;
PoolParams_t g_poolParams = nullptr;

int __cdecl PoolCreateWrap(int type, int count, int a3, int a4, int a5)
{
    if ((type == 0 || type == 2) && count == 0x64)
        count = 0x3E8; // BF1MemExt: 100 -> 1000
    return g_poolCreate(type, count, a3, a4, a5);
}

void __cdecl PoolParamsWrap(int type, int p1, int p2, int p3)
{
    if (type == 0 && p1 == 0x64)
        p1 = 0x3E8;
    g_poolParams(type, p1, p2, p3);
}

struct Found
{
    int hits;
    uint32_t rva[kMaxHits];
    bool ok;
};

Found g_found[kGroupCount];
bool g_setOn[kSetCount] = { true, true, true, true };
bool g_iniRead = false;
// 22c: [Limits] MainHeapMB - the main heap's two sites (size push, end lea) take it instead of 128 MB
constexpr uint32_t kMainHeapStock = 0x04000000;
constexpr uint32_t kMainHeapRaised = 0x08000000;
uint32_t g_mainHeapBytes = kMainHeapRaised;
char g_raisedHeaps[160] = "main heap 64 -> 128 MB, debug heap 2 -> 4 MB, application heap 15.25 -> 20 MB";

// a site's raised value (22c: the main heap's from MainHeapMB)
uint32_t Raised(const Site& st)
{
    return (st.kind == kConst && st.size == 4 && st.value == kMainHeapStock && st.raised == kMainHeapRaised) ? g_mainHeapBytes
                                                                                                           : st.raised;
}

// what the early patching did (filled while the main thread may be paused: no logging there)
// 18r: per set, with how and when
enum AddrState
{
    kAddrNotMoved, // the exe is at 0x00400000: both forms are the same
    kAddrUnfixed,  // the heap code still had the exe file's address (before Steam's fix-ups)
    kAddrFixed     // fixed up (after Steam's fix-ups)
};
struct EarlyResult
{
    bool tried;             // GOG 1.3: the at-load write ran
    bool done[kSetCount];
    int writes[kSetCount];
    int failedWrites[kSetCount];
    char why[kSetCount][360];
    // Steam heaps (the early thread)
    double tReadable;       // code section unlocked, ms after load
    double tHeapCode;       // heap code readable
    double tWrite;          // heaps written
    double tPausedUs;       // the pause that wrote them
    int attempts;           // pauses
    int busy;               // pauses that found the main thread inside heap_main
    int addrState;
    uint32_t heapPtr;       // the main heap pointer when it was already made
    uint32_t eip;           // the main thread's EIP (rva if in the exe) at the write
    bool eipInExe;
};
EarlyResult g_early;
HANDLE g_mainThread = nullptr;
DWORD g_mainThreadId = 0;
double g_t0 = 0.0;          // LimitsScanEarly (DllMain): "after load"

bool Readable(uintptr_t start, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (start < 0x10000 || !VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    return start + len <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

uint32_t ReadAt(uint32_t rva, int size)
{
    const uintptr_t p = Rva(rva);
    if (!Readable(p, static_cast<size_t>(size)))
        return 0xFFFFFFFFu;
    if (size == 1)
        return *reinterpret_cast<const uint8_t*>(p);
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(p), sizeof(v));
    return v;
}

// writes size bytes at rva, one page at a time (each page's protection put back as it was): no
// logging, no heap - safe while the main thread is paused
bool WriteAt(uint32_t rva, const void* bytes, size_t size)
{
    const auto* src = static_cast<const unsigned char*>(bytes);
    uintptr_t p = Rva(rva);
    while (size)
    {
        const size_t room = 0x1000 - (p & 0xFFF);
        const size_t n = size < room ? size : room;
        DWORD old = 0;
        if (!VirtualProtect(reinterpret_cast<void*>(p), n, PAGE_EXECUTE_READWRITE, &old))
            return false;
        memcpy(reinterpret_cast<void*>(p), src, n);
        DWORD tmp = 0;
        VirtualProtect(reinterpret_cast<void*>(p), n, old, &tmp);
        FlushInstructionCache(GetCurrentProcess(), reinterpret_cast<void*>(p), n);
        p += n;
        src += n;
        size -= n;
    }
    return true;
}

bool WriteValue(uint32_t rva, int size, uint32_t value)
{
    if (size == 1)
    {
        const uint8_t b = static_cast<uint8_t>(value);
        return WriteAt(rva, &b, 1);
    }
    return WriteAt(rva, &value, 4);
}

// an address the code uses -> rva in the exe (0 if outside it)
uint32_t AddrRva(uint32_t va)
{
    const BuildInfo* b = CurrentBuild();
    const uintptr_t base = ExeBase();
    if (!b || va < base || va >= base + b->sizeOfImage)
        return 0;
    return static_cast<uint32_t>(va - base);
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

bool SkipBuild()
{
    return !IsSteam() && !IsGog13(); // 1.2: BF1MemExt itself; unknown builds: nothing
}

double NowMs()
{
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return static_cast<double>(t.QuadPart) * 1000.0 / static_cast<double>(freq.QuadPart);
}

void ReadIni()
{
    if (g_iniRead)
        return;
    g_iniRead = true;
    char path[MAX_PATH];
    const DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (!n || n >= MAX_PATH)
        return;
    char* slash = strrchr(path, '\\');
    if (!slash)
        return;
    snprintf(slash + 1, static_cast<size_t>(MAX_PATH - (slash + 1 - path)), "BF1GameExt.ini");
    for (int s = 0; s < kSetCount; ++s)
        g_setOn[s] = GetPrivateProfileIntA("Limits", kSetKeys[s], 1, path) != 0;
    // 22c: MainHeapMB
    UINT mb = GetPrivateProfileIntA("Limits", "MainHeapMB", 128, path);
    mb = mb < 128 ? 128 : (mb > 512 ? 512 : mb);
    g_mainHeapBytes = static_cast<uint32_t>(mb) * 1024u * 1024u;
    snprintf(g_raisedHeaps, sizeof(g_raisedHeaps), "main heap 64 -> %u MB ([Limits] MainHeapMB), debug heap 2 -> 4 MB, application "
             "heap 15.25 -> 20 MB", mb);
}

uint32_t KnownRva(const Group& g)
{
#if defined(BF1_LIMITS_TEST)
    return g.gogRva; // the wine test runs the Steam path on the GOG 1.3 exe
#else
    return IsSteam() ? g.steamRva : g.gogRva;
#endif
}

// the group's bytes at rva (both chunks) and its constants as the game has them (or already raised)
bool GroupAt(const Group& g, uint32_t rva, bool* raised)
{
    if (!MatchPatternAt(rva + g.chunk[0].off, g.chunk[0].pattern))
        return false;
    if (g.chunk[1].pattern && !MatchPatternAt(rva + g.chunk[1].off, g.chunk[1].pattern))
        return false;
    bool anyRaised = false;
    for (int s = 0; s < 14 && g.site[s].what; ++s)
    {
        const Site& st = g.site[s];
        if (st.kind != kConst)
            continue;
        const uint32_t v = ReadAt(rva + st.off, st.size);
        if (v == st.value)
            continue;
        if (st.raised && v == Raised(st))
        {
            anyRaised = true;
            continue;
        }
        return false;
    }
    if (raised)
        *raised = anyRaised;
    return true;
}

const Group* GroupNamed(const char* name)
{
    for (const Group& g : kGroups)
        if (!strcmp(g.name, name))
            return &g;
    return nullptr;
}

// ----- early: heaps and hi-res units (plain numbers), before the game uses them ----------------------

uint32_t HeapPtrRva()
{
#if defined(BF1_LIMITS_TEST)
    return kHeapPtrRvaGog;
#else
    return IsSteam() ? kHeapPtrRvaSteam : kHeapPtrRvaGog;
#endif
}

// writes one early set (all its sites checked first): no logging (the main thread may be paused).
// false = a site was not as expected (nothing written)
bool ApplyEarlySet(Set set, int* writes, int* failed)
{
    // check everything first
    for (const Group& g : kGroups)
    {
        if (g.set != set || !g.early)
            continue;
        bool raised = false;
        if (!GroupAt(g, KnownRva(g), &raised) || raised)
            return false;
    }
    for (const Group& g : kGroups)
    {
        if (g.set != set || !g.early)
            continue;
        const uint32_t rva = KnownRva(g);
        for (int s = 0; s < 14 && g.site[s].what; ++s)
        {
            const Site& st = g.site[s];
            if (st.kind != kConst || !st.raised)
                continue;
            if (WriteValue(rva + st.off, st.size, Raised(st)))
                ++*writes;
            else
                ++*failed;
        }
        if (!strcmp(g.name, "pools_init"))
        {
            // the three one-byte 100s: their calls go through the wrappers
            const uint8_t calls[3] = { kPoolCreate0Call, kPoolParams0Call, kPoolCreate2Call };
            for (uint8_t c : calls)
            {
                const uint32_t at = rva + c;
                int32_t rel = 0;
                memcpy(&rel, reinterpret_cast<const void*>(Rva(at + 1)), 4);
                const uintptr_t target = Rva(at) + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(rel));
                const uintptr_t wrap = c == kPoolParams0Call ? reinterpret_cast<uintptr_t>(&PoolParamsWrap)
                                                             : reinterpret_cast<uintptr_t>(&PoolCreateWrap);
                if (c == kPoolParams0Call)
                    g_poolParams = reinterpret_cast<PoolParams_t>(target);
                else
                    g_poolCreate = reinterpret_cast<PoolCreate_t>(target);
                const int32_t newRel = static_cast<int32_t>(wrap - (Rva(at) + 5));
                if (WriteAt(at + 1, &newRel, 4))
                    ++*writes;
                else
                    ++*failed;
            }
        }
    }
    return true;
}

// GOG 1.3, at load: both early sets
void ApplyEarly()
{
    g_early.tried = true;
    for (int s = kSetHeaps; s <= kSetHiRes; ++s)
        if (g_setOn[s])
            g_early.done[s] = ApplyEarlySet(static_cast<Set>(s), &g_early.writes[s], &g_early.failedWrites[s]) &&
                              !g_early.failedWrites[s];
}

// 18r: a range of the exe's code (half-open)
struct Span
{
    uintptr_t lo;
    uintptr_t hi;
};

// true if eip, or a dword on the stack from esp up to the top of its committed region (a return
// address), falls inside one of the spans; also true if the stack cannot be read (then nothing is
// written). No logging, no heap: safe while the main thread is paused.
bool InsideSpans(uintptr_t eip, uintptr_t esp, const Span* spans, int n)
{
    for (int i = 0; i < n; ++i)
        if (eip >= spans[i].lo && eip < spans[i].hi)
            return true;
    MEMORY_BASIC_INFORMATION mbi;
    if (!esp || !VirtualQuery(reinterpret_cast<LPCVOID>(esp), &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT ||
        (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return true;
    uintptr_t top = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    if (top - esp > 0x100000)
        top = esp + 0x100000;
    for (uintptr_t p = esp & ~static_cast<uintptr_t>(3); p + 4 <= top; p += 4)
    {
        const uintptr_t v = *reinterpret_cast<const uint32_t*>(p);
        for (int i = 0; i < n; ++i)
            if (v >= spans[i].lo && v < spans[i].hi)
                return true;
    }
    return false;
}

// pauses the main thread and reads where it is (false: not paused)
bool PauseMain(CONTEXT* ctx)
{
    if (!g_mainThread || SuspendThread(g_mainThread) == static_cast<DWORD>(-1))
        return false;
    memset(ctx, 0, sizeof(*ctx));
    ctx->ContextFlags = CONTEXT_CONTROL;
    if (!GetThreadContext(g_mainThread, ctx)) // also waits until the thread has really stopped
    {
        ResumeThread(g_mainThread);
        return false;
    }
    return true;
}

// 18r: Steam, the heaps - any time before the game makes its main heap. Polls until the heap code is
// readable (Steam unlocks in stages), then: pause the main thread, check the main heap pointer is
// still 0 and the thread is not inside heap_main, write, resume. Retried until written or the heap
// exists (60 s at most).
DWORD WINAPI SteamEarlyThread(LPVOID)
{
    const Group* hm = GroupNamed("heap_main");
    const Group* ha = GroupNamed("heap_app");
    char* const why = g_early.why[kSetHeaps];
    if (!hm || !ha)
    {
        snprintf(why, sizeof(g_early.why[0]), "internal: heap groups missing");
        return 0;
    }
    const uint32_t hmRva = KnownRva(*hm);
    const uint32_t haRva = KnownRva(*ha);
    const uint32_t ptrRva = HeapPtrRva();
    const uint32_t opUnfixed = kPreferredBase + ptrRva;
    const uint32_t opFixed = static_cast<uint32_t>(ExeBase() + ptrRva);
    const Span heapCode = { Rva(hmRva) + 1, Rva(hmRva) + kHeapCodeLen }; // +0 itself: the push has not run
    while (NowMs() - g_t0 < 60000.0)
    {
        if (!g_early.tReadable && CodeLooksUnlocked())
            g_early.tReadable = NowMs() - g_t0;
        bool r1 = false;
        bool r2 = false;
        if (!GroupAt(*hm, hmRva, &r1) || !GroupAt(*ha, haRva, &r2))
        {
            SwitchToThread();
            continue;
        }
        if (!g_early.tHeapCode)
            g_early.tHeapCode = NowMs() - g_t0;
        if (r1 || r2)
        {
            snprintf(why, sizeof(g_early.why[0]), "the heap values were already raised when the code became readable");
            return 0;
        }
        const uint32_t op = ReadAt(hmRva + kHeapPtrSite, 4);
        if (op != opUnfixed && op != opFixed)
        {
            snprintf(why, sizeof(g_early.why[0]),
                     "the main heap code's pointer operand reads 0x%08X - neither the exe file's (0x%08X) nor the fixed-up "
                     "one (0x%08X) for the pointer at rva 0x%06X: left as they are",
                     static_cast<unsigned>(op), static_cast<unsigned>(opUnfixed), static_cast<unsigned>(opFixed),
                     static_cast<unsigned>(ptrRva));
            return 0;
        }
        const uint32_t ptr = ReadAt(ptrRva, 4);
        if (ptr)
        {
            g_early.heapPtr = ptr;
            snprintf(why, sizeof(g_early.why[0]),
                     "%s (pointer 0x%08X at rva 0x%06X; %.0f ms after load; code readable %.0f ms, heap code %.0f ms; %d "
                     "pause(s), %d inside the heap code): left as they are",
                     g_early.busy ? "the game made its main heap while BF1GameExt waited for the main thread to leave "
                                    "heap_main"
                                  : "the game had already made its main heap when its code was first readable",
                     static_cast<unsigned>(ptr), static_cast<unsigned>(ptrRva), NowMs() - g_t0, g_early.tReadable,
                     g_early.tHeapCode, g_early.attempts, g_early.busy);
            return 0;
        }
        CONTEXT ctx;
        if (!PauseMain(&ctx))
        {
            snprintf(why, sizeof(g_early.why[0]), "could not pause the main thread (error %lu) - left as they are",
                     GetLastError());
            return 0;
        }
        // paused: only memory checks and writes until it is resumed (no logging, no formatting)
        ++g_early.attempts;
        const double p0 = NowMs();
        const uint32_t ptrNow = ReadAt(ptrRva, 4);
        const uint32_t opNow = ReadAt(hmRva + kHeapPtrSite, 4);
        const bool inside = InsideSpans(ctx.Eip, ctx.Esp, &heapCode, 1);
        int w = 0;
        int f = 0;
        bool wrote = false;
        if (!ptrNow && !inside)
            wrote = ApplyEarlySet(kSetHeaps, &w, &f);
        const double p1 = NowMs();
        ResumeThread(g_mainThread);
        if (wrote)
        {
            g_early.done[kSetHeaps] = !f;
            g_early.writes[kSetHeaps] = w;
            g_early.failedWrites[kSetHeaps] = f;
            g_early.tWrite = p0 - g_t0;
            g_early.tPausedUs = (p1 - p0) * 1000.0;
            g_early.addrState = opUnfixed == opFixed ? kAddrNotMoved : (opNow == opUnfixed ? kAddrUnfixed : kAddrFixed);
            g_early.eipInExe = AddrRva(ctx.Eip) != 0;
            g_early.eip = g_early.eipInExe ? AddrRva(ctx.Eip) : static_cast<uint32_t>(ctx.Eip);
            if (f)
                snprintf(why, sizeof(g_early.why[0]), "%d of %d writes failed", f, w + f);
            return 0;
        }
        if (ptrNow)
        {
            g_early.heapPtr = ptrNow;
            snprintf(why, sizeof(g_early.why[0]),
                     "the game made its main heap (pointer 0x%08X) while BF1GameExt was waiting for the main thread to "
                     "leave the heap code (%.0f ms after load; heap code readable %.0f ms; %d pause(s), %d inside): left "
                     "as they are",
                     static_cast<unsigned>(ptrNow), NowMs() - g_t0, g_early.tHeapCode, g_early.attempts, g_early.busy);
            return 0;
        }
        if (inside)
            ++g_early.busy;
        else
        {
            // not inside, pointer 0, yet nothing written: a site changed under us
            snprintf(why, sizeof(g_early.why[0]), "the heap sites were not all as expected when the main thread was paused");
            return 0;
        }
        Sleep(1); // inside heap_main: look again in a millisecond (until written or the pointer is set)
    }
    snprintf(why, sizeof(g_early.why[0]), "the heap code was not readable within 60 s - left as they are");
    return 0;
}

// ----- the scan (16a) ---------------------------------------------------------------------------

void FindGroup(int gi)
{
    const Group& g = kGroups[gi];
    Found& f = g_found[gi];
    memset(&f, 0, sizeof(f));
    f.hits = FindPattern(g.chunk[0].pattern, f.rva, kMaxHits);
    int okHits = 0;
    for (int h = 0; h < f.hits && h < kMaxHits; ++h)
    {
        bool ok = GroupAt(g, f.rva[h], nullptr);
        for (int s = 0; s < 14 && g.site[s].what && ok; ++s)
            if (g.site[s].kind != kConst && !AddrRva(ReadAt(f.rva[h] + g.site[s].off, 4)))
                ok = false;
        if (ok)
            ++okHits;
    }
    f.ok = f.hits == g.hits && okHits == g.hits;
    char where[64] = "";
    size_t n = 0;
    for (int h = 0; h < f.hits && h < kMaxHits; ++h)
        n += static_cast<size_t>(snprintf(where + n, sizeof(where) - n, "%s0x%06X", h ? ", " : "",
                                          static_cast<unsigned>(f.rva[h])));
    Log("[limits] %-15s %s: %d match%s (want %d)%s%s", g.name, f.ok ? "OK      " : "MISMATCH", f.hits,
        f.hits == 1 ? "" : "es", g.hits, f.hits ? " at rva " : "", where);
    if (f.ok)
        return; // 16a logged every site; 16b only the ones that are off
    for (int h = 0; h < f.hits && h < kMaxHits && h < g.hits; ++h)
    {
        for (int s = 0; s < 14 && g.site[s].what; ++s)
        {
            const Site& st = g.site[s];
            const uint32_t v = ReadAt(f.rva[h] + st.off, st.size);
            if (st.kind == kConst)
                Log("[limits]     %-36s 0x%08X %s", st.what, static_cast<unsigned>(v),
                    v == st.value ? "as expected" : (st.raised && v == Raised(st) ? "raised" : "UNEXPECTED"));
            else
                Log("[limits]     %-36s address rva 0x%06X%s", st.what, static_cast<unsigned>(AddrRva(v)),
                    AddrRva(v) ? "" : " - NOT IN THE EXE");
        }
    }
}

uint32_t SiteValue(const char* name, int s, int hit = 0)
{
    for (int gi = 0; gi < kGroupCount; ++gi)
        if (!strcmp(kGroups[gi].name, name) && g_found[gi].hits > hit)
            return ReadAt(g_found[gi].rva[hit] + kGroups[gi].site[s].off, kGroups[gi].site[s].size);
    return 0;
}

uint32_t SiteRva(const char* name, int s, int hit = 0)
{
    for (int gi = 0; gi < kGroupCount; ++gi)
        if (!strcmp(kGroups[gi].name, name) && g_found[gi].hits > hit)
            return g_found[gi].rva[hit] + kGroups[gi].site[s].off;
    return 0;
}

bool GroupOk(const char* name)
{
    for (int gi = 0; gi < kGroupCount; ++gi)
        if (!strcmp(kGroups[gi].name, name))
            return g_found[gi].ok;
    return false;
}

// ----- late: matrix pool and DLC list (addresses), on the game's thread ------------------------------

bool MatrixReady(char* d, size_t len)
{
    const uint32_t startN = SiteValue("matrix_alloc_n", 1);
    const uint32_t start1 = SiteValue("matrix_alloc_1", 1);
    const uint32_t startR = SiteValue("matrix_range", 0);
    const uint32_t endR = SiteValue("matrix_range", 1);
    const uint32_t idxN = SiteValue("matrix_alloc_n", 0);
    const uint32_t idx1 = SiteValue("matrix_alloc_1", 0);
    const bool same = startN && startN == start1 && startN == startR && idxN == idx1 &&
                      endR == startN + kMatrixCount * kMatrixBytes;
    snprintf(d, len, "pool at rva 0x%06X, %u matrices (%s)", static_cast<unsigned>(AddrRva(startN)),
             static_cast<unsigned>(kMatrixCount), same ? "all 3 places agree" : "THE PLACES DISAGREE");
    return GroupOk("matrix_alloc_n") && GroupOk("matrix_alloc_1") && GroupOk("matrix_range") && same;
}

void ApplyMatrix()
{
    const uint32_t start = reinterpret_cast<uintptr_t>(g_matrixStorage);
    const uint32_t end = start + kMatrixNewCount * kMatrixBytes;
    int w = 0;
    w += WriteValue(SiteRva("matrix_alloc_n", 1), 4, start);
    w += WriteValue(SiteRva("matrix_alloc_1", 1), 4, start);
    w += WriteValue(SiteRva("matrix_range", 0), 4, start);
    w += WriteValue(SiteRva("matrix_range", 1), 4, end);
    w += WriteValue(SiteRva("matrix_alloc_n", 2), 4, kMatrixNewCount);
    w += WriteValue(SiteRva("matrix_alloc_1", 2), 4, kMatrixNewCount);
    Log("[limits] matrix pool RAISED: %u -> %u matrices, now in BF1GameExt at 0x%08X (%u KB); %d of 6 places written",
        static_cast<unsigned>(kMatrixCount), static_cast<unsigned>(kMatrixNewCount), static_cast<unsigned>(start),
        static_cast<unsigned>(kMatrixNewCount * kMatrixBytes / 1024), w);
}

bool DlcReady(char* d, size_t len, uint32_t* countVa, uint32_t* listVa)
{
    const uint32_t count = SiteValue("dlc_add", 0);
    const uint32_t list = SiteValue("dlc_add", 4);
    const uint32_t buffer = SiteValue("dlc_add", 9);
    const uint32_t cap = SiteValue("dlc_add", 1);
    bool same = count && list;
    static const uint32_t kFields[] = { 0x00, 0x04, 0x08, 0x3A, 0x13D, 0x140 };
    int countRefs = 0;
    int listRefs = 0;
    for (int gi = 0; gi < kGroupCount; ++gi)
    {
        const Group& g = kGroups[gi];
        if (g.set != kSetDlc)
            continue;
        for (int h = 0; h < g_found[gi].hits && h < g.hits; ++h)
        {
            for (int s = 0; s < 14 && g.site[s].what; ++s)
            {
                const uint32_t v = ReadAt(g_found[gi].rva[h] + g.site[s].off, 4);
                if (g.site[s].kind == kList)
                {
                    ++listRefs;
                    bool known = false;
                    for (uint32_t fo : kFields)
                        known = known || v == list + fo;
                    same = same && known;
                }
                else if (g.site[s].kind == kGlobal && strstr(g.site[s].what, "DLC count"))
                {
                    ++countRefs;
                    same = same && v == count;
                }
            }
        }
    }
    const bool layout = buffer == list + kDlcCap * kDlcEntry;
    bool ok = same && layout && cap == kDlcCap && countRefs == 11 && listRefs == 12;
    for (int gi = 0; gi < kGroupCount; ++gi)
        if (kGroups[gi].set == kSetDlc)
            ok = ok && g_found[gi].ok;
    snprintf(d, len, "count at rva 0x%06X (%d places), list at rva 0x%06X (%d places, %s), cap %u, path buffer %s",
             static_cast<unsigned>(AddrRva(count)), countRefs, static_cast<unsigned>(AddrRva(list)), listRefs,
             same ? "all agree" : "THEY DISAGREE", static_cast<unsigned>(cap), layout ? "right after the list" : "NOT where expected");
    *countVa = count;
    *listVa = list;
    return ok;
}

void ApplyDlc(uint32_t countVa, uint32_t listVa)
{
    const uint32_t newList = reinterpret_cast<uintptr_t>(g_dlcStorage);
    uint32_t n = 0;
    memcpy(&n, reinterpret_cast<const void*>(static_cast<uintptr_t>(countVa)), 4);
    if (n > kDlcCap)
        n = kDlcCap;
    memcpy(g_dlcStorage, reinterpret_cast<const void*>(static_cast<uintptr_t>(listVa)), n * kDlcEntry);
    int w = 0;
    for (int gi = 0; gi < kGroupCount; ++gi)
    {
        const Group& g = kGroups[gi];
        if (g.set != kSetDlc)
            continue;
        for (int h = 0; h < g_found[gi].hits && h < g.hits; ++h)
        {
            for (int s = 0; s < 14 && g.site[s].what; ++s)
            {
                if (g.site[s].kind != kList)
                    continue;
                const uint32_t at = g_found[gi].rva[h] + g.site[s].off;
                const uint32_t v = ReadAt(at, 4);
                w += WriteValue(at, 4, newList + (v - listVa));
            }
        }
    }
    w += WriteValue(SiteRva("dlc_add", 1), 4, kDlcNewCap);
    Log("[limits] DLC missions RAISED: %u -> %u, the list now in BF1GameExt at 0x%08X (%u KB, the %u missions "
        "already in it copied); %d of 13 places written",
        static_cast<unsigned>(kDlcCap), static_cast<unsigned>(kDlcNewCap), static_cast<unsigned>(newList),
        static_cast<unsigned>(kDlcNewCap * kDlcEntry / 1024), static_cast<unsigned>(n), w);
}

const char* const kRaisedHeaps = g_raisedHeaps; // 22c: with MainHeapMB
const char* const kRaisedHiRes = "soldier animators 10 -> 100, render pools to BF1MemExt's sizes (3 calls through wrappers), "
                                 "pool minimum 80 -> 124";

// 18r: what was done before the game used them (GOG 1.3: both sets at load; Steam: the heaps)
void LogEarly()
{
    if (IsGog13())
    {
        for (int s = kSetHeaps; s <= kSetHiRes; ++s)
        {
            if (!g_setOn[s])
                Log("[limits] %s: off ([Limits] %s=0)", kSetNames[s], kSetKeys[s]);
            else if (!g_early.tried)
                Log("[limits] %s NOT RAISED: %s", kSetNames[s], g_early.why[s][0] ? g_early.why[s] : "not tried");
            else if (g_early.done[s])
                Log("[limits] %s RAISED at load (GOG 1.3, DllMain, before the game starts): %d writes - %s", kSetNames[s],
                    g_early.writes[s], s == kSetHeaps ? kRaisedHeaps : kRaisedHiRes);
            else
                Log("[limits] %s NOT RAISED: %s", kSetNames[s],
                    g_early.failedWrites[s] ? "some writes failed" : "its sites were not all as expected (left as they are)");
        }
        return;
    }
    // Steam: the heaps (the hi-res units are done next, at this DirectInput call)
    if (!g_setOn[kSetHeaps])
    {
        Log("[limits] heaps: off ([Limits] Heaps=0)");
        return;
    }
    if (!g_early.done[kSetHeaps])
    {
        Log("[limits] heaps NOT RAISED: %s", g_early.why[kSetHeaps][0] ? g_early.why[kSetHeaps] : "not tried");
        return;
    }
    const char* addr = g_early.addrState == kAddrUnfixed
                           ? "before Steam's address fix-ups"
                           : (g_early.addrState == kAddrFixed ? "after Steam's address fix-ups, before the game made its heap"
                                                              : "the exe was not moved (no fix-ups to tell by)");
    Log("[limits] heaps RAISED by the early thread %.0f ms after load (code readable %.0f ms, heap code %.0f ms), %s: "
        "main thread paused %.0f microseconds (pause %d; %d found it inside heap_main), main heap pointer 0, its EIP %s "
        "0x%08X; %d writes - %s",
        g_early.tWrite, g_early.tReadable, g_early.tHeapCode, addr, g_early.tPausedUs, g_early.attempts, g_early.busy,
        g_early.eipInExe ? "rva" : "at", static_cast<unsigned>(g_early.eip), g_early.writes[kSetHeaps], kRaisedHeaps);
}

const Found* FoundNamed(const char* name, const Group** g)
{
    for (int gi = 0; gi < kGroupCount; ++gi)
        if (!strcmp(kGroups[gi].name, name))
        {
            if (g)
                *g = &kGroups[gi];
            return &g_found[gi];
        }
    return nullptr;
}

// 18r: Steam, the hi-res units - at the game's DirectInput call, while the animator object does not exist
void HiResLate(bool onGameThread)
{
    if (!g_setOn[kSetHiRes])
    {
        Log("[limits] hi-res units: off ([Limits] HiResUnits=0)");
        return;
    }
    const double t = g_t0 ? NowMs() - g_t0 : static_cast<double>(ScanMsSinceAttach());
    // the animator object global: its create / delete pair, 5 places that must agree
    const Group* ag = nullptr;
    const Found* af = FoundNamed("anim_object", &ag);
    if (!af || !af->ok)
    {
        Log("[limits] hi-res units NOT RAISED: the animator object's create / delete code was not found once (%d "
            "match(es)) - left as they are",
            af ? af->hits : 0);
        return;
    }
    const uint32_t at = af->rva[0];
    const uint32_t objVa = ReadAt(at + 0x02, 4);
    bool same = true;
    for (int s = 0; s < 14 && ag->site[s].what; ++s)
        if (ag->site[s].kind == kGlobal)
            same = same && ReadAt(at + ag->site[s].off, 4) == objVa;
    const uint32_t objRva = AddrRva(objVa);
    if (!same || !objRva)
    {
        Log("[limits] hi-res units NOT RAISED: the animator object's 5 places %s (0x%08X) - left as they are",
            same ? "point outside the exe" : "DISAGREE", static_cast<unsigned>(objVa));
        return;
    }
    // where the hi-res units are not written from inside
    Span spans[kHiResSpanCount];
    int n = 0;
    for (const CodeSpan& cs : kHiResSpans)
    {
        const Found* f = FoundNamed(cs.group, nullptr);
        if (!f || !f->ok)
        {
            Log("[limits] hi-res units NOT RAISED: %s (%s) not found - left as they are", cs.group, cs.what);
            return;
        }
        const uintptr_t lo = Rva(static_cast<uint32_t>(static_cast<int32_t>(f->rva[0]) + cs.from));
        spans[n].lo = lo;
        spans[n].hi = lo + cs.len;
        ++n;
    }
    const uint32_t obj = ReadAt(objRva, 4);
    if (obj)
    {
        Log("[limits] hi-res units NOT RAISED: the animator object already exists (0x%08X, global at rva 0x%06X) - a "
            "match was loaded before the game's DirectInput call: left as they are",
            static_cast<unsigned>(obj), static_cast<unsigned>(objRva));
        return;
    }
    const bool onMain = g_mainThreadId && GetCurrentThreadId() == g_mainThreadId;
    int w = 0;
    int fl = 0;
    bool done = false;
    bool inside = false;
    int tries = 0;
    double pausedUs = 0.0;
    uint32_t objNow = 0;
    if (onMain)
    {
        // this is the main thread, in the game's DirectInput call: the game is not running its pool or
        // animator code now (its own stack is not scanned - callers' unused slots can hold old return
        // addresses from an earlier load, which would only make this refuse for nothing)
        done = ApplyEarlySet(kSetHiRes, &w, &fl);
    }
    else
    {
        // not the main thread (the scan came from the timer): pause it, check where it is
        for (tries = 1; tries <= 500 && !done; ++tries)
        {
            CONTEXT ctx;
            if (!PauseMain(&ctx))
                break;
            const double p0 = NowMs();
            objNow = ReadAt(objRva, 4);
            inside = InsideSpans(ctx.Eip, ctx.Esp, spans, n);
            if (!objNow && !inside)
                done = ApplyEarlySet(kSetHiRes, &w, &fl);
            pausedUs = (NowMs() - p0) * 1000.0;
            ResumeThread(g_mainThread);
            if (done || objNow || (!inside && !done))
                break;
            Sleep(1);
        }
    }
    g_early.done[kSetHiRes] = done && !fl;
    g_early.writes[kSetHiRes] = w;
    g_early.failedWrites[kSetHiRes] = fl;
    if (done)
    {
        if (onMain)
            Log("[limits] hi-res units RAISED at the game's DirectInput call (%.0f ms after load, on the game's main thread; "
                "animator object not made yet - global at rva 0x%06X is 0): %d writes (%d failed) - %s",
                t, static_cast<unsigned>(objRva), w, fl, kRaisedHiRes);
        else
            Log("[limits] hi-res units RAISED at the scan (%s, %.0f ms after load, not the main thread: it was paused "
                "%.0f microseconds, try %d; animator object not made yet - global at rva 0x%06X is 0): %d writes (%d "
                "failed) - %s",
                onGameThread ? "the game's DirectInput call" : "from the timer", t, pausedUs, tries,
                static_cast<unsigned>(objRva), w, fl, kRaisedHiRes);
        return;
    }
    if (objNow)
        Log("[limits] hi-res units NOT RAISED: the animator object was made meanwhile (0x%08X) - left as they are",
            static_cast<unsigned>(objNow));
    else if (inside)
        Log("[limits] hi-res units NOT RAISED: the main thread was inside %s (left as they are)",
            onMain ? "the pool / animator code (this call came from there)" : "the pool / animator code on every try");
    else if (!onMain && !g_mainThread)
        Log("[limits] hi-res units NOT RAISED: not on the main thread and no handle to pause it - left as they are");
    else
        Log("[limits] hi-res units NOT RAISED: their sites were not all as expected (left as they are)");
}

// 18r: every early value as it reads now (raised / stock) - proof the writes stuck
void ReadBack()
{
    for (int s = kSetHeaps; s <= kSetHiRes; ++s)
    {
        if (!g_setOn[s])
            continue;
        int raised = 0;
        int stock = 0;
        int other = 0;
        int calls = 0;
        int callsWrapped = 0;
        for (const Group& g : kGroups)
        {
            if (g.set != s || !g.early)
                continue;
            const uint32_t rva = KnownRva(g);
            for (int k = 0; k < 14 && g.site[k].what; ++k)
            {
                const Site& st = g.site[k];
                if (st.kind != kConst || !st.raised)
                    continue;
                const uint32_t v = ReadAt(rva + st.off, st.size);
                if (v == Raised(st))
                    ++raised;
                else if (v == st.value)
                    ++stock;
                else
                    ++other;
            }
            if (!strcmp(g.name, "pools_init"))
            {
                const uint8_t cs[3] = { kPoolCreate0Call, kPoolParams0Call, kPoolCreate2Call };
                for (uint8_t c : cs)
                {
                    ++calls;
                    const uint32_t a = rva + c;
                    const uintptr_t target = Rva(a) + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(
                                                              static_cast<int32_t>(ReadAt(a + 1, 4))));
                    const uintptr_t wrap = c == kPoolParams0Call ? reinterpret_cast<uintptr_t>(&PoolParamsWrap)
                                                                 : reinterpret_cast<uintptr_t>(&PoolCreateWrap);
                    if (ReadAt(a, 1) == 0xE8 && target == wrap)
                        ++callsWrapped;
                }
            }
        }
        if (s == kSetHeaps)
            Log("[limits] read back: heaps %d of %d values raised (%d stock%s)", raised, raised + stock + other, stock,
                other ? ", SOME UNEXPECTED" : "");
        else
            Log("[limits] read back: hi-res units %d of %d values raised (%d stock%s), %d of %d pool calls through the "
                "wrappers",
                raised, raised + stock + other, stock, other ? ", SOME UNEXPECTED" : "", callsWrapped, calls);
    }
}

} // namespace

void LimitsScanEarly()
{
    if (SkipBuild())
        return;
    g_t0 = NowMs();
    g_mainThreadId = GetCurrentThreadId(); // DllMain: the exe's main thread
    ReadIni();
    if (!g_setOn[kSetHeaps] && !g_setOn[kSetHiRes])
        return;
    if (IsGog13())
    {
        // GOG 1.3: readable and fixed up by Windows already; the game starts after DllMain
        bool readable = true;
        for (const Group& g : kGroups)
            if (g.early && g_setOn[g.set] && !GroupAt(g, g.gogRva, nullptr))
                readable = false;
        if (readable)
            ApplyEarly();
        else
            for (int s = kSetHeaps; s <= kSetHiRes; ++s)
                snprintf(g_early.why[s], sizeof(g_early.why[s]), "the GOG 1.3 sites were not where 16a found them");
        return;
    }
    // Steam: the main thread (this one) runs Steam's unlocker next, then the game
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &g_mainThread,
                         THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, 0))
    {
        g_mainThread = nullptr;
        snprintf(g_early.why[kSetHeaps], sizeof(g_early.why[0]), "no handle to the main thread (error %lu)",
                 GetLastError());
        return; // the hi-res units can still be done at the DirectInput call, on the main thread itself
    }
    if (!g_setOn[kSetHeaps])
        return; // the hi-res units: at the DirectInput call
    const HANDLE t = CreateThread(nullptr, 0, SteamEarlyThread, nullptr, 0, nullptr);
    if (t)
        CloseHandle(t);
    else
        snprintf(g_early.why[kSetHeaps], sizeof(g_early.why[0]), "the early thread did not start (error %lu)",
                 GetLastError());
}

void LimitsScanRun(bool onGameThread)
{
    static bool ran = false;
    if (ran)
        return; // once: the matrix pool and the DLC list are moved by then
    ran = true;
    if (SkipBuild())
    {
        Log("[limits] not GOG 1.3 or Steam (1.2: BF1MemExt itself raises the limits there) - nothing done");
        return;
    }
    ReadIni();
    Log("[limits] BF1MemExt limits (16b, 18r: Steam early sets reworked): [Limits] Heaps=%d HiResUnits=%d MatrixPool=%d "
        "DLCMissions=%d; %s, %s",
        g_setOn[kSetHeaps] ? 1 : 0, g_setOn[kSetHiRes] ? 1 : 0, g_setOn[kSetMatrix] ? 1 : 0, g_setOn[kSetDlc] ? 1 : 0,
        onGameThread ? "at the game's DirectInput call" : "from the timer",
        g_mainThreadId && GetCurrentThreadId() == g_mainThreadId ? "on the main thread" : "NOT on the main thread");
    LogEarly();
    for (int gi = 0; gi < kGroupCount; ++gi)
        FindGroup(gi);
    if (IsSteam())
        HiResLate(onGameThread); // 18r: Steam's hi-res units, before any match
    ReadBack();

    char d[200];
    // matrix pool
    if (!g_setOn[kSetMatrix])
        Log("[limits] matrix pool: off ([Limits] MatrixPool=0)");
    else if (!MatrixReady(d, sizeof(d)))
        Log("[limits] matrix pool NOT RAISED: %s", d);
    else if (!onGameThread)
        Log("[limits] matrix pool NOT RAISED: only done from the game's own thread (this scan came from the timer)");
    else
        ApplyMatrix();
    // DLC missions
    uint32_t countVa = 0;
    uint32_t listVa = 0;
    if (!g_setOn[kSetDlc])
        Log("[limits] DLC missions: off ([Limits] DLCMissions=0)");
    else if (!DlcReady(d, sizeof(d), &countVa, &listVa))
        Log("[limits] DLC missions NOT RAISED: %s", d);
    else if (!onGameThread)
        Log("[limits] DLC missions NOT RAISED: only done from the game's own thread (this scan came from the timer)");
    else
        ApplyDlc(countVa, listVa);
    Log("[limits] done - the BF1MemExt fixes (spawn screen, difficulty, Galactic Conquest, campaign menu, melee "
        "damage) come after the limits");
}

// 21j-r17: the addon mission list as the game has it now - for the MODS page's ADDON FOLDER (menus.cpp). Read from
// dlc_add's own sites each time: its count global (+02h), the list it writes (+37h, "list +0x00": the game's list, or
// BF1GameExt's once DLCMissions moved it) and the cap it checks (+06h, 512 or 4096).
bool LimitsDlcView(uint32_t* count, uint32_t* cap, uint32_t* stock, uintptr_t* list)
{
    *count = 0;
    *cap = 0;
    *stock = kDlcCap;
    *list = 0;
    if (SkipBuild() || !GroupOk("dlc_add"))
        return false;
    const uint32_t countVa = SiteValue("dlc_add", 0);
    const uint32_t listVa = SiteValue("dlc_add", 4);
    const uint32_t c = SiteValue("dlc_add", 1);
    if (!countVa || countVa == 0xFFFFFFFFu || !listVa || listVa == 0xFFFFFFFFu || (c != kDlcCap && c != kDlcNewCap) ||
        !Readable(countVa, 4))
        return false;
    uint32_t n = 0;
    memcpy(&n, reinterpret_cast<const void*>(static_cast<uintptr_t>(countVa)), sizeof(n));
    if (n > c)
        return false;
    *count = n;
    *cap = c;
    *list = listVa;
    return true;
}

// 21j-r17: entry i of that list: the mission name AddDownloadableContent copied to +08h (its second argument, up to 50
// characters - strncpy of its own length, no 0 after it: a slot that held a longer name keeps that name's tail, so the name is
// cut where its hash matches +04h, the hash the game took of it) and the addon folder it was added from (+3Ah: the full path
// the addon scan handed the addme script, 0-padded to 104h).
bool LimitsDlcEntry(uintptr_t list, uint32_t i, char* mission, size_t mlen, char* path, size_t plen)
{
    if (mlen)
        mission[0] = 0;
    if (plen)
        path[0] = 0;
    const uintptr_t e = list + static_cast<uintptr_t>(i) * kDlcEntry;
    if (!list || !mlen || !plen || !Readable(e, kDlcEntry))
        return false;
    const unsigned char* name = reinterpret_cast<const unsigned char*>(e + 0x08);
    size_t k = 0;
    while (k < 0x32 && k + 1 < mlen && name[k] >= 0x20 && name[k] < 0x7F)
    {
        mission[k] = static_cast<char>(name[k]);
        ++k;
    }
    mission[k] = 0;
    uint32_t want = 0;
    memcpy(&want, reinterpret_cast<const void*>(e + 0x04), sizeof(want));
    uint32_t h = 0x811C9DC5u;
    size_t cut = 0;
    for (size_t n = 0; n < k; ++n)
    {
        // the game's name hash (GOG 1.3 0x006FA0D0, PblHash's): FNV-1a over each character | 20h - printable: no sign to extend
        h = ((static_cast<uint32_t>(mission[n]) | 0x20u) ^ h) * 0x01000193u;
        if (h == want)
            cut = n + 1; // the longest one that matches
    }
    if (cut)
        mission[cut] = 0;
    const char* from = reinterpret_cast<const char*>(e + 0x3A);
    k = 0;
    while (k < 0x103 && k + 1 < plen && from[k])
    {
        path[k] = from[k];
        ++k;
    }
    path[k] = 0;
    return true;
}

} // namespace bf1
