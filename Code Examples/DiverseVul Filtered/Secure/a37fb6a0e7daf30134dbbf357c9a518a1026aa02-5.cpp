void enc_untrusted_disable_waiting(int32_t *const queue) {
  enc_untrusted_wait_queue_set_value(queue, kWaitQueueDisabled);
}