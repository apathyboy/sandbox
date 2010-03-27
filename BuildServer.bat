@echo off
SETLOCAL EnableDelayedExpansion

rem msvc_build.cmd
rem
rem This is the build script used for building the Combat Upgrade
rem Sandbox Server using Microsoft tools.


rem ----------------------------------------------------------------------------
rem ---Start of Main Execution -------------------------------------------------

rem Initialize environment variable defaults
call :SET_DEFAULTS

rem Process command line arguments first
goto :PROCESS_ARGUMENTS
:CONTINUE_FROM_PROCESS_ARGUMENTS

rem Build the environment and bail out if it fails
call :BUILD_ENVIRONMENT
if x%environment_built% == x goto :eof

call :BUILD_DEPENDENCIES

if not exist "%PROJECT_BASE%deps" (
  echo Missing OpenSWG dependencies!
  echo.
  echo Download the dependencies from %DEPENDENCIES_URL% and unpack it
  echo into the root project directory, then run this script again.
  exit /b 1
)

call :BUILD_PROJECT

if not %ERRORLEVEL% == 0 (
  if %HALT_ON_ERROR% == true (set /p halt=*** BUILD FAILED... PRESS ENTER TO CONTINUE ***)
  exit /b %ERRORLEVEL%
)

goto :eof
rem --- End of Main Execution --------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem ----------------------------------------------------------------------------
rem --- Helper Functions -------------------------------------------------------
rem ----------------------------------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of SET_DEFAULTS --------------------------------------------------
:SET_DEFAULTS

set DEPENDENCIES_VERSION=1.0.0
set DEPENDENCIES_FILE=cu-sandbox-deps-%DEPENDENCIES_VERSION%.zip
set DEPENDENCIES_URL=http://github.com/downloads/apathyboy/cu-sandbox/%DEPENDENCIES_FILE%
set PROJECT_BASE=%~dp0
set BUILD_TYPE=debug
set MSVC_VERSION=vc9
set REBUILD=build
set HALT_ON_ERROR=true

goto :eof
rem --- End of SET_DEFAULTS ----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of PROCESS_ARGUMENTS ---------------------------------------------
:PROCESS_ARGUMENTS

if "%0" == "" goto :CONTINUE_FROM_PROCESS_ARGUMENTS

if "%0" == "-h" (
  echo msvc_build.cmd Help
  echo.

  echo "    /rebuild                   Rebuilds the projects instead of incremental build."
  echo "    /clean                     Cleans the generated files"
  echo "    /build:[debug-release-all] Specifies the build type, defaults to debug"
  echo "    /msvc-version:[vc9]        Specifies the msvc version and project files to use"
)

if "%~0" == "/clean" (
  call :CLEAN_BUILD
)

if "%~0" == "/nohaltonerror" (
  set HALT_ON_ERROR=false
)

rem Check for /rebuild then set REBUILD
if "%~0" == "/rebuild" (
  set REBUILD=rebuild
)

rem Check for /build:x format and then set BUILD_TYPE
if "%~0" == "/build" (
  set BUILD_TYPE=%~1
  shift
)

rem Check for /msvc-version:x format and then set MSVC_VERSION
if "%~0" == "/msvc-version" (
  rem Only set if it's an allowed version
  if "%~1" == "vc9" (
    set MSVC_VERSION=%~1
  )

  shift
)

shift

goto :PROCESS_ARGUMENTS
rem --- End of PROCESS_ARGUMENTS -----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of CLEAN_BUILD ---------------------------------------------------
rem --- Cleans all output created by the build process, restoring the        ---
rem --- project to it's original state like a fresh checkout.                ---
:CLEAN_BUILD

echo Cleaning the build environment

if exist "%PROJECT_BASE%bin"  rmdir /S /Q "%PROJECT_BASE%bin"
if exist "%PROJECT_BASE%deps" rmdir /S /Q "%PROJECT_BASE%deps"
if exist "%PROJECT_BASE%lib"  rmdir /S /Q "%PROJECT_BASE%lib"
if exist "%PROJECT_BASE%obj"  rmdir /S /Q "%PROJECT_BASE%obj"

goto :eof
rem --- End of CLEAN_BUILD -----------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ENVIRONMENT ---------------------------------------------
:BUILD_ENVIRONMENT

call :BUILD_ENVIRONMENT_FOR_%MSVC_VERSION%

rem Set to devenv.exe for all versions of VS except express.
if exist "%VS_BASE_DIR%\Common7\IDE\devenv.com" (
    set "DEVENV=%VS_BASE_DIR%\Common7\IDE\devenv.com"
) else (
    set "DEVENV=%VS_BASE_DIR%\Common7\IDE\vcexpress.exe"
)

set DOTNET_BASE_DIR=%WINDIR%\Microsoft.NET\Framework\v3.5
if not exist "%DOTNET_BASE_DIR%" (
    echo ***** Microsoft .NET Framework 3.5 required *****
    exit /b 1
)

set "MSBUILD=%DOTNET_BASE_DIR%\msbuild.exe"

call "%VS_BASE_DIR%\VC\vcvarsall.bat" >NUL

set environment_built=yes

goto :eof
rem --- End of BUILD_ENVIRONMENT -----------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ENVIRONMENT_FOR_vc9 -------------------------------------
:BUILD_ENVIRONMENT_FOR_vc9

set "VS_BASE_DIR=%PROGRAMFILES(X86)%\Microsoft Visual Studio 9.0"
if not exist "!VS_BASE_DIR!" (
	  set "VS_BASE_DIR=%PROGRAMFILES%\Microsoft Visual Studio 9.0"
	  if not exist "!VS_BASE_DIR!" (
		rem TODO: Allow user to enter a path to their base visual Studio directory.

  	    echo ***** Microsoft Visual Studio 9.0 required *****
  	    exit /b 1
	  )
)

set "DOTNET_BASE_DIR=%WINDIR%\Microsoft.NET\Framework\v3.5"
if not exist "%DOTNET_BASE_DIR%" (
    echo ***** Microsoft .NET Framework 3.5 required *****
    exit /b 1
)

goto :eof
rem --- End of BUILD_ENVIRONMENT_FOR_vc9 ---------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_DEPENDENCIES --------------------------------------------
rem --- Builds all external dependencies needed by the project.              ---
:BUILD_DEPENDENCIES

echo ** Building dependencies necessary for this project **
echo.

if not exist "%PROJECT_BASE%deps" call :DOWNLOAD_DEPENDENCIES

if not exist "%PROJECT_BASE%deps\VERSION" (
  echo ** Dependencies out of date -- Updating now **
  if exist "%PROJECT_BASE%deps" rmdir /S /Q "%PROJECT_BASE%deps"
  call :DOWNLOAD_DEPENDENCIES
  echo ** Dependencies updated **
)

set /p current_version=<"%PROJECT_BASE%deps\VERSION"

if not %current_version% == %DEPENDENCIES_VERSION% (
  echo ** Dependencies out of date -- Updating now **
  if exist "%PROJECT_BASE%deps" rmdir /S /Q "%PROJECT_BASE%deps"
  call :DOWNLOAD_DEPENDENCIES
  echo ** Dependencies updated **
)

if exist "%PROJECT_BASE%deps\boost" call :BUILD_BOOST
if exist "%PROJECT_BASE%deps\gtest" call :BUILD_GTEST
if exist "%PROJECT_BASE%deps\gmock" call :BUILD_GMOCK
if exist "%PROJECT_BASE%deps\zlib"  call :BUILD_ZLIB

echo ** Building dependencies complete **

goto :eof
rem --- End of BUILD_DEPENDENCIES ----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of DOWNLOAD_DEPENDENCIES -----------------------------------------
rem --- Downloads the dependency package for the current version of the source -
:DOWNLOAD_DEPENDENCIES

cd "%PROJECT_BASE%"

if not exist "%PROJECT_BASE%%DEPENDENCIES_FILE%" (
  "%PROJECT_BASE%tools\wget.exe" !DEPENDENCIES_URL! -O "%PROJECT_BASE%%DEPENDENCIES_FILE%"
)

if exist "%PROJECT_BASE%%DEPENDENCIES_FILE%" (
  echo Extracting dependencies ...
  "%PROJECT_BASE%tools\unzip.exe" "%PROJECT_BASE%%DEPENDENCIES_FILE%" >NUL
  echo %DEPENDENCIES_VERSION% >"%PROJECT_BASE%deps\VERSION"
  echo Complete!
  echo.
)

goto :eof
rem --- End of DOWNLOAD_DEPENDENCIES -------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_BOOST ---------------------------------------------------
rem --- Builds the boost library for use with this project.                  ---
:BUILD_BOOST

echo BUILDING: Boost - http://www.boost.org/

rem Only build boost if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
  if exist "%PROJECT_BASE%deps\boost\stage\lib\libboost_*-mt-sgd.lib" (
    echo Boost libraries already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
  if exist "%PROJECT_BASE%deps\boost\stage\lib\libboost_*-mt-s.lib" (
	echo Boost libraries already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "all" (
  if exist "%PROJECT_BASE%deps\boost\stage\lib\libboost_*-mt-sgd.lib" (
   	if exist "%PROJECT_BASE%deps\boost\stage\lib\libboost_*-mt-s.lib" (
      echo Boost libraries already built ... skipping
      echo.
      goto :eof
    )
  )
)

rem Build BJAM which is needed to build boost.
if not exist "%PROJECT_BASE%deps\boost\tools\jam\src\bin.ntx86\bjam.exe" (
  cd "%PROJECT_BASE%deps\boost\tools\jam\src"
  cmd /q /c build.bat >NUL 2>&1
)

rem Build the boost libraries we need.
cd "%PROJECT_BASE%deps\boost"

if "%BUILD_TYPE%" == "debug" (
    cmd /c "%PROJECT_BASE%deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=debug link=static runtime-link=static threading=multi >NULL
)

if "%BUILD_TYPE%" == "release" (
    cmd /c "%PROJECT_BASE%deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=release link=static runtime-link=static threading=multi >NULL
)

if "%BUILD_TYPE%" == "all" (
    cmd /c "%PROJECT_BASE%deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=debug,release link=static runtime-link=static threading=multi >NULL
)

goto :eof
rem --- End of BUILD_BOOST -----------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GTEST ---------------------------------------------------
rem --- Builds all googletest library used for unit testing.                 ---
:BUILD_GTEST

echo BUILDING: Google Test - http://code.google.com/p/googletest/

rem Only build gtest if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
  if exist "%PROJECT_BASE%deps\gtest\msvc\debug\gtestd.lib" (
    echo Google Test library already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
  if exist "%PROJECT_BASE%deps\gtest\msvc\release\gtest.lib" (
    echo Google Test library already built ... skipping
    echo.
    goto :eof
  )
)

if "%BUILD_TYPE%" == "all" (
  if exist "%PROJECT_BASE%deps\gtest\msvc\debug\gtestd.lib" (
    if exist "%PROJECT_BASE%deps\gtest\msvc\release\gtest.lib" (
      echo Google Test library already built ... skipping
      echo.
      goto :eof
    )
  )
)

cd "%PROJECT_BASE%deps\gtest"

if exist "%PROJECT_BASE%deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gtest\msvc\*.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gtest\msvc\gtest.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gtest\msvc\*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gtest\msvc\gtest.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gtest\msvc\*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gtest\msvc\gtest.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gtest\msvc\*.cache" >NUL

  "%MSBUILD%" "%PROJECT_BASE%deps\gtest\msvc\gtest.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gtest\msvc\*.cache" >NUL
)
goto :eof
rem --- End of BUILD_GTEST -----------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GMOCK ---------------------------------------------------
rem --- Builds all googlemock library used for unit testing.                 ---
:BUILD_GMOCK

echo BUILDING: Google Mock - http://code.google.com/p/googlemock/

rem Only build mock if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
  if exist "%PROJECT_BASE%deps\gmock\msvc\debug\gmock.lib" (
    echo Google Mock library already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
  if exist "%PROJECT_BASE%deps\gmock\msvc\release\gmock.lib" (
    echo Google Mock library already built ... skipping
    echo.
    goto :eof
  )
)

if "%BUILD_TYPE%" == "all" (
  if exist "%PROJECT_BASE%deps\gmock\msvc\debug\gmock.lib" (
    if exist "%PROJECT_BASE%deps\gmock\msvc\release\gmock.lib" (
      echo Google Mock library already built ... skipping
      echo.
      goto :eof
	)
  )
)

cd "%PROJECT_BASE%deps\gmock"

if exist "%PROJECT_BASE%deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gmock\msvc\*.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gmock\msvc\gmock.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gmock\msvc\*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gmock\msvc\gmock.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gmock\msvc\*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
  "%MSBUILD%" "%PROJECT_BASE%deps\gmock\msvc\gmock.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gmock\msvc\*.cache" >NUL

  "%MSBUILD%" "%PROJECT_BASE%deps\gmock\msvc\gmock.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%deps\gmock\msvc\*.cache" >NUL
)
goto :eof
rem --- End of BUILD_GMOCK -----------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ZLIB ----------------------------------------------------
rem --- Builds the zlib library for use with this project.                   ---
:BUILD_ZLIB

echo BUILDING: zlib - http://www.zlib.net/

rem Only build zlib if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
  	echo zlib library already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib" (
    echo zlib library already built ... skipping
    echo.
    goto :eof
  )
)
if "%BUILD_TYPE%" == "all" (
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
    if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib" (
      echo zlib library already built ... skipping
      echo.
      goto :eof
    )
  )
)

rem Build the zlib library.
cd "%PROJECT_BASE%deps\zlib"


rem VS likes to create these .cache files and then complain about them existing afterwards.
rem Removing it as it's not needed.
if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
  "%MSBUILD%" "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln" /t:%REBUILD% /p:Platform=Win32,Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
  "%MSBUILD%" "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln" /t:%REBUILD% /p:Platform=Win32,Configuration="LIB Release",VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
  "%MSBUILD%" "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln" /t:%REBUILD% /p:Platform=Win32,Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" >NUL

  "%MSBUILD%" "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln" /t:%REBUILD% /p:Platform=Win32,Configuration="LIB Release",VCBuildAdditionalOptions="/useenv"
  if exist "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%deps\zlib\projects\visualc6\zlib.sln.cache" >NUL
)
goto :eof
rem --- End of BUILD_ZLIB ------------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_PROJECT -------------------------------------------------
rem --- Builds the actual project.                                           ---
:BUILD_PROJECT

cd "%PROJECT_BASE%"

if exist "%PROJECT_BASE%*.cache" del /S /Q "%PROJECT_BASE%*.cache" >NUL
if exist "%PROJECT_BASE%build-aux\sandbox_%MSVC_VERSION%-tests.xml" del /S /Q "%PROJECT_BASE%build-aux\sandbox_%MSVC_VERSION%-tests.xml" >NUL

if "%BUILD_TYPE%" == "debug" (
    "%MSBUILD%" "%PROJECT_BASE%sandbox_%MSVC_VERSION%.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
    if errorlevel 1 exit /b 1
    if exist "%PROJECT_BASE%*.cache" del /S /Q "%PROJECT_BASE%*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
    "%MSBUILD%" "%PROJECT_BASE%sandbox_%MSVC_VERSION%.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
    if errorlevel 1 exit /b 1
    if exist "%PROJECT_BASE%*.cache" del /S /Q "%PROJECT_BASE%*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
    "%MSBUILD%" "%PROJECT_BASE%sandbox_%MSVC_VERSION%.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
    if errorlevel 1 exit /b 1
    if exist "%PROJECT_BASE%*.cache" del /S /Q "%PROJECT_BASE%*.cache" >NUL

    "%MSBUILD%" "%PROJECT_BASE%sandbox_%MSVC_VERSION%.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
    if errorlevel 1 exit /b 1
    if exist "%PROJECT_BASE%*.cache" del /S /Q "%PROJECT_BASE%*.cache" >NUL
)

goto :eof
rem --- End of BUILD_PROJECT ---------------------------------------------------
rem ----------------------------------------------------------------------------

ENDLOCAL