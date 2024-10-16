static bool avrcp_handle_play(struct avrcp *session)
{
	struct avrcp_player *player = target_get_player(session);

	if (player == NULL)
		return false;

	return player->cb->play(player->user_data);
}