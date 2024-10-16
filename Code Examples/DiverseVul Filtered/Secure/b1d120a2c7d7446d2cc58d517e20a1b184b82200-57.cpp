clock_t enc_untrusted_times(struct tms *buf) {
  struct kLinux_tms klinux_buf {};
  int64_t result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_times, &klinux_buf);

  if (!FromkLinuxtms(&klinux_buf, buf)) {
    errno = EFAULT;
    return -1;
  }
  return static_cast<clock_t>(result);
}