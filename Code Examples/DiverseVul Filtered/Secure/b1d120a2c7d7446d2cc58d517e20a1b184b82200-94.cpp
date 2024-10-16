int enc_untrusted_lsetxattr(const char *path, const char *name,
                            const void *value, size_t size, int flags) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_lsetxattr,
                                             path, name, value, size, flags);
}