#ifndef __LOOT_TYPE_H__
#define __LOOT_TYPE_H__

#include <compare>
#include <optional>
#include <string>

namespace model {
struct LootType {
  std::string name{};   // example -> = "key";
  std::string file{};   // example -> = "assets/key.obj";
  std::string type{};   // example -> = "obj";
  std::optional<int> rotation = std::nullopt;       // example -> = 90;
  std::optional<std::string> color = std::nullopt;  // example -> = "#338844";
  std::optional<double> scale = std::nullopt;       // example -> = 0.03;
  size_t value{0};

  /// index type (for compatibility)
  int type_num{-1};

  auto operator<=>(const LootType&) const = default;
};

}  // namespace model

#endif  // __LOOT_TYPE_H__