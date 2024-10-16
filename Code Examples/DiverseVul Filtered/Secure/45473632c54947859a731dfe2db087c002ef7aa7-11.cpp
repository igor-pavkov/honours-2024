int tls1_check_curve(SSL *s, const unsigned char *p, size_t len)
	{
	const unsigned char *curves;
	size_t curveslen, i;
	unsigned int suiteb_flags = tls1_suiteb(s);
	if (len != 3 || p[0] != NAMED_CURVE_TYPE)
		return 0;
	/* Check curve matches Suite B preferences */
	if (suiteb_flags)
		{
		unsigned long cid = s->s3->tmp.new_cipher->id;
		if (p[1])
			return 0;
		if (cid == TLS1_CK_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256)
			{
			if (p[2] != TLSEXT_curve_P_256)
				return 0;
			}
		else if (cid == TLS1_CK_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384)
			{
			if (p[2] != TLSEXT_curve_P_384)
				return 0;
			}
		else	/* Should never happen */
			return 0;
		}
	tls1_get_curvelist(s, 0, &curves, &curveslen);
	for (i = 0; i < curveslen; i += 2, curves += 2)
		{
		if (p[1] == curves[0] && p[2] == curves[1])
			return 1;
		}
	return 0;
	}