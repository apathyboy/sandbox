cmake_minimum_required(VERSION 2.8)
project(sandbox CXX)

# current source version
set(SANDBOX_VERSION_MAJOR 3)
set(SANDBOX_VERSION_MINOR 0)
set(SANDBOX_VERSION_PATCH 0)
set(SANDBOX_VERSION "${SANDBOX_VERSION_MAJOR}.${SANDBOX_VERSION_MINOR}.${SANDBOX_VERSION_PATCH}")

# set the path to the custom cmake rules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

# include commonly used cmake modules
include(ExternalProject)

# create options for determining whether to download and build
# dependencies or to force the usage of system provided libraries
option(Sandbox_USE_SYSTEM_BOOST
    "If false, Sandbox will download and build its own copy of Boost" FALSE)
option(Sandbox_USE_SYSTEM_GTEST
    "If false, Sandbox will download and build its own copy of GTest" FALSE)

#
# Boost
#
set(Boost_BUILD_PROJECTS 
    date_time
    regex
    system
    thread)

if(Sandbox_USE_SYSTEM_BOOST)
    # the system provided boost was requested, search for it and fail if
    # at least the minimum version is not found
    find_package(Boost ${boost_version} ${Boost_BUILD_PROJECTS} REQUIRED)
    set(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR} PARENT_SCOPE)
    set(Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} PARENT_SCOPE)
else()
    # download and build the custom boost package
    include(External_Boost)
endif()
