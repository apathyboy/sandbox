
include(GroupSourcesByFilesystem)

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

add_library(shared ${SOURCES})

include_directories(${ZLIB_INCLUDE_DIR})
