static struct tevent_req *cli_session_setup_spnego_send(
	TALLOC_CTX *mem_ctx, struct tevent_context *ev, struct cli_state *cli,
	const char *user, const char *pass, const char *user_domain)
{
	struct tevent_req *req, *subreq;
	struct cli_session_setup_spnego_state *state;
	char *principal = NULL;
	char *OIDs[ASN1_MAX_OIDS];
	int i;
	const char *dest_realm = cli_state_remote_realm(cli);
	const DATA_BLOB *server_blob;

	req = tevent_req_create(mem_ctx, &state,
				struct cli_session_setup_spnego_state);
	if (req == NULL) {
		return NULL;
	}
	state->ev = ev;
	state->cli = cli;
	state->user = user;
	state->pass = pass;
	state->user_domain = user_domain;
	state->dest_realm = dest_realm;

	state->account = cli_session_setup_get_account(state, user);
	if (tevent_req_nomem(state->account, req)) {
		return tevent_req_post(req, ev);
	}

	state->target_hostname = smbXcli_conn_remote_name(cli->conn);
	server_blob = smbXcli_conn_server_gss_blob(cli->conn);

	DEBUG(3,("Doing spnego session setup (blob length=%lu)\n",
		 (unsigned long)server_blob->length));

	/* the server might not even do spnego */
	if (server_blob->length == 0) {
		DEBUG(3,("server didn't supply a full spnego negprot\n"));
		goto ntlmssp;
	}

#if 0
	file_save("negprot.dat", cli->secblob.data, cli->secblob.length);
#endif

	/* The server sent us the first part of the SPNEGO exchange in the
	 * negprot reply. It is WRONG to depend on the principal sent in the
	 * negprot reply, but right now we do it. If we don't receive one,
	 * we try to best guess, then fall back to NTLM.  */
	if (!spnego_parse_negTokenInit(state, *server_blob, OIDs,
				       &principal, NULL) ||
			OIDs[0] == NULL) {
		state->result = ADS_ERROR_NT(NT_STATUS_INVALID_PARAMETER);
		tevent_req_done(req);
		return tevent_req_post(req, ev);
	}

	/* make sure the server understands kerberos */
	for (i=0;OIDs[i];i++) {
		if (i == 0)
			DEBUG(3,("got OID=%s\n", OIDs[i]));
		else
			DEBUGADD(3,("got OID=%s\n", OIDs[i]));
		if (strcmp(OIDs[i], OID_KERBEROS5_OLD) == 0 ||
		    strcmp(OIDs[i], OID_KERBEROS5) == 0) {
			cli->got_kerberos_mechanism = True;
		}
		talloc_free(OIDs[i]);
	}

	DEBUG(3,("got principal=%s\n", principal ? principal : "<null>"));

#ifdef HAVE_KRB5
	/* If password is set we reauthenticate to kerberos server
	 * and do not store results */

	if (user && *user && cli->got_kerberos_mechanism && cli->use_kerberos) {
		char *tmp;

		tmp = cli_session_setup_get_principal(
			talloc_tos(), principal, state->target_hostname, dest_realm);
		TALLOC_FREE(principal);
		principal = tmp;

		if (pass && *pass) {
			int ret;

			use_in_memory_ccache();
			ret = kerberos_kinit_password(user, pass, 0 /* no time correction for now */, NULL);

			if (ret){
				DEBUG(0, ("Kinit for %s to access %s failed: %s\n", user, principal, error_message(ret)));
				TALLOC_FREE(principal);
				if (cli->fallback_after_kerberos)
					goto ntlmssp;
				state->result = ADS_ERROR_KRB5(ret);
				tevent_req_done(req);
				return tevent_req_post(req, ev);
			}
		}

		if (principal) {
			subreq = cli_session_setup_gensec_send(
				state, ev, cli,
				state->account, pass, user_domain,
				CRED_MUST_USE_KERBEROS,
				"cifs", state->target_hostname, principal);
			if (tevent_req_nomem(subreq, req)) {
				return tevent_req_post(req, ev);
			}
			tevent_req_set_callback(
				subreq, cli_session_setup_spnego_done_krb,
				req);
			return req;
		}
	}
#endif

ntlmssp:
	subreq = cli_session_setup_gensec_send(
		state, state->ev, state->cli,
		state->account, state->pass, state->user_domain,
		CRED_DONT_USE_KERBEROS,
		"cifs", state->target_hostname, NULL);
	if (tevent_req_nomem(subreq, req)) {
		return tevent_req_post(req, ev);
	}
	tevent_req_set_callback(
		subreq, cli_session_setup_spnego_done_ntlmssp, req);
	return req;
}