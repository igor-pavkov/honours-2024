ssize_t enc_untrusted_llistxattr(const char *path, char *list, size_t size) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_llistxattr, path, list, size);
}