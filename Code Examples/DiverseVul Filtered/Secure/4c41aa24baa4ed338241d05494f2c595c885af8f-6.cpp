int ncp_dirhandle_alloc(struct ncp_server* server, __u8 volnum, __le32 dirent,
			__u8* dirhandle) {
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 12);		/* subfunction */
	ncp_add_byte(server, NW_NS_DOS);
	ncp_add_byte(server, 0);
	ncp_add_word(server, 0);
	ncp_add_handle_path(server, volnum, dirent, 1, NULL);
	if ((result = ncp_request(server, 87)) == 0) {
		*dirhandle = ncp_reply_byte(server, 0);
	}
	ncp_unlock_server(server);
	return result;
}