
include(GroupSourcesByFilesystem)

file(GLOB_RECURSE SOURCES *.h *.hpp *.cpp *.cc)

GroupSourcesByFilesystem("${SOURCES}")

add_executable(sandbox ${SOURCES})

target_link_libraries(sandbox
    shared
    login
    zone
    ${Boost_LIBRARIES}
)

if(ZLIB_LIBRARY_DEBUG)
    target_link_libraries(sandbox debug ${ZLIB_LIBRARY_DEBUG})
    get_filename_component(ZLIB_PATH ${ZLIB_LIBRARY_DEBUG} PATH)
    get_filename_component(ZLIB_FILE ${ZLIB_LIBRARY_DEBUG} NAME_WE)
endif()

if(ZLIB_LIBRARY_RELEASE)
    target_link_libraries(sandbox optimized ${ZLIB_LIBRARY_RELEASE})
    get_filename_component(ZLIB_PATH ${ZLIB_LIBRARY_DEBUG} PATH)
    get_filename_component(ZLIB_FILE ${ZLIB_LIBRARY_DEBUG} NAME_WE)
endif()

if(WIN32)
    add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${ZLIB_PATH}/${ZLIB_FILE}.dll
            $<TARGET_FILE_DIR:sandbox>)
endif()

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/data/packets
        $<TARGET_FILE_DIR:sandbox>/packets
    COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_SOURCE_DIR}/data/sandbox.cfg.dist
        $<TARGET_FILE_DIR:sandbox>/sandbox.cfg)
