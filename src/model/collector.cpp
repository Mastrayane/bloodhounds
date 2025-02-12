#include "collector.h"

namespace model {

CollisionResult TryCollectPoint(Point2d a, Point2d b, Point2d c) {
  // Проверим, что перемещение ненулевое.
  // Тут приходится использовать строгое равенство, а не приближённое,
  // пскольку при сборе заказов придётся учитывать перемещение даже на небольшое
  // расстояние.
  assert(b.x != a.x || b.y != a.y);
  const double u_x = c.x - a.x;
  const double u_y = c.y - a.y;
  const double v_x = b.x - a.x;
  const double v_y = b.y - a.y;
  const double u_dot_v = u_x * v_x + u_y * v_y;
  const double u_len2 = u_x * u_x + u_y * u_y;
  const double v_len2 = v_x * v_x + v_y * v_y;
  const double proj_ratio = u_dot_v / v_len2;
  const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

  return CollisionResult(sq_distance, proj_ratio);
}

std::list<CollisionEvent> Collector::FindEvent() const {
  std::list<CollisionEvent> event;
  /**/
  for (auto& dogmove : m_dogs) {
    // handle loot collision
    for (const auto& loot : m_loots) {
      const auto col_result = TryCollectPoint(dogmove.dog->GetPosition(), dogmove.nextPos, loot.GetPosition());
      if (col_result.IsIntersect(dogmove.dog->GetWidth() + loot.GetWidth())) {
        event.emplace_back(dogmove.dog, loot, col_result.sq_distance, col_result.proj_ratio);
      }
    }

    // handle office collision
    for (const auto& office : m_offices) {
      auto col_result = TryCollectPoint(dogmove.dog->GetPosition(), dogmove.nextPos, office.GetPosition());
      if (col_result.IsIntersect(dogmove.dog->GetWidth() + office.GetWidth())) {
        event.emplace_back(dogmove.dog, office, col_result.sq_distance, col_result.proj_ratio);
      }
    }
  }
  /**/
  event.sort([](const CollisionEvent& lhs, const CollisionEvent& rhs) { return lhs.time < rhs.time; });

  return event;
}

void Collector::CollisionEventHandler() {
  std::list<CollisionEvent> event = FindEvent();

  for (auto it = event.begin(); it != event.end(); ++it) {
    // if Office need unload and calculte points
    if (std::holds_alternative<Office>(it->game_object)) {
      // get loots (extruct loot from bag)
      auto loots = it->dog->UnloadLoots();

      // add poits when unload loot 
      for (const auto& loot : loots) {
        it->dog->AddPoints(loot.GetLootType().value);
      }
    } // if loot -  collect loot and erase samevent
    else if (std::holds_alternative<Loot>(it->game_object) && !it->dog->IsFull()) {
      const auto& loot = std::get<Loot>(it->game_object);

      // collect loot
      it->dog->AddLoot(loot);

      // remove collected loots from game session list
      m_loots.remove(loot);

      // remove if next event has same loot
      auto next = std::next(it);
      while (next != event.end()) {
        if (std::holds_alternative<Loot>(it->game_object) && std::get<Loot>(it->game_object) == loot) {
          next = event.erase(next);
        }
        else {
          ++next;
        }
      }
    }
  }
}

}  // namespace model
