static bool finish_set_pool(PgSocket *client, bool takeover)
{
	PgUser *user = client->auth_user;
	/* pool user may be forced */
	if (client->db->forced_user) {
		user = client->db->forced_user;
	}
	client->pool = get_pool(client->db, user);
	if (!client->pool) {
		disconnect_client(client, true, "no memory for pool");
		return false;
	}

	if (cf_log_connections)
		slog_info(client, "login attempt: db=%s user=%s", client->db->name, client->auth_user->name);

	if (!check_fast_fail(client))
		return false;

	if (takeover)
		return true;

	if (client->pool->db->admin) {
		if (!admin_post_login(client))
			return false;
	}

	if (cf_auth_type <= AUTH_TRUST || client->own_user) {
		if (!finish_client_login(client))
			return false;
	} else {
		if (!send_client_authreq(client)) {
			disconnect_client(client, false, "failed to send auth req");
			return false;
		}
	}
	return true;
}