pid_t enc_untrusted_getppid() {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_getppid);
}