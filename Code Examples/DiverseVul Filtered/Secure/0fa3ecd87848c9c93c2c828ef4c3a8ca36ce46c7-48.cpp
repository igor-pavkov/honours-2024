void __iget(struct inode *inode)
{
	atomic_inc(&inode->i_count);
}