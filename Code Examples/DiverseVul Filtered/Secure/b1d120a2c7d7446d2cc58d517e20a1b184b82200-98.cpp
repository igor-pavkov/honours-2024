int enc_untrusted_setxattr(const char *path, const char *name,
                           const void *value, size_t size, int flags) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_setxattr,
                                             path, name, value, size, flags);
}