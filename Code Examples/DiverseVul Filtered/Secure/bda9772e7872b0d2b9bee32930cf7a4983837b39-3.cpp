bool TokLinuxFdSet(const fd_set *input, struct klinux_fd_set *output) {
  if (!input || !output) {
    output = nullptr;
    return false;
  }
  KLINUX_FD_ZERO(output);
  for (int fd = 0; fd < std::min(FD_SETSIZE, KLINUX_FD_SETSIZE); ++fd) {
    if (FD_ISSET(fd, input)) {
      KLINUX_FD_SET(fd, output);
    }
  }
  return true;
}