static int nfs_do_access(struct inode *inode, const struct cred *cred, int mask)
{
	struct nfs_access_entry cache;
	bool may_block = (mask & MAY_NOT_BLOCK) == 0;
	int cache_mask = -1;
	int status;

	trace_nfs_access_enter(inode);

	status = nfs_access_get_cached(inode, cred, &cache.mask, may_block);
	if (status == 0)
		goto out_cached;

	status = -ECHILD;
	if (!may_block)
		goto out;

	/*
	 * Determine which access bits we want to ask for...
	 */
	cache.mask = NFS_ACCESS_READ | NFS_ACCESS_MODIFY | NFS_ACCESS_EXTEND;
	if (nfs_server_capable(inode, NFS_CAP_XATTR)) {
		cache.mask |= NFS_ACCESS_XAREAD | NFS_ACCESS_XAWRITE |
		    NFS_ACCESS_XALIST;
	}
	if (S_ISDIR(inode->i_mode))
		cache.mask |= NFS_ACCESS_DELETE | NFS_ACCESS_LOOKUP;
	else
		cache.mask |= NFS_ACCESS_EXECUTE;
	status = NFS_PROTO(inode)->access(inode, &cache, cred);
	if (status != 0) {
		if (status == -ESTALE) {
			if (!S_ISDIR(inode->i_mode))
				nfs_set_inode_stale(inode);
			else
				nfs_zap_caches(inode);
		}
		goto out;
	}
	nfs_access_add_cache(inode, &cache, cred);
out_cached:
	cache_mask = nfs_access_calc_mask(cache.mask, inode->i_mode);
	if ((mask & ~cache_mask & (MAY_READ | MAY_WRITE | MAY_EXEC)) != 0)
		status = -EACCES;
out:
	trace_nfs_access_exit(inode, mask, cache_mask, status);
	return status;
}