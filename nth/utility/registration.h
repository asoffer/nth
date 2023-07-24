#ifndef NTH_UTILITY_REGISTRATION_H
#define NTH_UTILITY_REGISTRATION_H

namespace nth {

// `RegistrationToken`, in conjunction with `OdrUse` defined below, can be used
// to ensure that a funciton is run as part of initialization before `main`.
// Specifically, one should use the following pattern:
//
// ```
// nth::RegistrationToken const token =
//   function_to_be_executed_during_initialization;
// nth::OdrUse<&token> use_token;
// ```
//
// Remember to take care to declare these objects properly `inline` or `static`
// depending on the appropriate context.
struct RegistrationToken {
  RegistrationToken(auto f) { f(); }
};

// `OdrUse` is a class template whose parameter is a `RegistrationToken const*`.
// It does nothing other than ensure that the instantiation of `OdrUse` ODR-uses
// the `RegistrationToken` pointed to by the template parameter. It's primary
// use is as defined above, in conjuction with `RegistrationToken`.
template <RegistrationToken const *Addr>
struct OdrUse {
  constexpr OdrUse() : t_(*Addr) {}

 private:
  const RegistrationToken &t_;
};

}  // namespace nth

#endif  // NTH_UTILITY_REGISTRATION_H
