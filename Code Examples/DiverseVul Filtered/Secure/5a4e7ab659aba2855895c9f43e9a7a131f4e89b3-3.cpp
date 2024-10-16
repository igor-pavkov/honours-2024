void irc_cap_finish_negotiation (IRC_SERVER_REC *server)
{
	if (server->cap_complete)
		return;

	server->cap_complete = TRUE;
	irc_send_cmd_now(server, "CAP END");

	signal_emit("server cap end", 1, server);
}