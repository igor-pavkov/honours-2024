int enc_untrusted_getrusage(int who, struct rusage *usage) {
  struct klinux_rusage klinux_usage {};
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_getrusage, TokLinuxRusageTarget(who),
      &klinux_usage);

  if (result != -1) {
    if (!FromkLinuxRusage(&klinux_usage, usage)) {
      errno = EINVAL;
      return -1;
    }
  }
  return result;
}