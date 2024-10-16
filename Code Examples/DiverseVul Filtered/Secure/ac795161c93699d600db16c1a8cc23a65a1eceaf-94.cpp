int nfs_access_get_cached(struct inode *inode, const struct cred *cred,
			  u32 *mask, bool may_block)
{
	int status;

	status = nfs_access_get_cached_rcu(inode, cred, mask);
	if (status != 0)
		status = nfs_access_get_cached_locked(inode, cred, mask,
		    may_block);

	return status;
}