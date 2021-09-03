cmake_minimum_required(VERSION 2.8)
project(cosmic)

include(CTest)
find_program(MEMORYCHECK_COMMAND valgrind)
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-leak-kinds=all")

include_directories("${PROJECT_SOURCE_DIR}/include")

# Add DLL flag for windows shared lib
add_definitions(-DCOSMIC_DLL_EXPORT)

file(GLOB COSMIC_SOURCE src/*.c)

add_library(cosmic SHARED ${COSMIC_SOURCE})

add_executable(test_cosmic_list tests/test_list.c)
add_executable(test_cosmic_map tests/test_map.c)

target_link_libraries(test_cosmic_list cosmic)
target_link_libraries(test_cosmic_map cosmic)

add_test(NAME TEST_COSMIC_LIST COMMAND test_cosmic_list)
add_test(NAME TEST_COSMIC_MAP COMMAND test_cosmic_map)

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
install(TARGETS cosmic DESTINATION ${PROJECT_SOURCE_DIR}/lib)
