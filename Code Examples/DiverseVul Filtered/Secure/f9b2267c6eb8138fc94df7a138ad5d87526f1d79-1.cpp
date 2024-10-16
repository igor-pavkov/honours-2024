static void ldapsrv_terminate_connection(struct ldapsrv_connection *conn,
					 const char *reason)
{
	struct tevent_req *subreq;

	if (conn->limits.reason) {
		return;
	}

	DLIST_REMOVE(conn->service->connections, conn);

	conn->limits.endtime = timeval_current_ofs(0, 500);

	tevent_queue_stop(conn->sockets.send_queue);
	TALLOC_FREE(conn->sockets.read_req);
	TALLOC_FREE(conn->deferred_expire_disconnect);
	if (conn->active_call) {
		tevent_req_cancel(conn->active_call);
		conn->active_call = NULL;
	}

	conn->limits.reason = talloc_strdup(conn, reason);
	if (conn->limits.reason == NULL) {
		TALLOC_FREE(conn->sockets.tls);
		TALLOC_FREE(conn->sockets.sasl);
		TALLOC_FREE(conn->sockets.raw);
		stream_terminate_connection(conn->connection, reason);
		return;
	}

	subreq = tstream_disconnect_send(conn,
					 conn->connection->event.ctx,
					 conn->sockets.active);
	if (subreq == NULL) {
		TALLOC_FREE(conn->sockets.tls);
		TALLOC_FREE(conn->sockets.sasl);
		TALLOC_FREE(conn->sockets.raw);
		stream_terminate_connection(conn->connection, reason);
		return;
	}
	tevent_req_set_endtime(subreq,
			       conn->connection->event.ctx,
			       conn->limits.endtime);
	tevent_req_set_callback(subreq, ldapsrv_terminate_connection_done, conn);
}