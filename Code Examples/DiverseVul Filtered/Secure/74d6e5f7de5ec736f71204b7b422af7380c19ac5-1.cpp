static bool handle_client_work(PgSocket *client, PktHdr *pkt)
{
	SBuf *sbuf = &client->sbuf;

	switch (pkt->type) {

	/* one-packet queries */
	case 'Q':		/* Query */
		if (cf_disable_pqexec) {
			slog_error(client, "Client used 'Q' packet type.");
			disconnect_client(client, true, "PQexec disallowed");
			return false;
		}
	case 'F':		/* FunctionCall */

	/* request immediate response from server */
	case 'H':		/* Flush */
	case 'S':		/* Sync */

	/* copy end markers */
	case 'c':		/* CopyDone(F/B) */
	case 'f':		/* CopyFail(F/B) */

	/*
	 * extended protocol allows server (and thus pooler)
	 * to buffer packets until sync or flush is sent by client
	 */
	case 'P':		/* Parse */
	case 'E':		/* Execute */
	case 'C':		/* Close */
	case 'B':		/* Bind */
	case 'D':		/* Describe */
	case 'd':		/* CopyData(F/B) */

		/* update stats */
		if (!client->query_start) {
			client->pool->stats.request_count++;
			client->query_start = get_cached_time();
		}

		if (client->pool->db->admin)
			return admin_handle_client(client, pkt);

		/* aquire server */
		if (!find_server(client))
			return false;

		client->pool->stats.client_bytes += pkt->len;

		/* tag the server as dirty */
		client->link->ready = false;
		client->link->idle_tx = false;

		/* forward the packet */
		sbuf_prepare_send(sbuf, &client->link->sbuf, pkt->len);
		break;

	/* client wants to go away */
	default:
		slog_error(client, "unknown pkt from client: %d/0x%x", pkt->type, pkt->type);
		disconnect_client(client, true, "unknown pkt");
		return false;
	case 'X': /* Terminate */
		disconnect_client(client, false, "client close request");
		return false;
	}
	return true;
}