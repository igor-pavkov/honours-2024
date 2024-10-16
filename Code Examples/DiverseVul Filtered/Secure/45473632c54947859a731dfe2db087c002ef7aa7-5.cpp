int tls12_check_peer_sigalg(const EVP_MD **pmd, SSL *s,
				const unsigned char *sig, EVP_PKEY *pkey)
	{
	const unsigned char *sent_sigs;
	size_t sent_sigslen, i;
	int sigalg = tls12_get_sigid(pkey);
	/* Should never happen */
	if (sigalg == -1)
		return -1;
	/* Check key type is consistent with signature */
	if (sigalg != (int)sig[1])
		{
		SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,SSL_R_WRONG_SIGNATURE_TYPE);
		return 0;
		}
#ifndef OPENSSL_NO_EC
	if (pkey->type == EVP_PKEY_EC)
		{
		unsigned char curve_id[2], comp_id;
		/* Check compression and curve matches extensions */
		if (!tls1_set_ec_id(curve_id, &comp_id, pkey->pkey.ec))
			return 0;
		if (!s->server && !tls1_check_ec_key(s, curve_id, &comp_id))
			{
			SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,SSL_R_WRONG_CURVE);
			return 0;
			}
		/* If Suite B only P-384+SHA384 or P-256+SHA-256 allowed */
		if (tls1_suiteb(s))
			{
			if (curve_id[0])
				return 0;
			if (curve_id[1] == TLSEXT_curve_P_256)
				{
				if (sig[0] != TLSEXT_hash_sha256)
					{
					SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,
						SSL_R_ILLEGAL_SUITEB_DIGEST);
					return 0;
					}
				}
			else if (curve_id[1] == TLSEXT_curve_P_384)
				{
				if (sig[0] != TLSEXT_hash_sha384)
					{
					SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,
						SSL_R_ILLEGAL_SUITEB_DIGEST);
					return 0;
					}
				}
			else
				return 0;
			}
		}
	else if (tls1_suiteb(s))
		return 0;
#endif

	/* Check signature matches a type we sent */
	sent_sigslen = tls12_get_psigalgs(s, &sent_sigs);
	for (i = 0; i < sent_sigslen; i+=2, sent_sigs+=2)
		{
		if (sig[0] == sent_sigs[0] && sig[1] == sent_sigs[1])
			break;
		}
	/* Allow fallback to SHA1 if not strict mode */
	if (i == sent_sigslen && (sig[0] != TLSEXT_hash_sha1 || s->cert->cert_flags & SSL_CERT_FLAGS_CHECK_TLS_STRICT))
		{
		SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,SSL_R_WRONG_SIGNATURE_TYPE);
		return 0;
		}
	*pmd = tls12_get_hash(sig[0]);
	if (*pmd == NULL)
		{
		SSLerr(SSL_F_TLS12_CHECK_PEER_SIGALG,SSL_R_UNKNOWN_DIGEST);
		return 0;
		}
	/* Store the digest used so applications can retrieve it if they
	 * wish.
	 */
	if (s->session && s->session->sess_cert)
		s->session->sess_cert->peer_key->digest = *pmd;
	return 1;
	}