int sftp_reply_status(sftp_client_message msg, uint32_t status,
    const char *message) {
  ssh_buffer out;
  ssh_string s;

  out = ssh_buffer_new();
  if (out == NULL) {
    return -1;
  }

  s = ssh_string_from_char(message ? message : "");
  if (s == NULL) {
    SSH_BUFFER_FREE(out);
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      ssh_buffer_add_u32(out, htonl(status)) < 0 ||
      ssh_buffer_add_ssh_string(out, s) < 0 ||
      ssh_buffer_add_u32(out, 0) < 0 || /* language string */
      sftp_packet_write(msg->sftp, SSH_FXP_STATUS, out) < 0) {
    SSH_BUFFER_FREE(out);
    SSH_STRING_FREE(s);
    return -1;
  }

  SSH_BUFFER_FREE(out);
  SSH_STRING_FREE(s);

  return 0;
}