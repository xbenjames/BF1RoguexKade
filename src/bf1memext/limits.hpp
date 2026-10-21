// BF1GameExt - limits.hpp   (file build p23a-r1)
// Credit: the extended limits - BF1MemExt's limit patches, and Dark_Phantom (his limits were the first used with
//         BF1GameExt's split screen); brought into BF1GameExt by Mr. Rogue. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/bf1memext/ (was limits_scan.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Phase 16b: BF1MemExt limits on GOG 1.3 and Steam (18r: Steam's early sets reworked). See limits.cpp.
// 21j-r17: LimitsDlcView / LimitsDlcEntry - the addon mission list for the MODS page's ADDON FOLDER.
#pragma once

#include <cstddef>
#include <cstdint>

namespace bf1 {

// 21j-r17: the addon (DLC) mission list as the game has it now: the missions the addme scripts registered (count), the cap
// AddDownloadableContent checks (512 the game's own, 4096 with [Limits] DLCMissions=1), the game's own cap (stock: 512) and
// the list itself. false (count / cap / list 0) when the list's code was not found (GOG 1.2, unknown builds).
bool LimitsDlcView(uint32_t* count, uint32_t* cap, uint32_t* stock, uintptr_t* list);

// 21j-r17: entry i of that list: its mission name (AddDownloadableContent's second argument) and the full path of the addon
// folder it came from. false when the entry is not readable.
bool LimitsDlcEntry(uintptr_t list, uint32_t i, char* mission, size_t mlen, char* path, size_t plen);

// At load (DllMain, main thread). GOG 1.3: raises the heaps and hi-res units
// now. Steam (18r): starts the thread that raises the heaps any time before
// the game makes its main heap (main thread paused, heap pointer still 0,
// the thread not inside the heap code).
void LimitsScanEarly();

// With the other scans (the game's DirectInput call): checks every site,
// logs what the early part did, raises Steam's hi-res units (18r: while the
// animator object does not exist yet), reads every early value back, and
// raises the matrix pool and the DLC mission list - only when called on the
// game's own thread.
void LimitsScanRun(bool onGameThread);

} // namespace bf1
