int _cdk_sk_get_csum(cdk_pkt_seckey_t sk)
{
	u16 csum = 0, i;

	if (!sk)
		return 0;
	for (i = 0; i < cdk_pk_get_nskey(sk->pubkey_algo); i++)
		csum += checksum_mpi(sk->mpi[i]);
	return csum;
}