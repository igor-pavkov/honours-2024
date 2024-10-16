static struct tevent_req *cli_session_setup_nt1_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev,
	struct cli_state *cli, const char *user,
	const char *pass, size_t passlen,
	const char *ntpass, size_t ntpasslen,
	const char *workgroup)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_nt1_state *state;
	DATA_BLOB lm_response = data_blob_null;
	DATA_BLOB nt_response = data_blob_null;
	DATA_BLOB session_key = data_blob_null;
	uint16_t *vwv;
	uint8_t *bytes;
	char *workgroup_upper;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_nt1_state);
	if (req == NULL) {
		return NULL;
	}
	state->cli = cli;
	state->user = user;
	vwv = state->vwv;

	if (passlen == 0) {
		/* do nothing - guest login */
	} else if (passlen != 24) {
		if (lp_client_ntlmv2_auth()) {
			DATA_BLOB server_chal;
			DATA_BLOB names_blob;

			server_chal =
				data_blob_const(smb1cli_conn_server_challenge(cli->conn),
						8);

			/*
			 * note that the 'workgroup' here is a best
			 * guess - we don't know the server's domain
			 * at this point. Windows clients also don't
			 * use hostname...
			 */
			names_blob = NTLMv2_generate_names_blob(
				NULL, NULL, workgroup);

			if (tevent_req_nomem(names_blob.data, req)) {
				return tevent_req_post(req, ev);
			}

			if (!SMBNTLMv2encrypt(NULL, user, workgroup, pass,
					      &server_chal, &names_blob,
					      &lm_response, &nt_response,
					      NULL, &session_key)) {
				data_blob_free(&names_blob);
				tevent_req_nterror(
					req, NT_STATUS_ACCESS_DENIED);
				return tevent_req_post(req, ev);
			}
			data_blob_free(&names_blob);

		} else {
			uchar nt_hash[16];
			E_md4hash(pass, nt_hash);

#ifdef LANMAN_ONLY
			nt_response = data_blob_null;
#else
			nt_response = data_blob(NULL, 24);
			if (tevent_req_nomem(nt_response.data, req)) {
				return tevent_req_post(req, ev);
			}

			SMBNTencrypt(pass, smb1cli_conn_server_challenge(cli->conn),
				     nt_response.data);
#endif
			/* non encrypted password supplied. Ignore ntpass. */
			if (lp_client_lanman_auth()) {

				lm_response = data_blob(NULL, 24);
				if (tevent_req_nomem(lm_response.data, req)) {
					return tevent_req_post(req, ev);
				}

				if (!SMBencrypt(pass,
						smb1cli_conn_server_challenge(cli->conn),
						lm_response.data)) {
					/*
					 * Oops, the LM response is
					 * invalid, just put the NT
					 * response there instead
					 */
					data_blob_free(&lm_response);
					lm_response = data_blob(
						nt_response.data,
						nt_response.length);
				}
			} else {
				/*
				 * LM disabled, place NT# in LM field
				 * instead
				 */
				lm_response = data_blob(
					nt_response.data, nt_response.length);
			}

			if (tevent_req_nomem(lm_response.data, req)) {
				return tevent_req_post(req, ev);
			}

			session_key = data_blob(NULL, 16);
			if (tevent_req_nomem(session_key.data, req)) {
				return tevent_req_post(req, ev);
			}
#ifdef LANMAN_ONLY
			E_deshash(pass, session_key.data);
			memset(&session_key.data[8], '\0', 8);
#else
			SMBsesskeygen_ntv1(nt_hash, session_key.data);
#endif
		}
	} else {
		/* pre-encrypted password supplied.  Only used for 
		   security=server, can't do
		   signing because we don't have original key */

		lm_response = data_blob(pass, passlen);
		if (tevent_req_nomem(lm_response.data, req)) {
			return tevent_req_post(req, ev);
		}

		nt_response = data_blob(ntpass, ntpasslen);
		if (tevent_req_nomem(nt_response.data, req)) {
			return tevent_req_post(req, ev);
		}
	}

#ifdef LANMAN_ONLY
	state->response = data_blob_talloc(
		state, lm_response.data, lm_response.length);
#else
	state->response = data_blob_talloc(
		state, nt_response.data, nt_response.length);
#endif
	if (tevent_req_nomem(state->response.data, req)) {
		return tevent_req_post(req, ev);
	}

	if (session_key.data) {
		state->session_key = data_blob_talloc(
			state, session_key.data, session_key.length);
		if (tevent_req_nomem(state->session_key.data, req)) {
			return tevent_req_post(req, ev);
		}
	}
	data_blob_free(&session_key);

	SCVAL(vwv+0, 0, 0xff);
	SCVAL(vwv+0, 1, 0);
	SSVAL(vwv+1, 0, 0);
	SSVAL(vwv+2, 0, CLI_BUFFER_SIZE);
	SSVAL(vwv+3, 0, 2);
	SSVAL(vwv+4, 0, cli_state_get_vc_num(cli));
	SIVAL(vwv+5, 0, smb1cli_conn_server_session_key(cli->conn));
	SSVAL(vwv+7, 0, lm_response.length);
	SSVAL(vwv+8, 0, nt_response.length);
	SSVAL(vwv+9, 0, 0);
	SSVAL(vwv+10, 0, 0);
	SIVAL(vwv+11, 0, cli_session_setup_capabilities(cli, 0));

	bytes = talloc_array(state, uint8_t,
			     lm_response.length + nt_response.length);
	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}
	if (lm_response.length != 0) {
		memcpy(bytes, lm_response.data, lm_response.length);
	}
	if (nt_response.length != 0) {
		memcpy(bytes + lm_response.length,
		       nt_response.data, nt_response.length);
	}
	data_blob_free(&lm_response);
	data_blob_free(&nt_response);

	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   user, strlen(user)+1, NULL);

	/*
	 * Upper case here might help some NTLMv2 implementations
	 */
	workgroup_upper = talloc_strdup_upper(talloc_tos(), workgroup);
	if (tevent_req_nomem(workgroup_upper, req)) {
		return tevent_req_post(req, ev);
	}
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn),
				   workgroup_upper, strlen(workgroup_upper)+1,
				   NULL);
	TALLOC_FREE(workgroup_upper);

	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Unix", 5, NULL);
	bytes = smb_bytes_push_str(bytes, smbXcli_conn_use_unicode(cli->conn), "Samba", 6, NULL);
	if (tevent_req_nomem(bytes, req)) {
		return tevent_req_post(req, ev);
	}

	subreq = cli_smb_send(state, ev, cli, SMBsesssetupX, 0, 13, vwv,
			      talloc_get_size(bytes), bytes);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(subreq, cli_session_setup_nt1_done, req);
	return req;
}