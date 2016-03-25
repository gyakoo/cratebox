@echo off
echo == Building for VS2012 ==
echo.

bin\premake5.exe --file=win32vs2012.lua vs2012


:end
echo.
timeout 3