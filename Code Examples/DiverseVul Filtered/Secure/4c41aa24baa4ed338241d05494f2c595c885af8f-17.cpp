static void ncp_add_word(struct ncp_server *server, __le16 x)
{
	assert_server_locked(server);
	put_unaligned(x, (__le16 *) (&(server->packet[server->current_size])));
	server->current_size += 2;
	return;
}