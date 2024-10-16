static void server_ssl_deinit(gnutls_session_t *session)
{
	gnutls_deinit(*session);
	*session = NULL;
}