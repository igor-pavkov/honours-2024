bool FromkLinuxEpollEvent(const struct klinux_epoll_event *input,
                          struct epoll_event *output) {
  if (!input || !output) return false;
  output->events = FromkLinuxEpollEvents(input->events);
  if (input->events != 0 && output->events == 0) {
    return false;
  }
  output->data.u64 = input->data.u64;
  return true;
}