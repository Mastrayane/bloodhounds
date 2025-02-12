#ifndef __BUILDING_H__
#define __BUILDING_H__

#include <string>

#include "geometry.h"
#include "../tagged.h"

namespace model {

class Building {
 public:
  explicit Building(RectInt bounds) noexcept : m_bounds{bounds} {}

  const RectInt& GetBounds() const noexcept { return m_bounds; }

 private:
  RectInt m_bounds;
};

}  // namespace model

#endif  // __BUILDING_H__