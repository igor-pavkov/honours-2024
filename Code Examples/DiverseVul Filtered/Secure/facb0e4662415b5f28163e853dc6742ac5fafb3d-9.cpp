static CURLcode tftp_do(struct connectdata *conn, bool *done)
{
  tftp_state_data_t *state;
  CURLcode result;

  *done = FALSE;

  if(!conn->proto.tftpc) {
    result = tftp_connect(conn, done);
    if(result)
      return result;
  }

  state = (tftp_state_data_t *)conn->proto.tftpc;
  if(!state)
    return CURLE_TFTP_ILLEGAL;

  result = tftp_perform(conn, done);

  /* If tftp_perform() returned an error, use that for return code. If it
     was OK, see if tftp_translate_code() has an error. */
  if(!result)
    /* If we have encountered an internal tftp error, translate it. */
    result = tftp_translate_code(state->error);

  return result;
}