void enc_untrusted_wait_queue_set_value(int32_t *const queue, int32_t value) {
  TrustedPrimitives::UntrustedLocalMemcpy(queue, &value, sizeof(int32_t));
}