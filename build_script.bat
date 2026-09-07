@echo off
setlocal EnableExtensions

cd /d "%~dp0"
set "PROJECT=VisioF7"
set "FLASH_ADDRESS=0x08000000"
set "STM32_DEVICE=STM32F767VI"
set "STM32_PORT=SWD"
set "GDB=arm-none-eabi-gdb"
set "OPENOCD=openocd"
set "STM32_PROGRAMMER_CLI=STM32_Programmer_CLI.exe"
set "DEBUG_PRESET=Debug"

if "%~1"=="" goto :help
if /I "%~1"=="help" goto :help
if /I "%~1"=="debug" goto :command_debug
if /I "%~1"=="reldgb" goto :command_reldgb
if /I "%~1"=="release" goto :command_release
if /I "%~1"=="minsize" goto :command_minsize
if /I "%~1"=="upload" goto :command_upload
if /I "%~1"=="gdb" goto :command_gdb
if /I "%~1"=="clean" goto :command_clean

echo Unknown command: %~1
echo.
call :help
exit /b 2

:command_debug
call :build Debug
exit /b %errorlevel%

:command_reldgb
call :build RelWithDebInfo
exit /b %errorlevel%

:command_release
call :build Release
exit /b %errorlevel%

:command_minsize
call :build MinSizeRel
exit /b %errorlevel%

:command_upload
call :flash
exit /b %errorlevel%

:command_gdb
call :debug
exit /b %errorlevel%

:command_clean
call :clean
exit /b %errorlevel%

:help
echo Usage: %~nx0 ^<command^>
echo.
echo Commands:
echo   debug    Build Debug
echo   reldgb   Build RelWithDebInfo
echo   release  Build Release
echo   minsize  Build MinSizeRel
echo   upload   Build Debug and flash STM32 through ST-Link
echo   gdb      Build Debug and start OpenOCD + GDB
echo   clean    Remove the build directory
echo   help     Show this help
echo.
echo Examples:
echo   %~nx0 debug
echo   %~nx0 upload
exit /b 0

:build
set "PRESET=%~1"
echo.
echo [Configure] %PRESET%
cmake --preset "%PRESET%"
if errorlevel 1 exit /b 1
echo.
echo [Build] %PRESET%
cmake --build --preset "%PRESET%"
if errorlevel 1 exit /b 1
echo.
echo Build thanh cong: build\%PRESET%\%PROJECT%.elf
exit /b 0

:flash
call :build "%DEBUG_PRESET%"
if errorlevel 1 exit /b 1

where "%STM32_PROGRAMMER_CLI%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %STM32_PROGRAMMER_CLI% trong PATH.
    echo Dat bien STM32_PROGRAMMER_CLI neu cong cu nam o duong dan khac.
    exit /b 1
)

set "ELF=build\%DEBUG_PRESET%\%PROJECT%.elf"
echo.
echo [Flash] %ELF%
"%STM32_PROGRAMMER_CLI%" -c port=%STM32_PORT% -d "%ELF%" %FLASH_ADDRESS% -v -rst
if errorlevel 1 (
    echo Nap that bai.
    exit /b 1
)
echo Nap thanh cong.
exit /b 0

:debug
call :build "%DEBUG_PRESET%"
if errorlevel 1 exit /b 1

where "%OPENOCD%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay OpenOCD trong PATH.
    exit /b 1
)
where "%GDB%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %GDB% trong PATH.
    exit /b 1
)

set "ELF=build\%DEBUG_PRESET%\%PROJECT%.elf"
echo.
echo Khoi dong OpenOCD ST-Link tren cong 3333...
start "OpenOCD - ST-Link" cmd /k "%OPENOCD%" -f interface/stlink.cfg -f target/stm32f7x.cfg
timeout /t 2 /nobreak >nul
echo Ket noi GDB. Lenh 'continue' se chay chuong trinh.
"%GDB%" "%ELF%" -ex "target extended-remote localhost:3333" -ex "monitor reset halt" -ex "load" -ex "break main" -ex "continue"
exit /b 0

:clean
echo Xoa cac thu muc build...
if exist build rmdir /s /q build
echo Da xoa.
exit /b 0