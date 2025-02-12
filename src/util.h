#ifndef __UTIL_H__
#define __UTIL_H__

#include <filesystem>
namespace util {
namespace fs = std::filesystem;

/**
 * @brief Check path inside base
 *
 * @return true if path inside base
 */
bool IsSubPath(fs::path path, fs::path base);

/**
 * @brief decode url path to ascii string
 *
 * @param url encoded utl path
 * @return std::string decoded url path
 */
std::string UrlDecode(const std::string &url);

};      // namespace util
#endif  // __UTIL_H__