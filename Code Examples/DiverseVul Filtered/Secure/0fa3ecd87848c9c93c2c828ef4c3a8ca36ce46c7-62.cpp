void unlock_two_nondirectories(struct inode *inode1, struct inode *inode2)
{
	if (inode1 && !S_ISDIR(inode1->i_mode))
		inode_unlock(inode1);
	if (inode2 && !S_ISDIR(inode2->i_mode) && inode2 != inode1)
		inode_unlock(inode2);
}