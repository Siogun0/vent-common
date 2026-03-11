:: Скрипт обновления номера сборки
@echo off
setlocal enabledelayedexpansion

set headerloc=%1
::set headerloc="..\Core\Inc\bn.h"

if exist %headerloc% (
    FOR /F "tokens=1,2* delims=, " %%i in ('findstr /I "VERSION_BUILD" "%headerloc%"') do set bn=%%k
    set /a newbn=bn + 1
) else (
    set newbn=0
)

if %newbn% == 10000 (
    set newbn=0
)

::echo In file %headerloc%
::echo Old build number %bn%
::echo New build number %newbn%

echo /*It's autogenerate file! Don't edit it.*/ > %headerloc%
echo.  >> %headerloc%
echo #ifndef METADATA_BN_H__ >> %headerloc%
echo #define METADATA_BN_H_ >> %headerloc%
echo.  >> %headerloc%
echo #define VERSION_BUILD  %newbn% >> %headerloc%
echo.  >> %headerloc%
echo #endif /* METADATA_BN_H_ */ >> %headerloc%

endlocal