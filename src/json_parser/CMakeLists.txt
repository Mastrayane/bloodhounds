
set(SRCROOT ${PROJECT_SOURCE_DIR}/src)


set(SRC 
  boost_json.cpp
  tag_invokers.cpp
  tag_invokers.h
)

#include_directories(${SRCROOT})


add_library(${JSON_LIB} STATIC ${SRC})
target_include_directories(${JSON_LIB} PRIVATE CONAN_PKG::boost)
target_link_libraries(${JSON_LIB} PRIVATE CONAN_PKG::boost)