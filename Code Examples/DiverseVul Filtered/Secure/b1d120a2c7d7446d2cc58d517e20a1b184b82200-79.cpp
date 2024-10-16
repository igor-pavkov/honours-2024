int enc_untrusted_epoll_ctl(int epfd, int op, int fd,
                            struct epoll_event *event) {
  struct klinux_epoll_event klinux_event_tmp {};
  if (event != nullptr && !TokLinuxEpollEvent(event, &klinux_event_tmp)) {
    errno = EINVAL;
    return -1;
  }
  int klinux_op = TokLinuxEpollCtlOp(op);
  if (klinux_op == 0) {
    errno = EINVAL;
    return -1;
  }

  struct klinux_epoll_event *klinux_event =
      (event != nullptr) ? &klinux_event_tmp : nullptr;
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_epoll_ctl,
                                             epfd, klinux_op, fd, klinux_event);
}