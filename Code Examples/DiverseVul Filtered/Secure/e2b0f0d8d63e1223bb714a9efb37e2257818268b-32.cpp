static void set_ct_player(struct avrcp *session, struct avrcp_player *player)
{
	struct btd_service *service;

	if (session->controller->player == player)
		goto done;

	session->controller->player = player;
	service = btd_device_get_service(session->dev, AVRCP_TARGET_UUID);
	control_set_player(service, player ?
			media_player_get_path(player->user_data) : NULL);

done:
	set_addressed_player(session, player);
	set_browsed_player(session, player);
}