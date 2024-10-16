static void event_invalid_cap (IRC_SERVER_REC *server, const char *data, const char *from)
{
	/* The server didn't understand one (or more) requested caps, terminate the negotiation.
	 * This could be handled in a graceful way but since it shouldn't really ever happen this seems a
	 * good way to deal with 410 errors. */
	server->cap_complete = FALSE;
	irc_send_cmd_now(server, "CAP END");
}