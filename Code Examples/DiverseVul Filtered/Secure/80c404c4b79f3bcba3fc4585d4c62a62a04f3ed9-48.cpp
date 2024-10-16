MOCK_IMPL(int,
connection_ap_handshake_send_begin,(entry_connection_t *ap_conn))
{
  char payload[CELL_PAYLOAD_SIZE];
  int payload_len;
  int begin_type;
  const or_options_t *options = get_options();
  origin_circuit_t *circ;
  edge_connection_t *edge_conn = ENTRY_TO_EDGE_CONN(ap_conn);
  connection_t *base_conn = TO_CONN(edge_conn);
  tor_assert(edge_conn->on_circuit);
  circ = TO_ORIGIN_CIRCUIT(edge_conn->on_circuit);

  tor_assert(base_conn->type == CONN_TYPE_AP);
  tor_assert(base_conn->state == AP_CONN_STATE_CIRCUIT_WAIT);
  tor_assert(ap_conn->socks_request);
  tor_assert(SOCKS_COMMAND_IS_CONNECT(ap_conn->socks_request->command));

  edge_conn->stream_id = get_unique_stream_id_by_circ(circ);
  if (edge_conn->stream_id==0) {
    /* XXXX+ Instead of closing this stream, we should make it get
     * retried on another circuit. */
    connection_mark_unattached_ap(ap_conn, END_STREAM_REASON_INTERNAL);

    /* Mark this circuit "unusable for new streams". */
    mark_circuit_unusable_for_new_conns(circ);
    return -1;
  }

  /* Set up begin cell flags. */
  edge_conn->begincell_flags = connection_ap_get_begincell_flags(ap_conn);

  tor_snprintf(payload,RELAY_PAYLOAD_SIZE, "%s:%d",
               (circ->base_.purpose == CIRCUIT_PURPOSE_C_GENERAL ||
                circ->base_.purpose == CIRCUIT_PURPOSE_CONTROLLER) ?
                 ap_conn->socks_request->address : "",
               ap_conn->socks_request->port);
  payload_len = (int)strlen(payload)+1;
  if (payload_len <= RELAY_PAYLOAD_SIZE - 4 && edge_conn->begincell_flags) {
    set_uint32(payload + payload_len, htonl(edge_conn->begincell_flags));
    payload_len += 4;
  }

  log_info(LD_APP,
           "Sending relay cell %d on circ %u to begin stream %d.",
           (int)ap_conn->use_begindir,
           (unsigned)circ->base_.n_circ_id,
           edge_conn->stream_id);

  begin_type = ap_conn->use_begindir ?
                 RELAY_COMMAND_BEGIN_DIR : RELAY_COMMAND_BEGIN;

  /* Check that circuits are anonymised, based on their type. */
  if (begin_type == RELAY_COMMAND_BEGIN) {
    /* This connection is a standard OR connection.
     * Make sure its path length is anonymous, or that we're in a
     * non-anonymous mode. */
    assert_circ_anonymity_ok(circ, options);
  } else if (begin_type == RELAY_COMMAND_BEGIN_DIR) {
    /* This connection is a begindir directory connection.
     * Look at the linked directory connection to access the directory purpose.
     * If a BEGINDIR connection is ever not linked, that's a bug. */
    if (BUG(!base_conn->linked)) {
      return -1;
    }
    connection_t *linked_dir_conn_base = base_conn->linked_conn;
    /* If the linked connection has been unlinked by other code, we can't send
     * a begin cell on it. */
    if (!linked_dir_conn_base) {
      return -1;
    }
    /* Sensitive directory connections must have an anonymous path length.
     * Otherwise, directory connections are typically one-hop.
     * This matches the earlier check for directory connection path anonymity
     * in directory_initiate_request(). */
    if (purpose_needs_anonymity(linked_dir_conn_base->purpose,
                    TO_DIR_CONN(linked_dir_conn_base)->router_purpose,
                    TO_DIR_CONN(linked_dir_conn_base)->requested_resource)) {
      assert_circ_anonymity_ok(circ, options);
    }
  } else {
    /* This code was written for the two connection types BEGIN and BEGIN_DIR
     */
    tor_assert_unreached();
  }

  if (connection_edge_send_command(edge_conn, begin_type,
                  begin_type == RELAY_COMMAND_BEGIN ? payload : NULL,
                  begin_type == RELAY_COMMAND_BEGIN ? payload_len : 0) < 0)
    return -1; /* circuit is closed, don't continue */

  edge_conn->package_window = STREAMWINDOW_START;
  edge_conn->deliver_window = STREAMWINDOW_START;
  base_conn->state = AP_CONN_STATE_CONNECT_WAIT;
  log_info(LD_APP,"Address/port sent, ap socket "TOR_SOCKET_T_FORMAT
           ", n_circ_id %u",
           base_conn->s, (unsigned)circ->base_.n_circ_id);
  control_event_stream_status(ap_conn, STREAM_EVENT_SENT_CONNECT, 0);

  /* If there's queued-up data, send it now */
  if ((connection_get_inbuf_len(base_conn) ||
       ap_conn->sending_optimistic_data) &&
      connection_ap_supports_optimistic_data(ap_conn)) {
    log_info(LD_APP, "Sending up to %ld + %ld bytes of queued-up data",
             (long)connection_get_inbuf_len(base_conn),
             ap_conn->sending_optimistic_data ?
             (long)buf_datalen(ap_conn->sending_optimistic_data) : 0);
    if (connection_edge_package_raw_inbuf(edge_conn, 1, NULL) < 0) {
      connection_mark_for_close(base_conn);
    }
  }

  return 0;
}