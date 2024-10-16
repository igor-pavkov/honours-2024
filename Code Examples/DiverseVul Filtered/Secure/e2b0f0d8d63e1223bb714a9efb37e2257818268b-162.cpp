static uint16_t player_get_id(struct avrcp_player *player)
{
	if (player == NULL)
		return 0x0000;

	return player->id;
}