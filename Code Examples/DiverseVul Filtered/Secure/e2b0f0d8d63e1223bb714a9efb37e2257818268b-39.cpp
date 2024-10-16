struct avrcp_player *avrcp_get_target_player_by_device(struct btd_device *dev)
{
	struct avrcp_server *server;
	struct avrcp *session;
	struct avrcp_data *target;

	server = find_server(servers, device_get_adapter(dev));
	if (server == NULL)
		return NULL;

	session = find_session(server->sessions, dev);
	if (session == NULL)
		return NULL;

	target = session->target;
	if (target == NULL)
		return NULL;

	return target->player;
}