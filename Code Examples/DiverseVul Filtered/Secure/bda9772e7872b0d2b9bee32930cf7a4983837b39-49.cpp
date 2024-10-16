bool FromkLinuxFdSet(const struct klinux_fd_set *input, fd_set *output) {
  if (!input || !output) {
    output = nullptr;
    return false;
  }
  FD_ZERO(output);
  for (int fd = 0; fd < std::min(KLINUX_FD_SETSIZE, FD_SETSIZE); ++fd) {
    if (KLINUX_FD_ISSET(fd, input)) {
      FD_SET(fd, output);
    }
  }
  return true;
}