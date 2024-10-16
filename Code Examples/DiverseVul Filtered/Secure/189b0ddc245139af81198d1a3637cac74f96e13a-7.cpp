static int wait_for_partner(struct pipe_inode_info *pipe, unsigned int *cnt)
{
	DEFINE_WAIT(rdwait);
	int cur = *cnt;

	while (cur == *cnt) {
		prepare_to_wait(&pipe->rd_wait, &rdwait, TASK_INTERRUPTIBLE);
		pipe_unlock(pipe);
		schedule();
		finish_wait(&pipe->rd_wait, &rdwait);
		pipe_lock(pipe);
		if (signal_pending(current))
			break;
	}
	return cur == *cnt ? -ERESTARTSYS : 0;
}