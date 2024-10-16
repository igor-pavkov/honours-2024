bool FromkLinuxSiginfo(const klinux_siginfo_t *input, siginfo_t *output) {
  if (!input || !output) {
    return false;
  }
  output->si_signo = FromkLinuxSignalNumber(input->si_signo);
  output->si_code = FromkLinuxSignalNumber(input->si_code);
  return true;
}