bool TokLinuxRusage(const struct rusage *input, struct klinux_rusage *output) {
  if (!input || !output) {
    return false;
  }
  if (!TokLinuxtimeval(&input->ru_stime, &output->ru_stime) ||
      !TokLinuxtimeval(&input->ru_utime, &output->ru_utime)) {
    return false;
  }
  return true;
}