static NTSTATUS cli_sesssetup_blob_recv(struct tevent_req *req,
					TALLOC_CTX *mem_ctx,
					DATA_BLOB *pblob,
					uint8_t **pinbuf,
					struct iovec **precv_iov)
{
	struct cli_sesssetup_blob_state *state = tevent_req_data(
		req, struct cli_sesssetup_blob_state);
	NTSTATUS status;
	uint8_t *inbuf;
	struct iovec *recv_iov;

	if (tevent_req_is_nterror(req, &status)) {
		TALLOC_FREE(state->cli->smb2.session);
		cli_state_set_uid(state->cli, UID_FIELD_INVALID);
		return status;
	}

	inbuf = talloc_move(mem_ctx, &state->inbuf);
	recv_iov = talloc_move(mem_ctx, &state->recv_iov);
	if (pblob != NULL) {
		*pblob = state->ret_blob;
	}
	if (pinbuf != NULL) {
		*pinbuf = inbuf;
	}
	if (precv_iov != NULL) {
		*precv_iov = recv_iov;
	}
        /* could be NT_STATUS_MORE_PROCESSING_REQUIRED */
	return state->status;
}