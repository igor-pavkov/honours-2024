bool TokLinuxSigset(const sigset_t *input, klinux_sigset_t *output) {
  if (!input || !output) {
    output = nullptr;
    return false;
  }
  klinux_sigemptyset(output);
  for (int sig = 1; sig < NSIG; sig++) {
    if (sigismember(input, sig)) {
      int klinux_sig = TokLinuxSignalNumber(sig);
      if (klinux_sig != -1) {
        klinux_sigaddset(output, klinux_sig);
      }
    }
  }
  return true;
}