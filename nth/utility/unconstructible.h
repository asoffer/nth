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
// int x = GetTheNumber();       // Ok. `x` is 5.
// int x = GetTheNumber({}, 7);  // Compilation error.
// ```
//
template <typename T>
struct unconstructible_except_by {
 private:
  friend T;
  unconstructible_except_by() = default;
};

}  // namespace nth

#endif  // NTH_UTILITY_UNCONSTRUCTIBLE_H
