static int eaptls_authenticate(void *arg, EAP_HANDLER *handler)
{
	eaptls_status_t	status;
	tls_session_t *tls_session = (tls_session_t *) handler->opaque;
	REQUEST *request = handler->request;
	eap_tls_t *inst = (eap_tls_t *) arg;

	RDEBUG2("Authenticate");

	status = eaptls_process(handler);
	RDEBUG2("eaptls_process returned %d\n", status);
	switch (status) {
		/*
		 *	EAP-TLS handshake was successful, return an
		 *	EAP-TLS-Success packet here.
		 */
	case EAPTLS_SUCCESS:
		break;

		/*
		 *	The TLS code is still working on the TLS
		 *	exchange, and it's a valid TLS request.
		 *	do nothing.
		 */
	case EAPTLS_HANDLED:
		return 1;

		/*
		 *	Handshake is done, proceed with decoding tunneled
		 *	data.
		 */
	case EAPTLS_OK:
		RDEBUG2("Received unexpected tunneled data after successful handshake.");
#ifndef NDEBUG
		if ((debug_flag > 2) && fr_log_fp) {
			unsigned int i;
			unsigned int data_len;
			unsigned char buffer[1024];

			data_len = (tls_session->record_minus)(&tls_session->dirty_in,
						buffer, sizeof(buffer));
			log_debug("  Tunneled data (%u bytes)\n", data_len);
			for (i = 0; i < data_len; i++) {
				if ((i & 0x0f) == 0x00) fprintf(fr_log_fp, "  %x: ", i);
				if ((i & 0x0f) == 0x0f) fprintf(fr_log_fp, "\n");

				fprintf(fr_log_fp, "%02x ", buffer[i]);
			}
			fprintf(fr_log_fp, "\n");
		}
#endif

		eaptls_fail(handler, 0);
		return 0;
		break;

		/*
		 *	Anything else: fail.
		 *
		 *	Also, remove the session from the cache so that
		 *	the client can't re-use it.
		 */
	default:
		if (inst->conf->session_cache_enable) {	
			SSL_CTX_remove_session(inst->ctx,
					       tls_session->ssl->session);
		}

		return 0;
	}

	/*
	 *	New sessions cause some additional information to be
	 *	cached.
	 */
	if (!SSL_session_reused(tls_session->ssl)) {
		/*
		 *	FIXME: Store miscellaneous data.
		 */
		RDEBUG2("Adding user data to cached session");
		
#if 0
		SSL_SESSION_set_ex_data(tls_session->ssl->session,
					ssl_session_idx_user_session, session_data);
#endif
	} else {
		/*
		 *	FIXME: Retrieve miscellaneous data.
		 */
#if 0
		data = SSL_SESSION_get_ex_data(tls_session->ssl->session,
					       ssl_session_idx_user_session);

		if (!session_data) {
			radlog_request(L_ERR, 0, request,
				       "No user session data in cached session - "
				       " REJECTING");
			return 0;
		}
#endif

		RDEBUG2("Retrieved session data from cached session");
	}

	/*
	 *	Success: Automatically return MPPE keys.
	 */
	return eaptls_success(handler, 0);
}