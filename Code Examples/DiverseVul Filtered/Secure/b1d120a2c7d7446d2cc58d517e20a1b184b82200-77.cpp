int enc_untrusted_socket(int domain, int type, int protocol) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_socket, TokLinuxAfFamily(domain),
      TokLinuxSocketType(type), protocol);
}