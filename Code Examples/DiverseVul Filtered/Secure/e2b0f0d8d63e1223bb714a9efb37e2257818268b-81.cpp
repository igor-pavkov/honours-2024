struct avrcp_player *avrcp_register_player(struct btd_adapter *adapter,
						struct avrcp_player_cb *cb,
						void *user_data,
						GDestroyNotify destroy)
{
	struct avrcp_server *server;
	struct avrcp_player *player;
	GSList *l;
	static uint16_t id = 0;

	server = find_server(servers, adapter);
	if (!server)
		return NULL;

	player = g_new0(struct avrcp_player, 1);
	player->id = ++id;
	player->server = server;
	player->cb = cb;
	player->user_data = user_data;
	player->destroy = destroy;

	server->players = g_slist_append(server->players, player);

	/* Assign player to session without current player */
	for (l = server->sessions; l; l = l->next) {
		struct avrcp *session = l->data;
		struct avrcp_data *target = session->target;

		if (target == NULL)
			continue;

		if (target->player == NULL) {
			target->player = player;
			player->sessions = g_slist_append(player->sessions,
								session);
		}
	}

	avrcp_player_event(player,
				AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED, NULL);

	return player;
}