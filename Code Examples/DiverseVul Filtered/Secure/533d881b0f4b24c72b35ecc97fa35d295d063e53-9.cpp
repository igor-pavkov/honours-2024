int sftp_reply_attr(sftp_client_message msg, sftp_attributes attr) {
  ssh_buffer out;

  out = ssh_buffer_new();
  if (out == NULL) {
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      buffer_add_attributes(out, attr) < 0 ||
      sftp_packet_write(msg->sftp, SSH_FXP_ATTRS, out) < 0) {
    SSH_BUFFER_FREE(out);
    return -1;
  }
  SSH_BUFFER_FREE(out);

  return 0;
}