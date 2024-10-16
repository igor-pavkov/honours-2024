ssize_t enc_untrusted_listxattr(const char *path, char *list, size_t size) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_listxattr,
                                             path, list, size);
}