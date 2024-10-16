int enc_untrusted_isatty(int fd) {
  MessageWriter input;
  input.Push(fd);
  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kIsAttyHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_isatty", 2);

  int result = output.next<int>();

  // isatty() returns 1 if fd is an open file descriptor referring to a
  // terminal; otherwise 0 is returned, and errno is set to indicate the error.
  if (result == 0) {
    int klinux_errno = output.next<int>();
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}