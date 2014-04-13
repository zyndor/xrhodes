@echo off

if "%1"=="" goto :fail

if "%2"=="-u" call :undeploy %1

mkdir "%S3E_DIR%\..\modules\%1"
mkdir "%S3E_DIR%\..\modules\%1\h"
mkdir "%S3E_DIR%\..\modules\%1\lib\arm"
mkdir "%S3E_DIR%\..\modules\%1\lib\mips\linux"
mkdir "%S3E_DIR%\..\modules\%1\lib\x86

copy "%1\h\*" "%S3E_DIR%\..\modules\%1\h\"
copy "%1\lib\arm\*" "%S3E_DIR%\..\modules\%1\lib\arm\"
copy "%1\lib\mips\linux\*" "%S3E_DIR%\..\modules\%1\lib\mips\linux\"
copy "%1\lib\x86\*" "%S3E_DIR%\..\modules\%1\lib\x86\"
copy "%1\%1.mkf" "%S3E_DIR%\..\modules\%1\%1.mkf"

@echo Done.
goto :eof

:undeploy
call undeploy %1
goto :eof

:fail
echo Specify solution to build as 1st parameter.
pause
goto :eof
