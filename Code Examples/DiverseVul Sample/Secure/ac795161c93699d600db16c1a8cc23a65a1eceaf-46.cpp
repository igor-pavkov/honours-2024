static void nfs_dentry_iput(struct dentry *dentry, struct inode *inode)
{
	if (dentry->d_flags & DCACHE_NFSFS_RENAMED) {
		nfs_complete_unlink(dentry, inode);
		nfs_drop_nlink(inode);
	}
	iput(inode);
}