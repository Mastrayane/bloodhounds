#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__

#include <set>
#include <list>
#include <vector>
#include <memory>
#include <variant>

#include "loot.h"
#include "dog.h"
#include "geometry.h"
#include "office.h"

namespace model {

struct DogMove {
  DogPtr dog;
  Point2d nextPos;
  void UpdateMove() {
    dog->SetPosition(nextPos);
  }
};

struct CollisionResult {
  bool IsIntersect(double collect_radius) const {
    return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
  }

  // квадрат расстояния до точки
  double sq_distance;

  // доля пройденного отрезка
  double proj_ratio;
};

struct CollisionEvent {
  DogPtr dog{nullptr};
  std::variant<Loot, Office> game_object;  // maybe need replace to std::shared_ptr<GameObject>
  double sq_distance{.0};
  double time{.0};
};

CollisionResult TryCollectPoint(Point2d a, Point2d b, Point2d c);

class Collector {
 public:
  Collector(std::list<Loot>& loots, const std::vector<Office>& offices)
      : m_loots(loots), m_offices(offices) {}

  void AddDogToMoveUpdate(DogPtr dog, const Point2d& nextPos) {
    m_dogs.emplace_back(dog, nextPos);
  }

  void Update() {
    CollisionEventHandler();

    for (auto& dog : m_dogs) {
      dog.UpdateMove();
    }
  }

 private:
  std::list<CollisionEvent> FindEvent() const;

  void CollisionEventHandler();

  std::list<Loot>& m_loots;
  const std::vector<Office>& m_offices;
  std::vector<DogMove> m_dogs;
};

}  // namespace model

#endif  // __COLLECTOR_H__