static uint16_t player_settings_changed(struct avrcp_player *player,
						struct avrcp_header *pdu)
{
	GList *settings = player_list_settings(player);
	int size = 2;

	for (; settings; settings = settings->next) {
		const char *key = settings->data;
		int attr;
		int val;

		attr = attr_to_val(key);
		if (attr < 0)
			continue;

		val = player_get_setting(player, attr);
		if (val < 0)
			continue;

		pdu->params[size++] = attr;
		pdu->params[size++] = val;
	}

	g_list_free(settings);

	pdu->params[1] = (size - 2) >> 1;
	return size;
}