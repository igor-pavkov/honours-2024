static void player_destroy(gpointer data)
{
	struct avrcp_player *player = data;

	if (player->destroy)
		player->destroy(player->user_data);

	if (player->changed_id > 0)
		g_source_remove(player->changed_id);

	g_slist_free(player->sessions);
	g_free(player->path);
	g_free(player->change_path);
	g_free(player->features);
	g_free(player);
}