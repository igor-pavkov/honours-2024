uid_t enc_untrusted_getuid() {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_getuid);
}