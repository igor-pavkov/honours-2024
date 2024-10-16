static inline bool pipe_writable(const struct pipe_inode_info *pipe)
{
	unsigned int head = READ_ONCE(pipe->head);
	unsigned int tail = READ_ONCE(pipe->tail);
	unsigned int max_usage = READ_ONCE(pipe->max_usage);

	return !pipe_full(head, tail, max_usage) ||
		!READ_ONCE(pipe->readers);
}