static void avrcp_controller_server_remove(struct btd_profile *p,
						struct btd_adapter *adapter)
{
	struct avrcp_server *server;

	DBG("path %s", adapter_get_path(adapter));

	server = find_server(servers, adapter);
	if (!server)
		return;

	if (server->ct_record_id != 0) {
		adapter_service_remove(adapter, server->ct_record_id);
		server->ct_record_id = 0;
	}

	if (server->tg_record_id == 0)
		avrcp_server_unregister(server);
}