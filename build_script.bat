@echo off
setlocal EnableExtensions

cd /d "%~dp0"
for %%I in ("%~dp0.") do set "PROJECT=%%~nxI"
set "FLASH_ADDRESS=0x08020000"
set "STM32_DEVICE=STM32H734VI"
set "STM32_PORT=SWD"
set "STM32_PROGRAMMER_CLI=STM32_Programmer_CLI.exe"
set "DFU_UTIL=dfu-util.exe"
set "ARM_OBJCOPY=arm-none-eabi-objcopy.exe"
set "DFU_DEVICE=0483:52A4"
set "DFU_ALT=0"
set "DFU_ADDRESS=0x08020000"

if "%~1"=="" goto :help
if /I "%~1"=="help" goto :help
if /I "%~1"=="bootloader" goto :command_bootloader
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

:command_bootloader
if not exist "%~dp0bootloader\build_script.bat" (
    echo Khong tim thay bootloader\build_script.bat.
    exit /b 1
)
pushd "%~dp0bootloader"
if errorlevel 1 (
    echo Khong the truy cap thu muc bootloader.
    exit /b 1
)
if "%~2"=="" (
    call build_script.bat debug
) else if /I "%~2"=="upload" (
    call build_script.bat debug upload
) else (
    echo Usage: %~nx0 bootloader ^[upload^]
    popd
    exit /b 2
)
set "BOOTLOADER_ERRORLEVEL=%errorlevel%"
popd
exit /b %BOOTLOADER_ERRORLEVEL%

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
echo   bootloader [upload] Build bootloader; upload flashes existing Debug ELF without rebuilding
echo   debug [upload^|dfu] Build Debug, optionally flash it via ST-Link or USB
echo   reldgb [upload^|dfu] Build RelWithDebInfo, optionally flash/update it
echo   release [upload^|dfu] Build Release, optionally flash/update it
echo   minsize [upload^|dfu] Build MinSizeRel, optionally flash/update it
echo   reset             Reset MCU through ST-Link
echo   clean    Remove the build directory
echo   help     Show this help
echo.
echo Examples:
echo   %~nx0 bootloader
echo   %~nx0 bootloader upload
echo   %~nx0 debug
echo   %~nx0 debug upload
echo   %~nx0 debug dfu
echo   %~nx0 release upload
echo   %~nx0 release dfu
echo   %~nx0 reset
exit /b 0

:run_preset
set "PRESET=%~1"
if "%~2"=="" goto :run_build
if /I "%~2"=="upload" goto :run_upload
if /I "%~2"=="dfu" goto :run_dfu
goto :invalid_run_preset

:run_upload
call :flash "%PRESET%"
exit /b %errorlevel%

:run_dfu
call :dfu "%PRESET%"
exit /b %errorlevel%

:run_build
call :build "%PRESET%"
exit /b %errorlevel%

:invalid_run_preset
echo Usage: %~nx0 %~1 ^[upload^|dfu^]
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
cmake --build --preset "%PRESET%" -j32
if errorlevel 1 exit /b 1
echo.
echo Build thanh cong: build\%PRESET%\%PROJECT%.elf
exit /b 0

:dfu
set "PRESET=%~1"

where "%DFU_UTIL%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %DFU_UTIL% trong PATH.
    echo Cai dfu-util va them thu muc chua dfu-util.exe vao PATH.
    exit /b 1
)

where "%ARM_OBJCOPY%" >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay %ARM_OBJCOPY% trong PATH.
    echo Can GNU Arm Embedded Tools de tao file BIN.
    exit /b 1
)

set "ELF=build\%PRESET%\%PROJECT%.elf"
if not exist "%ELF%" (
    echo Khong tim thay ELF sau khi build: %ELF%
    echo Hay build truoc bang: %~nx0 %PRESET%
    exit /b 1
)

set "BIN=build\%PRESET%\%PROJECT%.bin"
echo.
echo [Convert] %ELF% ^> %BIN%
"%ARM_OBJCOPY%" -O binary "%ELF%" "%BIN%"
if errorlevel 1 (
    echo Tao BIN that bai.
    exit /b 1
)

echo.
echo [DFU] Nap runtime DFU qua dfu-util
echo [DFU] Thiet bi: %DFU_DEVICE%, alternate interface: %DFU_ALT%
echo [DFU] Dia chi staging: %DFU_ADDRESS%
"%DFU_UTIL%" -d "%DFU_DEVICE%" -a "%DFU_ALT%" -D "%BIN%" -s "%DFU_ADDRESS%:leave"
if errorlevel 1 (
    echo Runtime DFU that bai.
    exit /b 1
)
echo Runtime DFU thanh cong.
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
    echo Hay build truoc bang: %~nx0 %PRESET%
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