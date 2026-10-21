// BF1GameExt - egm.cpp   (file build p23a-r1)
// Credit: EGM's patches - the SWBF Launcher by Antstafer and abkarino (CC BY-NC 4.0: credit them, no commercial use);
//         brought into BF1GameExt by Mr. Rogue. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/antstafer_abkarino_egm/ (was egm_scan.cpp); file names in the comments and log lines follow the new names. Nothing else changed.
//
// Phase 22: EGM'S PATCHES, PORTED. 22a was the scan (log only); 22b (this build) puts them in - each group behind its
// own BF1GameExt.ini [EGM] switch, every site found by pattern and checked before anything is written, all-or-nothing
// per group (one site not as expected = that group is left alone, and the log says which and why).
//
// Credit: EGM's patches are the work of Antstafer and abkarino - the SWBF Launcher, the Enhanced Gameplay and Graphics
// Mod's memory launcher (https://github.com/abkarino/swbf-launcher), released under Creative Commons BY-NC 4.0. BF1GameExt
// carries them over to the 2020 exes (GOG 1.3 and Steam) as byte patterns; credited, and kept free (non-commercial).
//
// The launcher wrote fixed addresses for three builds (SPTest, GOG 1.3.5, Steam 1.3.7 - on a Steamless, 4 GB-patched
// exe) into a process it started suspended. BF1GameExt runs inside the game: every Steam site was matched on the
// decrypted Steam exe and turned into a pattern that finds the same instruction on GOG 1.3 too - 74 patterns (<= 113
// bytes, a concrete first byte, each found exactly once in both exes; relocated addresses, call distances, every byte
// EGM or BF1GameExt writes and every value [Limits] writes are ??), 164 sites, each checked on both exes by the sim.
//
// 22a (Mr. Rogue's Steam log, 2026-10-20): 157 sites - 148 stock, 9 [Limits]', 0 unexpected; the start-up probe ran on
// the main thread 278 ms after load, after Steam's unlock (230 ms) and its address fix-ups (271 ms), with no heap made
// and nothing built - so the early writes go there.
//
// WHEN:
//   - BEFORE THE C++ START-UP (the start-up hook): the render lists and the render item pool (RenderPools), the foliage
//     array (Foliage), the scene queue (FixEffects) - built by the game's C++ static constructors or at WinMain's start,
//     so before the DirectInput call. The C++ start-up runs the static constructors from a pointer table in .rdata
//     (_initterm over __xc_a .. __xc_z). At load (DllMain) BF1GameExt puts its own function in that table's first slot
//     that is not 0; when the game's start-up calls it (Steam: after its unlock), it finds every pattern, applies the
//     early groups, puts the slot back and calls the game's own function. GOG 1.3: the same (DllMain is before it too).
//   - AT THE GAME'S DIRECTINPUT CALL (on the game's thread, before any map; after [Limits]): everything else. Each
//     group checks first that what it changes is not in use yet (no map loaded, the sound system not started, ...).
//
// THE GROUPS (EGM's values unless noted):
//   - HighRes ([Limits] HiResUnits' sites, one owner per site): per value the larger of EGM's and BF1MemExt's - soldier
//     budget pool 0 a5 0x3E800 / a3 0x271000, vehicle pool 1 a5 / a4 0x55F000, a3 0x186A000 (LOD triangle budgets, no
//     memory), pool 0 count 1000 (BF1MemExt's, > EGM's 512), 512 soldier animators (EGM's, 4 MB per match) - with the
//     clean-up loop's end EGM missed. The one-byte counts go through BF1GameExt's own call wrappers (no code caves): the
//     animator vector constructor and the render pool create calls (pool 0 here, pool 2 for FixEffects).
//   - RenderPools: render item pool 4648 -> 24576 (1.1 MB, in BF1GameExt), sOPAQUE 1800 -> 7200, sShadow 1024 -> 4096,
//     sPostShadow 2312 -> 9248, sTRANSPARENT 800 -> 204800, sPOSTTRANSPARENT 512 -> 131072 (4 MB from the game's heap).
//   - FixedPools: the decal / AI path request / AI path / path node pools set up lazily (the game's own
//     SetSizeIfUnallocated, which 60+ pools already use): a map's own pool size takes effect.
//   - RayRequest: close units 175 -> 1000, the two ray request queues 200 -> 1000, per-team tracking 128 -> 1024 (the
//     array moves into BF1GameExt, 192 KB).
//   - MoreFollowers: squad 3 -> 9 (8 followers). MoreFlyers: the vehicle claim list 60 -> 127. MPBots: iMaxBots 32 -> 63
//     (the multiplayer host's bot cap only).
//   - Foliage: 2000 -> 8000 prop meshes (the array and its free stack move into BF1GameExt, 0.9 MB), always 254 clusters.
//   - Sound: 32 -> 127 channels, 64 -> 256 sounds, the stream mixer's per-channel arrays moved past its end - plus what
//     EGM missed: the ChannelSet destructor (its 2 counts / 2 offsets - streams leaked or left playing after an audio
//     change), init branch B and the /audiomaxchannels clamp. BF1GameExt builds its 256 SndSource3D with the game's own
//     constructor (the static constructor built the old 64 before WinMain; its exit destructor keeps destroying them).
//   - Aggro: the AI scan radius cap 350 -> 1000, the first-target field-of-view gate NOPped.
//   - Decal: the decal function's 3 loads of the shared 100.0 / 50.0 literals -> BF1GameExt's 1000.0 / 500.0.
//   - FixEffects: the scene queue 400 -> 100000 (early, 800 KB), render queue 2 100 -> 100000 (through the pool 2
//     wrapper), its drain cap 400 -> 100000; WaterEffect's slot array moved to +0x28 with its count at +0x20 (EGM's
//     rewrite, complete) - [EGM] WaterSplashes slots (default 800, EGM's own comment; EGM wrote 68736).
//   - PostHook (off by default): after each map's setup script (ScriptInit), Hooks\PostScriptInit.lua next to the exe is
//     run in the map's Lua with gMissionName set, then the game's CloseScript.
//   - Not carried: Heap (512 MB) and the large-address exe - [Limits] Heaps stays; Console - EGM's notes: not used;
//     Crouch - prone_fix fixes the same stale tap for every button and player; the matrix pool - [Limits] MatrixPool
//     (50000 > EGM's 32768), EGM's int3 'matrix full' trap left out; the launcher's plugin loader, crash log, Lua console.
//
// [EGM] Scan=1: a summary line per group (2: every site, as 22a; 0: nothing). InitProbe=0: no start-up hook (the early
// groups are then left alone).

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "egm.hpp"
#include "game_build.hpp"
#include "log.hpp"
#include "game_check.hpp"

namespace bf1 {
namespace {

enum Kind : uint8_t
{
    kConst, // a number: stock (or [Limits]' raised value)
    kFloat, // a float immediate
    kAddr,  // an address operand into the exe (array / object / literal / function)
    kCode,  // instruction bytes ("??" = a relocated dword)
    kCall,  // E8 rel32 - the callee (through its jump thunk)
    kData,  // an address operand; the dword it points at is the stock value
    kFunc   // a function's first byte
};

enum ProbeMode : uint8_t
{
    kProbeNone,
    kProbeVal,     // n dwords at the target
    kProbePtrAt,   // p = [target + off]; n dwords at p
    kProbePtrThen, // p = [target]; n dwords at p + off
    kProbeStr      // a string at the target
};

enum Act : uint8_t
{
    kActNone,
    kActSet,      // value (size 1 / 2 / 4)
    kActRebase,   // the operand -> BF1GameExt's block + offset
    kActBytes,    // code bytes ("NN NN NN NN" = the water slot count)
    kActCall,     // the E8's callee -> a BF1GameExt function / a game function
    kActPtr,      // the operand -> a BF1GameExt variable
    kActData,     // the dword the operand points at -> value (only if still stock)
    kActWaterSize // 0x28 + 4 x the water slot count
};

enum To : uint8_t
{
    kToNone,
    kToSetSize,  // the game's SetSizeIfUnallocated
    kToPoolWrap, // BF1GameExt's render pool create wrapper
    kToVecWrap,  // BF1GameExt's animator vector constructor wrapper
    kToPostHook, // BF1GameExt's PostScriptInit hook
    kToDecalFar, // BF1GameExt's 1000.0
    kToDecalFade // BF1GameExt's 500.0
};

constexpr uint32_t kSameOff = 0xFFFFFFFFu;

struct GroupInfo
{
    const char* name;
    const char* what;
    const char* when;
    int on;
    const char* note;
};

struct ChunkInfo
{
    uint8_t group;
    uint32_t gogRva;
    uint32_t steamRva;
    const char* pattern;
};

struct BlockInfo
{
    const char* name;
    uint32_t bytes;
};

struct SiteInfo
{
    uint8_t chunk;
    uint8_t off;
    uint8_t size;
    uint8_t kind;
    int8_t array;
    uint8_t probe;
    uint8_t probeN;
    uint16_t probeOff;
    uint32_t stock;
    uint32_t alt; // [Limits]' value (0: none)
    uint32_t aoff;
    uint32_t gogTarget;
    uint32_t steamTarget;
    const char* hex;
    const char* label;
    const char* egm;
    const char* plan;
    uint8_t loose;      // kCall: another BF1GameExt wrapper there is fine too
    uint8_t readyIdx;   // the probe's dwords [readyIdx, readyIdx + readyN) must be 0 before the group is applied
    uint8_t readyN;
    uint8_t readyEarly; // ... checked at the start-up hook (1) or at the DirectInput call (0)
    uint8_t act;
    uint8_t early;      // written at the start-up hook
    int8_t block;
    uint8_t to;
    uint32_t value;
    uint32_t newOff;
    const char* actHex;
};

// ---- generated by egm_gen.py / egm_emit.py from the two exes (do not edit by hand) ----
const GroupInfo kGroups[] = {
    { "HighRes", "hi-res units: no 8-unit cap, later LOD switch, more hi-res animations (merged with [Limits] HiResUnits)",
      "at the game's DirectInput call, inside [Limits]' own hi-res set (one owner per site)", 1,
      "EGM missed the clean-up loop's end (10 of 512 unlinked) - [Limits] raises it. 22b: the larger of EGM's / BF1MemExt's per value." },
    { "RenderPools", "MatrixPool's render item pool and render lists (the matrix pool itself: [Limits] MatrixPool, 50000 > EGM's 32768)",
      "the 5 render lists before the C++ start-up; the render item pool at the DirectInput call", 1,
      "Every render item pool reference is in EGM's list. Memory at EGM's sizes: lists 4 MB (sTRANSPARENT 2.3 MB), item pool 1.1 MB." },
    { "FixedPools", "decal / AI path request / AI path / path node pools: set up lazily (a map can size them)",
      "at the game's DirectInput call (before any map sets the pools up)", 1,
      "EGM points them at the game's own lazy SetSizeIfUnallocated (60+ pools use it): a map's own pool size takes effect." },
    { "RayRequest", "every AI active (the ~100 active AI cap): close units, ray request queues, per-team tracking",
      "at the game's DirectInput call (no map loaded: every team count 0, no close-units array)", 1,
      "Every tracking array reference is in EGM's list; the close-units array and its count change together." },
    { "MoreFollowers", "'Follow me' takes 8 units, not 2",
      "at the game's DirectInput call", 1,
      "The squad is counted, not stored: nothing to overflow." },
    { "MoreFlyers", "vehicle AI claim list 60 -> 127 (the ~20 vehicles cap)",
      "at the game's DirectInput call (the list is made at each map load)", 1,
      "The list's append has no capacity check (stock too): 127 only moves that line." },
    { "MPBots", "multiplayer host's bot cap 32 -> 63 (the MP game options only)",
      "at the game's DirectInput call", 1,
      "iMaxBots caps the multiplayer host's bots only (ifs_mp_gameopts.lua); EGM's launcher never applied it (its checkbox calls 'AIUnlock', not in its source)." },
    { "Foliage", "foliage: 2000 -> 8000 prop meshes, always 254 clusters",
      "the array move before the C++ start-up; the cluster NOP at the DirectInput call", 1,
      "Every reference is in EGM's list; the element constructor sets a vtable (before the C++ start-up). GrassScan's 4096 blink slots may fill." },
    { "Sound", "sound: 32 -> 127 channels, 64 -> 256 sounds (fewer blaster sounds cut out)",
      "at the game's DirectInput call (the sound system starts right after it), BF1GameExt's 256 SndSource3D built then", 1,
      "EGM MISSED: the ChannelSet destructor (4), init branch B (2), the /audiomaxchannels clamp (1) - 22b patches them too." },
    { "Aggro", "AI see and shoot to their weapon's max range (350 -> 1000 m cap)",
      "at the game's DirectInput call", 1,
      "Scan radius = min(max(best weapon range, 75), 350): the cap 350 -> 1000, the first-target view gate NOPped." },
    { "Decal", "decals drawn to 1000 m (fade from 500 m), not 100 / 50",
      "at the game's DirectInput call", 1,
      "100.0 / 50.0 are shared literals (37 / 18 users): only the decal function's 3 loads move to BF1GameExt's 1000 / 500." },
    { "FixEffects", "more effects at once: scene queue, render queue 2, its drain cap; water splashes 200 -> 800",
      "the scene queue before the C++ start-up; the rest at the DirectInput call (WaterEffect not made yet)", 1,
      "EGM's WaterEffect rewrite is complete but writes 68736 slots (its comment: 800) - 22b: 800. Scene queue 100000 = 800 KB." },
    { "PostHook", "Hooks\\PostScriptInit.lua after each map's setup script (gMissionName set)",
      "at the game's DirectInput call (one call redirected; the Lua file read after each map's setup)", 0,
      "Lua helpers found for the 22b stub (get, pushstring, setglobal, dofile, CloseScript); gMissionName read." },
};

const char* const kArrays[] = { "Channels", "Lit100", "Lit50", "Mixer", "PropMesh", "PropStack", "RenderItemPool", "Sounds", "Source3D", "Tracking", "Vec1", "Vec2" };

// group, GOG 1.3 rva, Steam rva, pattern
const ChunkInfo kChunks[] = {
    { 0, 0x2131FB, 0x21881B, "8D 4B 04 E8 ?? ?? ?? ?? 8B 3B 81 C6 20 20 00 00 81 FE ?? ?? ?? ?? 7C CD" },
    { 0, 0x2133D0, 0x2189F0, "83 3D ?? ?? ?? ?? 00 75 25 68 20 19 00 00 E8 ?? ?? ?? ?? 83 C4 04 85 C0" },
    { 0, 0x214FB0, 0x21A5D0, "68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 1D 68 ?? ?? ?? ?? 6A ?? 8D 70 10 C7 00 ?? ?? ?? ?? 68 20 20 00 00 56 E8 ?? ?? ?? ?? EB 02 33 F6 89 37 33 F6 8B 0F FF 77 18 03 CE E8 ?? ?? ?? ?? 81 C6 20 20 00 00 81 FE" },
    { 0, 0x215019, 0x21A639, "8D 4F 04 81 FE ?? ?? ?? ?? 7C DF 8D 85 38 FB FF FF 50 8D 85 88 F1 FF FF" },
    { 0, 0x28E073, 0x294853, "68 ?? ?? ?? ?? 6A 00 68 ?? ?? ?? ?? 6A ?? 6A 00 E8 ?? ?? ?? ?? 83 C4 14 C7 04 24 00 00 20 41 6A 00 E8 ?? ?? ?? ?? 6A 00 6A 00 6A ?? 6A 00 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 68" },
    { 0, 0x2EB650, 0x2F1E30, "55 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 00 00 00 00 50 56 57 A1 ?? ?? ?? ?? 33 C5 50 8D 45 F4 64 A3 00 00 00 00 8B 0D" },
    { 1, 0x044624, 0x047BD4, "68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 59 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 6A 00 6A FF 68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 59 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 68 ?? ?? ?? ?? B9" },
    { 1, 0x0446BA, 0x047C6A, "68 ?? ?? ?? ?? 6A 30 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 59 C3 CC CC CC CC CC CC CC CC CC 6A 00 6A FF 68 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 59 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 68 ?? ?? ?? ?? B9" },
    { 1, 0x276006, 0x27C9D6, "81 FB ?? ?? ?? ?? 0F 83 49 06 00 00 57 8B CA E8 ?? ?? ?? ?? 8D 3C 5B 89 45 C4 8B 45 DC 43 C1 E7 04 8B C8 81 C7" },
    { 1, 0x3D3B30, 0x40A0D0, "68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 30 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? C3 CC" },
    { 2, 0x0C4B86, 0x0C8F56, "B9 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 5D C3 CC CC CC CC CC CC CC CC CC CC CC 8B 0D ?? ?? ?? ?? 85 C9" },
    { 2, 0x1E52D6, 0x1EA8A6, "B9 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 5D C3 CC CC CC CC CC CC CC CC CC CC CC 55 8B EC FF 75 0C B9 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 5D C3 CC CC CC CC CC CC CC CC CC CC CC 55 8B EC FF 75 0C B9 ?? ?? ?? ?? FF 75 08 E8 ?? ?? ?? ?? 5D C3 CC CC CC CC CC CC CC CC CC CC CC 8B 41 10" },
    { 2, 0x08D630, 0x091A00, "55 8B EC 83 79 3C 00 75 0C 8B 45 08 89 41 34 8B 45 0C 89 41 30 5D C2 08 00" },
    { 3, 0x220B73, 0x226193, "8D 3C 85 ?? ?? ?? ?? C1 E0 ?? 03 07 8D 34 40 8D 0C F5 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 03 8B CB FF 50 64 8D 48 04 E8 ?? ?? ?? ?? FF 07 5F 0F 10 00 0F 11 04 F5" },
    { 3, 0x220EC9, 0x2264E9, "C1 E0 ?? 89 45 F8 8D 04 40 8D 1C C5 ?? ?? ?? ?? 89 5D FC 0F 1F 40 00 8B CB E8 ?? ?? ?? ?? 8B 4D F4 8B 09 3B F8 74 18 46 83 C3 18 89 5D FC 3B F1 7C E5 FF 05 ?? ?? ?? ?? 5E 5B 5F 8B E5 5D C3 8B 45 F8 8D 59 FF 8B 4D FC 03 C3 8D 3C 40 8D 04 FD ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 8B 45 F8 0F 10 04 FD" },
    { 3, 0x220F26, 0x226546, "0F 10 04 FD ?? ?? ?? ?? 03 C6 5E 8D 04 40 0F 11 04 C5 ?? ?? ?? ?? 8B 45 F4" },
    { 3, 0x220FC4, 0x2265E4, "69 F3 ?? ?? ?? ?? 81 C6 ?? ?? ?? ?? 8D 4E F8 E8 ?? ?? ?? ?? 85 C0 74 15" },
    { 3, 0x221053, 0x226673, "C7 45 F8 ?? ?? ?? ?? 33 F6 BB ?? ?? ?? ?? 89 75 F0 89 5D FC 8B 4D 18 56" },
    { 3, 0x221149, 0x226769, "81 45 F8 ?? ?? ?? ?? 83 C3 04 46 89 5D FC 89 75 F0 81 FB ?? ?? ?? ?? 0F 8C 01 FF FF FF" },
    { 3, 0x238850, 0x23DE70, "68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 10 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 85 C0 74 15 6A 00 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 6A 10 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 17 6A 00 68 ?? ?? ?? ?? 68" },
    { 3, 0x238EB0, 0x23E4D0, "68 ?? ?? ?? ?? FF 35 ?? ?? ?? ?? 51 F3 0F 11 04 24 50 E8 ?? ?? ?? ?? 83 C4 18" },
    { 4, 0x1558B4, 0x159C84, "83 F8 ?? 0F 8D AD FE FF FF 83 7D FC 01 0F 8F A3 FE FF FF 83 EC 08 8B CC" },
    { 4, 0x2306FF, 0x235D1F, "8D 4B 64 E8 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 83 C4 04 83 F8 ?? 7E 09 8B 03" },
    { 5, 0x201D9E, 0x2073BE, "6A ?? 8B C8 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? C3 C7 05 ?? ?? ?? ?? 00 00 00 00" },
    { 6, 0x17AF7B, 0x1803BB, "A1 ?? ?? ?? ?? 89 45 FC A1 ?? ?? ?? ?? 89 45 E8 33 C0 38 05" },
    { 7, 0x03E325, 0x0418B5, "68 ?? ?? ?? ?? 6A 70 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8" },
    { 7, 0x1FC52D, 0x201B4D, "3D FE 00 00 00 ?? ?? C7 05 ?? ?? ?? ?? FE 00 00 00 B8 FE 00 00 00 33 C9" },
    { 7, 0x1FE253, 0x203873, "B9 ?? ?? ?? ?? BA ?? ?? ?? ?? BE ?? ?? ?? ?? 66 89 02 8D 52 02 66 89 01 8D 49 70 40 66 3B C6 7C EE 66 89 35 ?? ?? ?? ?? 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 55 8B EC 66 A1" },
    { 7, 0x1FE2C7, 0x2038E7, "0F BF 04 45 ?? ?? ?? ?? F3 0F 10 45 10 6B F0 70 F3 0F 11 44 24 04 F3 0F 10 45 0C F3 0F 11 04 24 FF 75 08 81 C6" },
    { 7, 0x1FF452, 0x204A72, "66 46 6A 01 66 89 35 ?? ?? ?? ?? 66 89 04 55 ?? ?? ?? ?? E8" },
    { 7, 0x3D3860, 0x409DF0, "68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A 70 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? C3 CC" },
    { 8, 0x04CCA5, 0x050255, "68 ?? ?? ?? ?? 6A ?? 6A 2C 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68" },
    { 8, 0x0B52ED, 0x0B96BD, "6A ?? 6A 08 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 18 A3 ?? ?? ?? ?? E9" },
    { 8, 0x143F00, 0x1482D0, "FF 35 ?? ?? ?? ?? B8 02 00 00 00 FF 35 ?? ?? ?? ?? 0F 45 C6 FF 75 FC 51" },
    { 8, 0x305D4B, 0x30C4EB, "81 C6 48 05 00 00 81 FE ?? ?? ?? ?? 72 C7 C7 05 ?? ?? ?? ?? 00 00 00 00" },
    { 8, 0x306486, 0x30CC26, "B8 ?? ?? ?? ?? C6 05 ?? ?? ?? ?? 01 A3 ?? ?? ?? ?? 33 F6 0F 1F 80 00 00 00 00" },
    { 8, 0x3064D4, 0x30CC74, "C7 45 FC FF FF FF FF E8 ?? ?? ?? ?? 81 C6 48 05 00 00 81 FE" },
    { 8, 0x306670, 0x30CE10, "8B 0D ?? ?? ?? ?? 03 CE E8 ?? ?? ?? ?? 81 C6 48 05 00 00 81 FE" },
    { 8, 0x3079B3, 0x30E153, "68 ?? ?? ?? ?? 68 F8 08 00 00 E8 ?? ?? ?? ?? 83 C4 0C 89 85 EC EA FF FF C7 45 FC 03 00 00 00 85 C0 74 09 8B C8 E8 ?? ?? ?? ?? EB 02 33 C0 80 BD FB EA FF FF 00 C7 45 FC FF FF FF FF A3" },
    { 8, 0x307B9B, 0x30E33B, "83 BD F0 EA FF FF ?? 0F 83 09 01 00 00 8B 8D F4 EA FF FF B8" },
    { 8, 0x307C3E, 0x30E3DE, "83 F8 ?? 73 6E 8B C8 B8 ?? ?? ?? ?? 69 F1 48 05 00 00 2B C1 89 85 F4 EA FF FF" },
    { 8, 0x30852E, 0x30ECCE, "E8 ?? ?? ?? ?? 81 C6 48 05 00 00 81 FE ?? ?? ?? ?? 72 E1 5F 88 1D" },
    { 8, 0x308C11, 0x30F3B1, "81 C6 48 05 00 00 81 FE ?? ?? ?? ?? 72 E1 5E 88 1D ?? ?? ?? ?? E8" },
    { 8, 0x30F4F4, 0x315C94, "81 C6 C8 00 00 00 81 FE ?? ?? ?? ?? 72 E1 C7 05 ?? ?? ?? ?? 00 00 00 00" },
    { 8, 0x30F928, 0x3160C8, "B9 ?? ?? ?? ?? 33 DB 89 0D ?? ?? ?? ?? 89 5D F0 0F 1F 84 00 00 00 00 00" },
    { 8, 0x30F984, 0x316124, "8D B0 ?? ?? ?? ?? 8D 04 BD ?? ?? ?? ?? 8B CE 50 E8 ?? ?? ?? ?? 8D 04 BD ?? ?? ?? ?? 8B CE 50 E8 ?? ?? ?? ?? 43 89 5D F0 ?? ?? ?? 73 08 8B 0D ?? ?? ?? ?? EB 87 8B 4D F4 64 89 0D 00 00 00 00 59 5F 5E 5B 8B E5 5D C3 CC" },
    { 8, 0x310159, 0x3168F9, "8D 1C B5 ?? ?? ?? ?? F3 0F 7E 00 66 0F D6 03 8B 40 08 89 43 08 E8 ?? ?? ?? ?? FF 75 08 8D 3C B5 ?? ?? ?? ?? F3 0F 7E 00 66 0F D6 07 8B 40 08 89 47 08 6B 45 E8 2C 8D B0" },
    { 8, 0x310CA1, 0x317441, "81 FE ?? ?? ?? ?? 72 AA 5E C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 56 57 8B F9 E8 ?? ?? ?? ?? 8B CF E8 ?? ?? ?? ?? 8B F0 81 FE ?? ?? ?? ?? 72 08 81 FE ?? ?? ?? ?? 72 32 F3 0F 7E 06 8B CF 2B 0D ?? ?? ?? ?? B8 1F 85 EB 51 F7 E1 C1 EA 06 8D 04 52 8D 0C 85" },
    { 8, 0x324647, 0x32AE17, "6A ?? 6A 08 8D 86 ?? ?? ?? ?? C7 45 FC 00 00 00 00 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A ?? 6A 08 8D 86" },
    { 8, 0x324933, 0x32B103, "8B F1 68 ?? ?? ?? ?? 6A ?? 6A 08 8D 86 ?? ?? ?? ?? 50 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 6A ?? 6A 08 8D 86" },
    { 8, 0x324EE4, 0x32B6B4, "8D 79 30 56 8B CF E8 ?? ?? ?? ?? 85 C0 74 0C 46 83 FE ?? 72 EE 5F 83 C8 FF" },
    { 8, 0x325217, 0x32B9E7, "6A ?? 8D 87 ?? ?? ?? ?? 50 8D 4F 30 E8 ?? ?? ?? ?? 8D 87 ?? ?? ?? ?? 50 8D 4F 0C E8 ?? ?? ?? ?? 56 8D 4F 0C E8 ?? ?? ?? ?? 8D 87 ?? ?? ?? ?? 50 8D 87" },
    { 8, 0x3D4300, 0x40A8A0, "68 ?? ?? ?? ?? 6A ?? 6A 2C 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? C3 CC CC CC CC" },
    { 9, 0x238E77, 0x23E497, "C7 44 24 04 ?? ?? ?? ?? C7 04 24 00 00 96 42 57 E8 ?? ?? ?? ?? D9 1C 24" },
    { 9, 0x238F43, 0x23E563, "E8 ?? ?? ?? ?? D9 5D EC F3 0F 10 45 EC 83 C4 08 F3 0F 59 C0 0F 2F 45 F0" },
    { 10, 0x0C5078, 0x0C9448, "F3 0F 10 05 ?? ?? ?? ?? 83 C4 10 D9 5D F4 0F 2F 45 F4 76 63 8D 4D FC E8 ?? ?? ?? ?? 8D 48 48 8B 46 2C 01 01 8D 4D FC E8 ?? ?? ?? ?? 8D 48 4C 8B 46 30 01 01 8D 4D FC E8 ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? 83 EC 0C F3 0F 5C 45 F4 FF 40 50 C7 44 24 08 00 00 80 3F C7 44 24 04 00 00 00 00 F3 0F 5E 05" },
    { 11, 0x1E7D13, 0x1ED323, "B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68" },
    { 11, 0x23B42A, 0x240A4A, "33 FF ?? ?? ?? ?? ?? ?? 76 37 ?? ?? ?? 66 0F 1F 84 00 00 00 00 00 8B 0E 83 C1 0C E8 ?? ?? ?? ?? 85 C0 74 11 8B 0E 83 C1 0C E8 ?? ?? ?? ?? 8B C8 8B 10 FF 52 1C 47 83 C6 04 ?? ?? ?? ?? ?? ?? 72 D5 ?? ?? ?? ?? ?? ?? ?? ?? 8B 0E 85 C9 74 07 6A 01 E8 ?? ?? ?? ?? 83 C6 04 83 EF 01 75 EB" },
    { 11, 0x23B4B0, 0x240AD0, "83 3D ?? ?? ?? ?? 00 75 25 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 85 C0 74 0D 8B C8 E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? C3 33 C0 A3 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 8B 0D ?? ?? ?? ?? 85 C9 74 06 8B 01 6A 01 FF 10 C7 05 ?? ?? ?? ?? 00 00 00 00 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC CC 55" },
    { 11, 0x23B554, 0x240B74, "E8 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 0F 1F 44 00 00 6A 14" },
    { 11, 0x23B78A, 0x240DAA, "57 ?? ?? ?? ?? ?? ?? 85 C0 74 47 ?? ?? ?? 0F 1F 84 00 00 00 00 00 8B 0F 83 C1 0C E8 ?? ?? ?? ?? 85 C0 75 20 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? EB 04 43 83 C7 04 ?? ?? ?? ?? ?? ?? 3B D8 72 C4 33 DB 85 C0 74 36" },
    { 11, 0x23B800, 0x240E20, "8B 0F F3 0F 11 04 24 E8 ?? ?? ?? ?? 43 83 C7 04 ?? ?? ?? ?? ?? ?? 72 CD" },
    { 11, 0x23B857, 0x240E77, "8B F1 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 0F 83 BF 00 00 00 FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 85 C0 0F 84 AC 00 00 00 8B 10 8B C8 FF 52 04 50 8D 4D F8 E8 ?? ?? ?? ?? 8D 4D F8 E8 ?? ?? ?? ?? 85 C0 0F 84 8C 00 00 00 8D 45 F8 50 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 83 C1 0C E8" },
    { 11, 0x23B8D2, 0x240EF2, "89 41 08 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? FF 75 10 8D 4D F8 E8 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 6A 00 8D 4D F8 E8 ?? ?? ?? ?? 8B C8 E8" },
    { 11, 0x28E0F2, 0x2948D2, "68 ?? ?? ?? ?? 6A ?? 6A 02 E8 ?? ?? ?? ?? 83 C4 28 C7 04 24 00 00 20 41" },
    { 11, 0x2B5AE9, 0x2BC2C9, "C7 04 24 FF FF 7F 7F 8B C8 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 02 33 C0 89 46 38" },
    { 11, 0x2EB39E, 0x2F1B7E, "8B 4C 31 24 BF ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 85 9D 01 00 00 8B C7" },
    { 12, 0x0C2A50, 0x0C6E20, "B8 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC CC CC 32 C0 C3 CC CC CC CC CC CC CC CC CC CC CC CC CC A0" },
    { 12, 0x13DAC1, 0x141E91, "68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 08 E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0" },
    { 12, 0x184BB3, 0x189FF3, "CC CC CC CC CC CC CC CC CC CC CC CC CC A1 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC CC CC 8B 41 10" },
    { 12, 0x25C4B0, 0x262E80, "55 8B EC 8B 55 0C 85 D2 75 0D FF 75 08 E8 ?? ?? ?? ?? 83 C4 04 5D C3 8B CA 56 8D 71 01 0F 1F 00" },
    { 12, 0x25C910, 0x2632E0, "55 8B EC 56 57 8B 7D 08 FF 75 0C 57 8B 37 E8 ?? ?? ?? ?? 50 57 E8 ?? ?? ?? ?? 83 C4 10 8D 46 F8" },
    { 12, 0x25D790, 0x264160, "55 8B EC FF 75 0C FF 75 08 E8 ?? ?? ?? ?? 83 C4 08 85 C0 75 0E 6A FF 50 FF 75 08 E8" },
    { 12, 0x184910, 0x189D50, "83 3D ?? ?? ?? ?? 00 75 03 32 C0 C3 FF 35 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 C7 05 ?? ?? ?? ?? 00 00 00 00" },
};

const BlockInfo kBlocks[] = {
    { "RenderItemPool", 0x120000 },
    { "PropMesh", 0xDAC00 },
    { "PropStack", 0x3E80 },
    { "Tracking", 0x30000 },
    { "Channels", 0x29EB8 },
    { "Sounds", 0xC800 },
    { "Vec1", 0xC00 },
    { "Vec2", 0xC00 },
    { "Source3D", 0x2C00 },
    { "Mixer", 0x1ADC },
};

// chunk, offset, size, kind, array, probe, probe dwords, probe offset, stock, [Limits] value, offset in its array,
// target on GOG 1.3, target on Steam, code bytes, what, EGM, 22b,
// loose, ready index / count / at the start-up, 22b action, early, block, to, value, new offset, bytes
const SiteInfo kSites[] = {
    { 0, 0x12, 4, kConst, -1, kProbeNone, 0, 0x0, 0x14140, 0xC8C80, 0x0, 0x000000, 0x000000, nullptr,
      "animator clean-up loop end", "MISSED (stays 10)", "0x404000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x404000, kSameOff, nullptr },
    { 1, 0x02, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x5EC350, 0x62E380, nullptr,
      "animator object (global)", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 2, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x14150, 0xC8C90, 0x0, 0x000000, 0x000000, nullptr,
      "animator pool bytes", "0x404010 (512)", "0x404010 (512)",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x404010, kSameOff, nullptr },
    { 2, 0x17, 1, kConst, -1, kProbeNone, 0, 0x0, 0xA, 0x64, 0x0, 0x000000, 0x000000, nullptr,
      "animator count (imm8)", "512 (code cave)", "512 (wrapper on the vector constructor call)",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 2, 0x1D, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA, 0x64, 0x0, 0x000000, 0x000000, nullptr,
      "animator count (header)", "0x200", "0x200",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x200, kSameOff, nullptr },
    { 2, 0x27, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x059EC0, 0x05E230, nullptr,
      "animator vector constructor call", "(count via a code cave)", "through BF1GameExt: 512",
      0, 0, 0, 0, kActCall, 0, -1, kToVecWrap, 0x0, kSameOff, nullptr },
    { 2, 0x48, 4, kConst, -1, kProbeNone, 0, 0x0, 0x14140, 0xC8C80, 0x0, 0x000000, 0x000000, nullptr,
      "animator init loop end", "0x404000", "0x404000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x404000, kSameOff, nullptr },
    { 3, 0x05, 4, kConst, -1, kProbeNone, 0, 0x0, 0x14140, 0xC8C80, 0x0, 0x000000, 0x000000, nullptr,
      "animator link loop end", "0x404000", "0x404000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x404000, kSameOff, nullptr },
    { 4, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x3E8, 0x2710, 0x0, 0x000000, 0x000000, nullptr,
      "pool 0 a5 (soldier budget)", "0x3E800", "0x3E800",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E800, kSameOff, nullptr },
    { 4, 0x08, 4, kConst, -1, kProbeNone, 0, 0x0, 0x2710, 0x130B0, 0x0, 0x000000, 0x000000, nullptr,
      "pool 0 a3 (soldier budget)", "0x271000", "0x271000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x271000, kSameOff, nullptr },
    { 4, 0x0D, 1, kConst, -1, kProbeNone, 0, 0x0, 0x64, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "pool 0 count (imm8)", "512 (code cave)", "1000 ([Limits] call wrapper)",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 4, 0x10, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2EB650, 0x2F1E30, nullptr,
      "pool 0 create call", "(count via a code cave)", "through BF1GameExt: 1000",
      1, 0, 0, 0, kActCall, 0, -1, kToPoolWrap, 0x0, kSameOff, nullptr },
    { 4, 0x34, 4, kConst, -1, kProbeNone, 0, 0x0, 0x55F0, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "pool 1 a5 (vehicle lo-res budget)", "0x55F000", "0x55F000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x55F000, kSameOff, nullptr },
    { 4, 0x39, 4, kConst, -1, kProbeNone, 0, 0x0, 0x55F0, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "pool 1 a4 (vehicle lo-res budget)", "0x55F000", "0x55F000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x55F000, kSameOff, nullptr },
    { 4, 0x3E, 4, kConst, -1, kProbeNone, 0, 0x0, 0x186A0, 0xF4240, 0x0, 0x000000, 0x000000, nullptr,
      "pool 1 a3 (vehicle hi-res budget)", "0x186A000", "0x186A000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x186A000, kSameOff, nullptr },
    { 5, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2EB650, 0x2F1E30, nullptr,
      "render pool create", "", "the wrappers call it",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 6, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x708, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sOPAQUE size (static constructor)", "0x1C20", "0x1C20 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x1C20, kSameOff, nullptr },
    { 6, 0x06, 4, kAddr, -1, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x5F9974, 0x63B9C4, nullptr,
      "sOPAQUE list", "", "",
      0, 0, 4, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 6, 0x31, 4, kConst, -1, kProbeNone, 0, 0x0, 0x908, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sPostShadow size (static constructor)", "0x2420", "0x2420 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x2420, kSameOff, nullptr },
    { 6, 0x36, 4, kAddr, -1, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x615A78, 0x657AC8, nullptr,
      "sPostShadow list", "", "",
      0, 0, 4, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 6, 0x5D, 4, kConst, -1, kProbeNone, 0, 0x0, 0x200, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sPOSTTRANSPARENT size (static constructor)", "0x20000", "0x20000 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x20000, kSameOff, nullptr },
    { 6, 0x62, 4, kAddr, -1, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x5F9960, 0x63B9B0, nullptr,
      "sPOSTTRANSPARENT list", "", "",
      0, 0, 4, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 7, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x1228, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "render item pool count (static constructor)", "0x6000", "0x6000",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x6000, kSameOff, nullptr },
    { 7, 0x08, 4, kAddr, 6, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x645FC0, 0x688010, nullptr,
      "render item pool (static constructor)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 1, 0, kToNone, 0x0, kSameOff, nullptr },
    { 7, 0x2B, 4, kConst, -1, kProbeNone, 0, 0x0, 0x400, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sShadow size (static constructor)", "0x1000", "0x1000 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x1000, kSameOff, nullptr },
    { 7, 0x30, 4, kAddr, -1, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x615A68, 0x657AB8, nullptr,
      "sShadow list", "", "",
      0, 0, 4, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 7, 0x57, 4, kConst, -1, kProbeNone, 0, 0x0, 0x320, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sTRANSPARENT size (static constructor)", "0x32000", "0x32000 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x32000, kSameOff, nullptr },
    { 7, 0x5C, 4, kAddr, -1, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x615A88, 0x657AD8, nullptr,
      "sTRANSPARENT list", "", "",
      0, 0, 4, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 8, 0x02, 4, kConst, -1, kProbeNone, 0, 0x0, 0x1228, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "render item pool cap (allocator)", "0x6000", "0x6000",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x6000, kSameOff, nullptr },
    { 8, 0x25, 4, kAddr, 6, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x645FC0, 0x688010, nullptr,
      "render item pool (allocator)", "new array", "new array in BF1GameExt",
      0, 0, 0, 0, kActRebase, 1, 0, kToNone, 0x0, kSameOff, nullptr },
    { 9, 0x06, 4, kConst, -1, kProbeNone, 0, 0x0, 0x1228, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "render item pool count (exit destructor)", "0x6000", "0x6000",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x6000, kSameOff, nullptr },
    { 9, 0x0D, 4, kAddr, 6, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x645FC0, 0x688010, nullptr,
      "render item pool (exit destructor)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 1, 0, kToNone, 0x0, kSameOff, nullptr },
    { 10, 0x01, 4, kAddr, -1, kProbeVal, 17, 0x0, 0x0, 0x0, 0x0, 0x46B560, 0x4AD590, nullptr,
      "decal pool", "", "",
      0, 15, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 10, 0x08, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x18A290, 0x18F6D0, nullptr,
      "decal pool: call PoolInit (eager)", "call SetSizeIfUnallocated", "call SetSizeIfUnallocated",
      0, 0, 0, 0, kActCall, 0, -1, kToSetSize, 0x0, kSameOff, nullptr },
    { 11, 0x01, 4, kAddr, -1, kProbeVal, 17, 0x0, 0x0, 0x0, 0x0, 0x53FC70, 0x581CA0, nullptr,
      "AI path pool", "", "",
      0, 15, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 11, 0x08, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x18A290, 0x18F6D0, nullptr,
      "AI path pool: call PoolInit (eager)", "call SetSizeIfUnallocated", "call SetSizeIfUnallocated",
      0, 0, 0, 0, kActCall, 0, -1, kToSetSize, 0x0, kSameOff, nullptr },
    { 11, 0x21, 4, kAddr, -1, kProbeVal, 17, 0x0, 0x0, 0x0, 0x0, 0x53FCD0, 0x581D00, nullptr,
      "AI path node pool", "", "",
      0, 15, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 11, 0x28, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x18A290, 0x18F6D0, nullptr,
      "AI path node pool: call PoolInit (eager)", "call SetSizeIfUnallocated", "call SetSizeIfUnallocated",
      0, 0, 0, 0, kActCall, 0, -1, kToSetSize, 0x0, kSameOff, nullptr },
    { 11, 0x41, 4, kAddr, -1, kProbeVal, 17, 0x0, 0x0, 0x0, 0x0, 0x53FBE8, 0x581C18, nullptr,
      "AI path request pool", "", "",
      0, 15, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 11, 0x48, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x18A290, 0x18F6D0, nullptr,
      "AI path request pool: call PoolInit (eager)", "call SetSizeIfUnallocated", "call SetSizeIfUnallocated",
      0, 0, 0, 0, kActCall, 0, -1, kToSetSize, 0x0, kSameOff, nullptr },
    { 12, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x08D630, 0x091A00, nullptr,
      "SetSizeIfUnallocated (the game's lazy pool size)", "", "call target",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 13, 0x03, 4, kAddr, -1, kProbeVal, 8, 0x0, 0x0, 0x0, 0x0, 0x5F2EA0, 0x634ED0, nullptr,
      "per-team counts (track)", "", "",
      0, 0, 8, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 13, 0x09, 1, kConst, -1, kProbeNone, 0, 0x0, 0x7, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "track: log2 slots per team", "10", "10 (1024 per team)",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0xA, kSameOff, nullptr },
    { 13, 0x12, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5ECEA0, 0x62EED0, nullptr,
      "tracking array (track)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 13, 0x34, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x8, 0x5ECEA8, 0x62EED8, nullptr,
      "tracking array +8 (track)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 14, 0x02, 1, kConst, -1, kProbeNone, 0, 0x0, 0x7, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "untrack: log2 slots per team", "10", "10",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0xA, kSameOff, nullptr },
    { 14, 0x0C, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5ECEA0, 0x62EED0, nullptr,
      "tracking array (untrack)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 14, 0x50, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5ECEA0, 0x62EED0, nullptr,
      "tracking array (untrack)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 14, 0x61, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x8, 0x5ECEA8, 0x62EED8, nullptr,
      "tracking array +8 (untrack)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 15, 0x12, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x8, 0x5ECEA8, 0x62EED8, nullptr,
      "tracking array +8 (untrack)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 16, 0x02, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC00, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "fill: team stride", "0x6000", "0x6000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x6000, kSameOff, nullptr },
    { 16, 0x08, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x8, 0x5ECEA8, 0x62EED8, nullptr,
      "tracking array +8 (fill)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 17, 0x03, 4, kAddr, 9, kProbeNone, 0, 0x0, 0x0, 0x0, 0x8, 0x5ECEA8, 0x62EED8, nullptr,
      "tracking array +8 (getCloseUnits)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 3, kToNone, 0x0, kSameOff, nullptr },
    { 18, 0x03, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC00, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "getCloseUnits: team stride", "0x6000", "0x6000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x6000, kSameOff, nullptr },
    { 19, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x2BC, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "close units array bytes (175 pointers)", "0xFA0 (1000)", "0xFA0, only while no map is loaded",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0xFA0, kSameOff, nullptr },
    { 19, 0x0D, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x5F5E9C, 0x637ECC, nullptr,
      "close units array pointer", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 19, 0x20, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC8, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ray queue 1 sentinel", "0x3E8", "0x3E8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E8, kSameOff, nullptr },
    { 19, 0x25, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC8, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ray queue 1 capacity", "0x3E8", "0x3E8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E8, kSameOff, nullptr },
    { 19, 0x4A, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC8, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ray queue 2 sentinel", "0x3E8", "0x3E8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E8, kSameOff, nullptr },
    { 19, 0x4F, 4, kConst, -1, kProbeNone, 0, 0x0, 0xC8, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ray queue 2 capacity", "0x3E8", "0x3E8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E8, kSameOff, nullptr },
    { 20, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0xAF, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "getCloseUnits max count", "0x3E8", "0x3E8 (with the array)",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x3E8, kSameOff, nullptr },
    { 21, 0x02, 1, kConst, -1, kProbeNone, 0, 0x0, 0x3, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "recruit: squad size below", "9", "9",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x9, kSameOff, nullptr },
    { 22, 0x13, 1, kConst, -1, kProbeNone, 0, 0x0, 0x3, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "follow: squad size at most", "9", "9",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x9, kSameOff, nullptr },
    { 23, 0x01, 1, kConst, -1, kProbeNone, 0, 0x0, 0x3C, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "vehicle claim list capacity", "0x7F", "0x7F",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 24, 0x01, 4, kData, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x4396E8, 0x4796F8, nullptr,
      "iMaxBots (ScriptCB_GetNetGameDefaults)", "63", "63 (multiplayer host only)",
      0, 0, 0, 0, kActData, 0, -1, kToNone, 0x3F, kSameOff, nullptr },
    { 25, 0x01, 4, kConst, -1, kProbeNone, 0, 0x0, 0x7D0, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "prop mesh count (static constructor)", "8000", "8000 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x1F40, kSameOff, nullptr },
    { 25, 0x08, 4, kAddr, 4, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x56A6E0, 0x5AC710, nullptr,
      "prop mesh array (static constructor)", "new array", "new array",
      0, 0, 1, 1, kActRebase, 1, 1, kToNone, 0x0, kSameOff, nullptr },
    { 26, 0x05, 2, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "7E 0F",
      "cluster cap 254 (jle)", "NOP: always 254", "NOP",
      0, 0, 0, 0, kActBytes, 1, -1, kToNone, 0x0, kSameOff, "90 90" },
    { 27, 0x01, 4, kAddr, 4, kProbeNone, 0, 0x0, 0x0, 0x0, 0x1E, 0x56A6FE, 0x5AC72E, nullptr,
      "prop mesh array +0x1E (setIndex)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 1, 1, kToNone, 0x0, kSameOff, nullptr },
    { 27, 0x06, 4, kAddr, 5, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5A11E0, 0x5E3210, nullptr,
      "free index stack (setIndex)", "new stack", "new stack",
      0, 0, 0, 0, kActRebase, 1, 2, kToNone, 0x0, kSameOff, nullptr },
    { 27, 0x0B, 4, kConst, -1, kProbeNone, 0, 0x0, 0x7D0, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "prop mesh count (setIndex)", "8000", "8000",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x1F40, kSameOff, nullptr },
    { 27, 0x42, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x43A848, 0x47A858, nullptr,
      "free stack top", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 28, 0x04, 4, kAddr, 5, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5A11E0, 0x5E3210, nullptr,
      "free index stack (getPropObjectMesh)", "new stack", "new stack",
      0, 0, 0, 0, kActRebase, 1, 2, kToNone, 0x0, kSameOff, nullptr },
    { 28, 0x25, 4, kAddr, 4, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x56A6E0, 0x5AC710, nullptr,
      "prop mesh array (getPropObjectMesh)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 1, 1, kToNone, 0x0, kSameOff, nullptr },
    { 29, 0x0F, 4, kAddr, 5, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x5A11E0, 0x5E3210, nullptr,
      "free index stack (release)", "new stack", "new stack",
      0, 0, 0, 0, kActRebase, 1, 2, kToNone, 0x0, kSameOff, nullptr },
    { 30, 0x06, 4, kConst, -1, kProbeNone, 0, 0x0, 0x7D0, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "prop mesh count (exit destructor)", "8000", "8000",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x1F40, kSameOff, nullptr },
    { 30, 0x0D, 4, kAddr, 4, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x56A6E0, 0x5AC710, nullptr,
      "prop mesh array (exit destructor)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 1, 1, kToNone, 0x0, kSameOff, nullptr },
    { 31, 0x01, 4, kAddr, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x00C081, 0x00AF29, nullptr,
      "SndSource3D constructor (static constructor)", "", "BF1GameExt calls it for its 256",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 31, 0x06, 1, kConst, -1, kProbeNone, 0, 0x0, 0x40, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "SndSource3D static constructor count", "MISSED (64 built)", "BF1GameExt builds its 256",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 31, 0x0A, 4, kAddr, 8, kProbeVal, 4, 0x0, 0x0, 0x0, 0x0, 0x2172F90, 0x21B4FE0, nullptr,
      "SndSource3D array (static constructor)", "new array", "left (the old 64, built before WinMain)",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 32, 0x01, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "/audiomaxchannels clamp max", "MISSED (clamps back to 32)", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 33, 0x02, 4, kData, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x43831C, 0x47831C, nullptr,
      "channel count setting (global)", "127", "127 (if still 32)",
      0, 0, 0, 0, kActData, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 34, 0x08, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA900, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channels end (32 x 0x548)", "127 x 0x548", "127 x 0x548",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x29EB8, kSameOff, nullptr },
    { 35, 0x01, 4, kAddr, 0, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x902890, 0x9448E0, nullptr,
      "channels (snd_7) array", "new array (127)", "new array (127)",
      0, 0, 0, 0, kActRebase, 0, 4, kToNone, 0x0, kSameOff, nullptr },
    { 35, 0x0D, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x902884, 0x9448D4, nullptr,
      "channels pointer", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 36, 0x14, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA900, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channels end (32 x 0x548)", "127 x 0x548", "127 x 0x548",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x29EB8, kSameOff, nullptr },
    { 37, 0x15, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA900, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channels end (32 x 0x548)", "127 x 0x548", "127 x 0x548",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x29EB8, kSameOff, nullptr },
    { 38, 0x01, 4, kAddr, 3, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x216C288, 0x21AE2D8, nullptr,
      "stream mixer (snd_19) block", "new block", "new block",
      0, 0, 0, 0, kActRebase, 0, 9, kToNone, 0x0, kSameOff, nullptr },
    { 38, 0x3D, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x216C27C, 0x21AE2CC, nullptr,
      "mixer pointer", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 39, 0x06, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channel init: count (imm8)", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 39, 0x14, 4, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channel init: count", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 40, 0x02, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channel init branch B: count (imm8)", "MISSED", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 40, 0x08, 4, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channel init branch B: count", "MISSED", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 41, 0x0D, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA900, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channels end (32 x 0x548)", "127 x 0x548", "127 x 0x548",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x29EB8, kSameOff, nullptr },
    { 42, 0x08, 4, kConst, -1, kProbeNone, 0, 0x0, 0xA900, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "channels end (32 x 0x548)", "127 x 0x548", "127 x 0x548",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x29EB8, kSameOff, nullptr },
    { 43, 0x08, 4, kConst, -1, kProbeNone, 0, 0x0, 0x3200, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sounds end (64 x 0xC8)", "256 x 0xC8", "256 x 0xC8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0xC800, kSameOff, nullptr },
    { 44, 0x01, 4, kAddr, 7, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x216F728, 0x21B1778, nullptr,
      "sounds (s_snd_2) array", "new array (256)", "new array (256)",
      0, 0, 0, 0, kActRebase, 0, 5, kToNone, 0x0, kSameOff, nullptr },
    { 44, 0x09, 4, kAddr, -1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x216F720, 0x21B1770, nullptr,
      "sounds pointer", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 45, 0x02, 4, kAddr, 8, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172F90, 0x21B4FE0, nullptr,
      "SndSource3D array", "new array (256)", "new array (256), built by BF1GameExt",
      0, 0, 0, 0, kActRebase, 0, 8, kToNone, 0x0, kSameOff, nullptr },
    { 45, 0x09, 4, kAddr, 11, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172970, 0x21B49C0, nullptr,
      "vectors 2", "new array (256)", "new array (256)",
      0, 0, 0, 0, kActRebase, 0, 7, kToNone, 0x0, kSameOff, nullptr },
    { 45, 0x18, 4, kAddr, 10, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172C90, 0x21B4CE0, nullptr,
      "vectors 1", "new array (256)", "new array (256)",
      0, 0, 0, 0, kActRebase, 0, 6, kToNone, 0x0, kSameOff, nullptr },
    { 45, 0x28, 3, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "83 FB 40",
      "sound init loop: cmp ebx, 64", "jmp to a cave (256)", "jmp to the padding after it (256)",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "EB 1E 90" },
    { 45, 0x48, 8, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "CC CC CC CC CC CC CC CC",
      "padding after the sound init (cave)", "cmp ebx, 256; jmp back", "cmp ebx, 256; jmp back",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "81 FB 00 01 00 00 EB DB" },
    { 46, 0x03, 4, kAddr, 11, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172970, 0x21B49C0, nullptr,
      "vectors 2", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 7, kToNone, 0x0, kSameOff, nullptr },
    { 46, 0x20, 4, kAddr, 10, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172C90, 0x21B4CE0, nullptr,
      "vectors 1", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 6, kToNone, 0x0, kSameOff, nullptr },
    { 46, 0x38, 4, kAddr, 8, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172F90, 0x21B4FE0, nullptr,
      "SndSource3D array", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 8, kToNone, 0x0, kSameOff, nullptr },
    { 47, 0x02, 4, kConst, -1, kProbeNone, 0, 0x0, 0x3200, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "sounds end (64 x 0xC8)", "256 x 0xC8", "256 x 0xC8",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0xC800, kSameOff, nullptr },
    { 47, 0x43, 4, kAddr, 11, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172970, 0x21B49C0, nullptr,
      "vectors 2 (cmp)", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 7, kToNone, 0x0, kSameOff, nullptr },
    { 47, 0x4B, 4, kAddr, 11, kProbeNone, 0, 0x0, 0x0, 0x0, 0x300, 0x2172C70, 0x21B4CC0, nullptr,
      "vectors 2 end (cmp)", "new array end", "new array end",
      0, 0, 0, 0, kActRebase, 0, 7, kToNone, 0x0, 0xC00, nullptr },
    { 47, 0x6D, 4, kAddr, 11, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172970, 0x21B49C0, nullptr,
      "vectors 2", "new array", "new array",
      0, 0, 0, 0, kActRebase, 0, 7, kToNone, 0x0, kSameOff, nullptr },
    { 48, 0x01, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet ctor: gap2 count", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 48, 0x06, 4, kConst, -1, kProbeNone, 0, 0x0, 0x230, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet ctor: gap2 offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x1054, kSameOff, nullptr },
    { 48, 0x22, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet ctor: gap3 count", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 48, 0x27, 4, kConst, -1, kProbeNone, 0, 0x0, 0x330, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet ctor: gap3 offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x144C, kSameOff, nullptr },
    { 49, 0x08, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet dtor: gap3 count", "MISSED", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 49, 0x0D, 4, kConst, -1, kProbeNone, 0, 0x0, 0x330, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet dtor: gap3 offset", "MISSED", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x144C, kSameOff, nullptr },
    { 49, 0x1D, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet dtor: gap2 count", "MISSED", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 49, 0x22, 4, kConst, -1, kProbeNone, 0, 0x0, 0x230, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "ChannelSet dtor: gap2 offset", "MISSED", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x1054, kSameOff, nullptr },
    { 50, 0x12, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: free channel search count", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 51, 0x01, 1, kConst, -1, kProbeNone, 0, 0x0, 0x20, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: channel count (imm8)", "127", "127",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x7F, kSameOff, nullptr },
    { 51, 0x04, 4, kConst, -1, kProbeNone, 0, 0x0, 0x2CC, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: gap2 offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x10F0, kSameOff, nullptr },
    { 51, 0x13, 4, kConst, -1, kProbeNone, 0, 0x0, 0xCC, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: gap offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x900, kSameOff, nullptr },
    { 51, 0x2B, 4, kConst, -1, kProbeNone, 0, 0x0, 0x4CC, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: gap4 offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x18E0, kSameOff, nullptr },
    { 51, 0x32, 4, kConst, -1, kProbeNone, 0, 0x0, 0x3CC, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "mixer: gap3 offset", "new offset", "new offset",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x14E8, kSameOff, nullptr },
    { 52, 0x06, 1, kConst, -1, kProbeNone, 0, 0x0, 0x40, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "SndSource3D exit destructor count", "MISSED (64)", "0x40 -> 256 is 2 bytes: wrapper",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 52, 0x0A, 4, kAddr, 8, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2172F90, 0x21B4FE0, nullptr,
      "SndSource3D array (exit destructor)", "new array", "new array",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 53, 0x04, 4, kFloat, -1, kProbeNone, 0, 0x0, 0x43AF0000, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "AI scan radius cap (350.0)", "1000.0", "1000.0",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x447A0000, kSameOff, nullptr },
    { 54, 0x18, 2, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "76 71",
      "first target: field-of-view gate (jbe)", "NOP", "NOP",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "90 90" },
    { 55, 0x04, 4, kAddr, 1, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x3DA860, 0x410A80, nullptr,
      "decal draw distance (100.0, shared literal)", "-> own 1000.0", "-> BF1GameExt 1000.0",
      0, 0, 0, 0, kActPtr, 0, -1, kToDecalFar, 0x0, kSameOff, nullptr },
    { 55, 0x40, 4, kAddr, 1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x3DA860, 0x410A80, nullptr,
      "decal draw distance (100.0, shared literal)", "-> own 1000.0", "-> BF1GameExt 1000.0",
      0, 0, 0, 0, kActPtr, 0, -1, kToDecalFar, 0x0, kSameOff, nullptr },
    { 55, 0x63, 4, kAddr, 2, kProbeVal, 1, 0x0, 0x0, 0x0, 0x0, 0x3DA85C, 0x410A7C, nullptr,
      "decal fade length (50.0, shared literal)", "-> own 500.0", "-> BF1GameExt 500.0",
      0, 0, 0, 0, kActPtr, 0, -1, kToDecalFade, 0x0, kSameOff, nullptr },
    { 56, 0x01, 4, kAddr, -1, kProbePtrAt, 4, 0x38, 0x0, 0x0, 0x0, 0x54063C, 0x58266C, nullptr,
      "scene manager", "", "",
      0, 0, 1, 1, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 57, 0x02, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "39 BB 44 03 00 00",
      "WaterEffect Init: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "39 7B 20 90 90 90" },
    { 57, 0x0A, 3, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8D 73 24",
      "WaterEffect Init: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8D 73 28" },
    { 57, 0x39, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "3B BB 44 03 00 00",
      "WaterEffect Init: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "3B 7B 20 90 90 90" },
    { 57, 0x41, 3, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8D 73 24",
      "WaterEffect Init: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8D 73 28" },
    { 57, 0x44, 5, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "BF C8 00 00 00",
      "WaterEffect Init: 200", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "BF NN NN NN NN" },
    { 57, 0x5E, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "89 BB 44 03 00 00",
      "WaterEffect Init: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "89 7B 20 90 90 90" },
    { 58, 0x02, 4, kAddr, -1, kProbePtrThen, 2, 0x344, 0x0, 0x0, 0x0, 0x5F60D0, 0x638100, nullptr,
      "WaterEffect object (singleton)", "", "",
      0, 0, 1, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 58, 0x0A, 4, kConst, -1, kProbeNone, 0, 0x0, 0x348, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "WaterEffect object size", "0x43228 (68736 slots)", "0x28 + 4 x 800",
      0, 0, 0, 0, kActWaterSize, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 59, 0x05, 18, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "C7 86 44 03 00 00 00 00 00 00 BF C8 00 00 00 83 C6 24",
      "WaterEffect Shutdown: count, 200, array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "C7 46 20 00 00 00 00 90 90 90 BF NN NN NN NN 83 C6 28" },
    { 60, 0x01, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00",
      "WaterEffect Update: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90" },
    { 60, 0x0B, 3, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8D 7E 24",
      "WaterEffect Update: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8D 7E 28" },
    { 60, 0x24, 30, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00 8B 0F 8B 44 86 20 89 07 FF 8E 44 03 00 00 8B 86 44 03 00 00 89 4C 86 24",
      "WaterEffect Update: remove", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90 8B 0F 8B 44 86 24 89 07 FF 4E 20 90 90 90 8B 46 20 90 90 90 89 4C 86 28" },
    { 60, 0x48, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00",
      "WaterEffect Update: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90" },
    { 60, 0x58, 3, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8D 7E 24",
      "WaterEffect Update: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8D 7E 28" },
    { 61, 0x10, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "3B 9E 44 03 00 00",
      "WaterEffect Update: count", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "3B 5E 20 90 90 90" },
    { 62, 0x02, 10, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "81 BE 44 03 00 00 C8 00 00 00",
      "WaterEffect Add: full check (200)", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "81 7E 20 NN NN NN NN 90 90 90" },
    { 62, 0x49, 10, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00 8B 4C 86 24",
      "WaterEffect Add: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90 8B 4C 86 28" },
    { 62, 0x5B, 10, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00 8B 4C 86 24",
      "WaterEffect Add: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90 8B 4C 86 28" },
    { 63, 0x03, 10, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 8E 44 03 00 00 8B 4C 8E 24",
      "WaterEffect Add: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 4E 20 90 90 90 8B 4C 8E 28" },
    { 63, 0x12, 15, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "8B 86 44 03 00 00 8D 4D F8 6A 01 FF 74 86 24",
      "WaterEffect Add: array", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "8B 46 20 90 90 90 8D 4D F8 6A 01 FF 74 86 28" },
    { 63, 0x50, 6, kCode, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x000000, 0x000000, "FF 86 44 03 00 00",
      "WaterEffect Add: count++", "rewritten (count +0x20, array +0x28)", "same rewrite, 800 slots",
      0, 0, 0, 0, kActBytes, 0, -1, kToNone, 0x0, kSameOff, "FF 46 20 90 90 90" },
    { 64, 0x06, 1, kConst, -1, kProbeNone, 0, 0x0, 0x64, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "render queue 2 count (pool 2, imm8)", "100000 (code cave)", "100000 (the pool 2 call through BF1GameExt)",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 64, 0x09, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x2EB650, 0x2F1E30, nullptr,
      "pool 2 create call", "(count via a code cave)", "through BF1GameExt: 100000",
      1, 0, 0, 0, kActCall, 0, -1, kToPoolWrap, 0x0, kSameOff, nullptr },
    { 65, 0x0A, 4, kConst, -1, kProbeNone, 0, 0x0, 0x190, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "scene queue capacity (built at WinMain start)", "100000", "100000 (before the C++ start-up)",
      0, 0, 0, 0, kActSet, 1, -1, kToNone, 0x186A0, kSameOff, nullptr },
    { 66, 0x05, 4, kConst, -1, kProbeNone, 0, 0x0, 0x190, 0x0, 0x0, 0x000000, 0x000000, nullptr,
      "render queue 2 drain cap per frame", "100000", "100000",
      0, 0, 0, 0, kActSet, 0, -1, kToNone, 0x186A0, kSameOff, nullptr },
    { 67, 0x01, 4, kAddr, -1, kProbeStr, 0, 0x0, 0x0, 0x0, 0x0, 0x4A4D3C, 0x4E6D6C, nullptr,
      "gMissionName", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 68, 0x0D, 5, kCall, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x184910, 0x189D50, nullptr,
      "after the map's ScriptInit: call LuaScript::CloseScript", "jmp to its stub", "call through BF1GameExt",
      0, 0, 0, 0, kActCall, 0, -1, kToPostHook, 0x0, kSameOff, nullptr },
    { 69, 0x0D, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x184BC0, 0x18A000, nullptr,
      "LuaScript::get", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 70, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x25C4B0, 0x262E80, nullptr,
      "lua_pushstring", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 71, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x25C910, 0x2632E0, nullptr,
      "lua_setglobal", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 72, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x25D790, 0x264160, nullptr,
      "lua_dofile", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
    { 73, 0x00, 0, kFunc, -1, kProbeNone, 0, 0x0, 0x0, 0x0, 0x0, 0x184910, 0x189D50, nullptr,
      "LuaScript::CloseScript", "", "",
      0, 0, 0, 0, kActNone, 0, -1, kToNone, 0x0, kSameOff, nullptr },
};
// ---- end of generated tables ----

constexpr int kGroupCount = static_cast<int>(sizeof(kGroups) / sizeof(kGroups[0]));
constexpr int kChunkCount = static_cast<int>(sizeof(kChunks) / sizeof(kChunks[0]));
constexpr int kSiteCount = static_cast<int>(sizeof(kSites) / sizeof(kSites[0]));
constexpr int kArrayCount = static_cast<int>(sizeof(kArrays) / sizeof(kArrays[0]));
constexpr int kBlockCount = static_cast<int>(sizeof(kBlocks) / sizeof(kBlocks[0]));
constexpr int kMaxHits = 4;
constexpr uint32_t kPreferredBase = 0x00400000;
constexpr int kWaterDefault = 800;
constexpr int kWaterMin = 200;
constexpr int kWaterMax = 4096;

// the C++ start-up (per build): the first slot of the constructor table that is not 0, the function it holds, the
// CRT's start-up state (0 none, 1 initializing, 2 done) and the main heap pointer (0 until the game's first allocation)
struct StartupInfo
{
    uint32_t slot;
    uint32_t target;
    uint32_t crtState;
    uint32_t heapPtr;
};
const StartupInfo kStartupSteam = { 0x0040C104, 0x003454D0, 0x021B9B00, 0x00581F18 };
const StartupInfo kStartupGog = { 0x003D6104, 0x0033EAB6, 0x02177AB0, 0x0053FEE8 };

struct ChunkFound
{
    int hits;
    uint32_t rva[kMaxHits];
    uint32_t at; // the one match (0: none or several)
};
ChunkFound g_cf[kChunkCount];
bool g_found = false;

bool g_iniRead = false;
int g_scanLevel = 1;
bool g_probeOn = true;
bool g_groupOn[kGroupCount];
int g_water = kWaterDefault;
char g_exeDir[MAX_PATH] = "";
double g_t0 = 0.0;
DWORD g_mainThreadId = 0;
volatile LONG g_scanDone = 0;

// per group: what happened (filled at the start-up hook / the DirectInput call)
struct GroupResult
{
    int earlyWrites;
    int diWrites;
    int failedWrites;
    bool earlyDone;
    bool diDone;
    char why[200];
};
GroupResult g_res[kGroupCount];

// BF1GameExt's own memory, the decal literals, the wrappers' targets
void* g_block[kBlockCount];
float g_decalFar = 1000.0f;
float g_decalFade = 500.0f;

typedef void(__stdcall* VecCtor_t)(void* p, unsigned size, int count, void* ctor);
typedef int(__cdecl* PoolCreate_t)(int type, int count, int a3, int a4, int a5);
typedef void*(__cdecl* LuaGet_t)();
typedef void(__cdecl* LuaStr_t)(void* L, const char* s);
typedef int(__cdecl* LuaDoFile_t)(void* L, const char* path);
typedef bool(__cdecl* CloseScript_t)();
typedef void(__fastcall* Ctor_t)(void* self, void* unusedEdx); // a thiscall with no arguments

VecCtor_t g_vecCtor = nullptr;
PoolCreate_t g_poolCreate = nullptr;
volatile LONG g_pool0 = 0; // pool 0's count through the wrapper (0: the game's own)
volatile LONG g_pool2 = 0; // pool 2's
LuaGet_t g_luaGet = nullptr;
LuaStr_t g_luaPushString = nullptr;
LuaStr_t g_luaSetGlobal = nullptr;
LuaDoFile_t g_luaDoFile = nullptr;
CloseScript_t g_closeScript = nullptr;
const char* g_missionName = nullptr;
char g_postPath[MAX_PATH + 40] = "";
volatile LONG g_postRuns = 0;

// the start-up slot and its hook
enum SlotState
{
    kSlotNotRead,
    kSlotRelocated,   // holds base + target: the hook went in
    kSlotUnrelocated, // holds 0x400000 + target with the exe elsewhere: left alone
    kSlotOther        // anything else: left alone
};
int g_slotState = kSlotNotRead;
uint32_t g_slotValue = 0;
bool g_probeInstalled = false;
uint32_t g_origInit = 0;
const char* g_probeWhy = "";

struct ProbeResult
{
    volatile LONG ran;
    double t;
    double tDone;
    bool onMain;
    uint32_t crt;
    uint32_t heap;
    int chunksTried;
    int chunksAtKnown;
    int operandsTried;
    int operandsRelocated;
    bool restored;
};
ProbeResult g_probe = {};

// Steam: the unlock, timed
struct WatchResult
{
    bool started;
    double tUnlocked;
    double tDecrypted;
    double tRelocated;
    uint32_t crtAt[3];
    uint32_t heapAt[3];
    bool moved;
    const char* end;
};
WatchResult g_watch = {};

// ----- small helpers ------------------------------------------------------------------------------------------------

double NowMs()
{
    static LARGE_INTEGER freq = {};
    if (!freq.QuadPart)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return static_cast<double>(t.QuadPart) * 1000.0 / static_cast<double>(freq.QuadPart);
}

double Since0()
{
    return g_t0 ? NowMs() - g_t0 : static_cast<double>(ScanMsSinceAttach());
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

const StartupInfo* Startup()
{
    return IsSteam() ? &kStartupSteam : (IsGog13() ? &kStartupGog : nullptr);
}

bool Readable(uintptr_t start, size_t len)
{
    MEMORY_BASIC_INFORMATION mbi;
    if (start < 0x10000 || !VirtualQuery(reinterpret_cast<LPCVOID>(start), &mbi, sizeof(mbi)))
        return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    return start + len <= reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
}

uint32_t ReadVa(uintptr_t va, int size)
{
    if (!Readable(va, static_cast<size_t>(size)))
        return 0xFFFFFFFFu;
    if (size == 1)
        return *reinterpret_cast<const uint8_t*>(va);
    if (size == 2)
    {
        uint16_t w = 0;
        memcpy(&w, reinterpret_cast<const void*>(va), sizeof(w));
        return w;
    }
    uint32_t v = 0;
    memcpy(&v, reinterpret_cast<const void*>(va), sizeof(v));
    return v;
}

uint32_t ReadAt(uint32_t rva, int size)
{
    return ReadVa(Rva(rva), size);
}

uint32_t AddrRva(uint32_t va)
{
    const BuildInfo* b = CurrentBuild();
    const uintptr_t base = ExeBase();
    if (!b || va < base || va >= base + b->sizeOfImage)
        return 0;
    return static_cast<uint32_t>(va - base);
}

uint32_t KnownRva(const ChunkInfo& c)
{
    return IsSteam() ? c.steamRva : c.gogRva;
}

uint32_t KnownTarget(const SiteInfo& s)
{
    return IsSteam() ? s.steamTarget : s.gogTarget;
}

int GroupOf(const SiteInfo& s)
{
    return kChunks[s.chunk].group;
}

void ReadIni()
{
    if (g_iniRead)
        return;
    g_iniRead = true;
    for (int i = 0; i < kGroupCount; ++i)
        g_groupOn[i] = kGroups[i].on != 0;
    char path[MAX_PATH];
    const DWORD n = GetModuleFileNameA(nullptr, path, MAX_PATH);
    if (!n || n >= MAX_PATH)
        return;
    char* slash = strrchr(path, '\\');
    if (!slash)
        return;
    *slash = 0;
    snprintf(g_exeDir, sizeof(g_exeDir), "%s", path);
    char ini[MAX_PATH + 32];
    snprintf(ini, sizeof(ini), "%s\\BF1GameExt.ini", path);
    g_scanLevel = static_cast<int>(GetPrivateProfileIntA("EGM", "Scan", 1, ini));
    g_probeOn = GetPrivateProfileIntA("EGM", "InitProbe", 1, ini) != 0;
    for (int i = 0; i < kGroupCount; ++i)
        g_groupOn[i] = GetPrivateProfileIntA("EGM", kGroups[i].name, kGroups[i].on, ini) != 0;
    int w = static_cast<int>(GetPrivateProfileIntA("EGM", "WaterSplashes", kWaterDefault, ini));
    if (w < kWaterMin)
        w = kWaterMin;
    if (w > kWaterMax)
        w = kWaterMax;
    g_water = w;
    snprintf(g_postPath, sizeof(g_postPath), "%s\\Hooks\\PostScriptInit.lua", path);
}

// writes size bytes at va, one page at a time (each page's protection put back as it was): no logging, no heap
bool WriteVa(uintptr_t va, const void* bytes, size_t size)
{
    const auto* src = static_cast<const unsigned char*>(bytes);
    uintptr_t p = va;
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

bool WriteDword(uintptr_t va, uint32_t v)
{
    return WriteVa(va, &v, 4);
}

// the address a site's operand holds, if the site's chunk matches at the build's known rva and the operand is relocated
// to where the build has it (the start-up hook and the watcher, before any search)
bool OperandAtKnown(const SiteInfo& s, uint32_t* va)
{
    const ChunkInfo& c = kChunks[s.chunk];
    if (!MatchPatternAt(KnownRva(c), c.pattern))
        return false;
    const uint32_t v = ReadAt(KnownRva(c) + s.off, 4);
    *va = v;
    return v == static_cast<uint32_t>(ExeBase() + KnownTarget(s));
}

// one probe's values, as text
void ProbeText(const SiteInfo& s, uint32_t va, char* out, size_t len)
{
    out[0] = 0;
    size_t n = 0;
    uintptr_t p = va;
    if (s.probe == kProbeStr)
    {
        char str[64] = {};
        for (int i = 0; i < 63 && Readable(p + static_cast<uintptr_t>(i), 1); ++i)
        {
            const char ch = *reinterpret_cast<const char*>(p + static_cast<uintptr_t>(i));
            if (!ch)
                break;
            str[i] = ch >= 0x20 && ch < 0x7F ? ch : '?';
        }
        snprintf(out, len, "\"%s\"", str);
        return;
    }
    if (s.probe == kProbePtrAt)
    {
        const uint32_t q = ReadVa(va + s.probeOff, 4);
        n += static_cast<size_t>(snprintf(out + n, len - n, "[+0x%X] = 0x%08X: ", s.probeOff, static_cast<unsigned>(q)));
        if (!q || q == 0xFFFFFFFFu)
            return;
        p = q;
    }
    else if (s.probe == kProbePtrThen)
    {
        const uint32_t q = ReadVa(va, 4);
        n += static_cast<size_t>(snprintf(out + n, len - n, "-> 0x%08X, +0x%X: ", static_cast<unsigned>(q), s.probeOff));
        if (!q || q == 0xFFFFFFFFu)
            return;
        p = q + s.probeOff;
    }
    for (int i = 0; i < s.probeN && n + 12 < len; ++i)
    {
        const uint32_t v = ReadVa(p + static_cast<uintptr_t>(i) * 4, 4);
        n += static_cast<size_t>(snprintf(out + n, len - n, "%08X ", static_cast<unsigned>(v)));
    }
}

// a probe's "not in use yet" test (its operand already checked): the dwords must be 0 (the pointer, for the two
// pointer modes)
bool ProbeReady(const SiteInfo& s, uint32_t va)
{
    if (!s.readyN)
        return true;
    if (s.probe == kProbePtrAt)
        return ReadVa(va + s.probeOff, 4) == 0;
    if (s.probe == kProbePtrThen)
        return ReadVa(va, 4) == 0;
    for (int i = 0; i < s.readyN; ++i)
        if (ReadVa(va + (static_cast<uintptr_t>(s.readyIdx) + static_cast<uintptr_t>(i)) * 4, 4) != 0)
            return false;
    return true;
}

// ----- the patterns -------------------------------------------------------------------------------------------------

void FindChunks()
{
    for (int i = 0; i < kChunkCount; ++i)
    {
        ChunkFound& f = g_cf[i];
        memset(&f, 0, sizeof(f));
        f.hits = FindPattern(kChunks[i].pattern, f.rva, kMaxHits);
        f.at = f.hits == 1 ? f.rva[0] : 0;
    }
    g_found = true;
}

// after the start-up hook found them: still there? (the patterns do not see the bytes 22b writes)
int RecheckChunks()
{
    int moved = 0;
    for (int i = 0; i < kChunkCount; ++i)
    {
        ChunkFound& f = g_cf[i];
        if (f.at && MatchPatternAt(f.at, kChunks[i].pattern))
            continue;
        ++moved;
        memset(&f, 0, sizeof(f));
        f.hits = FindPattern(kChunks[i].pattern, f.rva, kMaxHits);
        f.at = f.hits == 1 ? f.rva[0] : 0;
    }
    return moved;
}

// the callee of an E8 at va, through one jump thunk (as a va; 0 if not a call)
uintptr_t CalleeVa(uintptr_t va, bool throughThunk)
{
    if (ReadVa(va, 1) != 0xE8)
        return 0;
    const int32_t rel = static_cast<int32_t>(ReadVa(va + 1, 4));
    uintptr_t t = va + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(rel));
    if (throughThunk && ReadVa(t, 1) == 0xE9)
    {
        const int32_t r2 = static_cast<int32_t>(ReadVa(t + 1, 4));
        t = t + 5 + static_cast<uintptr_t>(static_cast<intptr_t>(r2));
    }
    return t;
}

int SiteByLabel(const char* label)
{
    for (int i = 0; i < kSiteCount; ++i)
        if (!strcmp(kSites[i].label, label))
            return i;
    return -1;
}

// the va of a kFunc site's function (0 if its pattern was not found once)
uintptr_t FuncVa(const char* label)
{
    const int i = SiteByLabel(label);
    if (i < 0 || !g_cf[kSites[i].chunk].at)
        return 0;
    return Rva(g_cf[kSites[i].chunk].at + kSites[i].off);
}

// the address a kAddr site holds, if it is the build's (0 otherwise)
uint32_t OperandVa(const SiteInfo& s)
{
    const uint32_t at = g_cf[s.chunk].at;
    if (!at)
        return 0;
    const uint32_t v = ReadAt(at + s.off, 4);
    return AddrRva(v) && AddrRva(v) == KnownTarget(s) ? v : 0;
}

// ----- the wrappers -------------------------------------------------------------------------------------------------

// the soldier animators: the game constructs its count (10, or [Limits]' 100) - 512 here (the pool, its header and loops
// are raised to match)
void __stdcall EgmVecWrap(void* p, unsigned size, int count, void* ctor)
{
    if (size == 0x2020 && (count == 10 || count == 100))
        count = 512;
    g_vecCtor(p, size, count, ctor);
}

// the render pools: pool 0's count (100 -> 1000) and pool 2's (render queue 2, 100 -> 100000)
int __cdecl EgmPoolWrap(int type, int count, int a3, int a4, int a5)
{
    if (type == 0 && count == 0x64 && g_pool0)
        count = static_cast<int>(g_pool0);
    else if (type == 2 && count == 0x64 && g_pool2)
        count = static_cast<int>(g_pool2);
    return g_poolCreate(type, count, a3, a4, a5);
}

// after a map's setup script: Hooks\PostScriptInit.lua (if there) in the map's Lua, then the game's CloseScript
bool __cdecl EgmPostHook()
{
    void* L = g_luaGet ? g_luaGet() : nullptr;
    if (L && g_postPath[0] && GetFileAttributesA(g_postPath) != INVALID_FILE_ATTRIBUTES)
    {
        const char* mission = g_missionName ? g_missionName : "";
        g_luaPushString(L, mission);
        g_luaSetGlobal(L, "gMissionName");
        const int r = g_luaDoFile(L, g_postPath);
        const LONG n = InterlockedIncrement(&g_postRuns);
        Log("[egm] PostHook #%ld: ran %s for \"%.48s\" - lua_dofile %d%s", static_cast<long>(n), g_postPath, mission, r,
            r ? " (an error: see the script)" : "");
    }
    return g_closeScript();
}

// ----- site state ---------------------------------------------------------------------------------------------------

enum SiteState
{
    kStStock,   // as the game has it
    kStRaised,  // [Limits]' value
    kStWritten, // what 22b writes
    kStOther,   // UNEXPECTED
    kStMissing  // its pattern was not found once
};

uintptr_t ToVa(uint8_t to)
{
    switch (to)
    {
    case kToSetSize:
        return FuncVa("SetSizeIfUnallocated (the game's lazy pool size)");
    case kToPoolWrap:
        return reinterpret_cast<uintptr_t>(&EgmPoolWrap);
    case kToVecWrap:
        return reinterpret_cast<uintptr_t>(&EgmVecWrap);
    case kToPostHook:
        return reinterpret_cast<uintptr_t>(&EgmPostHook);
    case kToDecalFar:
        return reinterpret_cast<uintptr_t>(&g_decalFar);
    case kToDecalFade:
        return reinterpret_cast<uintptr_t>(&g_decalFade);
    default:
        return 0;
    }
}

// the bytes a kActBytes site gets (the water count put in for "NN NN NN NN"); returns the length
int ActBytes(const SiteInfo& s, unsigned char* out, int max)
{
    int n = 0;
    const char* h = s.actHex;
    int nn = 0;
    while (*h && n < max)
    {
        if (*h == ' ')
        {
            ++h;
            continue;
        }
        if (h[0] == 'N' && h[1] == 'N')
        {
            out[n++] = static_cast<unsigned char>((static_cast<uint32_t>(g_water) >> (8 * nn)) & 0xFF);
            ++nn;
            h += 2;
            continue;
        }
        unsigned v = 0;
        for (int k = 0; k < 2; ++k)
        {
            const char c = h[k];
            v = (v << 4) | static_cast<unsigned>(c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' + 10 : 0));
        }
        out[n++] = static_cast<unsigned char>(v);
        h += 2;
    }
    return n;
}

uint32_t ActValue(const SiteInfo& s)
{
    return s.act == kActWaterSize ? 0x28u + 4u * static_cast<uint32_t>(g_water) : s.value;
}

// the address a kActRebase site gets
uint32_t RebaseVa(const SiteInfo& s)
{
    if (s.block < 0 || !g_block[s.block])
        return 0;
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_block[s.block]) + (s.newOff == kSameOff ? s.aoff : s.newOff));
}

// true if the site already holds what 22b writes there
bool IsWritten(const SiteInfo& s, uint32_t rva)
{
    switch (s.act)
    {
    case kActSet:
    case kActWaterSize:
        return ReadAt(rva, s.size) == ActValue(s);
    case kActRebase:
    {
        const uint32_t v = RebaseVa(s);
        return v && ReadAt(rva, 4) == v;
    }
    case kActBytes:
    {
        unsigned char b[64];
        const int n = ActBytes(s, b, static_cast<int>(sizeof(b)));
        for (int i = 0; i < n; ++i)
            if (ReadAt(rva + static_cast<uint32_t>(i), 1) != b[i])
                return false;
        return n > 0;
    }
    case kActCall:
    {
        const uintptr_t t = ToVa(s.to);
        return t && CalleeVa(Rva(rva), false) == t;
    }
    case kActPtr:
        return ReadAt(rva, 4) == static_cast<uint32_t>(ToVa(s.to));
    default:
        return false;
    }
}

// one site's state; now = text for the log (may be null)
int SiteNow(const SiteInfo& s, char* now, size_t len)
{
    char dummy[8];
    if (!now)
    {
        now = dummy;
        len = sizeof(dummy);
    }
    now[0] = 0;
    const uint32_t at = g_cf[s.chunk].at;
    if (!at)
    {
        snprintf(now, len, "NOT FOUND (its pattern matched %d times)", g_cf[s.chunk].hits);
        return kStMissing;
    }
    const uint32_t rva = at + s.off;
    if (s.act != kActNone && s.act != kActData && IsWritten(s, rva))
    {
        snprintf(now, len, "BF1GameExt's (22b)");
        return kStWritten;
    }
    int state = kStStock;
    switch (s.kind)
    {
    case kConst:
    {
        const uint32_t v = ReadAt(rva, s.size);
        state = v == s.stock ? kStStock : (s.alt && v == s.alt ? kStRaised : kStOther);
        if (state == kStOther)
            snprintf(now, len, "0x%X UNEXPECTED (stock 0x%X)", static_cast<unsigned>(v), static_cast<unsigned>(s.stock));
        else
            snprintf(now, len, "0x%X %s", static_cast<unsigned>(v), state == kStStock ? "stock" : "[Limits]' (BF1MemExt's)");
        break;
    }
    case kFloat:
    {
        const uint32_t v = ReadAt(rva, 4);
        float f = 0.0f;
        memcpy(&f, &v, 4);
        state = v == s.stock ? kStStock : kStOther;
        snprintf(now, len, "%.1f %s", static_cast<double>(f), state == kStStock ? "stock" : "UNEXPECTED");
        break;
    }
    case kAddr:
    case kData:
    {
        const uint32_t v = ReadAt(rva, 4);
        const uint32_t r = AddrRva(v);
        const bool ok = r && r == KnownTarget(s);
        state = ok ? kStStock : kStOther;
        if (s.kind == kData)
        {
            const uint32_t d = r ? ReadAt(r, 4) : 0xFFFFFFFFu;
            if (ok && d == s.value && s.act == kActData)
            {
                snprintf(now, len, "-> rva 0x%06X: %u BF1GameExt's (22b)", static_cast<unsigned>(r), static_cast<unsigned>(d));
                return kStWritten;
            }
            if (ok && d != s.stock)
                state = kStOther;
            snprintf(now, len, "-> rva 0x%06X: %u %s", static_cast<unsigned>(r), static_cast<unsigned>(d),
                     state == kStStock ? "stock" : "not stock");
        }
        else
            snprintf(now, len, "-> rva 0x%06X%s", static_cast<unsigned>(r), ok ? "" : " UNEXPECTED");
        break;
    }
    case kCode:
    {
        const bool ok = MatchPatternAt(rva, s.hex);
        state = ok ? kStStock : kStOther;
        size_t n = 0;
        for (int i = 0; i < s.size && n + 4 < len; ++i)
            n += static_cast<size_t>(snprintf(now + n, len - n, "%02X ", static_cast<unsigned>(ReadAt(rva + static_cast<uint32_t>(i), 1))));
        snprintf(now + n, len - n, "%s", ok ? "stock" : "UNEXPECTED");
        break;
    }
    case kCall:
    {
        const uintptr_t c = CalleeVa(Rva(rva), true);
        const uint32_t cr = AddrRva(static_cast<uint32_t>(c));
        if (cr && cr == KnownTarget(s))
            state = kStStock;
        else if (s.loose && c && !cr)
            state = kStRaised; // another BF1GameExt wrapper ([Limits]')
        else
            state = kStOther;
        if (state == kStRaised)
            snprintf(now, len, "calls 0x%08X ([Limits]' wrapper)", static_cast<unsigned>(c));
        else
            snprintf(now, len, "calls rva 0x%06X%s", static_cast<unsigned>(cr), state == kStStock ? "" : " UNEXPECTED");
        break;
    }
    default: // kFunc
        state = rva == KnownTarget(s) ? kStStock : kStOther;
        snprintf(now, len, "function at rva 0x%06X%s", static_cast<unsigned>(rva), state == kStStock ? "" : " (not the known rva)");
        break;
    }
    return state;
}

struct Tally
{
    int stock;
    int raised;
    int written;
    int other;
    int missing;
};

void Count(Tally* t, int state)
{
    if (state == kStStock)
        ++t->stock;
    else if (state == kStRaised)
        ++t->raised;
    else if (state == kStWritten)
        ++t->written;
    else if (state == kStMissing)
        ++t->missing;
    else
        ++t->other;
}

void LogSiteLine(const SiteInfo& s, int state, const char* now)
{
    const uint32_t at = g_cf[s.chunk].at;
    if (state == kStMissing)
        Log("[egm]    %-46s %s", s.label, now);
    else if (s.egm[0] || s.plan[0])
        Log("[egm]    %-46s rva 0x%06X  %s | EGM: %s | 22b: %s", s.label, static_cast<unsigned>(at + s.off), now, s.egm, s.plan);
    else
        Log("[egm]    %-46s rva 0x%06X  %s", s.label, static_cast<unsigned>(at + s.off), now);
    if (s.probe != kProbeNone && s.kind == kAddr && state == kStStock)
    {
        char pt[200];
        ProbeText(s, ReadAt(at + s.off, 4), pt, sizeof(pt));
        Log("[egm]       now: %s", pt);
    }
}

// ----- applying a group ---------------------------------------------------------------------------------------------

// can the group's part (early: the start-up hook's; else the DirectInput call's) go in? Every site it writes found and
// as expected (stock, [Limits]' or already 22b's), every probe of that part ready (not in use yet)
bool GroupReady(int gi, bool early, char* why, size_t len)
{
    int parts = 0;
    for (int i = 0; i < kSiteCount; ++i)
    {
        const SiteInfo& s = kSites[i];
        if (GroupOf(s) != gi)
            continue;
        const bool mine = s.act != kActNone && (s.early != 0) == early;
        const bool probe = s.readyN && (s.readyEarly != 0) == early;
        if (!mine && !probe)
            continue;
        if (mine)
            ++parts;
        char now[200];
        const int st = SiteNow(s, now, sizeof(now));
        if (st == kStMissing || (st == kStOther && !(s.kind == kData && s.act == kActData)))
        {
            snprintf(why, len, "%s: %s", s.label, now);
            return false;
        }
        if (probe)
        {
            const uint32_t va = OperandVa(s);
            if (!va || !ProbeReady(s, va))
            {
                char pt[160];
                ProbeText(s, va, pt, sizeof(pt));
                snprintf(why, len, "%s is in use already (%s)", s.label, pt);
                return false;
            }
        }
    }
    if (!parts)
    {
        snprintf(why, len, "nothing in this part");
        return false;
    }
    return true;
}

bool AllocBlocks(int gi, bool early, char* why, size_t len)
{
    for (int i = 0; i < kSiteCount; ++i)
    {
        const SiteInfo& s = kSites[i];
        if (GroupOf(s) != gi || s.act != kActRebase || (s.early != 0) != early || s.block < 0 || g_block[s.block])
            continue;
        g_block[s.block] = VirtualAlloc(nullptr, kBlocks[s.block].bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!g_block[s.block])
        {
            snprintf(why, len, "no memory for %s (%u bytes, error %lu)", kBlocks[s.block].name,
                     static_cast<unsigned>(kBlocks[s.block].bytes), GetLastError());
            return false;
        }
    }
    return true;
}

// the group's own preparations before its sites are written (false: nothing written)
bool GroupPrepare(int gi, bool early, char* why, size_t len)
{
    const char* name = kGroups[gi].name;
    if (!strcmp(name, "HighRes") && !early)
    {
        const int vs = SiteByLabel("animator vector constructor call");
        const uintptr_t pc = FuncVa("render pool create");
        if (vs < 0 || !pc)
        {
            snprintf(why, len, "the vector constructor / pool create not found");
            return false;
        }
        const uintptr_t vc = CalleeVa(Rva(g_cf[kSites[vs].chunk].at + kSites[vs].off), false);
        if (!vc || vc == reinterpret_cast<uintptr_t>(&EgmVecWrap))
        {
            if (!g_vecCtor)
            {
                snprintf(why, len, "the vector constructor call is not the game's");
                return false;
            }
        }
        else
            g_vecCtor = reinterpret_cast<VecCtor_t>(vc);
        g_poolCreate = reinterpret_cast<PoolCreate_t>(pc);
        InterlockedExchange(&g_pool0, 1000);
    }
    if (!strcmp(name, "FixEffects") && !early)
    {
        const uintptr_t pc = FuncVa("render pool create");
        if (!pc)
        {
            snprintf(why, len, "the pool create not found");
            return false;
        }
        g_poolCreate = reinterpret_cast<PoolCreate_t>(pc);
        InterlockedExchange(&g_pool2, 100000);
    }
    if (!strcmp(name, "Sound") && !early)
    {
        const int cs = SiteByLabel("SndSource3D constructor (static constructor)");
        const uint32_t ctor = cs >= 0 ? OperandVa(kSites[cs]) : 0;
        int b = -1;
        for (int k = 0; k < kBlockCount; ++k)
            if (!strcmp(kBlocks[k].name, "Source3D"))
                b = k;
        if (!ctor || b < 0 || !g_block[b])
        {
            snprintf(why, len, "the SndSource3D constructor / block not ready");
            return false;
        }
        // BF1GameExt's 256 SndSource3D, built by the game's own constructor (0x2C each)
        const Ctor_t c = reinterpret_cast<Ctor_t>(static_cast<uintptr_t>(ctor));
        for (uint32_t k = 0; k < 256; ++k)
            c(static_cast<unsigned char*>(g_block[b]) + k * 0x2C, nullptr);
    }
    if (!strcmp(name, "PostHook") && !early)
    {
        g_luaGet = reinterpret_cast<LuaGet_t>(FuncVa("LuaScript::get"));
        g_luaPushString = reinterpret_cast<LuaStr_t>(FuncVa("lua_pushstring"));
        g_luaSetGlobal = reinterpret_cast<LuaStr_t>(FuncVa("lua_setglobal"));
        g_luaDoFile = reinterpret_cast<LuaDoFile_t>(FuncVa("lua_dofile"));
        g_closeScript = reinterpret_cast<CloseScript_t>(FuncVa("LuaScript::CloseScript"));
        const int ms = SiteByLabel("gMissionName");
        const uint32_t m = ms >= 0 ? OperandVa(kSites[ms]) : 0;
        g_missionName = m ? reinterpret_cast<const char*>(static_cast<uintptr_t>(m)) : nullptr;
        if (!g_luaGet || !g_luaPushString || !g_luaSetGlobal || !g_luaDoFile || !g_closeScript || !g_missionName)
        {
            snprintf(why, len, "a Lua helper not found");
            return false;
        }
    }
    return true;
}

// writes one site (already checked); false if the write failed
bool WriteSite(const SiteInfo& s)
{
    const uint32_t rva = g_cf[s.chunk].at + s.off;
    const uintptr_t va = Rva(rva);
    switch (s.act)
    {
    case kActSet:
    case kActWaterSize:
    {
        const uint32_t v = ActValue(s);
        return WriteVa(va, &v, s.size);
    }
    case kActRebase:
        return WriteDword(va, RebaseVa(s));
    case kActBytes:
    {
        unsigned char b[64];
        const int n = ActBytes(s, b, static_cast<int>(sizeof(b)));
        return n > 0 && WriteVa(va, b, static_cast<size_t>(n));
    }
    case kActCall:
    {
        const uintptr_t t = ToVa(s.to);
        if (!t || ReadVa(va, 1) != 0xE8)
            return false;
        const int32_t rel = static_cast<int32_t>(t - (va + 5));
        return WriteVa(va + 1, &rel, 4);
    }
    case kActPtr:
        return WriteDword(va, static_cast<uint32_t>(ToVa(s.to)));
    case kActData:
    {
        const uint32_t tv = ReadAt(rva, 4);
        if (!AddrRva(tv) || ReadVa(tv, 4) != s.stock)
            return true; // not stock (a setting chose another): left as it is
        return WriteDword(tv, s.value);
    }
    default:
        return true;
    }
}

// applies one group's part; returns true if it went in (or was in already)
bool ApplyGroup(int gi, bool early)
{
    GroupResult& r = g_res[gi];
    if (!g_groupOn[gi])
        return false;
    char why[200] = "";
    if (!GroupReady(gi, early, why, sizeof(why)))
    {
        if (strcmp(why, "nothing in this part"))
            snprintf(r.why, sizeof(r.why), "%s: %s", early ? "at the start-up" : "at the DirectInput call", why);
        return false;
    }
    if (!AllocBlocks(gi, early, why, sizeof(why)) || !GroupPrepare(gi, early, why, sizeof(why)))
    {
        snprintf(r.why, sizeof(r.why), "%s: %s", early ? "at the start-up" : "at the DirectInput call", why);
        return false;
    }
    int w = 0;
    int f = 0;
    for (int i = 0; i < kSiteCount; ++i)
    {
        const SiteInfo& s = kSites[i];
        if (GroupOf(s) != gi || s.act == kActNone || (s.early != 0) != early)
            continue;
        if (s.act != kActData && IsWritten(s, g_cf[s.chunk].at + s.off))
            continue;
        if (WriteSite(s))
            ++w;
        else
            ++f;
    }
    if (early)
    {
        r.earlyWrites = w;
        r.earlyDone = !f;
    }
    else
    {
        r.diWrites = w;
        r.diDone = !f;
    }
    r.failedWrites += f;
    return !f;
}

// ----- the start-up hook --------------------------------------------------------------------------------------------

void __cdecl InitProbe()
{
    const StartupInfo* st = Startup();
    const uint32_t orig = g_origInit;
    // put the slot back first: whatever happens below, the game's own function runs next
    if (st)
        g_probe.restored = WriteDword(Rva(st->slot), orig);
    g_probe.t = Since0();
    g_probe.onMain = GetCurrentThreadId() == g_mainThreadId;
    g_probe.crt = st ? ReadAt(st->crtState, 4) : 0;
    g_probe.heap = st ? ReadAt(st->heapPtr, 4) : 0;
    for (int i = 0; i < kChunkCount; ++i)
    {
        ++g_probe.chunksTried;
        if (MatchPatternAt(KnownRva(kChunks[i]), kChunks[i].pattern))
            ++g_probe.chunksAtKnown;
    }
    for (int i = 0; i < kSiteCount; ++i)
    {
        const SiteInfo& s = kSites[i];
        if (s.kind != kAddr || s.probe == kProbeNone)
            continue;
        ++g_probe.operandsTried;
        uint32_t va = 0;
        if (OperandAtKnown(s, &va))
            ++g_probe.operandsRelocated;
    }
    Log("[egm] start-up hook: the C++ start-up's first constructor, %.0f ms after load, %s; CRT state %u, main heap "
        "pointer 0x%08X%s; %d of %d EGM patterns at this build's rvas, %d of %d object operands relocated; slot %s",
        g_probe.t, g_probe.onMain ? "on the main thread" : "NOT on the main thread", static_cast<unsigned>(g_probe.crt),
        static_cast<unsigned>(g_probe.heap), g_probe.heap ? " (the heap is already made)" : " (no heap yet)",
        g_probe.chunksAtKnown, g_probe.chunksTried, g_probe.operandsRelocated, g_probe.operandsTried,
        g_probe.restored ? "put back" : "NOT put back");
    const bool safe = g_probe.onMain && g_probe.crt == 1 && g_probe.chunksAtKnown == g_probe.chunksTried &&
                      g_probe.operandsRelocated == g_probe.operandsTried;
    if (safe)
    {
        FindChunks();
        for (int gi = 0; gi < kGroupCount; ++gi)
        {
            if (!g_groupOn[gi])
                continue;
            const bool hasEarly = [gi]() {
                for (int i = 0; i < kSiteCount; ++i)
                    if (GroupOf(kSites[i]) == gi && kSites[i].act != kActNone && kSites[i].early)
                        return true;
                return false;
            }();
            if (!hasEarly)
                continue;
            if (ApplyGroup(gi, true))
                Log("[egm]   %s: applied at the start-up - %d writes", kGroups[gi].name, g_res[gi].earlyWrites);
            else
                Log("[egm]   %s: NOT applied at the start-up - %s", kGroups[gi].name,
                    g_res[gi].why[0] ? g_res[gi].why : "some writes failed");
        }
    }
    else
        Log("[egm]   the early groups are left alone: not the moment expected (main thread, CRT state 1, every pattern "
            "and operand in place)");
    g_probe.tDone = Since0();
    InterlockedExchange(&g_probe.ran, 1);
    if (orig)
        reinterpret_cast<void(__cdecl*)()>(static_cast<uintptr_t>(orig))();
}

// ----- Steam: the unlock, timed -------------------------------------------------------------------------------------

DWORD WINAPI WatchThread(LPVOID)
{
    const StartupInfo* st = Startup();
    const int si = SiteByLabel("sOPAQUE list");
    if (si < 0 || !st)
    {
        g_watch.end = "internal: no start-up site";
        return 0;
    }
    const SiteInfo& s = kSites[si];
    const ChunkInfo& c = kChunks[s.chunk];
    const uint32_t want = static_cast<uint32_t>(ExeBase() + s.steamTarget);
    g_watch.moved = ExeBase() != kPreferredBase;
    while (Since0() < 60000.0)
    {
        if (g_probe.ran)
        {
            g_watch.end = "the start-up hook ran";
            break;
        }
        if (g_scanDone)
        {
            g_watch.end = "the scan ran";
            break;
        }
        if (!g_watch.tUnlocked && CodeLooksUnlocked())
        {
            g_watch.tUnlocked = Since0();
            g_watch.crtAt[0] = ReadAt(st->crtState, 4);
            g_watch.heapAt[0] = ReadAt(st->heapPtr, 4);
        }
        if (!g_watch.tDecrypted && MatchPatternAt(c.steamRva, c.pattern))
        {
            g_watch.tDecrypted = Since0();
            g_watch.crtAt[1] = ReadAt(st->crtState, 4);
            g_watch.heapAt[1] = ReadAt(st->heapPtr, 4);
        }
        if (g_watch.tDecrypted && !g_watch.tRelocated && ReadAt(c.steamRva + s.off, 4) == want)
        {
            g_watch.tRelocated = Since0();
            g_watch.crtAt[2] = ReadAt(st->crtState, 4);
            g_watch.heapAt[2] = ReadAt(st->heapPtr, 4);
        }
        if (g_watch.tUnlocked && g_watch.tDecrypted && g_watch.tRelocated)
        {
            g_watch.end = "all three seen";
            break;
        }
        if (Since0() < 15000.0)
            SwitchToThread();
        else
            Sleep(1);
    }
    if (!g_watch.end)
        g_watch.end = "60 s passed";
    return 0;
}

// ----- the DirectInput call -----------------------------------------------------------------------------------------

void LogArrays(int gi)
{
    for (int a = 0; a < kArrayCount; ++a)
    {
        int n = 0;
        int agree = 0;
        int moved = 0;
        uint32_t base = 0;
        for (int i = 0; i < kSiteCount; ++i)
        {
            const SiteInfo& s = kSites[i];
            if (s.array != a || GroupOf(s) != gi || !g_cf[s.chunk].at)
                continue;
            const uint32_t v = ReadAt(g_cf[s.chunk].at + s.off, 4);
            const uint32_t r = AddrRva(v);
            ++n;
            if (!r)
            {
                ++moved; // into BF1GameExt's block (22b)
                continue;
            }
            const uint32_t b = r - s.aoff;
            if (!base)
                base = b;
            if (b == base)
                ++agree;
        }
        if (!n)
            continue;
        if (moved == n)
            Log("[egm]    %s: all %d places point into BF1GameExt's own memory now", kArrays[a], n);
        else
            Log("[egm]    %s: at rva 0x%06X, %d of %d places agree%s%s", kArrays[a], static_cast<unsigned>(base), agree, n,
                moved ? " (the others moved)" : "", agree + moved == n ? "" : " - THEY DISAGREE");
    }
}

void LogStartup()
{
    const StartupInfo* st = Startup();
    if (!g_probeOn)
        Log("[egm] start-up hook: off ([EGM] InitProbe=0) - RenderPools, Foliage and FixEffects' scene queue need it");
    else if (!g_probeInstalled)
        Log("[egm] start-up hook: NOT put in - %s (slot rva 0x%06X held 0x%08X; the exe at 0x%08X)", g_probeWhy,
            st ? static_cast<unsigned>(st->slot) : 0u, static_cast<unsigned>(g_slotValue), static_cast<unsigned>(ExeBase()));
    else if (!g_probe.ran)
        Log("[egm] start-up hook: put in at load (slot rva 0x%06X, relocated), but it NEVER RAN", st ? static_cast<unsigned>(st->slot) : 0u);
    else
        Log("[egm] start-up hook: ran %.0f ms after load (done %.0f ms), %s, CRT state %u, main heap pointer 0x%08X, %d of %d "
            "patterns at their rvas, %d of %d object operands relocated, slot %s",
            g_probe.t, g_probe.tDone, g_probe.onMain ? "main thread" : "NOT the main thread", static_cast<unsigned>(g_probe.crt),
            static_cast<unsigned>(g_probe.heap), g_probe.chunksAtKnown, g_probe.chunksTried, g_probe.operandsRelocated,
            g_probe.operandsTried, g_probe.restored ? "put back" : "NOT put back");
    if (!g_watch.started)
        return;
    Log("[egm] Steam unlock watcher (%s): code readable %.0f ms (CRT %u, heap 0x%08X), decrypted %.0f ms (CRT %u, heap "
        "0x%08X), relocated %.0f ms (CRT %u, heap 0x%08X)%s",
        g_watch.end ? g_watch.end : "still running", g_watch.tUnlocked, static_cast<unsigned>(g_watch.crtAt[0]),
        static_cast<unsigned>(g_watch.heapAt[0]), g_watch.tDecrypted, static_cast<unsigned>(g_watch.crtAt[1]),
        static_cast<unsigned>(g_watch.heapAt[1]), g_watch.tRelocated, static_cast<unsigned>(g_watch.crtAt[2]),
        static_cast<unsigned>(g_watch.heapAt[2]), g_watch.moved ? "" : " (the exe is at 0x00400000: decrypted = relocated)");
}

} // namespace

void EgmScanEarly()
{
    if (!IsSteam() && !IsGog13())
        return;
    g_t0 = NowMs();
    g_mainThreadId = GetCurrentThreadId();
    ReadIni();
    if (g_scanLevel <= 0 && !g_probeOn)
        return;
    const StartupInfo* st = Startup();
    if (!st)
        return;
    g_slotValue = ReadAt(st->slot, 4);
    const uint32_t relocated = static_cast<uint32_t>(ExeBase() + st->target);
    const uint32_t fileValue = kPreferredBase + st->target;
    bool before = true; // the slots before it are 0 (it is the first constructor)
    for (uint32_t r = st->slot - 0x104; r < st->slot; r += 4)
        before = before && ReadAt(r, 4) == 0;
    if (g_slotValue == relocated && before)
        g_slotState = kSlotRelocated;
    else if (g_slotValue == fileValue)
        g_slotState = kSlotUnrelocated;
    else
        g_slotState = kSlotOther;
    if (!g_probeOn)
        g_probeWhy = "off";
    else if (g_slotState == kSlotUnrelocated)
        g_probeWhy = "the slot still held the exe file's address (not relocated yet)";
    else if (g_slotState == kSlotOther)
        g_probeWhy = "the slot did not hold the start-up function";
    else
    {
        g_origInit = g_slotValue;
        const uint32_t probe = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&InitProbe));
        if (WriteDword(Rva(st->slot), probe))
            g_probeInstalled = true;
        else
            g_probeWhy = "the slot could not be written";
    }
    if (IsSteam())
    {
        const HANDLE t = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
        if (t)
        {
            g_watch.started = true;
            CloseHandle(t);
        }
    }
}

void EgmScanRun(bool onGameThread)
{
    static bool ran = false;
    if (ran)
        return;
    ran = true;
    if (!IsSteam() && !IsGog13())
    {
        Log("[egm] not GOG 1.3 or Steam - EGM's patches are not used");
        return;
    }
    ReadIni();
    InterlockedExchange(&g_scanDone, 1);
    char on[300];
    size_t n = 0;
    for (int gi = 0; gi < kGroupCount; ++gi)
        n += static_cast<size_t>(snprintf(on + n, sizeof(on) - n, "%s%s=%d", gi ? " " : "", kGroups[gi].name, g_groupOn[gi] ? 1 : 0));
    Log("[egm] 22b: EGM's patches (the SWBF Launcher by Antstafer and abkarino, CC BY-NC 4.0) - [EGM] %s WaterSplashes=%d "
        "Scan=%d InitProbe=%d; %s, %.0f ms after load, %s, %s",
        on, g_water, g_scanLevel, g_probeOn ? 1 : 0, IsSteam() ? "Steam" : "GOG 1.3", Since0(),
        onGameThread ? "at the game's DirectInput call" : "from the timer",
        GetCurrentThreadId() == g_mainThreadId ? "on the main thread" : "NOT on the main thread");
    LogStartup();
    const double f0 = NowMs();
    int moved = 0;
    if (g_found)
        moved = RecheckChunks();
    else
        FindChunks();
    int once = 0;
    int known = 0;
    for (int i = 0; i < kChunkCount; ++i)
    {
        once += g_cf[i].at ? 1 : 0;
        known += g_cf[i].at && g_cf[i].at == KnownRva(kChunks[i]) ? 1 : 0;
    }
    Log("[egm] %d of %d patterns found exactly once (%d at this build's known rva%s), %.0f ms", once, kChunkCount, known,
        g_found && !moved ? "; all where the start-up hook found them" : "", NowMs() - f0);
    Tally total = {};
    int applied = 0;
    int groupsOn = 0;
    for (int gi = 0; gi < kGroupCount; ++gi)
    {
        const GroupInfo& g = kGroups[gi];
        GroupResult& r = g_res[gi];
        // the state before this call's writes (every site; the early groups' already show BF1GameExt's)
        Tally t = {};
        if (g_scanLevel >= 2)
            Log("[egm] == %s: %s", g.name, g.what);
        for (int i = 0; i < kSiteCount; ++i)
        {
            const SiteInfo& s = kSites[i];
            if (GroupOf(s) != gi)
                continue;
            char now[220];
            const int st = SiteNow(s, now, sizeof(now));
            Count(&t, st);
            if (g_scanLevel >= 2 || (g_scanLevel >= 1 && (st == kStOther || st == kStMissing) &&
                                     !(s.kind == kData && s.act == kActData)))
                LogSiteLine(s, st, now);
        }
        if (g_scanLevel >= 2)
            LogArrays(gi);
        // this call's part
        bool diPart = false;
        for (int i = 0; i < kSiteCount && !diPart; ++i)
            diPart = GroupOf(kSites[i]) == gi && kSites[i].act != kActNone && !kSites[i].early;
        if (g_groupOn[gi])
            ++groupsOn;
        if (g_groupOn[gi] && diPart)
        {
            if (!onGameThread)
                snprintf(r.why, sizeof(r.why), "at the DirectInput call: only from the game's own thread (this came from the timer)");
            else
                ApplyGroup(gi, false);
        }
        bool hasEarly = false;
        for (int i = 0; i < kSiteCount && !hasEarly; ++i)
            hasEarly = GroupOf(kSites[i]) == gi && kSites[i].act != kActNone && kSites[i].early;
        const bool ok = g_groupOn[gi] && (!hasEarly || r.earlyDone) && (!diPart || r.diDone);
        if (ok)
            ++applied;
        if (!g_groupOn[gi])
            Log("[egm] %-13s off ([EGM] %s=0) - %d sites: %d stock, %d [Limits]', %d other, %d not found", g.name, g.name,
                t.stock + t.raised + t.written + t.other + t.missing, t.stock, t.raised, t.other, t.missing);
        else if (ok)
            Log("[egm] %-13s ON: %d writes%s - %s", g.name, r.earlyWrites + r.diWrites,
                hasEarly ? (diPart ? " (start-up + DirectInput call)" : " (at the start-up)") : "", g.what);
        else
            Log("[egm] %-13s NOT APPLIED%s: %s", g.name, (hasEarly && r.earlyDone) ? " (its start-up part is in)" : "",
                r.why[0] ? r.why : (r.failedWrites ? "some writes failed" : (hasEarly && !g_probe.ran ? "the start-up hook did not run" : "not tried")));
        if (!strcmp(g.name, "FixEffects") && ok)
            Log("[egm]               water splashes: %d slots (object 0x%X bytes); scene queue and render queue 2: 100000",
                g_water, 0x28 + 4 * g_water);
        total.stock += t.stock;
        total.raised += t.raised;
        total.written += t.written;
        total.other += t.other;
        total.missing += t.missing;
    }
    Log("[egm] done: %d of %d groups on and in. Before this call's writes: %d sites - %d stock, %d [Limits]', %d already "
        "BF1GameExt's (start-up), %d other, %d not found",
        applied, groupsOn, kSiteCount, total.stock, total.raised, total.written, total.other, total.missing);
    if (g_scanLevel >= 2)
    {
        // every site after the writes
        for (int gi = 0; gi < kGroupCount; ++gi)
        {
            Tally t = {};
            for (int i = 0; i < kSiteCount; ++i)
                if (GroupOf(kSites[i]) == gi)
                    Count(&t, SiteNow(kSites[i], nullptr, 0));
            Log("[egm] after: %-13s %d BF1GameExt's, %d stock, %d [Limits]', %d other, %d not found", kGroups[gi].name,
                t.written, t.stock, t.raised, t.other, t.missing);
            LogArrays(gi);
        }
    }
}

} // namespace bf1
