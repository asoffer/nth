#include "nth/debug/trace/internal/traversal_action.h"

#include <string_view>
#include <vector>

#include "nth/debug/trace/internal/formatter.h"
#include "nth/io/string_printer.h"

namespace nth::debug::internal_trace {

void TraverseTraced(bounded_string_printer &printer,
                    std::vector<TraversalAction> &&stack) {
  std::vector<std::string_view> chunks;

  formatter f;
  while (not stack.empty()) {
    auto action = stack.back();
    stack.pop_back();
    if (action.enter()) {
      if (not chunks.empty()) {
        chunks.back() = (chunks.back() == "\u2570\u2500 " ? "   " : "\u2502  ");
      }
      chunks.push_back("\u251c\u2500 ");
    } else if (action.expand()) {
      action.act(f, printer, stack);
    } else if (action.last()) {
      chunks.back() = "\u2570\u2500 ";
    } else if (action.exit()) {
      chunks.pop_back();
    } else {
      printer.write("    ");
      for (std::string_view chunk : chunks) { printer.write(chunk); }
      action.act(f, printer, stack);
      printer.write("\n");
    }
  }
}

}  // namespace nth::debug::internal_trace
