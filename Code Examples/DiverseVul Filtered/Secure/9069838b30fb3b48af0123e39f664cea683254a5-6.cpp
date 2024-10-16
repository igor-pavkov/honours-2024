static void do_sec_send(struct connectdata *conn, curl_socket_t fd,
                        const char *from, int length)
{
  int bytes, htonl_bytes; /* 32-bit integers for htonl */
  char *buffer = NULL;
  char *cmd_buffer;
  size_t cmd_size = 0;
  CURLcode error;
  enum protection_level prot_level = conn->data_prot;
  bool iscmd = (prot_level == PROT_CMD)?TRUE:FALSE;

  DEBUGASSERT(prot_level > PROT_NONE && prot_level < PROT_LAST);

  if(iscmd) {
    if(!strncmp(from, "PASS ", 5) || !strncmp(from, "ACCT ", 5))
      prot_level = PROT_PRIVATE;
    else
      prot_level = conn->command_prot;
  }
  bytes = conn->mech->encode(conn->app_data, from, length, prot_level,
                             (void **)&buffer);
  if(!buffer || bytes <= 0)
    return; /* error */

  if(iscmd) {
    error = Curl_base64_encode(conn->data, buffer, curlx_sitouz(bytes),
                               &cmd_buffer, &cmd_size);
    if(error) {
      free(buffer);
      return; /* error */
    }
    if(cmd_size > 0) {
      static const char *enc = "ENC ";
      static const char *mic = "MIC ";
      if(prot_level == PROT_PRIVATE)
        socket_write(conn, fd, enc, 4);
      else
        socket_write(conn, fd, mic, 4);

      socket_write(conn, fd, cmd_buffer, cmd_size);
      socket_write(conn, fd, "\r\n", 2);
      infof(conn->data, "Send: %s%s\n", prot_level == PROT_PRIVATE?enc:mic,
            cmd_buffer);
      free(cmd_buffer);
    }
  }
  else {
    htonl_bytes = htonl(bytes);
    socket_write(conn, fd, &htonl_bytes, sizeof(htonl_bytes));
    socket_write(conn, fd, buffer, curlx_sitouz(bytes));
  }
  free(buffer);
}