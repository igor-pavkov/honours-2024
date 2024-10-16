int tls1_process_sigalgs(SSL *s, const unsigned char *data, int dsize)
	{
	int idx;
	size_t i;
	const EVP_MD *md;
	CERT *c = s->cert;
	TLS_SIGALGS *sigptr;
	/* Extension ignored for inappropriate versions */
	if (!SSL_USE_SIGALGS(s))
		return 1;
	/* Should never happen */
	if (!c)
		return 0;

	c->peer_sigalgs = OPENSSL_malloc(dsize);
	if (!c->peer_sigalgs)
		return 0;
	c->peer_sigalgslen = dsize;
	memcpy(c->peer_sigalgs, data, dsize);

	tls1_set_shared_sigalgs(s);

#ifdef OPENSSL_SSL_DEBUG_BROKEN_PROTOCOL
	if (s->cert->cert_flags & SSL_CERT_FLAG_BROKEN_PROTOCOL)
		{
		/* Use first set signature preference to force message
		 * digest, ignoring any peer preferences.
		 */
		const unsigned char *sigs = NULL;
		if (s->server)
			sigs = c->conf_sigalgs;
		else
			sigs = c->client_sigalgs;
		if (sigs)
			{
			idx = tls12_get_pkey_idx(sigs[1]);
			md = tls12_get_hash(sigs[0]);
			c->pkeys[idx].digest = md;
			c->pkeys[idx].valid_flags = CERT_PKEY_EXPLICIT_SIGN;
			if (idx == SSL_PKEY_RSA_SIGN)
				{
				c->pkeys[SSL_PKEY_RSA_ENC].valid_flags = CERT_PKEY_EXPLICIT_SIGN;
				c->pkeys[SSL_PKEY_RSA_ENC].digest = md;
				}
			}
		}
#endif

	for (i = 0, sigptr = c->shared_sigalgs;
			i < c->shared_sigalgslen; i++, sigptr++)
		{
		idx = tls12_get_pkey_idx(sigptr->rsign);
		if (idx > 0 && c->pkeys[idx].digest == NULL)
			{
			md = tls12_get_hash(sigptr->rhash);
			c->pkeys[idx].digest = md;
			c->pkeys[idx].valid_flags = CERT_PKEY_EXPLICIT_SIGN;
			if (idx == SSL_PKEY_RSA_SIGN)
				{
				c->pkeys[SSL_PKEY_RSA_ENC].valid_flags = CERT_PKEY_EXPLICIT_SIGN;
				c->pkeys[SSL_PKEY_RSA_ENC].digest = md;
				}
			}

		}
	/* In strict mode leave unset digests as NULL to indicate we can't
	 * use the certificate for signing.
	 */
	if (!(s->cert->cert_flags & SSL_CERT_FLAGS_CHECK_TLS_STRICT))
		{
		/* Set any remaining keys to default values. NOTE: if alg is
		 * not supported it stays as NULL.
	 	 */
#ifndef OPENSSL_NO_DSA
		if (!c->pkeys[SSL_PKEY_DSA_SIGN].digest)
			c->pkeys[SSL_PKEY_DSA_SIGN].digest = EVP_sha1();
#endif
#ifndef OPENSSL_NO_RSA
		if (!c->pkeys[SSL_PKEY_RSA_SIGN].digest)
			{
			c->pkeys[SSL_PKEY_RSA_SIGN].digest = EVP_sha1();
			c->pkeys[SSL_PKEY_RSA_ENC].digest = EVP_sha1();
			}
#endif
#ifndef OPENSSL_NO_ECDSA
		if (!c->pkeys[SSL_PKEY_ECC].digest)
			c->pkeys[SSL_PKEY_ECC].digest = EVP_sha1();
#endif
		}
	return 1;
	}