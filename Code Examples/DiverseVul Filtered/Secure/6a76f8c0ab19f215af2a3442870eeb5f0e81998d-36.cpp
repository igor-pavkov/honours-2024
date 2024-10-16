static int __unregister_ftrace_function(struct ftrace_ops *ops)
{
	int ret;

	if (ftrace_disabled)
		return -ENODEV;

	if (WARN_ON(!(ops->flags & FTRACE_OPS_FL_ENABLED)))
		return -EBUSY;

	if (FTRACE_WARN_ON(ops == &global_ops))
		return -EINVAL;

	if (ops->flags & FTRACE_OPS_FL_GLOBAL) {
		ret = remove_ftrace_list_ops(&ftrace_global_list,
					     &global_ops, ops);
		if (!ret)
			ops->flags &= ~FTRACE_OPS_FL_ENABLED;
	} else if (ops->flags & FTRACE_OPS_FL_CONTROL) {
		ret = remove_ftrace_list_ops(&ftrace_control_list,
					     &control_ops, ops);
		if (!ret) {
			/*
			 * The ftrace_ops is now removed from the list,
			 * so there'll be no new users. We must ensure
			 * all current users are done before we free
			 * the control data.
			 */
			synchronize_sched();
			control_ops_free(ops);
		}
	} else
		ret = remove_ftrace_ops(&ftrace_ops_list, ops);

	if (ret < 0)
		return ret;

	if (ftrace_enabled)
		update_ftrace_function();

	/*
	 * Dynamic ops may be freed, we must make sure that all
	 * callers are done before leaving this function.
	 */
	if (ops->flags & FTRACE_OPS_FL_DYNAMIC)
		synchronize_sched();

	return 0;
}