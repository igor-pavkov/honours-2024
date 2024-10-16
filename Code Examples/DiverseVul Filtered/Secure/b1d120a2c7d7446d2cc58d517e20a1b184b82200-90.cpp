int enc_untrusted_getsockopt(int sockfd, int level, int optname, void *optval,
                             socklen_t *optlen) {
  if (!optval || !optlen || *optlen == 0) {
    errno = EINVAL;
    return -1;
  }

  MessageWriter input;
  input.Push<int>(sockfd);
  input.Push<int>(level);
  input.Push<int>(TokLinuxOptionName(level, optname));
  input.PushByReference(Extent{reinterpret_cast<char *>(optval), *optlen});
  MessageReader output;
  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kGetSockOptHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_getsockopt", 3);

  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  Extent opt_received = output.next();

  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return -1;
  }

  // The returned |optlen| should not exceed the buffer size.
  if (opt_received.size() <= *optlen) {
    *optlen = opt_received.size();
  }
  memcpy(optval, opt_received.data(), *optlen);
  return result;
}