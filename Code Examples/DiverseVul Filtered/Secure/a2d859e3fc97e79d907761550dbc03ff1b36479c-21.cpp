static int sctp_process_hn_param(const struct sctp_association *asoc,
				 union sctp_params param,
				 struct sctp_chunk *chunk,
				 struct sctp_chunk **errp)
{
	__u16 len = ntohs(param.p->length);

	/* Processing of the HOST_NAME parameter will generate an
	 * ABORT.  If we've accumulated any non-fatal errors, they
	 * would be unrecognized parameters and we should not include
	 * them in the ABORT.
	 */
	if (*errp)
		sctp_chunk_free(*errp);

	*errp = sctp_make_op_error(asoc, chunk, SCTP_ERROR_DNS_FAILED,
				   param.v, len, 0);

	/* Stop processing this chunk. */
	return 0;
}