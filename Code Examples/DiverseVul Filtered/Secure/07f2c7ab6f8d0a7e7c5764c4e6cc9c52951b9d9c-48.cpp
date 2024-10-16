struct sctp_chunk *sctp_make_strreset_addstrm(
					const struct sctp_association *asoc,
					__u16 out, __u16 in)
{
	struct sctp_strreset_addstrm addstrm;
	__u16 size = sizeof(addstrm);
	struct sctp_chunk *retval;

	retval = sctp_make_reconf(asoc, (!!out + !!in) * size);
	if (!retval)
		return NULL;

	if (out) {
		addstrm.param_hdr.type = SCTP_PARAM_RESET_ADD_OUT_STREAMS;
		addstrm.param_hdr.length = htons(size);
		addstrm.number_of_streams = htons(out);
		addstrm.request_seq = htonl(asoc->strreset_outseq);
		addstrm.reserved = 0;

		sctp_addto_chunk(retval, size, &addstrm);
	}

	if (in) {
		addstrm.param_hdr.type = SCTP_PARAM_RESET_ADD_IN_STREAMS;
		addstrm.param_hdr.length = htons(size);
		addstrm.number_of_streams = htons(in);
		addstrm.request_seq = htonl(asoc->strreset_outseq + !!out);
		addstrm.reserved = 0;

		sctp_addto_chunk(retval, size, &addstrm);
	}

	return retval;
}