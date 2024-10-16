static uint8_t avrcp_handle_list_player_values(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	uint16_t len = ntohs(pdu->params_len);
	unsigned int i;

	if (len != 1)
		goto err;

	if (player_get_setting(player, pdu->params[0]) < 0)
		goto err;

	len = attr_get_max_val(pdu->params[0]);

	for (i = 1; i <= len; i++)
		pdu->params[i] = i;

	pdu->params[0] = len;
	pdu->params_len = htons(len + 1);

	return AVC_CTYPE_STABLE;

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}