ssize_t enc_untrusted_getxattr(const char *path, const char *name, void *value,
                               size_t size) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_getxattr,
                                             path, name, value, size);
}