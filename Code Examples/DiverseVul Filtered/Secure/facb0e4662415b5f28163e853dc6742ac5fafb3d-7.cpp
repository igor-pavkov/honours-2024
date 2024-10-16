static CURLcode tftp_perform(struct connectdata *conn, bool *dophase_done)
{
  CURLcode              result = CURLE_OK;
  tftp_state_data_t     *state = (tftp_state_data_t *)conn->proto.tftpc;

  *dophase_done = FALSE;

  result = tftp_state_machine(state, TFTP_EVENT_INIT);

  if((state->state == TFTP_STATE_FIN) || result)
    return result;

  tftp_multi_statemach(conn, dophase_done);

  if(*dophase_done)
    DEBUGF(infof(conn->data, "DO phase is complete\n"));

  return result;
}