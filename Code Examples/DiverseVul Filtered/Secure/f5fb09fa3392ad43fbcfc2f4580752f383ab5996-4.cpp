static int minix_write_inode(struct inode * inode, int wait)
{
	brelse(minix_update_inode(inode));
	return 0;
}