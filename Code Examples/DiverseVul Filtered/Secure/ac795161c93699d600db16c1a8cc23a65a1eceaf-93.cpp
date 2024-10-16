static int nfs_check_verifier(struct inode *dir, struct dentry *dentry,
			      int rcu_walk)
{
	if (IS_ROOT(dentry))
		return 1;
	if (NFS_SERVER(dir)->flags & NFS_MOUNT_LOOKUP_CACHE_NONE)
		return 0;
	if (!nfs_dentry_verify_change(dir, dentry))
		return 0;
	/* Revalidate nfsi->cache_change_attribute before we declare a match */
	if (nfs_mapping_need_revalidate_inode(dir)) {
		if (rcu_walk)
			return 0;
		if (__nfs_revalidate_inode(NFS_SERVER(dir), dir) < 0)
			return 0;
	}
	if (!nfs_dentry_verify_change(dir, dentry))
		return 0;
	return 1;
}