int cdk_pk_get_npkey(int algo)
{
	if (is_RSA(algo))
		return RSA_PUBLIC_PARAMS;
	else if (is_DSA(algo))
		return DSA_PUBLIC_PARAMS;
	else if (is_ELG(algo))
		return 3;
	else {
		gnutls_assert();
		return 0;
	}
}