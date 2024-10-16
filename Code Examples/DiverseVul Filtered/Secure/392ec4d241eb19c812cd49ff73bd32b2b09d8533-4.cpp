static void dns_tcp_send(struct stream_connection *conn, uint16_t flags)
{
	struct dns_tcp_connection *dnsconn = talloc_get_type(conn->private_data,
							     struct dns_tcp_connection);
	/* this should never be triggered! */
	dns_tcp_terminate_connection(dnsconn, "dns_tcp_send: called");
}