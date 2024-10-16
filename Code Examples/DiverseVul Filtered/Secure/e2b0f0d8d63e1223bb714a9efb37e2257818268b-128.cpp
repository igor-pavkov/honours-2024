static void player_remove(gpointer data)
{
	struct avrcp_player *player = data;
	GSList *l;

	/* Don't remove reserved player */
	if (!player->id)
		return;

	for (l = player->sessions; l; l = l->next) {
		struct avrcp *session = l->data;
		struct avrcp_data *controller = session->controller;

		controller->players = g_slist_remove(controller->players,
								player);

		/* Check if current player is being removed */
		if (controller->player == player)
			set_ct_player(session, g_slist_nth_data(
						controller->players, 0));
	}

	player_destroy(player);
}