int enc_untrusted_getitimer(int which, struct itimerval *curr_value) {
  struct klinux_itimerval klinux_curr_value {};
  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_getitimer, TokLinuxItimerType(which),
      &klinux_curr_value);

  if (!curr_value || !FromkLinuxItimerval(&klinux_curr_value, curr_value)) {
    errno = EFAULT;
    return -1;
  }
  return result;
}