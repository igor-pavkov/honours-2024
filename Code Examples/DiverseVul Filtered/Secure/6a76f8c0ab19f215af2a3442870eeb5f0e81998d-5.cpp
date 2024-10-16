static void *t_hash_start(struct seq_file *m, loff_t *pos)
{
	struct ftrace_iterator *iter = m->private;
	void *p = NULL;
	loff_t l;

	if (!(iter->flags & FTRACE_ITER_DO_HASH))
		return NULL;

	if (iter->func_pos > *pos)
		return NULL;

	iter->hidx = 0;
	for (l = 0; l <= (*pos - iter->func_pos); ) {
		p = t_hash_next(m, &l);
		if (!p)
			break;
	}
	if (!p)
		return NULL;

	/* Only set this if we have an item */
	iter->flags |= FTRACE_ITER_HASH;

	return iter;
}