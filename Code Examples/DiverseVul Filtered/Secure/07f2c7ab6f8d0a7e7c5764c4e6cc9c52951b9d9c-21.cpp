struct sctp_chunk *sctp_make_strreset_resp(const struct sctp_association *asoc,
					   __u32 result, __u32 sn)
{
	struct sctp_strreset_resp resp;
	__u16 length = sizeof(resp);
	struct sctp_chunk *retval;

	retval = sctp_make_reconf(asoc, length);
	if (!retval)
		return NULL;

	resp.param_hdr.type = SCTP_PARAM_RESET_RESPONSE;
	resp.param_hdr.length = htons(length);
	resp.response_seq = htonl(sn);
	resp.result = htonl(result);

	sctp_addto_chunk(retval, sizeof(resp), &resp);

	return retval;
}