struct sctp_chunk *sctp_make_strreset_tsnreq(
					const struct sctp_association *asoc)
{
	struct sctp_strreset_tsnreq tsnreq;
	__u16 length = sizeof(tsnreq);
	struct sctp_chunk *retval;

	retval = sctp_make_reconf(asoc, length);
	if (!retval)
		return NULL;

	tsnreq.param_hdr.type = SCTP_PARAM_RESET_TSN_REQUEST;
	tsnreq.param_hdr.length = htons(length);
	tsnreq.request_seq = htonl(asoc->strreset_outseq);

	sctp_addto_chunk(retval, sizeof(tsnreq), &tsnreq);

	return retval;
}