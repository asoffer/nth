SECTIONS {
  nth_contract : {
    __start_nth_contract = .;
    KEEP(*(nth_contract))
    __stop_nth_contract = .;
  }
} INSERT AFTER .data;
