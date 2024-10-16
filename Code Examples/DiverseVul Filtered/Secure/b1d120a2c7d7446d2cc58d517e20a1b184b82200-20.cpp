int enc_untrusted_utimes(const char *filename, const struct timeval times[2]) {
  struct kLinux_timeval klinux_times[2];
  if (!TokLinuxtimeval(&times[0], &klinux_times[0]) ||
      !TokLinuxtimeval(&times[1], &klinux_times[1])) {
    errno = EBADE;
    return -1;
  }
  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_utimes,
                                             filename, klinux_times);
}