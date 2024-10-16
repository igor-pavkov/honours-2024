int enc_untrusted_poll(struct pollfd *fds, nfds_t nfds, int timeout) {
  auto klinux_fds = absl::make_unique<struct klinux_pollfd[]>(nfds);
  for (int i = 0; i < nfds; ++i) {
    if (!TokLinuxPollfd(&fds[i], &klinux_fds[i])) {
      errno = EFAULT;
      return -1;
    }
  }

  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_poll, klinux_fds.get(),
      static_cast<uint64_t>(nfds), timeout);

  if (result < 0) {
    return result;
  }

  for (int i = 0; i < nfds; ++i) {
    if (!FromkLinuxPollfd(&klinux_fds[i], &fds[i])) {
      errno = EFAULT;
      return -1;
    }
  }
  return result;
}