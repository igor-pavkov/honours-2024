static struct inode_security_struct *inode_security_rcu(struct inode *inode, bool rcu)
{
	int error;

	error = __inode_security_revalidate(inode, NULL, !rcu);
	if (error)
		return ERR_PTR(error);
	return inode->i_security;
}