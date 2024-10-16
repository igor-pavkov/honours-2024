char *enc_untrusted_if_indextoname(unsigned int ifindex, char *ifname) {
  if (!ifname) {
    return nullptr;
  }

  MessageWriter input;
  input.Push(ifindex);
  MessageReader output;

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kIfIndexToNameHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_indextoname", 2);

  Extent ifname_buffer = output.next();
  memcpy(ifname, ifname_buffer.As<char>(),
         std::min(ifname_buffer.size(), static_cast<size_t>(IF_NAMESIZE)));
  int klinux_errno = output.next<int>();
  if (ifname_buffer.empty()) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return ifname;
}