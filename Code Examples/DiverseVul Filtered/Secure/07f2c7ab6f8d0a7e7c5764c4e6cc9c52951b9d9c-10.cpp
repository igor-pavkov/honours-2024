struct sctp_chunk *sctp_make_abort_violation(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk,
					const __u8 *payload,
					const size_t paylen)
{
	struct sctp_chunk  *retval;
	struct sctp_paramhdr phdr;

	retval = sctp_make_abort(asoc, chunk, sizeof(struct sctp_errhdr) +
					      paylen + sizeof(phdr));
	if (!retval)
		goto end;

	sctp_init_cause(retval, SCTP_ERROR_PROTO_VIOLATION, paylen +
							    sizeof(phdr));

	phdr.type = htons(chunk->chunk_hdr->type);
	phdr.length = chunk->chunk_hdr->length;
	sctp_addto_chunk(retval, paylen, payload);
	sctp_addto_param(retval, sizeof(phdr), &phdr);

end:
	return retval;
}