static void ldapsrv_disconnect_ticket_expired(struct tevent_req *subreq)
{
	struct ldapsrv_connection *conn = tevent_req_callback_data(
		subreq, struct ldapsrv_connection);
	bool ok;

	ok = tevent_wakeup_recv(subreq);
	TALLOC_FREE(subreq);
	if (!ok) {
		DBG_WARNING("tevent_wakeup_recv failed\n");
	}
	conn->deferred_expire_disconnect = NULL;
	ldapsrv_terminate_connection(conn, "network session expired");
}