#include "nth/utility/early_exit.h"

#include <string>
#include <variant>

#include "nth/test/test.h"
#include "nth/utility/early_exit_test_cases.h"

namespace nth {
namespace {

struct Error : nth::early_exitable<Error> {
  using nth::early_exitable<Error>::early_exitable;
  explicit Error(int n) : value_(n) {}

  friend bool operator==(Error lhs, Error rhs) {
    return lhs.value_ == rhs.value_;
  }
  explicit operator bool() const { return value_ >= 0; }

  int value() const { return value_; }

 private:
  int value_;
};

template <typename T>
struct ErrorOr : nth::early_exitable<ErrorOr<T>> {
  using nth::early_exitable<ErrorOr>::early_exitable;

  explicit ErrorOr(Error e) : value_(e) {}
  explicit ErrorOr(T t) : value_(t) {}

  explicit operator Error const&() const { return std::get<Error>(value_); }

  friend bool operator==(ErrorOr const& lhs, ErrorOr const&rhs) {
    return lhs.value_ == rhs.value_;
  }
  explicit operator bool() const { return std::holds_alternative<T>(value_); }
  T const& operator*() const { return std::get<T>(value_); }

 private:
  std::variant<Error, T> value_ = Error(0);
};

static_assert(supports_early_exit<Error>);
static_assert(supports_early_exit<ErrorOr<int>>);
static_assert(supports_early_exit<ErrorOr<std::string>>);

NTH_INVOKE_TEST("nth/utility/early-exit") {
  co_yield nth::type<Error>;
  co_yield nth::type<ErrorOr<int>>;
  co_yield nth::type<ErrorOr<std::string>>;
}

NTH_TEST("early_exit/return-void/continue") {
  int count = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await Error(5);
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 2);
}

NTH_TEST("early_exit/return-void/exit") {
  int count = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await Error(-3);
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-3));
  NTH_EXPECT(count == 1);
}

NTH_TEST("early_exit/return-value/continue") {
  int count = 0;

  ErrorOr v = [&]() -> ErrorOr<std::string> {
    ++count;
    std::string value = co_await ErrorOr<std::string>("hello");
    ++count;
    co_return ErrorOr(value + ", world");
  }();

  NTH_EXPECT(v == ErrorOr<std::string>("hello, world"));
  NTH_EXPECT(count == 2);
}

NTH_TEST("early_exit/return-value/exit") {
  int count = 0;

  ErrorOr<std::string> v = [&]() -> ErrorOr<std::string> {
    ++count;
    co_await ErrorOr<std::string>(Error(-3));
    ++count;
    co_return ErrorOr<std::string>("ok");
  }();

  NTH_EXPECT(v == ErrorOr<std::string>(Error(-3)));
  NTH_EXPECT(count == 1);
}

NTH_TEST("early_exit/return-value/convert/continue") {
  int count = 0;

  Error v = [&]() -> Error {
    ++count;
    std::string s = co_await ErrorOr<std::string>("hello");
    NTH_EXPECT(s == "hello");
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 2);
}

NTH_TEST("early_exit/return-value/convert/exit") {
  int count = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await ErrorOr<std::string>(Error(-3));
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-3));
  NTH_EXPECT(count == 1);
}

NTH_TEST("early_exit/on-exit/return-void/take-parameter/continue") {
  int count              = 0;
  int exit_handler_total = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                     });
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                     });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 3);
  NTH_EXPECT(exit_handler_total == 0);
}

NTH_TEST("early_exit/on-exit/return-void/take-parameter/exit") {
  int count              = 0;
  int exit_handler_total = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                     });
    ++count;
    co_await on_exit(ErrorOr<std::string>(Error(-3)),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                     });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-3));
  NTH_EXPECT(count == 2);
  NTH_EXPECT(exit_handler_total == -3);
}

NTH_TEST("early_exit/on-exit/return-void/take-void/continue") {
  int count              = 0;
  int exit_handler_count = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&] { ++exit_handler_count; });
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&] { ++exit_handler_count; });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 3);
  NTH_EXPECT(exit_handler_count == 0);
}

NTH_TEST("early_exit/on-exit/return-void/take-void/exit") {
  int count              = 0;
  int exit_handler_count = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&] { ++exit_handler_count; });
    ++count;
    co_await on_exit(ErrorOr<std::string>(Error(-3)),
                     [&] { ++exit_handler_count; });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-3));
  NTH_EXPECT(count == 2);
  NTH_EXPECT(exit_handler_count == 1);
}

NTH_TEST("early_exit/on-exit/return-value/take-parameter/continue") {
  int count              = 0;
  int exit_handler_total = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                       return Error(-3);
                     });
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                       return Error(-4);
                     });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 3);
  NTH_EXPECT(exit_handler_total == 0);
}

NTH_TEST("early_exit/on-exit/return-value/take-parameter/exit") {
  int count              = 0;
  int exit_handler_total = 0;

  Error v = [&]() -> Error {
    ++count;
    co_await on_exit(ErrorOr<std::string>("hello"),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                       return Error(-3);
                     });
    ++count;
    co_await on_exit(ErrorOr<std::string>(Error(-2)),
                     [&](ErrorOr<std::string> const& e) {
                       exit_handler_total += static_cast<Error>(e).value();
                       return Error(-4);
                     });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-4));
  NTH_EXPECT(count == 2);
  NTH_EXPECT(exit_handler_total == -2);
}

NTH_TEST("early_exit/on-exit/return-value/take-void/continue") {
  int count              = 0;
  int exit_handler_count = 0;

  Error v = [&]() -> Error {
    ++count;
    std::string s = co_await on_exit(ErrorOr<std::string>("hello"), [&] {
      ++exit_handler_count;
      return Error(-3);
    });
    ++count;
    s = co_await on_exit(ErrorOr<std::string>("hello"), [&] {
      ++exit_handler_count;
      return Error(-4);
    });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(3));
  NTH_EXPECT(count == 3);
  NTH_EXPECT(exit_handler_count == 0);
}

NTH_TEST("early_exit/on-exit/return-value/take-void/exit") {
  int count              = 0;
  int exit_handler_count = 0;

  Error v = [&]() -> Error {
    ++count;
    std::string s = co_await on_exit(ErrorOr<std::string>("hello"), [&] {
      ++exit_handler_count;
      return Error(-3);
    });
    ++count;
    s = co_await on_exit(ErrorOr<std::string>(Error(-3)), [&] {
      ++exit_handler_count;
      return Error(-4);
    });
    ++count;
    co_return Error(3);
  }();

  NTH_EXPECT(v == Error(-4));
  NTH_EXPECT(count == 2);
  NTH_EXPECT(exit_handler_count == 1);
}

}  // namespace
}  // namespace nth
