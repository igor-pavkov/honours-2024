ssize_t enc_untrusted_flistxattr(int fd, char *list, size_t size) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_flistxattr, fd, list, size);
}