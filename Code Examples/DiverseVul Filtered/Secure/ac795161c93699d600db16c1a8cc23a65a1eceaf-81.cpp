static int nfs_dentry_delete(const struct dentry *dentry)
{
	dfprintk(VFS, "NFS: dentry_delete(%pd2, %x)\n",
		dentry, dentry->d_flags);

	/* Unhash any dentry with a stale inode */
	if (d_really_is_positive(dentry) && NFS_STALE(d_inode(dentry)))
		return 1;

	if (dentry->d_flags & DCACHE_NFSFS_RENAMED) {
		/* Unhash it, so that ->d_iput() would be called */
		return 1;
	}
	if (!(dentry->d_sb->s_flags & SB_ACTIVE)) {
		/* Unhash it, so that ancestors of killed async unlink
		 * files will be cleaned up during umount */
		return 1;
	}
	return 0;

}