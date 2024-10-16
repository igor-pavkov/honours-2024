static int generate_hs_traffic_keys(gnutls_session_t session)
{
	int ret;
	unsigned null_key = 0;

	if (unlikely(session->key.proto.tls13.temp_secret_size == 0))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	ret = _tls13_derive_secret(session, DERIVED_LABEL, sizeof(DERIVED_LABEL)-1,
				   NULL, 0, session->key.proto.tls13.temp_secret,
				   session->key.proto.tls13.temp_secret);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	if ((session->security_parameters.entity == GNUTLS_CLIENT &&
	      (!(session->internals.hsk_flags & HSK_KEY_SHARE_RECEIVED) ||
	        (!(session->internals.hsk_flags & HSK_PSK_KE_MODE_DHE_PSK) &&
	           session->internals.resumed != RESUME_FALSE))) ||
	    (session->security_parameters.entity == GNUTLS_SERVER &&
	      !(session->internals.hsk_flags & HSK_KEY_SHARE_SENT))) {

		if ((session->internals.hsk_flags & HSK_PSK_SELECTED) &&
		    (session->internals.hsk_flags & HSK_PSK_KE_MODE_PSK)) {
			null_key = 1;
		}
	}

	if (null_key) {
		uint8_t digest[MAX_HASH_SIZE];
		unsigned digest_size;

		if (unlikely(session->security_parameters.prf == NULL))
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		digest_size = session->security_parameters.prf->output_size;
		memset(digest, 0, digest_size);

		ret = _tls13_update_secret(session, digest, digest_size);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	} else {
		if (unlikely(session->key.key.size == 0))
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		ret = _tls13_update_secret(session, session->key.key.data, session->key.key.size);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	}

	return 0;
}