extern "C" void enc_set_dispatch_syscall(syscall_dispatch_callback callback) {
  global_syscall_callback = callback;
}