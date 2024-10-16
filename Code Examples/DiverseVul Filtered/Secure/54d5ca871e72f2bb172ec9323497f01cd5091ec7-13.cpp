static inline struct inode *d_inode_rcu(const struct dentry *dentry)
{
	return ACCESS_ONCE(dentry->d_inode);
}