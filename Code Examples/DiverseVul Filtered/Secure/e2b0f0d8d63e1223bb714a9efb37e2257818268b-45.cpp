static uint16_t player_get_uid_counter(struct avrcp_player *player)
{
	if (player == NULL)
		return 0x0000;

	return player->uid_counter;
}