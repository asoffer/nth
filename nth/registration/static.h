#ifndef NTH_REGISTRATION_STATIC_H
#define NTH_REGISTRATION_STATIC_H

namespace nth {

// `registration_token`, in conjunction with `odr_use` defined below, can be used
// to ensure that a funciton is run as part of initialization before `main`.
// Specifically, one should use the following pattern:
//
// ```
// nth::registration_token const token =
//   function_to_be_executed_during_initialization;
// nth::odr_use<&token> use_token;
// ```
//
// Remember to take care to declare these objects properly `inline` or `static`
// depending on the appropriate context.
struct registration_token {
  registration_token(auto f) { f(); }
};

// `odr_use` is a class template whose parameter is a `registration_token const*`.
// It does nothing other than ensure that the instantiation of `odr_use` ODR-uses
// the `registration_token` pointed to by the template parameter. It's primary
// use is as defined above, in conjuction with `registration_token`.
template <registration_token const *Addr>
struct odr_use {
  constexpr odr_use() : t_(*Addr) {}

 private:
  const registration_token &t_;
};

}  // namespace nth

#endif  // NTH_REGISTRATION_STATIC_H
