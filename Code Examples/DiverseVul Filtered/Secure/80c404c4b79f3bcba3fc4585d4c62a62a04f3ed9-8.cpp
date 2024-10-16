MOCK_IMPL(int,
connection_ap_rewrite_and_attach_if_allowed,(entry_connection_t *conn,
                                             origin_circuit_t *circ,
                                             crypt_path_t *cpath))
{
  const or_options_t *options = get_options();

  if (options->LeaveStreamsUnattached) {
    connection_entry_set_controller_wait(conn);
    return 0;
  }
  return connection_ap_handshake_rewrite_and_attach(conn, circ, cpath);
}