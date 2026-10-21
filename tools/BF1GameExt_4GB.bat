@echo off
rem BF1GameExt_4GB.bat   (file build p22e-r2)
rem OPTIONAL: turns the 4 GB flag on Battlefront.exe on or off (for EGM's maps at full detail) - runs BF1GameExt_4GB.ps1,
rem which must be next to this file. Put both next to Battlefront.exe (where dinput8.dll is) and double-click this.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0BF1GameExt_4GB.ps1" %*
pause
