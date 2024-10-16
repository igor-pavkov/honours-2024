int enc_untrusted_inotify_read(int fd, size_t count, char **serialized_events,
                               size_t *serialized_events_len) {
  MessageWriter input;
  MessageReader output;
  input.Push<int>(fd);
  input.Push<uint64_t>(count);

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kInotifyReadHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_inotify_read", 2,
                           /*match_exact_params=*/false);
  int result = output.next<int>();
  int klinux_errno = output.next<int>();
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
    return -1;
  }

  Extent serialized_buf = output.next();
  *serialized_events_len = serialized_buf.size();

  // The caller to this host call owns memory pointed by |*serialized_events|.
  *serialized_events = reinterpret_cast<char *>(malloc(*serialized_events_len));
  if (!serialized_events) {
    errno = ENOMEM;
    return -1;
  }
  memcpy(*serialized_events, serialized_buf.As<char>(), *serialized_events_len);
  return result;
}