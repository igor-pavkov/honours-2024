static CURLcode tftp_done(struct connectdata *conn, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  tftp_state_data_t *state = (tftp_state_data_t *)conn->proto.tftpc;

  (void)status; /* unused */
  (void)premature; /* not used */

  if(Curl_pgrsDone(conn))
    return CURLE_ABORTED_BY_CALLBACK;

  /* If we have encountered an error */
  if(state)
    result = tftp_translate_code(state->error);

  return result;
}