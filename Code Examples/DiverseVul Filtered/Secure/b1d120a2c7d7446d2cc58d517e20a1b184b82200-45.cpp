pid_t enc_untrusted_waitpid(pid_t pid, int *status, int options) {
  int klinux_status;
  pid_t result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_wait4, pid, &klinux_status,
      TokLinuxWaitOption(options), /*rusage=*/nullptr);

  if (status) {
    *status = FromkLinuxToNewlibWstatus(klinux_status);
  }
  return result;
}