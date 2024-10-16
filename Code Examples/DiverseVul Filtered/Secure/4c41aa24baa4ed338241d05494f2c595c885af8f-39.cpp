int ncp_get_volume_info_with_number(struct ncp_server* server,
			     int n, struct ncp_volume_info* target) {
	int result;
	int len;

	ncp_init_request_s(server, 44);
	ncp_add_byte(server, n);

	if ((result = ncp_request(server, 22)) != 0) {
		goto out;
	}
	target->total_blocks = ncp_reply_dword_lh(server, 0);
	target->free_blocks = ncp_reply_dword_lh(server, 4);
	target->purgeable_blocks = ncp_reply_dword_lh(server, 8);
	target->not_yet_purgeable_blocks = ncp_reply_dword_lh(server, 12);
	target->total_dir_entries = ncp_reply_dword_lh(server, 16);
	target->available_dir_entries = ncp_reply_dword_lh(server, 20);
	target->sectors_per_block = ncp_reply_byte(server, 28);

	memset(&(target->volume_name), 0, sizeof(target->volume_name));

	result = -EIO;
	len = ncp_reply_byte(server, 29);
	if (len > NCP_VOLNAME_LEN) {
		ncp_dbg(1, "volume name too long: %d\n", len);
		goto out;
	}
	memcpy(&(target->volume_name), ncp_reply_data(server, 30), len);
	result = 0;
out:
	ncp_unlock_server(server);
	return result;
}