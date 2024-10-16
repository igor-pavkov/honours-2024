static struct avrcp_player *create_ct_player(struct avrcp *session,
								uint16_t id)
{
	struct avrcp_player *player;
	struct media_player *mp;
	const char *path;

	player = g_new0(struct avrcp_player, 1);
	player->id = id;
	player->sessions = g_slist_prepend(player->sessions, session);

	path = device_get_path(session->dev);

	mp = media_player_controller_create(path, id);
	if (mp == NULL)
		return NULL;

	media_player_set_callbacks(mp, &ct_cbs, player);
	player->user_data = mp;
	player->destroy = (GDestroyNotify) media_player_destroy;

	if (session->controller->player == NULL)
		set_ct_player(session, player);

	session->controller->players = g_slist_prepend(
						session->controller->players,
						player);

	return player;
}