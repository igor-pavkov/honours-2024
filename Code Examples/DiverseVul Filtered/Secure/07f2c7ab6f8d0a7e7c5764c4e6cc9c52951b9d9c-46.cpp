struct sctp_chunk *sctp_make_asconf_set_prim(struct sctp_association *asoc,
					     union sctp_addr *addr)
{
	struct sctp_af *af = sctp_get_af_specific(addr->v4.sin_family);
	union sctp_addr_param addrparam;
	struct sctp_addip_param	param;
	struct sctp_chunk *retval;
	int len = sizeof(param);
	int addrlen;

	addrlen = af->to_addr_param(addr, &addrparam);
	if (!addrlen)
		return NULL;
	len += addrlen;

	/* Create the chunk and make asconf header. */
	retval = sctp_make_asconf(asoc, addr, len);
	if (!retval)
		return NULL;

	param.param_hdr.type = SCTP_PARAM_SET_PRIMARY;
	param.param_hdr.length = htons(len);
	param.crr_id = 0;

	sctp_addto_chunk(retval, sizeof(param), &param);
	sctp_addto_chunk(retval, addrlen, &addrparam);

	return retval;
}