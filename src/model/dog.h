#ifndef __DOG_H__
#define __DOG_H__

#include <list>
#include <memory>
#include <chrono>

#include "geometry.h"
#include "token_generator.h"
#include "game_object.h"
#include "loot.h"

namespace model {

using namespace std::literals;

class Dog : public GameObject {
 public:
  using Id = util::Tagged<uint32_t, Dog>;

  explicit Dog(const Dog::Id& id);

  const Id& GetId() const;

  void SetToken(const Token& token);
  const Token& GetToken() const;

  /**
   * @brief get current speed
   * @return Point2d with sp
   */
  const Point2d& GetSpeed() const noexcept;

  /**
   * @brief set current speed to (0.0, 0.0)
   */
  void Stop();

  void SetDefaultSpeed(const double& speed);

  /**
   * @brief set direction as "U", "D", "L", "R", "".
   * @exception  std::invalid_argument
   */
  void SetDir(const std::string& dir);

  /**
   * @brief get direction.
   * @return std::string  with  "U", "D", "L", "R".
   */
  const std::string& GetDir() const noexcept;

  void AddLoot(const Loot& loot);

  bool IsFull() const;

  [[nodiscard("Unload all resource from the Dog object")]] std::list<Loot> UnloadLoots();

  const std::list<Loot>& GetLoots() const noexcept;

  void AddPoints(int points);
  int GetScore() const noexcept;

  void SetBagSize(size_t size);
  size_t GetBagSize() const noexcept;

  void UpdateTimer(std::chrono::milliseconds delta_time);

  std::chrono::milliseconds GetDownTime() const;

  std::chrono::milliseconds GetPlayTime() const;

 private:
  Token m_token;
  Id m_id;
  static int ID_COUNTER;

  Point2d m_speed;
  std::string m_dir;
  double m_default_speed;

  std::list<Loot> m_loots;
  size_t m_bag_size;

  int m_score;
  std::chrono::milliseconds  m_play_time;
  std::chrono::milliseconds  m_downtime;
  bool m_moving;
};

using DogPtr = std::shared_ptr<Dog>;
using DogWeakPtr = std::weak_ptr<Dog>;
using DogConstPtr = std::shared_ptr<const Dog>;


}  // namespace model

#endif  // __DOG_H__