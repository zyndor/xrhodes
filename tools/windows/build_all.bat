::
:: XRhodes
::
:: copyright (c) Gyorgy Straub. All rights reserved.
::
:: License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
::
::==============================================================================
:: Builds every target of xr3 for windows.

@echo off
pushd %~dp0

cd ..\..\.projects\windows

set MSBUILD="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"
set SLN="xr3.sln"

%MSBUILD% %SLN% /p:Configuration=Debug /p:Platform=win32
%MSBUILD% %SLN% /p:Configuration=Debug /p:Platform=x64
%MSBUILD% %SLN% /p:Configuration=Release /p:Platform=win32
%MSBUILD% %SLN% /p:Configuration=Release /p:Platform=x64

popd
