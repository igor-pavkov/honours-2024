static int generate_rms_keys(gnutls_session_t session)
{
	int ret;

	ret = _tls13_derive_secret(session, RMS_MASTER_LABEL, sizeof(RMS_MASTER_LABEL)-1,
				   session->internals.handshake_hash_buffer.data,
				   session->internals.handshake_hash_buffer_client_finished_len,
				   session->key.proto.tls13.temp_secret,
				   session->key.proto.tls13.ap_rms);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return 0;
}