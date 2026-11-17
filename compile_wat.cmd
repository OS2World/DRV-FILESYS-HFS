@echo off
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
if "%PROJ_ROOT%"=="" set PROJ_ROOT=.
set INCLUDE=%WATCOM%\H;%WATCOM%\H\OS2;%PROJ_ROOT%\src\include
set LIB=%WATCOM%\LIB386;%WATCOM%\LIB386\OS2;%WATCOM%\LIB286;%WATCOM%\LIB286\OS2
set EDPATH=%WATCOM%\EDDAT

if not exist "%WATCOM%\BINP\WMAKE.EXE" goto missing_wmake
if "%1"=="clean" goto clean

echo Building HFS/2 ...
echo Project root: %PROJ_ROOT%
if not exist "%PROJ_ROOT%\bin" mkdir "%PROJ_ROOT%\bin"
if not exist "%PROJ_ROOT%\bin\obj" mkdir "%PROJ_ROOT%\bin\obj"
if not exist "%PROJ_ROOT%\dll" mkdir "%PROJ_ROOT%\dll"
if not exist "%PROJ_ROOT%\lib" mkdir "%PROJ_ROOT%\lib"
"%WATCOM%\BINP\WMAKE.EXE" -a -z -f "%PROJ_ROOT%\Makefile.wat" "PROJ_ROOT=%PROJ_ROOT%" > compile.log 2>&1
if errorlevel 1 goto build_failed
if not exist "%PROJ_ROOT%\bin\hfs.ifs" goto missing_driver
if not exist "%PROJ_ROOT%\bin\hfsflt.flt" goto missing_filter
if not exist "%PROJ_ROOT%\bin\hfs.exe" goto missing_ring3_exe
if not exist "%PROJ_ROOT%\bin\fskill.exe" goto missing_utility
if not exist "%PROJ_ROOT%\bin\sync.exe" goto missing_utility
if not exist "%PROJ_ROOT%\bin\readres.exe" goto missing_utility
if not exist "%PROJ_ROOT%\bin\bindisk.exe" goto missing_utility
if not exist "%PROJ_ROOT%\bin\prepare.exe" goto missing_utility
if not exist "%PROJ_ROOT%\dll\uhfs.dll" goto missing_dll
if not exist "%PROJ_ROOT%\lib\libhfs16.lib" goto missing_uhfs_lib
if not exist "%PROJ_ROOT%\lib\libhfs32.lib" goto missing_ring3_lib
echo OK: %PROJ_ROOT%\bin\hfs.ifs
dir "%PROJ_ROOT%\bin\hfs.ifs"
echo OK: %PROJ_ROOT%\bin\hfsflt.flt
dir "%PROJ_ROOT%\bin\hfsflt.flt"
echo OK: %PROJ_ROOT%\bin\hfs.exe and utility executables
dir "%PROJ_ROOT%\bin\*.exe"
echo OK: %PROJ_ROOT%\dll\uhfs.dll
dir "%PROJ_ROOT%\dll\uhfs.dll"
echo OK: %PROJ_ROOT%\lib\libhfs16.lib
dir "%PROJ_ROOT%\lib\libhfs16.lib"
echo OK: %PROJ_ROOT%\lib\libhfs32.lib
dir "%PROJ_ROOT%\lib\libhfs32.lib"
echo Object files emitted under %PROJ_ROOT%\bin\obj
dir "%PROJ_ROOT%\bin\obj\*.obj" /s
echo Build log: compile.log
exit 0

:clean
"%WATCOM%\BINP\WMAKE.EXE" -f "%PROJ_ROOT%\Makefile.wat" "PROJ_ROOT=%PROJ_ROOT%" clean
if errorlevel 1 goto clean_failed
echo OK: Known build outputs cleaned.
exit 0

:missing_wmake
echo FAILED: Open Watcom wmake was not found at %WATCOM%\BINP\WMAKE.EXE.
exit 1

:build_failed
type compile.log
echo FAILED: Open Watcom build failed. See compile.log.
exit 1

:missing_driver
echo FAILED: Build did not produce %PROJ_ROOT%\bin\hfs.ifs.
exit 1

:missing_filter
echo FAILED: Build did not produce %PROJ_ROOT%\bin\hfsflt.flt.
exit 1

:missing_dll
echo FAILED: Build did not produce %PROJ_ROOT%\dll\uhfs.dll.
exit 1

:missing_uhfs_lib
echo FAILED: Build did not produce %PROJ_ROOT%\lib\libhfs16.lib.
exit 1

:missing_ring3_lib
echo FAILED: Build did not produce %PROJ_ROOT%\lib\libhfs32.lib.
exit 1

:missing_ring3_exe
echo FAILED: Build did not produce %PROJ_ROOT%\bin\hfs.exe.
exit 1

:missing_utility
echo FAILED: One or more utility executables were not produced under %PROJ_ROOT%\bin.
exit 1

:clean_failed
echo FAILED: Could not clean known build outputs.
exit 1
