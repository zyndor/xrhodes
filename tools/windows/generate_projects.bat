@echo off
pushd %~dp0
premake5 --target=windows vs2015 --file=../../premake5.lua
popd