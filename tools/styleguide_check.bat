@echo off

rem The base path is one directory above this script.
call :BASE_PATH %~dp0..\

if not exist %1 (
  echo.
  echo Invalid path passed to this script: %1
  echo.
  exit /b 1  
)

for %%i in (%1\*.*) do (
  if %%~xi == .h   call :CHECK_FILE %%i
  if %%~xi == .hpp call :CHECK_FILE %%i
  if %%~xi == .cc  call :CHECK_FILE %%i
  if %%~xi == .cpp call :CHECK_FILE %%i
)

goto :eof

rem -------------------------------- HELPER FUNCTIONS --------------------------

rem -------- :BASE_PATH ---------
:BASE_PATH

set "PROJECT_ROOT=%~dp1"

goto :eof
rem -----------------------------

rem -------- :CHECK_FILE --------
:CHECK_FILE

%PROJECT_ROOT%tools\cpplint.py --verbose=0 --output=vs7 --filter=-runtime/references,-readability/streams %1

rem Bail out if any errors are detected
if errorlevel 1 exit 1

goto :eof
rem -----------------------------

$(SolutionDir)tools\styleguide_check.bat $(ProjectDir)