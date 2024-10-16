static void destroy_inode(struct inode *inode)
{
	BUG_ON(!list_empty(&inode->i_lru));
	__destroy_inode(inode);
	if (inode->i_sb->s_op->destroy_inode)
		inode->i_sb->s_op->destroy_inode(inode);
	else
		call_rcu(&inode->i_rcu, i_callback);
}