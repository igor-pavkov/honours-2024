void avrcp_player_event(struct avrcp_player *player, uint8_t id,
							const void *data)
{
	uint8_t buf[AVRCP_HEADER_LENGTH + 9];
	struct avrcp_header *pdu = (void *) buf;
	uint8_t code;
	uint16_t size;
	GSList *l;

	if (player->sessions == NULL)
		return;

	memset(buf, 0, sizeof(buf));

	set_company_id(pdu->company_id, IEEEID_BTSIG);

	pdu->pdu_id = AVRCP_REGISTER_NOTIFICATION;

	DBG("id=%u", id);

	if (id != AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED && player->changed_id) {
		code = AVC_CTYPE_REJECTED;
		size = 1;
		pdu->params[0] = AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED;
		goto done;
	}

	code = AVC_CTYPE_CHANGED;
	pdu->params[0] = id;

	switch (id) {
	case AVRCP_EVENT_STATUS_CHANGED:
		size = 2;
		pdu->params[1] = play_status_to_val(data);

		break;
	case AVRCP_EVENT_TRACK_CHANGED:
		size = 9;
		memcpy(&pdu->params[1], data, sizeof(uint64_t));

		break;
	case AVRCP_EVENT_TRACK_REACHED_END:
	case AVRCP_EVENT_TRACK_REACHED_START:
		size = 1;
		break;
	case AVRCP_EVENT_SETTINGS_CHANGED:
		size = player_settings_changed(player, pdu);
		break;
	case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
		size = 5;
		memcpy(&pdu->params[1], &player->id, sizeof(uint16_t));
		memcpy(&pdu->params[3], &player->uid_counter, sizeof(uint16_t));
		break;
	case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
		size = 1;
		break;
	default:
		error("Unknown event %u", id);
		return;
	}

done:
	pdu->params_len = htons(size);

	for (l = player->sessions; l; l = l->next) {
		struct avrcp *session = l->data;
		int err;

		if (!(session->registered_events & (1 << id)))
			continue;

		err = avctp_send_vendordep(session->conn,
					session->transaction_events[id],
					code, AVC_SUBUNIT_PANEL,
					buf, size + AVRCP_HEADER_LENGTH);

		if (err < 0)
			continue;

		/* Unregister event as per AVRCP 1.3 spec, section 5.4.2 */
		session->registered_events ^= 1 << id;
	}

	return;
}