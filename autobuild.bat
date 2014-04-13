@echo off

if "%1"=="" goto :fail

::set IDE_PATH="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
::set IDE_SUFFIX=vc9

set IDE_PATH="C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\vcexpress.exe"
set IDE_SUFFIX=vc10x

@echo building '%1'

@echo cleaning build directory
del /q /f "%1/build_%1_%IDE_SUFFIX%"
del /q /f "%1/lib"

@echo building project...
"%1/%1.mkb" --no-ide

@echo executing builds...
@echo on
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "x86 Debug" /log "%~dp0/%1/logs/x86 Debug log.xml"
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "GCC ARM Release" /log "%~dp0/%1/logs/GCC ARM Release log.xml"
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "GCC ARM Debug" /log "%~dp0/%1/logs/GCC ARM Debug log.xml"
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "x86 Release" /log "%~dp0/%1/logs/x86 Release log.xml"
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "GCC MIPS Debug" /log "%~dp0/%1/logs/GCC MIPS Debug log.xml"
%IDE_PATH% "%1/build_%1_%IDE_SUFFIX%/%1_%IDE_SUFFIX%.sln" /build "GCC MIPS Release" /log "%~dp0/%1/logs/GCC MIPS Release log.xml"
@echo off

if "%2"=="-s" call :shutdown

goto :eof

:shutdown
shutdown /s /t
goto :eof

:fail
echo Specify solution to build as 1st parameter.
pause
goto :eof