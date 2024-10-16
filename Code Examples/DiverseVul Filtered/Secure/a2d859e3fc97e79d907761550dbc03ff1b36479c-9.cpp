static void sctp_process_ext_param(struct sctp_association *asoc,
				   union sctp_params param)
{
	__u16 num_ext = ntohs(param.p->length) - sizeof(struct sctp_paramhdr);
	int i;

	for (i = 0; i < num_ext; i++) {
		switch (param.ext->chunks[i]) {
		case SCTP_CID_RECONF:
			if (asoc->ep->reconf_enable)
				asoc->peer.reconf_capable = 1;
			break;
		case SCTP_CID_FWD_TSN:
			if (asoc->ep->prsctp_enable)
				asoc->peer.prsctp_capable = 1;
			break;
		case SCTP_CID_AUTH:
			/* if the peer reports AUTH, assume that he
			 * supports AUTH.
			 */
			if (asoc->ep->auth_enable)
				asoc->peer.auth_capable = 1;
			break;
		case SCTP_CID_ASCONF:
		case SCTP_CID_ASCONF_ACK:
			if (asoc->ep->asconf_enable)
				asoc->peer.asconf_capable = 1;
			break;
		case SCTP_CID_I_DATA:
			if (asoc->ep->intl_enable)
				asoc->peer.intl_capable = 1;
			break;
		default:
			break;
		}
	}
}