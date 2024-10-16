static u16 checksum_mpi(bigint_t m)
{
	byte buf[MAX_MPI_BYTES + 2];
	size_t nread;
	unsigned int i;
	u16 chksum = 0;

	if (!m)
		return 0;
	nread = DIM(buf);
	if (_gnutls_mpi_print_pgp(m, buf, &nread) < 0)
		return 0;
	for (i = 0; i < nread; i++)
		chksum += buf[i];
	return chksum;
}