cmake_minimum_required(VERSION 2.8)
project(sandbox CXX)

# current source version
set(SANDBOX_VERSION_MAJOR 3)
set(SANDBOX_VERSION_MINOR 0)
set(SANDBOX_VERSION_PATCH 0)
set(SANDBOX_VERSION "${SANDBOX_VERSION_MAJOR}.${SANDBOX_VERSION_MINOR}.${SANDBOX_VERSION_PATCH}")

# set the path to the custom cmake rules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

# Some automated systems need an xml report which requires a second run of the
# tests. To prevent these unnecessary runnings during normal builds the
# following option is set with a default of false.
OPTION(ENABLE_TEST_REPORT
    "Enables the generation of a test report when running tests" FALSE)

# On gcc platforms the c++0x flag is needed to enable features used by this project.
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
    # If not gcc and on WIN32 we're using visual studio, perform related checks here.
    IF(WIN32)
        # If using visual studio's compiler make sure we're using at least version 10
        IF(MSVC_VERSION LESS 1700)
            MESSAGE(FATAL_ERROR "MSVC 11 or higher is required to build this library")
        ENDIF()

        SET(_WIN32_WINNT 0x0501 CACHE INTERNAL "Setting _WIN32_WINNT to 0x0501 for Windows XP minimum APIs")
        SET(WINVER 0x0501 CACHE INTERNAL "Setting WINVER to 0x0501 for Windows XP minimum APIs")

        ADD_DEFINITIONS (/D _WIN32_WINNT=${_WIN32_WINNT})
        MESSAGE(STATUS "- MSVC: Set minimum Windows API version")

        ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS /D _SCL_SECURE_NO_WARNINGS)
        MESSAGE(STATUS "- MSVC: Disabled NON-SECURE warnings")
    ENDIF()
ENDIF()

# Use the static/multithreaded libraries.
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)

FIND_PACKAGE(Boost 1.44.0 COMPONENTS date_time program_options regex thread system REQUIRED)
FIND_PACKAGE(GLM REQUIRED)
FIND_PACKAGE(ZLib REQUIRED)

# Add boost and tbb to the include and link directories as everything needs it.
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src ${Boost_INCLUDE_DIRS} ${GLM_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# Enable testing and add gtest to the include directories.
ENABLE_TESTING()

ADD_SUBDIRECTORY(src)
