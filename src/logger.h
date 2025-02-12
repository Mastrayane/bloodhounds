#ifndef __LOGGER_H__
#define __LOGGER_H__

// #define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/date_time.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <cstdint>

BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", boost::json::value);
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime);

using namespace std::literals;
namespace http = boost::beast::http;
class Logger final {
  Logger() = delete;

 public:
  enum class Where {
    READ,
    WRITE,
    ACCEPT,
  };

  static void LogExit(const int& code);
  static void LogExit(const std::exception& ex);
  static void LogErr(const boost::system::error_code& ec, std::string_view where);
  static void LogErr(const boost::system::error_code& ec, const Where& where);

  static void LogDebug(const std::string& deb, const std::string& where);

  static void Init();

 private:
  static void LogFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);
};

template <class RequestHandler>
class LogRequestHandler {
  template <typename Body, typename Allocator>
  static void LogRequest(const boost::asio::ip::tcp::endpoint& endp, http::request<Body, http::basic_fields<Allocator>>& req) {
    // message — строка request received
    // data — объект с полями:
    // ip — IP-адрес клиента (полученный через endpoint.address().to_string()),
    // URI — запрошенный адрес,
    // method — использованный метод HTTP.
    boost::json::object obj{
        {"ip", endp.address().to_string()},
        {"URI", req.target()},
        {"method", req.method_string()}};
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, obj) << "request received"sv;
  }

  static void LogResponse(int64_t delta, int code, std::string content) {
    // message — строка response sent
    // data — объект с полями:
    // response_time — время формирования ответа в миллисекундах (целое число).
    // code — статус-код ответа, например, 200 (http::response<T>::result_int()).
    // content_type — строка или null, если заголовок в ответе отсутствует.
    if (content.empty()) {
      content = "null";
    }
    boost::json::object obj;
    obj = {
        {"response_time", delta},
        {"code", code},
        {"content_type", content}};
    BOOST_LOG_TRIVIAL(info) << boost::log::add_value(additional_data, obj) << "response sent"sv;
  }

 public:
  explicit LogRequestHandler(RequestHandler& h) : decorated_(h) {}

  template <typename Body, typename Allocator, typename Send>
  void operator()(const boost::asio::ip::tcp::endpoint& endp, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    LogRequest(endp, req);
    decorated_(endp, std::move(req), [send = std::move(send)](auto&& response) {
      std::chrono::high_resolution_clock timer;
      auto start = timer.now();

      const int code_result = response.result_int();
      const std::string content_type = static_cast<std::string>(response.at(http::field::content_type));
      
      send(response);

      auto stop = timer.now();
      auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

      LogRequestHandler::LogResponse(deltaTime, code_result, content_type);
    });
  }

 private:
  RequestHandler& decorated_;
};

#endif  // __LOGGER_H__