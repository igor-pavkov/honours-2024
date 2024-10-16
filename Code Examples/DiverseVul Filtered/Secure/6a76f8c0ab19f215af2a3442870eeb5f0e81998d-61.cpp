int register_ftrace_function(struct ftrace_ops *ops)
{
	int ret = -1;

	mutex_lock(&ftrace_lock);

	ret = __register_ftrace_function(ops);
	if (!ret)
		ret = ftrace_startup(ops, 0);

	mutex_unlock(&ftrace_lock);

	return ret;
}