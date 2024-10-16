int sftp_reply_names_add(sftp_client_message msg, const char *file,
    const char *longname, sftp_attributes attr) {
  ssh_string name;

  name = ssh_string_from_char(file);
  if (name == NULL) {
    return -1;
  }

  if (msg->attrbuf == NULL) {
    msg->attrbuf = ssh_buffer_new();
    if (msg->attrbuf == NULL) {
      SSH_STRING_FREE(name);
      return -1;
    }
  }

  if (ssh_buffer_add_ssh_string(msg->attrbuf, name) < 0) {
    SSH_STRING_FREE(name);
    return -1;
  }

  SSH_STRING_FREE(name);
  name = ssh_string_from_char(longname);
  if (name == NULL) {
    return -1;
  }
  if (ssh_buffer_add_ssh_string(msg->attrbuf,name) < 0 ||
      buffer_add_attributes(msg->attrbuf,attr) < 0) {
    SSH_STRING_FREE(name);
    return -1;
  }
  SSH_STRING_FREE(name);
  msg->attr_num++;

  return 0;
}