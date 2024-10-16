int enc_untrusted_unlink(const char *pathname) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_unlink,
                                             pathname);
}