static int generate_non_auth_rms_keys(gnutls_session_t session)
{
	int ret;
	/* we simulate client finished */
	uint8_t finished[MAX_HASH_SIZE+TLS_HANDSHAKE_HEADER_SIZE];
	unsigned spos;

	ret = _gnutls13_compute_finished(session->security_parameters.prf,
					 session->key.proto.tls13.hs_ckey,
					 &session->internals.handshake_hash_buffer,
					 finished+TLS_HANDSHAKE_HEADER_SIZE);
	if (ret < 0)
		return gnutls_assert_val(ret);

	spos = session->internals.handshake_hash_buffer.length;

	finished[0] = GNUTLS_HANDSHAKE_FINISHED;
	_gnutls_write_uint24(session->security_parameters.prf->output_size, finished+1);

	ret = _gnutls_buffer_append_data(&session->internals.handshake_hash_buffer, finished,
					 TLS_HANDSHAKE_HEADER_SIZE+session->security_parameters.prf->output_size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _tls13_derive_secret(session, RMS_MASTER_LABEL, sizeof(RMS_MASTER_LABEL)-1,
				   session->internals.handshake_hash_buffer.data,
				   session->internals.handshake_hash_buffer.length,
				   session->key.proto.tls13.temp_secret,
				   session->key.proto.tls13.ap_rms);
	if (ret < 0)
		return gnutls_assert_val(ret);

	session->internals.handshake_hash_buffer.length = spos;

	return 0;
}