mode_t enc_untrusted_umask(mode_t mask) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_umask,
                                             mask);
}