void *gnutls_session_get_ptr(gnutls_session_t session)
{
	return session->internals.user_ptr;
}