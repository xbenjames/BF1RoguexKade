BF1GameExt p21j-r31 - all three scripts in console.lvl   (file build p21j-r31)

ConsoleBuilder\   the standalone console.lvl munger (the mod tools are in its ToolsFL\Bin) - unpack it anywhere.
    Console\Scripts has all three scripts: BF1GameExt_split.lua (p21j-r30), BF1GameExt_ctrlpage.lua (p21j-r30),
    BF1GameExt_pause34.lua (p21j-r31). Double-click MungeConsole.bat: it compiles them, makes console.lvl, lists it and
    copies it to the game's GameData\Data\_LVL_PC (the game's folder is set at the top of _BUILD_PC\Console\munge.bat).

BF1GameExt_source\   controller_scan.cpp and dllmain.cpp (r31 - rebuild the DLL; the .vcxproj and BF1GameExt.ini did not
                      change) and the three scripts.

Then delete the old BF1GameExt_split.lua, BF1GameExt_pause34.lua and BF1GameExt_ctrlpage.lua next to the game's exe
(GameData): console.lvl comes first anyway, but if it were ever gone those old copies would be used again
(the pause34 there was p21h-r10).

The game's log: first line PHASE 21J R31; the [scripts] line lists console.lvl's three scripts with their builds.

ConsoleMunge_source\ConsoleMunge.cpp - the source of ToolsFL\Bin\ConsoleMunge.exe.
