pub mod internal {
  #[repr(C)]
  pub struct CxxStringView(&'static u8, usize);

  impl CxxStringView {
    pub const fn new(s: &'static str) -> CxxStringView {
      // length - 1, to account for the null-terminator added by `concat!`
      // before the call to this function.
      CxxStringView(null_terminated(s), s.as_bytes().len() - 1)
    }
  }

  pub const fn null_terminated(s: &'static str) -> &'static u8 {
    unsafe { &*s.as_bytes().as_ptr() }
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
  file: &'static u8,
  function: &'static u8,
  line: u32,
}

#[repr(C)]
pub struct LogLine {
  verbosity_path: internal::CxxStringView,
  location: LogLocation,
  enabled: std::sync::atomic::AtomicBool,
}

const _: () = {
  assert!(std::mem::size_of::<LogLine>() == 48);
};

pub fn lines() -> &'static [LogLine] {
  let start = &raw const internal::__start_nth_log_line as *const LogLine;
  let end = &raw const internal::__stop_nth_log_line as *const LogLine;
  unsafe { std::slice::from_raw_parts(start, end.offset_from(start) as usize) }
}

#[cfg(test)]
mod tests {
  use crate::log::{self, LogLine, LogLocation, internal};
  use std::sync::atomic::AtomicBool;

  #[cfg_attr(target_vendor = "apple", unsafe(link_section = "__DATA,nth_log_line"))]
  #[cfg_attr(not(target_vendor = "apple"), unsafe(link_section = "nth_log_line"))]
  #[used]
  static LINE: LogLine = LogLine {
    verbosity_path: internal::CxxStringView::new("verbosity\0"),
    location: LogLocation {
      file: internal::null_terminated("file\0"),
      function: internal::null_terminated("function\0"),
      line: 17,
    },
    enabled: AtomicBool::new(false),
  };

  #[test]
  fn one_line_registered() {
    assert_eq!(log::lines().len(), 1);
    let line = &log::lines()[0];
    assert_eq!(line.location.line, 17);
    assert_eq!(line.location.line, 17);
  }
}
