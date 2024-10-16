static struct sctp_chunk *sctp_make_reconf(const struct sctp_association *asoc,
					   int length)
{
	struct sctp_reconf_chunk *reconf;
	struct sctp_chunk *retval;

	retval = sctp_make_control(asoc, SCTP_CID_RECONF, 0, length,
				   GFP_ATOMIC);
	if (!retval)
		return NULL;

	reconf = (struct sctp_reconf_chunk *)retval->chunk_hdr;
	retval->param_hdr.v = reconf->params;

	return retval;
}