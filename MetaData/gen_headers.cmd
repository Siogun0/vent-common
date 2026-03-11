:: Скрипт генерации персональных данных
@echo off
echo Generate Personal data
set headerloc=..\Core\Inc\personal.h
echo /*It's autogenerate file! Don't edit it.*/ > %headerloc%
echo.  >> %headerloc%
echo #ifndef PERSONAL_H_ >> %headerloc%
echo #define PERSONAL_H_ >> %headerloc%
echo.  >> %headerloc%
echo #define META_USERNAME   "%USERNAME%@%COMPUTERNAME%" >> %headerloc%
echo.  >> %headerloc%
echo #endif /* PERSONAL_H_ */ >> %headerloc%

set headerloc=..\Core\Inc\bn.h
if exist %headerloc% (
	exit
)
echo Buil Number created
echo /*It's autogenerate file! Don't edit it.*/ > %headerloc%
echo.  >> %headerloc%
echo #ifndef METADATA_BN_H__ >> %headerloc%
echo #define METADATA_BN_H_ >> %headerloc%
echo.  >> %headerloc%
echo #define VERSION_BUILD  0 >> %headerloc%
echo.  >> %headerloc%
echo #endif /* METADATA_BN_H_ */ >> %headerloc%


