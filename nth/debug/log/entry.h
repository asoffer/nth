#ifndef NTH_DEBUG_LOG_ENTRY_H
#define NTH_DEBUG_LOG_ENTRY_H

#include <string>
#include <string_view>
#include <vector>

namespace nth {

// TODO: Review API and document.
struct LogEntry {
  std::string& data() & { return data_; }

  void set_placeholder_count(size_t placeholders) {
    offsets_.reserve(1 + placeholders);
  }

  void demarcate() { offsets_.push_back(data_.size()); }

  struct const_iterator {
    const_iterator& operator++() {
      ++offset_;
      return *this;
    }
    const_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }

    std::string_view operator*() const {
      size_t start = *(offset_ - 1);
      size_t end   = *offset_;
      return std::string_view(s_ + start, end - start);
    }

    friend bool operator==(const_iterator const&,
                           const_iterator const&) = default;
    friend bool operator!=(const_iterator const&,
                           const_iterator const&) = default;

   private:
    friend LogEntry;

    explicit const_iterator(char const* s,
                            typename std::vector<size_t>::const_iterator o)
        : s_(s), offset_(o) {}

    char const* s_;
    typename std::vector<size_t>::const_iterator offset_;
  };

  const_iterator begin() const {
    return const_iterator(data_.data(), std::next(offsets_.begin()));
  }

  const_iterator end() const {
    return const_iterator(data_.data(), offsets_.end());
  }

 private:
  std::string data_;
  std::vector<size_t> offsets_ = {0};
};

}  // namespace nth

#endif  // NTH_DEBUG_LOG_ENTRY_H
