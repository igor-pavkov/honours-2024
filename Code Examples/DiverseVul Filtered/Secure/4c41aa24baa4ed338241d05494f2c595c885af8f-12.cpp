int ncp_open_create_file_or_subdir(struct ncp_server *server,
				   struct inode *dir, const char *name,
				   int open_create_mode,
				   __le32 create_attributes,
				   __le16 desired_acc_rights,
				   struct ncp_entry_info *target)
{
	__le16 search_attribs = cpu_to_le16(0x0006);
	__u8  volnum;
	__le32 dirent;
	int result;

	volnum = NCP_FINFO(dir)->volNumber;
	dirent = NCP_FINFO(dir)->dirEntNum;

	if ((create_attributes & aDIR) != 0) {
		search_attribs |= cpu_to_le16(0x8000);
	}
	ncp_init_request(server);
	ncp_add_byte(server, 1);	/* subfunction */
	ncp_add_byte(server, server->name_space[volnum]);
	ncp_add_byte(server, open_create_mode);
	ncp_add_word(server, search_attribs);
	ncp_add_dword(server, RIM_ALL);
	ncp_add_dword(server, create_attributes);
	/* The desired acc rights seem to be the inherited rights mask
	   for directories */
	ncp_add_word(server, desired_acc_rights);
	ncp_add_handle_path(server, volnum, dirent, 1, name);

	if ((result = ncp_request(server, 87)) != 0)
		goto out;
	if (!(create_attributes & aDIR))
		target->opened = 1;

	/* in target there's a new finfo to fill */
	ncp_extract_file_info(ncp_reply_data(server, 6), &(target->i));
	target->volume = target->i.volNumber;
	ConvertToNWfromDWORD(ncp_reply_le16(server, 0),
			     ncp_reply_le16(server, 2),
			     target->file_handle);
	
	ncp_unlock_server(server);

	(void)ncp_obtain_nfs_info(server, &(target->i));
	return 0;

out:
	ncp_unlock_server(server);
	return result;
}