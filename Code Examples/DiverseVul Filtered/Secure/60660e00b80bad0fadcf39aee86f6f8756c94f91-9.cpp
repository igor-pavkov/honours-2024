static void ssl_disconnect(struct tunnel *tunnel)
{
	if (!tunnel->ssl_handle)
		return;

	SSL_shutdown(tunnel->ssl_handle);
	SSL_free(tunnel->ssl_handle);
	SSL_CTX_free(tunnel->ssl_context);
	close(tunnel->ssl_socket);

	tunnel->ssl_handle = NULL;
	tunnel->ssl_context = NULL;
}