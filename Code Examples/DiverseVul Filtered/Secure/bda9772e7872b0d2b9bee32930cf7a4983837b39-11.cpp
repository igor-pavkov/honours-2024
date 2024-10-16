bool SockaddrTokLinuxSockaddrIn(const struct sockaddr *input,
                                socklen_t input_addrlen,
                                klinux_sockaddr_in *output) {
  if (!input || !output || input_addrlen == 0 || input->sa_family != AF_INET ||
      input_addrlen < sizeof(struct sockaddr_in)) {
    output = nullptr;
    return false;
  }

  struct sockaddr_in *sockaddr_in_from = const_cast<struct sockaddr_in *>(
      reinterpret_cast<const struct sockaddr_in *>(input));

  output->klinux_sin_family = kLinux_AF_INET;
  output->klinux_sin_port = sockaddr_in_from->sin_port;
  InitializeToZeroSingle(&output->klinux_sin_addr);
  ReinterpretCopySingle(&output->klinux_sin_addr, &sockaddr_in_from->sin_addr);
  InitializeToZeroArray(output->klinux_sin_zero);
  ReinterpretCopyArray(output->klinux_sin_zero, sockaddr_in_from->sin_zero,
                       std::min(sizeof(output->klinux_sin_zero),
                                sizeof(sockaddr_in_from->sin_zero)));
  return true;
}