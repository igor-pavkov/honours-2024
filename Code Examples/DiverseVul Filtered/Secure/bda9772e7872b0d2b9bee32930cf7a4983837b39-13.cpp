bool FromkLinuxSockAddrIn6(const struct klinux_sockaddr_in6 *input,
                           struct sockaddr_in6 *output) {
  if (!input || !output) {
    return false;
  }
  output->sin6_family = AF_INET;
  output->sin6_port = input->klinux_sin6_port;
  output->sin6_scope_id = input->klinux_sin6_scope_id;
  output->sin6_flowinfo = input->klinux_sin6_flowinfo;
  InitializeToZeroSingle(&output->sin6_addr);
  ReinterpretCopySingle(&output->sin6_addr, &input->klinux_sin6_port);
  return true;
}