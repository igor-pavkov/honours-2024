static cdk_error_t skip_packet(cdk_stream_t inp, size_t pktlen)
{
	byte buf[BUFSIZE];
	size_t nread, buflen = DIM(buf);

	while (pktlen > 0) {
		cdk_error_t rc;
		rc = stream_read(inp, buf, pktlen > buflen ? buflen : pktlen,
			    &nread);
		if (rc)
			return rc;
		pktlen -= nread;
	}

	assert(pktlen == 0);
	return 0;
}