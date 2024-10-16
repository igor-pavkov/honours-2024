static void btrfs_double_inode_unlock(struct inode *inode1, struct inode *inode2)
{
	mutex_unlock(&inode1->i_mutex);
	mutex_unlock(&inode2->i_mutex);
}