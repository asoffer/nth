#include "nth/strings/interpolate/interpolate.h"

#include <utility>

#include "nth/meta/type.h"

namespace nth {

struct point {
  friend constexpr auto NthDefaultInterpolationString(
      decltype(nth::type<point>)) {
    return interpolation_string("(x = {}, y = {})");
  }

  template <nth::io::printer_type P>
  friend auto NthFormat(P p, point const &pt) {
    return P::print(std::move(p), pt.x, pt.y);
  }
  int x = 1;
  int y = 2;
};

}  // namespace nth

int main() {
  nth::interpolating_printer<"{} {y = {x}, x = {}}", nth::file_printer> f(
      nth::stderr_printer);
  nth::point p{.x = 17, .y = 34};
  nth::io::print(std::move(f), p, p);
  return 0;
}
