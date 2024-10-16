void enc_untrusted_notify(int32_t *const queue, int32_t num_threads) {
  enc_untrusted_sys_futex_wake(queue, num_threads);
}