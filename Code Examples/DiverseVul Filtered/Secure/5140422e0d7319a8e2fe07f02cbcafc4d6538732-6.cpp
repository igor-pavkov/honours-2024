int _cdk_pk_algo_usage(int algo)
{
	int usage;

	/* The ElGamal sign+encrypt algorithm is not supported any longer. */
	switch (algo) {
	case CDK_PK_RSA:
		usage = CDK_KEY_USG_SIGN | CDK_KEY_USG_ENCR;
		break;
	case CDK_PK_RSA_E:
		usage = CDK_KEY_USG_ENCR;
		break;
	case CDK_PK_RSA_S:
		usage = CDK_KEY_USG_SIGN;
		break;
	case CDK_PK_ELG_E:
		usage = CDK_KEY_USG_ENCR;
		break;
	case CDK_PK_DSA:
		usage = CDK_KEY_USG_SIGN;
		break;
	default:
		usage = 0;
	}
	return usage;
}