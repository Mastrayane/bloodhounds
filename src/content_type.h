#ifndef __CONTENT_TYPE_H__
#define __CONTENT_TYPE_H__

#include <string_view>
#include <unordered_map>

namespace http_handler {
using namespace std::string_view_literals;

struct ErrStr {
  ErrStr() = delete;
  constexpr static std::string_view MAP_NOT_FOUND = R"({"code": "mapNotFound", "message": "Map not found"})"sv;
  constexpr static std::string_view BAD_REQ = R"({"code": "badRequest", "message": "Bad request"})"sv;
  constexpr static std::string_view BAD_PARSE = R"({"code": "invalidArgument", "message": "Join game request parse error"})"sv;
  constexpr static std::string_view USERNAME_EMPTY = R"({"code": "invalidArgument", "message": "Invalid name"})"sv;
  constexpr static std::string_view POST_INVALID = R"({"code": "invalidMethod", "message": "Only POST method is expected"})"sv;
  constexpr static std::string_view GET_INVALID = R"({"code": "invalidMethod", "message": "Only GET method is expected"})"sv;
};

struct CacheControl {
  CacheControl() = delete;
  constexpr static std::string_view NO_CACHE = "no-cache"sv;
};

struct ContentType {
  ContentType() = delete;
  constexpr static std::string_view TEXT_HTML = "text/html"sv;
  constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
  constexpr static std::string_view APP_JSON = "application/json"sv;
  constexpr static std::string_view MEDIA_UNKNOWN = "application/octet-stream"sv;
  static const std::unordered_map<std::string, std::string> DICT;
};

}  // namespace http_handler
#endif  // __CONTENT_TYPE_H__