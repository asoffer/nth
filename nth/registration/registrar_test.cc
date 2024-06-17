#include "nth/registration/registrar.h"

#include "nth/test/raw/test.h"

size_t Count(typename nth::registrar<int64_t>::range_type const &r) {
  size_t count = 0;
  for ([[maybe_unused]] auto unused : r) { ++count; }
  return count;
}

int main() {
  nth::registrar<int64_t> registrar;

  auto registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 0);

  registrar.insert(0);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 1);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }

  registrar.insert(1);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 2);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 1);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }

  registrar.insert(2);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 3);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 2);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 1);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }

  registrar.insert(3);
  registrar.insert(4);
  registrar.insert(5);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 6);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 5);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 4);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 3);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 2);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 1);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }

  registrar.insert(6);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 7);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 6);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 5);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 4);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 3);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 2);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 1);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }

  registrar.insert(7);
  registrar.insert(8);
  registry = registrar.registry();
  NTH_RAW_TEST_ASSERT(Count(registry) == 9);
  {
    auto iter = registry.begin();
    NTH_RAW_TEST_ASSERT(*iter == 8);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 7);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 6);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 5);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 4);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 3);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 2);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 1);
    ++iter;
    NTH_RAW_TEST_ASSERT(*iter == 0);
  }
}
