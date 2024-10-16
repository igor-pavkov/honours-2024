int enc_untrusted_usleep(useconds_t usec) {
  MessageWriter input;
  input.Push(usec);
  MessageReader output;
  asylo::primitives::PrimitiveStatus status =
      asylo::host_call::NonSystemCallDispatcher(
          asylo::host_call::kUSleepHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_usleep", 2);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();

  // usleep() returns 0 on success. On error, -1 is returned, with errno set to
  // indicate the cause of the error.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }

  return result;
}