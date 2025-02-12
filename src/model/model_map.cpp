#include "model_map.h"

#include <random>

namespace model {

using namespace std::literals;

void Map::AddOffice(Office office) {
  if (warehouse_id_to_index_.contains(office.GetId())) {
    throw std::invalid_argument("Duplicate warehouse");
  }

  const size_t index = offices_.size();
  Office& offc = offices_.emplace_back(std::move(office));
  try {
    warehouse_id_to_index_.emplace(offc.GetId(), index);
  } catch (const std::exception&) {
    // Удаляем офис из вектора, если не удалось вставить в unordered_map
    offices_.pop_back();
    throw;
  }
}

void Map::AddLootType(LootType loot) {
  loot.type_num = m_loot_types.size();
  m_loot_types.push_back(loot);
}

Point2d Map::GetRandPoint(bool enable) const {
  if (roads_.size() == 0) {
    throw std::logic_error("Must be one or mode roads");
  }

  // preset to first road in start position
  Point2d ret = roads_.at(0).GetStart();

  // random position turn off
  if (!enable) {
    return ret;
  }

  static std::random_device rd;
  std::mt19937 gen{rd()};

  // random number of road
  int num_r = 0;
  if (roads_.size() > 1) {
    std::uniform_int_distribution<int> dist(0, roads_.size() - 1);
    num_r = dist(gen);
  }

  const Road& road = roads_.at(num_r);
  const Point2i& start = road.GetStart();
  const Point2i& end = road.GetEnd();

  ret = road.GetStart();

  // random by wide has no tested
  // double width;
  // {
  //   std::uniform_real_distribution<> dist(-Road::HALF_WIDTH, Road::HALF_WIDTH);
  //   width = dist(gen);
  //   width = ((int)(width * 100 + .5) / 100.0);  // round two digit
  // }

  if (road.IsHorizontal()) {
    std::uniform_real_distribution<> dist(start.x, end.x);
    ret.x = dist(gen);
    // ret.y += width;
  } else {
    std::uniform_real_distribution<> dist(start.y, end.y);
    ret.y = dist(gen);
    // ret.x += width;
  }

  ret.x = (static_cast<int>(ret.x * 100 + .5) / 100.0);  // round two digit
  ret.y = (static_cast<int>(ret.y * 100 + .5) / 100.0);  // round two digit

  return ret;
}

void Map::SetDogSpeed(double speed) {
  m_dog_speed = speed;
}

double Map::GetDogSpeed() const noexcept {
  return m_dog_speed;
}

void Map::SetBagCapacity(size_t size) {
  m_bag_capacity = size;
}

size_t Map::GetBagCapacity() const noexcept {
  return m_bag_capacity;
}

}  // namespace model