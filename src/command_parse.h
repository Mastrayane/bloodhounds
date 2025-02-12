#ifndef __COMMAND_PARSE_H__
#define __COMMAND_PARSE_H__

#include <boost/program_options.hpp>
#include <optional>
#include <vector>
#include <iostream>

namespace c_parse {

using namespace std::literals;

struct Args {
  unsigned int tick_period{0};
  std::string config_file{};
  std::string www_root{};
  bool random_spawn{false};
  std::string state_file{};
  unsigned int save_state_period{0};
  std::string db_url{};
};

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]);

}

#endif  // __COMMAND_PARSE_H__