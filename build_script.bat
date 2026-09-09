@echo off
setlocal EnableExtensions

cd /d "%~dp0"
for %%I in ("%~dp0.") do set "PROJECT=%%~nxI"
set "FLASH_ADDRESS=0x08000000"
set "STM32_DEVICE=STM32F427VI"
set "STM32_PORT=SWD"
set "STM32_PROGRAMMER_CLI=STM32_Programmer_CLI.exe"

if "%~1"=="" goto :help
if /I "%~1"=="help" goto :help
if /I "%~1"=="debug" goto :command_debug
if /I "%~1"=="reldgb" goto :command_reldgb
if /I "%~1"=="release" goto :command_release
if /I "%~1"=="minsize" goto :command_minsize
if /I "%~1"=="reset" goto :command_reset
if /I "%~1"=="clean" goto :command_clean

echo Unknown command: %~1
echo.
call :help
exit /b 2

:command_debug
call :run_preset Debug "%~2"
exit /b %errorlevel%

:command_reldgb
call :run_preset RelWithDebInfo "%~2"
exit /b %errorlevel%

:command_release
call :run_preset Release "%~2"
exit /b %errorlevel%

:command_minsize
call :run_preset MinSizeRel "%~2"
exit /b %errorlevel%

:command_reset
call :reset
exit /b %errorlevel%

:command_clean
call :clean
exit /b %errorlevel%

:help
echo Usage: %~nx0 ^<command^>
echo.
echo Commands:
echo   debug [upload]    Build Debug, optionally flash it
echo   reldgb [upload]   Build RelWithDebInfo, optionally flash it
echo   release [upload] Build Release, optionally flash it
echo   minsize [upload] Build MinSizeRel, optionally flash it
echo   reset             Reset MCU through ST-Link
echo   clean    Remove the build directory
echo   help     Show this help
echo.
echo Examples:
echo   %~nx0 debug
echo   %~nx0 debug upload
echo   %~nx0 release upload
echo   %~nx0 reset
exit /b 0

:run_preset
set "PRESET=%~1"
if "%~2"=="" goto :run_build
if /I not "%~2"=="upload" goto :invalid_run_preset
call :flash "%PRESET%"
exit /b %errorlevel%

:run_build
call :build "%PRESET%"
exit /b %errorlevel%

:invalid_run_preset
echo Usage: %~nx0 %~1 ^[upload^]
exit /b 2

:reset
where "%STM32_PROGRAMMER_CLI%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %STM32_PROGRAMMER_CLI% trong PATH.
    echo Dat bien STM32_PROGRAMMER_CLI neu cong cu nam o duong dan khac.
    exit /b 1
)

echo.
echo [Reset] MCU qua ST-Link
"%STM32_PROGRAMMER_CLI%" -c port=%STM32_PORT% -rst
if errorlevel 1 (
    echo Reset that bai.
    exit /b 1
)
echo Reset thanh cong.
exit /b 0

:build
set "PRESET=%~1"
set "ELF=build\%PRESET%\%PROJECT%.elf"
if exist "%ELF%" (
    echo.
    echo [Remove old ELF] %ELF%
    del /q "%ELF%"
    if exist "%ELF%" (
        echo Khong the xoa ELF cu.
        exit /b 1
    )
)
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
set "PRESET=%~1"

where "%STM32_PROGRAMMER_CLI%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %STM32_PROGRAMMER_CLI% trong PATH.
    echo Dat bien STM32_PROGRAMMER_CLI neu cong cu nam o duong dan khac.
    exit /b 1
)

set "ELF=build\%PRESET%\%PROJECT%.elf"
if not exist "%ELF%" (
    echo Khong tim thay ELF sau khi build: %ELF%
    exit /b 1
)
echo.
echo [Flash] %ELF%
"%STM32_PROGRAMMER_CLI%" -c port=%STM32_PORT% -d "%ELF%" %FLASH_ADDRESS% -v -rst
if errorlevel 1 (
    echo Nap that bai.
    exit /b 1
)
echo Nap thanh cong.
exit /b 0

:clean
echo Xoa cac thu muc build...
if exist build rmdir /s /q build
echo Da xoa.
exit /b 0