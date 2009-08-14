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

if not exist "%PROJECT_BASE%\deps" (
  echo Missing OpenSWG dependencies!
  echo.
  echo Download the dependencies from %DEPENDENCIES_URL% and unpack it
  echo into the root project directory, then run this script again.
  exit /b 1
)

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
             
set DEPENDENCIES_FILE=cu_sandbox-deps-1.0.0.zip   
set DEPENDENCIES_URL=http://www.openswg.com/downloads/cu_sandbox-deps-1.0.0.zip
set PROJECT_BASE=%~dp0
set BUILD_TYPE=debug
set MSVC_VERSION=vc9

goto :eof
rem --- End of SET_DEFAULTS ----------------------------------------------------



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

if "%0" == "/clean" (
	  call :CLEAN_BUILD
)
 
rem Check for /rebuild then set REBUILD
if "%0" == "/rebuild" (
	  set REBUILD=rebuild
)    

rem Check for /build:x format and then set BUILD_TYPE
if "%0" == "/build" (
	  set BUILD_TYPE=%1
    shift
)

rem Check for /msvc-version:x format and then set MSVC_VERSION
if "%0" == "/msvc-version" (
	  rem Only set if it's an allowed version
	  if "%1" == "vc9" (
		    set MSVC_VERSION=%1
	  )
  
    shift
)

shift

goto :PROCESS_ARGUMENTS
rem --- End of PROCESS_ARGUMENTS -----------------------------------------------



rem --- Start of CLEAN_BUILD ---------------------------------------------------
rem --- Cleans all output created by the build process, restoring the        ---
rem --- project to it's original state like a fresh checkout.                ---
:CLEAN_BUILD

echo Cleaning the build environment

if exist "%PROJECT_BASE%\lib" rmdir /S /Q "%PROJECT_BASE%\lib"
if exist "%PROJECT_BASE%\bin" rmdir /S /Q "%PROJECT_BASE%\bin" 
if exist "%PROJECT_BASE%\deps" rmdir /S /Q "%PROJECT_BASE%\deps"

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

call "%VS_BASE_DIR%\VC\vcvarsall.bat"
  
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

if not exist "%PROJECT_BASE%\deps" call :DOWNLOAD_DEPENDENCIES

if exist "%PROJECT_BASE%\deps\boost" call :BUILD_BOOST
if exist "%PROJECT_BASE%\deps\gtest" call :BUILD_GTEST  
if exist "%PROJECT_BASE%\deps\gmock" call :BUILD_GMOCK
if exist "%PROJECT_BASE%\deps\zlib" call :BUILD_ZLIB

echo ** Building dependencies complete **

goto :eof
rem --- End of BUILD_DEPENDENCIES ----------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of DOWNLOAD_DEPENDENCIES -----------------------------------------
rem --- Downloads the dependency package for the current version of the source -
:DOWNLOAD_DEPENDENCIES    

if not exist "%PROJECT_BASE%\%DEPENDENCIES_FILE%" (
    "%PROJECT_BASE%\tools\wget.exe" %DEPENDENCIES_URL% -O "%PROJECT_BASE%\%DEPENDENCIES_FILE%"
)

if exist "%PROJECT_BASE%\%DEPENDENCIES_FILE%" (
    "%PROJECT_BASE%\tools\unzip.exe" %PROJECT_BASE%\%DEPENDENCIES_FILE% -d %PROJECT_BASE% >NULL
)
                                                      
goto :eof
rem --- End of DOWNLOAD_DEPENDENCIES ----------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_BOOST ---------------------------------------------------
rem --- Builds the boost library for use with this project.                  ---
:BUILD_BOOST

echo BUILDING: Boost - http://www.boost.org/

rem Only build boost if it hasn't been built already.
rem Only build lua if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
    if exist "%PROJECT_BASE%\deps\boost\stage\lib\libboost_*-mt-sgd.lib" (
        echo Boost libraries already built ... skipping
        echo.
        goto :eof
    )
)
if "%BUILD_TYPE%" == "release" (
    if exist "%PROJECT_BASE%\deps\boost\stage\lib\libboost_*-mt-s.lib" (
        echo Boost libraries already built ... skipping
        echo.
        goto :eof
    )
)
if "%BUILD_TYPE%" == "all" (
    if exist "%PROJECT_BASE%\deps\boost\stage\lib\libboost_*-mt-sgd.lib" (
        if exist "%PROJECT_BASE%\deps\boost\stage\lib\libboost_*-mt-s.lib" (
            echo Boost libraries already built ... skipping
            echo.
            goto :eof
        )
    )
)

rem Build BJAM which is needed to build boost.
if not exist "%PROJECT_BASE%\deps\boost\tools\jam\src\bin.ntx86\bjam.exe" (
    cd "%PROJECT_BASE%\deps\boost\tools\jam\src"
    cmd /c "%PROJECT_BASE%\deps\boost\tools\jam\src\build" >NULL
)

rem Build the boost libraries we need.
cd "%PROJECT_BASE%\deps\boost"


if "%BUILD_TYPE%" == "debug" (
    cmd /c "%PROJECT_BASE%\deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=debug link=static runtime-link=static threading=multi >NULL
)

if "%BUILD_TYPE%" == "release" (
    cmd /c "%PROJECT_BASE%\deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=release link=static runtime-link=static threading=multi >NULL
)

if "%BUILD_TYPE%" == "all" (
    cmd /c "%PROJECT_BASE%\deps\boost\tools\jam\src\bin.ntx86\bjam" --toolset=msvc --with-date_time --with-regex --with-thread --with-system --with-program_options variant=debug,release link=static runtime-link=static threading=multi >NULL
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
	  if exist "%PROJECT_BASE%\deps\gtest\msvc\debug\gtestd.lib" (
  	    echo Google Test library already built ... skipping
  	    echo.
  	    goto :eof
    )
)
if "%BUILD_TYPE%" == "release" (
	  if exist "%PROJECT_BASE%\deps\gtest\msvc\release\gtest.lib" (
  	    echo Google Test library already built ... skipping
  	    echo.
  	    goto :eof
    )
)
if "%BUILD_TYPE%" == "all" (
	  if exist "%PROJECT_BASE%\deps\gtest\msvc\debug\gtestd.lib" (
		    if exist "%PROJECT_BASE%\deps\gtest\msvc\release\gtest.lib" (
  		      echo Google Test library already built ... skipping
  		      echo.
  		      goto :eof
  	    )
    )
)

cd "%PROJECT_BASE%\deps\gtest"
    
if exist "%PROJECT_BASE%\deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gtest\msvc\*.cache"
    
if "%BUILD_TYPE%" == "debug" (
    "%MSBUILD%" "%PROJECT_BASE%\deps\gtest\msvc\gtest.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gtest\msvc\*.cache"
)

if "%BUILD_TYPE%" == "release" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gtest\msvc\gtest.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gtest\msvc\*.cache"
)

if "%BUILD_TYPE%" == "all" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gtest\msvc\gtest.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv" >NULL
	  if exist "%PROJECT_BASE%\deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gtest\msvc\*.cache"
	
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gtest\msvc\gtest.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gtest\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gtest\msvc\*.cache"
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
	  if exist "%PROJECT_BASE%\deps\gmock\msvc\debug\gmock.lib" (
  	    echo Google Mock library already built ... skipping
  	    echo.
  	    goto :eof
    )
)
if "%BUILD_TYPE%" == "release" (
	  if exist "%PROJECT_BASE%\deps\gmock\msvc\release\gmock.lib" (
  	    echo Google Mock library already built ... skipping
  	    echo.
  	    goto :eof
    )
)
if "%BUILD_TYPE%" == "all" (
	  if exist "%PROJECT_BASE%\deps\gmock\msvc\debug\gmock.lib" (
		    if exist "%PROJECT_BASE%\deps\gmock\msvc\release\gmock.lib" (
  		      echo Google Mock library already built ... skipping
  		      echo.
  		      goto :eof
  	    )
    )
)

cd "%PROJECT_BASE%\deps\gmock"
    
if exist "%PROJECT_BASE%\deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gmock\msvc\*.cache"
    
if "%BUILD_TYPE%" == "debug" (
    "%MSBUILD%" "%PROJECT_BASE%\deps\gmock\msvc\gmock.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gmock\msvc\*.cache"
)

if "%BUILD_TYPE%" == "release" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gmock\msvc\gmock.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gmock\msvc\*.cache"
)

if "%BUILD_TYPE%" == "all" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gmock\msvc\gmock.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv" >NULL
	  if exist "%PROJECT_BASE%\deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gmock\msvc\*.cache"
	
	  "%MSBUILD%" "%PROJECT_BASE%\deps\gmock\msvc\gmock.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\gmock\msvc\*.cache" del /S /Q "%PROJECT_BASE%\deps\gmock\msvc\*.cache"
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
	  if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
        echo zlib library already built ... skipping
        echo.
        goto :eof
    )
)
if "%BUILD_TYPE%" == "release" (
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib" (
        echo zlib library already built ... skipping
        echo.
        goto :eof
    )
)
if "%BUILD_TYPE%" == "all" (
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\Win32_LIB_Debug\zlibd.lib" (
        if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\Win32_LIB_Release\zlib.lib" (
            echo zlib library already built ... skipping
            echo.
            goto :eof
        )
    )
)

rem Build the zlib library.
cd "%PROJECT_BASE%\deps\zlib"


rem VS likes to create these .cache files and then complain about them existing afterwards.
rem Removing it as it's not needed.
if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache"

if "%BUILD_TYPE%" == "debug" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln" /t:rebuild /p:Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache"
)

if "%BUILD_TYPE%" == "release" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln" /t:rebuild /p:Configuration="LIB Release",VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache"
)

if "%BUILD_TYPE%" == "all" (
	  "%MSBUILD%" "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln" /t:rebuild /p:Configuration="LIB Debug",VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache"

	  "%MSBUILD%" "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln" /t:rebuild /p:Configuration="LIB Release",VCBuildAdditionalOptions="/useenv" >NULL
    if exist "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache" del /S /Q "%PROJECT_BASE%\deps\zlib\projects\visualc6\zlib.sln.cache"
)
goto :eof
rem --- End of BUILD_ZLIB ------------------------------------------------------
rem ----------------------------------------------------------------------------



rem ----------------------------------------------------------------------------
rem --- Start of BUILD_PROJECT -------------------------------------------------
rem --- Builds the actual project.                                           ---
:BUILD_PROJECT

if "%BUILD_TYPE%" == "debug" (
	  "%MSBUILD%" "%PROJECT_BASE%\sandbox_%MSVC_VERSION%.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
)

if "%BUILD_TYPE%" == "release" (
	  "%MSBUILD%" "%PROJECT_BASE%\sandbox_%MSVC_VERSION%.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"
)

if "%BUILD_TYPE%" == "all" (
	  "%MSBUILD%" "%PROJECT_BASE%\sandbox_%MSVC_VERSION%.sln" /t:rebuild /p:Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	  "%MSBUILD%" "%PROJECT_BASE%\sandbox_%MSVC_VERSION%.sln" /t:rebuild /p:Configuration=Release,VCBuildAdditionalOptions="/useenv"
)

goto :eof
rem --- End of BUILD_PROJECT ---------------------------------------------------
rem ----------------------------------------------------------------------------

ENDLOCAL