static uint8_t avrcp_handle_ct_battery_status(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	uint16_t len = ntohs(pdu->params_len);
	const char *valstr;

	if (len != 1)
		goto err;

	valstr = battery_status_to_str(pdu->params[0]);
	if (valstr == NULL)
		goto err;

	pdu->params_len = 0;

	return AVC_CTYPE_STABLE;

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}