static void ncp_add_be32(struct ncp_server *server, __u32 x)
{
	assert_server_locked(server);
	put_unaligned(cpu_to_be32(x), (__be32 *)(&(server->packet[server->current_size])));
	server->current_size += 4;
}