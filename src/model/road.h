#ifndef __ROAD_H__
#define __ROAD_H__

#include <utility>

#include "geometry.h"
#include "../tagged.h"

namespace model {

class Road {
  struct HorizontalTag {
    HorizontalTag() = default;
  };

  struct VerticalTag {
    VerticalTag() = default;
  };

 public:
  constexpr static HorizontalTag HORIZONTAL{};
  constexpr static VerticalTag VERTICAL{};

  Road(HorizontalTag, Point2i start, int end_x) noexcept
      : start_{start},
        end_{end_x, start.y},
        m_min_X(std::min(start.x, end_x) - Road::HALF_WIDTH),
        m_max_X(std::max(start.x, end_x) + Road::HALF_WIDTH),
        m_min_Y(start.y - Road::HALF_WIDTH),
        m_max_Y(start.y + Road::HALF_WIDTH) {}

  Road(VerticalTag, Point2i start, int end_y) noexcept
      : start_{start},
        end_{start.x, end_y},
        m_min_X(start.x - Road::HALF_WIDTH),
        m_max_X(start.x + Road::HALF_WIDTH),
        m_min_Y(std::min(start.y, end_y) - Road::HALF_WIDTH),
        m_max_Y(std::max(start.y, end_y) + Road::HALF_WIDTH) {}

  bool IsHorizontal() const noexcept { return start_.y == end_.y; }

  bool IsVertical() const noexcept { return start_.x == end_.x; }

  Point2i GetStart() const noexcept { return start_; }

  Point2i GetEnd() const noexcept { return end_; }

  bool Contains(const Point2d& point) const noexcept {
    return (point.x >= m_min_X) && (point.x <= m_max_X) && (point.y >= m_min_Y) && (point.y <= m_max_Y);
  }

  const double& GetMinX() const noexcept { return m_min_X; }
  const double& GetMaxX() const noexcept { return m_max_X; }
  const double& GetMinY() const noexcept { return m_min_Y; }
  const double& GetMaxY() const noexcept { return m_max_Y; }

  static constexpr double HALF_WIDTH = 0.4;

 private:
  const Point2i start_;
  const Point2i end_;

  const double m_min_X;
  const double m_max_X;
  const double m_min_Y;
  const double m_max_Y;
};

}  // namespace model

#endif  // __ROAD_H__