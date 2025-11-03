cmake_minimum_required(VERSION 3.26)
project(cosmic)

include(GNUInstallDirs)

include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")

file(GLOB COSMIC_SOURCE src/*.c)
file(GLOB COSMIC_INCLUDE_DIRS include/cosmic/*.h)

set(cosmic_targets)
add_library(cosmic SHARED ${COSMIC_SOURCE})
target_compile_definitions(cosmic PUBLIC COSMIC_DLL_EXPORT)

if (MSVC)
    target_compile_options(cosmic PRIVATE /W4 /WX)
else()
    # Allow posix system calls for socket
    target_compile_definitions(cosmic PRIVATE -D_POSIX_C_SOURCE=200809)
    target_compile_options(cosmic PRIVATE -Wall -Wextra -pedantic -Werror -std=c99)
endif()

target_include_directories(cosmic PUBLIC
 $<INSTALL_INTERFACE:include>
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Required for all apparently
set_target_properties(cosmic
    PROPERTIES
    PUBLIC_HEADER "${COSMIC_INCLUDE_DIRS}"
)

# Which lib to link against (default to shared)
set(COSMIC_LIBRARY cosmic)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/library_config/libcosmic.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libcosmic.pc" @ONLY)

file(GLOB files tests/*.c)
foreach(file ${files})
    get_filename_component(tname ${file} NAME_WLE)
    add_executable(${tname} ${file})
    target_link_libraries(${tname} ${COSMIC_LIBRARY})
    add_test(NAME ${tname} COMMAND ${tname})
endforeach()

install(
    TARGETS cosmic
    EXPORT cosmic_targets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cosmic
)

install(EXPORT cosmic_targets FILE cosmicConfig.cmake DESTINATION cmake)

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/library_config/uninstall.cmake")
