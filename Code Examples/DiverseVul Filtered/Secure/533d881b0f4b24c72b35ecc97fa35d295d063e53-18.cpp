int sftp_reply_names(sftp_client_message msg) {
  ssh_buffer out;

  out = ssh_buffer_new();
  if (out == NULL) {
    SSH_BUFFER_FREE(msg->attrbuf);
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      ssh_buffer_add_u32(out, htonl(msg->attr_num)) < 0 ||
      ssh_buffer_add_data(out, ssh_buffer_get(msg->attrbuf),
        ssh_buffer_get_len(msg->attrbuf)) < 0 ||
      sftp_packet_write(msg->sftp, SSH_FXP_NAME, out) < 0) {
    SSH_BUFFER_FREE(out);
    SSH_BUFFER_FREE(msg->attrbuf);
    return -1;
  }

  SSH_BUFFER_FREE(out);
  SSH_BUFFER_FREE(msg->attrbuf);

  msg->attr_num = 0;
  msg->attrbuf = NULL;

  return 0;
}