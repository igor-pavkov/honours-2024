static void controller_destroy(struct avrcp *session)
{
	struct avrcp_data *controller = session->controller;

	DBG("%p", controller);

	g_slist_free_full(controller->players, player_destroy);

	g_free(controller);
}