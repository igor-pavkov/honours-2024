int cdk_pk_get_nenc(int algo)
{
	if (is_RSA(algo))
		return 1;
	else if (is_ELG(algo))
		return 2;
	else
		return 0;
}