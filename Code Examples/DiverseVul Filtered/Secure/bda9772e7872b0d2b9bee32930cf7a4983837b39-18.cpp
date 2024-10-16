bool SockaddrTokLinuxSockaddrIn6(const struct sockaddr *input,
                                 socklen_t input_addrlen,
                                 klinux_sockaddr_in6 *output) {
  if (!input || !output || input_addrlen == 0 || input->sa_family != AF_INET6 ||
      input_addrlen < sizeof(struct sockaddr_in6)) {
    output = nullptr;
    return false;
  }

  struct sockaddr_in6 *sockaddr_in6_from = const_cast<struct sockaddr_in6 *>(
      reinterpret_cast<const struct sockaddr_in6 *>(input));

  output->klinux_sin6_family = kLinux_AF_INET6;
  output->klinux_sin6_flowinfo = sockaddr_in6_from->sin6_flowinfo;
  output->klinux_sin6_port = sockaddr_in6_from->sin6_port;
  output->klinux_sin6_scope_id = sockaddr_in6_from->sin6_scope_id;
  InitializeToZeroSingle(&output->klinux_sin6_addr);
  ReinterpretCopySingle(&output->klinux_sin6_addr,
                        &sockaddr_in6_from->sin6_addr);
  return true;
}