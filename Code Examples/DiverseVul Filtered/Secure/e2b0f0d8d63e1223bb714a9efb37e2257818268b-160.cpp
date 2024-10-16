static uint32_t player_get_position(struct avrcp_player *player)
{
	if (player == NULL)
		return 0;

	return player->cb->get_position(player->user_data);
}