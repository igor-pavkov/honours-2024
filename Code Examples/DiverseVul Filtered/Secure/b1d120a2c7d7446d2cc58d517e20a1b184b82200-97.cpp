int64_t enc_untrusted_sysconf(int name) {
  int kLinux_name = TokLinuxSysconfConstant(name);
  if (kLinux_name == -1) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push(kLinux_name);
  MessageReader output;
  asylo::primitives::PrimitiveStatus status =
      asylo::host_call::NonSystemCallDispatcher(
          asylo::host_call::kSysconfHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sysconf", 2);

  int64_t result = output.next<int>();
  int klinux_errno = output.next<int>();
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }

  return result;
}