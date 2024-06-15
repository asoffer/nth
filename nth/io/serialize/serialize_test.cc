#include "nth/io/serialize/serialize.h"

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
  std::string value;
};

struct result_type {
  explicit result_type() : message_("unknown failure") {}
  explicit result_type(std::string message) : message_(std::move(message)) {}
  static result_type success() { return result_type(""); }

  operator bool() const { return message_.empty(); };

  friend bool operator==(result_type const&, result_type const&) = default;

  friend void NthPrint(auto& p, auto&, result_type const& r) {
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

struct BasicSerializer {
  BasicSerializer(std::string& s) : content_(s) {}
  void start_failing() { fail_ = true; }

  bool write(std::string_view str) {
    if (fail_) { return false; }
    content_.append(str);
    return true;
  }

  result_type write_with_result(std::string_view str) {
    if (fail_) {
      return result_type("Failed attempting to write: " + std::string(str));
    }
    content_.append(str);
    return result_type::success();
  }

  std::string& context(nth::type_tag<std::string>) { return content_; }

 private:
  std::string& content_;
  bool fail_ = false;
};

struct BoolReturningSerializer : BasicSerializer {
  using BasicSerializer::BasicSerializer;
};

static_assert(serializer_with_context<BoolReturningSerializer, std::string>);
static_assert(not serializer_with_context<BoolReturningSerializer, bool>);

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

struct ResultReturningSerializer : BasicSerializer {
  using nth_serializer_result_type = result_type;

  using BasicSerializer::BasicSerializer;
};

bool NthSerialize(BoolReturningSerializer& s, Thing const& t) {
  return s.write(t.value);
}

NTH_TEST("serialize/bool-result") {
  std::string content;
  BoolReturningSerializer s(content);
  Thing t1{.value = "hello"};
  Thing t2{.value = "world"};

  NTH_EXPECT(serialize(s, t1));
  NTH_EXPECT(content == "hello");

  NTH_EXPECT(serialize(s, t1));
  NTH_EXPECT(content == "hellohello");

  NTH_EXPECT(serialize(s, t1, t2));
  NTH_EXPECT(content == "hellohellohelloworld");

  s.start_failing();
  NTH_EXPECT(not serialize(s, t1, t2));
}

result_type NthSerialize(ResultReturningSerializer& s, Thing const& t) {
  return s.write_with_result(t.value);
}

NTH_TEST("serialize/custom-result") {
  std::string content;
  ResultReturningSerializer s(content);
  Thing t1{.value = "hello"};
  Thing t2{.value = "world"};

  NTH_EXPECT(serialize(s, t1) == true);
  NTH_EXPECT(content == "hello");

  NTH_EXPECT(serialize(s, t1) == true);
  NTH_EXPECT(content == "hellohello");

  NTH_EXPECT(serialize(s, t1, t2) == true);
  NTH_EXPECT(content == "hellohellohelloworld");

  s.start_failing();
  NTH_EXPECT(serialize(s, t1, t2) ==
             result_type("Failed attempting to write: hello"));
}

template <size_t N>
result_type NthSerialize(ResultReturningSerializer& s,
                         std::array<Thing, N> const& ts) {
  for (size_t i = 0; i < N; ++i) { co_await serialize(s, ts[i]); }
  co_return result_type::success();
}

NTH_TEST("serialize/coroutine") {
  std::string content;
  ResultReturningSerializer s(content);

  std::array<Thing, 2> things{
      Thing{.value = "hello"},
      Thing{.value = "world"},
  };

  std::array<Thing, 4> more_things{
      Thing{.value = "a"},
      Thing{.value = "b"},
      Thing{.value = "c"},
      Thing{.value = "d"},
  };

  NTH_EXPECT(serialize(s, things) == true);
  NTH_EXPECT(content == "helloworld");

  s.start_failing();
  NTH_EXPECT(serialize(s, more_things) == false);
}

}  // namespace
}  // namespace nth::io
