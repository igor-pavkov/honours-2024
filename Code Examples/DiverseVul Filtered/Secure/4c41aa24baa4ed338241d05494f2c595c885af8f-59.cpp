static inline void ncp_init_request(struct ncp_server *server)
{
	ncp_lock_server(server);

	server->current_size = sizeof(struct ncp_request_header);
	server->has_subfunction = 0;
}