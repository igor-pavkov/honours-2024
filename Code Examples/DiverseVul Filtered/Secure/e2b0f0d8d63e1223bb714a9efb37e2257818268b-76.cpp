static GList *player_list_settings(struct avrcp_player *player)
{
	if (player == NULL)
		return NULL;

	return player->cb->list_settings(player->user_data);
}