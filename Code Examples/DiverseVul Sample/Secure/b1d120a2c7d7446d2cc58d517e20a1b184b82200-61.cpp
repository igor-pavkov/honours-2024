ssize_t enc_untrusted_sendmsg(int sockfd, const struct msghdr *msg, int flags) {
  size_t total_message_size = CalculateTotalMessageSize(msg);
  std::unique_ptr<char[]> msg_iov_buffer(new char[total_message_size]);
  size_t copied_bytes = 0;
  for (int i = 0; i < msg->msg_iovlen; ++i) {
    memcpy(msg_iov_buffer.get() + copied_bytes, msg->msg_iov[i].iov_base,
           msg->msg_iov[i].iov_len);
    copied_bytes += msg->msg_iov[i].iov_len;
  }

  MessageWriter input;
  input.Push(sockfd);
  input.PushByReference(Extent{msg->msg_name, msg->msg_namelen});
  input.PushByReference(Extent{msg_iov_buffer.get(), total_message_size});
  input.PushByReference(Extent{msg->msg_control, msg->msg_controllen});
  input.Push(msg->msg_flags);
  input.Push(flags);
  MessageReader output;

  const auto status = NonSystemCallDispatcher(
      ::asylo::host_call::kSendMsgHandler, &input, &output);
  CheckStatusAndParamCount(status, output, "enc_untrusted_sendmsg", 2);

  ssize_t result = output.next<ssize_t>();
  int klinux_errno = output.next<int>();
  // sendmsg() returns the number of characters sent. On error, -1 is returned,
  // with errno set to indicate the cause of the error.
  if (result == -1) {
    errno = FromkLinuxErrorNumber(klinux_errno);
  }
  return result;
}