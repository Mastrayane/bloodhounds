#include "api_handler.h"

#include <boost/url.hpp>

#include <cassert>

#include "content_type.h"
#include "logger.h"
#include "util.h"

namespace http_handler {

    json::value JsAnswer(std::string code, std::string message) {
        return json::object{ {"code", code}, {"message", message} };
    }

    ApiHandler::ApiHandler(Application& app, const StringRequest& req)
        : m_app(app),
        m_req(req),
        m_target(util::UrlDecode(static_cast<std::string>(req.target()))) {
        if (!m_target.starts_with("/api/")) {
            throw std::invalid_argument("Request target for ApiHandler should starts with \"api\" but target =="s + m_target);
        }
    }

    StringResponse ApiHandler::Response() {
        if (m_target.starts_with(ApiKey::Maps)) {
            return MapRequest();
        }
        else if (m_target == ApiKey::Join) {
            return PlayerJoinRequest();
        }
        else if (m_target == ApiKey::Players) {
            return PlayerListRequest();
        }
        else if (m_target == ApiKey::State) {
            return GetGameState();
        }
        else if (m_target == ApiKey::Action) {
            return PostAction();
        }
        else if (m_target == ApiKey::Tick) {
            return PostTick();
        }
        else if (m_target.starts_with(ApiKey::Records)) {
            return GetRecords();
        }
        else {
            return MakeJsonResponse(http::status::bad_request, JsAnswer("badRequest", "Bad request"));
        }
    }

    StringResponse ApiHandler::MapRequest() {
        // check method
        if (m_req.method() != http::verb::get && m_req.method() != http::verb::head) {
            return MakeJsonResponse(http::status::method_not_allowed, JsAnswer("invalidMethod", "Only GET method is expected"), CacheControl::NO_CACHE, "GET, HEAD"sv);
        }

        // maps array request
        if (m_target == ApiKey::Maps) {
            json::array arr{};
            for (const auto& map : m_app.GetMaps()) {
                json::value v = { {"id", *map.GetId()}, {"name", map.GetName()} };
                arr.push_back(v);
            }
            return MakeJsonResponse(http::status::ok, arr, CacheControl::NO_CACHE);
        }

        // map by name request
        if (m_target.starts_with(ApiKey::MapsPrefix)) {
            const std::string map_name = m_target.substr(ApiKey::MapsPrefix.size());
            auto map = m_app.FindMap(model::Map::Id(map_name));
            if (map) {
                return MakeJsonResponse(http::status::ok, json::value_from(*map), CacheControl::NO_CACHE);
            }

            return MakeJsonResponse(http::status::not_found, JsAnswer("mapNotFound", "Map not found"));
        }

        // fail map request
        return MakeJsonResponse(http::status::bad_request, JsAnswer("badRequest", "Bad request"));
    }

    StringResponse ApiHandler::PlayerJoinRequest() {
        // check method
        if (m_req.method() != http::verb::post) {
            return MakeJsonResponse(http::status::method_not_allowed, JsAnswer("invalidMethod", "Only POST method is expected"), CacheControl::NO_CACHE, "POST"sv);
        }

        std::string user_name{};
        std::string map_id{};

        // check parsing
        try {
            boost::json::value jv = boost::json::parse(m_req.body());
            user_name = jv.as_object().at("userName").as_string();
            map_id = jv.as_object().at("mapId").as_string();
        }
        catch (const std::exception&) {
            return MakeJsonResponse(http::status::bad_request, JsAnswer("invalidArgument", "Join game request parse error"), CacheControl::NO_CACHE);
        }

        // check if userName is empty
        if (user_name.empty()) {
            return MakeJsonResponse(http::status::bad_request, JsAnswer("invalidArgument", "Invalid name"), CacheControl::NO_CACHE);
        }

        // check map id exist
        if (auto map = m_app.FindMap(model::Map::Id(map_id)); map == std::nullopt) {
            return MakeJsonResponse(http::status::not_found, JsAnswer("mapNotFound", "Map not found"), CacheControl::NO_CACHE);
        }

        boost::json::object js_response{};
        const model::Player& player = m_app.JoinGame(model::Map::Id(map_id), user_name);
        js_response = {
            {"authToken", *player.GetToken()},
            {"playerId", *player.GetId()} };

        return MakeJsonResponse(http::status::ok, js_response, CacheControl::NO_CACHE);
    }

    StringResponse ApiHandler::PlayerListRequest() {
        bool isGetOrHead = (m_req.method() == http::verb::get || m_req.method() == http::verb::head);
        if (!isGetOrHead) {
            return MakeJsonResponse(http::status::method_not_allowed,
                JsAnswer("invalidMethod", "Invalid method"),
                CacheControl::NO_CACHE,
                "GET, HEAD"sv);
        }

        return ExecuteAuthorized([this](model::Player& p) {
            boost::json::object js_response{};
            for (auto it = m_app.GetPlayers().cbegin(); it != m_app.GetPlayers().cend(); ++it) {
                if (it->second.GetSession() == p.GetSession())
                    js_response[std::to_string(*it->second.GetId())] = { {"name", it->second.GetName()} };
            }

            if (js_response.empty()) {
                throw std::logic_error("Response can not be empty in Player List request");
            }

            return MakeJsonResponse(http::status::ok,
                js_response,
                CacheControl::NO_CACHE); });
    }

    StringResponse ApiHandler::GetGameState() {
        bool isGetOrHead = (m_req.method() == http::verb::get || m_req.method() == http::verb::head);
        if (!isGetOrHead) {
            return MakeJsonResponse(http::status::method_not_allowed,
                JsAnswer("invalidMethod", "Invalid method"),
                CacheControl::NO_CACHE,
                "GET, HEAD"sv);
        }

        return ExecuteAuthorized([this](model::Player& player) {
            boost::json::object js_response{};
            boost::json::object js_players{};  // part of response
            boost::json::object js_lost_loots{};  // part of response

            // get list of players  //! mayby concurency
            for (auto it = m_app.GetPlayers().cbegin(); it != m_app.GetPlayers().cend(); ++it) {
                if (it->second.GetSession() == player.GetSession()) {
                    assert(it->second.GetDog() != nullptr);
                    js_players[std::to_string(*it->second.GetId())] = boost::json::value_from(*it->second.GetDog());
                }
            }

            // get list of loot
            const auto& sess = player.GetSession();
            for (const auto& loot : sess->GetLoots()) {
                js_lost_loots[std::to_string(loot.GetId())] = { {"type", loot.GetLootType().type_num}, {
                  "pos", {loot.GetPosition().x, loot.GetPosition().y}
                } };
            }

            // formation of the response
            js_response["players"] = js_players;
            js_response["lostObjects"] = js_lost_loots;

            return MakeJsonResponse(http::status::ok,
                js_response,
                CacheControl::NO_CACHE); });
    }

    StringResponse ApiHandler::PostAction() {
        // check method
        if (m_req.method() != http::verb::post) {
            return MakeJsonResponse(http::status::method_not_allowed,
                JsAnswer("invalidMethod", "Invalid method"),
                CacheControl::NO_CACHE,
                "POST"sv);
        }

        if (!m_req.count(http::field::content_type) || m_req.at(http::field::content_type) != ContentType::APP_JSON) {
            return MakeJsonResponse(http::status::unauthorized,
                JsAnswer("invalidArgument", "Invalid content type"),
                CacheControl::NO_CACHE);
        }

        return ExecuteAuthorized([this](model::Player& p) {
            // check parsing
            try {
                boost::json::value jv = boost::json::parse(m_req.body());
                p.GetDog()->SetDir(static_cast<std::string>(jv.as_object().at("move").as_string()));
            }
            catch (const std::exception&) {
                return MakeJsonResponse(http::status::bad_request, JsAnswer("invalidArgument", "Failed to parse action"), CacheControl::NO_CACHE);
            }

            // empty response
            return MakeJsonResponse(http::status::ok,
                json::object(),
                CacheControl::NO_CACHE); });
    }

    StringResponse ApiHandler::PostTick() {
        // check method
        if (m_req.method() != http::verb::post) {
            return MakeJsonResponse(http::status::method_not_allowed, JsAnswer("invalidMethod", "Only POST method is expected"), CacheControl::NO_CACHE, "POST"sv);
        }

        int64_t ms = 0;
        // check parsing
        try {
            boost::json::value jv = boost::json::parse(m_req.body());
            ms = jv.as_object().at("timeDelta").as_int64();
        }
        catch (const std::exception& e) {
            return MakeJsonResponse(http::status::bad_request, JsAnswer("invalidArgument", "Failed to parse tick request JSON "s + e.what()), CacheControl::NO_CACHE);
        }

        if (m_app.IsManualTicker()) {
            try {
                m_app.Update(std::chrono::milliseconds(ms));
                m_app.SaveState();
            }
            catch (const std::exception& e) {
                Logger::LogExit(e);
                throw;
            }
        }

        // empty response
        return MakeJsonResponse(http::status::ok,
            json::object(),
            CacheControl::NO_CACHE);
    }

    StringResponse ApiHandler::GetRecords() {
        // check method
        if (!(m_req.method() == http::verb::get || m_req.method() == http::verb::head)) {
            return MakeJsonResponse(http::status::method_not_allowed,
                JsAnswer("invalidMethod", "Invalid method"),
                CacheControl::NO_CACHE,
                "GET, HEAD"sv);
        }

        // def
        size_t start{ 0 };
        size_t max_items{ dbconn::ConnectionFactory::MaxItemReq };

        // url parse
        boost::urls::url_view url = boost::urls::parse_relative_ref(m_target).value();
        if (url.has_query()) {
            boost::urls::params_view params_ref = url.params();
            for (const auto& v : params_ref) {
                try {
                    if (v.key == "start") {
                        start = std::stoul(v.value);
                    }
                    else if (v.key == "maxItems") {
                        max_items = std::stoul(v.value);
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    throw;
                }
            }
        }

        // check max_items value
        if (max_items > dbconn::ConnectionFactory::MaxItemReq) {
            return MakeJsonResponse(http::status::bad_request,
                JsAnswer("invalidArgument",
                    "maxItem request more than"s + std::to_string(dbconn::ConnectionFactory::MaxItemReq)),
                CacheControl::NO_CACHE);
        }

        auto players = m_app.GetPlayerInfoList(start, max_items);

        const boost::json::value jv_response = boost::json::value_from(std::move(players));

        return MakeJsonResponse(http::status::ok, jv_response, CacheControl::NO_CACHE);
    }

    std::optional<model::Token> ApiHandler::TryExtractToken() {
        std::string auth{};  // for return value

        // check field
        if (m_req.count(http::field::authorization)) {
            auth = m_req.at(http::field::authorization);
        }
        else {
            return std::nullopt;
        }

        // check type Authorization
        const std::string scode = "Bearer ";
        if (auth.starts_with(scode)) {
            auth = auth.substr(scode.size());
        }
        else {
            return std::nullopt;
        }

        // check is hex simbols
        if (!std::all_of(auth.begin(), auth.end(), ::isxdigit)) {
            return std::nullopt;
        }

        // check length token
        if (auth.size() != model::TOKEN_LENGHT) {
            return std::nullopt;
        }

        // to lower
        std::transform(auth.begin(), auth.end(), auth.begin(), [](unsigned char c) { return std::tolower(c); });
        return model::Token(auth);
    }

    StringResponse ApiHandler::MakeJsonResponse(const http::status& status,
        const json::value& val,
        std::string_view cache_control,
        std::string_view allow) {
        StringResponse response(status, m_req.version());
        response.set(http::field::content_type, ContentType::APP_JSON);
        response.body() = json::serialize(val);
        response.prepare_payload();
        response.keep_alive(m_req.keep_alive());
        // if (!cache_control.empty()) {
        response.set(http::field::cache_control, cache_control);
        //}
        // if (!allow.empty()) {
        response.set(http::field::allow, allow);
        //}
        return response;
    }

    StringResponse ApiHandler::ExecuteAuthorized(std::function<StringResponse(model::Player& player)> action) {
        // 

        if (auto token = this->TryExtractToken()) {
            auto player = m_app.FindPlayer(*token);
            if (player == std::nullopt) {
                return MakeJsonResponse(http::status::unauthorized,
                    JsAnswer("unknownToken", "Player token has not been found"),
                    CacheControl::NO_CACHE);
            }

            return action(*player);
        }
        else {
            return MakeJsonResponse(http::status::unauthorized,
                JsAnswer("invalidToken", "Authorization header is missing"),
                CacheControl::NO_CACHE);
        }
    }

}  // namespace http_handler