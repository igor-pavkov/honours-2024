int cdk_pk_get_nskey(int algo)
{
	int ret;

	if (is_RSA(algo))
		ret = RSA_PRIVATE_PARAMS - 2;	/* we don't have exp1 and exp2 */
	else if (is_DSA(algo))
		ret = DSA_PRIVATE_PARAMS;
	else if (is_ELG(algo))
		ret = 4;
	else {
		gnutls_assert();
		return 0;
	}

	ret -= cdk_pk_get_npkey(algo);
	return ret;
}