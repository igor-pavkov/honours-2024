static int sec_set_protection_level(struct connectdata *conn)
{
  int code;
  enum protection_level level = conn->request_data_prot;

  DEBUGASSERT(level > PROT_NONE && level < PROT_LAST);

  if(!conn->sec_complete) {
    infof(conn->data, "Trying to change the protection level after the"
                      " completion of the data exchange.\n");
    return -1;
  }

  /* Bail out if we try to set up the same level */
  if(conn->data_prot == level)
    return 0;

  if(level) {
    char *pbsz;
    static unsigned int buffer_size = 1 << 20; /* 1048576 */

    code = ftp_send_command(conn, "PBSZ %u", buffer_size);
    if(code < 0)
      return -1;

    if(code/100 != 2) {
      failf(conn->data, "Failed to set the protection's buffer size.");
      return -1;
    }
    conn->buffer_size = buffer_size;

    pbsz = strstr(conn->data->state.buffer, "PBSZ=");
    if(pbsz) {
      /* ignore return code, use default value if it fails */
      (void)sscanf(pbsz, "PBSZ=%u", &buffer_size);
      if(buffer_size < conn->buffer_size)
        conn->buffer_size = buffer_size;
    }
  }

  /* Now try to negiociate the protection level. */
  code = ftp_send_command(conn, "PROT %c", level_to_char(level));

  if(code < 0)
    return -1;

  if(code/100 != 2) {
    failf(conn->data, "Failed to set the protection level.");
    return -1;
  }

  conn->data_prot = level;
  if(level == PROT_PRIVATE)
    conn->command_prot = level;

  return 0;
}