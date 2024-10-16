int _gnutls13_handshake_server(gnutls_session_t session)
{
	int ret = 0;

	switch (STATE) {
	case STATE90:
		ret = _gnutls13_handshake_hash_buffers_synth(session, session->security_parameters.prf, 0);
		STATE = STATE90;
		IMED_RET_FATAL("reset handshake buffers", ret, 0);
		FALLTHROUGH;
	case STATE91:
		ret = _gnutls13_send_hello_retry_request(session, AGAIN(STATE91));
		STATE = STATE91;
		IMED_RET("send hello retry request", ret, 0);
		FALLTHROUGH;
	case STATE92:
#ifdef TLS13_APPENDIX_D4
		ret = _gnutls_send_change_cipher_spec(session, AGAIN(STATE92));
		STATE = STATE92;
		IMED_RET("send change cipher spec", ret, 0);
#endif
		FALLTHROUGH;
	case STATE93:
		ret =
		    _gnutls_recv_handshake(session,
					   GNUTLS_HANDSHAKE_CLIENT_HELLO,
					   0, NULL);
		if (ret == GNUTLS_E_INT_RET_0) {
			/* this is triggered by post_client_hello, and instructs the
			 * handshake to proceed but be put on hold */
			ret = GNUTLS_E_INTERRUPTED;
			STATE = STATE94; /* hello already parsed -> move to next state */
		} else {
			STATE = STATE93;
		}

		IMED_RET("recv client hello", ret, 0);
		FALLTHROUGH;
	case STATE94:
		ret = _gnutls_send_server_hello(session, AGAIN(STATE94));
		STATE = STATE94;
		IMED_RET("send hello", ret, 0);
		FALLTHROUGH;
	case STATE99:
	case STATE100:
#ifdef TLS13_APPENDIX_D4
		/* don't send CCS twice: when HRR has already been
		 * sent, CCS should have followed it (see above) */
		if (!(session->internals.hsk_flags & HSK_HRR_SENT)) {
			ret = _gnutls_send_change_cipher_spec(session, AGAIN(STATE100));
			STATE = STATE100;
			IMED_RET("send change cipher spec", ret, 0);
		}
#endif
		FALLTHROUGH;
	case STATE101:
		STATE = STATE101;
		if (session->internals.hsk_flags & HSK_EARLY_DATA_ACCEPTED) {
			ret = _tls13_read_connection_state_init(session, STAGE_EARLY);
			if (ret == 0) {
				_gnutls_epoch_bump(session);
				ret = _gnutls_epoch_dup(session, EPOCH_READ_CURRENT);
			}
			IMED_RET_FATAL("set early traffic keys", ret, 0);

			ret = generate_hs_traffic_keys(session);
			IMED_RET_FATAL("generate hs traffic keys", ret, 0);

			ret = _tls13_write_connection_state_init(session, STAGE_HS);
		} else {
			ret = generate_hs_traffic_keys(session);
			IMED_RET_FATAL("generate hs traffic keys", ret, 0);

			ret = _tls13_connection_state_init(session, STAGE_HS);
		}
		IMED_RET_FATAL("set hs traffic keys", ret, 0);
		FALLTHROUGH;
	case STATE102:
		ret = _gnutls13_send_encrypted_extensions(session, AGAIN(STATE102));
		STATE = STATE102;
		IMED_RET("send encrypted extensions", ret, 0);
		FALLTHROUGH;
	case STATE103:
		ret = _gnutls13_send_certificate_request(session, AGAIN(STATE103));
		STATE = STATE103;
		IMED_RET("send certificate request", ret, 0);
		FALLTHROUGH;
	case STATE104:
		ret = _gnutls13_send_certificate(session, AGAIN(STATE104));
		STATE = STATE104;
		IMED_RET("send certificate", ret, 0);
		FALLTHROUGH;
	case STATE105:
		ret = _gnutls13_send_certificate_verify(session, AGAIN(STATE105));
		STATE = STATE105;
		IMED_RET("send certificate verify", ret, 0);
		FALLTHROUGH;
	case STATE106:
		ret = _gnutls13_send_finished(session, AGAIN(STATE106));
		STATE = STATE106;
		IMED_RET("send finished", ret, 0);
		FALLTHROUGH;
	case STATE107:
		ret = _gnutls13_recv_end_of_early_data(session);
		STATE = STATE107;
		IMED_RET("recv end of early data", ret, 0);

		if (session->internals.hsk_flags & HSK_EARLY_DATA_ACCEPTED) {
			ret = _tls13_read_connection_state_init(session, STAGE_HS);
			IMED_RET_FATAL("set hs traffic key after receiving early data", ret, 0);
		}
		FALLTHROUGH;
	case STATE108:
		/* At this point our sending keys should be the app keys
		 * see 4.4.4 at draft-ietf-tls-tls13-28 */
		ret =
		    generate_ap_traffic_keys(session);
		IMED_RET_FATAL("generate app keys", ret, 0);

		/* If the session is unauthenticated, try to optimize the handshake by
		 * sending the session ticket early. */
		if (!(session->internals.hsk_flags & (HSK_CRT_REQ_SENT|HSK_PSK_SELECTED))) {
			STATE = STATE108;

			ret = generate_non_auth_rms_keys(session);
			IMED_RET_FATAL("generate rms keys", ret, 0);

			session->internals.hsk_flags |= HSK_EARLY_START_USED;
			_gnutls_handshake_log("HSK[%p]: unauthenticated session eligible for early start\n", session);
		}

		ret = _tls13_write_connection_state_init(session, STAGE_APP);
		IMED_RET_FATAL("set write app keys", ret, 0);

		_gnutls_handshake_log("HSK[%p]: switching early to application traffic keys\n", session);

		FALLTHROUGH;
	case STATE109:
		if (session->internals.resumed != RESUME_FALSE)
			_gnutls_set_resumed_parameters(session);

		if (session->internals.hsk_flags & HSK_EARLY_START_USED) {
			ret = _gnutls13_send_session_ticket(session, TLS13_TICKETS_TO_SEND,
							    AGAIN(STATE109));

			STATE = STATE109;
			IMED_RET("send session ticket", ret, 0);

			/* complete this phase of the handshake. We
			 * should be called again by gnutls_record_recv()
			 */

			if (session->internals.flags & GNUTLS_ENABLE_EARLY_START) {
				STATE = STATE113; /* finished */
				gnutls_assert();

				session->internals.recv_state = RECV_STATE_EARLY_START;
				return 0;
			}
		}
		FALLTHROUGH;
	case STATE110:
		ret = _gnutls13_recv_certificate(session);
		STATE = STATE110;
		IMED_RET("recv certificate", ret, 0);
		FALLTHROUGH;
	case STATE111:
		ret = _gnutls13_recv_certificate_verify(session);
		STATE = STATE111;
		IMED_RET("recv certificate verify", ret, 0);
		FALLTHROUGH;
	case STATE112:
		ret = _gnutls_run_verify_callback(session, GNUTLS_CLIENT);
		STATE = STATE112;
		if (ret < 0)
			return gnutls_assert_val(ret);
		FALLTHROUGH;
	case STATE113: /* can enter from STATE109 */
		ret = _gnutls13_recv_finished(session);
		STATE = STATE113;
		IMED_RET("recv finished", ret, 0);
		FALLTHROUGH;
	case STATE114:
		/* If we did request a client certificate, then we can
		 * only send the tickets here */
		STATE = STATE114;

		if (!(session->internals.hsk_flags & HSK_EARLY_START_USED)) {
			ret = generate_rms_keys(session);
			IMED_RET_FATAL("generate rms keys", ret, 0);
		}

		ret = _tls13_read_connection_state_init(session, STAGE_APP);
		IMED_RET_FATAL("set read app keys", ret, 0);

		FALLTHROUGH;
	case STATE115:
		if (!(session->internals.hsk_flags & (HSK_TLS13_TICKET_SENT|HSK_EARLY_START_USED))) {
			ret = _gnutls13_send_session_ticket(session, TLS13_TICKETS_TO_SEND,
							    AGAIN(STATE115));
			STATE = STATE115;
			IMED_RET("send session ticket", ret, 0);
		}

		STATE = STATE0;
		break;
	default:
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
	}

	/* explicitly reset any early start flags */
	gnutls_mutex_lock(&session->internals.post_negotiation_lock);
	session->internals.recv_state = RECV_STATE_0;
	session->internals.initial_negotiation_completed = 1;
	gnutls_mutex_unlock(&session->internals.post_negotiation_lock);

	SAVE_TRANSCRIPT;


	return 0;
}