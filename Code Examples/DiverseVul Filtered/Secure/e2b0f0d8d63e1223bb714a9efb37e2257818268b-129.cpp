static uint8_t avrcp_handle_get_current_player_value(struct avrcp *session,
						struct avrcp_header *pdu,
						uint8_t transaction)
{
	struct avrcp_player *player = target_get_player(session);
	uint16_t len = ntohs(pdu->params_len);
	uint8_t *settings;
	unsigned int i;

	if (len <= 1 || pdu->params[0] != len - 1)
		goto err;

	/*
	 * Save a copy of requested settings because we can override them
	 * while responding
	 */
	settings = g_memdup(&pdu->params[1], pdu->params[0]);
	len = 0;

	/*
	 * From sec. 5.7 of AVRCP 1.3 spec, we should igore non-existent IDs
	 * and send a response with the existent ones. Only if all IDs are
	 * non-existent we should send an error.
	 */
	for (i = 0; i < pdu->params[0]; i++) {
		int val;

		if (settings[i] < AVRCP_ATTRIBUTE_EQUALIZER ||
					settings[i] > AVRCP_ATTRIBUTE_SCAN) {
			DBG("Ignoring %u", settings[i]);
			continue;
		}

		val = player_get_setting(player, settings[i]);
		if (val < 0)
			continue;

		pdu->params[++len] = settings[i];
		pdu->params[++len] = val;
	}

	g_free(settings);

	if (len) {
		pdu->params[0] = len / 2;
		pdu->params_len = htons(len + 1);

		return AVC_CTYPE_STABLE;
	}

	error("No valid attributes in request");

err:
	pdu->params_len = htons(1);
	pdu->params[0] = AVRCP_STATUS_INVALID_PARAM;

	return AVC_CTYPE_REJECTED;
}