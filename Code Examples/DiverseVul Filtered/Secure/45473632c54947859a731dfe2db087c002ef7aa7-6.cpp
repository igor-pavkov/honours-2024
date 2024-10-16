void ssl_set_client_disabled(SSL *s)
	{
	CERT *c = s->cert;
	const unsigned char *sigalgs;
	size_t i, sigalgslen;
	int have_rsa = 0, have_dsa = 0, have_ecdsa = 0;
	c->mask_a = 0;
	c->mask_k = 0;
	/* Don't allow TLS 1.2 only ciphers if we don't suppport them */
	if (!SSL_CLIENT_USE_TLS1_2_CIPHERS(s))
		c->mask_ssl = SSL_TLSV1_2;
	else
		c->mask_ssl = 0;
	/* Now go through all signature algorithms seeing if we support
	 * any for RSA, DSA, ECDSA. Do this for all versions not just
	 * TLS 1.2.
	 */
	sigalgslen = tls12_get_psigalgs(s, &sigalgs);
	for (i = 0; i < sigalgslen; i += 2, sigalgs += 2)
		{
		switch(sigalgs[1])
			{
#ifndef OPENSSL_NO_RSA
		case TLSEXT_signature_rsa:
			have_rsa = 1;
			break;
#endif
#ifndef OPENSSL_NO_DSA
		case TLSEXT_signature_dsa:
			have_dsa = 1;
			break;
#endif
#ifndef OPENSSL_NO_ECDSA
		case TLSEXT_signature_ecdsa:
			have_ecdsa = 1;
			break;
#endif
			}
		}
	/* Disable auth and static DH if we don't include any appropriate
	 * signature algorithms.
	 */
	if (!have_rsa)
		{
		c->mask_a |= SSL_aRSA;
		c->mask_k |= SSL_kDHr|SSL_kECDHr;
		}
	if (!have_dsa)
		{
		c->mask_a |= SSL_aDSS;
		c->mask_k |= SSL_kDHd;
		}
	if (!have_ecdsa)
		{
		c->mask_a |= SSL_aECDSA;
		c->mask_k |= SSL_kECDHe;
		}
#ifndef OPENSSL_NO_KRB5
	if (!kssl_tgt_is_available(s->kssl_ctx))
		{
		c->mask_a |= SSL_aKRB5;
		c->mask_k |= SSL_kKRB5;
		}
#endif
#ifndef OPENSSL_NO_PSK
	/* with PSK there must be client callback set */
	if (!s->psk_client_callback)
		{
		c->mask_a |= SSL_aPSK;
		c->mask_k |= SSL_kPSK;
		}
#endif /* OPENSSL_NO_PSK */
	c->valid = 1;
	}