#include <string_view>
#include <vector>

namespace nth {
struct LogEntry;

namespace internal_debug {

struct log_entry_component_iterator {
  log_entry_component_iterator& operator++();
  log_entry_component_iterator operator++(int);
  std::string_view operator*() const;

  friend bool operator==(log_entry_component_iterator const&,
                         log_entry_component_iterator const&) = default;
  friend bool operator!=(log_entry_component_iterator const&,
                         log_entry_component_iterator const&) = default;

 private:
  friend nth::LogEntry;

  using vector_iterator = typename std::vector<size_t>::const_iterator;

  explicit log_entry_component_iterator(char const* s, vector_iterator o)
      : s_(s), offset_(o) {}

  char const* s_;
  vector_iterator offset_;
};

}  // namespace internal_debug
}  // namespace nth
