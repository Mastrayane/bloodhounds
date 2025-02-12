#ifndef __OFFICE_H__
#define __OFFICE_H__

#include <string>

#include "geometry.h"
#include "../tagged.h"
#include "game_object.h"

namespace model {
class Office : public GameObject {
 public:
  using Id = util::Tagged<std::string, Office>;

  Office(Id const& id, Point2i const& position, Point2i const& offset, double const& width = 0.5) noexcept
      : m_id(id),
        GameObject(position, width),
        m_offset{offset} {}

  const Id& GetId() const noexcept { return m_id; }

  Point2i GetOffset() const noexcept { return m_offset; }

 private:
  const Id m_id;
  const Point2i m_offset;
};

}  // namespace model

#endif  // __OFFICE_H__