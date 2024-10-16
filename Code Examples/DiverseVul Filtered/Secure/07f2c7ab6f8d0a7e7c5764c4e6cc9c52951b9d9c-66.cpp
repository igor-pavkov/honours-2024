struct sctp_chunk *sctp_make_ifwdtsn(const struct sctp_association *asoc,
				     __u32 new_cum_tsn, size_t nstreams,
				     struct sctp_ifwdtsn_skip *skiplist)
{
	struct sctp_chunk *retval = NULL;
	struct sctp_ifwdtsn_hdr ftsn_hdr;
	size_t hint;

	hint = (nstreams + 1) * sizeof(__u32);

	retval = sctp_make_control(asoc, SCTP_CID_I_FWD_TSN, 0, hint,
				   GFP_ATOMIC);
	if (!retval)
		return NULL;

	ftsn_hdr.new_cum_tsn = htonl(new_cum_tsn);
	retval->subh.ifwdtsn_hdr =
		sctp_addto_chunk(retval, sizeof(ftsn_hdr), &ftsn_hdr);

	sctp_addto_chunk(retval, nstreams * sizeof(skiplist[0]), skiplist);

	return retval;
}