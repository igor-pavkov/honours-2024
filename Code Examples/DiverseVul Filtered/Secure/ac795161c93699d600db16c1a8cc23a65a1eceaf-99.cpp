static int nfs_weak_revalidate(struct dentry *dentry, unsigned int flags)
{
	struct inode *inode = d_inode(dentry);
	int error = 0;

	/*
	 * I believe we can only get a negative dentry here in the case of a
	 * procfs-style symlink. Just assume it's correct for now, but we may
	 * eventually need to do something more here.
	 */
	if (!inode) {
		dfprintk(LOOKUPCACHE, "%s: %pd2 has negative inode\n",
				__func__, dentry);
		return 1;
	}

	if (is_bad_inode(inode)) {
		dfprintk(LOOKUPCACHE, "%s: %pd2 has dud inode\n",
				__func__, dentry);
		return 0;
	}

	error = nfs_lookup_verify_inode(inode, flags);
	dfprintk(LOOKUPCACHE, "NFS: %s: inode %lu is %s\n",
			__func__, inode->i_ino, error ? "invalid" : "valid");
	return !error;
}