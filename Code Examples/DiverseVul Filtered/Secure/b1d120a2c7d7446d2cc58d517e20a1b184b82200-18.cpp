int enc_untrusted_setitimer(int which, const struct itimerval *new_value,
                            struct itimerval *old_value) {
  struct klinux_itimerval klinux_new_value {};
  struct klinux_itimerval klinux_old_value {};
  if (!TokLinuxItimerval(new_value, &klinux_new_value)) {
    errno = EFAULT;
    return -1;
  }

  int result = EnsureInitializedAndDispatchSyscall(
      asylo::system_call::kSYS_setitimer, TokLinuxItimerType(which),
      &klinux_new_value, &klinux_old_value);

  if (old_value != nullptr &&
      !FromkLinuxItimerval(&klinux_old_value, old_value)) {
    errno = EFAULT;
    return -1;
  }
  return result;
}