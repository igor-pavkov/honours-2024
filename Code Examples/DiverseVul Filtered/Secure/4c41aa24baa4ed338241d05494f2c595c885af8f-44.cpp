int ncp_ren_or_mov_file_or_subdir(struct ncp_server *server,
				struct inode *old_dir, const char *old_name,
				struct inode *new_dir, const char *new_name)
{
        int result;
        __le16 old_type = cpu_to_le16(0x06);

/* If somebody can do it atomic, call me... vandrove@vc.cvut.cz */
	result = ncp_RenameNSEntry(server, old_dir, old_name, old_type,
	                                   new_dir, new_name);
        if (result == 0xFF)	/* File Not Found, try directory */
	{
		old_type = cpu_to_le16(0x16);
		result = ncp_RenameNSEntry(server, old_dir, old_name, old_type,
						   new_dir, new_name);
	}
	if (result != 0x92) return result;	/* All except NO_FILES_RENAMED */
	result = ncp_del_file_or_subdir(server, new_dir, new_name);
	if (result != 0) return -EACCES;
	result = ncp_RenameNSEntry(server, old_dir, old_name, old_type,
					   new_dir, new_name);
	return result;
}