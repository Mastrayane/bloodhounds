

cmake_minimum_required(VERSION 3.13)

project(game_server_tests CXX)
set(CMAKE_CXX_STANDARD 20)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
#include(${CMAKE_SOURCE_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)
include(CTest)
include(Catch)

set(TEST_EXEC game_server_tests)

set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)


set(SRC_TEST 
  ${TEST_DIR}/state-serialization-tests.cpp
)

add_executable(${TEST_EXEC} ${SRC_TEST})
target_include_directories(${TEST_EXEC} PRIVATE CONAN_PKG::catch2 CONAN_PKG::boost)
target_link_libraries(${TEST_EXEC} PRIVATE Threads::Threads CONAN_PKG::catch2 CONAN_PKG::boost ${MODEL_LIB})
catch_discover_tests(${TEST_EXEC})
