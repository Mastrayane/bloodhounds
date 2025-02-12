#include "game_object.h"

namespace model {

void GameObject::SetPosition(Point2d const& position) {
  m_position = position;
}

const Point2d& GameObject::GetPosition() const noexcept {
  return m_position;
}

void GameObject::SetWidth(double width) {
  m_width = width;
}

double GameObject::GetWidth() const noexcept {
  return m_width;
}

GameObject::GameObject(Point2d const& pos, double width) : m_position(pos), m_width(width) {
}

}  // namespace model