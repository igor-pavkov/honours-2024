static void minix_delete_inode(struct inode *inode)
{
	truncate_inode_pages(&inode->i_data, 0);
	inode->i_size = 0;
	minix_truncate(inode);
	minix_free_inode(inode);
}