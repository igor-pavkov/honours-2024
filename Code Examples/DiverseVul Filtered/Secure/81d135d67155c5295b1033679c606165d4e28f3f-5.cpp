static int conn_upkeep(struct connectdata *conn,
                       void *param)
{
  /* Param is unused. */
  (void)param;

  if(conn->handler->connection_check) {
    /* Do a protocol-specific keepalive check on the connection. */
    conn->handler->connection_check(conn, CONNCHECK_KEEPALIVE);
  }

  return 0; /* continue iteration */
}