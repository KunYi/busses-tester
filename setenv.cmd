@echo off

arm-none-eabi-gcc -v 2> NUL
if %ERRORLEVEL% EQU 0 goto gccDone

set ARM_GCC_DIR=%ProgramFiles%\GNU Tools ARM Embedded
if exist "%ARM_GCC_DIR%" goto foundGCC

set ARM_GCC_DIR=%ProgramFiles(x86)%\GNU Tools ARM Embedded
if exist "%ARM_GCC_DIR%" goto foundGCC

echo Please ensure that arm-none-eabi-gcc is installed to Program Files.
echo Please install the latest Windows Installer from 
echo     https://launchpad.net/gcc-arm-embedded/+download
goto :EOF

:foundGCC

dir /O-N /B "%ARM_GCC_DIR%" > "%TMP%\dirs.txt"
set /p ARM_GCC_LATEST_DIR=< "%TMP%\dirs.txt"
set ARM_GCC_DIR=%ARM_GCC_DIR%\%ARM_GCC_LATEST_DIR%
set PATH=%ARM_GCC_DIR%\bin;%PATH%

:gccDone

nmake /nologo /? > NUL
if %ERRORLEVEL% EQU 0 goto nmakeDone

set NMAKE_DIR=%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\bin\amd64
if exist "%NMAKE_DIR%" goto foundNmake

set NMAKE_DIR=%ProgramFiles%\Microsoft Visual Studio 14.0\VC\bin
if exist "%NMAKE_DIR%" goto foundNmake

echo Please ensure that Visual Studio is installed (for nmake).
echo Please install Visual Studio from 
echo     https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx
goto :EOF

:foundNmake
set PATH=%NMAKE_DIR%;%PATH%

:nmakeDone