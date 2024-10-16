static gboolean avrcp_player_value_rsp(struct avctp *conn, uint8_t code,
					uint8_t subunit, uint8_t transaction,
					uint8_t *operands, size_t operand_count,
					void *user_data)
{
	struct avrcp *session = user_data;
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	struct avrcp_header *pdu = (void *) operands;
	uint8_t count;
	int i;

	if (pdu == NULL) {
		media_player_set_setting(mp, "Error", "Timeout");
		return FALSE;
	}

	if (code == AVC_CTYPE_REJECTED) {
		media_player_set_setting(mp, "Error",
					status_to_str(pdu->params[0]));
		return FALSE;
	}

	count = pdu->params[0];

	if (pdu->params_len < count * 2)
		return FALSE;

	for (i = 1; count > 0; count--, i += 2) {
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

	return FALSE;
}