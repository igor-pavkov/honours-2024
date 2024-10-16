static void set_addressed_player(struct avrcp *session,
					struct avrcp_player *player)
{
	if (!player || !player->id || player->addressed ||
				session->controller->version < 0x0104)
		return;

	/* Set player as addressed */
	avrcp_set_addressed_player(session, player);
}