struct sctp_chunk *sctp_make_asconf_update_ip(struct sctp_association *asoc,
					      union sctp_addr *laddr,
					      struct sockaddr *addrs,
					      int addrcnt, __be16 flags)
{
	union sctp_addr_param addr_param;
	struct sctp_addip_param	param;
	int paramlen = sizeof(param);
	struct sctp_chunk *retval;
	int addr_param_len = 0;
	union sctp_addr *addr;
	int totallen = 0, i;
	int del_pickup = 0;
	struct sctp_af *af;
	void *addr_buf;

	/* Get total length of all the address parameters. */
	addr_buf = addrs;
	for (i = 0; i < addrcnt; i++) {
		addr = addr_buf;
		af = sctp_get_af_specific(addr->v4.sin_family);
		addr_param_len = af->to_addr_param(addr, &addr_param);

		totallen += paramlen;
		totallen += addr_param_len;

		addr_buf += af->sockaddr_len;
		if (asoc->asconf_addr_del_pending && !del_pickup) {
			/* reuse the parameter length from the same scope one */
			totallen += paramlen;
			totallen += addr_param_len;
			del_pickup = 1;

			pr_debug("%s: picked same-scope del_pending addr, "
				 "totallen for all addresses is %d\n",
				 __func__, totallen);
		}
	}

	/* Create an asconf chunk with the required length. */
	retval = sctp_make_asconf(asoc, laddr, totallen);
	if (!retval)
		return NULL;

	/* Add the address parameters to the asconf chunk. */
	addr_buf = addrs;
	for (i = 0; i < addrcnt; i++) {
		addr = addr_buf;
		af = sctp_get_af_specific(addr->v4.sin_family);
		addr_param_len = af->to_addr_param(addr, &addr_param);
		param.param_hdr.type = flags;
		param.param_hdr.length = htons(paramlen + addr_param_len);
		param.crr_id = htonl(i);

		sctp_addto_chunk(retval, paramlen, &param);
		sctp_addto_chunk(retval, addr_param_len, &addr_param);

		addr_buf += af->sockaddr_len;
	}
	if (flags == SCTP_PARAM_ADD_IP && del_pickup) {
		addr = asoc->asconf_addr_del_pending;
		af = sctp_get_af_specific(addr->v4.sin_family);
		addr_param_len = af->to_addr_param(addr, &addr_param);
		param.param_hdr.type = SCTP_PARAM_DEL_IP;
		param.param_hdr.length = htons(paramlen + addr_param_len);
		param.crr_id = htonl(i);

		sctp_addto_chunk(retval, paramlen, &param);
		sctp_addto_chunk(retval, addr_param_len, &addr_param);
	}
	return retval;
}