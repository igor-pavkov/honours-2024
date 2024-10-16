static void reset_iter_read(struct ftrace_iterator *iter)
{
	iter->pos = 0;
	iter->func_pos = 0;
	iter->flags &= ~(FTRACE_ITER_PRINTALL | FTRACE_ITER_HASH);
}