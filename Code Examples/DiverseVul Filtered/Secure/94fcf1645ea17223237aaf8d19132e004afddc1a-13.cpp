static cdk_error_t read_mpi(cdk_stream_t inp, bigint_t * ret_m, int secure)
{
	bigint_t m;
	int err;
	byte buf[MAX_MPI_BYTES + 2];
	size_t nread, nbits;
	cdk_error_t rc;

	if (!inp || !ret_m)
		return CDK_Inv_Value;

	*ret_m = NULL;
	nbits = read_16(inp);
	nread = (nbits + 7) / 8;

	if (nbits > MAX_MPI_BITS || nbits == 0) {
		_gnutls_write_log("read_mpi: too large %d bits\n",
				  (int) nbits);
		return gnutls_assert_val(CDK_MPI_Error);	/* Sanity check */
	}

	rc = stream_read(inp, buf + 2, nread, &nread);
	if (!rc && nread != ((nbits + 7) / 8)) {
		_gnutls_write_log("read_mpi: too short %d < %d\n",
				  (int) nread, (int) ((nbits + 7) / 8));
		return gnutls_assert_val(CDK_MPI_Error);
	}

	buf[0] = nbits >> 8;
	buf[1] = nbits >> 0;
	nread += 2;
	err = _gnutls_mpi_init_scan_pgp(&m, buf, nread);
	if (err < 0)
		return gnutls_assert_val(map_gnutls_error(err));

	*ret_m = m;
	return rc;
}