static ssize_t sec_recv(struct connectdata *conn, int sockindex,
                        char *buffer, size_t len, CURLcode *err)
{
  size_t bytes_read;
  size_t total_read = 0;
  curl_socket_t fd = conn->sock[sockindex];

  *err = CURLE_OK;

  /* Handle clear text response. */
  if(conn->sec_complete == 0 || conn->data_prot == PROT_CLEAR)
      return read(fd, buffer, len);

  if(conn->in_buffer.eof_flag) {
    conn->in_buffer.eof_flag = 0;
    return 0;
  }

  bytes_read = buffer_read(&conn->in_buffer, buffer, len);
  len -= bytes_read;
  total_read += bytes_read;
  buffer += bytes_read;

  while(len > 0) {
    if(read_data(conn, fd, &conn->in_buffer))
      return -1;
    if(conn->in_buffer.size == 0) {
      if(bytes_read > 0)
        conn->in_buffer.eof_flag = 1;
      return bytes_read;
    }
    bytes_read = buffer_read(&conn->in_buffer, buffer, len);
    len -= bytes_read;
    total_read += bytes_read;
    buffer += bytes_read;
  }
  return total_read;
}