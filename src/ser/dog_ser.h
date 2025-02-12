#ifndef __DOG_SER_H__
#define __DOG_SER_H__

#include "../model.h"
#include "tag_ser.h"
#include "point_ser.h"
#include "loot_ser.h"

namespace boost::serialization
{
  using namespace model;

template <typename Archive>
void save_construct_data(Archive& ar, const Dog* t, const unsigned int file_version) {

  auto id = *t->GetId();
  auto token = *t->GetToken();
  ar << id;
  ar << token;
  ar << t->GetScore();
  ar << t->GetPosition();
  ar << t->GetWidth();
  ar << t->GetBagSize();
  ar << t->GetLoots();
  
}

template <typename Archive>
void load_construct_data(Archive& ar, Dog* t, const unsigned int file_version) {
  Dog::Id::ValueType id;
  Token::ValueType token;
  int score;
  Point2d pos;
  double width;
  size_t bag_size;
  std::list<Loot> loots;

  ar >> id;
  ar >> token;
  ar >> score;
  ar >> pos;
  ar >> width;
  ar >> bag_size;
  ar >> loots;

  ::new (t) Dog(Dog::Id(id));
  t->SetToken(Token(token));
  t->AddPoints(score);
  t->SetPosition(pos);
  t->SetWidth(width);
  t->SetBagSize(bag_size);

  for(auto loot : loots) {
    t->AddLoot(loot);
  }

}

template <typename Archive>
void serialize(Archive& ar, Dog& dog, const unsigned version) {
  // ** note that this is empty **
}

} // namespace model
#endif // __DOG_SER_H__