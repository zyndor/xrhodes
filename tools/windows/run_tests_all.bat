::
:: XRhodes
::
:: copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
	call %%i --gtest_filter=%FILTER%
)
popd