cmake_minimum_required(VERSION 2.8)
project(cosmic)

include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror -std=c90 -ansi)
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

# Add DLL flag for windows shared lib
add_definitions(-DCOSMIC_DLL_EXPORT)
add_definitions(-D_POSIX_C_SOURCE)

file(GLOB COSMIC_SOURCE src/*.c)

add_library(cosmic SHARED ${COSMIC_SOURCE})

add_executable(test_cosmic_list tests/test_list.c)
add_executable(test_cosmic_map tests/test_map.c)
add_executable(test_cosmic_io_mem tests/test_io_mem.c)
add_executable(test_cosmic_json tests/test_json.c)
add_executable(test_cosmic_json_write tests/test_json_write.c)
add_executable(test_cosmic_json_read tests/test_json_read.c)
add_executable(test_cosmic_socket tests/test_socket.c)

target_link_libraries(test_cosmic_list cosmic)
target_link_libraries(test_cosmic_map cosmic)
target_link_libraries(test_cosmic_io_mem cosmic)
target_link_libraries(test_cosmic_json cosmic)
target_link_libraries(test_cosmic_json_write cosmic)
target_link_libraries(test_cosmic_json_read cosmic)
target_link_libraries(test_cosmic_socket cosmic)

add_test(NAME TEST_COSMIC_LIST COMMAND test_cosmic_list)
add_test(NAME TEST_COSMIC_MAP COMMAND test_cosmic_map)
add_test(NAME TEST_COSMIC_IO_MEM COMMAND test_cosmic_io_mem)
add_test(NAME TEST_COSMIC_JSON COMMAND test_cosmic_json)
add_test(NAME TEST_COSMIC_JSON_WRITE COMMAND test_cosmic_json_write)
add_test(NAME TEST_COSMIC_JSON_READ COMMAND test_cosmic_json_read)
add_test(NAME TEST_COSMIC_SOCKET COMMAND test_cosmic_socket)

install(TARGETS cosmic DESTINATION ${PROJECT_SOURCE_DIR}/lib)
