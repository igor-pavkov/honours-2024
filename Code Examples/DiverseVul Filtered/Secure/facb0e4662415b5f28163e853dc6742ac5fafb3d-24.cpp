static CURLcode tftp_set_timeouts(tftp_state_data_t *state)
{
  time_t maxtime, timeout;
  timediff_t timeout_ms;
  bool start = (state->state == TFTP_STATE_START) ? TRUE : FALSE;

  time(&state->start_time);

  /* Compute drop-dead time */
  timeout_ms = Curl_timeleft(state->conn->data, NULL, start);

  if(timeout_ms < 0) {
    /* time-out, bail out, go home */
    failf(state->conn->data, "Connection time-out");
    return CURLE_OPERATION_TIMEDOUT;
  }

  if(start) {

    maxtime = (time_t)(timeout_ms + 500) / 1000;
    state->max_time = state->start_time + maxtime;

    /* Set per-block timeout to total */
    timeout = maxtime;

    /* Average restart after 5 seconds */
    state->retry_max = (int)timeout/5;

    if(state->retry_max < 1)
      /* avoid division by zero below */
      state->retry_max = 1;

    /* Compute the re-start interval to suit the timeout */
    state->retry_time = (int)timeout/state->retry_max;
    if(state->retry_time<1)
      state->retry_time = 1;

  }
  else {
    if(timeout_ms > 0)
      maxtime = (time_t)(timeout_ms + 500) / 1000;
    else
      maxtime = 3600;

    state->max_time = state->start_time + maxtime;

    /* Set per-block timeout to total */
    timeout = maxtime;

    /* Average reposting an ACK after 5 seconds */
    state->retry_max = (int)timeout/5;
  }
  /* But bound the total number */
  if(state->retry_max<3)
    state->retry_max = 3;

  if(state->retry_max>50)
    state->retry_max = 50;

  /* Compute the re-ACK interval to suit the timeout */
  state->retry_time = (int)(timeout/state->retry_max);
  if(state->retry_time<1)
    state->retry_time = 1;

  infof(state->conn->data,
        "set timeouts for state %d; Total %ld, retry %d maxtry %d\n",
        (int)state->state, (long)(state->max_time-state->start_time),
        state->retry_time, state->retry_max);

  /* init RX time */
  time(&state->rx_time);

  return CURLE_OK;
}