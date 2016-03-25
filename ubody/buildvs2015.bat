@echo off
echo == Building for VS2015 ==
echo.

bin\premake5.exe --file=vs2015.lua vs2015


:end
echo.
timeout 3