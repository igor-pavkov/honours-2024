int cdk_pk_get_nsig(int algo)
{
	if (is_RSA(algo))
		return 1;
	else if (is_DSA(algo))
		return 2;
	else
		return 0;
}