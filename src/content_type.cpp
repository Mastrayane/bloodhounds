#include "content_type.h"
#include <initializer_list>
#include <string>

namespace http_handler {

using namespace std::literals;

const std::unordered_map<std::string, std::string> ContentType::DICT = {
    {".htm", "text/html"},
    {".html", "text/html"},
    {"html", "text/html"},
    {".css", "text/css"},
    {".txt", "text/plain"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {"json", "application/json"},
    {".xml", "application/xml"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpe", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {".bmp", "image/bmp"},
    {".ico", "image/vnd.microsoft.icon"},
    {".tiff", "image/tiff"},
    {".tif", "image/tiff"},
    {".svg", "image/svg+xml"},
    {".svgz", "image/svg+xml"},
    {".mp3", "audio/mpeg"},
};

}  // namespace http_handler