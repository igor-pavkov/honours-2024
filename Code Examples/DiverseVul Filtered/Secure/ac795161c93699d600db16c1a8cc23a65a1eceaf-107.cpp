static int nfs_execute_ok(struct inode *inode, int mask)
{
	struct nfs_server *server = NFS_SERVER(inode);
	int ret = 0;

	if (S_ISDIR(inode->i_mode))
		return 0;
	if (nfs_check_cache_invalid(inode, NFS_INO_INVALID_MODE)) {
		if (mask & MAY_NOT_BLOCK)
			return -ECHILD;
		ret = __nfs_revalidate_inode(server, inode);
	}
	if (ret == 0 && !execute_ok(inode))
		ret = -EACCES;
	return ret;
}