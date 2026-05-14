/// A type intended to be ABI-compatible with C++ `std::string_view`, viewing
/// string contents that are available for the lifetime of the program.
///
/// Note that C++ provides no ABI guarantees whatsoever, and implementations are
/// free to implement `std::string_view` in numerous different ABI-incompatible
/// ways, so it is not possible to guarantee this behavior according to any
/// language specifications. Rather, we rely on tests on each particular
/// supported platform to ensure correctness.
#[repr(C)]
#[derive(Clone, Copy)]
pub struct StaticStringView(&'static u8, usize);

#[macro_export]
macro_rules! internal_null_terminated {
  ($lit:expr) => {
    unsafe {
      $crate::cxx::StaticStringView::new_null_terminated_unchecked(concat!(
        $lit, "\0"
      ))
    }
  };
}
pub use internal_null_terminated as null_terminated;

impl StaticStringView {
  pub const fn new(s: &'static str) -> StaticStringView {
    let bytes = s.as_bytes();
    StaticStringView(unsafe { &*bytes.as_ptr() }, bytes.len())
  }

  pub const unsafe fn new_null_terminated_unchecked(
    s: &'static str,
  ) -> StaticStringView {
    let bytes = s.as_bytes();
    StaticStringView(unsafe { &*bytes.as_ptr() }, bytes.len() - 1)
  }

  pub const fn data(self) -> &'static u8 {
    &self.0
  }
}

impl From<StaticStringView> for &'static [u8] {
  fn from(sv: StaticStringView) -> &'static [u8] {
    unsafe { std::slice::from_raw_parts(sv.0 as *const u8, sv.1) }
  }
}

#[cfg(test)]
mod tests {
  use crate::cxx::StaticStringView;
  use crate::cxx::{self};

  fn bytes(sv: StaticStringView) -> &'static [u8] {
    sv.into()
  }

  #[test]
  fn new_preserves_length_and_contents() {
    assert_eq!(bytes(StaticStringView::new("hello")), b"hello");
  }

  #[test]
  fn new_with_embedded_nulls() {
    assert_eq!(bytes(StaticStringView::new("ab\0cd")), b"ab\0cd");
  }

  #[test]
  fn new_null_terminated_unchecked_strips_terminator() {
    let sv =
      unsafe { StaticStringView::new_null_terminated_unchecked("hello\0") };
    assert_eq!(bytes(sv), b"hello");
  }

  #[test]
  fn new_null_terminated_unchecked_with_empty_payload() {
    let sv = unsafe { StaticStringView::new_null_terminated_unchecked("\0") };
    assert_eq!(bytes(sv), b"");
  }

  #[test]
  fn null_terminated_macro_excludes_terminator_from_view() {
    let sv = cxx::null_terminated!("hello");
    assert_eq!(bytes(sv), b"hello");
  }

  #[test]
  fn null_terminated_macro_with_empty_literal() {
    let sv = cxx::null_terminated!("");
    assert_eq!(bytes(sv), b"");
  }

  #[test]
  fn null_terminated_macro_writes_null_after_payload() {
    // The macro relies on `concat!($lit, "\0")` and a length-1 view, so the
    // byte just past the view's payload must be the null terminator.
    let sv = cxx::null_terminated!("abc");
    let payload = bytes(sv);
    assert_eq!(payload, b"abc");
    let past_end = unsafe { *payload.as_ptr().add(payload.len()) };
    assert_eq!(past_end, 0);
  }

  #[test]
  fn copy_yields_equivalent_view() {
    let sv = StaticStringView::new("copyable");
    let dup = sv;
    assert_eq!(bytes(sv), bytes(dup));
  }

  // Defined in `cxx_abi_test.cc` as `std::string_view const "abcdef"`. If
  // `StaticStringView` is ABI-compatible with `std::string_view` on this
  // platform, reading the same memory through `StaticStringView` yields the
  // same payload.
  unsafe extern "C" {
    static nth_rust_cxx_abi_test_view: StaticStringView;
  }

  #[test]
  fn abi_compatible_with_cxx_string_view() {
    assert_eq!(bytes(unsafe { nth_rust_cxx_abi_test_view }), b"abcdef");
  }
}
