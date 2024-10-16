static void dns_tcp_terminate_connection(struct dns_tcp_connection *dnsconn, const char *reason)
{
	stream_terminate_connection(dnsconn->conn, reason);
}