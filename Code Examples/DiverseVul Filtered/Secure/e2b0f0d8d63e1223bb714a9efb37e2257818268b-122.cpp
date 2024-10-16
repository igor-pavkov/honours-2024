static struct avrcp_player *find_tg_player(struct avrcp *session, uint16_t id)
{
	struct avrcp_server *server = session->server;
	GSList *l;

	for (l = server->players; l; l = l->next) {
		struct avrcp_player *player = l->data;

		if (player->id == id)
			return player;
	}

	return NULL;
}