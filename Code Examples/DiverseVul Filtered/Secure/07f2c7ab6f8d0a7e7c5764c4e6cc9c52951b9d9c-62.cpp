static struct sctp_chunk *sctp_make_asconf(struct sctp_association *asoc,
					   union sctp_addr *addr,
					   int vparam_len)
{
	struct sctp_addiphdr asconf;
	struct sctp_chunk *retval;
	int length = sizeof(asconf) + vparam_len;
	union sctp_addr_param addrparam;
	int addrlen;
	struct sctp_af *af = sctp_get_af_specific(addr->v4.sin_family);

	addrlen = af->to_addr_param(addr, &addrparam);
	if (!addrlen)
		return NULL;
	length += addrlen;

	/* Create the chunk.  */
	retval = sctp_make_control(asoc, SCTP_CID_ASCONF, 0, length,
				   GFP_ATOMIC);
	if (!retval)
		return NULL;

	asconf.serial = htonl(asoc->addip_serial++);

	retval->subh.addip_hdr =
		sctp_addto_chunk(retval, sizeof(asconf), &asconf);
	retval->param_hdr.v =
		sctp_addto_chunk(retval, addrlen, &addrparam);

	return retval;
}