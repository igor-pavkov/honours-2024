static NTSTATUS cli_connect_sock_recv(struct tevent_req *req,
				      int *pfd, uint16_t *pport)
{
	struct cli_connect_sock_state *state = tevent_req_data(
		req, struct cli_connect_sock_state);
	NTSTATUS status;

	if (tevent_req_is_nterror(req, &status)) {
		return status;
	}
	*pfd = state->fd;
	*pport = state->port;
	return NT_STATUS_OK;
}