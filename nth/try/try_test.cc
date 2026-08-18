#include "nth/try/try.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <concepts>
#include <csignal>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

#include "nth/test/test.h"

namespace {

#if defined(NTH_DFATAL)
bool Aborts(auto&& func) {
  pid_t pid = fork();
  if (pid < 0) { return false; }
  if (pid == 0) {
    int dev_null = open("/dev/null", O_WRONLY);
    if (dev_null != -1) {
      dup2(dev_null, STDERR_FILENO);
      close(dev_null);
    }
    static_cast<void>(func());
    std::exit(0);
  }
  int status = 0;
  waitpid(pid, &status, 0);
  return WIFSIGNALED(status) && (WTERMSIG(status) == SIGABRT);
}
#endif

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
  int* ptr    = [&]() -> int* {
    NTH_TRY(static_cast<int*>(nullptr));
    ++counter;
    return static_cast<int*>(nullptr);
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(ptr == nullptr);

  ptr = [&] -> int* {
    int& c = NTH_TRY(&counter);
    ++counter;
    return &c;
  }();
  NTH_ASSERT(ptr == &counter);
  NTH_ASSERT(counter == 1);

  double* d = nullptr;
  ptr       = [&] -> int* {
    NTH_TRY(d);
    ++counter;
    return 0;
  }();
  NTH_ASSERT(counter == 1);

  NTH_ASSERT(not[&]()->bool {
    [[maybe_unused]] double& val = NTH_TRY(d);
    return true;
  }());
  NTH_ASSERT(counter == 1);
}

NTH_TEST("try/optional") {
  int counter       = 0;
  std::optional opt = [&] -> std::optional<int> {
    NTH_TRY(std::optional<int>());
    ++counter;
    return std::optional<int>();
  }();
  NTH_ASSERT(counter == 0);
  NTH_ASSERT(opt == std::nullopt);

  opt = [&] -> std::optional<int> {
    std::optional<int> o(counter);
    int const& c = NTH_TRY(o);
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
    int const& c = NTH_TRY(status_or);
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
  Uncopyable(Uncopyable const&) = delete;
  Uncopyable(Uncopyable&&)      = default;
  int n                         = 17;
};

NTH_TEST("try/main/uncopyable") {
  std::optional<Uncopyable> opt;
  int counter = 0;
  NTH_EXPECT([&] {
    Uncopyable const& uncopyable = NTH_TRY((nth::try_main), opt);
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
      Uncopyable const& u = NTH_TRY(s);
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

NTH_TEST("try/continue") {
  std::vector<int> nums;
  for (int i = 0; i < 10; ++i) {
    int* ptr = (i % 3 == 0) ? nullptr : &i;
    nums.push_back(NTH_UNWRAP_OR(continue, ptr));
  }
  NTH_ASSERT(nums.size() == 6u);
  NTH_EXPECT(nums[0] == 1);
  NTH_EXPECT(nums[1] == 2);
  NTH_EXPECT(nums[2] == 4);
  NTH_EXPECT(nums[3] == 5);
  NTH_EXPECT(nums[4] == 7);
  NTH_EXPECT(nums[5] == 8);
}

NTH_TEST("try/break") {
  std::vector<int> nums;
  for (int i = 0; i < 10; ++i) {
    int* ptr = (i == 7) ? nullptr : &i;
    nums.push_back(NTH_UNWRAP_OR(break, ptr));
  }
  NTH_ASSERT(nums.size() == 7u);
  NTH_EXPECT(nums[0] == 0);
  NTH_EXPECT(nums[1] == 1);
  NTH_EXPECT(nums[2] == 2);
  NTH_EXPECT(nums[3] == 3);
  NTH_EXPECT(nums[4] == 4);
  NTH_EXPECT(nums[5] == 5);
  NTH_EXPECT(nums[6] == 6);
}

struct Unformattable {
  bool b;
};

struct UnformattableHandler {
  static constexpr bool okay(Unformattable const& u) { return u.b; }

  static constexpr Unformattable transform_value(Unformattable u) { return u; }
  static constexpr Unformattable transform_return(Unformattable u) { return u; }
};

NTH_TEST("try/handler/unformattable") {
  // Testing that unformattable objects still compile.
  [[maybe_unused]] auto run = [](bool b) {
    Unformattable u{.b = b};
    UnformattableHandler handler;
    return NTH_UNWRAP((handler), u);
  };
}

struct Mercurial {
  std::string_view value() & { return "&"; }
  std::string_view value() && { return "&&"; }
  std::string_view value() const& { return "const &"; }
  std::string_view value() const&& { return "const &&"; }
};

struct MercurialHandler {
  using type = std::variant<Mercurial, Mercurial>;

  static constexpr bool okay(type const& value) { return value.index() == 0; }

  template <typename T>
    requires std::same_as<std::remove_cvref_t<T>, type>
  static constexpr std::pair<int, std::string_view> transform_value(T&& value) {
    return {0, std::get<0>(std::forward<T>(value)).value()};
  }

  template <typename T>
    requires std::same_as<std::remove_cvref_t<T>, type>
  static constexpr std::pair<int, std::string_view> transform_return(
      T&& value) {
    return {1, std::get<1>(std::forward<T>(value)).value()};
  }
};

NTH_TEST("try/handler/value_category") {
  using namespace std::string_view_literals;
  auto success = std::in_place_index<0>;
  auto failure = std::in_place_index<1>;
  auto run     = []<typename T>(nth::type_tag<T>, auto index) {
    MercurialHandler handler;
    std::remove_reference_t<T> value{index};
    return NTH_TRY((handler), static_cast<T>(value));
  };
  using T = MercurialHandler::type;
  NTH_EXPECT(run(nth::type<T&>, success) == std::pair{0, "&"sv});
  NTH_EXPECT(run(nth::type<T&>, failure) == std::pair{1, "&"sv});
  NTH_EXPECT(run(nth::type<T&&>, success) == std::pair{0, "&&"sv});
  NTH_EXPECT(run(nth::type<T&&>, failure) == std::pair{1, "&&"sv});
  NTH_EXPECT(run(nth::type<T const&>, success) == std::pair{0, "const &"sv});
  NTH_EXPECT(run(nth::type<T const&>, failure) == std::pair{1, "const &"sv});
  NTH_EXPECT(run(nth::type<T const&&>, success) == std::pair{0, "const &&"sv});
  NTH_EXPECT(run(nth::type<T const&&>, failure) == std::pair{1, "const &&"sv});
}

struct LifetimeProbe {
  explicit LifetimeProbe(int v) : value(v) {}
  ~LifetimeProbe() { value = -1; }
  int value;
};

struct LifetimeProbeHandler {
  static constexpr bool okay(LifetimeProbe const&) { return true; }
  static constexpr int transform_value(LifetimeProbe const& p) {
    return p.value;
  }
  static constexpr int transform_value(LifetimeProbe&& p) { return p.value; }
  static constexpr int transform_return(LifetimeProbe const&) { return -2; }
};

LifetimeProbe MakeLifetimeProbe(int v) { return LifetimeProbe(v); }

NTH_TEST("try/xvalue_temporary_lifetime") {
  LifetimeProbeHandler handler;
  int result = [&] {
    return NTH_TRY((handler), std::move(MakeLifetimeProbe(42)));
  }();
  NTH_EXPECT(result == 42);
}

NTH_TEST("unwrap/xvalue_temporary_lifetime") {
  LifetimeProbeHandler handler;
  int result = NTH_UNWRAP((handler), std::move(MakeLifetimeProbe(42)));
  NTH_EXPECT(result == 42);
}

NTH_TEST("try/optional/mutable-reference") {
  std::optional<int> o(5);
  std::optional<int> result = [&]() -> std::optional<int> {
    int& r = NTH_TRY(o);
    r      = 10;
    return r;
  }();
  NTH_EXPECT(*o == 10);
  NTH_EXPECT(result == std::optional<int>(10));
}

NTH_TEST("try/parenthesized-expression") {
  // A lone fully-parenthesized argument is the expression itself, not a
  // handler. Parenthesization also protects top-level commas.
  std::optional<int> o(5);
  std::optional<int> result = [&]() -> std::optional<int> {
    int x = NTH_TRY((o));
    return x + 1;
  }();
  NTH_EXPECT(result == std::optional<int>(6));

  NTH_EXPECT(NTH_UNWRAP((std::optional<std::pair<int, int>>({3, 4}))) ==
             std::pair(3, 4));
}

NTH_TEST("try/pointer-to-optional") {
  auto opt = [&]() -> std::optional<std::string> {
    NTH_TRY(static_cast<int*>(nullptr));
    return "hello";
  }();
  NTH_EXPECT(opt == std::nullopt);
}

NTH_TEST("try/optional-to-pointer") {
  std::string s = "hello";
  auto ptr      = [&]() -> std::string* {
    NTH_TRY(std::optional<int>{});
    return &s;
  }();
  NTH_EXPECT(ptr == nullptr);
}

#if defined(NTH_DFATAL)

NTH_TEST("try/dfatal/defined/bool") {
  int counter = 0;
  bool result = [&]() -> bool {
    NTH_TRY((nth::dfatal), true);
    ++counter;
    return true;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(result);

  NTH_EXPECT(Aborts([]() -> bool {
    NTH_TRY((nth::dfatal), false);
    return true;
  }));
}

NTH_TEST("try/dfatal/defined/pointer") {
  int counter = 0;
  int x       = 42;
  int* ptr    = [&]() -> int* {
    int& r = NTH_TRY((nth::dfatal), &x);
    ++counter;
    r = 100;
    return &r;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(ptr == &x);
  NTH_EXPECT(x == 100);

  NTH_EXPECT(Aborts([]() -> int* {
    [[maybe_unused]] int& r =
        NTH_TRY((nth::dfatal), static_cast<int*>(nullptr));
    return nullptr;
  }));
}

NTH_TEST("try/dfatal/defined/optional") {
  int counter = 0;
  std::optional<int> o(5);
  std::optional<int> opt = [&]() -> std::optional<int> {
    int const& c = NTH_TRY((nth::dfatal), o);
    ++counter;
    return c + 1;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(opt == 6);

  // Mutable reference mutation
  opt = [&]() -> std::optional<int> {
    int& r = NTH_TRY((nth::dfatal), o);
    r      = 10;
    return r;
  }();
  NTH_EXPECT(*o == 10);
  NTH_EXPECT(opt == 10);

  // Move-only / uncopyable
  std::optional<int> uncopyable_result = [&]() -> std::optional<int> {
    Uncopyable u = NTH_TRY((nth::dfatal), std::optional(Uncopyable(77)));
    return u.n;
  }();
  NTH_ASSERT(uncopyable_result.has_value());
  NTH_EXPECT(*uncopyable_result == 77);

  NTH_EXPECT(Aborts([]() -> std::optional<int> {
    int val = NTH_TRY((nth::dfatal), std::optional<int>());
    return val;
  }));
}

NTH_TEST("try/dfatal/defined/custom_handler") {
  Handler handler;
  int counter = 0;

  // Custom handler returns 89 on success (input false), aborts on failure
  // (input true).
  NTH_EXPECT([&]() -> int {
    auto result = NTH_TRY((nth::dfatal(handler)), false);
    ++counter;
    return result;
  }() == 89);
  NTH_EXPECT(counter == 1);

  NTH_EXPECT(Aborts([&]() -> int {
    auto result = NTH_TRY((nth::dfatal(handler)), true);
    return result;
  }));
}

NTH_TEST("try/dfatal/defined/zero_args") {
  int counter = 0;
  auto res    = [&]() -> std::optional<int> {
    int val = NTH_TRY((nth::dfatal()), std::optional<int>(99));
    ++counter;
    return val;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(res == 99);

  NTH_EXPECT(Aborts([]() -> std::optional<int> {
    std::optional<int> opt;
    int val = NTH_TRY((nth::dfatal()), opt);
    return val;
  }));
}

NTH_TEST("try/dfatal/defined/value_category") {
  using namespace std::string_view_literals;
  auto success     = std::in_place_index<0>;
  auto failure     = std::in_place_index<1>;
  auto run_success = []<typename T>(
                         nth::type_tag<T>,
                         auto index) -> std::pair<int, std::string_view> {
    MercurialHandler handler;
    std::remove_reference_t<T> value{index};
    return NTH_TRY((nth::dfatal(handler)), static_cast<T>(value));
  };
  using T = MercurialHandler::type;
  NTH_EXPECT(run_success(nth::type<T&>, success) == std::pair{0, "&"sv});
  NTH_EXPECT(run_success(nth::type<T&&>, success) == std::pair{0, "&&"sv});
  NTH_EXPECT(run_success(nth::type<T const&>, success) ==
             std::pair{0, "const &"sv});
  NTH_EXPECT(run_success(nth::type<T const&&>, success) ==
             std::pair{0, "const &&"sv});

  auto run_failure = []<typename T>(
                         nth::type_tag<T>,
                         auto index) -> std::pair<int, std::string_view> {
    MercurialHandler handler;
    std::remove_reference_t<T> value{index};
    return NTH_TRY((nth::dfatal(handler)), static_cast<T>(value));
  };
  NTH_EXPECT(Aborts([&] { run_failure(nth::type<T&>, failure); }));
  NTH_EXPECT(Aborts([&] { run_failure(nth::type<T&&>, failure); }));
  NTH_EXPECT(Aborts([&] { run_failure(nth::type<T const&>, failure); }));
  NTH_EXPECT(Aborts([&] { run_failure(nth::type<T const&&>, failure); }));
}

#else  // not defined(NTH_DFATAL)

NTH_TEST("try/dfatal/not_defined/pointer") {
  int counter = 0;
  int* ptr    = [&]() -> int* {
    NTH_TRY((nth::dfatal), static_cast<int*>(nullptr));
    ++counter;
    return static_cast<int*>(nullptr);
  }();
  NTH_EXPECT(counter == 0);
  NTH_EXPECT(ptr == nullptr);

  int x = 42;
  ptr   = [&]() -> int* {
    int& r = NTH_TRY((nth::dfatal), &x);
    ++counter;
    r = 100;
    return &r;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(ptr == &x);
  NTH_EXPECT(x == 100);
}

NTH_TEST("try/dfatal/not_defined/optional") {
  int counter            = 0;
  std::optional<int> opt = [&]() -> std::optional<int> {
    NTH_TRY((nth::dfatal), std::optional<int>());
    ++counter;
    return 1;
  }();
  NTH_EXPECT(counter == 0);
  NTH_EXPECT(opt == std::nullopt);

  std::optional<int> o(5);
  opt = [&]() -> std::optional<int> {
    int const& c = NTH_TRY((nth::dfatal), o);
    ++counter;
    return c + 1;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(opt == 6);

  // Mutable reference mutation
  opt = [&]() -> std::optional<int> {
    int& r = NTH_TRY((nth::dfatal), o);
    r      = 10;
    return r;
  }();
  NTH_EXPECT(*o == 10);
  NTH_EXPECT(opt == 10);

  // Move-only / uncopyable
  std::optional<int> uncopyable_result = [&]() -> std::optional<int> {
    Uncopyable u = NTH_TRY((nth::dfatal), std::optional(Uncopyable(77)));
    return u.n;
  }();
  NTH_ASSERT(uncopyable_result.has_value());
  NTH_EXPECT(*uncopyable_result == 77);
}

NTH_TEST("try/dfatal/not_defined/custom_handler") {
  Handler handler;
  int counter = 0;

  // Custom handler returns 89 on success (input false), 17 on failure (input
  // true).
  NTH_EXPECT([&] {
    auto result = NTH_TRY((nth::dfatal(handler)), false);
    ++counter;
    return result;
  }() == 89);
  NTH_EXPECT(counter == 1);

  NTH_EXPECT([&] {
    auto result = NTH_TRY((nth::dfatal(handler)), true);
    ++counter;
    return result;
  }() == 17);
  NTH_EXPECT(counter == 1);
}

NTH_TEST("try/dfatal/not_defined/zero_args") {
  std::optional<int> opt;
  int counter = 0;
  auto res    = [&]() -> std::optional<int> {
    int val = NTH_TRY((nth::dfatal()), std::optional<int>(99));
    ++counter;
    return val;
  }();
  NTH_EXPECT(counter == 1);
  NTH_EXPECT(res == 99);

  counter = 0;
  res     = [&]() -> std::optional<int> {
    int val = NTH_TRY((nth::dfatal()), opt);
    ++counter;
    return val;
  }();
  NTH_EXPECT(counter == 0);
  NTH_EXPECT(res == std::nullopt);
}

NTH_TEST("try/dfatal/not_defined/value_category") {
  using namespace std::string_view_literals;
  auto success = std::in_place_index<0>;
  auto failure = std::in_place_index<1>;
  auto run     = []<typename T>(nth::type_tag<T>, auto index) {
    MercurialHandler handler;
    std::remove_reference_t<T> value{index};
    return NTH_TRY((nth::dfatal(handler)), static_cast<T>(value));
  };
  using T = MercurialHandler::type;
  NTH_EXPECT(run(nth::type<T&>, success) == std::pair{0, "&"sv});
  NTH_EXPECT(run(nth::type<T&>, failure) == std::pair{1, "&"sv});
  NTH_EXPECT(run(nth::type<T&&>, success) == std::pair{0, "&&"sv});
  NTH_EXPECT(run(nth::type<T&&>, failure) == std::pair{1, "&&"sv});
  NTH_EXPECT(run(nth::type<T const&>, success) == std::pair{0, "const &"sv});
  NTH_EXPECT(run(nth::type<T const&>, failure) == std::pair{1, "const &"sv});
  NTH_EXPECT(run(nth::type<T const&&>, success) == std::pair{0, "const &&"sv});
  NTH_EXPECT(run(nth::type<T const&&>, failure) == std::pair{1, "const &&"sv});
}

#endif  // defined(NTH_DFATAL)

}  // namespace
