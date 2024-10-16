static uint8_t avrcp_handle_displayable_charset(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	uint16_t len = ntohs(pdu->params_len);

	if (len < 3) {
		pdu->params_len = htons(1);
		pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
		return AVC_CTYPE_REJECTED;
	}

	/*
	 * We acknowledge the commands, but we always use UTF-8 for
	 * encoding since CT is obliged to support it.
	 */
	pdu->params_len = 0;
	return AVC_CTYPE_STABLE;
}