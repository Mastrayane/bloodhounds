#ifndef __PLAYER_LIST_H__
#define __PLAYER_LIST_H__

#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include <optional>
#include <functional>

#include "../tagged.h"
#include "player.h"

namespace model {

class PlayerList {
 public:
  using Container = std::unordered_map<Token, Player, TokenHasher>;
  using Record = std::function<void(model::PlayerInfo)>;
  
  PlayerList();
  ~PlayerList();


  std::optional<std::reference_wrapper<Player>> FindPlayer(const Token& token);

  Player& CreatePlayer(const std::string& name, size_t id);
  void AddPlayer(Player&& player);

  const Container& GetContainer() const;

  void Update();

  void SetRecorder(Record record);
  
  auto cbegin() { return m_players.cbegin(); }
  auto cend() { return m_players.cend(); }

 private:
  Container m_players;
  Record m_record;
};

}  // namespace model

#endif  // __PLAYER_LIST_H__