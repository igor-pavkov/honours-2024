static void avrcp_setting_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	uint8_t count = pdu->params[1];
	int i;

	for (i = 2; count > 0; count--, i += 2) {
		const char *key;
		const char *value;

		key = attr_to_str(pdu->params[i]);
		if (key == NULL)
			continue;

		value = attrval_to_str(pdu->params[i], pdu->params[i + 1]);
		if (value == NULL)
			continue;

		media_player_set_setting(mp, key, value);
	}
}