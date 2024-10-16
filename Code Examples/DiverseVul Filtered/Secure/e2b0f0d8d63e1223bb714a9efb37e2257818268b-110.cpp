static uint8_t avrcp_handle_register_notification(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	struct btd_device *dev = session->dev;
	uint16_t len = ntohs(pdu->params_len);
	uint64_t uid;
	int8_t volume;

	/*
	 * 1 byte for EventID, 4 bytes for Playback interval but the latest
	 * one is applicable only for EVENT_PLAYBACK_POS_CHANGED. See AVRCP
	 * 1.3 spec, section 5.4.2.
	 */
	if (len != 5)
		goto err;

	/* Check if event is supported otherwise reject */
	if (!(session->supported_events & (1 << pdu->params[0])))
		goto err;

	switch (pdu->params[0]) {
	case AVRCP_EVENT_STATUS_CHANGED:
		len = 2;
		pdu->params[1] = player_get_status(player);

		break;
	case AVRCP_EVENT_TRACK_CHANGED:
		len = 9;
		uid = player_get_uid(player);
		memcpy(&pdu->params[1], &uid, sizeof(uint64_t));

		break;
	case AVRCP_EVENT_TRACK_REACHED_END:
	case AVRCP_EVENT_TRACK_REACHED_START:
		len = 1;
		break;
	case AVRCP_EVENT_SETTINGS_CHANGED:
		len = player_settings_changed(player, pdu);
		break;
	case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
		len = 5;
		bt_put_be16(player_get_id(player), &pdu->params[1]);
		bt_put_be16(player_get_uid_counter(player), &pdu->params[3]);
		break;
	case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
		len = 1;
		break;
	case AVRCP_EVENT_VOLUME_CHANGED:
		volume = media_transport_get_device_volume(dev);
		if (volume < 0)
			goto err;

		pdu->params[1] = volume;
		len = 2;

		break;
	default:
		/* All other events are not supported yet */
		goto err;
	}

	/* Register event and save the transaction used */
	session->registered_events |= (1 << pdu->params[0]);
	session->transaction_events[pdu->params[0]] = transaction;

	pdu->params_len = htons(len);

	return AVC_CTYPE_INTERIM;

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;
	return AVC_CTYPE_REJECTED;
}