static void state_changed(struct btd_device *device, avctp_state_t old_state,
					avctp_state_t new_state, int err,
					void *user_data)
{
	struct avrcp_server *server;
	struct avrcp *session;

	server = find_server(servers, device_get_adapter(device));
	if (!server)
		return;

	session = find_session(server->sessions, device);

	switch (new_state) {
	case AVCTP_STATE_DISCONNECTED:
		if (session == NULL)
			break;

		session_destroy(session, err);

		break;
	case AVCTP_STATE_CONNECTING:
		if (session != NULL)
			break;

		session_create(server, device);

		break;
	case AVCTP_STATE_CONNECTED:
		if (session == NULL || session->control_id > 0)
			break;

		session_init_control(session);

		break;
	case AVCTP_STATE_BROWSING_CONNECTED:
		if (session == NULL || session->browsing_id > 0)
			break;

		session_init_browsing(session);

		break;
	case AVCTP_STATE_BROWSING_CONNECTING:
	default:
		return;
	}
}