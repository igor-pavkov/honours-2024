int cdk_pk_get_nbits(cdk_pubkey_t pk)
{
	if (!pk || !pk->mpi[0])
		return 0;
	return _gnutls_mpi_get_nbits(pk->mpi[0]);
}