static int ftrace_startup(struct ftrace_ops *ops, int command)
{
	bool hash_enable = true;

	if (unlikely(ftrace_disabled))
		return -ENODEV;

	ftrace_start_up++;
	command |= FTRACE_UPDATE_CALLS;

	/* ops marked global share the filter hashes */
	if (ops->flags & FTRACE_OPS_FL_GLOBAL) {
		ops = &global_ops;
		/* Don't update hash if global is already set */
		if (global_start_up)
			hash_enable = false;
		global_start_up++;
	}

	ops->flags |= FTRACE_OPS_FL_ENABLED;
	if (hash_enable)
		ftrace_hash_rec_enable(ops, 1);

	ftrace_startup_enable(command);

	return 0;
}