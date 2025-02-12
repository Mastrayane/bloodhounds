#ifndef __GAME_SESSION_SER_H__
#define __GAME_SESSION_SER_H__

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#define BOOST_NO_MEMBER_TEMPLATE_FRIENDS

#include <boost/serialization/shared_ptr.hpp>

#include <map>
#include <string>
#include <list>

#include "../model.h"
#include "loot_ser.h"
#include "player_ser.h"

namespace model {

class GameSessionSer {
 public:
  GameSessionSer() = default;

  explicit GameSessionSer(const Game::SessPtrList& sess_list, const PlayerList::Container& players)
      : m_sess_list(sess_list) {
    // player collection as std::vector
    for(const auto& [ _, player] : players) {
      m_players.emplace_back(player);
    }

    // map ids vector 
    for (const auto& player : m_players) {
      map_id.push_back(*player.GetSession()->GetMap().GetId());
    }
    
    // free loots (uncollected loots)
    for(const auto& sess : m_sess_list){
      m_loots.insert( std::pair<std::string, std::list<Loot>>(*sess->GetMap().GetId(), sess->GetLoots()));
    }

    // requre same size
    if (map_id.size() != players.size()) {
      throw std::logic_error("serialization logic error");
    }
  }

  template <class Archive>
  void serialize(Archive& ar, const unsigned int file_version) {
    ar & map_id;
    ar & m_players;
    ar & m_loots;
  }

  void UpdateGame(Game& game, PlayerList& plist) {
    assert(map_id.size() == m_players.size());

    // tempolary save random status
    const bool b_random = game.IsRandomSpawn();
    game.SetRandomSpawn(false);

    // restore player list & gamesession
    for (size_t i = 0; i < m_players.size(); ++i) {
      auto sess = game.GetSession(Map::Id(map_id[i]));
      m_players[i].SetSession(sess);
      plist.AddPlayer(std::move(m_players[i]));
    }

    // restore uncollected loots
    for (auto [id, loot_list] : m_loots) {
      auto sess = game.GetSession(Map::Id(id));
      for (auto& loot : loot_list) {
        sess->AddLoot(std::move(loot));
      }
    }

    // restore random status
    game.SetRandomSpawn(b_random);
  }

 private:
  Game::SessPtrList m_sess_list;
  std::vector<Player> m_players;
  std::vector<std::string> map_id;
  std::map<std::string, std::list<Loot>> m_loots;
};

}  // namespace model

#endif  // __GAME_SESSION_SER_H__