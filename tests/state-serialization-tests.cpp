#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/serialization/list.hpp>
#define BOOST_NO_MEMBER_TEMPLATE_FRIENDS
#include <boost/serialization/shared_ptr.hpp>
// #include <boost/serialization/unique_ptr.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <sstream>
#include <ranges>
#include <memory>

#include "../src/model.h"
#include "../src/model_serialization.h"
#include "../src/ser/player_ser.h"
#include "../src/ser/game_session_ser.h"

using namespace model;
using namespace std::literals;
namespace {

using InputArchive = boost::archive::text_iarchive;
using OutputArchive = boost::archive::text_oarchive;

struct Fixture {
  std::stringstream strm;
  OutputArchive output_archive{strm};
  Point2d point;
  LootType loot_type;

  Fixture() {
    loot_type = LootType{
        .name{"key"},
        .file{"assets/key.obj"},
        .type{"obj"},
        .rotation{std::nullopt},
        .color{"#338844"},
        .scale{0.03},
        .value{10},
        .type_num{1}};

    point = Point2d{10.5, 20.5};

    const size_t count_loot = 10;
    const size_t dog_id = 12;
    const double width = 0.6;
    const std::string token = "0123456789abcdef0123456789abcdef";
    const size_t points = 100;

    dog = std::make_shared<Dog>(Dog::Id(dog_id));
    dog->SetBagSize(count_loot);
    dog->SetPosition(point);
    dog->SetWidth(width);
    dog->SetToken(Token(token));
    dog->AddPoints(points);

    for (auto i = 0, x = 0, y = 0; i < count_loot; i++, x += 2, y += 3) {
      LootType lt = loot_type;
      lt.value = (i + 1) * 10;
      Loot loot(lt, Point2d{x, y}, i);
      loots.push_back(loot);

      dog->AddLoot(loot);
    }

    const std::string name = "player_one";
    const size_t pl_id = 50;
    player = std::make_shared<Player>(dog->GetToken(), name, Player::Id(pl_id));
    player->SetDog(dog);
  }

  std::list<Loot> loots;
  std::shared_ptr<Dog> dog;
  std::shared_ptr<Player> player;
};

struct GameObjectDerived : public model::GameObject {
  GameObjectDerived() : model::GameObject() {}
  template <typename Archive>
  void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
    ar& boost::serialization::base_object<model::GameObject>(*this);
  }
};

}  // namespace

SCENARIO_METHOD(Fixture, "Point serialization") {
  GIVEN("A point") {
    point = Point2d{10, 20};
    WHEN("point is serialized") {
      output_archive << point;

      THEN("it is equal to point after serialization") {
        InputArchive input_archive{strm};
        model::Point2d restored_point;
        input_archive >> restored_point;
        CHECK(point == restored_point);
      }
    }
  }
}

SCENARIO_METHOD(Fixture, "Loot & LootType serialization") {
  GIVEN("A LootType") {
    REQUIRE(loot_type != LootType());

    WHEN("LootType is serialized") {
      output_archive << loot_type;

      THEN("it is equal to loot_type after serialization") {
        InputArchive input_archive{strm};
        LootType restored_loot_type;
        REQUIRE(loot_type != restored_loot_type);
        input_archive >> restored_loot_type;
        CHECK(loot_type == restored_loot_type);
      }
    }

    AND_GIVEN("A Loot") {
      const unsigned id = 5;
      const Loot loot(loot_type, point, id);

      WHEN("Loot is serialised") {
        REQUIRE(loot.GetLootType() == loot_type);
        const Loot* ptr = &loot;
        output_archive << ptr;

        THEN("it is equal to loot after serialization") {
          InputArchive input_archive{strm};
          // Loot *restored_loot = static_cast<Loot*>(::operator new(sizeof(Loot)));
          Loot* restored_loot = nullptr;
          input_archive >> restored_loot;
          REQUIRE(restored_loot != nullptr);
          CHECK(loot == *restored_loot);
          CHECK(point == restored_loot->GetPosition());
          CHECK(id == restored_loot->GetId());
          if (restored_loot) {
            delete restored_loot;
          }
        }
      }
    }

    AND_GIVEN("List of loot") {
      REQUIRE(loots.size() != 0);

      WHEN("List Loot is serialised") {
        output_archive << loots;

        THEN("it is equal to every loot after serialization") {
          InputArchive input_archive{strm};
          std::list<Loot> restored_loots;
          input_archive >> restored_loots;
          REQUIRE(restored_loots.size() == loots.size());
          CHECK(std::equal(loots.cbegin(), loots.cend(), restored_loots.cbegin()));
        }
      }
    }
  }
}

SCENARIO_METHOD(Fixture, "GameObjectBase serialization") {
  GIVEN("Derive") {
    const model::Point2d pos{10, 20};
    const double width = 0.5;
    GameObjectDerived gobject;
    gobject.SetPosition(pos);
    gobject.SetWidth(width);
    WHEN("GameObject is serialized") {
      output_archive << gobject;

      THEN("it is equal to GameObject after serialization") {
        InputArchive input_archive{strm};
        GameObjectDerived restored_game_object;
        input_archive >> restored_game_object;
        CHECK(restored_game_object.GetPosition() == gobject.GetPosition());
        CHECK(restored_game_object.GetWidth() == gobject.GetWidth());
      }
    }
  }
}

SCENARIO_METHOD(Fixture, "Dog serialization") {
  GIVEN("A Dog") {

    REQUIRE(dog != nullptr);

    WHEN("Dog serialized") {
      output_archive << dog;

      THEN("it is equal to Dog after serialization") {
        InputArchive input_archive{strm};
        // Dog* restored_dog = nullptr;
        DogPtr restored_dog;
        input_archive >> restored_dog;
        CHECK(dog->GetId() == restored_dog->GetId());
        CHECK(dog->GetToken() == restored_dog->GetToken());
        CHECK(dog->GetScore() == restored_dog->GetScore());
        CHECK(dog->GetPosition() == restored_dog->GetPosition());
        CHECK(dog->GetWidth() == restored_dog->GetWidth());
        REQUIRE(dog->GetLoots().size() == restored_dog->GetLoots().size());
        CHECK(std::equal(dog->GetLoots().cbegin(), dog->GetLoots().cend(), restored_dog->GetLoots().cbegin()));
      }
    }
  }
}


SCENARIO_METHOD(Fixture, "Player serialization (whithout session)") {

  GIVEN("A Player") {
    REQUIRE(dog != nullptr);
    REQUIRE(player != nullptr);

    //? player->SetSession();
    WHEN("Player serialize") {
      output_archive << player;

      THEN("it is equal to Player after serialization") {
        InputArchive input_archive{strm};
        std::shared_ptr<Player> restored_player;
        input_archive >> restored_player;
        CHECK(player->GetId() == restored_player->GetId());
        CHECK(player->GetName() == restored_player->GetName());
        CHECK(player->GetToken() == restored_player->GetToken());

        auto r_dog = restored_player->GetDog();

        CHECK(dog->GetId() == r_dog->GetId());
        CHECK(dog->GetToken() == r_dog->GetToken());
      }
    }
  }
}
