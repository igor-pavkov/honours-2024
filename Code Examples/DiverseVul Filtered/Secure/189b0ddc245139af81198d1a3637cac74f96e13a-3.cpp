void pipe_wait_writable(struct pipe_inode_info *pipe)
{
	pipe_unlock(pipe);
	wait_event_interruptible(pipe->wr_wait, pipe_writable(pipe));
	pipe_lock(pipe);
}