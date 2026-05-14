#include <string_view>

// Defines a `std::string_view` with C linkage so that Rust can read it through
// `nth::cxx::StaticStringView` and verify the two types are ABI-compatible.
// See `cxx.rs`.
extern "C" std::string_view const nth_rust_cxx_abi_test_view = "abcdef";
