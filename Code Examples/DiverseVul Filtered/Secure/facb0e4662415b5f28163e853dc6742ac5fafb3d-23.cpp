static CURLcode tftp_state_machine(tftp_state_data_t *state,
                                   tftp_event_t event)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = state->conn->data;

  switch(state->state) {
  case TFTP_STATE_START:
    DEBUGF(infof(data, "TFTP_STATE_START\n"));
    result = tftp_send_first(state, event);
    break;
  case TFTP_STATE_RX:
    DEBUGF(infof(data, "TFTP_STATE_RX\n"));
    result = tftp_rx(state, event);
    break;
  case TFTP_STATE_TX:
    DEBUGF(infof(data, "TFTP_STATE_TX\n"));
    result = tftp_tx(state, event);
    break;
  case TFTP_STATE_FIN:
    infof(data, "%s\n", "TFTP finished");
    break;
  default:
    DEBUGF(infof(data, "STATE: %d\n", state->state));
    failf(data, "%s", "Internal state machine error");
    result = CURLE_TFTP_ILLEGAL;
    break;
  }

  return result;
}