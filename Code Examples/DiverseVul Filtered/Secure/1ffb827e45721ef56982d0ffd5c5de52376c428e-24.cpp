void gnutls_session_set_ptr(gnutls_session_t session, void *ptr)
{
	session->internals.user_ptr = ptr;
}