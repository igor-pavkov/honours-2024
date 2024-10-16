unsigned long account_pipe_buffers(struct user_struct *user,
				   unsigned long old, unsigned long new)
{
	return atomic_long_add_return(new - old, &user->pipe_bufs);
}