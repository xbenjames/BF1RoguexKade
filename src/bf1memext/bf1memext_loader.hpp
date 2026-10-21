// BF1GameExt - bf1memext_loader.hpp   (file build p23a-r1)
// Credit: BF1MemExt (the extended limits DLL this loads on GOG 1.2), and Dark_Phantom. See CREDITS.txt in this folder.
// 23a (v6.3): moved to src/bf1memext/ (was memext_loader.hpp); file names in the comments and log lines follow the new names. Nothing else changed.
// Loads BF1MemExt.dll without patching Battlefront.exe on disk.
//
// BF1MemExt.exe normally adds a ".bf1ext" startup section to the exe that loads
// BF1MemExt.dll just before the game's entry point runs. This does the same job
// from memory: DllMain points the exe's entry point at EntryHook, and when the
// game starts, EntryHook puts the original entry bytes back, loads
// BF1MemExt.dll, then runs the game's real entry point.
#pragma once

namespace bf1 {

// Called from DllMain. Byte-checks the entry point, then installs the hook.
// Returns false (and logs why) if the exe isn't the clean GOG v1.2 exe.
bool MemExtInstallEntryHook();

} // namespace bf1
