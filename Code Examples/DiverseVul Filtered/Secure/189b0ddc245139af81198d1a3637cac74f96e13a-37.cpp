bool too_many_pipe_buffers_soft(unsigned long user_bufs)
{
	unsigned long soft_limit = READ_ONCE(pipe_user_pages_soft);

	return soft_limit && user_bufs > soft_limit;
}