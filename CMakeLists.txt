cmake_minimum_required(VERSION 2.8)
project(cosmic)

include(GNUInstallDirs)

include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")


if (MSVC)
    # DLL builder define
    add_compile_options(/W4 /WX)
else()
    # Allow posix system calls for socket
    add_definitions(-D_POSIX_C_SOURCE)
    add_compile_options(-Wall -Wextra -pedantic -Werror -ansi)
endif()

include_directories(include)

file(GLOB COSMIC_SOURCE src/*.c)
file(GLOB COSMIC_PUBLIC_HEADERS include/cosmic/*.h)

option(COSMIC_SHARED "Build shared libcosmic" ON)
option(COSMIC_STATIC "Build static libcosmic" ON)

if (NOT COSMIC_SHARED AND NOT COSMIC_STATIC)
    message(FATAL_ERROR "Nothing to build")
endif()

set(cosmic_targets)

if (COSMIC_SHARED)
    add_library(cosmic SHARED ${COSMIC_SOURCE})
    target_compile_definitions(cosmic PUBLIC COSMIC_DLL_EXPORT)
    set(cosmic_targets cosmic)
endif()

if (COSMIC_STATIC)
    add_library(cosmic-static STATIC ${COSMIC_SOURCE})
    target_compile_definitions(cosmic-static PUBLIC COSMIC_STATIC)
    set(cosmic_targets ${cosmic_targets} cosmic-static)
endif()

# Which lib to link against (default to shared)
set(cosmic_lib cosmic)
if (NOT COSMIC_SHARED)
    set(cosmic_lib cosmic-static)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/library_config/libcosmic.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/libcosmic.pc" @ONLY)

add_executable(test_cosmic_list tests/test_list.c)
add_executable(test_cosmic_map tests/test_map.c)
add_executable(test_cosmic_io_mem tests/test_io_mem.c)
add_executable(test_cosmic_json tests/test_json.c)
add_executable(test_cosmic_json_write tests/test_json_write.c)
add_executable(test_cosmic_json_read tests/test_json_read.c)
add_executable(test_cosmic_socket tests/test_socket.c)

target_link_libraries(test_cosmic_list ${cosmic_lib})
target_link_libraries(test_cosmic_map ${cosmic_lib})
target_link_libraries(test_cosmic_io_mem ${cosmic_lib})
target_link_libraries(test_cosmic_json ${cosmic_lib})
target_link_libraries(test_cosmic_json_write ${cosmic_lib})
target_link_libraries(test_cosmic_json_read ${cosmic_lib})
target_link_libraries(test_cosmic_socket ${cosmic_lib})

add_test(NAME TEST_COSMIC_LIST COMMAND test_cosmic_list)
add_test(NAME TEST_COSMIC_MAP COMMAND test_cosmic_map)
add_test(NAME TEST_COSMIC_IO_MEM COMMAND test_cosmic_io_mem)
add_test(NAME TEST_COSMIC_JSON COMMAND test_cosmic_json)
add_test(NAME TEST_COSMIC_JSON_WRITE COMMAND test_cosmic_json_write)
add_test(NAME TEST_COSMIC_JSON_READ COMMAND test_cosmic_json_read)
add_test(NAME TEST_COSMIC_SOCKET COMMAND test_cosmic_socket)

install(
    TARGETS ${cosmic_targets}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${COSMIC_PUBLIC_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cosmic")
install (FILES "${CMAKE_CURRENT_BINARY_DIR}/libcosmic.pc" DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/library_config/uninstall.cmake")
