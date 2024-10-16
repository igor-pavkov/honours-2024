static struct avrcp_player *find_ct_player(struct avrcp *session, uint16_t id)
{
	GSList *l;

	for (l = session->controller->players; l; l = l->next) {
		struct avrcp_player *player = l->data;

		if (player->id == 0) {
			player->id = id;
			return player;
		}

		if (player->id == id)
			return player;
	}

	return NULL;
}