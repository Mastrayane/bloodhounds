#include "command_parse.h"

namespace c_parse {

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[]) {
  namespace po = boost::program_options;

  po::options_description desc{"Allowed options"s};
  Args args;

  // Allowed options:
  // -h [ --help ]                     produce help message
  // -t [ --tick-period ] milliseconds set tick period
  // -c [ --config-file ] file         set config file path
  // -w [ --www-root ] dir             set static files root
  // --randomize-spawn-points          spawn dogs at random positions
  desc.add_options()                                                                                                //
      ("help,h", "produce help message")                                                                            //
      ("tick-period,t", po::value<unsigned int>(&args.tick_period)->value_name("milliseconds"), "set tick period")  //
      ("config-file,c", po::value(&args.config_file)->value_name("file"), "set config file path")                   //
      ("www-root,w", po::value(&args.www_root)->value_name("dir"), "set static files root")                         //
      ("randomize-spawn-points", po::bool_switch(&args.random_spawn), "spawn dogs at random positions")             //
      ("state-file", po::value(&args.state_file)->value_name("file"), "set game save file")                         //
      ("save-state-period", po::value<unsigned int>(&args.save_state_period)->value_name("milliseconds"), "set period for autosave");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.contains("help"s)) {
    std::cout << desc;
    return std::nullopt;
  }

  if (vm.contains("config-file") && vm.contains("www-root")) {
    return args;
  } else {
    throw std::runtime_error("Usage: game_server --config-file <game-config-json> --www-root <dir-to-content>");
  }

  return std::nullopt;
}

}  // namespace c_parse