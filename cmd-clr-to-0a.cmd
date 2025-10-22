@echo off
setlocal enabledelayedexpansion

REM Script to permanently set PowerShell and CMD colors to green on black (0a)
REM This script modifies registry settings to make changes persistent

echo =====================================
echo Setting Console Colors to Green on Black
echo =====================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: This script is not running as Administrator.
    echo Some registry changes may fail. Consider running as Administrator for full effect.
    echo.
    pause
)

REM Set color for current session
color 0a
echo Current session color changed to green on black.
echo.

REM Registry paths for console settings
set "CMD_REG_PATH=HKCU\Console\%%SystemRoot%%_system32_cmd.exe"
set "POWERSHELL_REG_PATH=HKCU\Console\%%SystemRoot%%_system32_WindowsPowerShell_v1.0_powershell.exe"
set "POWERSHELL_ISE_REG_PATH=HKCU\Console\%%SystemRoot%%_system32_WindowsPowerShell_v1.0_PowerShell_ISE.exe"
set "GLOBAL_CONSOLE_REG_PATH=HKCU\Console"

REM Color values (DWORD): 0x0000000a = green text on black background
set "SCREEN_COLORS=0x0000000a"
set "POPUP_COLORS=0x000000f0"

echo Applying registry changes...
echo.

REM Set default console colors globally
echo Setting global console defaults...
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "ScreenColors" /t REG_DWORD /d %SCREEN_COLORS% /f >nul 2>&1
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "PopupColors" /t REG_DWORD /d %POPUP_COLORS% /f >nul 2>&1

REM Set CMD specific colors
echo Setting Command Prompt colors...
reg add "%CMD_REG_PATH%" /v "ScreenColors" /t REG_DWORD /d %SCREEN_COLORS% /f >nul 2>&1
reg add "%CMD_REG_PATH%" /v "PopupColors" /t REG_DWORD /d %POPUP_COLORS% /f >nul 2>&1

REM Set PowerShell specific colors
echo Setting PowerShell colors...
reg add "%POWERSHELL_REG_PATH%" /v "ScreenColors" /t REG_DWORD /d %SCREEN_COLORS% /f >nul 2>&1
reg add "%POWERSHELL_REG_PATH%" /v "PopupColors" /t REG_DWORD /d %POPUP_COLORS% /f >nul 2>&1

REM Set PowerShell ISE colors
echo Setting PowerShell ISE colors...
reg add "%POWERSHELL_ISE_REG_PATH%" /v "ScreenColors" /t REG_DWORD /d %SCREEN_COLORS% /f >nul 2>&1
reg add "%POWERSHELL_ISE_REG_PATH%" /v "PopupColors" /t REG_DWORD /d %POPUP_COLORS% /f >nul 2>&1

REM Additional settings for better console experience
echo Setting additional console properties...

REM Set font and window properties for better experience
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "FaceName" /t REG_SZ /d "Consolas" /f >nul 2>&1
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "FontFamily" /t REG_DWORD /d 0x00000036 /f >nul 2>&1
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "FontSize" /t REG_DWORD /d 0x000E0000 /f >nul 2>&1
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "FontWeight" /t REG_DWORD /d 0x00000190 /f >nul 2>&1
reg add "%GLOBAL_CONSOLE_REG_PATH%" /v "WindowAlpha" /t REG_DWORD /d 0x000000E6 /f >nul 2>&1

REM Apply same font settings to specific consoles
for %%i in ("%CMD_REG_PATH%" "%POWERSHELL_REG_PATH%" "%POWERSHELL_ISE_REG_PATH%") do (
    reg add %%i /v "FaceName" /t REG_SZ /d "Consolas" /f >nul 2>&1
    reg add %%i /v "FontFamily" /t REG_DWORD /d 0x00000036 /f >nul 2>&1
    reg add %%i /v "FontSize" /t REG_DWORD /d 0x000E0000 /f >nul 2>&1
    reg add %%i /v "FontWeight" /t REG_DWORD /d 0x00000190 /f >nul 2>&1
    reg add %%i /v "WindowAlpha" /t REG_DWORD /d 0x000000E6 /f >nul 2>&1
)

REM Handle Administrator consoles by setting machine-wide defaults
REM These will affect "Run as Administrator" instances
echo Setting Administrator console colors...

REM Try to set HKLM entries (requires admin privileges)
reg add "HKLM\Console" /v "ScreenColors" /t REG_DWORD /d %SCREEN_COLORS% /f >nul 2>&1
reg add "HKLM\Console" /v "PopupColors" /t REG_DWORD /d %POPUP_COLORS% /f >nul 2>&1

if %errorlevel% equ 0 (
    echo Administrator console colors set successfully.
) else (
    echo Note: Could not set Administrator console colors - requires Administrator privileges.
)

echo.
echo =====================================
echo Registry changes completed!
echo =====================================
echo.
echo Changes applied:
echo - Global console colors: Green on Black (0a)
echo - Command Prompt colors: Green on Black (0a)  
echo - PowerShell colors: Green on Black (0a)
echo - PowerShell ISE colors: Green on Black (0a)
echo - Font set to Consolas for better readability
echo.
echo IMPORTANT NOTES:
echo - Current session already shows new colors
echo - New CMD/PowerShell windows will use green on black
echo - Changes are permanent and will persist after reboot
echo - To reset colors, run: reg delete "HKCU\Console" /v "ScreenColors" /f
echo.
echo For "Run as Administrator" windows to show new colors,
echo this script should be run as Administrator.
echo.
pause
