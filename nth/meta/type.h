#ifndef NTH_META_TYPE_H
#define NTH_META_TYPE_H

#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>

#include "nth/meta/compile_time_string.h"
#include "nth/meta/sequence.h"

namespace nth {
namespace internal_type {

template <typename T>
struct Type;

template <typename>
struct FunctionSignature;

template <typename T>
void WriteTo(std::ostream& os);

}  // namespace internal_type

struct TypeId {
  TypeId() = delete;

  friend bool operator==(TypeId lhs, TypeId rhs) = default;
  friend bool operator!=(TypeId lhs, TypeId rhs) = default;

  template <typename S>
  friend void AbslStringify(S& sink, TypeId id) {
    std::stringstream ss;
    ss << id;
    sink.Append(ss.str());
  }

  friend std::ostream& operator<<(std::ostream& os, TypeId id) {
    id.id_(os);
    return os;
  }

 private:
  template <typename T>
  friend struct ::nth::internal_type::Type;

  explicit TypeId(void (*id)(std::ostream&)) : id_(id) {}

  void  (*id_)(std::ostream&) = 0;
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
  constexpr static bool is_a() {
    return IsAImpl<T, P>::value;
  }

  operator TypeId() const { return TypeId(&WriteTo<T>); }

  constexpr static Type<std::decay_t<T>> decayed() { return {}; }

  constexpr static size_t size() { return sizeof(T); }
  constexpr static size_t alignment() { return alignof(T); }

  constexpr static auto return_type() requires(std::is_function_v<T>) {
    return internal_type::FunctionSignature<T>::return_type;
  };

  constexpr static auto parameters() requires(std::is_function_v<T>) {
    return internal_type::FunctionSignature<T>::parameters;
  };

  constexpr static auto dependent(auto&& value) {
    return std::forward<decltype(value)>(value);
  }

  constexpr static auto name() {
    constexpr std::string_view indicator = "[T = ";
    constexpr CompileTimeString str(__PRETTY_FUNCTION__);
    constexpr size_t index =
        std::string_view(str).find(indicator) + indicator.size();
    return str.template substr<index, str.size() - (index + 1)>();
  }

  template <typename S>
  friend void AbslStringify(S& sink, Type) {
    std::stringstream ss;
    WriteTo<T>(ss);
    sink.Append(ss.str());
  }

  friend std::ostream& operator<<(std::ostream& os, Type) {
    WriteTo<T>(os);
    return os;
  }
};

template <typename T>
std::true_type IsTypeImpl(Type<T> const*);
std::false_type IsTypeImpl(...);

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

namespace internal_type {

template <typename R, typename... Parameters>
struct FunctionSignature<R(Parameters...)> {
  static constexpr auto return_type = type<R>;
  static constexpr auto parameters  = sequence<type<Parameters>...>;
};

template <typename T>
void WriteTo(std::ostream& os) {
  if constexpr (std::is_same_v<T, bool>) {
    os << "bool";
  } else if constexpr (std::is_same_v<T, char>) {
    os << "bool";
  } else if constexpr (std::is_same_v<T, signed char>) {
    os << "signed char";
  } else if constexpr (std::is_same_v<T, unsigned char>) {
    os << "unsigned char";
  } else if constexpr (std::is_same_v<T, short>) {
    os << "short";
  } else if constexpr (std::is_same_v<T, unsigned short>) {
    os << "unsigned short";
  } else if constexpr (std::is_same_v<T, int>) {
    os << "int";
  } else if constexpr (std::is_same_v<T, unsigned int>) {
    os << "unsigned int";
  } else if constexpr (std::is_same_v<T, long>) {
    os << "long";
  } else if constexpr (std::is_same_v<T, unsigned long>) {
    os << "unsigned long";
  } else if constexpr (std::is_same_v<T, long long>) {
    os << "long long";
  } else if constexpr (std::is_same_v<T, unsigned long long>) {
    os << "unsigned long long";
  } else if constexpr (std::is_const_v<T>) {
    WriteTo<std::remove_const_t<T>>(os);
    os << " const";
  } else if constexpr (std::is_volatile_v<T>) {
    WriteTo<std::remove_volatile_t<T>>(os);
    os << " volatile";
  } else if constexpr (std::is_pointer_v<T>) {
    WriteTo<std::remove_pointer_t<T>>(os);
    os << "*";
  } else if constexpr (std::is_lvalue_reference_v<T>) {
    WriteTo<std::remove_reference_t<T>>(os);
    os << "&";
  } else if constexpr (std::is_rvalue_reference_v<T>) {
    WriteTo<std::remove_reference_t<T>>(os);
    os << "&&";
  } else {
    os << ::nth::type<T>.name();
  }
}

}  // namespace internal_type
}  // namespace nth

#endif  // NTH_META_TYPE_H
