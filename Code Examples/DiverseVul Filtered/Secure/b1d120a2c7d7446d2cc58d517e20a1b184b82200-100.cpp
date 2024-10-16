int enc_untrusted_ftruncate(int fd, off_t length) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_ftruncate,
                                             fd, length);
}