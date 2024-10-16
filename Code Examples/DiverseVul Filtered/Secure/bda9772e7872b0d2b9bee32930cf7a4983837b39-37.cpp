bool FromkLinuxSigset(const klinux_sigset_t *input, sigset_t *output) {
  if (!input || !output) {
    output = nullptr;
    return false;
  }
  sigemptyset(output);
  for (int klinux_sig = 1; klinux_sig < kLinux_NSIG; klinux_sig++) {
    if (klinux_sigismember(input, klinux_sig)) {
      int sig = FromkLinuxSignalNumber(klinux_sig);
      if (sig != -1) {
        sigaddset(output, sig);
      }
    }
  }
  return true;
}