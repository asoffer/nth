#ifndef NTH_UTILITY_UNCONSTRUCTIBLE_H
#define NTH_UTILITY_UNCONSTRUCTIBLE_H

namespace nth {

// An empty type which is only constructible by its template parameter `T`. The
// purpose of such a type is to aid in ensuring that certain default arguments
// to functions are not specified explicitly.
//
// Specifically, a member function for a struct `MyType`, may accept an
// `unconstructible_except_by<MyType>` and provide a default of `{}` for it. For
// example,
//
// ```
// struct MyType {
//   static int GetTheNumber(nth::unconstructible_except_by<MyType> = {},
//                           int default_value = 5) {
//     return default_value;
//   }
// };
//
// int x = MyType::GetTheNumber();       // Ok. `x` is 5.
// int x = MyType::GetTheNumber({}, 7);  // Compilation error.
// ```
//
// While as a general rule, we believe it is preferrable to implement this
// behavior as an overload set where `GetTheNumber()` is public and
// `GetTheNumber(int)` is private to `MyType`, there are situatinos where this
// is infeasible. In particular, if the storage allocated by the needs to live
// at least as long as the return value, a default parameter is the only
// mechanism by which function authors can allocate storage in the caller. Even
// this is frought, as the storage will be temporary, but there are times where
// this technique is valuable. Such uses should likely also use
// `NTH_ATTRIBUTE(lifetimebound)` to ensure callers do not reference the storage
// after destruction.
template <typename T>
struct unconstructible_except_by {
 private:
  friend T;
  constexpr unconstructible_except_by() = default;
};

}  // namespace nth

#endif  // NTH_UTILITY_UNCONSTRUCTIBLE_H
