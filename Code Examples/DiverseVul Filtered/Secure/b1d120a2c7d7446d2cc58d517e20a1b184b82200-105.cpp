pid_t enc_untrusted_setsid() {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_setsid);
}