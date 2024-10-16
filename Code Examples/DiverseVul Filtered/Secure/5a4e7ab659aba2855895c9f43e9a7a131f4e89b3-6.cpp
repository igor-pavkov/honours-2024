int irc_cap_toggle (IRC_SERVER_REC *server, char *cap, int enable)
{
	if (cap == NULL || *cap == '\0')
		return FALSE;

	/* If the negotiation hasn't been completed yet just queue the requests */
	if (!server->cap_complete) {
		if (enable && !gslist_find_string(server->cap_queue, cap)) {
			server->cap_queue = g_slist_prepend(server->cap_queue, g_strdup(cap));
			return TRUE;
		}
		else if (!enable && gslist_find_string(server->cap_queue, cap)) {
			server->cap_queue = gslist_delete_string(server->cap_queue, cap, g_free);
			return TRUE;
		}

		return FALSE;
	}

	if (enable && !gslist_find_string(server->cap_active, cap)) {
		/* Make sure the required cap is supported by the server */
		if (!g_hash_table_lookup_extended(server->cap_supported, cap, NULL, NULL))
			return FALSE;

		signal_emit("server cap req", 2, server, cap);
		irc_send_cmdv(server, "CAP REQ %s", cap);
		return TRUE;
	}
	else if (!enable && gslist_find_string(server->cap_active, cap)) {
		char *negcap = g_strdup_printf("-%s", cap);

		signal_emit("server cap req", 2, server, negcap);
		irc_send_cmdv(server, "CAP REQ %s", negcap);

		g_free(negcap);
		return TRUE;
	}

	return FALSE;
}