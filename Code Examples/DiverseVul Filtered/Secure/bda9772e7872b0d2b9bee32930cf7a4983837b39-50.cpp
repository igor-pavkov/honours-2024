bool FromkLinuxPollfd(const struct klinux_pollfd *input,
                      struct pollfd *output) {
  if (!input || !output) return false;

  output->fd = input->klinux_fd;
  output->events = FromkLinuxPollEvent(input->klinux_events);
  output->revents = FromkLinuxPollEvent(input->klinux_revents);
  return true;
}