static bool avrcp_handle_previous(struct avrcp *session)
{
	struct avrcp_player *player = target_get_player(session);

	if (player == NULL)
		return false;

	return player->cb->previous(player->user_data);
}