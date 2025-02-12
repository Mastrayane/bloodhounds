/**
 * @file session.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __SESSION_H__
#define __SESSION_H__

// #define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>

#include "report_error.h"

namespace http_server {

namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = boost::beast::http;
using HttpRequest = http::request<boost::beast::http::string_body>;


/**
 * @brief 
 * 
 */
class SessionBase {
 public:
  SessionBase(const SessionBase&) = delete;  // restric
  SessionBase& operator=(const SessionBase&) = delete;  // restric

  void Run();

  template <typename Body, typename Fields>
  void Write(http::response<Body, Fields>&& response);

  tcp::endpoint GetEndpoint() const;

 protected:
  explicit SessionBase(tcp::socket&& socket);
  ~SessionBase() = default;

 private:
  // tcp_stream содержит внутри себя сокет и добавляет поддержку таймаутов
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  HttpRequest request_;

  virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;
  virtual void HandleRequest(HttpRequest&& request) = 0;

  void Read();
  void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read);

  void Close();

  void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written);

};

/**
 * @brief 
 * 
 * @tparam RequestHandler 
 */
template <typename RequestHandler>
class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>> {
 public:
  template <typename Handler>
  Session(tcp::socket&& socket, Handler&& request_handler)
      : SessionBase(std::move(socket)), request_handler_(std::forward<Handler>(request_handler)) {}

 private:
  RequestHandler request_handler_;

  std::shared_ptr<SessionBase> GetSharedThis() override { return this->shared_from_this(); }

  void HandleRequest(HttpRequest&& request) override;
};


template <class Body, class Fields>
void SessionBase::Write(boost::beast::http::response<Body, Fields>&& response) {
  // Запись выполняется асинхронно, поэтому response перемещаем в область кучи
  auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));

  auto self = GetSharedThis();
  http::async_write(stream_, *safe_response, [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
    self->OnWrite(safe_response->need_eof(), ec, bytes_written);
  });
}


template <typename RequestHandler>
void Session<RequestHandler>::HandleRequest(HttpRequest&& request) {
  // Захватываем умный указатель на текущий объект Session в лямбде,
  // чтобы продлить время жизни сессии до вызова лямбды.
  // Используется generic-лямбда функция, способная принять response произвольного типа
  request_handler_(GetEndpoint(),  std::move(request), [self = this->shared_from_this()](auto&& response) {
      self->Write(std::move(response));
  });
}

}  // namespace http_server

#endif  // __SESSION_H__