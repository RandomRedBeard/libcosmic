cmake_minimum_required(VERSION 3.26)
project(cosmic)

include(GNUInstallDirs)

include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    # Allow posix system calls for socket
    add_definitions(-D_POSIX_C_SOURCE)
    add_compile_options(-Wall -Wextra -pedantic -Werror -ansi)
endif()

file(GLOB COSMIC_SOURCE src/*.c)
file(GLOB COSMIC_PUBLIC_HEADERS include/cosmic/*.h)

set(cosmic_targets)
add_library(cosmic SHARED ${COSMIC_SOURCE})
target_compile_definitions(cosmic PUBLIC COSMIC_DLL_EXPORT)
target_include_directories(cosmic PUBLIC
 $<INSTALL_INTERFACE:include>
 $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
    target_include_directories(${tname} PUBLIC
        $<BUILD_INTERFACE:${COSMIC_PUBLIC_HEADERS}>
     )
    add_test(NAME ${tname} COMMAND ${tname})
endforeach()

install(
    TARGETS cosmic
    EXPORT cosmic_targets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cosmic
)

install(EXPORT cosmic_targets FILE cosmicConfig.cmake DESTINATION lib/cosmic)