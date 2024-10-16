static bool decide_startup_pool(PgSocket *client, PktHdr *pkt)
{
	const char *username = NULL, *dbname = NULL;
	const char *key, *val;
	bool ok;

	while (1) {
		ok = mbuf_get_string(&pkt->data, &key);
		if (!ok || *key == 0)
			break;
		ok = mbuf_get_string(&pkt->data, &val);
		if (!ok)
			break;

		if (strcmp(key, "database") == 0) {
			slog_debug(client, "got var: %s=%s", key, val);
			dbname = val;
		} else if (strcmp(key, "user") == 0) {
			slog_debug(client, "got var: %s=%s", key, val);
			username = val;
		} else if (varcache_set(&client->vars, key, val)) {
			slog_debug(client, "got var: %s=%s", key, val);
		} else if (strlist_contains(cf_ignore_startup_params, key)) {
			slog_debug(client, "ignoring startup parameter: %s=%s", key, val);
		} else {
			slog_warning(client, "unsupported startup parameter: %s=%s", key, val);
			disconnect_client(client, true, "Unsupported startup parameter: %s", key);
			return false;
		}
	}
	if (!username || !username[0]) {
		disconnect_client(client, true, "No username supplied");
		return false;
	}

	/* if missing dbname, default to username */
	if (!dbname || !dbname[0])
		dbname = username;

	/* check if limit allows, dont limit admin db
	   nb: new incoming conn will be attached to PgSocket, thus
	   get_active_client_count() counts it */
	if (get_active_client_count() > cf_max_client_conn) {
		if (strcmp(dbname, "pgbouncer") != 0) {
			disconnect_client(client, true, "no more connections allowed (max_client_conn)");
			return false;
		}
	}

	/* find pool and log about it */
	if (set_pool(client, dbname, username)) {
		if (cf_log_connections)
			slog_info(client, "login attempt: db=%s user=%s", dbname, username);
		return true;
	} else {
		if (cf_log_connections)
			slog_info(client, "login failed: db=%s user=%s", dbname, username);
		return false;
	}
}