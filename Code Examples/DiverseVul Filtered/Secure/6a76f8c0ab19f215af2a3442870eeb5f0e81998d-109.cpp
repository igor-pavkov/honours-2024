struct ftrace_rec_iter *ftrace_rec_iter_start(void)
{
	/*
	 * We only use a single iterator.
	 * Protected by the ftrace_lock mutex.
	 */
	static struct ftrace_rec_iter ftrace_rec_iter;
	struct ftrace_rec_iter *iter = &ftrace_rec_iter;

	iter->pg = ftrace_pages_start;
	iter->index = 0;

	/* Could have empty pages */
	while (iter->pg && !iter->pg->index)
		iter->pg = iter->pg->next;

	if (!iter->pg)
		return NULL;

	return iter;
}