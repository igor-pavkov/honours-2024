bool FromkLinuxItimerval(const struct klinux_itimerval *input,
                         struct itimerval *output) {
  if (!input || !output) {
    return false;
  }
  if (!FromkLinuxtimeval(&input->klinux_it_interval, &output->it_interval) ||
      !FromkLinuxtimeval(&input->klinux_it_value, &output->it_value)) {
    return false;
  }
  return true;
}