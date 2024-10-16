static void btrfs_double_extent_lock(struct inode *inode1, u64 loff1,
				     struct inode *inode2, u64 loff2, u64 len)
{
	if (inode1 < inode2) {
		swap(inode1, inode2);
		swap(loff1, loff2);
	}
	lock_extent_range(inode1, loff1, len);
	lock_extent_range(inode2, loff2, len);
}