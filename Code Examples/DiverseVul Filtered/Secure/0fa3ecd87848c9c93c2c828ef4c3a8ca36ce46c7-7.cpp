static int update_time(struct inode *inode, struct timespec64 *time, int flags)
{
	int (*update_time)(struct inode *, struct timespec64 *, int);

	update_time = inode->i_op->update_time ? inode->i_op->update_time :
		generic_update_time;

	return update_time(inode, time, flags);
}