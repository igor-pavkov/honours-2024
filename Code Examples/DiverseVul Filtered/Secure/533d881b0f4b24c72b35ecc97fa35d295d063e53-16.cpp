int sftp_reply_name(sftp_client_message msg, const char *name,
    sftp_attributes attr) {
  ssh_buffer out;
  ssh_string file;

  out = ssh_buffer_new();
  if (out == NULL) {
    return -1;
  }

  file = ssh_string_from_char(name);
  if (file == NULL) {
    SSH_BUFFER_FREE(out);
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      ssh_buffer_add_u32(out, htonl(1)) < 0 ||
      ssh_buffer_add_ssh_string(out, file) < 0 ||
      ssh_buffer_add_ssh_string(out, file) < 0 || /* The protocol is broken here between 3 & 4 */
      buffer_add_attributes(out, attr) < 0 ||
      sftp_packet_write(msg->sftp, SSH_FXP_NAME, out) < 0) {
    SSH_BUFFER_FREE(out);
    SSH_STRING_FREE(file);
    return -1;
  }
  SSH_BUFFER_FREE(out);
  SSH_STRING_FREE(file);

  return 0;
}