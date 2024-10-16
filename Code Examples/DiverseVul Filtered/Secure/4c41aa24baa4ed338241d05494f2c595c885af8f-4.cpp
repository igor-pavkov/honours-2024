static inline void assert_server_locked(struct ncp_server *server)
{
	if (server->lock == 0) {
		ncp_dbg(1, "server not locked!\n");
	}
}