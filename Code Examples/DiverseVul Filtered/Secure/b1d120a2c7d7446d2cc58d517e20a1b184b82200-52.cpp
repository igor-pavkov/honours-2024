int enc_untrusted_setsockopt(int sockfd, int level, int optname,
                             const void *optval, socklen_t optlen) {
  return EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_setsockopt, sockfd, level,
      TokLinuxOptionName(level, optname), optval, optlen);
}