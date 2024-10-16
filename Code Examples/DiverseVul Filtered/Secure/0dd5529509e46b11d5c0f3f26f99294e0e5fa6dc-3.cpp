int dane_verify_crt (dane_state_t s,
	const gnutls_datum_t *chain, unsigned chain_size,
	gnutls_certificate_type_t chain_type,
	const char * hostname, const char* proto, unsigned int port,
	unsigned int sflags, unsigned int vflags,
	unsigned int *verify)
{
dane_state_t _s = NULL;
dane_query_t r = NULL;
int ret;

	*verify = 0;
	if (s == NULL) {
		ret = dane_state_init(&_s, sflags);
		if (ret < 0) {
		        gnutls_assert();
			return ret;
		}
	} else
		_s = s;

	ret = dane_query_tlsa(_s, &r, hostname, proto, port);
	if (ret < 0) {
	        gnutls_assert();
		goto cleanup;
	}
        ret = dane_verify_crt_raw (s, chain, chain_size, chain_type,
                                   r, sflags, vflags, verify);
cleanup:
	if (s == NULL) dane_state_deinit(_s);
	if (r != NULL) dane_query_deinit(r);
	return ret;
}