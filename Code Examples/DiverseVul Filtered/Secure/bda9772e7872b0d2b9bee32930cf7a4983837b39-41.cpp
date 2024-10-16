bool FromkLinuxSockAddrUn(const struct klinux_sockaddr_un *input,
                          struct sockaddr_un *output) {
  if (!input || !output) {
    return false;
  }
  output->sun_family = AF_UNIX;
  InitializeToZeroArray(output->sun_path);
  ReinterpretCopyArray(
      output->sun_path, input->klinux_sun_path,
      sizeof(struct klinux_sockaddr_un) - sizeof(input->klinux_sun_family));
  return true;
}