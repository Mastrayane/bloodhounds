#pragma once
#ifndef __MODEL__LOOT_H__
#define __MODEL__LOOT_H__

#include <optional>
#include <string>
#include <compare>

#include "geometry.h"
#include "loot_type.h"
#include "game_object.h"

namespace model {

class Loot : public GameObject {
 public:
  Loot(const LootType& type, const Point2d& pos, unsigned id)
      : m_loot_type(type)
      , GameObject(pos)
      , m_id(id) {}

  const LootType& GetLootType() const noexcept { return m_loot_type; }
  unsigned GetId() const noexcept { return m_id; }

  bool operator==(const Loot& other) const {
    return m_id == other.m_id && m_loot_type.type_num == other.m_loot_type.type_num;
  }

  bool operator!=(const Loot& other) const {
    return !(*this == other);
  }

 private:
  const LootType m_loot_type;
  const unsigned m_id;
};

}  // namespace model

#endif  // __MODEL__LOOT_H__