bool FromkLinuxRusage(const struct klinux_rusage *input,
                      struct rusage *output) {
  if (!input || !output) {
    return false;
  }
  if (!FromkLinuxtimeval(&input->ru_stime, &output->ru_stime) ||
      !FromkLinuxtimeval(&input->ru_utime, &output->ru_utime)) {
    return false;
  }
  return true;
}