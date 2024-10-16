bool FromkLinuxSockAddrIn(const struct klinux_sockaddr_in *input,
                          struct sockaddr_in *output) {
  if (!input || !output) {
    return false;
  }
  output->sin_family = AF_INET;
  output->sin_port = input->klinux_sin_port;
  InitializeToZeroSingle(&output->sin_addr);
  ReinterpretCopySingle(&output->sin_addr, &input->klinux_sin_port);
  InitializeToZeroArray(output->sin_zero);
  ReinterpretCopyArray(
      output->sin_zero, input->klinux_sin_zero,
      std::min(sizeof(output->sin_zero), sizeof(input->klinux_sin_zero)));
  return true;
}