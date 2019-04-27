@echo off
pushd "%~dp0"

@echo [ Execute cmake build... ]
cmake -BBuild

:exit
popd
@echo.
pause
@echo on