void sftp_client_message_free(sftp_client_message msg) {
  if (msg == NULL) {
    return;
  }

  SAFE_FREE(msg->filename);
  SAFE_FREE(msg->submessage);
  SSH_STRING_FREE(msg->data);
  SSH_STRING_FREE(msg->handle);
  sftp_attributes_free(msg->attr);
  SSH_BUFFER_FREE(msg->complete_message);
  SAFE_FREE(msg->str_data);
  ZERO_STRUCTP(msg);
  SAFE_FREE(msg);
}