echo Onbuild:
echo   Config:   %1
echo   Platform: %2
echo   Target:   %3

copy "..\_lib\log4cpp\bin\%2\log4cpp.dll" "..\_%1\%2\log4cpp.dll"