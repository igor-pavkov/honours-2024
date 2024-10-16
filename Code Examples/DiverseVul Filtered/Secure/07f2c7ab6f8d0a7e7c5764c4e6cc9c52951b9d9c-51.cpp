struct sctp_chunk *sctp_make_strreset_tsnresp(struct sctp_association *asoc,
					      __u32 result, __u32 sn,
					      __u32 sender_tsn,
					      __u32 receiver_tsn)
{
	struct sctp_strreset_resptsn tsnresp;
	__u16 length = sizeof(tsnresp);
	struct sctp_chunk *retval;

	retval = sctp_make_reconf(asoc, length);
	if (!retval)
		return NULL;

	tsnresp.param_hdr.type = SCTP_PARAM_RESET_RESPONSE;
	tsnresp.param_hdr.length = htons(length);

	tsnresp.response_seq = htonl(sn);
	tsnresp.result = htonl(result);
	tsnresp.senders_next_tsn = htonl(sender_tsn);
	tsnresp.receivers_next_tsn = htonl(receiver_tsn);

	sctp_addto_chunk(retval, sizeof(tsnresp), &tsnresp);

	return retval;
}