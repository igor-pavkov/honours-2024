void avrcp_unregister_player(struct avrcp_player *player)
{
	struct avrcp_server *server = player->server;
	GSList *l;

	server->players = g_slist_remove(server->players, player);

	/* Remove player from sessions using it */
	for (l = player->sessions; l; l = l->next) {
		struct avrcp *session = l->data;
		struct avrcp_data *target = session->target;

		if (target == NULL)
			continue;

		if (target->player != player)
			continue;

		target->player = g_slist_nth_data(server->players, 0);
		if (target->player) {
			target->player->sessions = g_slist_append(
						target->player->sessions,
						session);
			notify_addressed_player_changed(player);
		}
	}

	avrcp_player_event(player,
				AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED, NULL);

	player_destroy(player);
}