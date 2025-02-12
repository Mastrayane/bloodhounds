#ifndef __API_HANDLER_H__
#define __API_HANDLER_H__

// #define BOOST_BEAST_USE_STD_STRING_VIEW
#include <boost/beast/http.hpp>
#include <functional>
#include <string_view>

#include "application.h"
#include "model/player_list.h"
#include "json_parser.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using namespace std::literals;

using StringResponse = http::response<http::string_body>;
using StringRequest = http::request<http::string_body>;

/// Api constexpr values
struct ApiKey {
  static constexpr std::string_view Action = "/api/v1/game/player/action"sv;
  static constexpr std::string_view Join = "/api/v1/game/join"sv;
  static constexpr std::string_view Maps = "/api/v1/maps"sv;
  static constexpr std::string_view MapsPrefix = "/api/v1/maps/"sv;
  static constexpr std::string_view Players = "/api/v1/game/players"sv;
  static constexpr std::string_view Records = "/api/v1/game/records"sv;
  static constexpr std::string_view State = "/api/v1/game/state"sv;
  static constexpr std::string_view Tick = "/api/v1/game/tick"sv;
};

class ApiHandler {
 public:
  ApiHandler(Application& app, const StringRequest& req);

  StringResponse Response();

 private:
  Application& m_app;
  const StringRequest& m_req;
  const std::string m_target;

  StringResponse MapRequest();

  StringResponse PlayerJoinRequest();

  StringResponse PlayerListRequest();

  StringResponse GetGameState();

  StringResponse PostAction();

  StringResponse PostTick();

  StringResponse GetRecords();

  /**
   * @brief
   * @return Token if can extruct
   */
  std::optional<model::Token> TryExtractToken();

  StringResponse ExecuteAuthorized(std::function<StringResponse(model::Player& player)> action);

  /**
   * @brief Create Response from
   *
   * @param status netork status
   * @param val json value
   * @param cache_control
   * @param allow
   * @return StringResponse
   */
  StringResponse MakeJsonResponse(const http::status& status,
                                  const json::value& val,
                                  std::string_view cache_control = "no-cache"sv,
                                  std::string_view allow = std::string_view());

  
};

}  // namespace http_handler

#endif  // __API_HANDLER_H__