static struct avrcp_server *avrcp_server_register(struct btd_adapter *adapter)
{
	struct avrcp_server *server;

	if (avctp_register(adapter, TRUE) < 0)
		return NULL;

	server = g_new0(struct avrcp_server, 1);
	server->adapter = btd_adapter_ref(adapter);

	servers = g_slist_append(servers, server);

	if (!avctp_id)
		avctp_id = avctp_add_state_cb(NULL, state_changed, NULL);

	return server;
}