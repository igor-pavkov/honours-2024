off_t enc_untrusted_lseek(int fd, off_t offset, int whence) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_lseek, fd,
                                             offset, whence);
}