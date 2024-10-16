int enc_untrusted_fchmod(int fd, mode_t mode) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_fchmod,
                                             fd, mode);
}