void enc_untrusted_destroy_wait_queue(int32_t *const queue) {
  // This is a no op on purpose. Wait queue pointers are now
  // registered to be freed on enclave exit.
}