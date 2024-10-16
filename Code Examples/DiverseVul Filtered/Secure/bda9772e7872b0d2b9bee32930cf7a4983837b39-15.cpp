bool TokLinuxSiginfo(const siginfo_t *input, klinux_siginfo_t *output) {
  if (!input || !output) {
    return false;
  }
  output->si_signo = TokLinuxSignalNumber(input->si_signo);
  output->si_code = TokLinuxSignalNumber(input->si_code);
  return true;
}