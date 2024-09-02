#ifndef NTH_TYPE_EXTEND_INTERNAL_FORMAT_H
#define NTH_TYPE_EXTEND_INTERNAL_FORMAT_H

#include "nth/container/stack.h"
#include "nth/debug/unreachable.h"
#include "nth/types/structure.h"

namespace nth::ext::internal_format {

template <typename T>
struct entry {
  std::string_view key;
  T const &value;
};

struct basic_structural_formatter {
  explicit basic_structural_formatter(std::string_view separator = ",")
      : separator_(separator) {}

 protected:
  std::string_view separator_;
  struct nesting {
    char open[2];
    char close[2];
    int width;
  };
  nth::stack<nesting> nesting_;
};

}  // namespace nth::ext::internal_format

#endif  // NTH_TYPE_EXTEND_INTERNAL_FORMAT_H
