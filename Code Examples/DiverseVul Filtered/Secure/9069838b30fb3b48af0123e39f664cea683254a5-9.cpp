static ssize_t sec_write(struct connectdata *conn, curl_socket_t fd,
                         const char *buffer, size_t length)
{
  ssize_t tx = 0, len = conn->buffer_size;

  len -= conn->mech->overhead(conn->app_data, conn->data_prot,
                              curlx_sztosi(len));
  if(len <= 0)
    len = length;
  while(length) {
    if(length < (size_t)len)
      len = length;

    do_sec_send(conn, fd, buffer, curlx_sztosi(len));
    length -= len;
    buffer += len;
    tx += len;
  }
  return tx;
}