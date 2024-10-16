static long pipe_set_size(struct pipe_inode_info *pipe, unsigned long arg)
{
	unsigned long user_bufs;
	unsigned int nr_slots, size;
	long ret = 0;

#ifdef CONFIG_WATCH_QUEUE
	if (pipe->watch_queue)
		return -EBUSY;
#endif

	size = round_pipe_size(arg);
	nr_slots = size >> PAGE_SHIFT;

	if (!nr_slots)
		return -EINVAL;

	/*
	 * If trying to increase the pipe capacity, check that an
	 * unprivileged user is not trying to exceed various limits
	 * (soft limit check here, hard limit check just below).
	 * Decreasing the pipe capacity is always permitted, even
	 * if the user is currently over a limit.
	 */
	if (nr_slots > pipe->max_usage &&
			size > pipe_max_size && !capable(CAP_SYS_RESOURCE))
		return -EPERM;

	user_bufs = account_pipe_buffers(pipe->user, pipe->nr_accounted, nr_slots);

	if (nr_slots > pipe->max_usage &&
			(too_many_pipe_buffers_hard(user_bufs) ||
			 too_many_pipe_buffers_soft(user_bufs)) &&
			pipe_is_unprivileged_user()) {
		ret = -EPERM;
		goto out_revert_acct;
	}

	ret = pipe_resize_ring(pipe, nr_slots);
	if (ret < 0)
		goto out_revert_acct;

	pipe->max_usage = nr_slots;
	pipe->nr_accounted = nr_slots;
	return pipe->max_usage * PAGE_SIZE;

out_revert_acct:
	(void) account_pipe_buffers(pipe->user, nr_slots, pipe->nr_accounted);
	return ret;
}