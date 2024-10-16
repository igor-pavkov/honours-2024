struct timespec64 current_time(struct inode *inode)
{
	struct timespec64 now = current_kernel_time64();

	if (unlikely(!inode->i_sb)) {
		WARN(1, "current_time() called with uninitialized super_block in the inode");
		return now;
	}

	return timespec64_trunc(now, inode->i_sb->s_time_gran);
}