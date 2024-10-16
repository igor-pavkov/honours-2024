int ncp_modify_file_or_subdir_dos_info_path(struct ncp_server *server,
					    struct inode *dir,
					    const char *path,
					    __le32 info_mask,
					    const struct nw_modify_dos_info *info)
{
	__u8  volnum = NCP_FINFO(dir)->volNumber;
	__le32 dirent = NCP_FINFO(dir)->dirEntNum;
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 7);	/* subfunction */
	ncp_add_byte(server, server->name_space[volnum]);
	ncp_add_byte(server, 0);	/* reserved */
	ncp_add_word(server, cpu_to_le16(0x8006));	/* search attribs: all */

	ncp_add_dword(server, info_mask);
	ncp_add_mem(server, info, sizeof(*info));
	ncp_add_handle_path(server, volnum, dirent, 1, path);

	result = ncp_request(server, 87);
	ncp_unlock_server(server);
	return result;
}