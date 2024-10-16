MOCK_IMPL(void,
connection_mark_unattached_ap_,(entry_connection_t *conn, int endreason,
                                int line, const char *file))
{
  connection_t *base_conn = ENTRY_TO_CONN(conn);
  tor_assert(base_conn->type == CONN_TYPE_AP);
  ENTRY_TO_EDGE_CONN(conn)->edge_has_sent_end = 1; /* no circ yet */

  if (base_conn->marked_for_close) {
    /* This call will warn as appropriate. */
    connection_mark_for_close_(base_conn, line, file);
    return;
  }

  if (!conn->socks_request->has_finished) {
    if (endreason & END_STREAM_REASON_FLAG_ALREADY_SOCKS_REPLIED)
      log_warn(LD_BUG,
               "stream (marked at %s:%d) sending two socks replies?",
               file, line);

    if (SOCKS_COMMAND_IS_CONNECT(conn->socks_request->command))
      connection_ap_handshake_socks_reply(conn, NULL, 0, endreason);
    else if (SOCKS_COMMAND_IS_RESOLVE(conn->socks_request->command))
      connection_ap_handshake_socks_resolved(conn,
                                             RESOLVED_TYPE_ERROR_TRANSIENT,
                                             0, NULL, -1, -1);
    else /* unknown or no handshake at all. send no response. */
      conn->socks_request->has_finished = 1;
  }

  connection_mark_and_flush_(base_conn, line, file);

  ENTRY_TO_EDGE_CONN(conn)->end_reason = endreason;
}