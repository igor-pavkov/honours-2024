struct sctp_chunk *sctp_make_violation_paramlen(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk,
					struct sctp_paramhdr *param)
{
	static const char error[] = "The following parameter had invalid length:";
	size_t payload_len = sizeof(error) + sizeof(struct sctp_errhdr) +
			     sizeof(*param);
	struct sctp_chunk *retval;

	retval = sctp_make_abort(asoc, chunk, payload_len);
	if (!retval)
		goto nodata;

	sctp_init_cause(retval, SCTP_ERROR_PROTO_VIOLATION,
			sizeof(error) + sizeof(*param));
	sctp_addto_chunk(retval, sizeof(error), error);
	sctp_addto_param(retval, sizeof(*param), param);

nodata:
	return retval;
}