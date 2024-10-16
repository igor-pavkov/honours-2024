ssize_t enc_untrusted_fgetxattr(int fd, const char *name, void *value,
                                size_t size) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_fgetxattr,
                                             fd, name, value, size);
}