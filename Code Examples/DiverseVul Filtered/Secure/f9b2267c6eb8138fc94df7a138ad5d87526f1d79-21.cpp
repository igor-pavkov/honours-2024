static void ldapsrv_call_process_done(struct tevent_req *subreq)
{
	struct ldapsrv_call *call =
		tevent_req_callback_data(subreq,
		struct ldapsrv_call);
	struct ldapsrv_connection *conn = call->conn;
	NTSTATUS status;

	conn->active_call = NULL;

	status = ldapsrv_process_call_recv(subreq);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		ldapsrv_terminate_connection(conn, nt_errstr(status));
		return;
	}

	if (call->wait_send != NULL) {
		subreq = call->wait_send(call,
					 conn->connection->event.ctx,
					 call->wait_private);
		if (subreq == NULL) {
			ldapsrv_terminate_connection(conn,
					"ldapsrv_call_process_done: "
					"call->wait_send - no memory");
			return;
		}
		tevent_req_set_callback(subreq,
					ldapsrv_call_wait_done,
					call);
		conn->active_call = subreq;
		return;
	}

	ldapsrv_call_writev_start(call);
}