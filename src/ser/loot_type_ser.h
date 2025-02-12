#ifndef __LOOT_TYPE_SERIALIZATION_H__
#define __LOOT_TYPE_SERIALIZATION_H__

//#include <boost/serialization/optional.hpp>
#include "optional_ser.h"
#include "../model.h"

namespace model
{

/// LootType
template <typename Archive>
void serialize(Archive& ar, LootType& loot_type, [[maybe_unused]] const unsigned version) {
  ar& loot_type.name;
  ar& loot_type.file;
  ar& loot_type.type;
  ar& loot_type.rotation;
  ar& loot_type.color;
  ar& loot_type.scale;
  ar& loot_type.value;

  //! may be wrong behaviour becaus type_num depend on vector index
  ar& loot_type.type_num;
}
} // namespace model


#endif // __LOOT_TYPE_SERIALIZATION_H__