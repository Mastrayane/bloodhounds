#ifndef __LISTENER_H__
#define __LISTENER_H__

// boost.beast будет использовать std::string_view вместо boost::string_view
// #define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "session.h"

// Ядро асинхронного HTTP-сервера будет располагаться в пространстве имён http_server
namespace http_server {

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace sys = boost::system;

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
 public:
  template <typename Handler>
  Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler);

  void Run() { DoAccept(); }

 private:
  net::io_context& ioc_;
  tcp::acceptor acceptor_;
  RequestHandler request_handler_;

  void DoAccept();
  void OnAccept(sys::error_code ec, tcp::socket socket);
  void AsyncRunSession(tcp::socket&& socket);
};

template <typename RequestHandler>
template <typename Handler>
Listener<RequestHandler>::Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler)
    : ioc_(ioc)
      // Обработчики асинхронных операций acceptor_ будут вызываться в своём strand
      ,
      acceptor_(net::make_strand(ioc)),
      request_handler_(std::forward<Handler>(request_handler)) {
  // Открываем acceptor, используя протокол (IPv4 или IPv6), указанный в endpoint
  acceptor_.open(endpoint.protocol());

  // После закрытия TCP-соединения сокет некоторое время может считаться занятым,
  // чтобы компьютеры могли обменяться завершающими пакетами данных.
  // Однако это может помешать повторно открыть сокет в полузакрытом состоянии.
  // Флаг reuse_address разрешает открыть сокет, когда он "наполовину закрыт"
  acceptor_.set_option(net::socket_base::reuse_address(true));
  // Привязываем acceptor к адресу и порту endpoint
  acceptor_.bind(endpoint);
  // Переводим acceptor в состояние, в котором он способен принимать новые соединения
  // Благодаря этому новые подключения будут помещаться в очередь ожидающих соединений
  acceptor_.listen(net::socket_base::max_listen_connections);
}

template <typename RequestHandler>
void Listener<RequestHandler>::DoAccept() {
  acceptor_.async_accept(
      // Передаём последовательный исполнитель, в котором будут вызываться обработчики
      // асинхронных операций сокета
      net::make_strand(ioc_),
      // С помощью bind_front_handler создаём обработчик, привязанный к методу OnAccept
      // текущего объекта.
      // Так как Listener — шаблонный класс, нужно подсказать компилятору, что
      // shared_from_this — метод класса, а не свободная функция.
      // Для этого вызываем его, используя this
      // Этот вызов bind_front_handler аналогичен
      // namespace ph = std::placeholders;
      // std::bind(&Listener::OnAccept, this->shared_from_this(), ph::_1, ph::_2)
      beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));
}

template <typename RequestHandler>
void Listener<RequestHandler>::OnAccept(sys::error_code ec, tcp::socket socket) {
  using namespace std::literals;

  if (ec) {
    return ReportError(ec, "accept"sv);
  }

  // Асинхронно обрабатываем сессию
  AsyncRunSession(std::move(socket));

  // Принимаем новое соединение
  DoAccept();
}

template <typename RequestHandler>
void Listener<RequestHandler>::AsyncRunSession(tcp::socket&& socket) {
  std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();
}

}  // namespace http_server
#endif  // __LISTENER_H__