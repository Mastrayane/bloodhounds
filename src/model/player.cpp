#include "player.h"

namespace model {

Player::Player(const Token& token, const std::string& name, const Player::Id& id) : m_id(id), m_token(token), m_name(name), m_dog{} {
}

Player::Id Player::GetId() const noexcept {
  return m_id;
}

const std::string& Player::GetName() const noexcept {
  return m_name;
}

void Player::SetDog(DogPtr dog) {
  m_dog = dog;
  m_dog->SetToken(m_token);
}

DogPtr Player::GetDog() {
  return m_dog;
}

DogConstPtr Player::GetDog() const {
  return m_dog;
}

const Token& Player::GetToken() const {
  return m_token;
}

std::shared_ptr<GameSession> Player::GetSession() {
  return m_gsess;
}

const std::shared_ptr<GameSession> Player::GetSession() const {
  return m_gsess;
}

void Player::SetSession(std::shared_ptr<GameSession> sess) {
  m_gsess = sess;
  m_gsess->AddDog(m_dog);
}

PlayerInfo Player::GetInfo() const {
  assert(m_dog != nullptr);
  const auto& play_time = std::chrono::duration<double>(m_dog->GetPlayTime()).count();
  PlayerInfo info{
      .name = m_name,
      .score = m_dog->GetScore(),
      .play_time = play_time};
  return info;  // RVO
}

bool Player::IsRetired() const {
  return m_dog->GetDownTime() >= m_gsess->GetRetirementTime();
}

}  // namespace model
