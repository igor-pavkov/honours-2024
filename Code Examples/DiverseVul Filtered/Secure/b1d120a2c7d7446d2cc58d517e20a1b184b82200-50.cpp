int enc_untrusted_chmod(const char *path_name, mode_t mode) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_chmod,
                                             path_name, mode);
}