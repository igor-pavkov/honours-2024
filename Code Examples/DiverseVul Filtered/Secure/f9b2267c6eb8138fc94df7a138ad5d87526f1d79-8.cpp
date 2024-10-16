static void ldapsrv_call_postprocess_done(struct tevent_req *subreq)
{
	struct ldapsrv_call *call =
		tevent_req_callback_data(subreq,
		struct ldapsrv_call);
	struct ldapsrv_connection *conn = call->conn;
	NTSTATUS status;

	status = call->postprocess_recv(subreq);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		const char *reason;

		reason = talloc_asprintf(call, "ldapsrv_call_postprocess_done: "
					 "call->postprocess_recv() - %s",
					 nt_errstr(status));
		if (reason == NULL) {
			reason = nt_errstr(status);
		}

		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	TALLOC_FREE(call);

	ldapsrv_call_read_next(conn);
}