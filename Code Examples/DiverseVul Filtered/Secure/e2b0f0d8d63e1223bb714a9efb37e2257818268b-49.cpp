static uint64_t player_get_uid(struct avrcp_player *player)
{
	if (player == NULL)
		return UINT64_MAX;

	return player->cb->get_uid(player->user_data);
}