int enc_untrusted_sys_futex_wake(int32_t *futex, int32_t num) {
  if (!TrustedPrimitives::IsOutsideEnclave(futex, sizeof(int32_t))) {
    TrustedPrimitives::BestEffortAbort(
        "enc_untrusted_sys_futex_wake: futex word should be in untrusted "
        "local memory.");
  }

  MessageWriter input;
  MessageReader output;
  input.Push<uint64_t>(reinterpret_cast<uint64_t>(futex));
  input.Push<int32_t>(num);
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kSysFutexWakeHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sys_futex_wake", 2);
  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}