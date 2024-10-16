MOCK_IMPL(void,
connection_mark_unattached_ap_,(entry_connection_t *conn, int endreason,
                                int line, const char *file))
{
  connection_t *base_conn = ENTRY_TO_CONN(conn);
  edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(conn);
  tor_assert(base_conn->type == CONN_TYPE_AP);
  ENTRY_TO_EDGE_CONN(conn)->edge_has_sent_end = 1; /* no circ yet */

  /* If this is a rendezvous stream and it is failing without ever
   * being attached to a circuit, assume that an attempt to connect to
   * the destination hidden service has just ended.
   *
   * XXXX This condition doesn't limit to only streams failing
   * without ever being attached.  That sloppiness should be harmless,
   * but we should fix it someday anyway. */
  if ((edge_conn->on_circuit != NULL || edge_conn->edge_has_sent_end) &&
      connection_edge_is_rendezvous_stream(edge_conn)) {
    if (edge_conn->rend_data) {
      rend_client_note_connection_attempt_ended(edge_conn->rend_data);
    }
  }

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