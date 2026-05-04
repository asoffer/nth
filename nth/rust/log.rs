pub mod internal {
  #[repr(C)]
  #[derive(Clone, Copy)]
  pub struct CxxStaticStringView(&'static u8, usize);

  impl CxxStaticStringView {
    pub const fn new(s: &'static str) -> CxxStaticStringView {
      // length - 1, to account for the null-terminator added by `concat!`
      // before the call to this function.
      CxxStaticStringView(null_terminated(s), s.as_bytes().len() - 1)
    }
  }

  impl From<CxxStaticStringView> for &'static [u8] {
    fn from(sv: CxxStaticStringView) -> &'static [u8] {
      unsafe { std::slice::from_raw_parts(sv.0 as *const u8, sv.1) }
    }
  }

  #[repr(C, align(8))]
  pub struct Opaque(pub [u8; 24]);
  impl Opaque {
    pub const fn new() -> Opaque {
      Opaque([0u8; 24])
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
  pub file: &'static u8,
  pub function: &'static u8,
  pub line: u32,
}

#[repr(C)]
pub struct LogLine {
  pub verbosity_path: internal::CxxStaticStringView,
  pub location: LogLocation,
  pub opaque: internal::Opaque,
  pub enabled: std::sync::atomic::AtomicBool,
}

#[macro_export]
macro_rules! log_line {
  ($verbosity:literal ) => {
    log_line!($verbosity, line = ::std::line!())
  };
  ($verbosity:literal, line = $line:expr) => {
    $crate::log::LogLine {
      verbosity_path: $crate::log::internal::CxxStaticStringView::new(concat!(
        $verbosity, "\0"
      )),
      location: $crate::log::LogLocation {
        file: $crate::log::internal::null_terminated(concat!(::std::file!())),
        line: $line,
        function: $crate::log::internal::null_terminated(concat!(
          ::std::module_path!(),
          "\0"
        )),
      },
      opaque: $crate::log::internal::Opaque::new(),
      enabled: ::std::sync::atomic::AtomicBool::new(false),
    }
  };
}

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
  static LINE: LogLine = log_line!("verbosity", line = 17);

  #[test]
  fn one_line_registered() {
    assert_eq!(log::lines().len(), 1);
    let line = &log::lines()[0];
    assert_eq!(Into::<&'static [u8]>::into(line.verbosity_path), b"verbosity");
    assert_eq!(line.location.line, 17);
  }
}
