echo Onbuild:
echo   Config:   %1
echo   Platform: %2
echo   Target:   %3

::copy "..\_lib\log4cpp\bin\%2\log4cpp.dll" "..\_%1\%2\log4cpp.dll"
copy "..\_lib\winsdk\gpsvc.dll" "..\_%1\%2\bin\gpsvc.dll"
copy "..\_lib\winsdk\IEShims.dll" "..\_%1\%2\bin\IEShims.dll"
copy "..\_lib\winsdk\msvcp110.dll" "..\_%1\%2\bin\msvcp110.dll"
copy "..\_lib\winsdk\msvcr110.dll" "..\_%1\%2\bin\msvcr110.dll"
copy "..\_lib\winsdk\sysntfy.dll" "..\_%1\%2\bin\sysntfy.dll"

