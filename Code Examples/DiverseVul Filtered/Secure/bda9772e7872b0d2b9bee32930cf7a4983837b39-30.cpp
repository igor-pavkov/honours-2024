bool TokLinuxItimerval(const struct itimerval *input,
                       struct klinux_itimerval *output) {
  if (!input || !output) {
    return false;
  }
  if (!TokLinuxtimeval(&input->it_interval, &output->klinux_it_interval) ||
      !TokLinuxtimeval(&input->it_value, &output->klinux_it_value)) {
    return false;
  }
  return true;
}