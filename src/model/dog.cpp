#include "dog.h"

namespace model {

Dog::Dog(const Dog::Id& id)
    : GameObject({0.0, 0.0}, 0.6),
      m_token(""),
      m_id(id),
      m_speed(0.f, 0.f),
      m_dir("U"s),
      m_default_speed(0.0),
      m_bag_size(0),
      m_score(0),
      m_downtime(0ms),
      m_play_time(0ms),
      m_moving(false) {
}

const Dog::Id& Dog::GetId() const {
  return m_id;
}

void Dog::SetToken(const Token& token) {
  m_token = token;
}


const Token& Dog::GetToken() const {
  return m_token;
}

const Point2d& Dog::GetSpeed() const noexcept {
  return m_speed;
}

void Dog::Stop() {
  m_speed.x = 0.0;
  m_speed.y = 0.0;
}

void Dog::SetDefaultSpeed(const double& speed) {
  m_default_speed = speed;
}

const std::string& Dog::GetDir() const noexcept {
  return m_dir;
}

bool model::Dog::IsFull() const {
  return m_bag_size == m_loots.size();
}

void Dog::AddLoot(const Loot& loot) {
  if (m_loots.size() >= m_bag_size) {
    throw std::logic_error("Bag is full");
  }
  m_loots.push_back(loot);
}

std::list<Loot> Dog::UnloadLoots() {
  return std::move(m_loots);
}

const std::list<Loot>& Dog::GetLoots() const noexcept {
  return m_loots;
}

void Dog::AddPoints(int points) {
  m_score += points;
}

int Dog::GetScore() const noexcept {
  return m_score;
}


void Dog::SetDir(const std::string& dir) {
  // any change dir not mean idle 
  m_downtime = 0ms;
  m_moving = true;
  
  if (dir == "") {
    m_moving = false;
    Stop();
    return;
  }

  if (dir == "U") {
    m_speed = {0.0, -m_default_speed};
  } else if (dir == "R") {
    m_speed = {m_default_speed, 0.0};
  } else if (dir == "D") {
    m_speed = {0.0, m_default_speed};
  } else if (dir == "L") {
    m_speed = {-m_default_speed, 0.0};
  } else {
    throw std::invalid_argument("Argumen\'"s + dir + "\' must be 'U', 'R', 'D', 'L' or \"\"."s);
  }
  
  m_dir = dir;
}

void Dog::SetBagSize(size_t size) {
  m_bag_size = size;
}

size_t Dog::GetBagSize() const noexcept{
  return m_bag_size;
}

void Dog::UpdateTimer(std::chrono::milliseconds delta_time) {
  m_play_time += delta_time;
  if(!m_moving) {
    m_downtime += delta_time;
  } else {
    m_downtime = 0ms;
  }
}

std::chrono::milliseconds Dog::GetDownTime() const {
  return m_downtime;
}

std::chrono::milliseconds Dog::GetPlayTime() const {
  return m_play_time;
}


}  // namespace model