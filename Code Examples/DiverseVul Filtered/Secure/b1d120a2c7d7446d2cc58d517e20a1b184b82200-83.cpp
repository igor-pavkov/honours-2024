int enc_untrusted_fsetxattr(int fd, const char *name, const void *value,
                            size_t size, int flags) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_fsetxattr,
                                             fd, name, value, size, flags);
}