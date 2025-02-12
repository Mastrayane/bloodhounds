/**
 * @file serve_http.hpp
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __SERVERHTTP_HPP__
#define __SERVERHTTP_HPP__

#include <boost/asio/ip/tcp.hpp>
#include <memory>

#include "listener.h"

namespace http_server {

template <typename RequestHandler>
inline void ServeHttp(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint& endpoint, RequestHandler&& handler) {
  // При помощи decay_t исключим ссылки из типа RequestHandler,
  // чтобы Listener хранил RequestHandler по значению
  using MyListener = Listener<std::decay_t<RequestHandler>>;

  std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
}

}  // namespace http_server

#endif // __SERVERHTTP_HPP__