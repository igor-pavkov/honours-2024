uid_t enc_untrusted_geteuid() {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_geteuid);
}