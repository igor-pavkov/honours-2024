static inline struct inode *vfs_select_inode(struct dentry *dentry,
					     unsigned open_flags)
{
	struct inode *inode = d_inode(dentry);

	if (inode && unlikely(dentry->d_flags & DCACHE_OP_SELECT_INODE))
		inode = dentry->d_op->d_select_inode(dentry, open_flags);

	return inode;
}