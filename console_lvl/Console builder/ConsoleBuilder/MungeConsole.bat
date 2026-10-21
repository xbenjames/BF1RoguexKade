@ECHO OFF
REM MungeConsole.bat   (file build p21j-r30b)
REM BF1GameExt's console.lvl with the mod tools' ScriptMunge + LevelPack (in ToolsFL\Bin), as MungeShellGOG.bat munges
REM the shell: the scripts in Console\Scripts -> _LVL_PC\console.lvl, then copied to the game's GameData\Data\_LVL_PC
REM (the game's folder is set at the top of _BUILD_PC\Console\munge.bat). Works from wherever this folder is.
REM The output is in MungeLogConsole.txt too.
IF NOT EXIST Console\Scripts MKDIR Console\Scripts
if EXIST MungeLogConsole.txt del MungeLogConsole.txt

cd _BUILD_PC
 powershell -NoProfile -Command ".\mungeconsole.bat | tee ..\MungeLogConsole.txt"
 cd ..

if EXIST MungeLogConsole.txt %systemroot%\Notepad.exe MungeLogConsole.txt

Echo Done Munging, Potential errors can be found in MungeLogConsole.txt

PAUSE
