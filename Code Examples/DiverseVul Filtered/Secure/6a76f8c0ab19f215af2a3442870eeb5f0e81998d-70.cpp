int unregister_ftrace_function(struct ftrace_ops *ops)
{
	int ret;

	mutex_lock(&ftrace_lock);
	ret = __unregister_ftrace_function(ops);
	if (!ret)
		ftrace_shutdown(ops, 0);
	mutex_unlock(&ftrace_lock);

	return ret;
}