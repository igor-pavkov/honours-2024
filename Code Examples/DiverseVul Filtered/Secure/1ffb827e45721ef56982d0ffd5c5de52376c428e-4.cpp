gnutls_protocol_t gnutls_protocol_get_version(gnutls_session_t session)
{
	return get_num_version(session);
}