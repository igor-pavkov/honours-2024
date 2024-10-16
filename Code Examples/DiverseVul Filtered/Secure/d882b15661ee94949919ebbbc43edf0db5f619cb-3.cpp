static int server_ssl_init(gnutls_session_t *session, int client_fd)
{
	int res;

	if (*session != NULL) {
		gnutls_deinit(*session);
		*session = NULL;
	}

	if ((res = gnutls_init(session, GNUTLS_SERVER)) != GNUTLS_E_SUCCESS)
		goto fail;

	if ((res = gnutls_set_default_priority(*session)) != GNUTLS_E_SUCCESS)
		goto fail;

	if ((res = gnutls_credentials_set(*session, GNUTLS_CRD_CERTIFICATE, x509_cred)) != GNUTLS_E_SUCCESS)
		goto fail;

	gnutls_transport_set_ptr(*session, (gnutls_transport_ptr_t) (ptrdiff_t) client_fd);

	if ((res = gnutls_handshake(*session)) !=  GNUTLS_E_SUCCESS)
		goto fail;

	return GNUTLS_E_SUCCESS;

fail:
	gnutls_deinit(*session);
	*session = NULL;
	return res;
}