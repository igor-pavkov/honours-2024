extern "C" bool enc_is_syscall_dispatcher_set() {
  return global_syscall_callback != nullptr;
}