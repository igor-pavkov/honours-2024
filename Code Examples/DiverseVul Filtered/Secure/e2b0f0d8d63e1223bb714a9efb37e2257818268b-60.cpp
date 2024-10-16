static void target_destroy(struct avrcp *session)
{
	struct avrcp_data *target = session->target;
	struct avrcp_player *player = target->player;

	DBG("%p", target);

	if (player != NULL)
		player->sessions = g_slist_remove(player->sessions, session);

	g_free(target);
}