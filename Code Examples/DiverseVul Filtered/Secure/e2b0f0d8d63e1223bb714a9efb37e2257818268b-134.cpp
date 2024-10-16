static void controller_init(struct avrcp *session)
{
	struct avrcp_player *player;
	struct btd_service *service;
	struct avrcp_data *controller;

	if (session->controller != NULL)
		return;

	controller = data_init(session, AVRCP_TARGET_UUID);
	session->controller = controller;

	DBG("%p version 0x%04x", controller, controller->version);

	service = btd_device_get_service(session->dev, AVRCP_TARGET_UUID);
	btd_service_connecting_complete(service, 0);

	/* Only create player if category 1 is supported */
	if (controller->features & AVRCP_FEATURE_CATEGORY_1) {
		player = create_ct_player(session, 0);
		if (player == NULL)
			return;
	}

	if (controller->version < 0x0103)
		return;

	avrcp_get_capabilities(session);

	if (controller->version < 0x0104)
		return;

	if (!(controller->features & AVRCP_FEATURE_BROWSING))
		return;

	avrcp_connect_browsing(session);
}