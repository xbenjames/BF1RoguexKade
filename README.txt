BF1GameExt - version 6.3 (build p23a-r1)
Star Wars Battlefront (2004): split screen for up to 4 players, extended limits, EGM's patches, sprint, ragdolls,
the console spawn screens and more - one dinput8.dll next to Battlefront.exe (GOG 1.3, Steam; GOG 1.2 with BF1MemExt).

Version 6.3 only reorganised the source: folders by who wrote what, the "_scan" files renamed. Nothing in the code
changed - the game runs exactly as 22e. The log's first line names the build: PHASE 23A R1.


WHERE THINGS ARE
----------------
BF1GameExt.sln / BF1GameExt.vcxproj        open the .sln in Visual Studio; Solution Explorer shows these same folders
src\mr_rogue\                              Mr. Rogue's work
    core\          dllmain, the log, the crash logger, the memory log, the exe check, per-build addresses,
                   Steam's locked-code lookups, the Direct3D hooks ([perf] lines)
    splitscreen\   split screen: the switch, side-by-side, players 3 / 4, player 2's input, pads 2 - 4,
                   auto-spawn, players 2 - 4's spawn screens and pause menus, split shadows / effects
    menus\         the menus: options pages, MODS, the split screen sign-in, free camera, Instant Action options,
                   tutorials (and the Lua the DLL carries)
    spawn\         the class wheel and the console spawn screens
    sprint\        sprint and the stamina bar
    ragdoll\       ragdolls (Bullet physics)
    fixes\         the prone fix, the loading screen fill, the texture cap
    diagnostics\   old log-only checks (input, loading screen, prone) - nothing in the game changes through them
src\kade_fett\                             Kade Fett's fixes, adapted for 4-player split screen (waiting for his code)
src\antstafer_abkarino_egm\                EGM's patches (the SWBF Launcher by Antstafer and abkarino)
src\bf1memext\                             the extended limits (BF1MemExt) and its loader for GOG 1.2
third_party\bullet3\                       Bullet Physics 3.25 (the ragdolls' physics library, its own licence)
console_lvl\                               the Lua scripts that go into console.lvl (Data\_LVL_PC)
config\                                    BF1GameExt.ini (copy it next to Battlefront.exe)
tools\                                     BF1GameExt_4GB.bat / .ps1 - the optional 4 GB patch (GOG only)
archive\                                   old copies and INI backups - not built, not shipped


CREDITS
-------
Mr. Rogue                       BF1GameExt: split screen, menus, spawn screens, sprint, ragdolls, fixes
Kade Fett                       his fixes (adapted for 4-player split screen), and support
Antstafer and abkarino          EGM's patches - the SWBF Launcher (CC BY-NC 4.0)
Dark_Phantom                    the first extended limits used with BF1GameExt, and the GOG source for the shell and common
S1thK3nny, Ryan Hank, Sleepy    BF2GameExt and the BF2 extended limits work this grew out of
Dave                            support and testing
Bullet Physics                  the ragdolls' physics library (zlib licence, third_party\bullet3\LICENSE.txt)


THE RENAMED FILES (v6.3)
------------------------
scan                  -> src\mr_rogue\core\game_check
frame_capture         -> src\mr_rogue\core\render_hooks
split_test            -> src\mr_rogue\splitscreen\split_core
playern_scan          -> src\mr_rogue\splitscreen\players34
pad2                  -> src\mr_rogue\splitscreen\pads
p2spawn_scan          -> src\mr_rogue\splitscreen\split_spawn_menus
controller_scan       -> src\mr_rogue\menus\menus
classwheel_scan       -> src\mr_rogue\spawn\class_wheel
spawn_scan            -> src\mr_rogue\spawn\spawn_manager
spawn_ui_scan         -> src\mr_rogue\spawn\spawn_ui
input_scan            -> src\mr_rogue\diagnostics\input_log
load_scan             -> src\mr_rogue\diagnostics\loadscreen_log
prone_scan            -> src\mr_rogue\diagnostics\prone_log
egm_scan              -> src\antstafer_abkarino_egm\egm
limits_scan           -> src\bf1memext\limits
memext_loader         -> src\bf1memext\bf1memext_loader
Every other file kept its name and moved to the folder above. The log's [tags] did not change.
