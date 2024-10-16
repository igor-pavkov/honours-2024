ssize_t enc_untrusted_lgetxattr(const char *path, const char *name, void *value,
                                size_t size) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_lgetxattr,
                                             path, name, value, size);
}