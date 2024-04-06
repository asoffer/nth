#include "nth/strings/null_terminated_string_view.h"

#include "nth/test/test.h"

namespace nth {
namespace {

NTH_TEST("null_terminated_string_view/default-construction") {
  null_terminated_string_view s;
  NTH_EXPECT(s.size() == size_t{0});
  NTH_EXPECT(s.data() != nullptr);
}

NTH_TEST("null_terminated_string_view/char-array-construction") {
  char buffer[] = "abc";
  null_terminated_string_view s(null_terminated_string_view::from_array,
                                buffer);
  NTH_EXPECT(s.size() == size_t{3});
  NTH_EXPECT(s.data() == buffer);
}

NTH_TEST("null_terminated_string_view/char-pointer-construction") {
  char buffer[] = "abc";
  null_terminated_string_view s(null_terminated_string_view::from_pointer,
                                buffer);
  NTH_EXPECT(s.size() == size_t{3});
  NTH_EXPECT(s.data() == buffer);
}

NTH_TEST("null_terminated_string_view/constexpr-char-pointer-construction") {
  static constexpr char buffer[] = "abc";
  constexpr null_terminated_string_view s(
      null_terminated_string_view::from_pointer, buffer);
  NTH_EXPECT(s.size() == size_t{3});
  NTH_EXPECT(s.data() == buffer);
}

NTH_TEST("null_terminated_string_view/empty") {
  NTH_EXPECT(null_terminated_string_view().empty());
  NTH_EXPECT(
      null_terminated_string_view(null_terminated_string_view::from_array, "")
          .empty());
  NTH_EXPECT(not null_terminated_string_view(
                     null_terminated_string_view::from_array, "abc")
                     .empty());
}

NTH_TEST("null_terminated_string_view/size") {
  NTH_EXPECT(null_terminated_string_view().size() == size_t{0});
  NTH_EXPECT(
      null_terminated_string_view(null_terminated_string_view::from_array, "")
          .size() == size_t{0});
  NTH_EXPECT(null_terminated_string_view(
                 null_terminated_string_view::from_array, "abc")
                 .size() == size_t{3});
}

NTH_TEST("null_terminated_string_view/index") {
  null_terminated_string_view s(null_terminated_string_view::from_array, "abc");
  NTH_EXPECT(s[0] == 'a');
  NTH_EXPECT(s[1] == 'b');
  NTH_EXPECT(s[2] == 'c');
  NTH_EXPECT(s[3] == '\0');
}

NTH_TEST("null_terminated_string_view/index-empty") {
  null_terminated_string_view s;
  NTH_EXPECT(s[0] == '\0');
}

NTH_TEST("null_terminated_string_view/remove_prefix") {
  null_terminated_string_view s(null_terminated_string_view::from_array,
                                "abcdef");
  s.remove_prefix(0);
  NTH_EXPECT(s == null_terminated_string_view(
                      null_terminated_string_view::from_array, "abcdef"));
  s.remove_prefix(1);
  NTH_EXPECT(s == null_terminated_string_view(
                      null_terminated_string_view::from_array, "bcdef"));
  s.remove_prefix(3);
  NTH_EXPECT(s == null_terminated_string_view(
                      null_terminated_string_view::from_array, "ef"));
}

}  // namespace
}  // namespace nth
