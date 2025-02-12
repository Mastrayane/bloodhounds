/**
 * @file std_optional_serialization.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Realization for std::optional as <boost/serialization/optional.hpp>
 * @version 0.1
 * @date 2023-01-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __STD_OPTIONAL_SERIALIZATION_H__
#define __STD_OPTIONAL_SERIALIZATION_H__
/**/
#include <boost/serialization/split_free.hpp>

#include <optional>

namespace boost::serialization {

template <typename Archive, typename T>
void save(Archive& ar, const std::optional<T>& opt, unsigned int version) {
  const bool has_value = opt.has_value();
  ar << opt.has_value();
  if (has_value) {
    ar << *opt;
  }
}

template <typename Archive, typename T>
void load(Archive& ar, std::optional<T>& opt, unsigned int version) {
  bool has_value{false};
  ar >> has_value;
  if (has_value) {
    opt = T();
    ar >> *opt;
  } else {
    opt.reset();
  }
}

template <class Archive, class T>
void serialize(Archive& ar, std::optional<T>& t, const unsigned int version) {
  boost::serialization::split_free(ar, t, version);
}

}  

#endif  // __STD_OPTIONAL_SERIALIZATION_H__