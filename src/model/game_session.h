#ifndef __MODEL_GSES_H__
#define __MODEL_GSES_H__

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <list>

#include "../tagged.h"
#include "dog.h"
#include "model_map.h"
#include "loot_generator.h"
#include "loot.h"

namespace model {

class GameSession {
 public:

  using Id = util::Tagged<uint32_t, GameSession>;
  using DogPtrList = std::list<DogWeakPtr>;

  explicit GameSession(/*Id id,*/ const Map& map, LootGenerator gen);
  ~GameSession();

  Id GetId() const noexcept { return m_id; } 

  const Map& GetMap() const noexcept;

  void AddDog(DogPtr dog);

  void Update(std::chrono::milliseconds delta_time);

  void SetDogRandomSpawn(bool enable = true);

  void AddLoot(Loot&& loot) { m_loots.emplace_back(std::move(loot)); }

  const std::list<Loot>& GetLoots() const noexcept { return m_loots;}
  //const std::list<std::shared_ptr<GameObject>>& GetObjects() const noexcept { return m_objects;}

  // const DogPtrList& GetDogs() const noexcept { return m_dogs; }

  void SetRetirementTime(std::chrono::milliseconds downtime);
  std::chrono::milliseconds GetRetirementTime() const noexcept;

 private:

  void GenerateLoot(std::chrono::milliseconds delta_time);

  void DogsUpdate(std::chrono::milliseconds delta_time);

  Id m_id;
  const Map& m_map;
  DogPtrList m_dogs;
  bool m_random_spawn;

  LootGenerator m_loot_gen;

  std::list<Loot> m_loots;

  int m_loot_id = 0;

  std::chrono::milliseconds m_retirement_time;
};

}  // namespace model

#endif  // __MODEL_GSES_H__