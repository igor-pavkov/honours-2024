static uint8_t avrcp_handle_get_play_status(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	uint16_t len = ntohs(pdu->params_len);
	uint32_t position;
	uint32_t duration;

	if (len != 0) {
		pdu->params_len = htons(1);
		pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
		return AVC_CTYPE_REJECTED;
	}

	position = player_get_position(player);
	duration = player_get_duration(player);

	position = htonl(position);
	duration = htonl(duration);

	memcpy(&pdu->params[0], &duration, 4);
	memcpy(&pdu->params[4], &position, 4);
	pdu->params[8] = player_get_status(player);

	pdu->params_len = htons(9);

	return AVC_CTYPE_STABLE;
}