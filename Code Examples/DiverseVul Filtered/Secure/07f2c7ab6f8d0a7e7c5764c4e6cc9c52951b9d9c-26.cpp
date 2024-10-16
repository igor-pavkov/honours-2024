static __be16 sctp_get_asconf_response(struct sctp_chunk *asconf_ack,
				       struct sctp_addip_param *asconf_param,
				       int no_err)
{
	struct sctp_addip_param	*asconf_ack_param;
	struct sctp_errhdr *err_param;
	int asconf_ack_len;
	__be16 err_code;
	int length;

	if (no_err)
		err_code = SCTP_ERROR_NO_ERROR;
	else
		err_code = SCTP_ERROR_REQ_REFUSED;

	asconf_ack_len = ntohs(asconf_ack->chunk_hdr->length) -
			 sizeof(struct sctp_chunkhdr);

	/* Skip the addiphdr from the asconf_ack chunk and store a pointer to
	 * the first asconf_ack parameter.
	 */
	length = sizeof(struct sctp_addiphdr);
	asconf_ack_param = (struct sctp_addip_param *)(asconf_ack->skb->data +
						       length);
	asconf_ack_len -= length;

	while (asconf_ack_len > 0) {
		if (asconf_ack_param->crr_id == asconf_param->crr_id) {
			switch (asconf_ack_param->param_hdr.type) {
			case SCTP_PARAM_SUCCESS_REPORT:
				return SCTP_ERROR_NO_ERROR;
			case SCTP_PARAM_ERR_CAUSE:
				length = sizeof(*asconf_ack_param);
				err_param = (void *)asconf_ack_param + length;
				asconf_ack_len -= length;
				if (asconf_ack_len > 0)
					return err_param->cause;
				else
					return SCTP_ERROR_INV_PARAM;
				break;
			default:
				return SCTP_ERROR_INV_PARAM;
			}
		}

		length = ntohs(asconf_ack_param->param_hdr.length);
		asconf_ack_param = (void *)asconf_ack_param + length;
		asconf_ack_len -= length;
	}

	return err_code;
}