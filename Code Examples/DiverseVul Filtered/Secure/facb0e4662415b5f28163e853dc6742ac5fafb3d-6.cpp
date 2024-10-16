static CURLcode tftp_connect_for_rx(tftp_state_data_t *state,
                                    tftp_event_t event)
{
  CURLcode result;
#ifndef CURL_DISABLE_VERBOSE_STRINGS
  struct Curl_easy *data = state->conn->data;

  infof(data, "%s\n", "Connected for receive");
#endif
  state->state = TFTP_STATE_RX;
  result = tftp_set_timeouts(state);
  if(result)
    return result;
  return tftp_rx(state, event);
}