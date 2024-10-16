static int sctp_verify_ext_param(struct net *net,
				 const struct sctp_endpoint *ep,
				 union sctp_params param)
{
	__u16 num_ext = ntohs(param.p->length) - sizeof(struct sctp_paramhdr);
	int have_asconf = 0;
	int have_auth = 0;
	int i;

	for (i = 0; i < num_ext; i++) {
		switch (param.ext->chunks[i]) {
		case SCTP_CID_AUTH:
			have_auth = 1;
			break;
		case SCTP_CID_ASCONF:
		case SCTP_CID_ASCONF_ACK:
			have_asconf = 1;
			break;
		}
	}

	/* ADD-IP Security: The draft requires us to ABORT or ignore the
	 * INIT/INIT-ACK if ADD-IP is listed, but AUTH is not.  Do this
	 * only if ADD-IP is turned on and we are not backward-compatible
	 * mode.
	 */
	if (net->sctp.addip_noauth)
		return 1;

	if (ep->asconf_enable && !have_auth && have_asconf)
		return 0;

	return 1;
}