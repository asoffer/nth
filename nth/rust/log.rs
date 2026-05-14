pub mod internal {
  #[repr(C, align(8))]
  pub struct Opaque(pub [u8; 24]);
  impl Opaque {
    pub const fn new() -> Opaque {
      Opaque([0u8; 24])
    }
  }

  #[cfg(not(target_vendor = "apple"))]
  unsafe extern "C" {
    pub(crate) static __start_nth_log_line: crate::log::LogLine;
    pub(crate) static __stop_nth_log_line: crate::log::LogLine;
  }

  #[cfg(target_vendor = "apple")]
  unsafe extern "C" {
    #[link_name = "\x01section$start$__DATA$nth_log_line"]
    pub(crate) static __start_nth_log_line: crate::log::LogLine;
    #[link_name = "\x01section$end$__DATA$nth_log_line"]
    pub(crate) static __stop_nth_log_line: crate::log::LogLine;
  }
}

#[repr(C)]
pub struct LogLocation {
  pub file: &'static u8,
  pub function: &'static u8,
  pub line: u32,
}

#[repr(C)]
pub struct LogLine {
  pub verbosity_path: crate::cxx::StaticStringView,
  pub location: LogLocation,
  pub opaque: internal::Opaque,
  pub enabled: std::sync::atomic::AtomicBool,
}

#[macro_export]
macro_rules! internal_log_line {
  ($verbosity:literal) => {
    $crate::internal_log_line!($verbosity, line = ::std::line!())
  };
  ($verbosity:literal, line = $line:expr) => {
    $crate::log::LogLine {
      verbosity_path: $crate::cxx::null_terminated!($verbosity),
      location: $crate::log::LogLocation {
        file: $crate::cxx::null_terminated!(::std::file!()).data(),
        line: $line,
        function: $crate::cxx::null_terminated!(::std::module_path!()).data(),
      },
      opaque: $crate::log::internal::Opaque::new(),
      enabled: ::std::sync::atomic::AtomicBool::new(false),
    }
  };
}
pub use crate::internal_log_line as line;

const _: () = {
  assert!(std::mem::size_of::<LogLine>() == 72);
};

pub fn lines() -> &'static [LogLine] {
  let start = &raw const internal::__start_nth_log_line as *const LogLine;
  let end = &raw const internal::__stop_nth_log_line as *const LogLine;
  unsafe { std::slice::from_raw_parts(start, end.offset_from(start) as usize) }
}

#[cfg(test)]
mod tests {
  use crate::log::LogLine;
  use crate::log::{self};

  #[cfg_attr(
    target_vendor = "apple",
    unsafe(link_section = "__DATA,nth_log_line")
  )]
  #[cfg_attr(
    not(target_vendor = "apple"),
    unsafe(link_section = "nth_log_line")
  )]
  #[used]
  static LINE: LogLine = log::line!("verbosity", line = 17);

  #[test]
  fn one_line_registered() {
    assert_eq!(log::lines().len(), 1);
    let line = &log::lines()[0];
    assert_eq!(Into::<&'static [u8]>::into(line.verbosity_path), b"verbosity");
    assert_eq!(line.location.line, 17);
  }
}
