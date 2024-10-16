int _gnutls_session_is_resumable(gnutls_session_t session)
{
	return session->internals.resumable;
}