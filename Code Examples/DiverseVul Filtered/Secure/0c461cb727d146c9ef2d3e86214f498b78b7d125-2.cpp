static struct inode_security_struct *backing_inode_security(struct dentry *dentry)
{
	struct inode *inode = d_backing_inode(dentry);

	__inode_security_revalidate(inode, dentry, true);
	return inode->i_security;
}