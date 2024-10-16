static inline struct sctp_chunk *sctp_make_op_error_limited(
					const struct sctp_association *asoc,
					const struct sctp_chunk *chunk)
{
	size_t size = SCTP_DEFAULT_MAXSEGMENT;
	struct sctp_sock *sp = NULL;

	if (asoc) {
		size = min_t(size_t, size, asoc->pathmtu);
		sp = sctp_sk(asoc->base.sk);
	}

	size = sctp_mtu_payload(sp, size, sizeof(struct sctp_errhdr));

	return sctp_make_op_error_space(asoc, chunk, size);
}