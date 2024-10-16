static gboolean avrcp_handle_event(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_header *pdu = (void *) operands;
	uint8_t event;

	if (!pdu)
		return FALSE;

	if ((code != AVC_CTYPE_INTERIM && code != AVC_CTYPE_CHANGED)) {
		if (pdu->params[0] == AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED &&
				code == AVC_CTYPE_REJECTED) {
			int i;

			/* Lookup event by transaction */
			for (i = 0; i <= AVRCP_EVENT_LAST; i++) {
				if (session->transaction_events[i] ==
								transaction) {
					event = i;
					goto changed;
				}
			}
		}
		return FALSE;
	}

	event = pdu->params[0];

	if (code == AVC_CTYPE_CHANGED) {
		goto changed;
	}

	switch (event) {
	case AVRCP_EVENT_VOLUME_CHANGED:
		avrcp_volume_changed(session, pdu);
		break;
	case AVRCP_EVENT_STATUS_CHANGED:
		avrcp_status_changed(session, pdu);
		break;
	case AVRCP_EVENT_TRACK_CHANGED:
		avrcp_track_changed(session, pdu);
		break;
	case AVRCP_EVENT_PLAYBACK_POS_CHANGED:
		avrcp_playback_pos_changed(session, pdu);
		break;
	case AVRCP_EVENT_SETTINGS_CHANGED:
		avrcp_setting_changed(session, pdu);
		break;
	case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
		avrcp_available_players_changed(session, pdu);
		break;
	case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
		avrcp_addressed_player_changed(session, pdu);
		break;
	case AVRCP_EVENT_UIDS_CHANGED:
		avrcp_uids_changed(session, pdu);
		break;
	}

	session->registered_events |= (1 << event);
	session->transaction_events[event] = transaction;

	return TRUE;

changed:
	session->registered_events ^= (1 << event);
	session->transaction_events[event] = 0;
	avrcp_register_notification(session, event);
	return FALSE;
}