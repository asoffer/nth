#include <concepts>
#include <cstddef>
#include <type_traits>

namespace nth {

// A concept matching anything that can hash a `T`. Specifically, any object
// invocable with a `T` returning a type convertible to `size_t`.
template <typename H, typename T>
concept hasher = std::invocable<H, T> and
    std::convertible_to<std::invoke_result_t<H, T>, size_t>;

}  // namespace nth
