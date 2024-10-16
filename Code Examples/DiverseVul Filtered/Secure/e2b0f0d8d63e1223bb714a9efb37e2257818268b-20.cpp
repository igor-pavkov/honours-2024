static void avrcp_addressed_player_changed(struct avrcp *session,
						struct avrcp_header *pdu)
{
	struct avrcp_player *player = session->controller->player;
	uint16_t id = get_be16(&pdu->params[1]);

	if (player != NULL && player->id == id)
		return;

	player = find_ct_player(session, id);
	if (player == NULL) {
		player = create_ct_player(session, id);
		if (player == NULL)
			return;
	}

	player->addressed = true;
	player->uid_counter = get_be16(&pdu->params[3]);
	set_ct_player(session, player);

	if (player->features != NULL)
		return;

	avrcp_get_media_player_list(session);
}