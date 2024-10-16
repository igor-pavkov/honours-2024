int gnutls_session_resumption_requested(gnutls_session_t session)
{
	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		return 0;
	} else {
		return session->internals.resumption_requested;
	}
}