::
:: XRhodes
::
:: copyright (c) Gyorgy Straub. All rights reserved.
::
:: License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
::
::==============================================================================
:: Generates VS2017 projects.

@echo off
pushd %~dp0
premake5 --target=windows vs2017 --file=../../premake5.lua
popd