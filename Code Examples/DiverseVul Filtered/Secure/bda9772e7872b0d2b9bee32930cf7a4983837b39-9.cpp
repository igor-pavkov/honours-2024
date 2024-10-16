bool SockaddrTokLinuxSockaddrUn(const struct sockaddr *input,
                                socklen_t input_addrlen,
                                klinux_sockaddr_un *output) {
  if (!input || !output || input_addrlen == 0 || input->sa_family != AF_UNIX ||
      input_addrlen < sizeof(output->klinux_sun_family)) {
    output = nullptr;
    return false;
  }

  struct sockaddr_un *sock_un = const_cast<struct sockaddr_un *>(
      reinterpret_cast<const struct sockaddr_un *>(input));
  output->klinux_sun_family = kLinux_AF_UNIX;
  InitializeToZeroArray(output->klinux_sun_path);
  ReinterpretCopyArray(output->klinux_sun_path, sock_un->sun_path,
                       input_addrlen - sizeof(input->sa_family));
  return true;
}