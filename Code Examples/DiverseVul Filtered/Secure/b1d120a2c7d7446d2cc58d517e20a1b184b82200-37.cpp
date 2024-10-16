size_t CalculateTotalMessageSize(const struct msghdr *msg) {
  size_t total_message_size = 0;
  for (int i = 0; i < msg->msg_iovlen; ++i) {
    total_message_size += msg->msg_iov[i].iov_len;
  }
  return total_message_size;
}