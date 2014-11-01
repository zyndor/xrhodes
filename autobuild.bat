@echo off

if "%1"=="" goto :fail

@echo building '%1'

@echo building project...
"%1/%1.mkb" --no-ide --clean

@echo executing builds...
@echo on
"%1/%1.mkb" --no-ide --buildenv=VC10X --debug --x86 --make
"%1/%1.mkb" --no-ide --buildenv=VC10X --release --x86 --make
"%1/%1.mkb" --no-ide --buildenv=VC10X --debug --arm --compiler=gcc --make
"%1/%1.mkb" --no-ide --buildenv=VC10X --release --arm --compiler=gcc --make
"%1/%1.mkb" --no-ide --buildenv=VC11XWP --buildenv-toolset=WP8 --debug --arm --make
"%1/%1.mkb" --no-ide --buildenv=VC11XWP --buildenv-toolset=WP8 --release --arm --make
"%1/%1.mkb" --no-ide --buildenv=VC11XWP --buildenv-toolset=WP8 --debug --x86 --make
"%1/%1.mkb" --no-ide --buildenv=VC11XWP --buildenv-toolset=WP8 --release --x86 --make
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