int _gnutls13_handshake_client(gnutls_session_t session)
{
	int ret = 0;

	switch (STATE) {
	case STATE99:
	case STATE100:
#ifdef TLS13_APPENDIX_D4
		/* We send it before keys are generated. That works because CCS
		 * is always being cached and queued and not being sent directly */
		ret = _gnutls_send_change_cipher_spec(session, AGAIN(STATE100));
		STATE = STATE100;
		IMED_RET("send change cipher spec", ret, 0);
#endif
		FALLTHROUGH;
	case STATE101:
		/* Note that we check IN_FLIGHT, not ACCEPTED
		 * here. This is because the client sends early data
		 * speculatively. */
		if (session->internals.hsk_flags & HSK_EARLY_DATA_IN_FLIGHT) {
			ret = _tls13_write_connection_state_init(session, STAGE_EARLY);
			if (ret == 0) {
				_gnutls_epoch_bump(session);
				ret = _gnutls_epoch_dup(session, EPOCH_WRITE_CURRENT);
			}
			STATE = STATE101;
			IMED_RET_FATAL("set early traffic keys", ret, 0);
		}
		FALLTHROUGH;
	case STATE102:
		ret = _gnutls13_send_early_data(session);
		STATE = STATE102;
		IMED_RET("send early data", ret, 0);
		FALLTHROUGH;
	case STATE103:
		STATE = STATE103;
		ret = generate_hs_traffic_keys(session);
		/* Note that we check IN_FLIGHT, not ACCEPTED
		 * here. This is because the client sends early data
		 * speculatively. */
		IMED_RET_FATAL("generate hs traffic keys", ret, 0);
		if (session->internals.hsk_flags & HSK_EARLY_DATA_IN_FLIGHT)
			ret = _tls13_read_connection_state_init(session, STAGE_HS);
		else
			ret = _tls13_connection_state_init(session, STAGE_HS);
		IMED_RET_FATAL("set hs traffic keys", ret, 0);
		FALLTHROUGH;
	case STATE104:
		ret = _gnutls13_recv_encrypted_extensions(session);
		STATE = STATE104;
		IMED_RET("recv encrypted extensions", ret, 0);
		FALLTHROUGH;
	case STATE105:
		ret = _gnutls13_recv_certificate_request(session);
		STATE = STATE105;
		IMED_RET("recv certificate request", ret, 0);
		FALLTHROUGH;
	case STATE106:
		ret = _gnutls13_recv_certificate(session);
		STATE = STATE106;
		IMED_RET("recv certificate", ret, 0);
		FALLTHROUGH;
	case STATE107:
		ret = _gnutls13_recv_certificate_verify(session);
		STATE = STATE107;
		IMED_RET("recv server certificate verify", ret, 0);
		FALLTHROUGH;
	case STATE108:
		ret = _gnutls_run_verify_callback(session, GNUTLS_CLIENT);
		STATE = STATE108;
		if (ret < 0)
			return gnutls_assert_val(ret);
		FALLTHROUGH;
	case STATE109:
		ret = _gnutls13_recv_finished(session);
		STATE = STATE109;
		IMED_RET("recv finished", ret, 0);
		FALLTHROUGH;
	case STATE110:
		ret = _gnutls13_send_end_of_early_data(session, AGAIN(STATE110));
		STATE = STATE110;
		IMED_RET("send end of early data", ret, 0);

		/* Note that we check IN_FLIGHT, not ACCEPTED
		 * here. This is because the client sends early data
		 * speculatively. */
		if (session->internals.hsk_flags & HSK_EARLY_DATA_IN_FLIGHT) {
			session->internals.hsk_flags &= ~HSK_EARLY_DATA_IN_FLIGHT;
			ret = _tls13_write_connection_state_init(session, STAGE_HS);
			IMED_RET_FATAL("set hs traffic key after sending early data", ret, 0);
		}
		FALLTHROUGH;
	case STATE111:
		ret = _gnutls13_send_certificate(session, AGAIN(STATE111));
		STATE = STATE111;
		IMED_RET("send certificate", ret, 0);
		FALLTHROUGH;
	case STATE112:
		ret = _gnutls13_send_certificate_verify(session, AGAIN(STATE112));
		STATE = STATE112;
		IMED_RET("send certificate verify", ret, 0);
		FALLTHROUGH;
	case STATE113:
		ret = _gnutls13_send_finished(session, AGAIN(STATE113));
		STATE = STATE113;
		IMED_RET("send finished", ret, 0);
		FALLTHROUGH;
	case STATE114:
		STATE = STATE114;

		ret =
		    generate_ap_traffic_keys(session);
		IMED_RET_FATAL("generate app keys", ret, 0);

		ret = generate_rms_keys(session);
		IMED_RET_FATAL("generate rms keys", ret, 0);

		/* set traffic keys */
		ret = _tls13_connection_state_init(session, STAGE_APP);
		IMED_RET_FATAL("set app keys", ret, 0);

		STATE = STATE0;
		break;
	default:
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
	}

	/* no lock of post_negotiation_lock is required here as this is not run
	 * after handshake */
	session->internals.recv_state = RECV_STATE_0;
	session->internals.initial_negotiation_completed = 1;

	SAVE_TRANSCRIPT;

	if (session->internals.resumed != RESUME_FALSE)
		_gnutls_set_resumed_parameters(session);

	return 0;
}