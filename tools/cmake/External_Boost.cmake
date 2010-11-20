# build the external boost library for the Sandbox project

if(NOT Boost_BUILD_PROJECTS)
    set(_build_projects "ALL")
else()
    foreach(_project ${Boost_BUILD_PROJECTS})        
        if(_build_projects)
            set(_build_projects "${_build_projects}^^${_project}")
        else()
            set(_build_projects "${_project}")
        endif()
    endforeach()
endif()

message("Boost build projects ${_build_projects}")

if(MSVC)
  set(_boost_lib_args
    -DENABLE_SHARED:BOOL=OFF
    -DENABLE_STATIC:BOOL=ON
  )
else()
  set(_boost_lib_args
    -DENABLE_SHARED:BOOL=ON
    -DENABLE_STATIC:BOOL=OFF
  )
endif()

ExternalProject_Add(Boost
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/vendor/boost
    GIT_REPOSITORY git://github.com/boost-lib/boost.git    
    CMAKE_ARGS
        ${_boost_lib_args}
        -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:PATH=<BINARY_DIR>/lib
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=<BINARY_DIR>/lib
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH=<BINARY_DIR>/lib
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=<BINARY_DIR>/lib
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_PROJECTS=${_build_projects}
        -DBUILD_SOVERSIONED:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DWITH_DOXYGEN:BOOL=OFF
        -DWITH_ICU:BOOL=OFF
        -DWITH_MPI:BOOL=OFF
        -DWITH_PYTHON:BOOL=OFF
    LIST_SEPARATOR ^^
    BUILD_COMMAND make genheaders all
    INSTALL_COMMAND ""
    )

# set the boost paths used by other projects
set(Boost_INCLUDE_DIR "${_boost_source_dir}" CACHE PATH "" FORCE)
set(Boost_LIBRARY_DIR "${_boost_binary_dir}/lib" CACHE PATH "" FORCE)
