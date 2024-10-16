int ncp_modify_nfs_info(struct ncp_server *server, __u8 volnum, __le32 dirent,
			       __u32 mode, __u32 rdev)

{
	int result = 0;

	ncp_init_request(server);
	if (server->name_space[volnum] == NW_NS_NFS) {
		ncp_add_byte(server, 25);	/* subfunction */
		ncp_add_byte(server, server->name_space[volnum]);
		ncp_add_byte(server, NW_NS_NFS);
		ncp_add_byte(server, volnum);
		ncp_add_dword(server, dirent);
		/* we must always operate on both nlinks and rdev, otherwise
		   rdev is not set */
		ncp_add_dword_lh(server, NSIBM_NFS_MODE | NSIBM_NFS_NLINKS | NSIBM_NFS_RDEV);
		ncp_add_dword_lh(server, mode);
		ncp_add_dword_lh(server, 1);	/* nlinks */
		ncp_add_dword_lh(server, rdev);
		result = ncp_request(server, 87);
	}
	ncp_unlock_server(server);
	return result;
}