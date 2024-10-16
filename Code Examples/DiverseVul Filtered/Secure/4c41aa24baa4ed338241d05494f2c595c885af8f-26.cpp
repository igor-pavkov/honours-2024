int ncp_dirhandle_free(struct ncp_server* server, __u8 dirhandle) {
	int result;
	
	ncp_init_request_s(server, 20);
	ncp_add_byte(server, dirhandle);
	result = ncp_request(server, 22);
	ncp_unlock_server(server);
	return result;
}