@echo off
setlocal enabledelayedexpansion

set "PF=%ProgramFiles%"
set "PF86=%ProgramFiles(x86)%"
where cl >nul 2>nul
if errorlevel 1 (
  set "VCVARSALL="
  if defined READERVIEW0_VCVARS set "VCVARSALL=%READERVIEW0_VCVARS%"
  if not defined VCVARSALL (
    set "VSWHERE=!PF86!\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "!VSWHERE!" (
      for /f "usebackq delims=" %%I in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSROOT=%%I"
      if defined VSROOT set "VCVARSALL=!VSROOT!\VC\Auxiliary\Build\vcvarsall.bat"
    )
  )
  if not defined VCVARSALL if exist "!PF!\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=!PF!\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
  if not defined VCVARSALL if exist "!PF!\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=!PF!\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
  if not defined VCVARSALL (
    echo [win32_build] Failed to locate vcvarsall.bat.
    exit /b 1
  )
  call "!VCVARSALL!" x64
  if errorlevel 1 exit /b 1
)

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT=%%~fI"
if not defined READERVIEW0_UI0_DIR for %%I in ("%ROOT%\..\ui0") do set "READERVIEW0_UI0_DIR=%%~fI"
if not defined READERVIEW0_ZERO_FOUNDATION_DIR if defined UI0_ZERO_FOUNDATION_DIR set "READERVIEW0_ZERO_FOUNDATION_DIR=%UI0_ZERO_FOUNDATION_DIR%"
if not defined READERVIEW0_ZERO_FOUNDATION_DIR for %%I in ("%READERVIEW0_UI0_DIR%\..\zero_foundation") do set "READERVIEW0_ZERO_FOUNDATION_DIR=%%~fI"

if not exist "%READERVIEW0_UI0_DIR%\code\ui0.c" (
  echo [win32_build] Missing UI0 at "%READERVIEW0_UI0_DIR%".
  exit /b 1
)
if not exist "%READERVIEW0_ZERO_FOUNDATION_DIR%\code\base\base_unicode.c" (
  echo [win32_build] Missing zero_foundation at "%READERVIEW0_ZERO_FOUNDATION_DIR%".
  exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\scripts\require_ui0_dependency_current.ps1"
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\scripts\audit_architecture.ps1"
if errorlevel 1 exit /b 1

set "OUT_DIR=%ROOT%\build\win32"
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"
pushd "%OUT_DIR%"
cl /nologo /std:c11 /W4 /WX /Zi /FS /Od /MD /D_CRT_SECURE_NO_WARNINGS ^
  /I "%ROOT%\code" ^
  /I "%READERVIEW0_UI0_DIR%\code" ^
  /I "%READERVIEW0_ZERO_FOUNDATION_DIR%\code" ^
  /Fe"readerview0_tests.exe" ^
  "%READERVIEW0_ZERO_FOUNDATION_DIR%\code\base\base_unicode.c" ^
  "%READERVIEW0_ZERO_FOUNDATION_DIR%\code\base\base_text_edit.c" ^
  "%READERVIEW0_ZERO_FOUNDATION_DIR%\code\base\base_text_history.c" ^
  "%READERVIEW0_UI0_DIR%\code\ui0.c" ^
  "%ROOT%\code\readerview0.c" ^
  "%ROOT%\code\tests\readerview0_tests_main.c"
if errorlevel 1 (
  popd
  exit /b 1
)
popd
if "%1"=="no_run" exit /b 0
pushd "%ROOT%"
call "%OUT_DIR%\readerview0_tests.exe"
set "EXIT_CODE=%ERRORLEVEL%"
popd
endlocal & exit /b %EXIT_CODE%
