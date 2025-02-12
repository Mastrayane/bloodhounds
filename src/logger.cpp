#include "logger.h"

void Logger::LogExit(const int& code) {
  boost::json::object obj{{"code", code}};
  if (code) {
    BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
  } else {
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, obj) << "server exited"sv;
  }
}

void Logger::LogExit(const std::exception& ex) {
  boost::json::object obj{{"exception", ex.what()}};
  BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "server exited"sv;
}

void Logger::LogErr(const boost::system::error_code& ec, std::string_view where) {
  boost::json::object obj = {
      {"code", ec.value()},
      {"text", ec.message()},
      {"where", where},
  };
  BOOST_LOG_TRIVIAL(error) << boost::log::add_value(additional_data, obj) << "error"sv;
}

void Logger::LogErr(const boost::system::error_code& ec, const Where& where) {
  switch (where) {
    case Where::READ:
      LogErr(ec, "read"sv);
      break;
    case Where::WRITE:
      LogErr(ec, "write"sv);
      break;
    case Where::ACCEPT:
      LogErr(ec, "accept"sv);
      break;
    default:
      throw std::logic_error("switch unknown code");
      break;
  }
}

void Logger::LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm) {
  auto ts = *rec[timestamp];
  strm << "{\"timestamp\":\""sv << to_iso_extended_string(ts) << "\","sv;

  auto json_data = boost::json::serialize(*rec[additional_data]);

  strm << "\"data\":"sv << json_data << ","sv;
  strm << "\"message\":\""sv << rec[boost::log::expressions::smessage] << "\"}"sv;
}

void Logger::Init() {
  boost::log::add_common_attributes();
  boost::log::add_console_log(
      std::cout,
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::format = &LogFormatter);
}

void Logger::LogDebug(const std::string& deb, const std::string& where) {
  boost::json::object obj = {
      {"text", deb},
      {"where", where},
  };
  BOOST_LOG_TRIVIAL(debug) << boost::log::add_value(additional_data, obj) << "debug"sv;
}
