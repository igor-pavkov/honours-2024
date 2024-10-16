unsigned int enc_untrusted_if_nametoindex(const char *ifname) {
  MessageWriter input;
  input.PushString(ifname);
  MessageReader output;

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kIfNameToIndexHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_nametoindex", 2);

  auto result = output.next<unsigned int>();
  int klinux_errno = output.next<int>();
  if (result == 0) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}