// BF1GameExt - game_build.hpp   (file build p23a-r1)
// 23a (v6.3): moved to src/mr_rogue/core/; file names in the comments and log lines follow the new names. Nothing else changed.
// Which Battlefront.exe is running, and where things are inside it.
//
// Every address is an RVA (offset from the exe's load address). The GOG 1.2
// exe always loads at 0x00400000; the 2020 exes (GOG 1.3, Steam) can load
// anywhere (ASLR), so code always goes through Rva(). Byte-checks account for
// the load address using the exe's own relocation table.
//
// Byte patterns may contain "??" for a byte that is not compared (call
// distances and absolute addresses, which differ between builds).
//
// Steam: the code is locked on disk by Steam's DRM and only unlocked in memory
// at startup. BF1GameExt never removes or bypasses that; it waits for the
// unlock, then finds every site in memory using the GOG 1.3 patterns
// (runtime_resolve.cpp).
#pragma once

#include <cstddef>
#include <cstdint>

namespace bf1 {

struct ByteSite
{
    const char* name;
    uint32_t rva;
    const char* hex; // bytes as they are in the exe file (preferred load address)
};

struct LuaSite
{
    const char* name;
    uint32_t funcRva;
};

struct BuildInfo
{
    const char* name;
    uint32_t timeDateStamp;
    uint32_t sizeOfImage;
    const char* sha256;

    // Entry point (for loading BF1MemExt before the game starts)
    uint32_t entryRva;
    const char* entryHex;
    bool loadsMemExt; // BF1MemExt only supports the 1.2 code

    // Split screen values
    uint32_t localPlayerCountRva; // DWORD, 1 = normal, 2 = split screen
    uint32_t wasSplitRva;         // BYTE, set when returning to the menus
    uint32_t bootSplitFlagRva;    // BYTE, developer "boot into split" switch

    const ByteSite* splitSites;
    size_t splitSiteCount;
    const LuaSite* luaSites;
    size_t luaSiteCount;

    // Controls (profile layout is the same in both builds: 0x458-byte
    // profiles, joystick flag at +0x370, binding tables at +0x34/+0xE0/+0x18C)
    uint32_t profileIndexRva;  // int, -1 = none
    uint32_t profileArrayRva;  // profile 0, profile 1 follows at +0x458
    uint32_t localSlotsRva;    // 2 x 0x30: +0x00 controller id, +0x10 profile
    const ByteSite* controlSites;
    size_t controlSiteCount;

    // Only meaningful on 1.2: do the menu .lvl files call 2020-only functions?
    bool checkMenuScripts;

    // Steam: code unreadable until Steam unlocks it in memory.
    bool codeLockedOnDisk;
    // Split screen and controls addresses are found in memory at scan time
    // (the zero RVAs above are filled in by runtime_resolve.cpp).
    bool resolveAtRuntime;
};

// Reads the exe's PE header in memory and picks the matching build.
// Call once, early (DllMain). Returns null for an unknown exe.
const BuildInfo* DetectBuild();

// The build found by DetectBuild (null if unknown).
const BuildInfo* CurrentBuild();

// Exe load address, and load address minus the preferred 0x00400000.
uintptr_t ExeBase();
intptr_t LoadDelta();

// Exe load address + rva.
uintptr_t Rva(uint32_t rva);

// Compares the bytes at site.rva with site.hex, after shifting every
// relocated address inside the site by LoadDelta(). Logs a MISMATCH with
// expected/actual bytes when it fails and logMismatch is true.
bool CheckSite(const ByteSite& site, bool logMismatch);

// RVA of a Lua function in the current build's table, or 0 if not listed.
uint32_t LuaFuncRva(const char* name);

// True if the bytes at rva match the pattern ("??" = any byte). No relocation
// adjustment: put "??" over absolute addresses.
bool MatchPatternAt(uint32_t rva, const char* pattern);

// Searches the exe's code section. Writes up to maxHits RVAs to hits and
// returns the total number of matches (which can be larger than maxHits).
int FindPattern(const char* pattern, uint32_t* hits, int maxHits);

// RVA and size of the exe's code section (false if not found).
bool CodeSection(uint32_t* rva, uint32_t* size);

// False while the code section still looks locked (Steam, before the unlock).
// Always true for builds whose code is readable on disk.
bool CodeLooksUnlocked();

// Replaces CurrentBuild() with a copy whose runtime-found addresses are filled in.
void PublishResolvedBuild(const BuildInfo* resolved);

} // namespace bf1
