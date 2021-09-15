cmake_minimum_required(VERSION 2.8)

set(MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")

if(NOT EXISTS ${MANIFEST})
    message(FATAL_ERROR "Cannot find install mainfest: ${MANIFEST}")
endif()

file(STRINGS ${MANIFEST} files)
foreach(file ${files})
    if(EXISTS ${file} OR IS_SYMLINK ${file})
        message(STATUS "Removing: \"${file}\"")
        file(REMOVE ${file})
    else()
        message(STATUS "Does-not-exist: \"${file}\"")
    endif()
endforeach(file)

message("PATH ${CMAKE_INSTALL_FULL_LIBDIR}")