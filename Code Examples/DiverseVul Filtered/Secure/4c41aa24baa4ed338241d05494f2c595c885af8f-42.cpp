static void ncp_add_be16(struct ncp_server *server, __u16 x)
{
	assert_server_locked(server);
	put_unaligned(cpu_to_be16(x), (__be16 *) (&(server->packet[server->current_size])));
	server->current_size += 2;
}