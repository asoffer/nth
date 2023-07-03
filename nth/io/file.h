#ifndef NTH_IO_FILE_H
#define NTH_IO_FILE_H

#include <cstdio>
#include <span>
#include <utility>

namespace nth {

// Represents a handle to a file on the underlying operating system.
struct file {
  // Constructs a file object not associated with any underlying system file.
  constexpr file() = default;

  file(file const&)            = delete;
  file& operator=(file const&) = delete;

  // Moves the underlying file handle from `f` to `*this`. Afterwards,
  // `static_cast<bool>(f)` is guaranteed to be true.
  file(file&& f) : file_ptr_(std::exchange(f.file_ptr_, nullptr)) {}

  // Moves the underlying file handle from `f` to `*this`. Afterwards,
  // `static_cast<bool>(f)` is guaranteed to be true.
  file& operator=(file&& f);

  // Destroys the file. The member function `close` must already have been
  // invoked on `*this`.
  ~file();

  // Returns the file position indicator.
  size_t tell() const;

  // Resets the underlying file pointer to the start of the file.
  void rewind();

  // Attempts to read up to `std::span(buffer).size()` bytes into `buffer`, and
  // returns a span which is a prefix of `buffer` consisting of the bytes read.
  [[nodiscard]] auto read_into(auto& buffer) requires(
      requires { std::span(buffer); } and
      std::is_trivially_copyable_v<
          typename decltype(std::span(buffer))::element_type>) {
    using type = typename decltype(std::span(buffer))::element_type;
    return read_into_impl(std::span<type>(buffer));
  }

  // Returns the underlying `std::FILE*`.
  std::FILE* get() { return file_ptr_; }

  // Returns whether the underlying file handle is set.
  explicit operator bool() const { return file_ptr_ != nullptr; }

  // Attempts to close the file, returning a bool indicating whether the
  // operation succeeded.
  [[nodiscard]] bool close();

 private:
  template <typename T>
  [[nodiscard]] auto read_into_impl(std::span<T> buffer) {
    size_t count =
        std::fread(buffer.data(), sizeof(T), buffer.size(), file_ptr_);
    return buffer.subspan(0, count);
  }

  friend file TemporaryFile();

  explicit file(std::FILE* f) : file_ptr_(f) {}

  std::FILE* file_ptr_ = nullptr;
};

// Creates a new temporary file with a unique name. The file will be be
// destoryed when `close` is invoked on the returned object.
file TemporaryFile();

}  // namespace nth

#endif  // NTH_IO_FILE_H
