static void cap_emit_signal (IRC_SERVER_REC *server, char *cmd, char *args)
{
	char *signal_name;

	signal_name = g_strdup_printf("server cap %s %s", cmd, args? args: "");
	signal_emit(signal_name, 1, server);
	g_free(signal_name);
}