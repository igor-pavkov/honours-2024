void pipe_wait_readable(struct pipe_inode_info *pipe)
{
	pipe_unlock(pipe);
	wait_event_interruptible(pipe->rd_wait, pipe_readable(pipe));
	pipe_lock(pipe);
}