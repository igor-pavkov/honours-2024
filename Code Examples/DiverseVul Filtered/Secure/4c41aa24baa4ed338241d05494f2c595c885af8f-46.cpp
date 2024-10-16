int ncp_modify_file_or_subdir_dos_info(struct ncp_server *server,
				       struct inode *dir,
				       __le32 info_mask,
				       const struct nw_modify_dos_info *info)
{
	return ncp_modify_file_or_subdir_dos_info_path(server, dir, NULL,
		info_mask, info);
}