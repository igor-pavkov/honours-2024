static void avrcp_status_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	struct avrcp_player *player = session->controller->player;
	struct media_player *mp = player->user_data;
	uint8_t value;
	const char *curval, *strval;

	value = pdu->params[1];

	curval = media_player_get_status(mp);
	strval = status_to_string(value);

	if (g_strcmp0(curval, strval) == 0)
		return;

	media_player_set_status(mp, strval);
	avrcp_get_play_status(session);
}