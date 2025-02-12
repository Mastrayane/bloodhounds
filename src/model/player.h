#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <memory>

#include "dog.h"
#include "token_generator.h"
#include "game_session.h"

namespace model {

struct PlayerInfo {
  // Token token;
  std::string name;
  int score;
  double play_time;
};

class Player {
 public:
  using Id = util::Tagged<uint32_t, Player>;

  explicit Player(const Token& token, const std::string& name, const Id& id);

  Id GetId() const noexcept;

  const std::string& GetName() const noexcept;

  void SetDog(DogPtr dog);
  DogPtr GetDog();
  DogConstPtr GetDog() const;

  const Token& GetToken() const;

  std::shared_ptr<GameSession> GetSession();
  const std::shared_ptr<GameSession> GetSession() const;

  void Update();

  void SetSession(std::shared_ptr<GameSession> sess);

  PlayerInfo GetInfo() const;

  bool IsRetired() const;

 private:
  Id m_id;
  Token m_token;
  std::string m_name;

  std::shared_ptr<GameSession> m_gsess;
  DogPtr m_dog;
};

}  // namespace model

#endif  // __PLAYER_H__