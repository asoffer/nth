#include "nth/io/deserialize/deserialize.h"

#include <array>
#include <coroutine>
#include <cstddef>
#include <string>
#include <utility>

#include "nth/base/platform.h"
#include "nth/test/test.h"

namespace nth::io {
namespace {

struct Thing {
  int value = 0;
};

struct BasicDeserializer {
  int get_value() { return value_++; }

 private:
  int value_ = 0;
};

struct BoolReturningDeserializer : BasicDeserializer {
  std::string& context(nth::type_tag<std::string>) { return context_; }

 private:
  std::string context_;
};

static_assert(deserializer_with_context<BoolReturningDeserializer, std::string>);
static_assert(not deserializer_with_context<BoolReturningDeserializer, bool>);

struct result_type {
  explicit result_type() : message_("unknown failure") {}
  explicit result_type(std::string message)
      : message_(std::move(message)) {}
  static result_type success() { return result_type(""); }

  operator bool() const { return message_.empty(); };

  friend void NthPrint(auto& p, auto& , result_type const &r) {
    p.write("result[");
    p.write(r.message_);
    p.write("]");
  }

  struct promise_type;

  bool await_ready() { return static_cast<bool>(*this); }
  void await_suspend(std::coroutine_handle<promise_type>);
  static constexpr void await_resume() {}

  private:
   friend struct result_promise_return_type;

   result_type(result_type*& location) { location = this; }
   std::string message_;
};

struct result_promise_return_type {
  result_promise_return_type(result_type::promise_type& promise);
  result_promise_return_type(result_promise_return_type&&)            = delete;
  result_promise_return_type(result_promise_return_type const&)       = delete;
  result_promise_return_type& operator=(result_promise_return_type&&) = delete;
  result_promise_return_type& operator=(result_promise_return_type const&) =
      delete;
  operator result_type() {
    // TODO: This assumes eager conversion `get_return_object()` to the actual
    // result type.
    return result_type(pointer_);
  }

 private:
  result_type*& pointer_;
};

struct result_type::promise_type {
  result_promise_return_type get_return_object() { return *this; }
  static constexpr std::suspend_never initial_suspend() noexcept { return {}; }
  static constexpr std::suspend_never final_suspend() noexcept { return {}; }
  static constexpr void unhandled_exception() noexcept {}
  void return_value(result_type value) { set(std::move(value)); }

  void set(result_type result) { *value_ = std::move(result); }

 private:
  friend result_promise_return_type;
  result_type* value_;
};

result_promise_return_type::result_promise_return_type(
    result_type::promise_type& promise)
    : pointer_(promise.value_) {}

void result_type::await_suspend(std::coroutine_handle<promise_type> h) {
  h.promise().set(std::move(*this));
  h.destroy();
}

struct ResultReturningDeserializer : BasicDeserializer {
  using nth_deserializer_result_type = result_type;
};

bool NthDeserialize(BoolReturningDeserializer& d, Thing& t) {
  t.value = d.get_value();
  return t.value < 5;
}

NTH_TEST("deserialize/bool-result") {
  BoolReturningDeserializer d;
  Thing t1{.value = -1};
  Thing t2{.value = -1};

  NTH_EXPECT(deserialize(d, t1));
  NTH_EXPECT(t1.value == 0);

  NTH_EXPECT(deserialize(d, t1));
  NTH_EXPECT(t1.value == 1);

  NTH_EXPECT(deserialize(d, t1, t2));
  NTH_EXPECT(t1.value == 2);
  NTH_EXPECT(t2.value == 3);

  NTH_EXPECT(not deserialize(d, t1, t2));
}

result_type NthDeserialize(ResultReturningDeserializer& d, Thing& t) {
  t.value = d.get_value();
  return result_type(t.value < 5 ? "" : "failure");
}

NTH_TEST("deserialize/custom-result") {
  ResultReturningDeserializer d;
  Thing t1{.value = -1};
  Thing t2{.value = -1};

  NTH_EXPECT(deserialize(d, t1) == true);
  NTH_EXPECT(t1.value == 0);

  NTH_EXPECT(deserialize(d, t1) == true);
  NTH_EXPECT(t1.value == 1);

  NTH_EXPECT(deserialize(d, t1, t2) == true);
  NTH_EXPECT(t1.value == 2);
  NTH_EXPECT(t2.value == 3);

  NTH_EXPECT(deserialize(d, t1, t2) == false);
}

template <size_t N>
result_type NthDeserialize(ResultReturningDeserializer& d,
                           std::array<Thing, N>& ts) {
  for (size_t i = 0; i < N; ++i) { co_await deserialize(d, ts[i]); }
  co_return result_type::success();
}

NTH_TEST("deserialize/coroutine") {
  ResultReturningDeserializer d;
  std::array<Thing, 2> things{
      Thing{.value = -1},
      Thing{.value = -1},
  };

  std::array<Thing, 4> more_things{
      Thing{.value = -1},
      Thing{.value = -1},
      Thing{.value = -1},
      Thing{.value = -1},
  };

  NTH_EXPECT(deserialize(d, things) == true);
  NTH_EXPECT(things[0].value == 0);
  NTH_EXPECT(things[1].value == 1);

  NTH_EXPECT(deserialize(d, more_things) == false);
}

}  // namespace
}  // namespace nth::io
