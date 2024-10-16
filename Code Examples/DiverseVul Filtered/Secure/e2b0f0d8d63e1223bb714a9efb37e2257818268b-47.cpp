static void session_destroy(struct avrcp *session, int err)
{
	struct avrcp_server *server = session->server;
	struct btd_service *service;

	server->sessions = g_slist_remove(server->sessions, session);

	session_abort_pending_pdu(session);

	service = btd_device_get_service(session->dev, AVRCP_TARGET_UUID);
	if (service != NULL) {
		if (session->control_id == 0)
			btd_service_connecting_complete(service, err);
		else
			btd_service_disconnecting_complete(service, 0);
	}

	service = btd_device_get_service(session->dev, AVRCP_REMOTE_UUID);
	if (service != NULL) {
		if (session->control_id == 0)
			btd_service_connecting_complete(service, err);
		else
			btd_service_disconnecting_complete(service, 0);
	}

	if (session->browsing_timer > 0)
		timeout_remove(session->browsing_timer);

	if (session->controller != NULL)
		controller_destroy(session);

	if (session->target != NULL)
		target_destroy(session);

	if (session->passthrough_id > 0)
		avctp_unregister_passthrough_handler(session->passthrough_id);

	if (session->control_id > 0)
		avctp_unregister_pdu_handler(session->control_id);

	if (session->browsing_id > 0)
		avctp_unregister_browsing_pdu_handler(session->browsing_id);

	g_free(session);
}