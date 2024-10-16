static int sctp_process_missing_param(const struct sctp_association *asoc,
				      enum sctp_param paramtype,
				      struct sctp_chunk *chunk,
				      struct sctp_chunk **errp)
{
	struct __sctp_missing report;
	__u16 len;

	len = SCTP_PAD4(sizeof(report));

	/* Make an ERROR chunk, preparing enough room for
	 * returning multiple unknown parameters.
	 */
	if (!*errp)
		*errp = sctp_make_op_error_space(asoc, chunk, len);

	if (*errp) {
		report.num_missing = htonl(1);
		report.type = paramtype;
		sctp_init_cause(*errp, SCTP_ERROR_MISS_PARAM,
				sizeof(report));
		sctp_addto_chunk(*errp, sizeof(report), &report);
	}

	/* Stop processing this chunk. */
	return 0;
}