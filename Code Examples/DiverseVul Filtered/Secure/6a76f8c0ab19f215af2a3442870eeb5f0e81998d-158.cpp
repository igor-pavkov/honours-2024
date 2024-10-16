static void *g_start(struct seq_file *m, loff_t *pos)
{
	mutex_lock(&graph_lock);

	/* Nothing, tell g_show to print all functions are enabled */
	if (!ftrace_graph_filter_enabled && !*pos)
		return (void *)1;

	return __g_next(m, pos);
}