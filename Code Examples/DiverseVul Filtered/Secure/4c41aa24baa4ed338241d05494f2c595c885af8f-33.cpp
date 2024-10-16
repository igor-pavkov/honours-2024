static inline void ncp_init_request_s(struct ncp_server *server, int subfunction)
{
	ncp_lock_server(server);
	
	server->current_size = sizeof(struct ncp_request_header) + 2;
	ncp_add_byte(server, subfunction);

	server->has_subfunction = 1;
}