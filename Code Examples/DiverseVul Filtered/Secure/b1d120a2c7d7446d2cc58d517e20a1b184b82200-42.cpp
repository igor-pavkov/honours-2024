int enc_untrusted_fsync(int fd) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_fsync,
                                             fd);
}