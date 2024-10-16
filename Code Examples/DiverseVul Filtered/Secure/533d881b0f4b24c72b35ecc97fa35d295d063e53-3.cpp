int sftp_reply_handle(sftp_client_message msg, ssh_string handle){
  ssh_buffer out;

  out = ssh_buffer_new();
  if (out == NULL) {
    return -1;
  }

  if (ssh_buffer_add_u32(out, msg->id) < 0 ||
      ssh_buffer_add_ssh_string(out, handle) < 0 ||
      sftp_packet_write(msg->sftp, SSH_FXP_HANDLE, out) < 0) {
    SSH_BUFFER_FREE(out);
    return -1;
  }
  SSH_BUFFER_FREE(out);

  return 0;
}