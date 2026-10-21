@ECHO OFF
REM _BUILD_PC\Console\munge.bat   (file build p21j-r30b) - BF1GameExt's console.lvl
ECHO START %~f0

REM 1 = each script keeps its file name and line numbers (a script error in the game names its line), 0 = stripped
REM (smaller, like the game's own shell scripts). BF1GameExt runs it either way.
IF "%CONSOLE_DEBUG_SYMBOLS%"=="" SET CONSOLE_DEBUG_SYMBOLS=1
REM the game's GameData folder: console.lvl is copied to its Data\_LVL_PC - change it if your game is somewhere else
IF "%BFX_GAMEDATA%"=="" SET "BFX_GAMEDATA=C:\Program Files (x86)\Steam\steamapps\common\Star Wars Battlefront (Classic 2004)\GameData"

REM the mod tools (ScriptMunge runs luac)
SET BFX_TOOL=
FOR %%t IN (ScriptMunge.exe LevelPack.exe luac.exe) DO IF NOT EXIST "..\..\ToolsFL\Bin\%%t" SET BFX_TOOL=%%t
IF DEFINED BFX_TOOL GOTO notools
IF NOT EXIST ..\..\Console\Scripts\*.lua GOTO noscripts
IF NOT EXIST munged MKDIR munged
DEL /Q munged\*.* 2>NUL
IF NOT EXIST ..\..\_LVL_PC MKDIR ..\..\_LVL_PC
IF EXIST ..\..\_LVL_PC\console.lvl DEL ..\..\_LVL_PC\console.lvl

ECHO The scripts in Console\Scripts:
FOR %%f IN (..\..\Console\Scripts\*.lua) DO @FINDSTR /C:".lua   (file build " "%%f"
FOR %%n IN (BF1GameExt_split BF1GameExt_pause34 BF1GameExt_ctrlpage) DO IF NOT EXIST "..\..\Console\Scripts\%%n.lua" ECHO NOTE: no %%n.lua in Console\Scripts - console.lvl will not have it; BF1GameExt uses the %%n.lua next to the game's exe, else its own copy

SET LUA_DEBUG_SYMBOLS=
IF "%CONSOLE_DEBUG_SYMBOLS%"=="1" SET LUA_DEBUG_SYMBOLS=1

ECHO SCRIPTMUNGE Console\Scripts\*.lua (debug symbols %CONSOLE_DEBUG_SYMBOLS%)
scriptmunge -sourcedir ..\..\Console -platform pc -inputfile Scripts\*.lua -outputdir munged\ -continue 2>&1

REM every script in Console\Scripts must have compiled - else no console.lvl at all (LevelPack would write one without
REM the scripts that did not)
SET BFX_MISSING=
FOR %%f IN (..\..\Console\Scripts\*.lua) DO IF NOT EXIST "munged\%%~nf.script" SET BFX_MISSING=%%~nxf
IF DEFINED BFX_MISSING GOTO notcompiled

REM console.req made from the scripts that are there (all of them go in console.lvl)
> munged\console.req ECHO ucft
>> munged\console.req ECHO {
>> munged\console.req ECHO     REQN
>> munged\console.req ECHO     {
>> munged\console.req ECHO         "script"
FOR %%f IN (..\..\Console\Scripts\*.lua) DO >> munged\console.req ECHO         "%%~nf"
>> munged\console.req ECHO     }
>> munged\console.req ECHO }

ECHO LEVELPACK console.req (the scripts in Console\Scripts)
levelpack -sourcedir munged -platform pc -inputdir munged\ -outputdir ..\..\_LVL_PC\ -inputfile console.req 2>&1

IF NOT EXIST ..\..\_LVL_PC\console.lvl GOTO failed
ECHO _LVL_PC\console.lvl made
IF EXIST ..\..\ToolsFL\Bin\ConsoleMunge.exe ConsoleMunge -list ..\..\_LVL_PC\console.lvl
IF NOT EXIST "%BFX_GAMEDATA%\Data\_LVL_PC\core.lvl" GOTO nogame
COPY /Y ..\..\_LVL_PC\console.lvl "%BFX_GAMEDATA%\Data\_LVL_PC\console.lvl"
IF ERRORLEVEL 1 GOTO nocopy
ECHO copied to the game: %BFX_GAMEDATA%\Data\_LVL_PC\console.lvl
GOTO end

:notools
ECHO **** ERROR: no %BFX_TOOL% in ToolsFL\Bin - ScriptMunge.exe, LevelPack.exe and luac.exe (the mod tools) must be
ECHO there; nothing was munged
GOTO end
:nogame
ECHO No game found at %BFX_GAMEDATA%
ECHO - copy _LVL_PC\console.lvl to your game's GameData\Data\_LVL_PC yourself (or set its folder at the top of
ECHO _BUILD_PC\Console\munge.bat)
GOTO end
:nocopy
ECHO **** ERROR: console.lvl could not be copied to the game (is the game running?) - it is in _LVL_PC\console.lvl
GOTO end
:noscripts
ECHO **** ERROR: no .lua files in Console\Scripts - put BF1GameExt_split.lua, BF1GameExt_pause34.lua and
ECHO BF1GameExt_ctrlpage.lua there
GOTO end
:notcompiled
ECHO **** ERROR: %BFX_MISSING% did not compile - its error is above; no console.lvl made, the game's console.lvl was
ECHO not touched
GOTO end
:failed
ECHO **** ERROR: console.lvl was not made - LevelPack's error is above; the game's console.lvl was not touched
:end
ECHO END %~f0
