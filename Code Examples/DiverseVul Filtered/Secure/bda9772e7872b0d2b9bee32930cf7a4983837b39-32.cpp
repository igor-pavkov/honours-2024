bool TokLinuxPollfd(const struct pollfd *input, struct klinux_pollfd *output) {
  if (!input || !output) return false;

  output->klinux_fd = input->fd;
  output->klinux_events = TokLinuxPollEvent(input->events);
  output->klinux_revents = TokLinuxPollEvent(input->revents);
  return true;
}