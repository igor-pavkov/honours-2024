static struct inode_security_struct *backing_inode_security_novalidate(struct dentry *dentry)
{
	struct inode *inode = d_backing_inode(dentry);

	return inode->i_security;
}