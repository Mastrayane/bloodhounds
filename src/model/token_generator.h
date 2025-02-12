#ifndef __TOKEN_GENERATOR_H__
#define __TOKEN_GENERATOR_H__

#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <cassert>

#include "../tagged.h"

namespace model {

namespace detail {
struct TokenTag {};

}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;
using TokenHasher = util::TaggedHasher<Token>;
constexpr int TOKEN_LENGHT = 32;

Token GenerateToken();

}  // namespace model

#endif  // __TOKEN_GENERATOR_H__