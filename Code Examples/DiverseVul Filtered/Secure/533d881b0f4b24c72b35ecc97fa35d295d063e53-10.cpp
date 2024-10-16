ssh_string sftp_handle_alloc(sftp_session sftp, void *info) {
  ssh_string ret;
  uint32_t val;
  uint32_t i;

  if (sftp->handles == NULL) {
    sftp->handles = calloc(SFTP_HANDLES, sizeof(void *));
    if (sftp->handles == NULL) {
      return NULL;
    }
  }

  for (i = 0; i < SFTP_HANDLES; i++) {
    if (sftp->handles[i] == NULL) {
      break;
    }
  }

  if (i == SFTP_HANDLES) {
    return NULL; /* no handle available */
  }

  val = i;
  ret = ssh_string_new(4);
  if (ret == NULL) {
    return NULL;
  }

  memcpy(ssh_string_data(ret), &val, sizeof(uint32_t));
  sftp->handles[i] = info;

  return ret;
}