void *sftp_handle(sftp_session sftp, ssh_string handle){
  uint32_t val;

  if (sftp->handles == NULL) {
    return NULL;
  }

  if (ssh_string_len(handle) != sizeof(uint32_t)) {
    return NULL;
  }

  memcpy(&val, ssh_string_data(handle), sizeof(uint32_t));

  if (val > SFTP_HANDLES) {
    return NULL;
  }

  return sftp->handles[val];
}