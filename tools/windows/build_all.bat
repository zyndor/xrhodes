::
:: XRhodes
::
:: copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
::
::==============================================================================
:: Builds every target of XR3.SDL

@echo off
pushd %~dp0

cd ..\..\.projects\windows

set MSBUILD="C:\Program Files (x86)\MSBuild\14.0\Bin\MSBuild.exe"
set SLN="xr3.SDL.sln"

%MSBUILD% %SLN% /p:Configuration=Debug /p:Platform=win32
%MSBUILD% %SLN% /p:Configuration=Debug /p:Platform=x64
%MSBUILD% %SLN% /p:Configuration=Release /p:Platform=win32
%MSBUILD% %SLN% /p:Configuration=Release /p:Platform=x64

popd
