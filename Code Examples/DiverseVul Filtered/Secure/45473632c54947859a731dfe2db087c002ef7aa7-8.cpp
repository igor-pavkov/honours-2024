int tls1_shared_curve(SSL *s, int nmatch)
	{
	const unsigned char *pref, *supp;
	size_t preflen, supplen, i, j;
	int k;
	/* Can't do anything on client side */
	if (s->server == 0)
		return -1;
	if (nmatch == -2)
		{
		if (tls1_suiteb(s))
			{
			/* For Suite B ciphersuite determines curve: we 
			 * already know these are acceptable due to previous
			 * checks.
			 */
			unsigned long cid = s->s3->tmp.new_cipher->id;
			if (cid == TLS1_CK_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256)
				return NID_X9_62_prime256v1; /* P-256 */
			if (cid == TLS1_CK_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384)
				return NID_secp384r1; /* P-384 */
			/* Should never happen */
			return NID_undef;
			}
		/* If not Suite B just return first preference shared curve */
		nmatch = 0;
		}
	tls1_get_curvelist(s, !!(s->options & SSL_OP_CIPHER_SERVER_PREFERENCE),
				&supp, &supplen);
	tls1_get_curvelist(s, !(s->options & SSL_OP_CIPHER_SERVER_PREFERENCE),
				&pref, &preflen);
	preflen /= 2;
	supplen /= 2;
	k = 0;
	for (i = 0; i < preflen; i++, pref+=2)
		{
		const unsigned char *tsupp = supp;
		for (j = 0; j < supplen; j++, tsupp+=2)
			{
			if (pref[0] == tsupp[0] && pref[1] == tsupp[1])
				{
				if (nmatch == k)
					{
					int id = (pref[0] << 8) | pref[1];
					return tls1_ec_curve_id2nid(id);
					}
				k++;
				}
			}
		}
	if (nmatch == -1)
		return k;
	return 0;
	}