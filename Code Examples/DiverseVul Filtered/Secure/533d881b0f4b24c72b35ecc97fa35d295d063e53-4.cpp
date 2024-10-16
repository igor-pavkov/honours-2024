void sftp_handle_remove(sftp_session sftp, void *handle) {
  int i;

  for (i = 0; i < SFTP_HANDLES; i++) {
    if (sftp->handles[i] == handle) {
      sftp->handles[i] = NULL;
      break;
    }
  }
}