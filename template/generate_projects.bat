@echo off
pushd %XR3_PATH%\tools\windows\
premake5 --file=%~dp0\premake5.lua --target=windows vs2017
popd