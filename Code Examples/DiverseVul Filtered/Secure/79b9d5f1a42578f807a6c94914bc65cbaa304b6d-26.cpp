static int http_getsock_do(struct connectdata *conn,
                           curl_socket_t *socks,
                           int numsocks)
{
  /* write mode */
  (void)numsocks; /* unused, we trust it to be at least 1 */
  socks[0] = conn->sock[FIRSTSOCKET];
  return GETSOCK_WRITESOCK(0);
}