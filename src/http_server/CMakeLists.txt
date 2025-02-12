


#set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SRC 
  session.cpp
)
message(${SRCDIR})

set(HEADER 
  report_error.h
  listener.h
  session.h
  serve_http.h
)

message(${SRC})

target_sources(${EXECUTE} PUBLIC ${SRC} ${HEADER})