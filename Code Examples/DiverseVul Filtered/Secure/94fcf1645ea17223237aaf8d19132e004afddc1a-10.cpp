static cdk_error_t read_s2k(cdk_stream_t inp, cdk_s2k_t s2k)
{
	size_t nread;

	s2k->mode = cdk_stream_getc(inp);
	s2k->hash_algo = cdk_stream_getc(inp);
	if (s2k->mode == CDK_S2K_SIMPLE)
		return 0;
	else if (s2k->mode == CDK_S2K_SALTED
		 || s2k->mode == CDK_S2K_ITERSALTED) {
		if (stream_read(inp, s2k->salt, DIM(s2k->salt), &nread))
			return CDK_Inv_Packet;
		if (nread != DIM(s2k->salt))
			return CDK_Inv_Packet;

		if (s2k->mode == CDK_S2K_ITERSALTED)
			s2k->count = cdk_stream_getc(inp);
	} else if (s2k->mode == CDK_S2K_GNU_EXT) {
		/* GNU extensions to the S2K : read DETAILS from gnupg */
		return 0;
	} else
		return CDK_Not_Implemented;

	return 0;
}