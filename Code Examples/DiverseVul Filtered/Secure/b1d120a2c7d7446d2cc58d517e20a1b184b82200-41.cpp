int enc_untrusted_clock_gettime(clockid_t clk_id, struct timespec *tp) {
  clockid_t klinux_clk_id = TokLinuxClockId(clk_id);
  if (klinux_clk_id == -1) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push<int64_t>(klinux_clk_id);
  MessageReader output;
  asylo::primitives::PrimitiveStatus status =
      asylo::host_call::NonSystemCallDispatcher(
          asylo::host_call::kClockGettimeHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_clock_gettime", 3);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  struct kLinux_timespec klinux_tp = output.next<struct kLinux_timespec>();

  // clock_gettime returns -1 on error and sets the errno.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return -1;
  }

  FromkLinuxtimespec(&klinux_tp, tp);
  return result;
}