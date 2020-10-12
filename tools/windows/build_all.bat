::
:: XRhodes
::
:: copyright (c) Gyorgy Straub. All rights reserved.
::
:: License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
::
::==============================================================================
:: Builds every target of xr3 for windows.
::
:: If MSBUILD_PATH is not set in the environment, it will look for it in the
:: MSVC installation directory. It will not set it.

@echo off
setlocal enabledelayedexpansion
pushd %~dp0

cd ..\..\.projects\windows

set MSVC_SEARCH_DIR="C:\Program Files (x86)\"
for /f "usebackq tokens=*" %%i in (
	`dir /s /b %MSVC_SEARCH_DIR% ^| findstr MSBuild.exe$ ^| findstr /v amd64`
) do (
	echo MSBuild exe found: %%i
	echo
	set MSBUILD_PATH="%%i"
)

set SLN="xr3.sln"

%MSBUILD_PATH% %SLN% /p:Configuration=Debug /p:Platform=win32
%MSBUILD_PATH% %SLN% /p:Configuration=Debug /p:Platform=x64
%MSBUILD_PATH% %SLN% /p:Configuration=Release /p:Platform=win32
%MSBUILD_PATH% %SLN% /p:Configuration=Release /p:Platform=x64

popd
