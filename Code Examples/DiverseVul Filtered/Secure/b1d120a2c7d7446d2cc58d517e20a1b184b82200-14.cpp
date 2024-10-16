int enc_untrusted_rmdir(const char *path) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_rmdir,
                                             path);
}