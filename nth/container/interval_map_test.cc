#include "nth/container/interval_map.h"

#include <string>

#include "nth/test/test.h"

namespace nth {
namespace {

using ::nth::debug::ElementsAreSequentially;

NTH_TEST("interval_map/default", auto key_type, auto mapped_type) {
  interval_map<nth::type_t<key_type>, nth::type_t<mapped_type>> map;
  NTH_EXPECT(map.empty());
  NTH_EXPECT(map.mapped_intervals().empty());
}

NTH_INVOKE_TEST("interval_map/default") {
  co_yield nth::TestArguments{nth::type<int>, nth::type<int>};
  co_yield nth::TestArguments{nth::type<double>, nth::type<int>};
  co_yield nth::TestArguments{nth::type<std::string>, nth::type<int>};
}

NTH_TEST("interval_map/insert_or_assign") {
  interval_map<int, std::string> map;
  map.insert_or_assign(interval(3, 5), "hello");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello")));

  map.insert_or_assign(interval(13, 15), "world");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(13, 15), "world")));

  map.insert_or_assign(interval(12, 14), "world");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(12, 15), "world")));

  map.insert_or_assign(interval(10, 12), "WORLD");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(10, 12), "WORLD"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(12, 15), "world")));

  map.insert_or_assign(interval(10, 12), "world");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(10, 15), "world")));

  map.insert_or_assign(interval(20, 22), "world");
  map.insert_or_assign(interval(14, 21), "world");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 5), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(10, 22), "world")));

  map.insert_or_assign(interval(3, 22), "hi");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(3, 22), "hi")));

  map.insert_or_assign(interval(24, 30), "there");
  map.insert_or_assign(interval(2, 24), "hello");
  NTH_EXPECT(map.mapped_intervals() >>= ElementsAreSequentially(
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(2, 24), "hello"),
                 std::pair<nth::interval<int> const, std::string>(
                     nth::interval(24, 30), "there")));
}

NTH_TEST("interval_map/contains") {
  interval_map<int, std::string> map;
  map.insert_or_assign(interval(3, 5), "a");
  map.insert_or_assign(interval(20, 23), "b");
  NTH_EXPECT(not map.contains(2));
  NTH_EXPECT(map.contains(3));
  NTH_EXPECT(map.contains(4));
  NTH_EXPECT(not map.contains(5));
  NTH_EXPECT(not map.contains(19));
  NTH_EXPECT(map.contains(20));
  NTH_EXPECT(map.contains(21));
  NTH_EXPECT(map.contains(22));
  NTH_EXPECT(not map.contains(23));
}

NTH_TEST("interval_map/covers") {
  interval_map<int, std::string> map;
  map.insert_or_assign(interval(3, 5), "a");
  map.insert_or_assign(interval(5, 7), "b");
  map.insert_or_assign(interval(8, 10), "c");
  NTH_EXPECT(not map.covers(interval(2, 2)));
  NTH_EXPECT(not map.covers(interval(2, 4)));
  NTH_EXPECT(map.covers(interval(3, 3)));
  NTH_EXPECT(map.covers(interval(3, 5)));
  NTH_EXPECT(map.covers(interval(3, 6)));
  NTH_EXPECT(map.covers(interval(4, 6)));
  NTH_EXPECT(map.covers(interval(4, 7)));
  NTH_EXPECT(not map.covers(interval(4, 10)));

  interval_set<int> i;
  i.insert(interval(3, 4));
  i.insert(interval(6, 7));
  NTH_EXPECT(map.covers(i));
  i.insert(interval(7, 8));
  NTH_EXPECT(not map.covers(i));
}

NTH_TEST("interval_map/at") {
  interval_map<int, std::string> map;
  map.insert_or_assign(interval(3, 5), "a");
  map.insert_or_assign(interval(20, 23), "b");
  NTH_EXPECT(map.at(3) == "a");
  NTH_EXPECT(map.at(4) == "a");
  NTH_EXPECT(map.at(20) == "b");
  NTH_EXPECT(map.at(21) == "b");
  NTH_EXPECT(map.at(22) == "b");
}


NTH_TEST("interval_map/mapped_range") {
  interval_map<int, std::string> map;
  map.insert_or_assign(interval(3, 5), "a");
  map.insert_or_assign(interval(6, 7), "b");
  NTH_EXPECT(map.mapped_range(2) == nullptr);
  NTH_EXPECT(map.mapped_range(3) != nullptr);
  NTH_EXPECT(map.mapped_range(4) != nullptr);
  NTH_EXPECT(map.mapped_range(5) == nullptr);
  NTH_EXPECT(map.mapped_range(6) != nullptr);
  NTH_EXPECT(map.mapped_range(7) == nullptr);
  NTH_EXPECT(map.mapped_range(8) == nullptr);
}

}  // namespace
}  // namespace nth
