#ifndef __GAME_OBJECT_SERIALIZATION_H__
#define __GAME_OBJECT_SERIALIZATION_H__

#include "../model.h"
#include "point_ser.h"

namespace model {

/// Base Class GameObject
/// Splited function
template <typename Archive>
void save(Archive& ar, const GameObject& game_object, unsigned int version) {
  Point2d pos = game_object.GetPosition();
  double width = game_object.GetWidth();
  ar << pos;
  ar << width;
}

template <typename Archive>
void load(Archive& ar, GameObject& game_object, unsigned int version) {
  Point2d pos;
  double width;
  ar >> pos;
  ar >> width;
  game_object.SetPosition(pos);
  game_object.SetWidth(width);
}

template <class Archive>
void serialize(Archive& ar, GameObject& t, const unsigned int file_version) {
  boost::serialization::split_free(ar, t, file_version);
}

}  // namespace model

#endif  // __GAME_OBJECT_SERIALIZATION_H__