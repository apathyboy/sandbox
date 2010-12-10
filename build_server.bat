@echo off
SETLOCAL EnableDelayedExpansion

rem BuildServer.bat
rem
rem This is the build script used for building this project and it's 
rem dependencies using Microsoft tools.

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

%PROJECT_DRIVE%
cd %PROJECT_BASE%

if %SKIPHEIGHTMAPS% == false (
call :DOWNLOAD_DATA_FILES
)

call :BUILD_DEPENDENCIES

if not exist "deps" (
	echo Missing SWGANH dependencies!
	echo.
	echo Download the dependencies from %DEPENDENCIES_URL% and unpack it
	echo into the root project directory, then run this script again.
	exit /b 1
)

if %DEPENDENCIESONLY% == true goto :eof

call :BUILD_PROJECT

if not %ERRORLEVEL% == 0 (
if %HALT_ON_ERROR% == true (set /p halt=*** BUILD FAILED... PRESS ENTER TO CONTINUE ***)
exit /b %ERRORLEVEL%
)

echo.
echo Server Successfully Built^^!

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

set DEPENDENCIES_VERSION=3.0.0
set DEPENDENCIES_FILE=sandbox-deps-%DEPENDENCIES_VERSION%-win.7z
set DEPENDENCIES_URL=http://github.com/downloads/apathyboy/sandbox/%DEPENDENCIES_FILE%
set "PROJECT_BASE=%~dp0"
set "PROJECT_DRIVE=%~d0"
set BUILD_TYPE=debug
set REBUILD=build
set MSVC_VERSION=10
set ALLHEIGHTMAPS=false
set SKIPHEIGHTMAPS=false
set DEPENDENCIESONLY=false
set BUILDNUMBER=0
set BUILD_ERROR=false
set HALT_ON_ERROR=true
set halt=

goto :eof
rem --- End of SET_DEFAULTS ----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of PROCESS_ARGUMENTS ---------------------------------------------
:PROCESS_ARGUMENTS

if "%~0" == "" goto :CONTINUE_FROM_PROCESS_ARGUMENTS


if "%~0" == "-h" (
	echo msvc_build.cmd Help
	echo.
	echo "    /builddeps                     Builds only the project dependencies"
	echo "    /nohaltonerror                 Skips halting on errors"
	echo "    /rebuild                       Rebuilds the projects instead of incremental build"
	echo "    /clean                         Cleans the generated files"
	echo "    /build [debug-release-all]     Specifies the build type, defaults to debug"
	echo "    /buildnumber [num]             Specifies a build number to be set rather than commit hash"
)

if "%~0" == "/clean" (
	call :CLEAN_BUILD
	goto :eof
)

if "%~0" == "/builddeps" (
	set DEPENDENCIESONLY=true
)

if "%~0" == "/nohaltonerror" (
	set HALT_ON_ERROR=false
)

rem Check for /rebuild then set REBUILD
if "%~0" == "/rebuild" (
	set REBUILD=rebuild
)


rem Check for /buildnumber x format and then set BUILDNUMBER
if "%~0" == "/buildnumber" (
	set BUILDNUMBER=%~1
	shift
)

rem Check for /build:x format and then set BUILD_TYPE
if "%~0" == "/build" (
set BUILD_TYPE=%~1
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

if exist "deps" rmdir /S /Q "deps"
if exist "build" rmdir /S /Q "build"

goto :eof
rem --- End of CLEAN_BUILD -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ENVIRONMENT ---------------------------------------------
:BUILD_ENVIRONMENT

if not exist "%VS100COMNTOOLS%" (
  set "VS100COMNTOOLS=%PROGRAMFILES(X86)%\Microsoft Visual Studio 10.0\Common7\Tools"
  if not exist "!VS100COMNTOOLS!" (
  	  set "VS100COMNTOOLS=%PROGRAMFILES%\Microsoft Visual Studio 10.0\Common7\Tools"
  	  if not exist "!VS100COMNTOOLS!" (          
  		    rem TODO: Allow user to enter a path to their base visual Studio directory.
         
    	    echo ***** Microsoft Visual Studio 10.0 required *****
    	    exit /b 1
  	  )
  )
)

set "MSBUILD=%WINDIR%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe"

call "%VS100COMNTOOLS%\vsvars32.bat" >NUL

set environment_built=yes

goto :eof
rem --- End of BUILD_ENVIRONMENT -----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_DEPENDENCIES --------------------------------------------
rem --- Builds all external dependencies needed by the project.              ---
:BUILD_DEPENDENCIES

echo ** Building dependencies necessary for this project **
echo.

if not exist "deps" call :DOWNLOAD_DEPENDENCIES

if not exist "deps\VERSION" (
	echo ** Dependencies out of date -- Updating now **
	call :CLEAN_BUILD
	call :DOWNLOAD_DEPENDENCIES
	echo ** Dependencies updated **
)

set /p current_version=<"deps\VERSION"

if not %current_version% == %DEPENDENCIES_VERSION% (
	echo ** Dependencies out of date -- Updating now **

	rem Need to do a full rebuild after updating dependenceies
	set REBUILD=rebuild

	call :CLEAN_BUILD
	call :DOWNLOAD_DEPENDENCIES
	echo ** Dependencies updated **
)

if exist "deps\boost" call :BUILD_BOOST
if exist "deps\glog" call :BUILD_GLOG
if exist "deps\gtest" call :BUILD_GTEST
if exist "deps\mysql-connector-cpp" call :BUILD_MYSQLCONN
if exist "deps\zlib" call :BUILD_ZLIB

echo ** Building dependencies complete **

goto :eof
rem --- End of BUILD_DEPENDENCIES ----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of DOWNLOAD_DEPENDENCIES -----------------------------------------
rem --- Downloads the dependency package for the current version of the source -
:DOWNLOAD_DEPENDENCIES

if not exist "%DEPENDENCIES_FILE%" (
	"tools\windows\wget.exe" --no-check-certificate !DEPENDENCIES_URL! -O "%DEPENDENCIES_FILE%"
)

if exist "%DEPENDENCIES_FILE%" (
	echo Extracting dependencies ...

	"tools\windows\7z.exe" x -y "%DEPENDENCIES_FILE%"
	echo %DEPENDENCIES_VERSION% >"deps\VERSION"
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

cd "%PROJECT_BASE%deps\boost"

rem Only build boost if it hasn't been built already.
if exist "stage\lib\libboost_*-mt-gd.lib" (
	if exist "stage\lib\libboost_*-mt.lib" (
		echo Boost libraries already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

rem Build BJAM which is needed to build boost.
if not exist "tools\jam\src\bin.ntx86\bjam.exe" (
	cd "tools\jam\src"
	cmd /q /c build.bat
	cd "%PROJECT_BASE%deps\boost"
)

rem Build the boost libraries we need.

cmd /c "tools\jam\src\bin.ntx86\bjam.exe" --toolset=msvc-%MSVC_VERSION%.0 --with-date_time --with-thread --with-regex --with-program_options --with-system variant=debug,release link=static runtime-link=shared threading=multi define=_SCL_SECURE_NO_WARNINGS=0

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_BOOST -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GLOG ----------------------------------------------------
rem --- Builds glog library used for logging.                                ---
:BUILD_GLOG

echo BUILDING: Google glog - http://code.google.com/p/google-glog/

cd "%PROJECT_BASE%deps\glog"

rem Only build gtest if it hasn't been built already.
if exist "debug\libglog.lib" (
	if exist "release\libglog.lib" (
		echo Google glog library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_GLOG ------------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GTEST ---------------------------------------------------
rem --- Builds all googletest library used for unit testing.                 ---
:BUILD_GTEST

echo BUILDING: Google Test - http://code.google.com/p/googletest/

cd "%PROJECT_BASE%deps\gtest\msvc"

rem Only build gtest if it hasn't been built already.
if exist "gtest-md\Debug\gtest-mdd.lib" (
	if exist "gtest-md\Release\gtest-md.lib" (
		echo Google Test library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_GTEST -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_MYSQLCONN -----------------------------------------------
rem --- Builds the mysql c++ connector library for use with this project.    ---
:BUILD_MYSQLCONN

echo BUILDING: Mysql Connector/C++ - https://launchpad.net/mysql-connector-cpp

cd "%PROJECT_BASE%deps\mysql-connector-cpp"

rem Only build mysql++ if it hasn't been built already.
if exist "driver\Debug\mysqlcppconn.lib" (
	if exist "driver\Release\mysqlcppconn.lib" (
		echo Mysql Connector/C++ already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

rem Build the mysql Connector/C++ library we need.

rem VS likes to create these .cache files and then complain about them existing afterwards.
rem Removing it as it's not needed.
if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "MYSQLCPPCONN.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "MYSQLCPPCONN.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_MYSQLCONN -------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ZLIB ----------------------------------------------------
rem --- Builds the zlib library for use with this project.                   ---
:BUILD_ZLIB

echo BUILDING: zlib - http://www.zlib.net/

cd "%PROJECT_BASE%deps\zlib\projects\visualc6"

rem Only build zlib if it hasn't been built already.
if exist "Win32_LIB_Debug\zlibd.lib" (
	if exist "Win32_LIB_Release\zlib.lib" (
		echo zlib library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

rem Build the zlib library.

rem VS likes to create these .cache files and then complain about them existing afterwards.
rem Removing it as it's not needed.
if exist "zlib.sln.cache" del /S /Q "zlib.sln.cache" >NUL

"%MSBUILD%" "zlib_vc%MSVC_VERSION%.sln" /t:build /p:Platform=Win32,Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

"%MSBUILD%" "zlib_vc%MSVC_VERSION%.sln" /t:build /p:Platform=Win32,Configuration="LIB Release",VCBuildAdditionalOptions="/useenv"
if exist "*.cache" del /S /Q "*.cache" >NUL

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_ZLIB ------------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_PROJECT -------------------------------------------------
rem --- Builds the actual project.                                           ---
:BUILD_PROJECT

if not exist "%PROJECT_BASE%build" (
    mkdir "%PROJECT_BASE%build"    
)
cd "%PROJECT_BASE%build"

cmake -G "Visual Studio 10" -DCMAKE_INSTALL_PREFIX=%PROJECT_BASE% -DENABLE_TEST_REPORT=ON ..

if exist "*.cache" del /S /Q "*.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "sandbox.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	"%MSBUILD%" "RUN_TESTS.vcxproj" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "sandbox.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1	
	"%MSBUILD%" "RUN_TESTS.vcxproj" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "sandbox.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1	
	"%MSBUILD%" "RUN_TESTS.vcxproj" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL

	"%MSBUILD%" "sandbox.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1	
	"%MSBUILD%" "RUN_TESTS.vcxproj" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

goto :eof
rem --- End of BUILD_PROJECT ---------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of SLEEP ---------------------------------------------------------
rem --- Waits some seconds before returning.                                 ---
:SLEEP
ping -n %1 -w 1 127.0.0.1>NUL
goto :eof
rem --- End of SLEEP -----------------------------------------------------------
rem ----------------------------------------------------------------------------

ENDLOCAL