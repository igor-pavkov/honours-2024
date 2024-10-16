int ncp_obtain_nfs_info(struct ncp_server *server,
		        struct nw_info_struct *target)

{
	int result = 0;
#ifdef CONFIG_NCPFS_NFS_NS
	__u32 volnum = target->volNumber;

	if (ncp_is_nfs_extras(server, volnum)) {
		ncp_init_request(server);
		ncp_add_byte(server, 19);	/* subfunction */
		ncp_add_byte(server, server->name_space[volnum]);
		ncp_add_byte(server, NW_NS_NFS);
		ncp_add_byte(server, 0);
		ncp_add_byte(server, volnum);
		ncp_add_dword(server, target->dirEntNum);
		/* We must retrieve both nlinks and rdev, otherwise some server versions
		   report zeroes instead of valid data */
		ncp_add_dword_lh(server, NSIBM_NFS_MODE | NSIBM_NFS_NLINKS | NSIBM_NFS_RDEV);

		if ((result = ncp_request(server, 87)) == 0) {
			ncp_extract_nfs_info(ncp_reply_data(server, 0), &target->nfs);
			ncp_dbg(1, "(%s) mode=0%o, rdev=0x%x\n",
				target->entryName, target->nfs.mode,
				target->nfs.rdev);
		} else {
			target->nfs.mode = 0;
			target->nfs.rdev = 0;
		}
	        ncp_unlock_server(server);

	} else
#endif
	{
		target->nfs.mode = 0;
		target->nfs.rdev = 0;
	}
	return result;
}