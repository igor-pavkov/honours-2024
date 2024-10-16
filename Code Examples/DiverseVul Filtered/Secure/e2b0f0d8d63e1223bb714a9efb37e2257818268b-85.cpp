static void set_browsed_player(struct avrcp *session,
					struct avrcp_player *player)
{
	if (!player || !player->id || player->browsed)
		return;

	if (media_player_get_browsable(player->user_data))
		avrcp_set_browsed_player(session, player);
}