
include(GroupSourcesByFilesystem)

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)
file(GLOB_RECURSE UNITTESTS *_unittest.cpp *_unittest.cc)

list(REMOVE_ITEM SOURCES ${UNITTESTS})

GroupSourcesByFilesystem("${SOURCES}")

add_library(shared ${SOURCES})

include_directories(${ZLIB_INCLUDE_DIR})
