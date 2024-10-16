int enc_untrusted_sys_futex_wait(int32_t *futex, int32_t expected,
                                 int64_t timeout_microsec) {
  if (!TrustedPrimitives::IsOutsideEnclave(futex, sizeof(int32_t))) {
    TrustedPrimitives::BestEffortAbort(
        "enc_untrusted_sys_futex_wait: futex word should be in untrusted "
        "local memory.");
  }

  MessageWriter input;
  MessageReader output;
  input.Push<uint64_t>(reinterpret_cast<uint64_t>(futex));
  input.Push<int32_t>(expected);
  input.Push<int64_t>(timeout_microsec);
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kSysFutexWaitHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sys_futex_wait", 2);
  int result = output.next<int>();
  int klinux_errno = output.next<int>();

  // If FUTEX_WAIT successfully causes the thread to be suspended in the kernel,
  // it returns a zero when the caller is woken up. Otherwise, it returns the
  // appropriate errno.
  if (result != 0) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}