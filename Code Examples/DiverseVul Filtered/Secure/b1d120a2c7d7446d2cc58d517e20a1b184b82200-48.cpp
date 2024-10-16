int enc_untrusted_accept(int sockfd, struct sockaddr *addr,
                         socklen_t *addrlen) {
  MessageWriter input;
  input.Push<int>(sockfd);
  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kAcceptHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_accept", 3);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();

  // accept() returns -1 on failure, with errno set to indicate the cause
  // of the error.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return result;
  }

  auto klinux_sockaddr_buf = output.next();
  const struct klinux_sockaddr *klinux_addr =
      klinux_sockaddr_buf.As<struct klinux_sockaddr>();
  FromkLinuxSockAddr(klinux_addr, klinux_sockaddr_buf.size(), addr, addrlen,
                     TrustedPrimitives::BestEffortAbort);
  return result;
}