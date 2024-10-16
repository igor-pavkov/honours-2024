static void dns_tcp_call_writev_done(struct tevent_req *subreq)
{
	struct dns_tcp_call *call = tevent_req_callback_data(subreq,
			struct dns_tcp_call);
	int sys_errno;
	int rc;

	rc = tstream_writev_queue_recv(subreq, &sys_errno);
	TALLOC_FREE(subreq);
	if (rc == -1) {
		const char *reason;

		reason = talloc_asprintf(call, "dns_tcp_call_writev_done: "
					 "tstream_writev_queue_recv() - %d:%s",
					 sys_errno, strerror(sys_errno));
		if (!reason) {
			reason = "dns_tcp_call_writev_done: tstream_writev_queue_recv() failed";
		}

		dns_tcp_terminate_connection(call->dns_conn, reason);
		return;
	}

	/* We don't care about errors */

	talloc_free(call);
}