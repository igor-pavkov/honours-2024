static struct tevent_req *cli_session_setup_lanman2_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	struct cli_state *cli, const char *user,
	const char *pass, size_t passlen,
	const char *workgroup)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_lanman2_state *state;
	DATA_BLOB lm_response = data_blob_null;
	uint16_t *vwv;
	uint8_t *bytes;
	char *tmp;
	uint16_t sec_mode = smb1cli_conn_server_security_mode(cli->conn);

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_lanman2_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;
	state->user = user;
	vwv = state->vwv;

	/*
	 * if in share level security then don't send a password now
	 */
	if (!(sec_mode & NEGOTIATE_SECURITY_USER_LEVEL)) {
		passlen = 0;
	}

	if (passlen > 0
	    && (sec_mode & NEGOTIATE_SECURITY_CHALLENGE_RESPONSE)
	    && passlen != 24) {
		/*
		 * Encrypted mode needed, and non encrypted password
		 * supplied.
		 */
		lm_response = data_blob(NULL, 24);
		if (tevent_req_nomem(lm_response.data, req)) {
			return tevent_req_post(req, ev);
		}

		if (!SMBencrypt(pass, smb1cli_conn_server_challenge(cli->conn),
				(uint8_t *)lm_response.data)) {
			DEBUG(1, ("Password is > 14 chars in length, and is "
				  "therefore incompatible with Lanman "
				  "authentication\n"));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return tevent_req_post(req, ev);
		}
	} else if ((sec_mode & NEGOTIATE_SECURITY_CHALLENGE_RESPONSE)
		   && passlen == 24) {
		/*
		 * Encrypted mode needed, and encrypted password
		 * supplied.
		 */
		lm_response = data_blob(pass, passlen);
		if (tevent_req_nomem(lm_response.data, req)) {
			return tevent_req_post(req, ev);
		}
	} else if (passlen > 0) {
		uint8_t *buf;
		size_t converted_size;
		/*
		 * Plaintext mode needed, assume plaintext supplied.
		 */
		buf = talloc_array(talloc_tos(), uint8_t, 0);
		buf = smb_bytes_push_str(buf, smbXcli_conn_use_unicode(cli->conn), pass, passlen+1,
					 &converted_size);
		if (tevent_req_nomem(buf, req)) {
			return tevent_req_post(req, ev);
		}
		lm_response = data_blob(pass, passlen);
		TALLOC_FREE(buf);
		if (tevent_req_nomem(lm_response.data, req)) {
			return tevent_req_post(req, ev);
		}
	}

	SCVAL(vwv+0, 0, 0xff);
	SCVAL(vwv+0, 1, 0);
	SSVAL(vwv+1, 0, 0);
	SSVAL(vwv+2, 0, CLI_BUFFER_SIZE);
	SSVAL(vwv+3, 0, 2);
	SSVAL(vwv+4, 0, 1);
	SIVAL(vwv+5, 0, smb1cli_conn_server_session_key(cli->conn));
	SSVAL(vwv+7, 0, lm_response.length);

	bytes = talloc_array(state, uint8_t, lm_response.length);
	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}
	if (lm_response.length != 0) {
		memcpy(bytes, lm_response.data, lm_response.length);
	}
	data_blob_free(&lm_response);

	tmp = talloc_strdup_upper(talloc_tos(), user);
	if (tevent_req_nomem(tmp, req)) {
		return tevent_req_post(req, ev);
	}
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), tmp, strlen(tmp)+1,
				   NULL);
	TALLOC_FREE(tmp);

	tmp = talloc_strdup_upper(talloc_tos(), workgroup);
	if (tevent_req_nomem(tmp, req)) {
		return tevent_req_post(req, ev);
	}
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), tmp, strlen(tmp)+1,
				   NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Unix", 5, NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Samba", 6, NULL);

	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}

	subreq = cli_smb_send(state, ev, cli, SMBsesssetupX, 0, 10, vwv,
			      talloc_get_size(bytes), bytes);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_session_setup_lanman2_done, req);
	return req;
}