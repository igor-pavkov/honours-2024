static void target_init(struct avrcp *session)
{
	struct avrcp_server *server = session->server;
	struct avrcp_data *target;
	struct avrcp_player *player;
	struct btd_service *service;

	if (session->target != NULL)
		return;

	target = data_init(session, AVRCP_REMOTE_UUID);
	session->target = target;

	DBG("%p version 0x%04x", target, target->version);

	service = btd_device_get_service(session->dev, AVRCP_REMOTE_UUID);
	btd_service_connecting_complete(service, 0);

	player = g_slist_nth_data(server->players, 0);
	if (player != NULL) {
		int8_t init_volume;
		target->player = player;
		player->sessions = g_slist_prepend(player->sessions, session);

		init_volume = media_player_get_device_volume(session->dev);
		media_transport_update_device_volume(session->dev, init_volume);
	}

	session->supported_events |= (1 << AVRCP_EVENT_STATUS_CHANGED) |
				(1 << AVRCP_EVENT_TRACK_CHANGED) |
				(1 << AVRCP_EVENT_TRACK_REACHED_START) |
				(1 << AVRCP_EVENT_TRACK_REACHED_END) |
				(1 << AVRCP_EVENT_SETTINGS_CHANGED);

	if (target->version < 0x0104)
		return;

	session->supported_events |=
				(1 << AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED) |
				(1 << AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED) |
				(1 << AVRCP_EVENT_VOLUME_CHANGED);

	/* Only check capabilities if controller is not supported */
	if (session->controller == NULL)
		avrcp_get_capabilities(session);

	if (!(target->features & AVRCP_FEATURE_BROWSING))
		return;

	avrcp_connect_browsing(session);
}