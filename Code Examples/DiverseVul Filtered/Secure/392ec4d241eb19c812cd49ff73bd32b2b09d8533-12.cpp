static void dns_tcp_call_process_done(struct tevent_req *subreq)
{
	struct dns_tcp_call *call = tevent_req_callback_data(subreq,
			struct dns_tcp_call);
	struct dns_tcp_connection *dns_conn = call->dns_conn;
	WERROR err;

	err = dns_process_recv(subreq, call, &call->out);
	TALLOC_FREE(subreq);
	if (!W_ERROR_IS_OK(err)) {
		DEBUG(1, ("dns_process returned %s\n", win_errstr(err)));
		dns_tcp_terminate_connection(dns_conn,
				"dns_tcp_call_loop: process function failed");
		return;
	}

	/* First add the length of the out buffer */
	RSSVAL(call->out_hdr, 0, call->out.length);
	call->out_iov[0].iov_base = (char *) call->out_hdr;
	call->out_iov[0].iov_len = 2;

	call->out_iov[1].iov_base = (char *) call->out.data;
	call->out_iov[1].iov_len = call->out.length;

	subreq = tstream_writev_queue_send(call,
					   dns_conn->conn->event.ctx,
					   dns_conn->tstream,
					   dns_conn->send_queue,
					   call->out_iov, 2);
	if (subreq == NULL) {
		dns_tcp_terminate_connection(dns_conn, "dns_tcp_call_loop: "
				"no memory for tstream_writev_queue_send");
		return;
	}
	tevent_req_set_callback(subreq, dns_tcp_call_writev_done, call);
}