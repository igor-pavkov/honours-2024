static struct inode_security_struct *inode_security(struct inode *inode)
{
	__inode_security_revalidate(inode, NULL, true);
	return inode->i_security;
}