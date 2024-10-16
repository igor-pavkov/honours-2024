int ncp_obtain_info(struct ncp_server *server, struct inode *dir, const char *path,
			struct nw_info_struct *target)
{
	__u8  volnum = NCP_FINFO(dir)->volNumber;
	__le32 dirent = NCP_FINFO(dir)->dirEntNum;
	int result;

	if (target == NULL) {
		pr_err("%s: invalid call\n", __func__);
		return -EINVAL;
	}
	ncp_init_request(server);
	ncp_add_byte(server, 6);	/* subfunction */
	ncp_add_byte(server, server->name_space[volnum]);
	ncp_add_byte(server, server->name_space[volnum]); /* N.B. twice ?? */
	ncp_add_word(server, cpu_to_le16(0x8006));	/* get all */
	ncp_add_dword(server, RIM_ALL);
	ncp_add_handle_path(server, volnum, dirent, 1, path);

	if ((result = ncp_request(server, 87)) != 0)
		goto out;
	ncp_extract_file_info(ncp_reply_data(server, 0), target);
	ncp_unlock_server(server);
	
	result = ncp_obtain_nfs_info(server, target);
	return result;

out:
	ncp_unlock_server(server);
	return result;
}