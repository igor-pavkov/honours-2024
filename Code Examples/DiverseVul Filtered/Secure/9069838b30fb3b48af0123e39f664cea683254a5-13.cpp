static ssize_t sec_send(struct connectdata *conn, int sockindex,
                        const void *buffer, size_t len, CURLcode *err)
{
  curl_socket_t fd = conn->sock[sockindex];
  *err = CURLE_OK;
  return sec_write(conn, fd, buffer, len);
}