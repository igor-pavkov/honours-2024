bool set_pool(PgSocket *client, const char *dbname, const char *username)
{
	PgDatabase *db;
	PgUser *user;

	/* find database */
	db = find_database(dbname);
	if (!db) {
		db = register_auto_database(dbname);
		if (!db) {
			disconnect_client(client, true, "No such database: %s", dbname);
			return false;
		}
		else {
			slog_info(client, "registered new auto-database: db = %s", dbname );
		}
	}

	/* find user */
	if (cf_auth_type == AUTH_ANY) {
		/* ignore requested user */
		user = NULL;

		if (db->forced_user == NULL) {
			slog_error(client, "auth_type=any requires forced user");
			disconnect_client(client, true, "bouncer config error");
			return false;
		}
		client->auth_user = db->forced_user;
	} else {
		/* the user clients wants to log in as */
		user = find_user(username);
		if (!user) {
			disconnect_client(client, true, "No such user: %s", username);
			return false;
		}
		client->auth_user = user;
	}

	/* pool user may be forced */
	if (db->forced_user)
		user = db->forced_user;
	client->pool = get_pool(db, user);
	if (!client->pool) {
		disconnect_client(client, true, "no memory for pool");
		return false;
	}

	return check_fast_fail(client);
}