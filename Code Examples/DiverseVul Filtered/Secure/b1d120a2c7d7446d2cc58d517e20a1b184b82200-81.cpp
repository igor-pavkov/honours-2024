int enc_untrusted_connect(int sockfd, const struct sockaddr *addr,
                          socklen_t addrlen) {
  socklen_t klinux_sock_len =
      std::max(std::max(sizeof(klinux_sockaddr_un), sizeof(klinux_sockaddr_in)),
               sizeof(klinux_sockaddr_in6));
  auto klinux_sock = absl::make_unique<char[]>(klinux_sock_len);

  if (!TokLinuxSockAddr(addr, addrlen,
                        reinterpret_cast<klinux_sockaddr *>(klinux_sock.get()),
                        &klinux_sock_len, TrustedPrimitives::BestEffortAbort)) {
    errno = EINVAL;
    return -1;
  }

  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_connect,
                                             sockfd, klinux_sock.get(),
                                             klinux_sock_len);
}