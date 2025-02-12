#ifndef __PLAYER_SER_H__
#define __PLAYER_SER_H__

#include "../model.h"
#include "dog_ser.h"

namespace boost::serialization {
  using namespace model;

template <typename Archive>
void save_construct_data(Archive& ar, const Player* ptr, const unsigned int file_version) {
  const std::string token = *ptr->GetToken();
  const std::string name = ptr->GetName();
  const Player::Id::ValueType id = *ptr->GetId();
  DogConstPtr dog = ptr->GetDog();

  ar << token;
  ar << name;
  ar << id;
  ar << dog;

}

template <typename Archive>
void load_construct_data(Archive& ar, Player* ptr, const unsigned int file_version) {
  std::string token;
  std::string name;
  Player::Id::ValueType id;
  DogPtr dog;

  ar >> token;
  ar >> name;
  ar >> id;
  ar >> dog;
  
  ::new (ptr) Player(Token(token), name, Player::Id(id));

  ptr->SetDog(dog);
}

template <typename Archive>
void serialize(Archive& ar, Player& player, const unsigned version) {
  // ** note that this is empty **
}


}


#endif // __PLAYER_SER_H__