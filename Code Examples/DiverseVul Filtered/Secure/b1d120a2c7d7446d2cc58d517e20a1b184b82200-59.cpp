int enc_untrusted_getsockname(int sockfd, struct sockaddr *addr,
                              socklen_t *addrlen) {
  if (!addr || !addrlen) {
    errno = EFAULT;
    return -1;
  }
  // Guard against -1 being passed as addrlen even though it's unsigned.
  if (*addrlen == 0 || *addrlen > INT32_MAX) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push<int>(sockfd);
  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kGetSocknameHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_getsockname", 3);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();

  // getsockname() returns 0 on success. On error, -1 is returned, with errno
  // set to indicate the cause of the error.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return result;
  }

  auto klinux_sockaddr_buf = output.next();
  const struct klinux_sockaddr *klinux_addr =
      klinux_sockaddr_buf.As<struct klinux_sockaddr>();
  if (!FromkLinuxSockAddr(klinux_addr, klinux_sockaddr_buf.size(), addr,
                          addrlen, TrustedPrimitives::BestEffortAbort)) {
    errno = EFAULT;
    return -1;
  }
  return result;
}