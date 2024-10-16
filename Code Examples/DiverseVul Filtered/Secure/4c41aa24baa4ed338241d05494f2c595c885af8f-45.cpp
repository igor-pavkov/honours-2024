static void ncp_add_byte(struct ncp_server *server, __u8 x)
{
	assert_server_locked(server);
	*(__u8 *) (&(server->packet[server->current_size])) = x;
	server->current_size += 1;
	return;
}