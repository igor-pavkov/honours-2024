int enc_untrusted_kill(pid_t pid, int sig) {
  int klinux_sig = TokLinuxSignalNumber(sig);
  if (klinux_sig < 0) {
    errno = EINVAL;
    return -1;
  }

  return EnsureInitializedAndDispatchSyscall(asylo::system_call::kSYS_kill, pid,
                                             klinux_sig);
}