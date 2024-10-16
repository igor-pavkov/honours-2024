static int domore_getsock(struct connectdata *conn,
                          curl_socket_t *socks,
                          int numsocks)
{
  if(conn && conn->handler->domore_getsock)
    return conn->handler->domore_getsock(conn, socks, numsocks);
  return GETSOCK_BLANK;
}