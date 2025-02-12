#include "player_list.h"

namespace model {

PlayerList::PlayerList() : m_record() {
}

PlayerList::~PlayerList() {
}

std::optional<std::reference_wrapper<Player>> PlayerList::FindPlayer(const Token& token) {
  if (m_players.contains(token)) {
    // check again
    const auto& player = m_players.at(token);
    if (player.IsRetired()) {
      m_record(player.GetInfo());
      m_players.erase(token);
      return std::nullopt;
    }
    return m_players.at(token);
  }
  return std::nullopt;
}

Player& PlayerList::CreatePlayer(const std::string& name, size_t id) {
  Token token = GenerateToken();
  auto pr = m_players.emplace(token, Player(token, name, Player::Id(id)));
  if (pr.second) {
    return m_players.at(token);
  }
  throw std::runtime_error("Fail to add or create Player");
}

void PlayerList::AddPlayer(Player&& player) {
  const Token token = player.GetToken();
  m_players.emplace(token, player);
}

const PlayerList::Container& PlayerList::GetContainer() const {
  return m_players;
}

void model::PlayerList::Update() {
  auto it = m_players.begin(); 
  while (it != m_players.end()) {
    const auto& player = it->second;
    if (player.IsRetired()) {
      // record
      auto player_info = player.GetInfo();
      m_record(player_info); // call recorder
      // delete
      it = m_players.erase(it);
    } else {
      ++it;
    }
  }
}

void model::PlayerList::SetRecorder(model::PlayerList::Record record) {
  m_record = record;
}

}  // namespace model