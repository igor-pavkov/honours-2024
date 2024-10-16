ssize_t enc_untrusted_write(int fd, const void *buf, size_t count) {
  return static_cast<ssize_t>(EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_write, fd, buf, count));
}