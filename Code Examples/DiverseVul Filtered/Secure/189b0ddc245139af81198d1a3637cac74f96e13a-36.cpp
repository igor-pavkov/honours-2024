static inline bool pipe_readable(const struct pipe_inode_info *pipe)
{
	unsigned int head = READ_ONCE(pipe->head);
	unsigned int tail = READ_ONCE(pipe->tail);
	unsigned int writers = READ_ONCE(pipe->writers);

	return !pipe_empty(head, tail) || !writers;
}