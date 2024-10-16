static uint8_t player_get_status(struct avrcp_player *player)
{
	const char *value;

	if (player == NULL)
		return AVRCP_PLAY_STATUS_STOPPED;

	value = player->cb->get_status(player->user_data);
	if (value == NULL)
		return AVRCP_PLAY_STATUS_STOPPED;

	return play_status_to_val(value);
}