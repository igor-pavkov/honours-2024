static void set_appname(PgSocket *client, const char *app_name)
{
	char buf[400], abuf[300];
	const char *details;

	if (cf_application_name_add_host) {
		/* give app a name */
		if (!app_name)
			app_name = "app";

		/* add details */
		details = pga_details(&client->remote_addr, abuf, sizeof(abuf));
		snprintf(buf, sizeof(buf), "%s - %s", app_name, details);
		app_name = buf;
	}
	if (app_name) {
		slog_debug(client, "using application_name: %s", app_name);
		varcache_set(&client->vars, "application_name", app_name);
	}
}