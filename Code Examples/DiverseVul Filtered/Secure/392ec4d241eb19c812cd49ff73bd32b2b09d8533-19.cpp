static void dns_tcp_accept(struct stream_connection *conn)
{
	struct dns_socket *dns_socket;
	struct dns_tcp_connection *dns_conn;
	struct tevent_req *subreq;
	int rc;

	dns_conn = talloc_zero(conn, struct dns_tcp_connection);
	if (dns_conn == NULL) {
		stream_terminate_connection(conn,
				"dns_tcp_accept: out of memory");
		return;
	}

	dns_conn->send_queue = tevent_queue_create(conn, "dns_tcp_accept");
	if (dns_conn->send_queue == NULL) {
		stream_terminate_connection(conn,
				"dns_tcp_accept: out of memory");
		return;
	}

	dns_socket = talloc_get_type(conn->private_data, struct dns_socket);

	TALLOC_FREE(conn->event.fde);

	rc = tstream_bsd_existing_socket(dns_conn,
			socket_get_fd(conn->socket),
			&dns_conn->tstream);
	if (rc < 0) {
		stream_terminate_connection(conn,
				"dns_tcp_accept: out of memory");
		return;
	}

	dns_conn->conn = conn;
	dns_conn->dns_socket = dns_socket;
	conn->private_data = dns_conn;

	/*
	 * The dns tcp pdu's has the length as 2 byte (initial_read_size),
	 * packet_full_request_u16 provides the pdu length then.
	 */
	subreq = tstream_read_pdu_blob_send(dns_conn,
					    dns_conn->conn->event.ctx,
					    dns_conn->tstream,
					    2, /* initial_read_size */
					    packet_full_request_u16,
					    dns_conn);
	if (subreq == NULL) {
		dns_tcp_terminate_connection(dns_conn, "dns_tcp_accept: "
				"no memory for tstream_read_pdu_blob_send");
		return;
	}
	tevent_req_set_callback(subreq, dns_tcp_call_loop, dns_conn);
}