int sctp_process_asconf_ack(struct sctp_association *asoc,
			    struct sctp_chunk *asconf_ack)
{
	struct sctp_chunk *asconf = asoc->addip_last_asconf;
	struct sctp_addip_param *asconf_param;
	__be16 err_code = SCTP_ERROR_NO_ERROR;
	union sctp_addr_param *addr_param;
	int asconf_len = asconf->skb->len;
	int all_param_pass = 0;
	int length = 0;
	int no_err = 1;
	int retval = 0;

	/* Skip the chunkhdr and addiphdr from the last asconf sent and store
	 * a pointer to address parameter.
	 */
	length = sizeof(struct sctp_addip_chunk);
	addr_param = (union sctp_addr_param *)(asconf->skb->data + length);
	asconf_len -= length;

	/* Skip the address parameter in the last asconf sent and store a
	 * pointer to the first asconf parameter.
	 */
	length = ntohs(addr_param->p.length);
	asconf_param = (void *)addr_param + length;
	asconf_len -= length;

	/* ADDIP 4.1
	 * A8) If there is no response(s) to specific TLV parameter(s), and no
	 * failures are indicated, then all request(s) are considered
	 * successful.
	 */
	if (asconf_ack->skb->len == sizeof(struct sctp_addiphdr))
		all_param_pass = 1;

	/* Process the TLVs contained in the last sent ASCONF chunk. */
	while (asconf_len > 0) {
		if (all_param_pass)
			err_code = SCTP_ERROR_NO_ERROR;
		else {
			err_code = sctp_get_asconf_response(asconf_ack,
							    asconf_param,
							    no_err);
			if (no_err && (SCTP_ERROR_NO_ERROR != err_code))
				no_err = 0;
		}

		switch (err_code) {
		case SCTP_ERROR_NO_ERROR:
			sctp_asconf_param_success(asoc, asconf_param);
			break;

		case SCTP_ERROR_RSRC_LOW:
			retval = 1;
			break;

		case SCTP_ERROR_UNKNOWN_PARAM:
			/* Disable sending this type of asconf parameter in
			 * future.
			 */
			asoc->peer.addip_disabled_mask |=
				asconf_param->param_hdr.type;
			break;

		case SCTP_ERROR_REQ_REFUSED:
		case SCTP_ERROR_DEL_LAST_IP:
		case SCTP_ERROR_DEL_SRC_IP:
		default:
			 break;
		}

		/* Skip the processed asconf parameter and move to the next
		 * one.
		 */
		length = ntohs(asconf_param->param_hdr.length);
		asconf_param = (void *)asconf_param + length;
		asconf_len -= length;
	}

	if (no_err && asoc->src_out_of_asoc_ok) {
		asoc->src_out_of_asoc_ok = 0;
		sctp_transport_immediate_rtx(asoc->peer.primary_path);
	}

	/* Free the cached last sent asconf chunk. */
	list_del_init(&asconf->transmitted_list);
	sctp_chunk_free(asconf);
	asoc->addip_last_asconf = NULL;

	return retval;
}