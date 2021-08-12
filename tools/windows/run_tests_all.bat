::
:: XRhodes
::
:: copyright (c) Gyorgy Straub. All rights reserved.
::
:: License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
::
::==============================================================================
:: Runs unit tests for all configurations.

@echo off
pushd %~dp0

set FILTER=*
if [] neq [%1] (
	set FILTER=%1
)

cd ..\..\.artifacts\windows\

for /f "" %%i in ('"dir /s /b unittests.exe"') do (
	echo %%i
	call %%i %FILTER%
)
popd