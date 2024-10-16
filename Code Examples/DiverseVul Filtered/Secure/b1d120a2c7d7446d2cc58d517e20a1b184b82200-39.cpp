int enc_untrusted_nanosleep(const struct timespec *req, struct timespec *rem) {
  struct kLinux_timespec klinux_req;
  if (!TokLinuxtimespec(req, &klinux_req)) {
    errno = EINVAL;
    return -1;
  }
  struct kLinux_timespec klinux_rem;

  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_nanosleep, &klinux_req, &klinux_rem);
  FromkLinuxtimespec(&klinux_rem, rem);
  return result;
}