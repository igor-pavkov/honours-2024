static gboolean avrcp_get_capabilities_resp(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_header *pdu = (void *) operands;
	uint16_t events = 0;
	uint8_t count;

	if (code == AVC_CTYPE_REJECTED || code == AVC_CTYPE_NOT_IMPLEMENTED ||
			pdu == NULL || pdu->params[0] != CAP_EVENTS_SUPPORTED)
		return FALSE;

	/* Connect browsing if pending */
	if (session->browsing_timer > 0) {
		timeout_remove(session->browsing_timer);
		session->browsing_timer = 0;
		avctp_connect_browsing(session->conn);
	}

	count = pdu->params[1];

	for (; count > 0; count--) {
		uint8_t event = pdu->params[1 + count];

		events |= (1 << event);

		switch (event) {
		case AVRCP_EVENT_STATUS_CHANGED:
		case AVRCP_EVENT_TRACK_CHANGED:
		case AVRCP_EVENT_PLAYBACK_POS_CHANGED:
		case AVRCP_EVENT_SETTINGS_CHANGED:
		case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
		case AVRCP_EVENT_UIDS_CHANGED:
		case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
			/* These events above requires a player */
			if (!session->controller ||
						!session->controller->player)
				break;
			/* fall through */
		case AVRCP_EVENT_VOLUME_CHANGED:
			avrcp_register_notification(session, event);
			break;
		}
	}

	if (!session->controller || !session->controller->player)
		return FALSE;

	/* Skip if player status/metadata if only volume changes is supported */
	if (events == (1 << AVRCP_EVENT_VOLUME_CHANGED))
		return FALSE;

	if ((session->controller->features & AVRCP_FEATURE_PLAYER_SETTINGS) &&
			!(events & (1 << AVRCP_EVENT_SETTINGS_CHANGED)))
		avrcp_list_player_attributes(session);

	if (!(events & (1 << AVRCP_EVENT_STATUS_CHANGED)))
		avrcp_get_play_status(session);

	if (!(events & (1 << AVRCP_EVENT_STATUS_CHANGED)))
		avrcp_get_element_attributes(session);

	return FALSE;
}