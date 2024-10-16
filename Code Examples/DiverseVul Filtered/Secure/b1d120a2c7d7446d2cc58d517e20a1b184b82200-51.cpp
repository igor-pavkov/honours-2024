int enc_untrusted_shutdown(int sockfd, int how) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_shutdown,
                                             sockfd, how);
}