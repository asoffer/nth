SECTIONS {
  nth_log_line : {
    __start_nth_log_line = .;
    KEEP(*(nth_log_line))
    __stop_nth_log_line = .;
  }
} INSERT AFTER .data;
