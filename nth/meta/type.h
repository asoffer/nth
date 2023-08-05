#ifndef NTH_META_TYPE_H
#define NTH_META_TYPE_H

#include <iosfwd>
#include <memory>
#include <type_traits>

#include "nth/meta/compile_time_string.h"
#include "nth/meta/sequence.h"

namespace nth {
namespace internal_type {

template <typename T>
struct Type;

template <typename>
struct FunctionSignature;

}  // namespace internal_type

struct TypeId {
  constexpr TypeId() = delete;

  friend constexpr bool operator==(TypeId lhs, TypeId rhs) = default;
  friend constexpr bool operator!=(TypeId lhs, TypeId rhs) = default;

  template <typename S>
  friend void AbslStringify(S& sink, TypeId id) {
    sink.Append(id.id_());
  }

  friend std::ostream& operator<<(std::ostream& os, TypeId id) {
    return os << id.id_();
  }

 private:
  template <typename T>
  friend struct ::nth::internal_type::Type;

  constexpr explicit TypeId(std::string_view (*id)()) : id_(id) {}

  std::string_view (*id_)() = 0;
};

namespace internal_type {

template <typename, template <typename...> typename>
struct IsAImpl : std::false_type {};
template <typename... TemplateArgs, template <typename...> typename P>
struct IsAImpl<P<TemplateArgs...>, P> : std::true_type {};

template <typename T>
struct Type {
  using type = T;

  template <typename R>
  constexpr bool operator==(Type<R>) const {
    return std::is_same_v<T, R>;
  }

  template <typename R>
  constexpr bool operator!=(Type<R> m) const {
    return not operator==(m);
  }

  template <template <typename...> typename P>
  static constexpr bool is_a() {
    return IsAImpl<T, P>::value;
  }

  constexpr operator TypeId() const {
    return TypeId(+[] { return std::string_view(Name); });
  }

  static constexpr Type<std::decay_t<T>> decayed() { return {}; }

  static constexpr Type<std::remove_reference_t<T>> without_reference() {
    return {};
  }

  static constexpr Type<std::remove_const_t<T>> without_const() { return {}; }
  static constexpr Type<std::remove_volatile_t<T>> without_volatile() {
    return {};
  }

  static constexpr size_t size() { return sizeof(T); }
  static constexpr size_t alignment() { return alignof(T); }

  static constexpr auto return_type() requires(std::is_function_v<T>) {
    return internal_type::FunctionSignature<T>::return_type;
  };

  static constexpr auto parameters() requires(std::is_function_v<T>) {
    return internal_type::FunctionSignature<T>::parameters;
  };

  static constexpr auto dependent(auto&& value) {
    return std::forward<decltype(value)>(value);
  }

  static consteval auto name() {
    constexpr std::string_view indicator = "[T = ";
    constexpr CompileTimeString str(__PRETTY_FUNCTION__);
    constexpr size_t index =
        std::string_view(str).find(indicator) + indicator.size();
    return str.template substr<index, str.size() - (index + 1)>();
  }

  template <typename S>
  friend void AbslStringify(S& sink, Type) {
    sink.Append(std::string_view(name()));
  }

  friend std::ostream& operator<<(std::ostream& os, Type) {
    return os << std::string_view(name());
  }

  private:
   static constexpr auto Name = Type::name();
};

template <typename T>
std::true_type IsTypeImpl(Type<T> const*);
std::false_type IsTypeImpl(...);

template <typename R, typename... Parameters>
struct FunctionSignature<R(Parameters...)> {
  static constexpr Type<R> return_type;
  static constexpr auto parameters  = sequence<Type<Parameters>{}...>;
};

}  // namespace internal_type

template <typename T>
concept Type =
    decltype(internal_type::IsTypeImpl(static_cast<T const*>(nullptr)))::value;

template <typename T>
inline constexpr internal_type::Type<T> type;

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
//   nth::type_sequence<Pack...>.template transform<[](auto) { return nth::type<T>; }>();
//   ```
// back into a pack.
template <typename T, typename...>
using first_t = T;

}  // namespace nth

#endif  // NTH_META_TYPE_H
