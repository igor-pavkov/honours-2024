ssize_t enc_untrusted_send(int sockfd, const void *buf, size_t len, int flags) {
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_sendto,
                                             sockfd, buf, len, flags,
                                             /*dest_addr=*/nullptr,
                                             /*addrlen=*/0);
}