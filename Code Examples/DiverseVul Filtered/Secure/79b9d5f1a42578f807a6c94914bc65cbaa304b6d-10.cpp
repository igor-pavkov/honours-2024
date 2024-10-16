static CURLcode https_connecting(struct connectdata *conn, bool *done)
{
  CURLcode result;
  DEBUGASSERT((conn) && (conn->handler->flags & PROTOPT_SSL));

  /* perform SSL initialization for this socket */
  result = Curl_ssl_connect_nonblocking(conn, FIRSTSOCKET, done);
  if(result)
    connclose(conn, "Failed HTTPS connection");

  return result;
}