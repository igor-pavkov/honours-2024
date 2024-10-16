static void ldapsrv_terminate_connection_done(struct tevent_req *subreq)
{
	struct ldapsrv_connection *conn =
		tevent_req_callback_data(subreq,
		struct ldapsrv_connection);
	int sys_errno;
	bool ok;

	tstream_disconnect_recv(subreq, &sys_errno);
	TALLOC_FREE(subreq);

	if (conn->sockets.active == conn->sockets.raw) {
		TALLOC_FREE(conn->sockets.tls);
		TALLOC_FREE(conn->sockets.sasl);
		TALLOC_FREE(conn->sockets.raw);
		stream_terminate_connection(conn->connection,
					    conn->limits.reason);
		return;
	}

	TALLOC_FREE(conn->sockets.tls);
	TALLOC_FREE(conn->sockets.sasl);
	conn->sockets.active = conn->sockets.raw;

	subreq = tstream_disconnect_send(conn,
					 conn->connection->event.ctx,
					 conn->sockets.active);
	if (subreq == NULL) {
		TALLOC_FREE(conn->sockets.raw);
		stream_terminate_connection(conn->connection,
					    conn->limits.reason);
		return;
	}
	ok = tevent_req_set_endtime(subreq,
				    conn->connection->event.ctx,
				    conn->limits.endtime);
	if (!ok) {
		TALLOC_FREE(conn->sockets.raw);
		stream_terminate_connection(conn->connection,
					    conn->limits.reason);
		return;
	}
	tevent_req_set_callback(subreq, ldapsrv_terminate_connection_done, conn);
}