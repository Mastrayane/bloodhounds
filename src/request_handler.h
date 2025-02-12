#pragma once

#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>
#include <filesystem>
#include <string_view>

#include "api_handler.h"
#include "application.h"
#include "http_server.h"
#include "model.h"
#include "response_maker.h"
#include "util.h"
#include "content_type.h"


namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

class RequestHandler : public std::enable_shared_from_this<RequestHandler> {
 public:
  explicit RequestHandler(boost::asio::io_context& ioc, Application& app) : m_ioc(ioc), m_app(app), m_strand(boost::asio::make_strand(ioc)) {}

  RequestHandler(const RequestHandler&) = delete;
  RequestHandler& operator=(const RequestHandler&) = delete;

  template <typename Body, typename Allocator, typename Send>
  void operator()(const boost::asio::ip::tcp::endpoint &endp, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    std::string target(req.target());
    if (target.starts_with("/api/")) {

      return boost::asio::dispatch(m_app.strand, [self = shared_from_this(), req = std::move(req), send = std::move(send)] {
        // check execute inside strand
        assert(self->m_app.strand.running_in_this_thread());
        ApiHandler a(self->m_app, req);
        send(a.Response());
      });
    } else {
      if (req.method() == http::verb::get || req.method() == http::verb::head) {
        FileRequest(std::move(req), std::move(send));
        return;
      }
    }

    send(MakeResponse(http::status::bad_request, ErrStr::BAD_REQ, req.version(), req.keep_alive()));
  }

  template <typename Body, typename Allocator, typename Send>
  void FileRequest(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    auto text_response = [&req, &send](http::status status, std::string_view text) {
      send(MakeResponse(status, text, req.version(), req.keep_alive()));
    };

    auto file_response = [&req, &send](http::status status, http::file_body::value_type& file, std::string_view content_type) {
      send(MakeResponse(status, file, req.version(), req.keep_alive(), content_type));
    };

    std::string target(req.target());
    if (target == "/")
      target = "/index.html";

    std::filesystem::path pf = m_app.GetContentDir();
    pf += target;

    if (!util::IsSubPath(pf, m_app.GetContentDir())) {
      return text_response(http::status::bad_request, "Permission deny, or incorrect url request"sv);
    }

    if (!std::filesystem::exists(pf) || !std::filesystem::is_regular_file(pf)) {
      return text_response(http::status::not_found, "Not found resource, or wrong request"sv);
    }

    http::file_body::value_type file;

    if (boost::system::error_code ec; file.open(pf.c_str(), boost::beast::file_mode::read, ec), ec) {
      throw std::logic_error("Wrong open file"s.append(ec.message()));
    }

    std::string ext = pf.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });

    std::string_view content_type = ContentType::DICT.count(ext) ? ContentType::DICT.at(ext) : ContentType::MEDIA_UNKNOWN;

    return file_response(http::status::ok, file, content_type);
  }

 private:
  boost::asio::io_context& m_ioc;
  Application& m_app;
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};

}  // namespace http_handler
