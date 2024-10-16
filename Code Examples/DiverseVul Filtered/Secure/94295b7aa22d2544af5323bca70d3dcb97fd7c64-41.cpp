struct tevent_req *cli_tcon_andx_create(TALLOC_CTX *mem_ctx,
					struct tevent_context *ev,
					struct cli_state *cli,
					const char *share, const char *dev,
					const char *pass, int passlen,
					struct tevent_req **psmbreq)
{
	struct tevent_req *req, *subreq;
	struct cli_tcon_andx_state *state;
	uint8_t p24[24];
	uint16_t *vwv;
	char *tmp = NULL;
	uint8_t *bytes;
	uint16_t sec_mode = smb1cli_conn_server_security_mode(cli->conn);
	uint16_t tcon_flags = 0;

	*psmbreq = NULL;

	req = tevent_req_create(mem_ctx, &state, struct cli_tcon_andx_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;
	vwv = state->vwv;

	cli->share = talloc_strdup(cli, share);
	if (!cli->share) {
		return NULL;
	}

	/* in user level security don't send a password now */
	if (sec_mode & NEGOTIATE_SECURITY_USER_LEVEL) {
		passlen = 1;
		pass = "";
	} else if (pass == NULL) {
		DEBUG(1, ("Server not using user level security and no "
			  "password supplied.\n"));
		goto access_denied;
	}

	if ((sec_mode & NEGOTIATE_SECURITY_CHALLENGE_RESPONSE) &&
	    *pass && passlen != 24) {
		if (!lp_client_lanman_auth()) {
			DEBUG(1, ("Server requested LANMAN password "
				  "(share-level security) but "
				  "'client lanman auth = no' or 'client ntlmv2 auth = yes'\n"));
			goto access_denied;
		}

		/*
		 * Non-encrypted passwords - convert to DOS codepage before
		 * encryption.
		 */
		SMBencrypt(pass, smb1cli_conn_server_challenge(cli->conn), p24);
		passlen = 24;
		pass = (const char *)p24;
	} else {
		if((sec_mode & (NEGOTIATE_SECURITY_USER_LEVEL
				     |NEGOTIATE_SECURITY_CHALLENGE_RESPONSE))
		   == 0) {
			uint8_t *tmp_pass;

			if (!lp_client_plaintext_auth() && (*pass)) {
				DEBUG(1, ("Server requested PLAINTEXT "
					  "password but "
					  "'client plaintext auth = no' or 'client ntlmv2 auth = yes'\n"));
				goto access_denied;
			}

			/*
			 * Non-encrypted passwords - convert to DOS codepage
			 * before using.
			 */
			tmp_pass = talloc_array(talloc_tos(), uint8_t, 0);
			if (tevent_req_nomem(tmp_pass, req)) {
				return tevent_req_post(req, ev);
			}
			tmp_pass = trans2_bytes_push_str(tmp_pass,
							 false, /* always DOS */
							 pass,
							 passlen,
							 NULL);
			if (tevent_req_nomem(tmp_pass, req)) {
				return tevent_req_post(req, ev);
			}
			pass = (const char *)tmp_pass;
			passlen = talloc_get_size(tmp_pass);
		}
	}

	tcon_flags |= TCONX_FLAG_EXTENDED_RESPONSE;
	tcon_flags |= TCONX_FLAG_EXTENDED_SIGNATURES;

	SCVAL(vwv+0, 0, 0xFF);
	SCVAL(vwv+0, 1, 0);
	SSVAL(vwv+1, 0, 0);
	SSVAL(vwv+2, 0, tcon_flags);
	SSVAL(vwv+3, 0, passlen);

	if (passlen && pass) {
		bytes = (uint8_t *)talloc_memdup(state, pass, passlen);
	} else {
		bytes = talloc_array(state, uint8_t, 0);
	}

	/*
	 * Add the sharename
	 */
	tmp = talloc_asprintf_strupper_m(talloc_tos(), "\\\\%s\\%s",
					 smbXcli_conn_remote_name(cli->conn), share);
	if (tmp == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), tmp, strlen(tmp)+1,
				   NULL);
	TALLOC_FREE(tmp);

	/*
	 * Add the devicetype
	 */
	tmp = talloc_strdup_upper(talloc_tos(), dev);
	if (tmp == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}
	bytes = smb_bytes_push_str(bytes, false, tmp, strlen(tmp)+1, NULL);
	TALLOC_FREE(tmp);

	if (bytes == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}

	state->bytes.iov_base = (void *)bytes;
	state->bytes.iov_len = talloc_get_size(bytes);

	subreq = cli_smb_req_create(state, ev, cli, SMBtconX, 0, 4, vwv,
				    1, &state->bytes);
	if (subreq == NULL) {
		TALLOC_FREE(req);
		return NULL;
	}
	tevent_req_set_callback(subreq, cli_tcon_andx_done, req);
	*psmbreq = subreq;
	return req;

 access_denied:
	tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
	return tevent_req_post(req, ev);
}