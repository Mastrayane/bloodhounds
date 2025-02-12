cmake_minimum_required(VERSION 3.13)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(SRCDIR src)

set(EXECUTE game_server)
set(MODEL_LIB model)
set(JSON_LIB json_parser)


set(SRC 
	${SRCDIR}/main.cpp
  ${SRCDIR}/command_parse.cpp
  ${SRCDIR}/logger.cpp
	${SRCDIR}/json_loader.cpp
  ${SRCDIR}/util.cpp
  ${SRCDIR}/application.cpp
  ${SRCDIR}/response_maker.cpp
  ${SRCDIR}/api_handler.cpp
  ${SRCDIR}/content_type.cpp
	#${SRCDIR}/request_handler.cpp

)

set(HEADER 
  ${SRCDIR}/sdk.h
  ${SRCDIR}/command_parse.h
  ${SRCDIR}/logger.h
  ${SRCDIR}/json_loader.h
  ${SRCDIR}/http_server.h
  ${SRCDIR}/tagged.h
  ${SRCDIR}/application.h
  ${SRCDIR}/request_handler.h
  ${SRCDIR}/content_type.h
  ${SRCDIR}/util.h
  ${SRCDIR}/response_maker.h
  ${SRCDIR}/conn_pool.h
)




find_package(Boost 1.81.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

#find_package(PostgreSQL REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_subdirectory(${SRCDIR}/model)
add_subdirectory(${SRCDIR}/json_parser)

add_executable(${EXECUTE} ${SRC} ${HEADER})
target_include_directories(${EXECUTE} PRIVATE CONAN_PKG::boost CONAN_PKG::libpq CONAN_PKG::libpqxx)
target_link_libraries(${EXECUTE} PRIVATE Threads::Threads CONAN_PKG::boost CONAN_PKG::libpq CONAN_PKG::libpqxx ${MODEL_LIB} ${JSON_LIB})
add_subdirectory(${SRCDIR}/http_server)

add_subdirectory(tests)
#catch_discover_tests(${CMAKE_BINARY_DIR}/bin/game_server_tests) 
