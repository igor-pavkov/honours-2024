pid_t enc_untrusted_wait3(int *status, int options, struct rusage *rusage) {
  int klinux_status;
  struct klinux_rusage klinux_usage;
  pid_t result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_wait4, /*pid=*/-1, &klinux_status,
      TokLinuxWaitOption(options), &klinux_usage);

  if (status) {
    *status = FromkLinuxToNewlibWstatus(klinux_status);
  }
  if (rusage) {
    if (!FromkLinuxRusage(&klinux_usage, rusage)) {
      errno = EINVAL;
      return -1;
    }
  }
  return result;
}