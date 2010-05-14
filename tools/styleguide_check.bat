@echo off

rem The base path is one directory above this script.
call :SET_DEFAULTS %~dp0..\ %1 %2 %3

cd %PROJECT_ROOT%

if not exist %1 (
  echo.
  echo Invalid path passed to this script: %1
  echo.
  exit /b 1  
)

call :FULL_CHECK %1

goto :eof

rem -------------------------------- HELPER FUNCTIONS --------------------------

rem -------- :SET_DEFAULTS ----------
:SET_DEFAULTS

set "PROJECT_ROOT=%~dp1"
set "PROJECT_NAME=%3"
set "PROJECT_CONFIGURATION=%4"

goto :eof
rem ------------------------------

rem -------- :CHECK_FILE ---------
:CHECK_FILE

for /F %%i In ('xcopy /DHYL %1 "%PROJECT_ROOT%build-aux\%PROJECT_NAME%\%PROJECT_CONFIGURATION%\CL.write.1.tlog" ^|Findstr /I "File"') Do set /a _Newer=%%i

if %_Newer% == 1 (
  %PROJECT_ROOT%tools\cpplint.py --verbose=0 --output=vs7 --filter=-runtime/references,-readability/streams %1
)

rem Bail out if any errors are detected
if errorlevel 1 exit 1

goto :eof
rem ------------------------------


rem -------- :FULL_CHECK ---------
:FULL_CHECK

for %%i in (%1*.*) do (
  if %%~xi == .h   call :CHECK_FILE %%i
  if %%~xi == .hpp call :CHECK_FILE %%i
  if %%~xi == .cc  call :CHECK_FILE %%i
  if %%~xi == .cpp call :CHECK_FILE %%i
)

goto :eof
rem ------------------------------
