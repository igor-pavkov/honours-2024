connection_edge_end(edge_connection_t *conn, uint8_t reason)
{
  char payload[RELAY_PAYLOAD_SIZE];
  size_t payload_len=1;
  circuit_t *circ;
  uint8_t control_reason = reason;

  if (conn->edge_has_sent_end) {
    log_warn(LD_BUG,"(Harmless.) Calling connection_edge_end (reason %d) "
             "on an already ended stream?", reason);
    tor_fragile_assert();
    return -1;
  }

  if (conn->base_.marked_for_close) {
    log_warn(LD_BUG,
             "called on conn that's already marked for close at %s:%d.",
             conn->base_.marked_for_close_file, conn->base_.marked_for_close);
    return 0;
  }

  circ = circuit_get_by_edge_conn(conn);
  if (circ && CIRCUIT_PURPOSE_IS_CLIENT(circ->purpose)) {
    /* If this is a client circuit, don't send the server an informative
     * reason code; it doesn't need to know why the client stream is
     * failing. */
    reason = END_STREAM_REASON_MISC;
  }

  payload[0] = (char)reason;
  if (reason == END_STREAM_REASON_EXITPOLICY &&
      !connection_edge_is_rendezvous_stream(conn)) {
    int addrlen;
    if (tor_addr_family(&conn->base_.addr) == AF_INET) {
      set_uint32(payload+1, tor_addr_to_ipv4n(&conn->base_.addr));
      addrlen = 4;
    } else {
      memcpy(payload+1, tor_addr_to_in6_addr8(&conn->base_.addr), 16);
      addrlen = 16;
    }
    set_uint32(payload+1+addrlen, htonl(clip_dns_ttl(conn->address_ttl)));
    payload_len += 4+addrlen;
  }

  if (circ && !circ->marked_for_close) {
    log_debug(LD_EDGE,"Sending end on conn (fd "TOR_SOCKET_T_FORMAT").",
              conn->base_.s);

    if (CIRCUIT_IS_ORIGIN(circ)) {
      origin_circuit_t *origin_circ = TO_ORIGIN_CIRCUIT(circ);
      connection_half_edge_add(conn, origin_circ);
    }

    connection_edge_send_command(conn, RELAY_COMMAND_END,
                                 payload, payload_len);
    /* We'll log warn if the connection was an hidden service and couldn't be
     * made because the service wasn't available. */
    warn_if_hs_unreachable(conn, control_reason);
  } else {
    log_debug(LD_EDGE,"No circ to send end on conn "
              "(fd "TOR_SOCKET_T_FORMAT").",
              conn->base_.s);
  }

  conn->edge_has_sent_end = 1;
  conn->end_reason = control_reason;
  return 0;
}