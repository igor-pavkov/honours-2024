int enc_untrusted_select(int nfds, fd_set *readfds, fd_set *writefds,
                         fd_set *exceptfds, struct timeval *timeout) {
  struct klinux_fd_set klinux_readfds, klinux_writefds, klinux_exceptfds;
  struct kLinux_timeval klinux_timeout;

  TokLinuxFdSet(readfds, &klinux_readfds);
  TokLinuxFdSet(writefds, &klinux_writefds);
  TokLinuxFdSet(exceptfds, &klinux_exceptfds);
  TokLinuxtimeval(timeout, &klinux_timeout);

  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_select, nfds, &klinux_readfds, &klinux_writefds,
      &klinux_exceptfds, &klinux_timeout);

  FromkLinuxFdSet(&klinux_readfds, readfds);
  FromkLinuxFdSet(&klinux_writefds, writefds);
  FromkLinuxFdSet(&klinux_exceptfds, exceptfds);
  return result;
}