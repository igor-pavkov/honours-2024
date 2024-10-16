pid_t enc_untrusted_getpid() {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_getpid);
}