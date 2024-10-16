void enc_untrusted_thread_wait(int32_t *const queue,
                               uint64_t timeout_microsec) {
  enc_untrusted_thread_wait_value(queue, kWaitQueueEnabled, timeout_microsec);
}