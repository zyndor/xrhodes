@echo off

if "%1"=="" goto :fail

@echo building '%1'

@echo building project...
"%1/%1.mkb" --no-ide --clean

@echo executing builds...
@echo on
"%1/%1.mkb" --no-ide --buildenv=VC10X --debug --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC10X --release --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC10X --debug --arm --compiler=gcc --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC10X --release --arm --compiler=gcc --make --clean

"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP8DEV --debug --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP8DEV --release --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP8DEV --debug --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP8DEV --release --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS8DEV --debug --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS8DEV --release --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS8DEV --debug --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS8DEV --release --x86 --make --clean

"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP81DEV --debug --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP81DEV --release --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP81DEV --debug --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WP81DEV --release --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS81DEV --debug --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS81DEV --release --arm --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS81DEV --debug --x86 --make --clean
"%1/%1.mkb" --no-ide --buildenv=VC12 --buildenv-toolset=WS81DEV --release --x86 --make --clean
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