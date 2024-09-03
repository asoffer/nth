# `//nth/process:env`

This target provides a mechanism to safely read and write environment variables. The function
`nth::env::load` returns the value of the enivronment variable if it is set and returns
`std::nullopt` otherwise. The `nth::env::store` overload set sets, unsets or resets an environment
variable.

Calls to `nth::env::load` and `nth::env::store` are thread-safe (they may be called without
external synchronization). Notably this is not true of `std::getenv` and `::setenv` which manipulate
globals without synchronization. It is imporatnt to note that the thread-safety does not extend to
`std::getenv` or `::setenv`. If any of these functions are also invoked without
synchronization against `nth::env::load` and `nth::env::store`, the behavior is undefined.
