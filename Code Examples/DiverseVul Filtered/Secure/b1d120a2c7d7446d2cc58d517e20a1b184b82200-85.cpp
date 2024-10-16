int enc_untrusted_epoll_create(int size) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_epoll_create, size);
}