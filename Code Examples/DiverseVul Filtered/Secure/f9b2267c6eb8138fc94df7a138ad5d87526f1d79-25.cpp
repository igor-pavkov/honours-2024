static void ldapsrv_call_writev_done(struct tevent_req *subreq)
{
	struct ldapsrv_call *call =
		tevent_req_callback_data(subreq,
		struct ldapsrv_call);
	struct ldapsrv_connection *conn = call->conn;
	int sys_errno;
	int rc;

	rc = tstream_writev_queue_recv(subreq, &sys_errno);
	TALLOC_FREE(subreq);

	/* This releases the ASN.1 encoded packets from memory */
	TALLOC_FREE(call->out_iov);
	if (rc == -1) {
		const char *reason;

		reason = talloc_asprintf(call, "ldapsrv_call_writev_done: "
					 "tstream_writev_queue_recv() - %d:%s",
					 sys_errno, strerror(sys_errno));
		if (reason == NULL) {
			reason = "ldapsrv_call_writev_done: "
				 "tstream_writev_queue_recv() failed";
		}

		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	if (call->postprocess_send) {
		subreq = call->postprocess_send(call,
						conn->connection->event.ctx,
						call->postprocess_private);
		if (subreq == NULL) {
			ldapsrv_terminate_connection(conn, "ldapsrv_call_writev_done: "
					"call->postprocess_send - no memory");
			return;
		}
		tevent_req_set_callback(subreq,
					ldapsrv_call_postprocess_done,
					call);
		return;
	}

	/* Perhaps still some more to send */
	if (call->replies != NULL) {
		ldapsrv_call_writev_start(call);
		return;
	}

	if (!call->notification.busy) {
		TALLOC_FREE(call);
	}

	ldapsrv_call_read_next(conn);
}