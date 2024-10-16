static void wake_up_partner(struct pipe_inode_info *pipe)
{
	wake_up_interruptible_all(&pipe->rd_wait);
}