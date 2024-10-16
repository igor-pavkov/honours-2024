static void btrfs_double_inode_lock(struct inode *inode1, struct inode *inode2)
{
	if (inode1 < inode2)
		swap(inode1, inode2);

	mutex_lock_nested(&inode1->i_mutex, I_MUTEX_PARENT);
	mutex_lock_nested(&inode2->i_mutex, I_MUTEX_CHILD);
}