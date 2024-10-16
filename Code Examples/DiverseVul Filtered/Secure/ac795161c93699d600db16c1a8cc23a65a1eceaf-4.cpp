int nfs_neg_need_reval(struct inode *dir, struct dentry *dentry,
		       unsigned int flags)
{
	if (flags & (LOOKUP_CREATE | LOOKUP_RENAME_TARGET))
		return 0;
	if (NFS_SERVER(dir)->flags & NFS_MOUNT_LOOKUP_CACHE_NONEG)
		return 1;
	/* Case insensitive server? Revalidate negative dentries */
	if (nfs_server_capable(dir, NFS_CAP_CASE_INSENSITIVE))
		return 1;
	return !nfs_check_verifier(dir, dentry, flags & LOOKUP_RCU);
}