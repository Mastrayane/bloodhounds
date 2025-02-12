#include "json_loader.h"

//#define BOOST_JSON_STANDALONE
// #include <boost/json.hpp>
#include <fstream>
#include <string>

#include "json_parser.h"

using namespace std::literals;

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
  // Загрузить содержимое файла json_path, например, в виде строки
  // Распарсить строку как JSON, используя boost::json::parse
  // Загрузить модель игры из файла

  // open file
  std::ifstream ifs(json_path);
  if (!ifs.is_open()) {
    throw std::runtime_error(("Fail to open "s).append(json_path));
  }

  // get data from file
  const std::string data((std::istreambuf_iterator<char>(ifs)),
                         (std::istreambuf_iterator<char>()));

  ifs.close();

  boost::json::error_code ec;

  // parse
  auto doc = boost::json::parse(data, ec);
  if (ec) {
    throw std::runtime_error(("Fail to json parse "s).append(ec.message()));
  }

  return boost::json::value_to<model::Game>(doc);
}

}  // namespace json_loader
