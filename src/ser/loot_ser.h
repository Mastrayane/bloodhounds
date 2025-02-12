#ifndef __LOOT_SERIALIZATION_H__
#define __LOOT_SERIALIZATION_H__

#include "../model.h"

// depends
#include "loot_type_ser.h"
#include "game_object_ser.h"

namespace boost {
namespace serialization {

using namespace model;

template <typename Archive>
void save_construct_data(Archive& ar, const Loot* t, const unsigned int file_version) {
  Point2d pos = t->GetPosition();
  double width = t->GetWidth();
  LootType lt = t->GetLootType();
  unsigned id = t->GetId();

  ar << pos;
  ar << width;
  ar << lt;
  ar << id;
}

template <class Archive>
void load_construct_data(Archive& ar, Loot* t, const unsigned int file_version) {
  Point2d pos;
  double width;
  LootType lt;
  unsigned id;

  ar >> pos;
  ar >> width;
  ar >> lt;
  ar >> id;

  ::new (t) Loot(lt, pos, id);
  t->SetWidth(width);
}

}  // namespace serialization
}  // namespace boost

namespace model {

template <typename Archive>
void serialize(Archive& ar, Loot& loot, const unsigned version) {
  // ** note that this is empty **
}

}  // namespace model

#endif  // __LOOT_SERIALIZATION_H__