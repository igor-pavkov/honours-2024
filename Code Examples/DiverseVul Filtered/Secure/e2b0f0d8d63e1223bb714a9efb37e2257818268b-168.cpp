static bool avrcp_handle_stop(struct avrcp *session)
{
	struct avrcp_player *player = target_get_player(session);

	if (player == NULL)
		return false;

	return player->cb->stop(player->user_data);
}