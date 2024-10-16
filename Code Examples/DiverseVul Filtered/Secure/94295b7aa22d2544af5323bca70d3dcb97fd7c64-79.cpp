static NTSTATUS cli_raw_tcon_recv(struct tevent_req *req,
				  uint16_t *max_xmit, uint16_t *tid)
{
	struct cli_raw_tcon_state *state = tevent_req_data(
		req, struct cli_raw_tcon_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		return status;
	}
	*max_xmit = SVAL(state->ret_vwv + 0, 0);
	*tid = SVAL(state->ret_vwv + 1, 0);
	return NT_STATUS_OK;
}