static bool cli_sesssetup_blob_next(struct cli_sesssetup_blob_state *state,
				    struct tevent_req **psubreq)
{
	struct tevent_req *subreq;
	uint16_t thistime;

	thistime = MIN(state->blob.length, state->max_blob_size);

	if (smbXcli_conn_protocol(state->cli->conn) >= PROTOCOL_SMB2_02) {

		state->smb2_blob.data = state->blob.data;
		state->smb2_blob.length = thistime;

		state->blob.data += thistime;
		state->blob.length -= thistime;

		subreq = smb2cli_session_setup_send(state, state->ev,
						    state->cli->conn,
						    state->cli->timeout,
						    state->cli->smb2.session,
						    0, /* in_flags */
						    SMB2_CAP_DFS, /* in_capabilities */
						    0, /* in_channel */
						    0, /* in_previous_session_id */
						    &state->smb2_blob);
		if (subreq == NULL) {
			return false;
		}
		*psubreq = subreq;
		return true;
	}

	SCVAL(state->vwv+0, 0, 0xFF);
	SCVAL(state->vwv+0, 1, 0);
	SSVAL(state->vwv+1, 0, 0);
	SSVAL(state->vwv+2, 0, CLI_BUFFER_SIZE);
	SSVAL(state->vwv+3, 0, 2);
	SSVAL(state->vwv+4, 0, 1);
	SIVAL(state->vwv+5, 0, 0);

	SSVAL(state->vwv+7, 0, thistime);

	SSVAL(state->vwv+8, 0, 0);
	SSVAL(state->vwv+9, 0, 0);
	SIVAL(state->vwv+10, 0,
		cli_session_setup_capabilities(state->cli, CAP_EXTENDED_SECURITY));

	state->buf = (uint8_t *)talloc_memdup(state, state->blob.data,
					      thistime);
	if (state->buf == NULL) {
		return false;
	}
	state->blob.data += thistime;
	state->blob.length -= thistime;

	state->buf = smb_bytes_push_str(state->buf, smbXcli_conn_use_unicode(state->cli->conn),
					"Unix", 5, NULL);
	state->buf = smb_bytes_push_str(state->buf, smbXcli_conn_use_unicode(state->cli->conn),
					"Samba", 6, NULL);
	if (state->buf == NULL) {
		return false;
	}
	subreq = cli_smb_send(state, state->ev, state->cli, SMBsesssetupX, 0,
			      12, state->vwv,
			      talloc_get_size(state->buf), state->buf);
	if (subreq == NULL) {
		return false;
	}
	*psubreq = subreq;
	return true;
}