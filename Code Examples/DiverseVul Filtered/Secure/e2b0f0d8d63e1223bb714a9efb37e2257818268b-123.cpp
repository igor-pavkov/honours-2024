static bool avrcp_handle_next(struct avrcp *session)
{
	struct avrcp_player *player = target_get_player(session);

	if (player == NULL)
		return false;

	return player->cb->next(player->user_data);
}