#include "token_generator.h"

namespace model {
Token GenerateToken() {
  std::random_device rd;
  std::mt19937_64 m_gen1{[&rd] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(rd);
  }()};
  std::mt19937_64 m_gen2{[&rd] {
    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
    return dist(rd);
  }()};

  std::stringstream ss;
  ss << std::setw(16) << std::setfill('0') << std::hex << m_gen1();
  ss << std::setw(16) << std::setfill('0') << std::hex << m_gen2();
  std::string res = ss.str();
  assert(res.size() == 32);
  return Token(ss.str());
}
}  // namespace model