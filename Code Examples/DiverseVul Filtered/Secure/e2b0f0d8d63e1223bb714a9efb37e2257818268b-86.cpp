static void avrcp_server_unregister(struct avrcp_server *server)
{
	g_slist_free_full(server->sessions, g_free);
	g_slist_free_full(server->players, player_destroy);

	servers = g_slist_remove(servers, server);

	avctp_unregister(server->adapter);
	btd_adapter_unref(server->adapter);
	g_free(server);

	if (servers)
		return;

	if (avctp_id) {
		avctp_remove_state_cb(avctp_id);
		avctp_id = 0;
	}
}