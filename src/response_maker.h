#ifndef __RESPONSE_MAKER_H__
#define __RESPONSE_MAKER_H__

// #define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast/http.hpp>
#include <string_view>

namespace http_handler {

namespace http = boost::beast::http;
using namespace std::string_view_literals;

using StringResponse = http::response<http::string_body>;
using FileResponse = http::response<http::file_body>;

StringResponse MakeResponse(http::status status, std::string_view text,
                            unsigned http_version,
                            bool keep_alive,
                            std::string_view content_type = "text/plain"sv, std::string_view cache_control = "",
                            std::string_view allow = "" );

FileResponse MakeResponse(http::status status, http::file_body::value_type& file,
                          unsigned http_version,
                          bool keep_alive,
                          std::string_view content_type = "application/octet-stream"sv, std::string_view cache_control = "" );

}  // namespace http_handler
#endif  // __RESPONSE_MAKER_H__