@echo off
SETLOCAL
rem msvc_build.cmd
rem
rem This is the build script used for building the Combat Upgrade
rem Sandbox Server using Microsoft tools.


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
call :BUILD_PROJECT

goto :eof
rem --- End of Main Execution --------------------------------------------------



rem ----------------------------------------------------------------------------
rem ----------------------------------------------------------------------------
rem --- Helper Functions -------------------------------------------------------
rem ----------------------------------------------------------------------------
rem ----------------------------------------------------------------------------


rem --- Start of SET_DEFAULTS --------------------------------------------------
:SET_DEFAULTS

set PROJECT_BASE=%~dp0\..
set BUILD_TYPE=debug
set MSVC_VERSION=vc9

goto :eof
rem --- End of SET_DEFAULTS ----------------------------------------------------



rem --- Start of PROCESS_ARGUMENTS ---------------------------------------------
:PROCESS_ARGUMENTS

if "%0" == "" goto :CONTINUE_FROM_PROCESS_ARGUMENTS

set arg=%0

if "%arg" == "-h" (
	echo msvc_build.cmd Help
	echo.

	echo     /clean												Cleans the generated files
	echo     /build:[debug-release-all] 	Specifies the build type, defaults to debug	
	echo     /msvc-version:[vc9] 					Specifies the msvc version and project files to use	
)

if "%arg:~1%" == "clean" (
	call :CLEAN_BUILD
)

rem Check for /build:x format and then set BUILD_TYPE
if "%arg:~1,5%" == "build" (
	set BUILD_TYPE=%arg:~7%
)

rem Check for /msvc-version:x format and then set MSVC_VERSION
if "%arg:~1,12%" == "msvc-version" (
	rem Only set if it's an allowed version
	if "%arg:~14%" == "vc9" (
		set MSVC_VERSION=%arg:~14%
	)
)

shift

goto :PROCESS_ARGUMENTS
rem --- End of PROCESS_ARGUMENTS -----------------------------------------------



rem --- Start of CLEAN_BUILD ---------------------------------------------------
rem --- Cleans all output created by the build process, restoring the        ---
rem --- project to it's original state like a fresh checkout.                ---
:CLEAN_BUILD

echo Cleaning the build environment

if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib rmdir /S /Q "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib
if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\bin rmdir /S /Q "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\bin

goto :eof
rem --- End of CLEAN_BUILD -----------------------------------------------------



rem --- Start of BUILD_ENVIRONMENT ---------------------------------------------
:BUILD_ENVIRONMENT

call :BUILD_ENVIRONMENT_FOR_%MSVC_VERSION%

rem Set to devenv.exe for all versions of VS except express. Required to upgrade
rem Visual Studio projects.
if exist "%VS_BASE_DIR%"\Common7\IDE\devenv.com (
  set "DEVENV=%VS_BASE_DIR%\Common7\IDE\devenv.com"
) else (
  set "DEVENV=%VS_BASE_DIR%\Common7\IDE\vcexpress.exe"
)



set DOTNET_BASE_DIR=C:\WINDOWS\Microsoft.NET\Framework\v3.5
if not exist "%DOTNET_BASE_DIR%" (
  echo ***** Microsoft .NET Framework 3.5 required *****
  exit /b 1
)

set "MSBUILD=%DOTNET_BASE_DIR%\msbuild.exe"

call "%VS_BASE_DIR%"\VC\vcvarsall.bat
  
set environment_built=yes

goto :eof
rem --- End of BUILD_ENVIRONMENT -----------------------------------------------



rem --- Start of BUILD_ENVIRONMENT_FOR_vc9 -------------------------------------
:BUILD_ENVIRONMENT_FOR_vc9

set "VS_BASE_DIR=C:\Program Files (x86)\Microsoft Visual Studio 9.0"
if not exist "%VS_BASE_DIR%" (
	set "VS_BASE_DIR=C:\Program Files\Microsoft Visual Studio 9.0"
	if not exist "%VS_BASE_DIR%" (
	
		rem TODO: Allow user to enter a path to their base visual Studio directory.

  	echo ***** Microsoft Visual Studio 9.0 required *****
  	exit /b 1	
	)	
) 

set "DOTNET_BASE_DIR=C:\WINDOWS\Microsoft.NET\Framework\v3.5"
if not exist "%DOTNET_BASE_DIR%" (
  echo ***** Microsoft .NET Framework 3.5 required *****
  exit /b 1
)

goto :eof
rem --- End of BUILD_ENVIRONMENT_FOR_vc9 ---------------------------------------



rem --- Start of BUILD_DEPENDENCIES --------------------------------------------
rem --- Builds all external dependencies needed by the project.              ---
:BUILD_DEPENDENCIES

echo ** Building dependencies necessary for this project **
echo.

if exist "%PROJECT_BASE%"\boost call :BUILD_BOOST
if exist "%PROJECT_BASE%"\gtest call :BUILD_GTEST
if exist "%PROJECT_BASE%"\gtest call :BUILD_ZLIB

echo ** Building dependencies complete **

goto :eof
rem --- End of BUILD_DEPENDENCIES ----------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_BOOST ---------------------------------------------------
rem --- Builds the boost library for use with this project.                  ---
:BUILD_BOOST

echo BUILDING: Boost libraries - http://www.boost.org/ 

rem Only build boost if it hasn't been built already.
if exist "%PROJECT_BASE%\msvc\%MSVC_VERSION%\lib\libboost_*.lib" (
  echo Boost libraries already built ... skipping
  echo.
  goto :eof
)

rem Build BJAM which is needed to build boost.
if not exist "%PROJECT_BASE%\boost\tools\jam\src\bin.ntx86\bjam.exe" (
  cd "%PROJECT_BASE%\boost\tools\jam\src"
  cmd /c "%PROJECT_BASE%\boost\tools\jam\src\build"
)

rem Build the boost libraries we need.
cd "%PROJECT_BASE%\boost"
cmd /c "%PROJECT_BASE%\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options --with-python --build-type=complete stage

if exist "%PROJECT_BASE%\boost\stage\lib" (
  echo Copying boost library files to final location
  xcopy "%PROJECT_BASE%"\boost\stage\lib" "%PROJECT_BASE%\msvc\%MSVC_VERSION%\lib" /s /e /Y /i
)
goto :eof
rem --- End of BUILD_BOOST -----------------------------------------------------
rem ----------------------------------------------------------------------------



rem --- Start of BUILD_GTEST ---------------------------------------------------
rem --- Builds all googletest library used for unit testing.                 ---
:BUILD_GTEST

echo BUILDING: Googletest library - http://code.google.com/p/googletest/

rem Only build gtest if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\gtestd.lib (
  	echo Googletest library already built ... skipping
  	echo.
  	goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\gtest.lib (
  	echo Googletest library already built ... skipping
  	echo.
  	goto :eof
  )
)
if "%BUILD_TYPE%" == "all" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\gtestd.lib (
		if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\gtest.lib (
  		echo Googletest library already built ... skipping
  		echo.
  		goto :eof
  	)
  )
)

cd %PROJECT_BASE%\gtest

echo %BUILD_TYPE%

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "%PROJECT_BASE%"\gtest\msvc\gtest.vcproj /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	
	if exist "%PROJECT_BASE%"\gtest\msvc\debug\gtestd.lib (
  	xcopy "%PROJECT_BASE%"\gtest\msvc\debug\gtestd.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "%PROJECT_BASE%"\gtest\msvc\gtest.vcproj /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"

	if exist "%PROJECT_BASE%"\gtest\msvc\release\gtest.lib (
  	xcopy "%PROJECT_BASE%"\gtest\msvc\release\gtest.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "%PROJECT_BASE%"\gtest\msvc\gtest.vcproj /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	
	if exist "%PROJECT_BASE%"\gtest\msvc\debug\gtestd.lib (
  	xcopy "%PROJECT_BASE%"\gtest\msvc\debug\gtestd.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
	
	"%MSBUILD%" "%PROJECT_BASE%"\gtest\msvc\gtest.vcproj /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"

	if exist "%PROJECT_BASE%"\gtest\msvc\release\gtest.lib (
  	xcopy "%PROJECT_BASE%"\gtest\msvc\release\gtest.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)
goto :eof
rem --- End of BUILD_GTEST -----------------------------------------------------



rem --- Start of BUILD_ZLIB ----------------------------------------------------
:BUILD_ZLIB

echo BUILDING: zlib library - http://www.zlib.net/

rem Only build zlib if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\zlibd.lib (
  	echo zlib library already built ... skipping
  	echo.
  	goto :eof
  )
)
if "%BUILD_TYPE%" == "release" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\zlib.lib (
  	echo zlib library already built ... skipping
  	echo.
  	goto :eof
  )
)
if "%BUILD_TYPE%" == "all" (
	if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\zlibd.lib (
		if exist "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib\zlib.lib (
  		echo zlib library already built ... skipping
  		echo.
  		goto :eof
  	)
  )
)

cd %PROJECT_BASE%\zlib

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "%PROJECT_BASE%"\zlib\projects\visualc6\zlib.vcproj /t:rebuild /p:Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv"
	
	if exist "%PROJECT_BASE%\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
  	xcopy "%PROJECT_BASE%\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "%PROJECT_BASE%"\zlib\projects\visualc6\zlib.vcproj /t:rebuild /p:Configuration="LIB Release",VCBuildAdditionalOptions="/useenv"

	if exist "%PROJECT_BASE%"\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib (
  	xcopy "%PROJECT_BASE%"\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "%PROJECT_BASE%"\zlib\projects\visualc6\zlib.vcproj /t:rebuild /p:Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv"
	
	if exist "%PROJECT_BASE%\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
  	xcopy "%PROJECT_BASE%\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
	
	"%MSBUILD%" "%PROJECT_BASE%"\zlib\projects\visualc6\zlib.vcproj /t:rebuild /p:Configuration="LIB Release",VCBuildAdditionalOptions="/useenv"

	if exist "%PROJECT_BASE%"\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib (
  	xcopy "%PROJECT_BASE%"\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\lib /s /e /Y
	)
)
goto :eof
rem --- End of BUILD_ZLIB ------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_PROJECT -------------------------------------------------
rem --- Builds the actual project.                                           ---
:BUILD_PROJECT

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\sandbox.sln /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\sandbox.sln /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\sandbox.sln /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	"%MSBUILD%" "%PROJECT_BASE%"\msvc\%MSVC_VERSION%\sandbox.sln /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"
)

goto :eof
rem --- End of BUILD_PROJECT ---------------------------------------------------
rem ----------------------------------------------------------------------------

ENDLOCAL