static uint8_t avrcp_handle_set_addressed_player(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player;
	uint16_t len = ntohs(pdu->params_len);
	uint16_t player_id = 0;
	uint8_t status;

	if (len < 1) {
		status = AVRCP_STATUS_INVALID_PARAM;
		goto err;
	}

	player_id = bt_get_be16(&pdu->params[0]);
	player = find_tg_player(session, player_id);
	pdu->packet_type = AVRCP_PACKET_TYPE_SINGLE;

	if (player) {
		player->addressed = true;
		status = AVRCP_STATUS_SUCCESS;
		pdu->params_len = htons(len);
		pdu->params[0] = status;
	} else {
		status = AVRCP_STATUS_INVALID_PLAYER_ID;
		goto err;
	}

	/* Don't emit player changed immediately since PTS expect the
	 * response of SetAddressedPlayer before the event.
	 */
	player->changed_id = g_idle_add(notify_addressed_player_changed,
								player);

	return AVC_CTYPE_ACCEPTED;

err:
	pdu->params_len = htons(sizeof(status));
	pdu->params[0] = status;
	return AVC_CTYPE_REJECTED;
}