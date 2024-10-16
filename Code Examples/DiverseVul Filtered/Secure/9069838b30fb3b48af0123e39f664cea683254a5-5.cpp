static int ftp_send_command(struct connectdata *conn, const char *message, ...)
{
  int ftp_code;
  ssize_t nread = 0;
  va_list args;
  char print_buffer[50];

  va_start(args, message);
  mvsnprintf(print_buffer, sizeof(print_buffer), message, args);
  va_end(args);

  if(Curl_ftpsend(conn, print_buffer)) {
    ftp_code = -1;
  }
  else {
    if(Curl_GetFTPResponse(&nread, conn, &ftp_code))
      ftp_code = -1;
  }

  (void)nread; /* Unused */
  return ftp_code;
}