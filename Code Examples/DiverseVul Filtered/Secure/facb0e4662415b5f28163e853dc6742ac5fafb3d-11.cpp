static CURLcode tftp_disconnect(struct connectdata *conn, bool dead_connection)
{
  tftp_state_data_t *state = conn->proto.tftpc;
  (void) dead_connection;

  /* done, free dynamically allocated pkt buffers */
  if(state) {
    Curl_safefree(state->rpacket.data);
    Curl_safefree(state->spacket.data);
    free(state);
  }

  return CURLE_OK;
}