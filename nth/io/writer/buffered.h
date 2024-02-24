#ifndef NTH_IO_WRITER_BUFFERED_H
#define NTH_IO_WRITER_BUFFERED_H

#include <cstddef>
#include <cstring>
#include <memory>

#include "nth/base/attributes.h"
#include "nth/debug/debug.h"
#include "nth/io/writer/writer.h"

namespace nth::io {

// Writes data to a buffer, only flushing the buffer to the underlying buffer
// when the buffer is filled.
template <nth::io::writer W>
struct buffered_writer {
  using underlying_writer_type = W;
  using cursor_type            = ptrdiff_t;

  explicit buffered_writer(size_t buffer_size,
                           W& w NTH_ATTRIBUTE(lifetimebound));

  std::optional<cursor_type> allocate(size_t n);

  cursor_type cursor() const;

  bool write(std::span<std::byte const> data);
  bool write_at(cursor_type c, std::span<std::byte const> data);

  bool flush();

  ~buffered_writer();

 private:
  size_t remaining_buffer_capacity() const {
    return buffer_size_ - (head_ - buffer_.get());
  }

  std::unique_ptr<std::byte[]> buffer_;
  std::byte* head_;
  size_t buffer_size_;
  underlying_writer_type& writer_;
};

// Implementation

template <nth::io::writer W>
buffered_writer<W>::buffered_writer(size_t buffer_size,
                                 W& w NTH_ATTRIBUTE(lifetimebound))
    : buffer_(new std::byte[buffer_size]),
      head_(buffer_.get()),
      buffer_size_(buffer_size),
      writer_(w) {}

template <nth::io::writer W>
std::optional<typename buffered_writer<W>::cursor_type>
buffered_writer<W>::allocate(size_t n) {
  if (n < remaining_buffer_capacity()) {
    auto c = cursor();
    head_ += n;
    return c;
  } else if (n < buffer_size_) {
    if (not flush()) { return std::nullopt; }
    auto c = cursor();
    head_ += n;
    return c;
  } else {
    return std::nullopt;
  }
}

template <nth::io::writer W>
buffered_writer<W>::cursor_type buffered_writer<W>::cursor() const {
  return writer_.cursor() + (head_ - buffer_.get());
}

template <nth::io::writer W>
bool buffered_writer<W>::write(std::span<std::byte const> data) {
  if (data.size() < remaining_buffer_capacity()) {
    std::memcpy(head_, data.data(), data.size());
    head_ += data.size();
    return true;
  } else if (data.size() < buffer_size_) {
    if (not flush()) { return false; }
    std::memcpy(buffer_.get(), data.data(), data.size());
    head_ = buffer_.get() + data.size();
    return true;
  } else {
    std::span<std::byte const> buffered(buffer_.get(), head_);
    if (not writer_.write(buffered)) { return false; }
    if (not writer_.write(data)) { return false; }
    head_ = buffer_.get();
    return true;
  }
}

template <nth::io::writer W>
bool buffered_writer<W>::write_at(cursor_type, std::span<std::byte const>) {
  NTH_UNIMPLEMENTED();
}

template <nth::io::writer W>
bool buffered_writer<W>::flush() {
  std::span<std::byte const> buffered(buffer_.get(), head_);
  head_ = buffer_.get();
  return writer_.write(buffered);
}

template <nth::io::writer W>
buffered_writer<W>::~buffered_writer() {
  NTH_REQUIRE((v.harden), buffer_.get() == head_);
}

}  // namespace nth::io

#endif  // NTH_IO_WRITER_BUFFERED_H
