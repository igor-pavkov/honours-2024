static void ldapsrv_call_wait_done(struct tevent_req *subreq)
{
	struct ldapsrv_call *call =
		tevent_req_callback_data(subreq,
		struct ldapsrv_call);
	struct ldapsrv_connection *conn = call->conn;
	NTSTATUS status;

	conn->active_call = NULL;

	status = call->wait_recv(subreq);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		const char *reason;

		reason = talloc_asprintf(call, "ldapsrv_call_wait_done: "
					 "call->wait_recv() - %s",
					 nt_errstr(status));
		if (reason == NULL) {
			reason = nt_errstr(status);
		}

		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	ldapsrv_call_writev_start(call);
}