#include "nth/try/try.h"

#include <optional>

#include "nth/test/test.h"

namespace {

NTH_TEST("try/bool") {
  int counter = 0;
  [&] {
    NTH_TRY(false);
    ++counter;
    return true;
  }();
  NTH_ASSERT(counter == 0);

  [&] {
    NTH_TRY(true);
    ++counter;
    return true;
  }();
  NTH_ASSERT(counter == 1);
}

NTH_TEST("try/pointer") {
  int counter = 0;
  int *ptr    = [&]() -> int    *{
    NTH_TRY(static_cast<int *>(nullptr));
    ++counter;
    return static_cast<int *>(nullptr);
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(ptr == nullptr);

  ptr = [&] -> int * {
    int &c = NTH_TRY(&counter);
    ++counter;
    return &c;
  }();
  NTH_ASSERT(ptr == &counter);
  NTH_ASSERT(counter == 1);

  double *d = nullptr;
  ptr       = [&] -> int       *{
    NTH_TRY(d);
    ++counter;
    return 0;
  }();
  NTH_ASSERT(counter == 1);

  NTH_ASSERT(not [&]()->bool {
    [[maybe_unused]] double &val = NTH_TRY(d);
    return true;
  }());
  NTH_ASSERT(counter == 1);
}

NTH_TEST("try/optional") {
  int counter       = 0;
  std::optional opt = [&] {
    NTH_TRY(std::optional<int>());
    ++counter;
    return std::optional<int>();
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(opt == std::nullopt);

  opt = [&] -> std::optional<int> {
    std::optional<int> o(counter);
    int const &c = NTH_TRY(o);
    ++counter;
    if (&*o == &c) { ++counter; }
    return c;
  }();
  NTH_ASSERT(opt.has_value());
  NTH_ASSERT(*opt == 0);
  NTH_ASSERT(counter == 2);
}

NTH_TEST("unwrap/optional") {
  auto result = NTH_UNWRAP(std::optional<int>(3));
  NTH_ASSERT(result == 3);
}

NTH_TEST("try/absl::Status") {
  int counter         = 0;
  absl::Status status = [&] -> absl::Status {
    NTH_TRY(absl::OkStatus());
    ++counter;
    NTH_TRY(absl::InternalError(""));
    ++counter;
    return absl::InternalError(":(");
  }();
  NTH_ASSERT(counter == 1);
  NTH_ASSERT(status == absl::InternalError(""));
}

NTH_TEST("try/absl::StatusOr") {
  int counter         = 0;
  absl::Status status = [&] -> absl::Status {
    NTH_TRY(absl::StatusOr<int>(absl::InternalError("")));
    ++counter;
    return absl::InternalError("");
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(status == absl::InternalError(""));

  absl::StatusOr<int> status_or = [&] -> absl::StatusOr<int> {
    absl::StatusOr<int> status_or(counter);
    int const &c = NTH_TRY(status_or);
    ++counter;
    if (&*status_or == &c) { ++counter; }
    return c;
  }();
  NTH_ASSERT(status_or.ok());
  NTH_ASSERT(*status_or == 0);
  NTH_ASSERT(counter == 2);
}

struct Handler {
  static constexpr bool okay(bool b) { return not b; }
  static constexpr int transform_return(bool) { return 17; }
  static constexpr int transform_value(bool) { return 89; }
};

NTH_TEST("try/handler") {
  Handler handler;

  int counter = 0;
  NTH_EXPECT([&] {
    auto result = NTH_TRY((handler), false);
    ++counter;
    return result;
  }() == 89);
  NTH_EXPECT(counter == 1);

  NTH_EXPECT([&] {
    auto result = NTH_TRY((handler), true);
    ++counter;
    return result;
  }() == 17);
  NTH_EXPECT(counter == 1);
}

struct Uncopyable {
  explicit Uncopyable(int n) : n(n) {}
  Uncopyable(Uncopyable const &) = delete;
  Uncopyable(Uncopyable &&)      = default;
  int n                          = 17;
};

NTH_TEST("try/main/uncopyable") {
  std::optional<Uncopyable> opt;
  int counter = 0;
  NTH_EXPECT([&] {
    Uncopyable const &uncopyable = NTH_TRY((nth::try_main), opt);
    counter += uncopyable.n;
    return 0;
  }() == 1);
  NTH_EXPECT(counter == 0);

  NTH_EXPECT([&] {
    Uncopyable uncopyable =
        NTH_TRY((nth::try_main), std::optional(Uncopyable(34)));
    counter += uncopyable.n;
    return 0;
  }() == 0);
  NTH_EXPECT(counter == 34);
}

NTH_TEST("try/status/uncopyable") {
  {
    absl::StatusOr<Uncopyable> s = Uncopyable(3);
    auto result                  = [&]() -> absl::StatusOr<int> {
      Uncopyable const &u = NTH_TRY(s);
      return u.n;
    }();

    NTH_ASSERT(result.ok());
    NTH_EXPECT(*result == 3);
  }

  {
    absl::StatusOr<Uncopyable> s = Uncopyable(4);
    auto result                  = [&]() -> absl::StatusOr<int> {
      Uncopyable u = NTH_TRY(std::move(s));
      return u.n;
    }();

    NTH_ASSERT(result.ok());
    NTH_EXPECT(*result == 4);
  }

  {
    absl::StatusOr<Uncopyable> s = Uncopyable(5);
    auto result                  = [&]() -> absl::StatusOr<int> {
      Uncopyable u = std::move(NTH_TRY(s));
      return u.n;
    }();

    NTH_ASSERT(result.ok());
    NTH_EXPECT(*result == 5);
  }
}

}  // namespace
