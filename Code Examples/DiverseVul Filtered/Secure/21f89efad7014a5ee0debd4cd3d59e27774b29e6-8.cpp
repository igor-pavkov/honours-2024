int gnutls_set_default_priority(gnutls_session_t session)
{
	return gnutls_priority_set_direct(session, NULL, NULL);
}