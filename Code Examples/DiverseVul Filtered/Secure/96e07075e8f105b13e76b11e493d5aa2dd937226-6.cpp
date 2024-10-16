static int generate_ap_traffic_keys(gnutls_session_t session)
{
	int ret;
	uint8_t zero[MAX_HASH_SIZE];

	ret = _tls13_derive_secret(session, DERIVED_LABEL, sizeof(DERIVED_LABEL)-1,
				   NULL, 0, session->key.proto.tls13.temp_secret,
				   session->key.proto.tls13.temp_secret);
	if (ret < 0)
		return gnutls_assert_val(ret);

	memset(zero, 0, session->security_parameters.prf->output_size);
	ret = _tls13_update_secret(session, zero, session->security_parameters.prf->output_size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _tls13_derive_secret(session, EXPORTER_MASTER_LABEL, sizeof(EXPORTER_MASTER_LABEL)-1,
				   session->internals.handshake_hash_buffer.data,
				   session->internals.handshake_hash_buffer_server_finished_len,
				   session->key.proto.tls13.temp_secret,
				   session->key.proto.tls13.ap_expkey);
	if (ret < 0)
		return gnutls_assert_val(ret);

	_gnutls_nss_keylog_write(session, "EXPORTER_SECRET",
				 session->key.proto.tls13.ap_expkey,
				 session->security_parameters.prf->output_size);

	_gnutls_epoch_bump(session);
	ret = _gnutls_epoch_dup(session, EPOCH_READ_CURRENT);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return 0;
}