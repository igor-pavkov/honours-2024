extern "C" void enc_set_error_handler(
    void (*abort_handler)(const char *message)) {
  error_handler = abort_handler;
}