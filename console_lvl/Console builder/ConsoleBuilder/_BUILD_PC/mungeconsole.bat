@ECHO OFF
REM _BUILD_PC\mungeconsole.bat   (file build p21j-r30b) - called by MungeConsole.bat (the tools in ToolsFL\Bin)
setlocal
set "PATH=%CD%\..\ToolsFL\Bin;%PATH%"
cd Console
call munge.bat
cd ..
endlocal
