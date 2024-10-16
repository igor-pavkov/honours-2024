int sftp_reply_data(sftp_client_message msg, const void *data, int len) {
  ssh_buffer out;

  out = ssh_buffer_new();
  if (out == NULL) {
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      ssh_buffer_add_u32(out, ntohl(len)) < 0 ||
      ssh_buffer_add_data(out, data, len) < 0 ||
      sftp_packet_write(msg->sftp, SSH_FXP_DATA, out) < 0) {
    SSH_BUFFER_FREE(out);
    return -1;
  }
  SSH_BUFFER_FREE(out);

  return 0;
}