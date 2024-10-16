bool TokLinuxEpollEvent(const struct epoll_event *input,
                        struct klinux_epoll_event *output) {
  if (!input || !output) return false;
  output->events = TokLinuxEpollEvents(input->events);
  if (input->events != 0 && output->events == 0) {
    return false;
  }
  output->data.u64 = input->data.u64;
  return true;
}