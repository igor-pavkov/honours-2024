bool handle_auth_response(PgSocket *client, PktHdr *pkt) {
	uint16_t columns;
	uint32_t length;
	const char *username, *password;
	PgUser user;
	PgSocket *server = client->link;

	switch(pkt->type) {
	case 'T':	/* RowDescription */
		if (!mbuf_get_uint16be(&pkt->data, &columns)) {
			disconnect_server(server, false, "bad packet");
			return false;
		}
		if (columns != 2u) {
			disconnect_server(server, false, "expected 1 column from login query, not %hu", columns);
			return false;
		}
		break;
	case 'D':	/* DataRow */
		memset(&user, 0, sizeof(user));
		if (!mbuf_get_uint16be(&pkt->data, &columns)) {
			disconnect_server(server, false, "bad packet");
			return false;
		}
		if (columns != 2u) {
			disconnect_server(server, false, "expected 1 column from login query, not %hu", columns);
			return false;
		}
		if (!mbuf_get_uint32be(&pkt->data, &length)) {
			disconnect_server(server, false, "bad packet");
			return false;
		}
		if (!mbuf_get_chars(&pkt->data, length, &username)) {
			disconnect_server(server, false, "bad packet");
			return false;
		}
		if (sizeof(user.name) - 1 < length)
			length = sizeof(user.name) - 1;
		memcpy(user.name, username, length);
		if (!mbuf_get_uint32be(&pkt->data, &length)) {
			disconnect_server(server, false, "bad packet");
			return false;
		}
		if (length == (uint32_t)-1) {
			/*
			 * NULL - set an md5 password with an impossible value,
			 * so that nothing will ever match
			 */
			password = "md5";
			length = 3;
		} else {
			if (!mbuf_get_chars(&pkt->data, length, &password)) {
				disconnect_server(server, false, "bad packet");
				return false;
			}
		}
		if (sizeof(user.passwd)  - 1 < length)
			length = sizeof(user.passwd) - 1;
		memcpy(user.passwd, password, length);

		client->auth_user = add_db_user(client->db, user.name, user.passwd);
		if (!client->auth_user) {
			disconnect_server(server, false, "unable to allocate new user for auth");
			return false;
		}
		break;
	case 'C':	/* CommandComplete */
		break;
	case '1':	/* ParseComplete */
		break;
	case '2':	/* BindComplete */
		break;
	case 'Z':	/* ReadyForQuery */
		sbuf_prepare_skip(&client->link->sbuf, pkt->len);
		if (!client->auth_user) {
			if (cf_log_connections)
				slog_info(client, "login failed: db=%s", client->db->name);
			disconnect_client(client, true, "No such user");
		} else {
			slog_noise(client, "auth query complete");
			client->link->resetting = true;
			sbuf_continue(&client->sbuf);
		}
		/*
		 * either sbuf_continue or disconnect_client could disconnect the server
		 * way down in their bowels of other callbacks. so check that, and
		 * return appropriately (similar to reuse_on_release)
		 */
		if (server->state == SV_FREE || server->state == SV_JUSTFREE)
			return false;
		return true;
	default:
		disconnect_server(server, false, "unexpected response from login query");
		return false;
	}
	sbuf_prepare_skip(&server->sbuf, pkt->len);
	return true;
}