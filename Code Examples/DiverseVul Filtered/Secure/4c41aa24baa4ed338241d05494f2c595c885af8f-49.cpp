static void ncp_add_dword(struct ncp_server *server, __le32 x)
{
	assert_server_locked(server);
	put_unaligned(x, (__le32 *) (&(server->packet[server->current_size])));
	server->current_size += 4;
	return;
}