int ftrace_nr_registered_ops(void)
{
	struct ftrace_ops *ops;
	int cnt = 0;

	mutex_lock(&ftrace_lock);

	for (ops = ftrace_ops_list;
	     ops != &ftrace_list_end; ops = ops->next)
		cnt++;

	mutex_unlock(&ftrace_lock);

	return cnt;
}