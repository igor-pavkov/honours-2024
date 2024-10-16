static struct inode_security_struct *inode_security_novalidate(struct inode *inode)
{
	return inode->i_security;
}