static int __register_ftrace_function(struct ftrace_ops *ops)
{
	if (unlikely(ftrace_disabled))
		return -ENODEV;

	if (FTRACE_WARN_ON(ops == &global_ops))
		return -EINVAL;

	if (WARN_ON(ops->flags & FTRACE_OPS_FL_ENABLED))
		return -EBUSY;

	/* We don't support both control and global flags set. */
	if ((ops->flags & FL_GLOBAL_CONTROL_MASK) == FL_GLOBAL_CONTROL_MASK)
		return -EINVAL;

#ifndef CONFIG_DYNAMIC_FTRACE_WITH_REGS
	/*
	 * If the ftrace_ops specifies SAVE_REGS, then it only can be used
	 * if the arch supports it, or SAVE_REGS_IF_SUPPORTED is also set.
	 * Setting SAVE_REGS_IF_SUPPORTED makes SAVE_REGS irrelevant.
	 */
	if (ops->flags & FTRACE_OPS_FL_SAVE_REGS &&
	    !(ops->flags & FTRACE_OPS_FL_SAVE_REGS_IF_SUPPORTED))
		return -EINVAL;

	if (ops->flags & FTRACE_OPS_FL_SAVE_REGS_IF_SUPPORTED)
		ops->flags |= FTRACE_OPS_FL_SAVE_REGS;
#endif

	if (!core_kernel_data((unsigned long)ops))
		ops->flags |= FTRACE_OPS_FL_DYNAMIC;

	if (ops->flags & FTRACE_OPS_FL_GLOBAL) {
		add_ftrace_list_ops(&ftrace_global_list, &global_ops, ops);
		ops->flags |= FTRACE_OPS_FL_ENABLED;
	} else if (ops->flags & FTRACE_OPS_FL_CONTROL) {
		if (control_ops_alloc(ops))
			return -ENOMEM;
		add_ftrace_list_ops(&ftrace_control_list, &control_ops, ops);
	} else
		add_ftrace_ops(&ftrace_ops_list, ops);

	if (ftrace_enabled)
		update_ftrace_function();

	return 0;
}