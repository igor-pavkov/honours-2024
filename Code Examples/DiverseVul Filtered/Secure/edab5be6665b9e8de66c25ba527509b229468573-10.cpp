bool set_pool(PgSocket *client, const char *dbname, const char *username, const char *password, bool takeover)
{
	/* find database */
	client->db = find_database(dbname);
	if (!client->db) {
		client->db = register_auto_database(dbname);
		if (!client->db) {
			disconnect_client(client, true, "No such database: %s", dbname);
			if (cf_log_connections)
				slog_info(client, "login failed: db=%s user=%s", dbname, username);
			return false;
		}
		else {
			slog_info(client, "registered new auto-database: db = %s", dbname );
		}
	}

	/* are new connections allowed? */
	if (client->db->db_disabled) {
		disconnect_client(client, true, "database does not allow connections: %s", dbname);
		return false;
	}

	if (client->db->admin) {
		if (admin_pre_login(client, username))
			return finish_set_pool(client, takeover);
	}

	/* find user */
	if (cf_auth_type == AUTH_ANY) {
		/* ignore requested user */
		if (client->db->forced_user == NULL) {
			slog_error(client, "auth_type=any requires forced user");
			disconnect_client(client, true, "bouncer config error");
			return false;
		}
		client->auth_user = client->db->forced_user;
	} else {
		/* the user clients wants to log in as */
		client->auth_user = find_user(username);
		if (!client->auth_user && client->db->auth_user) {
			if (takeover) {
				client->auth_user = add_db_user(client->db, username, password);
				return finish_set_pool(client, takeover);
			}
			start_auth_request(client, username);
			return false;
		}
		if (!client->auth_user) {
			disconnect_client(client, true, "No such user: %s", username);
			if (cf_log_connections)
				slog_info(client, "login failed: db=%s user=%s", dbname, username);
			return false;
		}
	}
	return finish_set_pool(client, takeover);
}