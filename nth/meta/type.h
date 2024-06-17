#ifndef NTH_META_TYPE_H
#define NTH_META_TYPE_H

#include <memory>
#include <type_traits>

#include "nth/meta/compile_time_string.h"
#include "nth/meta/concepts/convertible.h"
#include "nth/meta/sequence.h"

namespace nth {

struct type_id;

// `type_tag` is an empty type that enables encoding a type as a value. We use
// the variable template `type<T>` as a value of type `type_tag<T>`. This allows
// us to express compile-time queries about types in the more natural language
// of expressions, such as `type<X> == type<Y>`.
template <typename T>
struct type_tag final {
  using type = T;

  constexpr bool operator==(type_tag) const { return true; }
  template <typename R>
  constexpr bool operator==(type_tag<R>) const {
    return false;
  }

  template <typename R>
  constexpr bool operator!=(type_tag<R> m) const {
    return not operator==(m);
  }

  template <template <typename...> typename P>
  static constexpr bool is_a();

  constexpr operator type_id() const;

  template <nth::explicitly_convertible_to<T> U>
  static constexpr auto cast(U&& u) {
    return static_cast<T>(std::forward<U>(u));
  }

  static constexpr type_tag<std::decay_t<T>> decayed() { return {}; }

  static constexpr type_tag<std::remove_reference_t<T>> without_reference() {
    return {};
  }

  static constexpr type_tag<std::remove_const_t<T>> without_const() {
    return {};
  }
  static constexpr type_tag<std::remove_volatile_t<T>> without_volatile() {
    return {};
  }

  static constexpr size_t size() { return sizeof(T); }
  static constexpr size_t alignment() { return alignof(T); }

  static consteval auto name() {
    constexpr std::string_view indicator = "[T = ";
    constexpr compile_time_string str(__PRETTY_FUNCTION__);
    constexpr size_t index =
        std::string_view(str).find(indicator) + indicator.size();
    return str.template substr<index, str.size() - (index + 1)>();
  }

  static constexpr auto return_type() requires(std::is_function_v<T>);
  static constexpr auto parameters() requires(std::is_function_v<T>);

 private:
  static constexpr std::string_view get_name_impl() { return Name; }
  static constexpr auto Name = type_tag::name();
};

// `type_id` is an erased version of `type_tag<T>`. It stores a unique
// identifier so that two `type_id`s are equal precisely when they were
// constructed from the same `type_tag` instantiation. However, this comparison
// cannot be made at compile-time.
struct type_id final {
  constexpr type_id() = delete;

  friend constexpr bool operator==(type_id lhs, type_id rhs) = default;
  friend constexpr bool operator!=(type_id lhs, type_id rhs) = default;

  // TODO: We cannot name `io::format_spec` because that header depends on this
  // one.
  friend void NthFormat(auto& p, auto, type_id id) { p.write(id.id_()); }

 private:
  template <typename T>
  friend struct ::nth::type_tag;

  constexpr explicit type_id(std::string_view (*id)()) : id_(id) {}

  std::string_view (*id_)() = nullptr;
};

namespace internal_type {

template <typename>
struct FunctionSignature;

template <typename, template <typename...> typename>
struct IsAImpl : std::false_type {};
template <typename... TemplateArgs, template <typename...> typename P>
struct IsAImpl<P<TemplateArgs...>, P> : std::true_type {};

template <typename T>
std::true_type IsTypeImpl(type_tag<T> const*);
std::false_type IsTypeImpl(...);

template <typename R, typename... Parameters>
struct FunctionSignature<R(Parameters...)> {
  static constexpr type_tag<R> return_type;
  static constexpr auto parameters = sequence<type_tag<Parameters>{}...>;
};

}  // namespace internal_type

template <typename T>
constexpr auto type_tag<T>::return_type() requires(std::is_function_v<T>) {
  return internal_type::FunctionSignature<T>::return_type;
};

template <typename T>
constexpr auto type_tag<T>::parameters() requires(std::is_function_v<T>) {
  return internal_type::FunctionSignature<T>::parameters;
};

template <typename T>
template <template <typename...> typename P>
constexpr bool type_tag<T>::is_a() {
  return internal_type::IsAImpl<T, P>::value;
}

template <typename T>
constexpr type_tag<T>::operator type_id() const {
  return type_id(get_name_impl);
}

template <typename T>
concept Type =
    decltype(internal_type::IsTypeImpl(static_cast<T const*>(nullptr)))::value;

template <typename T>
inline constexpr type_tag<T> type;

template <typename... Ts>
inline constexpr auto type_sequence = sequence<type<Ts>...>;

template <Type auto t>
using type_t = typename decltype(t)::type;

// An alias which evaluates to its first argument. This alias is primarily
// useful in two common scenarios: First, when the you need to retrieve the
// first element of a pack (which could alternately be computed via
// `nth::type_t<nth::type_sequence<Pack...>.head()>`). Second, when you want to
// repeat the same type for each element of a pack, you can use
//
//   ```
//   nth::first_t<T, Pack>...
//   ```
// where you might otherwise need to coerce the cumbersome
//
//   ```
//   nth::type_sequence<Pack...>.template transform<[](auto) { return
//   nth::type<T>; }>();
//   ```
// back into a pack.
template <typename T, typename...>
using first_t = T;

}  // namespace nth

#endif  // NTH_META_TYPE_H
