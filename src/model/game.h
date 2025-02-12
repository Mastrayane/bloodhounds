#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

#include "game_session.h"

namespace model {

class Game {
 public:
  using Maps = std::vector<Map>;
  using SessPtr = std::shared_ptr<GameSession>;
  using SessPtrList = std::vector<SessPtr>;


  void AddMap(Map map);

  const Maps& GetMaps() const noexcept { return maps_; }

  std::optional<std::reference_wrapper<const Map>> FindMap(const Map::Id& id) const noexcept {
    if (auto it = map_id_to_index_.find(id); it != map_id_to_index_.end()) {
      return maps_.at(it->second);
    }
    return std::nullopt;
  }

  void SetDefaultSpeed(double speed) { m_default_speed = speed; }
  double GetDefaultSpeed() const { return m_default_speed; }

  // get exist or create new & return pointer
  SessPtr GetSession(const model::Map::Id& id);

  void Update(std::chrono::milliseconds delta_time);

  void SetRandomSpawn(bool enable = true);
  bool IsRandomSpawn() const;

  void SetLootGeneratorConfig(double period, double probability);

  void SetDefaultBagCapacity(size_t size);
  size_t GetDefaultBagCapacity() const noexcept;


  void SetDogRetirementTime(double seconds);
  void SetDogRetirementTime(std::chrono::milliseconds ret_time);

  const SessPtrList& GetSessionList() const;

 private:
  using MapIdHasher = util::TaggedHasher<Map::Id>;
  using MapIdToIndex = std::unordered_map<Map::Id, size_t, MapIdHasher>;

  std::vector<Map> maps_;
  MapIdToIndex map_id_to_index_;

  SessPtrList m_sess;

  double m_default_speed = 1.0;

  bool m_random_dog_spawn = false;

  double m_period_loot_gen = 0.0;
  double m_probability_loot_gen = 0.0;
  size_t m_default_bag_capacity = 3;

  std::chrono::milliseconds m_dog_retirement_time = 60000ms;
};

}  // namespace model

#endif  // __GAME_H__