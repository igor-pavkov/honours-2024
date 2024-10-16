void enc_untrusted_thread_wait_value(int32_t *const queue, int32_t value,
                                     uint64_t timeout_microsec) {
  enc_untrusted_sys_futex_wait(queue, value, timeout_microsec);
}