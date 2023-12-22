#include "nth/base/internal/global_registry.h"

#include "nth/debug/internal/raw_check.h"

using Registrar = nth::internal_base::Registrar<struct Key, int64_t>;

size_t Count(typename Registrar::range_type const &r) {
  size_t count = 0;
  for ([[maybe_unused]] auto unused : r) { ++count; }
  return count;
}

int main() {
  auto registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 0);

  Registrar::Register(0);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 1);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  Registrar::Register(1);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 2);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 1);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  Registrar::Register(2);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 3);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 2);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 1);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  Registrar::Register(3);
  Registrar::Register(4);
  Registrar::Register(5);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 6);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 5);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 4);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 3);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 2);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 1);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  Registrar::Register(6);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 7);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 6);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 5);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 4);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 3);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 2);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 1);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  Registrar::Register(7);
  Registrar::Register(8);
  registry = Registrar::Registry();
  NTH_DEBUG_INTERNAL_RAW_CHECK(Count(registry) == 9);
  {
    auto iter = registry.begin();
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 8);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 7);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 6);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 5);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 4);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 3);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 2);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 1);
    ++iter;
    NTH_DEBUG_INTERNAL_RAW_CHECK(*iter == 0);
  }

  return 0;
}

