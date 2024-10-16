void gnutls_priority_deinit(gnutls_priority_t priority_cache)
{
	gnutls_free(priority_cache);
}