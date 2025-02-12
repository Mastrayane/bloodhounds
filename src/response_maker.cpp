#include "response_maker.h"

namespace http_handler {


StringResponse MakeResponse(http::status status, std::string_view text, unsigned http_version,
                            bool keep_alive,
                            std::string_view content_type,
                            std::string_view cache_control,
                            std::string_view allow) {
  StringResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = text;
  response.prepare_payload();
  response.keep_alive(keep_alive);
  if (!cache_control.empty()) {
    response.set(http::field::cache_control, cache_control);
  }
  if (!allow.empty()) {
    response.set(http::field::allow, allow);
  }
  return response;
}

FileResponse MakeResponse(http::status status, http::file_body::value_type& file,
                          unsigned http_version,
                          bool keep_alive,
                          std::string_view content_type,
                          std::string_view cache_control) {
  FileResponse response(status, http_version);
  response.set(http::field::content_type, content_type);
  response.body() = std::move(file);
  response.prepare_payload();
  response.keep_alive(keep_alive);
  if (!cache_control.empty()) {
    response.set(http::field::cache_control, cache_control);
  }
  return response;
}

}