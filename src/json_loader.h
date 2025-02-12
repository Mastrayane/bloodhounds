#pragma once


#include <filesystem>

#include "model.h"


namespace json_loader {

/**
 * @brief Create model::Game from json map
 * 
 * @param json_path path to a json map file
 * @return model::Game 
 */
model::Game LoadGame(const std::filesystem::path& json_path);

}  // namespace json_loader
