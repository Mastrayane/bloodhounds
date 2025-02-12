#include "util.h"

namespace util {

bool IsSubPath(fs::path path, fs::path base) {
  // Приводим оба пути к каноничному виду (без . и ..)
  path = fs::weakly_canonical(path);
  base = fs::weakly_canonical(base);

  // Проверяем, что все компоненты base содержатся внутри path
  for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
    if (p == path.end() || *p != *b) {
      return false;
    }
  }
  return true;
}

std::string UrlDecode(const std::string& url) {
    std::string str;
    str.reserve(url.size());
    for (size_t i = 0; i < url.size(); ++i) {
        if (url[i] == '%' && i + 2 < url.size()) {
            const std::string r = url.substr(i + 1, 2);
            uint64_t value = std::stoull(r, nullptr, 16);
            if (value <= static_cast<uint64_t>(std::numeric_limits<char>::max())) {
                str.push_back(static_cast<char>(value));
            }
            else {
                // Обработка ошибки: значение выходит за пределы char
                throw std::invalid_argument("Invalid URL encoding: value out of range for char");
            }
            i += 2;
            continue;
        }
        else if (url[i] == '+') {
            str.push_back(' ');
        }
        else {
            str.push_back(url[i]);
        }
    }
    return str;
}

};  // namespace util