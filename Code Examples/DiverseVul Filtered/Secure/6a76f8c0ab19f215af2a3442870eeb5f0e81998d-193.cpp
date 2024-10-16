static int t_show(struct seq_file *m, void *v)
{
	struct ftrace_iterator *iter = m->private;
	struct dyn_ftrace *rec;

	if (iter->flags & FTRACE_ITER_HASH)
		return t_hash_show(m, iter);

	if (iter->flags & FTRACE_ITER_PRINTALL) {
		seq_printf(m, "#### all functions enabled ####\n");
		return 0;
	}

	rec = iter->func;

	if (!rec)
		return 0;

	seq_printf(m, "%ps", (void *)rec->ip);
	if (iter->flags & FTRACE_ITER_ENABLED)
		seq_printf(m, " (%ld)%s",
			   rec->flags & ~FTRACE_FL_MASK,
			   rec->flags & FTRACE_FL_REGS ? " R" : "");
	seq_printf(m, "\n");

	return 0;
}