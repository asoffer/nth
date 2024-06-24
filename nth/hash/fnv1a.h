#ifndef NTH_HASH_FNV1A_H
#define NTH_HASH_FNV1A_H

#include <cstdint>
#include <string_view>

namespace nth {

constexpr uint64_t fnv1a(std::string_view s) {
  uint64_t hash        = 14695981039346656037u;
  constexpr uint64_t p = 1099511628211;
  for (char c : s) { hash = (hash * p) ^ static_cast<uint64_t>(c); }
  return hash;
}

}  // namespace nth

#endif  // NTH_HASH_FNV1A_H
