#include "sdk.h"
//
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include <memory>
#include <thread>

#include "application.h"
#include "json_loader.h"
#include "request_handler.h"  // bad order  need upper then logger
#include "logger.h"           // bad order  need under request_handler
#include "ticker.h"
#include "command_parse.h"

using namespace std::literals;
namespace net = boost::asio;

namespace {

// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
  n = std::max(1u, n);
  std::vector<std::jthread> workers;
  workers.reserve(n - 1);
  // Запускаем n-1 рабочих потоков, выполняющих функцию fn
  while (--n) {
    workers.emplace_back(fn);
  }
  fn();
}


// sys enviropment for DB GAME_DB_URL=postgres://postgres:Mys3Cr3t@192.168.1.200:30432/test_db
constexpr const char DB_URL_ENV_NAME[]{"GAME_DB_URL"};


}  // namespace

int main(int argc, const char* argv[]) {

  c_parse::Args arg{};

  try {
    if (auto args = c_parse::ParseCommandLine(argc, argv)) {
      arg = *args;
    } else {
      return EXIT_FAILURE;
    }

    if (const auto* url_db = std::getenv(DB_URL_ENV_NAME)) {
      arg.db_url = url_db;
    } else {
      std::cout << "The environment variable \"GAME_DB_URL\" is not set" << std::endl;
      return EXIT_FAILURE;
    }


  } catch (const std::exception& e) {
    std::cout << "Parse arguments failure. " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  // initial logger
  Logger::Init();
  
  try {
    // 2. Инициализируем io_context
    // const unsigned num_threads = std::thread::hardware_concurrency();
    const unsigned num_threads = std::thread::hardware_concurrency() > 4u ? std::thread::hardware_concurrency() : 4u;
    net::io_context ioc(num_threads);

    // Application app(ioc, argv[1], argv[2]);
    // Application app(ioc, arg.config, arg.www_root);
    Application app(ioc, arg);

    // signal handler
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
      if (!ec) {
        ioc.stop();
      }
    });

    // request handler
    std::shared_ptr<http_handler::RequestHandler> handler = std::make_shared<http_handler::RequestHandler>(ioc, app);
    // decorator pattern
    LogRequestHandler<http_handler::RequestHandler> loghandler{*handler};

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;

    // run server listener
    http_server::ServeHttp(ioc, {address, port}, [&loghandler](auto&& endp, auto&& req, auto&& send) {
      loghandler(std::forward<decltype(endp)>(endp), std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
    });

    boost::json::value jv_port_address{{"port"s, 8080}, {"address"s, "0.0.0.0"s}};
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, jv_port_address) << "Server has started..."sv;

    //  run async process
    RunWorkers(std::max(1u, num_threads), [&ioc] { ioc.run(); });
    app.SaveState();

  } catch (const std::exception& ex) {
    Logger::LogExit(ex);
    return EXIT_FAILURE;
  }
  Logger::LogExit(0);
  return 0;
}
