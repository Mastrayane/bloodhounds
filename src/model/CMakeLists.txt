

message("Model source")

set(SRC_MODEL
  collector.cpp
  dog.cpp
  game.cpp
  game_object.cpp
  game_session.cpp
  #loot.cpp
  loot_generator.cpp
  model_map.cpp
  player.cpp
  player_list.cpp
  token_generator.cpp
)

set(HEADER_MODEL 
  building.h
  collector.h
  dog.h
  game.h
  game_object.h
  game_session.h
  geometry.h
  loot.h
  loot_generator.h
  model_map.h
  office.h
  player.h
  player_list.h
  road.h
  token_generator.h
)

add_library(${MODEL_LIB} STATIC ${SRC_MODEL} ${HEADER_MODEL})
