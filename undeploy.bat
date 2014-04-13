@if "%1"=="" goto :fail

@call del /q /f "%S3E_DIR%\..\modules\%1\h\*.*"
@call del /q /f "%S3E_DIR%\..\modules\%1\lib\arm\*.*"
@call del /q /f "%S3E_DIR%\..\modules\%1\lib\mips\linux\*.*"
@call del /q /f "%S3E_DIR%\..\modules\%1\lib\x86\*.*"
@call del /q /f "%S3E_DIR%\..\modules\%1\%1.mkf"
@call rmdir "%S3E_DIR%\..\modules\%1\h"
@call rmdir "%S3E_DIR%\..\modules\%1\lib\arm"
@call rmdir "%S3E_DIR%\..\modules\%1\lib\mips\linux"
@call rmdir "%S3E_DIR%\..\modules\%1\lib\x86"
@call rmdir "%S3E_DIR%\..\modules\%1\lib"
@call rmdir "%S3E_DIR%\..\modules\%1"

goto :eof

:fail
@echo Specify solution to undeploy as 1st parameter.
pause