struct tevent_req *cli_session_setup_send(TALLOC_CTX *mem_ctx,
					  struct tevent_context *ev,
					  struct cli_state *cli,
					  const char *user,
					  const char *pass, int passlen,
					  const char *ntpass, int ntpasslen,
					  const char *workgroup)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_state *state;
	char *p;
	char *user2;
	uint16_t sec_mode = smb1cli_conn_server_security_mode(cli->conn);

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_state);
	if (req == NULL) {
		return NULL;
	}

	if (user) {
		user2 = talloc_strdup(state, user);
	} else {
		user2 = talloc_strdup(state, "");
	}
	if (user2 == NULL) {
		tevent_req_oom(req);
		return tevent_req_post(req, ev);
	}

	if (!workgroup) {
		workgroup = "";
	}

	/* allow for workgroups as part of the username */
	if ((p=strchr_m(user2,'\\')) || (p=strchr_m(user2,'/')) ||
	    (p=strchr_m(user2,*lp_winbind_separator()))) {
		*p = 0;
		user = p+1;
		if (!strupper_m(user2)) {
			tevent_req_nterror(req, NT_STATUS_INVALID_PARAMETER);
			return tevent_req_post(req, ev);
		}
		workgroup = user2;
	}

	if (smbXcli_conn_protocol(cli->conn) < PROTOCOL_LANMAN1) {
		tevent_req_done(req);
		return tevent_req_post(req, ev);
	}

	/* now work out what sort of session setup we are going to
           do. I have split this into separate functions to make the
           flow a bit easier to understand (tridge) */

	/* if its an older server then we have to use the older request format */

	if (smbXcli_conn_protocol(cli->conn) < PROTOCOL_NT1) {
		if (!lp_client_lanman_auth() && passlen != 24 && (*pass)) {
			DEBUG(1, ("Server requested LM password but 'client lanman auth = no'"
				  " or 'client ntlmv2 auth = yes'\n"));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return tevent_req_post(req, ev);
		}

		if ((sec_mode & NEGOTIATE_SECURITY_CHALLENGE_RESPONSE) == 0 &&
		    !lp_client_plaintext_auth() && (*pass)) {
			DEBUG(1, ("Server requested PLAINTEXT password but 'client plaintext auth = no'"
				  " or 'client ntlmv2 auth = yes'\n"));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return tevent_req_post(req, ev);
		}

		subreq = cli_session_setup_lanman2_send(
			state, ev, cli, user, pass, passlen, workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_lanman2,
					req);
		return req;
	}

	if (smbXcli_conn_protocol(cli->conn) >= PROTOCOL_SMB2_02) {
		subreq = cli_session_setup_spnego_send(
			state, ev, cli, user, pass, workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_spnego,
					req);
		return req;
	}

	/*
	 * if the server supports extended security then use SPNEGO
	 * even for anonymous connections.
	 */
	if (smb1cli_conn_capabilities(cli->conn) & CAP_EXTENDED_SECURITY) {
		subreq = cli_session_setup_spnego_send(
			state, ev, cli, user, pass, workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_spnego,
					req);
		return req;
	}

	/* if no user is supplied then we have to do an anonymous connection.
	   passwords are ignored */

	if (!user || !*user) {
		subreq = cli_session_setup_guest_send(state, ev, cli);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_guest,
					req);
		return req;
	}

	/* if the server is share level then send a plaintext null
           password at this point. The password is sent in the tree
           connect */

	if ((sec_mode & NEGOTIATE_SECURITY_USER_LEVEL) == 0) {
		subreq = cli_session_setup_plain_send(
			state, ev, cli, user, "", workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_plain,
					req);
		return req;
	}

	/* if the server doesn't support encryption then we have to use 
	   plaintext. The second password is ignored */

	if ((sec_mode & NEGOTIATE_SECURITY_CHALLENGE_RESPONSE) == 0) {
		if (!lp_client_plaintext_auth() && (*pass)) {
			DEBUG(1, ("Server requested PLAINTEXT password but 'client plaintext auth = no'"
				  " or 'client ntlmv2 auth = yes'\n"));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return tevent_req_post(req, ev);
		}
		subreq = cli_session_setup_plain_send(
			state, ev, cli, user, pass, workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_plain,
					req);
		return req;
	}

	{
		/* otherwise do a NT1 style session setup */
		if (lp_client_ntlmv2_auth() && lp_client_use_spnego()) {
			/*
			 * Don't send an NTLMv2 response without NTLMSSP
			 * if we want to use spnego support
			 */
			DEBUG(1, ("Server does not support EXTENDED_SECURITY "
				  " but 'client use spnego = yes"
				  " and 'client ntlmv2 auth = yes'\n"));
			tevent_req_nterror(req, NT_STATUS_ACCESS_DENIED);
			return tevent_req_post(req, ev);
		}

		subreq = cli_session_setup_nt1_send(
			state, ev, cli, user, pass, passlen, ntpass, ntpasslen,
			workgroup);
		if (tevent_req_nomem(subreq, req)) {
			return tevent_req_post(req, ev);
		}
		tevent_req_set_callback(subreq, cli_session_setup_done_nt1,
					req);
		return req;
	}

	tevent_req_done(req);
	return tevent_req_post(req, ev);
}