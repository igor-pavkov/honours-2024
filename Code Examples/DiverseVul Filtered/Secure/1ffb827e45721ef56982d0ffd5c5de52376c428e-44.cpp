void gnutls_session_enable_compatibility_mode(gnutls_session_t session)
{
	ENABLE_COMPAT(&session->internals.priorities);
}