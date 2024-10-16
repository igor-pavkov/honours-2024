int Curl_protocol_getsock(struct connectdata *conn,
                          curl_socket_t *socks,
                          int numsocks)
{
  if(conn->handler->proto_getsock)
    return conn->handler->proto_getsock(conn, socks, numsocks);
  /* Backup getsock logic. Since there is a live socket in use, we must wait
     for it or it will be removed from watching when the multi_socket API is
     used. */
  socks[0] = conn->sock[FIRSTSOCKET];
  return GETSOCK_READSOCK(0) | GETSOCK_WRITESOCK(0);
}