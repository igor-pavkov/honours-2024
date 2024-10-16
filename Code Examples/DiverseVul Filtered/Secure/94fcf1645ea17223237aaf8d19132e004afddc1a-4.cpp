static cdk_error_t read_mdc(cdk_stream_t inp, cdk_pkt_mdc_t mdc)
{
	size_t n;
	cdk_error_t rc;

	if (!inp || !mdc)
		return CDK_Inv_Value;

	if (DEBUG_PKT)
		_gnutls_write_log("read_mdc:\n");

	rc = stream_read(inp, mdc->hash, DIM(mdc->hash), &n);
	if (rc)
		return rc;

	return n != DIM(mdc->hash) ? CDK_Inv_Packet : 0;
}