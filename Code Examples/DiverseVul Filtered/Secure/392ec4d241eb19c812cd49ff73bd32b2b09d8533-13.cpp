static NTSTATUS dns_add_socket(struct dns_server *dns,
			       const struct model_ops *model_ops,
			       const char *name,
			       const char *address,
			       uint16_t port)
{
	struct dns_socket *dns_socket;
	struct dns_udp_socket *dns_udp_socket;
	struct tevent_req *udpsubreq;
	NTSTATUS status;
	int ret;

	dns_socket = talloc(dns, struct dns_socket);
	NT_STATUS_HAVE_NO_MEMORY(dns_socket);

	dns_socket->dns = dns;

	ret = tsocket_address_inet_from_strings(dns_socket, "ip",
						address, port,
						&dns_socket->local_address);
	if (ret != 0) {
		status = map_nt_error_from_unix_common(errno);
		return status;
	}

	status = stream_setup_socket(dns->task,
				     dns->task->event_ctx,
				     dns->task->lp_ctx,
				     model_ops,
				     &dns_tcp_stream_ops,
				     "ip", address, &port,
				     lpcfg_socket_options(dns->task->lp_ctx),
				     dns_socket);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0,("Failed to bind to %s:%u TCP - %s\n",
			 address, port, nt_errstr(status)));
		talloc_free(dns_socket);
		return status;
	}

	dns_udp_socket = talloc(dns_socket, struct dns_udp_socket);
	NT_STATUS_HAVE_NO_MEMORY(dns_udp_socket);

	dns_udp_socket->dns_socket = dns_socket;

	ret = tdgram_inet_udp_socket(dns_socket->local_address,
				     NULL,
				     dns_udp_socket,
				     &dns_udp_socket->dgram);
	if (ret != 0) {
		status = map_nt_error_from_unix_common(errno);
		DEBUG(0,("Failed to bind to %s:%u UDP - %s\n",
			 address, port, nt_errstr(status)));
		return status;
	}

	dns_udp_socket->send_queue = tevent_queue_create(dns_udp_socket,
							 "dns_udp_send_queue");
	NT_STATUS_HAVE_NO_MEMORY(dns_udp_socket->send_queue);

	udpsubreq = tdgram_recvfrom_send(dns_udp_socket,
					 dns->task->event_ctx,
					 dns_udp_socket->dgram);
	NT_STATUS_HAVE_NO_MEMORY(udpsubreq);
	tevent_req_set_callback(udpsubreq, dns_udp_call_loop, dns_udp_socket);

	return NT_STATUS_OK;
}