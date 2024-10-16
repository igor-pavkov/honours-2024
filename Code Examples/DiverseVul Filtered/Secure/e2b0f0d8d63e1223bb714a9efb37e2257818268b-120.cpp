static uint32_t player_get_duration(struct avrcp_player *player)
{
	uint32_t num;

	if (player == NULL)
		return UINT32_MAX;

	num = player->cb->get_duration(player->user_data);
	if (num == 0)
		return UINT32_MAX;

	return num;
}