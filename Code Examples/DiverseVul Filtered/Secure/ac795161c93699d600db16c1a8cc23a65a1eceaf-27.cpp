static void nfs_d_release(struct dentry *dentry)
{
	/* free cached devname value, if it survived that far */
	if (unlikely(dentry->d_fsdata)) {
		if (dentry->d_flags & DCACHE_NFSFS_RENAMED)
			WARN_ON(1);
		else
			kfree(dentry->d_fsdata);
	}
}