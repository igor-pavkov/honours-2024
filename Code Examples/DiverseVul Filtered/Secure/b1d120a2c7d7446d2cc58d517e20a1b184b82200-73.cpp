int enc_untrusted_raise(int sig) {
  int klinux_sig = TokLinuxSignalNumber(sig);
  if (klinux_sig < 0) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push<int>(klinux_sig);
  MessageReader output;
  const auto status = NonSystemCallDispatcher(::asylo::host_call::kRaiseHandler,
                                              &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_raise", 2);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  if (result != 0) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}