bool too_many_pipe_buffers_hard(unsigned long user_bufs)
{
	unsigned long hard_limit = READ_ONCE(pipe_user_pages_hard);

	return hard_limit && user_bufs > hard_limit;
}